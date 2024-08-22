#ifndef VOXELOBJECT_HPP
#define VOXELOBJECT_HPP

#include <glm/vec3.hpp>
#include <vector>
#include <glm/matrix.hpp>


class Voxel
{
    public:
        glm::vec3 position;
        std::vector<unsigned int> indices;
        unsigned int VAO, VBO, EBO, texture;

        static const float VoxelVertices[192];

        Voxel(int _x, int _y, int _z);
        Voxel(int _x, int _y, int _z, std::vector<unsigned int> _indices);
        void addToIndices(std::vector<unsigned int> _indices);
        void createArrayAndBufferObjects();
        void draw();
        Voxel() = default;
        
        ~Voxel(){

        }
};

#endif