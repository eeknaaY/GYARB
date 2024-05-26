#ifndef CHUNKMANAGER_HPP
#define CHUNKMANAGER_HPP

#include "voxelobject.hpp"
#include <map>

class ChunkManager{
    public:
        std::map<int, std::map<int, Chunk>> chunkMap;
        
        void appendChunk(Chunk _chunk);
        std::vector<uint8_t> getTextureVectorFromPosition(int x, int z);
        std::vector<Voxel> getBufferArray(Chunk _chunk);
        void setInvisibleTextureVector();
};

#endif