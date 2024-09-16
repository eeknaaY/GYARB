#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include "../textures/textures.hpp"
#include "mesh.hpp"

Mesh::Mesh(){

}

Mesh::~Mesh(){
    if (bufferExists){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices){
    this->vertices = _vertices;
    this->indices = _indices;
}

void Mesh::bindMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    bufferExists = true;
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                 &indices[0], GL_DYNAMIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribIPointer(0, 1, GL_INT, sizeof(Vertex), (void*)0);
    // vertex texture coords
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(int));

    glBindVertexArray(0);

    texture = Textures::getTextureIndex();
}  

void Mesh::updateMesh(){
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                 &indices[0], GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void Mesh::draw(const Shader &shader, int x, int y, int z) 
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(32 * x, 32 * y, 32 * z)));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::bindskyboxMesh(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    bufferExists = true;
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices[0], GL_STATIC_DRAW);  

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    texture = Textures::loadCubemap();
}

void Mesh::drawSkybox(const Shader &shader){
    glDepthMask(GL_FALSE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}

