#include <glm/vec3.hpp>
#include <algorithm>
#include <vector>
#include <map>
#include <tuple>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const int chunkSize = 10;

class Chunk{
    
    public:
        unsigned int VAO;
        unsigned int VBO;
        
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

    private:
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Chunk::setChunkTextures(){
    // FIXME : Set the different textures, 0 == Air/See through
    // std::fill_n(voxelTextureArray, 1000, 1);
    voxelTextureArray = std::vector<int>(1000, 1);
    for (int i = 900; i<1000; i++){
        voxelTextureArray[i] = 0;
    }
        
}

unsigned int Chunk::getTextureFromPosition(int x, int y, int z){
    return voxelTextureArray[100 * y + 10 * z + x];
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

