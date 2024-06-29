#ifndef CAMERA_HPP
#define CAMERA_HPP

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
        float fov;
        glm::vec3 position;
        glm::vec3 up;
        glm::vec3 front;
        glm::vec3 target;
        glm::vec3 direction;
        glm::vec3 right;
        float yaw;
        float pitch;

    private:
        GLFWwindow* window;
};

#endif