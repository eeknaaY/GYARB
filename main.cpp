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

#include <chrono>

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

    Shader shadowMapShader("src/shaders/shadowMapShader.vs", "src/shaders/shadowMapShader.fs", "src/shaders/shadowMapShader.gs");
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
    projection = glm::perspective(glm::radians(gameCamera.fov), (float)windowWidth / (float)windowHeight, gameCamera.NEAR_FRUSTUM, gameCamera.FAR_FRUSTUM);
    gameCamera.projectionMatrix = projection;
    skyboxShader.setMat4("projectionSkybox", projection);
    voxelShader.use();
    voxelShader.setMat4("projection", projection);

    ShadowMap shadowMap = ShadowMap(gameCamera);
    shadowMap.bindMesh();

    Renderer gameRenderer = Renderer();
    ChunkManager* chunkManager = new ChunkManager();
    gameRenderer.chunkManager = chunkManager;

    for (int dz = -gameCamera.renderDistance; dz <= gameCamera.renderDistance; dz++){
        for (int dx = -gameCamera.renderDistance; dx <= gameCamera.renderDistance; dx++){
            Chunk* chunk = chunkManager->getChunk(dx, 0, dz);

            if (!chunk){
                auto start = std::chrono::high_resolution_clock::now();

                int LoD = 5;
                if (abs(dz) > 12 || abs(dx) > 12) LoD = 4;
                Chunk* _chunk = new Chunk(dx, 0, dz, LoD, chunkManager->noise);
                chunkManager->appendChunk(_chunk);

                auto stop = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop-start);
                printf("Creating chunk %i, %i data took: %.1f\n", dz, dx, duration.count());
            }
        }
    }

    chunkManager->updateBlockValue(0, 31, 0, 8);
    chunkManager->updateBlockValue(-1, 31, 0, 8);
    chunkManager->updateBlockValue(0, 31, -1, 8);
    chunkManager->updateBlockValue(-1, 31, -1, 8);


    for (int dz = -gameCamera.renderDistance; dz <= gameCamera.renderDistance; dz++){
        for (int dx = -gameCamera.renderDistance; dx <= gameCamera.renderDistance; dx++){
            for (int dy = 0;;dy++){
                Chunk* chunk = chunkManager->getChunk(dx, dy, dz);
                if (!chunk) break;
                chunkManager->updateChunkMesh(chunk, gameCamera); 
            }
        }
    }

    double startTime = glfwGetTime();

    bool buttonClicked = false;

    while (!glfwWindowShouldClose(window))
    {   
        frameCounter += 1;
        if (glfwGetTime() - startTime >= 1.0f){
            std::cout << "ms/frame: " << 1000.0 / double(frameCounter) << "\n";
            //printf("Speed: %.2f\n", gameCamera.automatedMovementSpeed);
            //printf("X: %.1f, Z: %.1f\nX: %.1f, Y: %.1f, Z: %.1f\n ", gameCamera.yaw, gameCamera.pitch, gameCamera.position.x, gameCamera.position.y,gameCamera.position.z);
            frameCounter = 0;
            startTime += 1;
        }


        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        gameCamera.processInput(deltaTime);

        gameCamera.position.z -= gameCamera.automatedMovementSpeed;
        


        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !rightMouseButtonDown){
            //gameCamera.automatedMovementSpeed -= 0.05;

            rightMouseButtonDown = true;
            Raycast::raycastInfo ray = Raycast::sendRaycast(gameCamera, chunkManager);
            if (ray.hit){
                glm::vec3 hitBlock = ray.position + ray.normal;
                chunkManager->updateBlockValueAndMesh(hitBlock.x, hitBlock.y, hitBlock.z, gameCamera.blockTypeSelected, gameCamera);
            } else {

            }
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE){
            rightMouseButtonDown = false;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !leftMouseButtonDown){
            //gameCamera.automatedMovementSpeed += 0.05;
            
            leftMouseButtonDown = true;
            Raycast::raycastInfo ray = Raycast::sendRaycast(gameCamera, chunkManager);
            if (ray.hit){
                chunkManager->updateBlockValueAndMesh(ray.position.x, ray.position.y, ray.position.z, 0, gameCamera);
            }
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
            leftMouseButtonDown = false;
        }

        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        view = glm::lookAt(gameCamera.position, gameCamera.position + gameCamera.front, gameCamera.up);
        gameCamera.viewMatrix = view;

        glm::vec3 sunPos = glm::vec3(gameCamera.position.x + 100, 100, gameCamera.position.z + 100);

        auto lightMatrices = shadowMap.getViewMatrices(gameCamera);
        glBindBuffer(GL_UNIFORM_BUFFER, shadowMap.matricesUBO);
        for (size_t i = 0; i < lightMatrices.size(); ++i)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);


        shadowMapShader.use();
        // Render Scene
        glViewport(0, 0, 2048, 2048);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Textures::getTextureIndex());
        gameRenderer.renderVisibleChunks(shadowMapShader, gameCamera);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skyboxShader.use();
        skyboxShader.setMat4("viewSkybox", glm::mat4(glm::mat3(view)));
        skybox.draw(skyboxShader);

        glm::mat4 Newview = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        Newview = glm::lookAt(glm::vec3(90, 55, 90), glm::vec3(0, 50, 0), gameCamera.up);

        voxelShader.use();
        voxelShader.setMat4("view", Newview);

        if (gameCamera.hasChangedChunk()){
            //chunkManager->startMeshingThreads(&gameCamera);
        }

        for (size_t i = 0; i < shadowMap.shadowCascadeLevels.size(); ++i)
        {
            voxelShader.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowMap.shadowCascadeLevels[i]);
        }
        
        voxelShader.setVec3("playerPosition", gameCamera.position);
        voxelShader.setMat4("viewMatrix", gameCamera.viewMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Textures::getTextureIndex());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMap.depthMaps);

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
