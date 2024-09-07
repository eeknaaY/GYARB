#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "src/gfx/chunk.hpp"
#include "src/gfx/chunkmanager.hpp"
#include "src/gfx/camera.hpp"
#include "src/gfx/window.hpp"
#include "src/gfx/mesh.hpp"

#include "src/shaders/shaders.hpp"
#include "src/structures/octree.hpp"

#include <thread>
#include <mutex>


void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void checkAllChunksAndDrawThemForNow(ChunkManager* _chunkManager, Shader _voxelShader, int* _chunkPosX, int* _chunkPosZ, std::vector<std::pair<int, int>>* finishedMesh);
void updateChunkMeshes(int* zPos, int* xPos, ChunkManager* chunkManager, Shader voxelShader, std::vector<std::pair<int, int>>* finishedMesh);

float lastFrame = 0.0f;
float deltaTime = 0.0f;

Camera gameCamera;
std::vector<std::pair<int, int>> finishedMeshes;

std::thread testThread;

int main(){
    srand(time(0));

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
    float const FAR_FRUSTUM = 400.0f;

    Shader voxelShader("src/shaders/vertexshader.vs", "src/shaders/fragmentshader.fs");
    voxelShader.use();
    glUniform1i(glGetUniformLocation(voxelShader.ID, "ourTexture"), 0);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(gameCamera.fov), (float)windowWidth / (float)windowHeight, CLOSE_FRUSTUM, FAR_FRUSTUM);
    voxelShader.setMat4("projection", projection);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    ChunkManager* chunkManager = new ChunkManager();

    double startTime = glfwGetTime();

    std::thread worker1(checkAllChunksAndDrawThemForNow, chunkManager, voxelShader, &gameCamera.currentChunk_x, &gameCamera.currentChunk_z, &finishedMeshes);
    worker1.join();

    while (finishedMeshes.size() != 0){
        std::pair<int, int> pair = finishedMeshes[0];
        chunkManager->getChunk(pair.first, pair.second)->updateMesh();
        finishedMeshes.erase(finishedMeshes.begin());
    }

    while (!glfwWindowShouldClose(window))
    {   
        frameCounter += 1;
        if (glfwGetTime() - startTime >= 1.0f){
            std::cout << "ms/frame: " << 1000.0 / double(frameCounter) << "\n";
            std::cout << "Chunk: " << gameCamera.currentChunk_x << " : " << gameCamera.currentChunk_z << "\n";
            frameCounter = 0;
            startTime += 1;
        }


        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        gameCamera.processInput(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        view = glm::lookAt(gameCamera.position, gameCamera.position + gameCamera.front, gameCamera.up);
        voxelShader.setMat4("view", view);

        if (gameCamera.hasChangedChunk()){
            // Joining instead of detaching for now.
            if (testThread.joinable()) testThread.join();
            testThread = std::thread(updateChunkMeshes, &gameCamera.currentChunk_z, &gameCamera.currentChunk_x, chunkManager, voxelShader, &finishedMeshes);
        } 
        
        while (finishedMeshes.size() != 0){
            std::pair<int, int> pair = finishedMeshes[0];
            Chunk* _chunk = chunkManager->getChunk(pair.first, pair.second);
            _chunk->updateMesh();
            finishedMeshes.erase(finishedMeshes.begin());
        }

        for (auto const& [key, val] : chunkManager->chunkMap){
            val->draw(voxelShader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void checkAllChunksAndDrawThemForNow(ChunkManager* _chunkManager, Shader _voxelShader, int* _chunkPosX, int* _chunkPosZ, std::vector<std::pair<int, int>>* finishedMesh){
    for (int dx = *_chunkPosX - 1; dx <= *_chunkPosX + 1; dx++){
            for (int dz = *_chunkPosZ - 1; dz <= *_chunkPosZ + 1; dz++){
                int curLoD = 1;
                if ((dx <= 1 && dx >= -1) && (dz <= 1 && dz >= -1)) curLoD = 5;
                else{
                    if ((dx <= 2 && dx >= -2) && (dz <= 2 && dz >= -2)) curLoD = 4;
                    else {
                        if ((dx <= 3 && dx >= -3) && (dz <= 3 && dz >= -3)) curLoD = 4;
                        else {
                            if ((dx <= 4 && dx >= -4) && (dz <= 4 && dz >= -4)) curLoD = 4;
                        }
                    }
                }


                Chunk* chunk = _chunkManager->getChunk(dx, dz);
                if (chunk){
                    //chunk->mesh = _chunkManager->buildMesh(_chunkManager->getChunk(dx, dz), _voxelShader, 5);
                } else{
                    _chunkManager->appendChunk(dx, dz, curLoD);

                    Chunk* _chunk = _chunkManager->getChunk(dx, dz);
                    Mesh updatedMesh = _chunkManager->buildMesh(_chunkManager->getChunk(dx, dz), _voxelShader);
                    _chunk->mesh.vertices = updatedMesh.vertices;
                    _chunk->mesh.indices = updatedMesh.indices;
                    
                    finishedMesh->push_back(move(std::make_pair(dx, dz)));
                    //_chunk->draw(voxelShader);
                }
            }
        }
}

void updateChunkMeshes(int* zPos, int* xPos, ChunkManager* chunkManager, Shader voxelShader, std::vector<std::pair<int, int>>* finishedMesh){
    for (int dz = *zPos - 1; dz <= *zPos + 1; dz++){
        for (int dx = *xPos - 1; dx <= *xPos + 1; dx++){
            Chunk* chunk = chunkManager->getChunk(dx, dz);
            if (chunk){
                if (chunk->currentLoD == 5) chunk->currentLoD = 4;
                else chunk->currentLoD = 5;
                
                Mesh updatedMesh = chunkManager->buildMesh(chunkManager->getChunk(dx, dz), voxelShader);
                chunk->mesh.vertices = updatedMesh.vertices;
                chunk->mesh.indices = updatedMesh.indices;

                finishedMesh->push_back(move(std::make_pair(dx, dz)));
            } else{
                chunkManager->appendChunk(dx, dz, 5);

                Chunk* _chunk = chunkManager->getChunk(dx, dz);
                Mesh updatedMesh = chunkManager->buildMesh(chunkManager->getChunk(dx, dz), voxelShader);
                _chunk->mesh.vertices = updatedMesh.vertices;
                _chunk->mesh.indices = updatedMesh.indices;
                
                finishedMesh->push_back(move(std::make_pair(dx, dz)));
                //_chunk->draw(voxelShader);
            }
        }
    }
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

