#ifndef VOXEL_HPP
#define VOXEL_HPP

#include <glm/vec4.hpp>

extern float VoxelVertices[18];

class Voxel{
    public:
        glm::vec4 position;
        Voxel(int _x, int _y, int _z, int _w);
};

#endif