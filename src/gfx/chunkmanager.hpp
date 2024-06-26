#ifndef CHUNKMANAGER_HPP
#define CHUNKMANAGER_HPP

#include "voxelobject.hpp"
#include <map>

class ChunkManager{
    public:
        void appendChunk(Chunk _chunk);
        Chunk* getChunk(int x, int z);

        std::vector<uint8_t> getTextureVectorFromPosition(int x, int z);
        std::vector<Voxel> getBufferArray(Chunk* _chunk);
        void setInvisibleTextureVector();
        std::map<std::pair<int, int>, Chunk*> chunkMap;

        
        ChunkManager(){
            setInvisibleTextureVector();
        }
};

#endif