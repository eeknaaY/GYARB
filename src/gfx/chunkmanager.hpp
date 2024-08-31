#ifndef CHUNKMANAGER_HPP
#define CHUNKMANAGER_HPP

#include "voxelobject.hpp"
#include <map>
#include "mesh.hpp"
#include "../shaders/shaders.hpp"

class ChunkManager{
    public:
        void appendChunk(Chunk _chunk);
        Chunk* getChunk(int x, int z);

        Mesh getBufferArray(Chunk* _chunk, Shader shader);
        std::map<std::pair<int, int>, Chunk*> chunkMap;

        ChunkManager(){
        }
    private:
        int getBlockValueFromPosition(Octree &octree, int x, int y, int z);
        void makeVoxelAccountedFor(bool bitset[], int x, int y, int z);
        bool isAirBlock(Octree &octree, int x, int y, int z);
        bool isAccountedFor(bool accountedVoxels[], int x, int y, int z);
        bool isFacingAirblock(Octree &octree, int x, int y, int z, int reverseConstant, int constantPos);
};

#endif