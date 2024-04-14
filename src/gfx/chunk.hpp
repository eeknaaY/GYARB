#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <glm/glm.hpp>
#include <map>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Chunk{
    public:
        unsigned int VAO;
        unsigned int VBO;
        static const int chunkSize = 10;

        int xCoordinate, zCoordinate;
        // FIXME : Smaller allocation?
        //unsigned int voxelTextureArray[1000];
        std::vector<int> voxelTextureArray;
//         std::vector<glm::vec3> voxelBufferArray;

        void setChunkTextures();
        unsigned int getTextureFromPosition(int x, int y, int z);
        void setInvisibleChunkTextures();

        Chunk(int _xCoordinate, int _zCoordinate){
            xCoordinate = _xCoordinate;
            zCoordinate = _zCoordinate;
        }
        Chunk() = default;
};
