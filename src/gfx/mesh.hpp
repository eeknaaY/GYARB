#pragma once

#include <vector>

#include "../shaders/shaders.hpp"
#include "glm/glm.hpp"

struct Vertex {
    int Position;
    glm::vec2 TexCoords;

    Vertex(int _x, int _y, int _z, float u, float v){
        Position = (_x << 12) | (_y << 6) | _z;

        TexCoords.x = u;
        TexCoords.y = v;
    }
};

class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        bool bufferExists;

        void draw(const Shader &shader, int x, int y, int z);
        void updateMesh();
        void bindMesh();
        Mesh();
        ~Mesh();
        Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices);

    private:
        unsigned int VAO, VBO, EBO, texture = 1;
};