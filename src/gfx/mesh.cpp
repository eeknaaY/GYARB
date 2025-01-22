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

Mesh::Mesh(std::vector<Vertex> opaqueVertices, std::vector<unsigned int> opaqueIndices, std::vector<Vertex> transparentVertices, std::vector<unsigned int> transparentIndices){
    this->opaqueVertices = opaqueVertices;
    this->opaqueIndices = opaqueIndices;
    this->transparentVertices = transparentVertices;
    this->transparentIndices = transparentIndices;
}

void Mesh::bindMesh(){
    glGenVertexArrays(1, &solidVAO);
    glGenBuffers(1, &solidVBO);
    glGenBuffers(1, &solidEBO);
    bufferExists = true;
  
    glBindVertexArray(solidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, solidVBO);

    glBufferData(GL_ARRAY_BUFFER, opaqueVertices.size() * sizeof(Vertex), &opaqueVertices[0], GL_DYNAMIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, opaqueIndices.size() * sizeof(unsigned int), 
                 &opaqueIndices[0], GL_DYNAMIC_DRAW);

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

    glBufferData(GL_ARRAY_BUFFER, transparentVertices.size() * sizeof(Vertex), &transparentVertices[0], GL_DYNAMIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transparentEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, transparentIndices.size() * sizeof(unsigned int), 
                 &transparentIndices[0], GL_DYNAMIC_DRAW);

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

    glBufferData(GL_ARRAY_BUFFER, opaqueVertices.size() * sizeof(Vertex), &opaqueVertices[0], GL_DYNAMIC_DRAW);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, opaqueIndices.size() * sizeof(unsigned int), 
                 &opaqueIndices[0], GL_DYNAMIC_DRAW);

    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);

    glBufferData(GL_ARRAY_BUFFER, transparentVertices.size() * sizeof(Vertex), &transparentVertices[0], GL_DYNAMIC_DRAW);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transparentEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, transparentIndices.size() * sizeof(unsigned int), 
                 &transparentIndices[0], GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void Mesh::updateTransparentMesh(){
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);

    glBufferData(GL_ARRAY_BUFFER, transparentVertices.size() * sizeof(Vertex), &transparentVertices[0], GL_DYNAMIC_DRAW);   

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transparentEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, transparentIndices.size() * sizeof(unsigned int), 
                 &transparentIndices[0], GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void Mesh::draw(const Shader &shader, int x, int y, int z) {
    drawOpaque(shader, x, y, z);
    drawTransparent(shader, x, y, z);
}

void Mesh::drawOpaque(const Shader &shader, int x, int y, int z){
    if (opaqueVertices.size() == 0) return;
    
    shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)));
    glBindVertexArray(solidVAO);
    glDrawElements(GL_TRIANGLES, opaqueIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::drawTransparent(const Shader &shader, int x, int y, int z){
    if (transparentVertices.size() == 0) return;

    shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)));
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(transparentVAO);
    glDrawElements(GL_TRIANGLES, transparentIndices.size(), GL_UNSIGNED_INT, 0);

    glDisable(GL_BLEND);
    glBindVertexArray(0);
}

void Mesh::drawChunk(const Shader &shader, int x, int y, int z) {
    drawOpaqueChunk(shader, x, y, z);
    drawTransparentChunk(shader, x, y, z);
}

void Mesh::drawOpaqueChunk(const Shader &shader, int x, int y, int z){
    drawOpaque(shader, 32 * x, 32 * y, 32 * z);
}

void Mesh::drawTransparentChunk(const Shader &shader, int x, int y, int z){
    drawTransparent(shader, 32 * x, 32 * y, 32 * z);
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
    for (const auto& v : corners){
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
    if (status != GL_FRAMEBUFFER_COMPLETE){
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

void LineMesh::bindMesh(){
    glGenVertexArrays(1, &solidVAO);
    glGenBuffers(1, &solidVBO);
    bufferExists = true;
  
    glBindVertexArray(solidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, solidVBO);

    glBufferData(GL_ARRAY_BUFFER, opaqueVertices.size() * sizeof(glm::vec3), &opaqueVertices[0], GL_DYNAMIC_DRAW);  

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}

void LineMesh::addCube(glm::vec3 pos, int size){
    float offSet = 0.005f;
    opaqueVertices.push_back(glm::vec3(-offSet + pos.x, -offSet + pos.y, -offSet + pos.z));
    opaqueVertices.push_back(glm::vec3(-offSet + pos.x, offSet + pos.y + size, -offSet + pos.z));

    opaqueVertices.push_back(glm::vec3(-offSet + pos.x, -offSet + pos.y, -offSet + pos.z));
    opaqueVertices.push_back(glm::vec3(offSet + pos.x + size, -offSet + pos.y, -offSet + pos.z));

    opaqueVertices.push_back(glm::vec3(-offSet + pos.x, -offSet + pos.y, -offSet + pos.z));
    opaqueVertices.push_back(glm::vec3(-offSet + pos.x, -offSet + pos.y, offSet + pos.z + size));

    opaqueVertices.push_back(glm::vec3(offSet + pos.x + size, -offSet + pos.y, -offSet + pos.z));
    opaqueVertices.push_back(glm::vec3(offSet + pos.x + size, offSet + pos.y + size, -offSet + pos.z));

    opaqueVertices.push_back(glm::vec3(-offSet + pos.x, -offSet + pos.y, offSet + pos.z + size));
    opaqueVertices.push_back(glm::vec3(-offSet + pos.x, offSet + pos.y + size, offSet + pos.z + size));

    opaqueVertices.push_back(glm::vec3(-offSet + pos.x, offSet + pos.y + size, -offSet + pos.z));
    opaqueVertices.push_back(glm::vec3(offSet + pos.x + size, offSet + pos.y + size, -offSet + pos.z));

    opaqueVertices.push_back(glm::vec3(-offSet + pos.x, offSet + pos.y + size, -offSet + pos.z));
    opaqueVertices.push_back(glm::vec3(-offSet + pos.x, offSet + pos.y + size, offSet + pos.z + size));

    opaqueVertices.push_back(glm::vec3(offSet + pos.x + size, offSet + pos.y + size, -offSet + pos.z));
    opaqueVertices.push_back(glm::vec3(offSet + pos.x + size, offSet + pos.y + size, offSet + pos.z + size));

    opaqueVertices.push_back(glm::vec3(-offSet + pos.x, offSet + pos.y + size, offSet + pos.z + size));
    opaqueVertices.push_back(glm::vec3(offSet + pos.x + size, offSet + pos.y + size, offSet + pos.z + size));

    opaqueVertices.push_back(glm::vec3(offSet + pos.x + size, -offSet + pos.y, offSet + pos.z + size));
    opaqueVertices.push_back(glm::vec3(offSet + pos.x + size, offSet + pos.y + size, offSet + pos.z + size));

    opaqueVertices.push_back(glm::vec3(-offSet + pos.x, -offSet + pos.y, offSet + pos.z + size));
    opaqueVertices.push_back(glm::vec3(offSet + pos.x + size, -offSet + pos.y, offSet + pos.z + size));

    opaqueVertices.push_back(glm::vec3(offSet + pos.x + size, -offSet + pos.y, -offSet + pos.z));
    opaqueVertices.push_back(glm::vec3(offSet + pos.x + size, -offSet + pos.y, offSet + pos.z + size));
}

void LineMesh::draw(){
    glDepthMask(GL_FALSE);
    glBindVertexArray(solidVAO);
    glDrawArrays(GL_LINES, 0, opaqueVertices.size());
    glDepthMask(GL_TRUE);
}
