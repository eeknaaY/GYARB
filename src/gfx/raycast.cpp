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

raycastInfo sendRaycast(const Camera &camera, ChunkManager* chunkManager){
    int x = (int)floor(camera.position.x);
    int y = (int)floor(camera.position.y);
    int z = (int)floor(camera.position.z);

    double dx = camera.front.x;
    double dy = camera.front.y;
    double dz = camera.front.z;

    int stepX = signum(dx);
    int stepY = signum(dy);
    int stepZ = signum(dz);

    double tMaxX = intbound(camera.position.x, dx);
    double tMaxY = intbound(camera.position.y, dy);
    double tMaxZ = intbound(camera.position.z, dz);

    double tDeltaX = stepX/dx;
    double tDeltaY = stepY/dy;
    double tDeltaZ = stepZ/dz;

    raycastInfo ray;

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
                // Update which cube we are now in.
                x += stepX;
                // Adjust tMaxX to the next X-oriented boundary crossing.
                tMaxX += tDeltaX;
                // Record the normal vector of the cube face we entered.
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
                // Identical to the second case, repeated for simplicity in
                // the conditionals.
                z += stepZ;
                tMaxZ += tDeltaZ;
                ray.normal = glm::vec3(0, 0, -stepZ);
            }
        }
    }

    return ray;
}

}