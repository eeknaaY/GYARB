#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "voxelobject.hpp"
#include "../shaders/shaders.hpp"
#include "mesh.hpp"
#include "../structures/octree.hpp"

class Chunk{
    public:
        static const int CHUNK_SIZE = 32;
        int xCoordinate, zCoordinate;

        Mesh mesh;
        Octree octree = Octree();

        void draw(const Shader &shader);

        Chunk(int _xCoordinate, int _zCoordinate){
            this->xCoordinate = _xCoordinate;
            this->zCoordinate = _zCoordinate;
        }

        Chunk() = default;
};

#endif