#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include <GLFW/glfw3.h>


class Camera{
    public:
        Camera(GLFWwindow* _window){
            window = _window;
        }

        Camera() = default;

        void processInput(float deltaTime);

        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 direction;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 front;

        float fov;
        float yaw;
        float pitch;
        float lastX;
        float lastY;
        bool firstMouse;

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