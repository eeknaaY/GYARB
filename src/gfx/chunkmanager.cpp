#include <map>
#include <vector>
#include "chunk.hpp"
#include <stdexcept>      // std::out_of_range



class ChunkManager{
    public:
        std::map<int, std::map<int, std::vector<uint8_t>>> chunkMap;
        
        void appendChunk(Chunk _chunk);
        std::vector<uint8_t> getTextureVectorFromPosition(int x, int z);
        std::vector<glm::vec3> getBufferArray(Chunk _chunk);
        void setInvisibleTextureVector();
};

unsigned int getTextureVectorIndexFromPosition(int x, int y, int z);

std::vector<glm::vec3> ChunkManager::getBufferArray(Chunk _chunk){
    std::vector<glm::vec3> voxelBufferArray;
    int xCoordinate = _chunk.xCoordinate;
    int zCoordinate = _chunk.zCoordinate;
    int chunkSize = Chunk::CHUNK_SIZE;
    int chunkHeight = Chunk::CHUNK_HEIGHT;

    for (int y = 0; y < chunkHeight ; y++){
        for(int z = 0; z < chunkSize; z++){
            for(int x = 0; x < chunkSize; x++){

                if (_chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x, y, z)] == 0) {continue;}
                // FIXME : Check the adjacent Chunk
                if (x == 0 && getTextureVectorFromPosition(xCoordinate - 1, zCoordinate)[getTextureVectorIndexFromPosition(chunkSize - 1, y, z)] == 0){
                    voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    continue;
                }

                if (z == 0 && getTextureVectorFromPosition(xCoordinate, zCoordinate - 1)[getTextureVectorIndexFromPosition(x, y, chunkSize - 1)] == 0){
                    voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    continue;
                }

                if (x == 9 && getTextureVectorFromPosition(xCoordinate + 1, zCoordinate)[getTextureVectorIndexFromPosition(0, y, z)] == 0){
                    voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    continue;
                }

                if (z == 9 && getTextureVectorFromPosition(xCoordinate, zCoordinate + 1)[getTextureVectorIndexFromPosition(x, y, 0)] == 0){
                    voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    continue;
                }

                // FIXME : Change to chunksize dependent, places bottom tiles
                if (y==0){
                    voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    continue;
                } 
                    
                // FIXME : Nearby blocks thats not on a different chunk
                if (_chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x + 1, y, z)] == 0){
                    voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    continue;
                }

                if (_chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x - 1, y, z)] == 0){
                    voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    continue;
                }

                if (_chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x, y + 1, z)] == 0){
                    voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    continue;
                }

                if (_chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x, y - 1, z)] == 0){
                    voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    continue;
                }

                if (_chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x, y, z + 1)] == 0){
                    voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    continue;
                }

                if (_chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x, y, z - 1)] == 0){
                    voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    continue;
                }
            }
        }
    }

    return voxelBufferArray;
}

void ChunkManager::appendChunk(Chunk _chunk){
    chunkMap[_chunk.xCoordinate][_chunk.zCoordinate] = _chunk.voxelTextureArray;
};

std::vector<uint8_t> emptyVoxelTextureArray;

void ChunkManager::setInvisibleTextureVector(){
    emptyVoxelTextureArray = std::vector<uint8_t>(Chunk::CHUNK_HEIGHT * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE, 0);
    // std::fill_n(voxelTextureArray, 1000, 1);
};

std::vector<uint8_t> ChunkManager::getTextureVectorFromPosition(int x, int z){
    try{
        return chunkMap.at(x).at(z);
    }
    catch (const std::out_of_range& oor){
        return emptyVoxelTextureArray;
    }
};

unsigned int getTextureVectorIndexFromPosition(int x, int y, int z){
    return 100 * y + 10 * z + x;
};


