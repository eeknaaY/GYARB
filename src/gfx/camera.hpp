#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include <GLFW/glfw3.h>

class Camera{
    public:
        Camera(GLFWwindow* _window){
            window = _window;
            position = glm::vec3(0.0f, 50.0f, 0.0f);
            target = glm::vec3(0.0f, 0.0f, 0.0f);
            direction = glm::normalize(target - position);
            up    = glm::vec3(0.0f, 1.0f,  0.0f);
            front = glm::vec3(0.0f, 0.0f, -1.0f);
            right = glm::normalize(glm::cross(front, up));

            fov   =  90.0f;
            yaw   = -90.0f;
            pitch =  0.0f;
            lastX =  1600.0f / 2.0;
            lastY =  900.0 / 2.0;
            firstMouse = true;
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

#endif