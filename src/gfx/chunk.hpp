#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "mesh.hpp"
#include "../shaders/shaders.hpp"
#include "../structures/octree.hpp"

class Chunk{
    public:
        static const int CHUNK_SIZE = 32;
        int xCoordinate, zCoordinate;

        Mesh mesh;
        Octree* octree;

        void draw(const Shader &shader);

        Chunk(int _xCoordinate, int _zCoordinate){
            this->xCoordinate = _xCoordinate;
            this->zCoordinate = _zCoordinate;
            octree = new Octree(_xCoordinate, _zCoordinate);
        }

        ~Chunk(){
            delete octree;
        }
        
        Chunk() = default;
};

#endif