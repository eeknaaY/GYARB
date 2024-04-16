#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <glm/glm.hpp>
#include <map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Chunk{
    public:
        static const int CHUNK_SIZE = 10;
        static const int CHUNK_HEIGHT = 50;
        int xCoordinate, zCoordinate;
        // FIXME : Smaller allocation?
        std::vector<uint8_t> voxelTextureArray;

        unsigned int getTextureFromPosition(int x, int y, int z);
        void setInvisibleChunkTextures();
        
        void initializeChunkTextureVector();
        void setChunkTextures();
        void setChunkTexture(int x, int y, int z, int textureValue);

        Chunk(int _xCoordinate, int _zCoordinate){
            xCoordinate = _xCoordinate;
            zCoordinate = _zCoordinate;
        }
        Chunk() = default;

    private:
};

