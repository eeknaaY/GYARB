#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include "camera.hpp"

bool Camera::hasChangedChunk() const {
    return (currentChunk_x != oldChunk_x) || (currentChunk_z != oldChunk_z) || (currentChunk_y != oldChunk_y);
}

bool Camera::hasChangedBlock() const {
    return position != oldPosition;
}

void Camera::processInput(GLFWwindow* window, float deltaTime)
{   
    handleMouse(window);
    this->oldPosition = position;
    processKeyInput(deltaTime);
    updateChunkPosition();
}

void Camera::updateChunkPosition(){
    this->oldChunk_x = this->currentChunk_x;
    this->oldChunk_z = this->currentChunk_z;
    this->oldChunk_y = this->currentChunk_y;

    currentChunk_x = (int)((position.x - (int)position.x % 32) / 32.f);
    currentChunk_z = (int)((position.z - (int)position.z % 32) / 32.f);
    currentChunk_y = (int)((position.y - (int)position.y % 32) / 32.f);

    if (position.x < 0) currentChunk_x--;
    if (position.z < 0) currentChunk_z--;
}

float Camera::distanceFromCamera(float x, float y, float z) const {
    return sqrtf((x - position.x) * (x - position.x) + (y - position.y) * (y - position.y) + (z - position.z) * (z - position.z));
}

std::vector<glm::vec4> Camera::getFrustumCornersWorldSpace(glm::mat4x4* projection) const {
    glm::mat4x4 inv;

    if (projection == nullptr){
        inv = glm::inverse(this->projectionMatrix * this->viewMatrix);
    } else {
        inv = glm::inverse(*projection * this->viewMatrix);
    }

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

void Camera::processKeyInput(float deltaTime){
    float cameraSpeed = 10.0f * deltaTime;
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

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
        blockTypeSelected = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
        blockTypeSelected = 2;
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS){
        blockTypeSelected = 3;
    }

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS){
        blockTypeSelected = 4;
    }

    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS){
        blockTypeSelected = 5;
    }

    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS){
        blockTypeSelected = 6;
    }

    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS){
        blockTypeSelected = 7;
    }

    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS){
        blockTypeSelected = 8;
    }

    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS){
        blockTypeSelected = 9;
    }

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS){
        blockTypeSelected = 10;
    }
}

void Camera::handleMouse(GLFWwindow* window){
    GLdouble xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    if (this->firstMouse)
    {
        this->lastX = xPos;
        this->lastY = yPos;
        this->firstMouse = false;
    }

    float xoffset = xPos - this->lastX;
    float yoffset = this->lastY - yPos; // reversed since y-coordinates go from bottom to top
    this->lastX = xPos;
    this->lastY = yPos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    this->yaw += xoffset;
    this->pitch += yoffset;

    // make sure that when this->pitch is out of bounds, screen doesn't get flipped
    if (this->pitch > 89.0f)
        this->pitch = 89.0f;
    if (this->pitch < -89.0f)
        this->pitch = -89.0f;

    glm::vec3 camFront;
    camFront.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    camFront.y = sin(glm::radians(this->pitch));
    camFront.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->front = glm::normalize(camFront);
}