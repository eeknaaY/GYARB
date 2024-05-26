#include <map>
#include <vector>
#include "chunk.hpp"
#include "voxelobject.hpp"
#include <stdexcept>      // std::out_of_range



class ChunkManager{
    public:
        std::map<int, std::map<int, Chunk>> chunkMap;
        
        void appendChunk(Chunk _chunk);
        std::vector<uint8_t> getTextureVectorFromPosition(int x, int z);
        std::vector<Voxel> getBufferArray(Chunk _chunk);
        void setInvisibleTextureVector();
};

unsigned int getTextureVectorIndexFromPosition(int x, int y, int z);

// #define LEFT_FACE std::vector<unsigned int>{10, 11, 8, 10, 8, 9}

std::vector<Voxel> ChunkManager::getBufferArray(Chunk _chunk){
    std::vector<Voxel> voxelBufferArray;
    int xCoordinate = _chunk.xCoordinate;
    int zCoordinate = _chunk.zCoordinate;
    int chunkSize = Chunk::CHUNK_SIZE;
    int chunkHeight = Chunk::CHUNK_HEIGHT;

    for (int y = 0; y < chunkHeight ; y++){
        for(int z = 0; z < chunkSize; z++){
            for(int x = 0; x < chunkSize; x++){
                // FIXME : JUST FOR NOW : DECIDE WHAT TO DO WHEN YOU HAVE AUTO GENERATING TERRAIN, HIDING ALL BLOCKS UNDERNEATH
                if (y < 44) continue;
                
                if (_chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x, y, z)] == 0) {continue;}
                
                // FIXME : Check the adjacent Chunk 
                Voxel _voxel = Voxel(xCoordinate * chunkSize + x, y, zCoordinate * chunkSize + z);

                bool HAS_LEFT_FACE = false;
                bool HAS_RIGHT_FACE = false;
                bool HAS_BACK_FACE = false;
                bool HAS_FRONT_FACE = false;
                
                if (x == 0 && getTextureVectorFromPosition(xCoordinate - 1, zCoordinate)[getTextureVectorIndexFromPosition(chunkSize - 1, y, z)] == 0){
                    // voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    _voxel.AddToIndices(std::vector<unsigned int>{10, 11, 8, 10, 8, 9});
                    HAS_LEFT_FACE = true;
                }
                
                if (x == 9 && getTextureVectorFromPosition(xCoordinate + 1, zCoordinate)[getTextureVectorIndexFromPosition(0, y, z)] == 0){
                    //voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    _voxel.AddToIndices(std::vector<unsigned int>{15, 13, 14, 15, 14, 12});
                    HAS_RIGHT_FACE = true;
                }

                if (z == 0 && getTextureVectorFromPosition(xCoordinate, zCoordinate - 1)[getTextureVectorIndexFromPosition(x, y, chunkSize - 1)] == 0){
                    //voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    _voxel.AddToIndices(std::vector<unsigned int>{0, 1, 2, 0, 3, 1});
                    HAS_BACK_FACE = true;
                }

                if (z == 9 && getTextureVectorFromPosition(xCoordinate, zCoordinate + 1)[getTextureVectorIndexFromPosition(x, y, 0)] == 0){
                    //voxelBufferArray.push_back(glm::vec3(chunkSize * xCoordinate + x, y, chunkSize * zCoordinate + z));
                    _voxel.AddToIndices(std::vector<unsigned int>{4, 5, 6, 4, 6, 7});
                    HAS_FRONT_FACE = true;
                }

                // FIXME : Place bottom tiles?
                // if (y==0){
                //     _voxel.AddToIndices(std::vector<unsigned int>{18, 19, 16, 18, 16, 17});
                // } 
                    
                // FIXME : Nearby blocks thats not on a different chunk
                if (x!=9 && !HAS_RIGHT_FACE && _chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x + 1, y, z)] == 0){
                    _voxel.AddToIndices(std::vector<unsigned int>{15, 13, 14, 15, 14, 12});
                }

                if (x!=0 && !HAS_LEFT_FACE && _chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x - 1, y, z)] == 0){
                    _voxel.AddToIndices(std::vector<unsigned int>{10, 11, 8, 10, 8, 9});
                }

                if (_chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x, y + 1, z)] == 0){
                    _voxel.AddToIndices(std::vector<unsigned int>{23, 21, 22, 23, 22, 20});
                }

                if (y!=0 && _chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x, y - 1, z)] == 0){
                    _voxel.AddToIndices(std::vector<unsigned int>{18, 19, 16, 18, 16, 17});
                }

                if (z!=9 && !HAS_FRONT_FACE && _chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x, y, z + 1)] == 0){
                    _voxel.AddToIndices(std::vector<unsigned int>{4, 5, 6, 4, 6, 7});
                }

                if (z!=0 && !HAS_BACK_FACE && _chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x, y, z - 1)] == 0){
                    _voxel.AddToIndices(std::vector<unsigned int>{0, 1, 2, 0, 3, 1});
                }

                if (_chunk.voxelTextureArray[getTextureVectorIndexFromPosition(x, y + 1, z)] == 0){
                    _voxel.AddToIndices(std::vector<unsigned int>{23, 21, 22, 23, 22, 20});
                }

                if (_voxel.indices.size() != 0){
                    voxelBufferArray.push_back(_voxel);
                }
            }
        }
    }

    return voxelBufferArray;
}

void ChunkManager::appendChunk(Chunk _chunk){
    chunkMap[_chunk.xCoordinate][_chunk.zCoordinate] = _chunk;
};

std::vector<uint8_t> emptyVoxelTextureArray;

void ChunkManager::setInvisibleTextureVector(){
    emptyVoxelTextureArray = std::vector<uint8_t>(Chunk::CHUNK_HEIGHT * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE, 0);
    // std::fill_n(voxelTextureArray, 1000, 1);
};

std::vector<uint8_t> ChunkManager::getTextureVectorFromPosition(int x, int z){
    try{
        return chunkMap.at(x).at(z).voxelTextureArray;
    }
    catch (const std::out_of_range& oor){
        return emptyVoxelTextureArray;
    }
};

unsigned int getTextureVectorIndexFromPosition(int x, int y, int z){
    return 100 * y + 10 * z + x;
};


