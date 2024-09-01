#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/glm.hpp"
#include <glad/glad.h>
#include "../shaders/shaders.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;

    Vertex(float _x, float _y, float _z, float u, float v){
        Position.x = _x;
        Position.y = _y;
        Position.z = _z;

        TexCoords.x = u;
        TexCoords.y = v;
    }
};

class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        void draw(const Shader &shader, int x, int z);
        Mesh();
        Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices);

    private:
        unsigned int VAO, VBO, EBO, texture;
        void setupMesh();
};

Mesh::Mesh(){

}

Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices){
    this->vertices = _vertices;
    this->indices = _indices;
    

    setupMesh();
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                 &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex texture coords
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int textureWidth, textureHeight, nrChannels;
    unsigned char *data = stbi_load("C:/Users/Johannes/Desktop/GYARB/src/gfx/TextureAtlas.png", &textureWidth, &textureHeight, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        //glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    glBindVertexArray(0);
}  

void Mesh::draw(const Shader &shader, int x, int z) 
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(32 * x, 0, 32 * z)));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
} 


