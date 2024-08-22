#include <glm/vec3.hpp>
#include <vector>
#include <glad/glad.h>
#include <iostream>
#include <glm/matrix.hpp>
#include "../shaders/shaders.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



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
};

Voxel::Voxel(int _x, int _y, int _z){
    position = glm::vec3(_x, _y, _z);
}

Voxel::Voxel(int _x, int _y, int _z, std::vector<unsigned int> _indices){
    position = glm::vec3(_x, _y, _z);
    indices = _indices;
};

void Voxel::addToIndices(std::vector<unsigned int> _indices){
    indices.insert(indices.end(), _indices.begin(), _indices.end());
}

void Voxel::createArrayAndBufferObjects(){
    // Snapshot position
    glm::vec3 _position = position;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 192 * sizeof(float), &VoxelVertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Coordinates data position 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Texture data position
    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int textureWidth, textureHeight, nrChannels;
    unsigned char *data = stbi_load("C:/Users/Johannes/Desktop/GYARB/src/gfx/TextureAtlas.png", &textureWidth, &textureHeight, &nrChannels, 0);

    if (data)
    {
        stbi_set_flip_vertically_on_load(true); 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    // The position.x value is being manipulated somehow after this function, you have to snapshot its position and re-assign it at the end.
    position = _position;
}

void Voxel::draw(){
    // FIXME : Make every chunk/entire world draw with 6 draw calls, 1 for each amount of sides drawing.
    // Look at instancing, linking matrix with VAO, since uniform in shader cant hold that many matrices
    // Occlusion culling?
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


const float Voxel::VoxelVertices[192] = {
    // Back face
    -0.5f, -0.5f, -0.5f, 0.00390625f, 0.93359375f,// Bottom-left 0 
    0.5f,  0.5f, -0.5f, 0.06640625f, 0.99609375f,// top-right 
    0.5f, -0.5f, -0.5f, 0.06640625f, 0.93359375f,// bottom-right  2   
    -0.5f,  0.5f, -0.5f, 0.00390625f, 0.99609375f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // bottom-left 4
    0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // bottom-right
    0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // top-right 6
    -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,// top-left
    // Left face
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f,// top-right, 8
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,// top-left
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left, 10
    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f,// bottom-right
    // Right face
    0.5f,  0.5f,  0.5f, 0.0f, 1.0f, // top-left, 12
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right
    0.5f,  0.5f, -0.5f, 1.0f, 1.0f,// top-right, 14
    0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // bottom-left     
    // Bottom face
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,// top-right 16
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-left
    0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // bottom-left 18
    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // bottom-right
    // Top face
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // top-left 20
    0.5f,  0.5f,  0.5f, 1.0f, 0.0f, // bottom-right
    0.5f,  0.5f, -0.5f, 1.0f, 1.0f,// top-right     22
    -0.5f,  0.5f,  0.5f, 0.0f, 0.0f // bottom-left        
};

