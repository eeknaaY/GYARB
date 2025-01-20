#include "src/gfx/chunkmanager.hpp"

#include "src/gfx/camera.hpp"
#include "src/gfx/window.hpp"
#include "src/gfx/renderer.hpp"
#include "src/shaders/shaders.hpp"
#include "src/textures/textures.hpp"
#include "src/gfx/raycast.hpp"

#include <chrono>

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
    glUniform1i(glGetUniformLocation(voxelShader.ID, "textureAtlas"), 0);
    glUniform1i(glGetUniformLocation(voxelShader.ID, "shadowMap"), 1);
    glUniform1i(glGetUniformLocation(voxelShader.ID, "skybox"), 2);
    voxelShader.setMat4("projection", gameCamera.projectionMatrix);
    voxelShader.setInt("viewDistance", gameCamera.FAR_FRUSTUM);

    ShadowMap shadowMap = ShadowMap(gameCamera);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Textures::getTextureIndex());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMap.depthMaps);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texture);

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
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;
    float viewDistance = 0;

    //float wait = 0;
    //std::cin >> wait;

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

        //float heightDifference = abs(gameCamera.position.y - (BiomeHandler::getHeightValue(gameCamera.position.x, gameCamera.position.z) + 3));
        // if (abs(gameCamera.position.y - (BiomeHandler::getHeightValue(gameCamera.position.x, gameCamera.position.z) + 3)) > 0.3f){
        //     if (gameCamera.position.y > BiomeHandler::getHeightValue(gameCamera.position.x, gameCamera.position.z) + 3){
        //         gameCamera.position.y -= 0.1f;
        //     } else {
        //         gameCamera.position.y += 0.5;
        //     }
        // }
        
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !rightMouseButtonDown){
            gameCamera.movementSpeed -= 1;

            rightMouseButtonDown = true;
            Raycast::RaycastInfo ray = Raycast::sendRaycast(gameCamera, 20, chunkManager);
            if (ray.hit){
                glm::vec3 selectedVoxel = ray.position + ray.normal;
                chunkManager->updateBlockValueAndMesh(selectedVoxel.x, selectedVoxel.y, selectedVoxel.z, gameCamera.blockTypeSelected, gameCamera);
            }
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE){
            rightMouseButtonDown = false;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !leftMouseButtonDown){
            gameCamera.movementSpeed += 1;
            
            leftMouseButtonDown = true;
            Raycast::RaycastInfo ray = Raycast::sendRaycast(gameCamera, 20, chunkManager);
            if (ray.hit){
                chunkManager->updateBlockValueAndMesh(ray.position.x, ray.position.y, ray.position.z, 0, gameCamera);
            }
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
            leftMouseButtonDown = false;
        }

        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        gameCamera.viewMatrix = glm::lookAt(gameCamera.position, gameCamera.position + gameCamera.front, gameCamera.up);

        std::vector<glm::mat4> lightMatrices = shadowMap.getViewMatrices(gameCamera);
        glBindBuffer(GL_UNIFORM_BUFFER, shadowMap.matricesUBO);
        for (size_t i = 0; i < lightMatrices.size(); ++i){
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Render shadow maps
        shadowMapShader.use();
        glViewport(0, 0, 2048, 2048);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        gameRenderer.renderVisibleChunks(shadowMapShader, gameCamera);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Draw skybox
        skyboxShader.use();
        skyboxShader.setMat4("viewSkybox", glm::mat4(glm::mat3(gameCamera.viewMatrix)));
        skybox.draw(skyboxShader);

        voxelShader.use();
        voxelShader.setMat4("view", gameCamera.viewMatrix);

        if (gameCamera.hasChangedChunk()){
            chunkManager->startMeshingThreads(&gameCamera);
        }

        for (int i = 0; i < shadowMap.shadowCascadeLevels.size(); i++){
            voxelShader.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowMap.shadowCascadeLevels[i]);
        }
        
        voxelShader.setVec3("playerPosition", gameCamera.position);
        voxelShader.setMat4("viewMatrix", gameCamera.viewMatrix);

        gameRenderer.renderVisibleChunks(voxelShader, gameCamera);
        gameRenderer.updataChunkData(voxelShader);

        if (viewDistance < gameCamera.FAR_FRUSTUM){
            // voxelShader = Shader("src/shaders/voxelShader.vs", "src/shaders/voxelShader.fs");

            voxelShader.use();
            // glUniform1i(glGetUniformLocation(voxelShader.ID, "textureAtlas"), 0);
            // glUniform1i(glGetUniformLocation(voxelShader.ID, "shadowMap"), 1);
            // glUniform1i(glGetUniformLocation(voxelShader.ID, "skybox"), 2);
            // voxelShader.setMat4("projection", gameCamera.projectionMatrix);
            voxelShader.setFloat("viewDistance", viewDistance);
            viewDistance++;
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
            voxelShader = Shader("src/shaders/voxelShader.vs", "src/shaders/voxelShader.fs");

            voxelShader.use();
            glUniform1i(glGetUniformLocation(voxelShader.ID, "textureAtlas"), 0);
            glUniform1i(glGetUniformLocation(voxelShader.ID, "shadowMap"), 1);
            glUniform1i(glGetUniformLocation(voxelShader.ID, "skybox"), 2);
            voxelShader.setMat4("projection", gameCamera.projectionMatrix);
            voxelShader.setFloat("viewDistance", viewDistance);
        } 

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}