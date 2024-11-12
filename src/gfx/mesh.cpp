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

    glEnableVertexAttribArray(1);	
    glVertexAttribIPointer(1, 1, GL_SHORT, sizeof(Vertex), (void*)sizeof(int));

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

    glEnableVertexAttribArray(1);	
    glVertexAttribIPointer(1, 1, GL_SHORT, sizeof(Vertex), (void*)sizeof(int));

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

void Mesh::drawChunk(const Shader &shader, int x, int y, int z) 
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

std::vector<glm::mat4> ShadowMap::getViewMatrices(const Camera& camera){
    std::vector<glm::mat4> viewMatrices;
    viewMatrices.push_back(getViewMatrix(camera, camera.NEAR_FRUSTUM, shadowCascadeLevels[0]));
    viewMatrices.push_back(getViewMatrix(camera, shadowCascadeLevels[0], shadowCascadeLevels[1]));
    viewMatrices.push_back(getViewMatrix(camera, shadowCascadeLevels[1], shadowCascadeLevels[2]));
    viewMatrices.push_back(getViewMatrix(camera, shadowCascadeLevels[2], camera.FAR_FRUSTUM));
    return viewMatrices;
}

glm::mat4 ShadowMap::getViewMatrix(const Camera& camera, float nearPlane, float farPlane){
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(90.f), (float)1600 / (float)900, nearPlane, farPlane);
    const auto corners = camera.getFrustumCornersWorldSpace(&projection);

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    glm::vec3 lightDir = glm::normalize(glm::vec3(-100, -50, -100));
    const auto lightView = glm::lookAt(center - lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    constexpr float zMult = 10.0f;
    if (minZ < 0){
        minZ *= zMult;
    }
    else{
        minZ /= zMult;
    }

    if (maxZ < 0){
        maxZ /= zMult;
    }
    else{
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;
}

void ShadowMap::bindMesh(){
    if (hasBindedTextures){
        return;
    }

    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    glGenFramebuffers(1, &depthMapFBO);
        
    glGenTextures(1, &depthMaps);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthMaps);
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_DEPTH_COMPONENT32F,
        SHADOW_WIDTH,
        SHADOW_HEIGHT,
        int(shadowCascadeLevels.size()) + 1,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr);
        
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        
    constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);
        
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMaps, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
        
    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
        throw 0;
    }
        

    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    hasBindedTextures = true;
}

