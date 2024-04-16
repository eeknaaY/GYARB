#include <glm/vec3.hpp>
#include <algorithm>
#include <vector>
#include <map>
#include <tuple>

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Chunk::setChunkTextures(){
    // FIXME : Set the different textures, 0 == Air/See through
    voxelTextureArray = std::vector<uint8_t>(CHUNK_HEIGHT * CHUNK_SIZE * CHUNK_SIZE, 1);
    for (int i = 4900; i<5000; i++){
        voxelTextureArray[i] = 0;
    }    
}

void Chunk::setChunkTexture(int x, int y, int z, int textureValue){
    voxelTextureArray[CHUNK_SIZE * CHUNK_SIZE * y + CHUNK_SIZE * z + x] = textureValue;   
}

void Chunk::initializeChunkTextureVector(){
    voxelTextureArray = std::vector<uint8_t>(CHUNK_HEIGHT * CHUNK_SIZE * CHUNK_SIZE, 1);
}

unsigned int Chunk::getTextureFromPosition(int x, int y, int z){
    return voxelTextureArray[100 * y + 10 * z + x];
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

