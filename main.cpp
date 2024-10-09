#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "src/gfx/chunk.hpp"
#include "src/gfx/chunkmanager.hpp"
#include "src/gfx/camera.hpp"
#include "src/gfx/window.hpp"
#include "src/gfx/mesh.hpp"
#include "src/gfx/renderer.hpp"

#include "src/shaders/shaders.hpp"
#include "src/structures/octree.hpp"
#include "src/textures/textures.hpp"
#include "src/gfx/raycast.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

float lastFrame = 0.0f;
float deltaTime = 0.0f;

bool rightMouseButtonDown = false;
bool leftMouseButtonDown = false;

Camera gameCamera;

int main(){
    int windowHeight = 900;
    int windowWidth = 1600;

    GLFWwindow* window = createWindow(windowWidth, windowHeight);
    gameCamera = Camera(window);

    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPosCallback(window, mouse_callback); 

    unsigned int frameCounter = 0;
    float const CLOSE_FRUSTUM = 0.1f;
    float const FAR_FRUSTUM = 600.0f;

    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);  
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    Shader shadowMapShader("src/shaders/shadowMapShader.vs", "src/shaders/shadowMapShader.fs");
    Shader skyboxShader("src/shaders/skyboxShader.vs", "src/shaders/skyboxShader.fs");
    Shader voxelShader("src/shaders/voxelShader.vs", "src/shaders/voxelShader.fs");
    voxelShader.use();
    glUniform1i(glGetUniformLocation(voxelShader.ID, "ourTexture"), 0);
    glUniform1i(glGetUniformLocation(voxelShader.ID, "shadowMap"), 1);

    SkyboxMesh skybox = SkyboxMesh();
    skybox.bindMesh();
    skyboxShader.use();
    glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(gameCamera.fov), (float)windowWidth / (float)windowHeight, CLOSE_FRUSTUM, FAR_FRUSTUM);
    skyboxShader.setMat4("projectionSkybox", projection);
    voxelShader.use();
    voxelShader.setMat4("projection", projection);

    ShadowMapping shadowMap = ShadowMapping();
    shadowMap.bindMesh();

    Renderer gameRenderer = Renderer();
    ChunkManager* chunkManager = new ChunkManager();
    gameRenderer.chunkManager = chunkManager;
    
    chunkManager->noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    chunkManager->noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    chunkManager->noise.SetFrequency(0.01);
    chunkManager->noise.SetFractalOctaves(3);
    chunkManager->noise.SetFractalWeightedStrength(8);

    for (int dz = -1; dz <= 1; dz++){
        for (int dx = -1; dx <= 1; dx++){
            Chunk* chunk = chunkManager->getChunk(dx, 0, dz);

            if (!chunk){
                int LoD = 5;
                //if (abs(dz) > 4 || abs(dx) > 4) LoD = 4;
                Chunk* _chunk = new Chunk(dx, 0, dz, LoD, chunkManager->noise);
                chunkManager->appendChunk(_chunk);
            }
        }
    }

    chunkManager->updateBlockValue(0, 31, 0, 8);
    chunkManager->updateBlockValue(-1, 31, 0, 8);
    chunkManager->updateBlockValue(0, 31, -1, 8);
    chunkManager->updateBlockValue(-1, 31, -1, 8);


    for (int dz = -12; dz <= 12; dz++){
        for (int dx = -12; dx <= 12; dx++){
            for (int dy = 0;;dy++){
                Chunk* chunk = chunkManager->getChunk(dx, dy, dz);
                if (!chunk) break;
                chunkManager->updateChunkMesh(chunk, gameCamera);   
            }
        }
    }

    double startTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {   
        frameCounter += 1;
        if (glfwGetTime() - startTime >= 1.0f){
            //std::cout << "ms/frame: " << 1000.0 / double(frameCounter) << "\n";
            printf("X: %i, Z: %i\nX: %.1f, Y: %.1f, Z: %.1f\n ", gameCamera.currentChunk_x, gameCamera.currentChunk_z, gameCamera.position.x, gameCamera.position.y,gameCamera.position.z);
            frameCounter = 0;
            startTime += 1;
        }


        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        gameCamera.processInput(deltaTime);
        //gameCamera.position.x -= 0.2;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !rightMouseButtonDown){
            rightMouseButtonDown = true;
            Raycast::raycastInfo ray = Raycast::sendRaycast(gameCamera, chunkManager);
            if (ray.hit){
                chunkManager->updateBlockValueAndMesh(ray.position.x + ray.normal.x, ray.position.y + ray.normal.y, ray.position.z + ray.normal.z, 7, gameCamera);
            } else {

            }
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE){
            rightMouseButtonDown = false;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !leftMouseButtonDown){
            leftMouseButtonDown = true;
            Raycast::raycastInfo ray = Raycast::sendRaycast(gameCamera, chunkManager);
            if (ray.hit){
                chunkManager->updateBlockValueAndMesh(ray.position.x, ray.position.y, ray.position.z, 0, gameCamera);
            }
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
            leftMouseButtonDown = false;
        }

        glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 50.f, 300.f);
        glm::vec3 sunPos = glm::vec3(gameCamera.position.x + 100, 100, gameCamera.position.z + 100);
        glm::mat4 lightView = glm::lookAt(sunPos, 
                                glm::vec3(gameCamera.position.x, 50.0f, gameCamera.position.z), 
                                glm::vec3(0.0f, 1.0f, 0.0f)); 
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        // Shader & Matrices 
        shadowMapShader.use();
        shadowMapShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        // Render Scene
        glViewport(0, 0, 2048, 2048);
        glCullFace(FRONT_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Textures::getTextureIndex());
        gameRenderer.renderVisibleChunks(shadowMapShader, gameCamera);
        glCullFace(BACK_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        view = glm::lookAt(gameCamera.position, gameCamera.position + gameCamera.front, gameCamera.up);

        glm::mat3 newView = glm::mat4(glm::mat3(view));
        skyboxShader.use();
        skyboxShader.setMat4("viewSkybox", newView);
        skybox.draw(skyboxShader);

        voxelShader.use();
        voxelShader.setMat4("view", view);

        if (gameCamera.hasChangedChunk()){
            //chunkManager->testStartMT(&gameCamera);
        }
        
        voxelShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        voxelShader.setVec3("playerPosition", gameCamera.position);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Textures::getTextureIndex());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMap.depthMap);

        gameRenderer.renderVisibleChunks(voxelShader, gameCamera);
        gameRenderer.updataChunkData();

        glfwSwapBuffers(window);
        //glFlush();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (gameCamera.firstMouse)
    {
        gameCamera.lastX = xpos;
        gameCamera.lastY = ypos;
        gameCamera.firstMouse = false;
    }

    float xoffset = xpos - gameCamera.lastX;
    float yoffset = gameCamera.lastY - ypos; // reversed since y-coordinates go from bottom to top
    gameCamera.lastX = xpos;
    gameCamera.lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    gameCamera.yaw += xoffset;
    gameCamera.pitch += yoffset;

    // make sure that when gameCamera.pitch is out of bounds, screen doesn't get flipped
    if (gameCamera.pitch > 89.0f)
        gameCamera.pitch = 89.0f;
    if (gameCamera.pitch < -89.0f)
        gameCamera.pitch = -89.0f;

    glm::vec3 camFront;
    camFront.x = cos(glm::radians(gameCamera.yaw)) * cos(glm::radians(gameCamera.pitch));
    camFront.y = sin(glm::radians(gameCamera.pitch));
    camFront.z = sin(glm::radians(gameCamera.yaw)) * cos(glm::radians(gameCamera.pitch));
    gameCamera.front = glm::normalize(camFront);
}
