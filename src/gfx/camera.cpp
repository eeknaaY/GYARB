#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include <GLFW/glfw3.h>


class Camera{
    Camera(GLFWwindow* _window){
        window = _window;
    }

    Camera() = default;
    
    public:
        void processInput(float deltaTime);
        float fov   =  120.0f;
        glm::vec3 position = glm::vec3(0.0f, 50.0f, 0.0f); 
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 direction = glm::normalize(target - position);
        glm::vec3 up    = glm::vec3(0.0f, 1.0f,  0.0f);
        glm::vec3 right = -glm::normalize(glm::cross(up, direction));
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        float yaw   = -90.0f;
        float pitch =  0.0f;

    private:
        GLFWwindow* window;

};


void Camera::processInput(float deltaTime)
{
    float cameraSpeed = 5.0f * deltaTime;
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