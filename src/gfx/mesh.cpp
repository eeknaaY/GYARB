#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include "../textures/textures.hpp"
#include "mesh.hpp"

Mesh::Mesh(){

}

Mesh::~Mesh(){
    if (bufferExists){
        glDeleteVertexArrays(1, &solidVAO);
        glDeleteVertexArrays(1, &transparentVAO);
        glDeleteBuffers(1, &solidVBO);
        glDeleteBuffers(1, &solidEBO);
        glDeleteBuffers(1, &transparentVBO);
        glDeleteBuffers(1, &transparentEBO);
    }
}

Mesh::Mesh(std::vector<Vertex> solid_vertices, std::vector<unsigned int> solid_indices, std::vector<Vertex> transparent_vertices, std::vector<unsigned int> transparent_indices){
    this->solid_vertices = solid_vertices;
    this->solid_indices = solid_indices;
    this->transparent_vertices = transparent_vertices;
    this->transparent_indices = transparent_indices;
}

void Mesh::bindMesh()
{
    glGenVertexArrays(1, &solidVAO);
    glGenBuffers(1, &solidVBO);
    glGenBuffers(1, &solidEBO);
    bufferExists = true;
  
    glBindVertexArray(solidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, solidVBO);

    glBufferData(GL_ARRAY_BUFFER, solid_vertices.size() * sizeof(Vertex), &solid_vertices[0], GL_DYNAMIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, solid_indices.size() * sizeof(unsigned int), 
                 &solid_indices[0], GL_DYNAMIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribIPointer(0, 1, GL_INT, sizeof(Vertex), (void*)0);
    // vertex texture coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(int));

    glBindVertexArray(0);

    // Transparent
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glGenBuffers(1, &transparentEBO);
  
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);

    glBufferData(GL_ARRAY_BUFFER, transparent_vertices.size() * sizeof(Vertex), &transparent_vertices[0], GL_DYNAMIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transparentEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, transparent_indices.size() * sizeof(unsigned int), 
                 &transparent_indices[0], GL_DYNAMIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribIPointer(0, 1, GL_INT, sizeof(Vertex), (void*)0);
    // vertex texture coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(int));

    glBindVertexArray(0);

    texture = Textures::getTextureIndex();
    bufferExists = true;
}  

void Mesh::updateMesh(){
    glBindVertexArray(solidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, solidVBO);

    glBufferData(GL_ARRAY_BUFFER, solid_vertices.size() * sizeof(Vertex), &solid_vertices[0], GL_DYNAMIC_DRAW);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, solid_indices.size() * sizeof(unsigned int), 
                 &solid_indices[0], GL_DYNAMIC_DRAW);

    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);

    glBufferData(GL_ARRAY_BUFFER, transparent_vertices.size() * sizeof(Vertex), &transparent_vertices[0], GL_DYNAMIC_DRAW);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transparentEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, transparent_indices.size() * sizeof(unsigned int), 
                 &transparent_indices[0], GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void Mesh::draw(const Shader &shader, int x, int y, int z) 
{
    shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)));

    glBindVertexArray(solidVAO);
    glDrawElements(GL_TRIANGLES, solid_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(transparentVAO);
    glDrawElements(GL_TRIANGLES, transparent_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::drawChunk(const Shader &shader, int x, int y, int z, bool cameraInChunk) 
{
    shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(32 * x, 32 * y, 32 * z)));

    if (solid_vertices.size() != 0){
        glBindVertexArray(solidVAO);
        glDrawElements(GL_TRIANGLES, solid_indices.size(), GL_UNSIGNED_INT, 0);
    }

    if (transparent_vertices.size() != 0){
        glBindVertexArray(transparentVAO);
        glDrawElements(GL_TRIANGLES, transparent_indices.size(), GL_UNSIGNED_INT, 0);
    }
    

    glBindVertexArray(0);
}

void SkyboxMesh::bindMesh(){
    glGenVertexArrays(1, &solidVAO);
    glGenBuffers(1, &solidVBO);
    bufferExists = true;
  
    glBindVertexArray(solidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, solidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), &boxVertices[0], GL_STATIC_DRAW);  

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    texture = Textures::loadCubemap();
}

void SkyboxMesh::draw(const Shader &shader){
    glDepthMask(GL_FALSE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glBindVertexArray(solidVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}

std::vector<glm::vec4> ShadowMapping::getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view){
    if (this->corners.size() != 0){
        return this->corners;
    }

    const auto inv = glm::inverse(proj * view);
    
    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = 
                    inv * glm::vec4(
                        2.0f * x - 1.0f,
                        2.0f * y - 1.0f,
                        2.0f * z - 1.0f,
                        1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }
    
    this->corners = frustumCorners;
    return frustumCorners;
}

glm::mat4 ShadowMapping::getViewMatrix(const glm::mat4& proj, const glm::mat4& view){
    if (this->corners.size() == 0){
        getFrustumCornersWorldSpace(proj, view);
    }

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();
    
    glm::vec3 lightDir = glm::vec3(-1);
    glm::mat4 lightViewMatrix = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : corners)
    {
        const auto trf = lightViewMatrix * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightViewMatrix;
}

void ShadowMapping::bindMesh(){
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
}

