#include <glm/vec3.hpp>
#include <algorithm>
#include <vector>
#include <map>
#include <tuple>
#include "voxelobject.hpp"
#include <glad/glad.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Chunk{
    public:
        static const int CHUNK_SIZE = 10;
        static const int CHUNK_HEIGHT = 50;
        int xCoordinate, zCoordinate;

        // FIXME : Use bitset
        std::vector<uint8_t> voxelTextureArray;
        std::vector<Voxel> voxelArray;
        std::vector<GLfloat> indices;
        // Always gonna have 6 in length so dont need vector
        // std::vector<unsigned int> VAOs;
        // std::vector<unsigned int> VBOs;
        // std::vector<unsigned int> EBOs;

        void initializeChunkTextureVector();
        void setInvisibleChunkTextures();
        uint8_t getTextureFromPosition(int x, int y, int z);
        
        void setChunkTextures();
        void setChunkTexture(int x, int y, int z, int textureValue);

        Chunk(int _xCoordinate, int _zCoordinate){
            xCoordinate = _xCoordinate;
            zCoordinate = _zCoordinate;
        }
        Chunk() = default;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// float VoxelVertices[] = {
//     // Back face
//     -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, // Bottom-left 0
//     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f, // top-right
//     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, // bottom-right  2   
//     -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f, // top-left
//     // Front face
//     -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // bottom-left 4
//     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // bottom-right
//     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // top-right 6
//     -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // top-left
//     // Left face
//     -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,// top-right, 8
//     -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,// top-left
//     -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,// bottom-left, 10
//     -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,// bottom-right
//     // Right face
//     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, // top-left, 12
//     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, // bottom-right
//     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, // top-right, 14
//     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, // bottom-left     
//     // Bottom face
//     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // top-right 16
//     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // top-left
//     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // bottom-left 18
//     -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // bottom-right
//     // Top face
//     -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, // top-left 20
//     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // bottom-right
//     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f, // top-right     22
//     -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f // bottom-left        
// };
// Rewrite everything with building mesh
// void Chunk::CreateArrayAndBufferObjects(int amountOfSidesDrawing){
//     unsigned int VAO, VBO, EBO;

//     glGenVertexArrays(1, &VAO);
//     glGenBuffers(1, &VBO);
//     glGenBuffers(1, &EBO);

//     glBindVertexArray(VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, 144 * sizeof(float), &VoxelVertices[0], GL_STATIC_DRAW);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

//     // Coordinates data position 
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

//     // Color data position 
//     glEnableVertexAttribArray(1);	
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//     glBindVertexArray(0);

//     std::size_t vec4Size = sizeof(glm::vec4);
//     glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(6 * sizeof(float)));
//     glEnableVertexAttribArray(4); 
//     glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size + 6 * sizeof(float)));
//     glEnableVertexAttribArray(5); 
//     glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size + 6 * sizeof(float)));
//     glEnableVertexAttribArray(6); 
//     glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size + 6 * sizeof(float)));

//     VAOs.push_back(VAO);
//     VBOs.push_back(VBO);
//     EBOs.push_back(EBO);
// }

void Chunk::setChunkTextures(){
    // FIXME : Set the different textures, 0 == Air/See through
    voxelTextureArray = std::vector<uint8_t>(CHUNK_HEIGHT * CHUNK_SIZE * CHUNK_SIZE, 1);
    for (int i = 4900; i<5000; i++){
        voxelTextureArray[i] = 0;
    }    
}

void Chunk::setChunkTexture(int x, int y, int z, int textureValue){
    voxelTextureArray[CHUNK_SIZE * CHUNK_SIZE * y + CHUNK_SIZE * z + x] = textureValue;   
}

void Chunk::initializeChunkTextureVector(){
    // FIXME : Really dont use this, replacement until I can automate setChunkTextures
    voxelTextureArray = std::vector<uint8_t>(CHUNK_HEIGHT * CHUNK_SIZE * CHUNK_SIZE, 1);
}

uint8_t Chunk::getTextureFromPosition(int x, int y, int z){
    return voxelTextureArray[CHUNK_SIZE * CHUNK_SIZE * y + CHUNK_SIZE * z + x];
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

