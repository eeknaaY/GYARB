#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "src/gfx/chunk.hpp"
#include "src/gfx/chunkmanager.hpp"
#include "src/gfx/camera.hpp"
#include "src/gfx/window.hpp"
#include "src/gfx/mesh.hpp"

#include "src/shaders/shaders.hpp"
#include "src/structures/octree.hpp"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

float lastFrame = 0.0f;
float deltaTime = 0.0f;

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
    glEnable(GL_DEPTH_TEST);

    float const CLOSE_FRUSTUM = 0.1f;
    float const FAR_FRUSTUM = 600.0f;

    Shader skyboxShader("src/shaders/skyboxShader.vs", "src/shaders/skyboxShader.fs");
    Shader voxelShader("src/shaders/voxelShader.vs", "src/shaders/voxelShader.fs");
    voxelShader.use();
    glUniform1i(glGetUniformLocation(voxelShader.ID, "ourTexture"), 0);

    Mesh skybox = Mesh();
    skybox.bindskyboxMesh();
    skyboxShader.use();
    glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(gameCamera.fov), (float)windowWidth / (float)windowHeight, CLOSE_FRUSTUM, FAR_FRUSTUM);
    skyboxShader.setMat4("projectionSkybox", projection);
    voxelShader.use();
    voxelShader.setMat4("projection", projection);

    glEnable(GL_MULTISAMPLE);  
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    ChunkManager* chunkManager = new ChunkManager();
    
    chunkManager->noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    chunkManager->noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    chunkManager->noise.SetFrequency(0.01);
    chunkManager->noise.SetFractalOctaves(3);
    chunkManager->noise.SetFractalWeightedStrength(8);

    for (int dz = -12; dz <= 12; dz++){
        for (int dx = -12; dx <= 12; dx++){
            Chunk* chunk = chunkManager->getChunk(dx, 0, dz);

            if (!chunk){
                Chunk* _chunk = new Chunk(dx, 0, dz, 5, chunkManager->noise);
                chunkManager->appendChunk(_chunk);
                continue;
            }
        }
    }

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
            std::cout << "ms/frame: " << 1000.0 / double(frameCounter) << "\n";
            frameCounter = 0;
            startTime += 1;
        }


        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        gameCamera.processInput(deltaTime);
        //gameCamera.position.x += 0.2;

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
                    skyboxShader = Shader("src/shaders/skyboxShader.vs", "src/shaders/skyboxShader.fs");
                    voxelShader = Shader("src/shaders/voxelShader.vs", "src/shaders/voxelShader.fs");
                    voxelShader.use();
                    glUniform1i(glGetUniformLocation(voxelShader.ID, "ourTexture"), 0);

                    Mesh skybox = Mesh();
                    skybox.bindskyboxMesh();
                    skyboxShader.use();
                    glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

                    glm::mat4 projection = glm::mat4(1.0f);
                    projection = glm::perspective(glm::radians(gameCamera.fov), (float)windowWidth / (float)windowHeight, CLOSE_FRUSTUM, FAR_FRUSTUM);
                    skyboxShader.setMat4("projectionSkybox", projection);
                    voxelShader.use();
                    voxelShader.setMat4("projection", projection);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        view = glm::lookAt(gameCamera.position, gameCamera.position + gameCamera.front, gameCamera.up);

        glm::mat3 newView = glm::mat4(glm::mat3(view));
        skyboxShader.use();
        skyboxShader.setMat4("viewSkybox", newView);
        skybox.drawSkybox(skyboxShader);

        voxelShader.use();
        voxelShader.setMat4("view", view);

        if (gameCamera.hasChangedChunk()){
            chunkManager->testStartMT(&gameCamera);
        }

        while (chunkManager->chunksToRemoveth1.size() != 0){
            std::pair<int, int> pair = chunkManager->chunksToRemoveth1[0];
            chunkManager->removeChunk(pair.first, pair.second);
            chunkManager->chunksToRemoveth1.erase(chunkManager->chunksToRemoveth1.begin());
        }

        while (chunkManager->chunksToRemoveth2.size() != 0){
            std::pair<int, int> pair = chunkManager->chunksToRemoveth2[0];
            chunkManager->removeChunk(pair.first, pair.second);
            chunkManager->chunksToRemoveth2.erase(chunkManager->chunksToRemoveth2.begin());
        }

        while (chunkManager->finishedMeshesth1.size() != 0){
            Chunk* chunk = chunkManager->finishedMeshesth1[0];
            if (chunk->mesh.vertices.size() != 0){
                chunk->updateMesh();
            }
            chunkManager->finishedMeshesth1.erase(chunkManager->finishedMeshesth1.begin());
        }
        
        while (chunkManager->finishedMeshesth2.size() != 0){
            Chunk* chunk = chunkManager->finishedMeshesth2[0];
            if (chunk->mesh.vertices.size() != 0){
                chunk->updateMesh();
            }
            chunkManager->finishedMeshesth2.erase(chunkManager->finishedMeshesth2.begin());
        }

        for (auto const& [key, val] : chunkManager->chunkMap){
            // FIXME remove chunks that are out of range here
            for (Chunk* chunk : val){
                chunk->draw(voxelShader);
            }
        }

        glfwSwapBuffers(window);
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
