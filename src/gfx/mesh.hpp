#pragma once

#include <vector>

#include "../shaders/shaders.hpp"
#include "glm/glm.hpp"

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
        bool bufferExists;

        void draw(const Shader &shader, int x, int z);
        void updateMesh();
        void bindMesh();
        Mesh();
        Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices);

    private:
        unsigned int VAO, VBO, EBO, texture = 1;
};