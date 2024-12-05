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

#include <chrono>

float lastFrame = 0.0f;
float deltaTime = 0.0f;

int main(){
    int windowHeight = 900;
    int windowWidth = 1600;

    GLFWwindow* window = createWindow(windowWidth, windowHeight);
    Camera gameCamera = Camera(window);

    Shader shadowMapShader("src/shaders/shadowMapShader.vs", "src/shaders/shadowMapShader.fs", "src/shaders/shadowMapShader.gs");
    Shader skyboxShader("src/shaders/skyboxShader.vs", "src/shaders/skyboxShader.fs");
    Shader voxelShader("src/shaders/voxelShader.vs", "src/shaders/voxelShader.fs");

    SkyboxMesh skybox = SkyboxMesh();
    skyboxShader.use();
    glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
    skyboxShader.setMat4("projectionSkybox", gameCamera.projectionMatrix);

    voxelShader.use();
    glUniform1i(glGetUniformLocation(voxelShader.ID, "ourTexture"), 0);
    glUniform1i(glGetUniformLocation(voxelShader.ID, "shadowMap"), 1);
    glUniform1i(glGetUniformLocation(voxelShader.ID, "skybox"), 2);
    voxelShader.setMat4("projection", gameCamera.projectionMatrix);

    ShadowMap shadowMap = ShadowMap(gameCamera);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Textures::getTextureIndex());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMap.depthMaps);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texture);
    
    shadowMapShader.use();

    ChunkManager* chunkManager = new ChunkManager();
    Renderer gameRenderer = Renderer(chunkManager);
    
    for (int dz = -gameCamera.renderDistance; dz <= gameCamera.renderDistance; dz++){
        for (int dx = -gameCamera.renderDistance; dx <= gameCamera.renderDistance; dx++){
            auto start = std::chrono::high_resolution_clock::now();

            int LoD = 5;
            if (abs(dz) > 12 || abs(dx) > 12) LoD = 4;
            Chunk* _chunk = new Chunk(dx, 0, dz, LoD);
            chunkManager->appendChunk(_chunk);

            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop-start);
            printf("Creating chunk %i, %i data took: %.1d\n", dz, dx, duration.count());
        }
    }

    chunkManager->setBlockValue(2, 31, 2, 8);
    chunkManager->setBlockValue(0, 31, 0, 8);
    chunkManager->setBlockValue(2, 32, 2, 8);

    for (int dz = -gameCamera.renderDistance; dz <= gameCamera.renderDistance; dz++){
        for (int dx = -gameCamera.renderDistance; dx <= gameCamera.renderDistance; dx++){
            std::vector<Chunk*> chunkVector = chunkManager->getChunkVector(dx, dz);
            for (Chunk* chunk : chunkVector){
                chunkManager->updateChunkMesh(chunk, gameCamera);
            }
        }
    }

    double startTime = glfwGetTime();
    int frameCounter = 0;
    bool rightMouseButtonDown = false;
    bool leftMouseButtonDown = false;

    while (!glfwWindowShouldClose(window)){   
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
        gameCamera.processInput(window, deltaTime);
        
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !rightMouseButtonDown){
            //gameCamera.automatedMovementSpeed -= 0.05;

            rightMouseButtonDown = true;
            Raycast::raycastInfo ray = Raycast::sendRaycast(gameCamera, chunkManager);
            if (ray.hit){
                glm::vec3 selectedVoxel = ray.position + ray.normal;
                chunkManager->updateBlockValueAndMesh(selectedVoxel.x, selectedVoxel.y, selectedVoxel.z, gameCamera.blockTypeSelected, gameCamera);
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

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, Textures::getTextureIndex());
        gameRenderer.renderVisibleChunks(shadowMapShader, gameCamera);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Draw skybox
        skyboxShader.use();
        skyboxShader.setMat4("viewSkybox", glm::mat4(glm::mat3(view)));
        skybox.draw(skyboxShader);
        // glClearColor(0.0f, 0.6f, 0.8f, 1.0f);

        voxelShader.use();
        voxelShader.setMat4("view", view);

        if (gameCamera.hasChangedChunk()){
            //chunkManager->startMeshingThreads(&gameCamera);
        }

        for (int i = 0; i < shadowMap.shadowCascadeLevels.size(); i++){
            voxelShader.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowMap.shadowCascadeLevels[i]);
        }
        
        voxelShader.setVec3("playerPosition", gameCamera.position);
        voxelShader.setMat4("viewMatrix", gameCamera.viewMatrix);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, Textures::getTextureIndex());
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMap.depthMaps);
        // glActiveTexture(GL_TEXTURE2);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texture);
        

        gameRenderer.renderVisibleChunks(voxelShader, gameCamera);
        gameRenderer.updataChunkData();

        glfwSwapBuffers(window);
        //glFlush();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}