#include <glm/vec3.hpp>
#include <vector>
#include <glad/glad.h>
#include <iostream>
#include <glm/matrix.hpp>

float VoxelVertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, // Bottom-left 0
    0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f, // top-right
    0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, // bottom-right  2   
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // bottom-left 4
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // bottom-right
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // top-right 6
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // top-left
    // Left face
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,// top-right, 8
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,// top-left
    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,// bottom-left, 10
    -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,// bottom-right
    // Right face
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, // top-left, 12
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, // bottom-right
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, // top-right, 14
    0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, // bottom-left     
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // top-right 16
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // top-left
    0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // bottom-left 18
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // bottom-right
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, // top-left 20
    0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // bottom-right
    0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, // top-right     22
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f // bottom-left        
};

// unsigned int VoxelIndices[] = {
// // Back face
//     0, 1, 2,
//     0, 3, 1,
// // Front face
//     4, 5, 6,
//     4, 6, 7,
// // Left face
//     10, 11, 8,
//     10, 8, 9,
// // Right face
//     15, 13, 14,
//     15, 14, 12,
// // Bottom face
//     18, 19, 16,
//     18, 16, 17,
// // Top face
//     23, 21, 22,
//     23, 22, 20
// };


class Voxel
{
    public:
        // int, int, int
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

Voxel::Voxel(int _x, int _y, int _z){
    position = glm::vec3(_x, _y, _z);
}

Voxel::Voxel(int _x, int _y, int _z, std::vector<unsigned int> _indices){
    position = glm::vec3(_x, _y, _z);
    indices = _indices;
};

void Voxel::AddToIndices(std::vector<unsigned int> _indices){
    indices.insert(indices.end(), _indices.begin(), _indices.end());
}

void Voxel::CreateArrayAndBufferObjects(){
    // Snapshot position
    glm::vec3 _position = position;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 144 * sizeof(float), &VoxelVertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Coordinates data position 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    // Color data position 
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // The position.x value is being manipulated somehow after this function, you have to snapshot its position and re-assign it at the end.
    position = _position;
}

void Voxel::Draw(){
    // FIXME : Make every chunk/entire world draw with 6 draw calls, 1 for each amount of sides drawing.
    // Look at instancing, linking matrix with VAO, since uniform in shader cant hold that many matrices
    // Occlusion culling?
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
