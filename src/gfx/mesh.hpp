#pragma once

#include <vector>
#include "../shaders/shaders.hpp"
#include "glm/glm.hpp"

struct Vertex {
    int Position;
    glm::vec2 TexCoords;

    Vertex(int _x, int _y, int _z, float u, float v, int faceIndex, int textureID){
        // XYZ = 6 bits
        // faceIndex = 3 bits
        // textureID 
        Position =  (textureID << 21) | (faceIndex << 18) | (_x << 12) | (_y << 6) | _z;
        TexCoords.x = u;
        TexCoords.y = v;
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
        void bindMesh();

        Mesh();
        ~Mesh();
        Mesh(std::vector<Vertex> solid_vertices, std::vector<unsigned int> solid_indices, std::vector<Vertex> transparent_vertices, std::vector<unsigned int> transparent_indices);

    protected:
        unsigned int solidVAO, solidVBO, solidEBO, transparentVAO, transparentVBO, transparentEBO, texture = 1;
};

class SkyboxMesh : public Mesh{
    public:
        void draw(const Shader &shader);
        void bindMesh();
        
    private:
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

class ShadowMapping{
    public:
        void bindMesh();
        unsigned int depthMapFBO, depthMap;
};
