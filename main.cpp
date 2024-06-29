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

#include <vector>
#include <ctime>
#include <chrono>

glm::vec3 cameraPos = glm::vec3(0.0f, 50.0f, 0.0f);  
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
glm::vec3 cameraRight = -glm::normalize(glm::cross(cameraUp, cameraDirection));
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
bool firstMouse = true;
float yaw   = 0.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  1600.0f / 2.0;
float lastY =  900.0 / 2.0;
float fov   =  120.0f;

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int main(){
    srand(time(0)); 

    int windowHeight = 900;
    int windowWidth = 1600;
    GLFWwindow* window = createWindow(windowWidth, windowHeight);
    glfwSetKeyCallback(window, keyCallback);

    Shader voxelShader("src/shaders/vertexshader.vs", "src/shaders/fragmentshader.fs");

    // Cursor shit
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPosCallback(window, mouse_callback); 

    unsigned int frameCounter = 0;
    glEnable(GL_DEPTH_TEST);

    float const CLOSE_FRUSTUM = 0.1f;
    float const FAR_FRUSTUM = 100.0f;

    voxelShader.use();
    // testShader.setInt("ourTexture", 0);
    glUniform1i( glGetUniformLocation(voxelShader.ID, "ourTexture"), 0);
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(fov), (float)windowWidth / (float)windowHeight, CLOSE_FRUSTUM, FAR_FRUSTUM);
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
            std::cout << "x->" << cameraPos.x << "   z->:"<<cameraPos.z << "\n";
            frameCounter = 0;
            startTime += 1;
        }

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        processInput(window);
        chunkPosX = round(cameraPos.x)/Chunk::CHUNK_SIZE;
        chunkPosZ = round(cameraPos.z)/Chunk::CHUNK_SIZE;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); 

        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
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

void processInput(GLFWwindow *window)
{
    float cameraSpeed = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * glm::normalize(glm::vec3(cameraFront.x * cos(pitch * 3.14 / 180), 0, cameraFront.z * cos(pitch * 3.14/180)));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * glm::normalize(glm::vec3(cameraFront.x * cos(pitch * 3.14 / 180), 0, cameraFront.z * cos(pitch * 3.14/180)));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
