#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "mesh.hpp"
#include "../shaders/shaders.hpp"
#include "../structures/octree.hpp"
#include "FastNoiseLite.h"

#include <memory>
#include <iostream>

enum class Block{
    Air = 0,
    Grass,
    Stone,
    Dirt,
    Grassy_Dirt,
    Wood_Log,
    Leaf,
    Glass,
    Wooden_Plank,
    Water = 17
};

class Chunk{
    public:
        static const int CHUNK_SIZE = 32;
        int xCoordinate, yCoordinate, zCoordinate, currentLoD;

        Biome* biomeType;
        Mesh mesh;
        Mesh backupMesh;
        Octree* octree;
        
        void draw(const Shader &shader);
        void drawTransparent(const Shader &shader);
        void drawOpaque(const Shader &shader);

        void updateMesh();
        void useBackupMesh();
        void updateTransparentMesh();
        void sortTransparentFaces(const Camera& camera);
        void setBlockValue(int x, int y, int z, int blockValue);
        int getBlockValue(int x, int y, int z);
        

        Chunk(int _xCoordinate, int _yCoordinate, int _zCoordinate, int LoD){
            biomeType = BiomeHandler::getBiome(_xCoordinate, _zCoordinate);
            this->xCoordinate = _xCoordinate;
            this->zCoordinate = _zCoordinate;
            this->yCoordinate = _yCoordinate;
            currentLoD = LoD;
            octree = new Octree();
            setInitialBlockValues();
            octree->optimizeTree();
            octreeExists = true;
        }

        Chunk(int fillValue, int _xCoordinate, int _yCoordinate, int _zCoordinate, int LoD){
            biomeType = BiomeHandler::getBiome(_xCoordinate, _zCoordinate);
            this->xCoordinate = _xCoordinate;
            this->zCoordinate = _zCoordinate;
            this->yCoordinate = _yCoordinate;
            currentLoD = LoD;
            // Initialize empty octree
            octree = new Octree(fillValue);
            octreeExists = true;
        }

        ~Chunk(){
            delete octree;
            octree = 0;
        }
        
        Chunk() = default;

    private:
        bool octreeExists = false;
        void setInitialBlockValues();
};

#endif