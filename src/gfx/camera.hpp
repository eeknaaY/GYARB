#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "GLFW/glfw3.h"
#include <vector>

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

            int width, height;
            glfwGetWindowSize(_window, &width, &height);
            projectionMatrix = glm::perspective(glm::radians(fov), (float)width / (float)height, NEAR_FRUSTUM, FAR_FRUSTUM);
        }

        Camera() = default;

        void updateChunkPosition();
        void processInput(GLFWwindow* window, float deltaTime);
        void processKeyInput(float deltaTime);
        bool hasChangedChunk() const;
        std::vector<glm::vec4> getFrustumCornersWorldSpace(glm::mat4x4* projection = nullptr) const;
        void handleMouse(GLFWwindow* window);
        float distanceFromCamera(float x, float y, float z, bool squareRoot = true) const;
        bool hasChangedBlock() const;

        float NEAR_FRUSTUM = 0.1f;
        float FAR_FRUSTUM = 800.0f;
        int renderDistance = 7;

        float automatedMovementSpeed = 0;
        int blockTypeSelected = 0;

        int currentChunk_x = 0;
        int currentChunk_z = 0;
        int currentChunk_y = 0;
        int oldChunk_x = 0;
        int oldChunk_z = 0;
        int oldChunk_y = 0;

        glm::mat4 projectionMatrix;
        glm::mat4 viewMatrix;

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
        glm::vec3 oldPosition;
        GLFWwindow* window;
};

#endif