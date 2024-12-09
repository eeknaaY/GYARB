#include <cmath>
#include "raycast.hpp"


namespace Raycast{

int signum(float val){
    return (val > 0) ? 1 : (val < 0) ? -1 : 0;
}

double intbound(double s, double ds){
    if (ds == 0) return FLT_MAX;

    if (ds > 0){
        return (ceil(s)-s)/ds;
    } else {
        return intbound(-s, -ds);
    }
}

RaycastInfo sendRaycast(const Camera &camera, ChunkManager* chunkManager){
    int x = (int)floor(camera.position.x);
    int y = (int)floor(camera.position.y);
    int z = (int)floor(camera.position.z);

    float dx = camera.front.x;
    float dy = camera.front.y;
    float dz = camera.front.z;

    int stepX = signum(dx);
    int stepY = signum(dy);
    int stepZ = signum(dz);

    float tMaxX = intbound(camera.position.x, dx);
    float tMaxY = intbound(camera.position.y, dy);
    float tMaxZ = intbound(camera.position.z, dz);

    float tDeltaX = stepX/dx;
    float tDeltaY = stepY/dy;
    float tDeltaZ = stepZ/dz;

    RaycastInfo ray;

    const int MAX_STEPS = 30;
    for (int i = 0; i < MAX_STEPS; i++){
        int blockValue = chunkManager->getBlockValue(x, y, z);

        if (blockValue != 0 && blockValue != 17){
            ray.position = glm::vec3(x, y, z);
            ray.hit = true;
            return ray;
        } else {
            // Uncomment to enable making every block the raycast goes through a block
            //chunkManager->updateBlockValue(x, y, z, 8);
        }

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                x += stepX;
                tMaxX += tDeltaX;
                ray.normal = glm::vec3(-stepX, 0, 0);

            } else {
                z += stepZ;
                tMaxZ += tDeltaZ;
                ray.normal = glm::vec3(0, 0, -stepZ);
            }
        } else {
            if (tMaxY < tMaxZ) {
                y += stepY;
                tMaxY += tDeltaY;
                ray.normal = glm::vec3(0, -stepY, 0);
            } else {
                z += stepZ;
                tMaxZ += tDeltaZ;
                ray.normal = glm::vec3(0, 0, -stepZ);
            }
        }
    }

    return ray;
}

}