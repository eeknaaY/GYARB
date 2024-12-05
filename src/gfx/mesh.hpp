#pragma once

#include <vector>
#include "../shaders/shaders.hpp"
#include "glm/glm.hpp"
#include "camera.hpp"

struct Vertex {
    int bitPackedData1;
    short bitPackedData2;
    Vertex(){};
    Vertex(int _x, int _y, int _z, int faceIndex, int textureID, int uvID, int blockWidth, int blockHeight){
        // XYZ = 18 bits
        // faceIndex = 3 bits
        // textureID 8 bits
        // uvID = 2 bits
        // blockSize = 12 bits
        bitPackedData1 = (uvID << 29) | (textureID << 21) | (faceIndex << 18) | (_x << 12) | (_y << 6) | _z;
        bitPackedData2 = (blockHeight << 6) | blockWidth;
    }
};

class Mesh {
    public:
        std::vector<Vertex> solid_vertices;
        std::vector<unsigned int> solid_indices;
        std::vector<Vertex> transparent_vertices;
        std::vector<unsigned int> transparent_indices;
        bool bufferExists = false;

        void draw(const Shader &shader, int x, int y, int z);
        void drawChunk(const Shader &shader, int x, int y, int z);
        void updateMesh();
        void updateTransparentMesh();
        void bindMesh();

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
        float boxVerticesWithTextures[180] = {
            // positions          
            -1.0f,  1.0f, -1.0f, 0.0, 0.0,
            -1.0f, -1.0f, -1.0f, 0.0, 0.0,
            1.0f, -1.0f, -1.0f, 0.0, 0.0,
            1.0f, -1.0f, -1.0f, 0.0, 0.0,
            1.0f,  1.0f, -1.0f, 0.0, 0.0,
            -1.0f,  1.0f, -1.0f, 0.0, 0.0,

            -1.0f, -1.0f,  1.0f, 0.0, 0.0,
            -1.0f, -1.0f, -1.0f, 0.0, 0.0,
            -1.0f,  1.0f, -1.0f, 0.0, 0.0,
            -1.0f,  1.0f, -1.0f, 0.0, 0.0,
            -1.0f,  1.0f,  1.0f, 0.0, 0.0,
            -1.0f, -1.0f,  1.0f, 0.0, 0.0,

            1.0f, -1.0f, -1.0f, 0.0, 0.0,
            1.0f, -1.0f,  1.0f, 0.0, 0.0,
            1.0f,  1.0f,  1.0f, 0.0, 0.0,
            1.0f,  1.0f,  1.0f, 0.0, 0.0,
            1.0f,  1.0f, -1.0f, 0.0, 0.0,
            1.0f, -1.0f, -1.0f, 0.0, 0.0,

            -1.0f, -1.0f,  1.0f, 0.0, 0.0,
            -1.0f,  1.0f,  1.0f, 0.0, 0.0,
            1.0f,  1.0f,  1.0f, 0.0, 0.0,
            1.0f,  1.0f,  1.0f, 0.0, 0.0,
            1.0f, -1.0f,  1.0f, 0.0, 0.0,
            -1.0f, -1.0f,  1.0f, 0.0, 0.0,

            -1.0f,  1.0f, -1.0f, 0.0, 0.0,
            1.0f,  1.0f, -1.0f, 0.0, 0.0,
            1.0f,  1.0f,  1.0f, 0.0, 0.0,
            1.0f,  1.0f,  1.0f, 0.0, 0.0,
            -1.0f,  1.0f,  1.0f, 0.0, 0.0,
            -1.0f,  1.0f, -1.0f, 0.0, 0.0,

            -1.0f, -1.0f, -1.0f, 0.0, 0.0,
            -1.0f, -1.0f,  1.0f, 0.0, 0.0,
            1.0f, -1.0f, -1.0f, 0.0, 0.0,
            1.0f, -1.0f, -1.0f, 0.0, 0.0,
            -1.0f, -1.0f,  1.0f, 0.0, 0.0,
            1.0f, -1.0f,  1.0f, 0.0, 0.0
        };
};

class ShadowMap{
    public:
        inline static std::vector<float> shadowCascadeLevels;
        unsigned int depthMapFBO, depthMaps, matricesUBO;

        glm::mat4 getViewMatrix(const Camera& camera, float nearPlane, float farPlane);
        std::vector<glm::mat4> getViewMatrices(const Camera& camera);

        ShadowMap(const Camera& camera){
            shadowCascadeLevels = std::vector<float>{camera.FAR_FRUSTUM / 16.f, camera.FAR_FRUSTUM / 4.f, camera.FAR_FRUSTUM / 2.f};
            bindMesh();
        }
    private:
        void bindMesh();
        inline static bool hasBindedTextures = false;
};
