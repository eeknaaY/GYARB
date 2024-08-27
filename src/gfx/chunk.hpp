#ifndef CHUNK_HPP
#define CHUNK_HPP


#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "voxelobject.hpp"
#include "../shaders/shaders.hpp"
#include "mesh.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Chunk{
    public:
        static const int CHUNK_SIZE = 10;
        static const int CHUNK_HEIGHT = 50;
        int xCoordinate, zCoordinate;

        std::vector<uint8_t> voxelTextureArray;
        std::vector<Voxel> voxelArray;
        std::vector<unsigned int> indices;

        Mesh mesh;
        
        uint8_t getTextureFromPosition(int x, int y, int z);
        void changeVoxelArray(std::vector<Voxel> arr);
        void updateVerticesArray();
        
        void setChunkTextures();
        void setChunkTexture(int x, int y, int z, int textureValue);

        void addToIndices(std::vector<unsigned int> _indices);
        void draw(const Shader &shader);

        Chunk(int _xCoordinate, int _zCoordinate){
            this->xCoordinate = _xCoordinate;
            this->zCoordinate = _zCoordinate;

            this->voxelTextureArray = std::vector<uint8_t>(CHUNK_HEIGHT * CHUNK_SIZE * CHUNK_SIZE, 1);
            this->setChunkTextures();
        }

        Chunk() = default;
};

#endif