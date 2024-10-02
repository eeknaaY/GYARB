#pragma once

#include <glm/glm.hpp>
#include "chunkmanager.hpp"
#include "camera.hpp"

namespace Raycast{
    struct raycastInfo{
        bool hit = false;
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 normal = glm::vec3(0, 0, 0);
    };


    raycastInfo sendRaycast(const Camera &camera, ChunkManager* chunkManager);
}

