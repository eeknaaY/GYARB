#ifndef CHUNK_HPP
#define CHUNK_HPP


#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "voxelobject.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Chunk{
    public:
        static const int CHUNK_SIZE = 10;
        static const int CHUNK_HEIGHT = 50;
        int xCoordinate, zCoordinate;

        std::vector<uint8_t> voxelTextureArray;
        std::vector<Voxel> voxelArray;
        
        uint8_t getTextureFromPosition(int x, int y, int z);
        void changeVoxelArray(std::vector<Voxel> arr);
        
        void setChunkTextures();
        void setChunkTexture(int x, int y, int z, int textureValue);

        Chunk(int _xCoordinate, int _zCoordinate){
            this->xCoordinate = _xCoordinate;
            this->zCoordinate = _zCoordinate;

            this->voxelTextureArray = std::vector<uint8_t>(CHUNK_HEIGHT * CHUNK_SIZE * CHUNK_SIZE, 1);
            this->setChunkTextures();
        }

        Chunk() = default;
};

#endif