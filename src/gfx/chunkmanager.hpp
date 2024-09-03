#ifndef CHUNKMANAGER_HPP
#define CHUNKMANAGER_HPP

#include <map>
#include "mesh.hpp"
#include "../shaders/shaders.hpp"

class ChunkManager{
    public:
        std::map<std::pair<int, int>, Chunk*> chunkMap;

        void appendChunk(int x, int z);
        Chunk* getChunk(int x, int z);
        Mesh buildMesh(Chunk* _chunk, const Shader &shader, int LoD);

        ChunkManager(){
        }
    private:
        // These shouldn't really be inside ChunkManager, but in a meshing class, couldnt fix that so now its here for now
        int getBlockValueFromPosition(Chunk* chunk, int x, int y, int z, int LoD);
        void makeVoxelAccountedFor(bool bitset[], int x, int y, int z);
        bool isAirBlock(Chunk* chunk, int x, int y, int z, int LoD);
        bool isAccountedFor(bool accountedVoxels[], int x, int y, int z);
        bool isFacingAirblock(Chunk* chunk, int x, int y, int z, int reverseConstant, int constantPos, int LoD);
        void getTextureCoordinates(int textureValue, float &u, float &v);
};

#endif