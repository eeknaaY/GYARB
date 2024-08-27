#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <vector>
#include <map>
#include <tuple>
#include "voxelobject.hpp"
#include <glad/glad.h>
#include <stdexcept>      // std::out_of_range
#include "../shaders/shaders.hpp"
#include "mesh.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Chunk{
    public:
        static const int CHUNK_SIZE = 10;
        static const int CHUNK_HEIGHT = 50;
        int xCoordinate, zCoordinate;

        // FIXME : Use bitset? Nah not really, maybe, if you can get binary greedy meshing to work
        // std::bitset<CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT> voxelTextureArray;
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

void Chunk::updateVerticesArray(){
    for(Voxel _voxel : voxelArray){
        indices.insert(indices.end(), _voxel.indices.begin(), _voxel.indices.end());
    }
}

void Chunk::setChunkTextures(){
    // FIXME : Set the different textures, 0 == Air/See through
    for (int i = 4900; i<5000; i++){
        voxelTextureArray[i] = 0;
    }

    // FIXME : Change this for the perlin noise
    for (int _x = 0; _x < Chunk::CHUNK_SIZE; _x++){
        for (int _z = 0; _z < Chunk::CHUNK_SIZE; _z++){
            int chosenHeight = 40 + static_cast<int>(cos(time(nullptr))) + static_cast<int>(round(2 * sin(0.3f * (_x + Chunk::CHUNK_SIZE * this->xCoordinate + _z + Chunk::CHUNK_SIZE * this->zCoordinate) )));
            for (int _y = 0; _y < Chunk::CHUNK_HEIGHT; _y++){
                if (_y > chosenHeight) {
                    setChunkTexture(_x, _y, _z, 0);
                }
            }
        }
    }
}

void Chunk::changeVoxelArray(std::vector<Voxel> arr){
    voxelArray = arr;
}

void Chunk::setChunkTexture(int x, int y, int z, int textureValue){
    voxelTextureArray[CHUNK_SIZE * CHUNK_SIZE * y + CHUNK_SIZE * z + x] = textureValue;   
}

uint8_t Chunk::getTextureFromPosition(int x, int y, int z){
    return voxelTextureArray[CHUNK_SIZE * CHUNK_SIZE * y + CHUNK_SIZE * z + x];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Chunk::draw(const Shader &shader){
    mesh.draw(shader, xCoordinate, zCoordinate);
}