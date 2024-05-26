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
        unsigned int VAO, VBO, EBO, instanceVBO;

        glm::mat4 modelMatrix;

        Voxel(int _x, int _y, int _z);
        Voxel(int _x, int _y, int _z, std::vector<unsigned int> _indices);
        void AddToIndices(std::vector<unsigned int> _indices);
        void CreateArrayAndBufferObjects();
        void Draw();
        Voxel() = default;
};

#endif