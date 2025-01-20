#pragma once

#include <glm/glm.hpp>
#include "chunkmanager.hpp"
#include "camera.hpp"

namespace Raycast{
    enum class Direction{
        FORWARD,
        BACK,
        LEFT,
        RIGHT,
        DOWN,
        UP
    };

    struct RaycastInfo{
        bool hit = false;
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 normal = glm::vec3(0, 0, 0);
    };


    RaycastInfo sendRaycast(const Camera &camera, int maxSteps, ChunkManager* chunkManager, Direction direction = Direction::FORWARD);
}

