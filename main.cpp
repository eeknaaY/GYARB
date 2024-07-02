#include <glad/glad.h>
#include "src/gfx/window.hpp"
#include "src/shaders/shaders.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include "src/gfx/chunk.hpp"
#include "src/gfx/chunkmanager.hpp"
#include <iterator>
#include "src/gfx/camera.hpp"

#include <vector>
#include <ctime>
#include <chrono>

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
float lastFrame = 0.0f;
float deltaTime = 0.0f;

Camera* gameCamera;

int main(){
    srand(time(0)); 

    int windowHeight = 900;
    int windowWidth = 1600;

    GLFWwindow* window = createWindow(windowWidth, windowHeight);

    gameCamera = new Camera(window);



    glfwSetKeyCallback(window, keyCallback);
    // Cursor shit
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPosCallback(window, mouse_callback); 

    unsigned int frameCounter = 0;
    glEnable(GL_DEPTH_TEST);

    float const CLOSE_FRUSTUM = 0.1f;
    float const FAR_FRUSTUM = 100.0f;

    Shader voxelShader("src/shaders/vertexshader.vs", "src/shaders/fragmentshader.fs");
    voxelShader.use();
    glUniform1i( glGetUniformLocation(voxelShader.ID, "ourTexture"), 0);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(gameCamera->fov), (float)windowWidth / (float)windowHeight, CLOSE_FRUSTUM, FAR_FRUSTUM);
    voxelShader.setMat4("projection", projection);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    ChunkManager chunkManager;

    const int renderDistance = 3;
    float chunkPosX = 0;
    float chunkPosZ = 0;

    // Create starting chunks here, create new in while loop, if they dont exit aka return nullptr, create new one. Something lile that?
    for (int x = 0; x <= 3; x++){
        for (int z = 0; z <= 2; z++){
            // Creating chunks
            chunkManager.appendChunk(Chunk(x, z));
        }
    }

    // Create starting chunks here, create new in while loop, if they dont exit aka return nullptr, create new one.
    for (int x = 0; x <= 3; x++){
        for (int z = 0; z <= 2; z++){
            Chunk *_chunk = chunkManager.getChunk(x, z);
            // Returns nullptr if chunk doesnt exist
            if (!_chunk) continue;

            // Generates faces for the chunk
            std::vector<Voxel> arr = chunkManager.getBufferArray(_chunk);
            if (arr.size() != 0){
                _chunk->voxelArray = arr;
            }

            // FIXME
            // For the fuck all insane amounts of faces that im drawing one by one, create their vao's for some god unknown reason.
            // Goddamn fix this goddamn ugly shit already.
            for (int i = 0; i < _chunk->voxelArray.size(); i++){
                _chunk->voxelArray[i].CreateArrayAndBufferObjects();

                glm::mat4 model = glm::mat4(1.0f);
                _chunk->voxelArray[i].modelMatrix = glm::translate(model, _chunk->voxelArray[i].position);
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

        chunkPosX = round(gameCamera->position.x)/Chunk::CHUNK_SIZE;
        chunkPosZ = round(gameCamera->position.z)/Chunk::CHUNK_SIZE;

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        gameCamera->processInput(deltaTime);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); 

        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        view = glm::lookAt(gameCamera->position, gameCamera->position + gameCamera->front, gameCamera->up);
        voxelShader.setMat4("view", view);

        for (int x = (int)(chunkPosX - renderDistance); x <= (int)(chunkPosX + renderDistance); x++){
            for (int z = (int)(chunkPosZ - renderDistance); z <= (int)(chunkPosZ + renderDistance); z++){
                Chunk* _chunk = chunkManager.getChunk(x, z);
                if (!_chunk) continue;

                for (int i = 0; i < _chunk->voxelArray.size(); i++){
                    voxelShader.setMat4("model", _chunk->voxelArray[i].modelMatrix);
                    _chunk->voxelArray[i].Draw();
                }
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

    if (gameCamera->firstMouse)
    {
        gameCamera->lastX = xpos;
        gameCamera->lastY = ypos;
        gameCamera->firstMouse = false;
    }

    float xoffset = xpos - gameCamera->lastX;
    float yoffset = gameCamera->lastY - ypos; // reversed since y-coordinates go from bottom to top
    gameCamera->lastX = xpos;
    gameCamera->lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    gameCamera->yaw += xoffset;
    gameCamera->pitch += yoffset;

    // make sure that when gameCamera->pitch is out of bounds, screen doesn't get flipped
    if (gameCamera->pitch > 89.0f)
        gameCamera->pitch = 89.0f;
    if (gameCamera->pitch < -89.0f)
        gameCamera->pitch = -89.0f;

    glm::vec3 camFront;
    camFront.x = cos(glm::radians(gameCamera->yaw)) * cos(glm::radians(gameCamera->pitch));
    camFront.y = sin(glm::radians(gameCamera->pitch));
    camFront.z = sin(glm::radians(gameCamera->yaw)) * cos(glm::radians(gameCamera->pitch));
    gameCamera->front = glm::normalize(camFront);
}

