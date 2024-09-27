#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "mesh.hpp"
#include "../shaders/shaders.hpp"
#include "../structures/octree.hpp"
#include "FastNoiseLite.h"

#include <memory>
#include <iostream>

class Chunk{
    public:
        static const int CHUNK_SIZE = 32;
        int xCoordinate, yCoordinate, zCoordinate, currentLoD;

        Mesh mesh;
        Octree* octree;

        void draw(const Shader &shader);
        void updateMesh();

        Chunk(int _xCoordinate, int _yCoordinate, int _zCoordinate, int LoD, FastNoiseLite noise){
            this->xCoordinate = _xCoordinate;
            this->zCoordinate = _zCoordinate;
            this->yCoordinate = _yCoordinate;
            currentLoD = LoD;
            octree = new Octree(_xCoordinate, _yCoordinate, _zCoordinate, noise);
        }

        ~Chunk(){
            delete octree;
            octree = 0;
        }
        
        Chunk() = default;
};

#endif