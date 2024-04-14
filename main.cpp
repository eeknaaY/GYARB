#include <glad/glad.h>
#include "src/gfx/window.hpp"
#include "src/gfx/voxel.hpp"
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

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);  
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  1600.0f / 2.0;
float lastY =  900.0 / 2.0;
float fov   =  90.0f;

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int main(){
    int windowHeight = 900;
    int windowWidth = 1600;

    GLFWwindow* window = createWindow(windowWidth, windowHeight);
    glfwSetKeyCallback(window, keyCallback);

    Shader testShader("src/shaders/vertexshader.vs", "src/shaders/fragmentshader.fs");

    ChunkManager chunkManager;

    Chunk chunk1(0, 0);
    Chunk chunk2(1, 0);
    Chunk chunk3(1, 1);
    Chunk chunk4(0, 1);
    chunkManager.setInvisibleTextureVector();
    chunk1.setChunkTextures();
    chunk2.setChunkTextures();
    chunk3.setChunkTextures();
    chunk4.setChunkTextures();
    chunkManager.appendChunk(chunk1);
    chunkManager.appendChunk(chunk2);
    chunkManager.appendChunk(chunk3);
     chunkManager.appendChunk(chunk4);
    
    std::vector<glm::vec3> v1 = chunkManager.getBufferArray(chunk1);
    std::vector<glm::vec3> v2 = chunkManager.getBufferArray(chunk2);
    std::vector<glm::vec3> v3 = chunkManager.getBufferArray(chunk3);
    std::vector<glm::vec3> v4 = chunkManager.getBufferArray(chunk4);

    std::vector<glm::vec3> buffer;
    buffer.insert(buffer.end(), v1.begin(), v1.end());
    buffer.insert(buffer.end(), v2.begin(), v2.end());
    buffer.insert(buffer.end(), v3.begin(), v3.end());
    buffer.insert(buffer.end(), v4.begin(), v4.end());


    // Cursor shit
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPosCallback(window, mouse_callback); 

    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * sizeof(buffer), &buffer[0].x, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VoxelVertices), VoxelVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // also set instance data
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute

    unsigned int frameCounter = 0;
    time_t startTime = time(&startTime);
    glEnable(GL_DEPTH_TEST);

    testShader.use();
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
    testShader.setMat4("projection", projection);

    glEnable(GL_CULL_FACE);
glCullFace(GL_BACK);
glFrontFace(GL_CCW);
    while (!glfwWindowShouldClose(window))
    {
        if ((difftime(time(NULL), startTime) >= 1)){
            std::cout << "FPS: " << std::to_string(frameCounter) << "\n";
            startTime = time(&startTime);
            frameCounter = 0;
        }

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        processInput(window);
        
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); 

        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        testShader.setMat4("view", view);


        // calculate the model matrix for each object and pass it to shader before drawing
        for (int i = 0; i < buffer.size(); i++){
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, buffer[i]);
            testShader.setMat4("model", model);
            glBindVertexArray(quadVAO);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 1); // 100 triangles of 6 vertices each
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        frameCounter += 1;
    }

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

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
