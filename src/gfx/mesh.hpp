#pragma once

#include <vector>
#include "../shaders/shaders.hpp"
#include "glm/glm.hpp"
#include "camera.hpp"
#include "../structures/octree.hpp"

struct Vertex {
    int bitPackedData1;
    short bitPackedData2;
    Vertex(){};
    Vertex(int x, int y, int z, int faceIndex, int textureID, int uvID, int blockWidth, int blockHeight){
        // XYZ = 18 bits
        // faceIndex = 3 bits
        // textureID 8 bits
        // uvID = 2 bits
        // blockSize = 12 bits
        bitPackedData1 = (uvID << 29) | (textureID << 21) | (faceIndex << 18) | (x << 12) | (y << 6) | z;
        bitPackedData2 = (blockHeight << 6) | blockWidth;
    }
};

class Mesh {
    public:
        std::vector<Vertex> opaqueVertices;
        std::vector<unsigned int> opaqueIndices;
        std::vector<Vertex> transparentVertices;
        std::vector<unsigned int> transparentIndices;

        bool bufferExists = false;
        
        void draw(const Shader &shader, int x, int y, int z);
        void drawOpaque(const Shader &shader, int x, int y, int z);
        void drawTransparent(const Shader &shader, int x, int y, int z);

        void drawChunk(const Shader &shader, int x, int y, int z);
        void drawOpaqueChunk(const Shader &shader, int x, int y, int z);
        void drawTransparentChunk(const Shader &shader, int x, int y, int z);

        void updateMesh();
        void updateTransparentMesh();
        void bindMesh();

        void addTree(Octree* octree, int x, int y, int z);

        Mesh();
        ~Mesh();
        Mesh(std::vector<Vertex> solid_vertices, std::vector<unsigned int> solid_indices, std::vector<Vertex> transparent_vertices, std::vector<unsigned int> transparent_indices);

    protected:
        unsigned int solidVAO, solidVBO, solidEBO, transparentVAO, transparentVBO, transparentEBO, texture = 1;
};

class SkyboxMesh : public Mesh{
    public:
        SkyboxMesh(){
            bindMesh();
        }

        void draw(const Shader &shader);
        unsigned int texture;
        
    private:
        void bindMesh();
        float boxVertices[108] = {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
        };
};

class ShadowMap{
    public:
        inline static std::vector<float> shadowCascadeLevels;
        unsigned int depthMapFBO, depthMaps, matricesUBO;

        glm::mat4 getViewMatrix(const Camera& camera, float nearPlane, float farPlane);
        std::vector<glm::mat4> getViewMatrices(const Camera& camera);

        ShadowMap(const Camera& camera){
            shadowCascadeLevels = std::vector<float>{camera.FAR_FRUSTUM / 16.f, camera.FAR_FRUSTUM / 8.f, camera.FAR_FRUSTUM / 4.f, camera.FAR_FRUSTUM};
            bindMesh();
        }
    private:
        void bindMesh();
        inline static bool hasBindedTextures = false;
};

class LineMesh : public Mesh{
    std::vector<glm::vec3> opaqueVertices;

    public:
        void draw();
        void addCube(glm::vec3 pos, int size);
        void bindMesh();

        LineMesh(){
            
        };
};
