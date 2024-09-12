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

    Shader voxelShader("src/shaders/vertexshader.vs", "src/shaders/fragmentshader.fs");
    voxelShader.use();
    glUniform1i(glGetUniformLocation(voxelShader.ID, "ourTexture"), 0);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(gameCamera.fov), (float)windowWidth / (float)windowHeight, CLOSE_FRUSTUM, FAR_FRUSTUM);
    voxelShader.setMat4("projection", projection);

    glEnable(GL_MULTISAMPLE);  
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    ChunkManager* chunkManager = new ChunkManager();

    for (int dz = -12; dz <= 12; dz++){
        for (int dx = -12; dx <= 12; dx++){
            Chunk* chunk = chunkManager->getChunk(dx, dz);

            if (!chunk){
                Chunk* _chunk = new Chunk(dx, dz, 5);
                chunkManager->appendChunk(_chunk);
                continue;
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        view = glm::lookAt(gameCamera.position, gameCamera.position + gameCamera.front, gameCamera.up);
        voxelShader.setMat4("view", view);

        if (gameCamera.hasChangedChunk()){
            chunkManager->testStartMT(&gameCamera);
        }

        while (chunkManager->chunksToRemove.size() != 0){
            std::pair<int, int> pair = chunkManager->chunksToRemove[0];
            chunkManager->removeChunk(pair.first, pair.second);
            chunkManager->chunksToRemove.erase(chunkManager->chunksToRemove.begin());
        }

        while (chunkManager->finishedMeshes.size() != 0){
            Chunk* chunk = chunkManager->finishedMeshes[0];
            if (!chunkManager->getChunk(chunk->xCoordinate, chunk->zCoordinate)){
                chunkManager->appendChunk(chunk);
            }
            chunk->updateMesh();
            chunkManager->finishedMeshes.erase(chunkManager->finishedMeshes.begin());
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
