#pragma once

#include "voxel.hpp"
#include <map>

class ChunkManager{
    public:
        std::map<int, std::map<int, std::vector<uint8_t>>> chunkMap;
        
        void appendChunk(Chunk _chunk);
        std::vector<uint8_t> getTextureVectorFromPosition(int x, int z);
        std::vector<glm::vec3> getBufferArray(Chunk _chunk);
        void setInvisibleTextureVector();
};