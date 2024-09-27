#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include <GLFW/glfw3.h>
#include "camera.hpp"

bool Camera::hasChangedChunk(){
    return (currentChunk_x != oldChunk_x) || (currentChunk_z != oldChunk_z);
}

void Camera::processInput(float deltaTime)
{   
    updateMovement(deltaTime);
    updateChunkPosition();
}

void Camera::updateChunkPosition(){

    this->oldChunk_x = this->currentChunk_x;
    this->oldChunk_z = this->currentChunk_z;

    currentChunk_x = (int)(((int)round(position.x) - (int)round(position.x) % 32) / 32.f);
    currentChunk_z = (int)(((int)round(position.z) - (int)round(position.z) % 32) / 32.f);

    if (position.x < 0) currentChunk_x--;
    if (position.z < 0) currentChunk_z--;
}

void Camera::updateMovement(float deltaTime){
    float cameraSpeed = 40.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += cameraSpeed * glm::normalize(glm::vec3(front.x * cos(pitch * 3.14 / 180), 0, front.z * cos(pitch * 3.14/180)));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= cameraSpeed * glm::normalize(glm::vec3(front.x * cos(pitch * 3.14 / 180), 0, front.z * cos(pitch * 3.14/180)));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += glm::normalize(glm::cross(front, up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += cameraSpeed * up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        position -= cameraSpeed * up;
}