#pragma once

#include <glm/glm.hpp>
#include "chunkmanager.hpp"
#include "camera.hpp"

namespace Raycast{
    struct RaycastInfo{
        bool hit = false;
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 normal = glm::vec3(0, 0, 0);
    };


    RaycastInfo sendRaycast(const Camera &camera, ChunkManager* chunkManager);
}

