#include <glm/glm.hpp>
#include <glm/vec4.hpp>

float VoxelVertices[18] = {
    0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  // bottom left
    0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
};

class Voxel
{
    public:
        glm::vec4 position;

        Voxel(int _x, int _y, int _z, int _w);
};

Voxel::Voxel(int _x, int _y, int _z, int _w){
    position = glm::vec4(_x, _y, _z, _w);
}