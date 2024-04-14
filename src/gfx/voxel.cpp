#include <glm/vec3.hpp>
#include "window.hpp"

class Voxel
{
    public:
        glm::vec3 position;

        Voxel(int _x, int _y, int _z);
        Voxel() = default;
};

Voxel::Voxel(int _x, int _y, int _z){
    position = glm::vec3(_x, _y, _z);
}