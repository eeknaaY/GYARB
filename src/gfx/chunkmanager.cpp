#include <map>
#include <vector>
#include "chunk.hpp"
#include "voxelobject.hpp"
#include "mesh.hpp"
#include <stdexcept>      // std::out_of_range
#include "../structures/octree.hpp"
#include "../shaders/shaders.hpp"

struct voxelFace{
    short x, y, z, width = 0, height = 1, texture;

    voxelFace(short _x, short _y, short _z){
        x = _x;
        y = _y;
        z = _z;
    }
};

class ChunkManager{
    public:
        void appendChunk(Chunk _chunk);
        Chunk* getChunk(int x, int z);

        Mesh getBufferArray(Chunk* _chunk, Shader shader);
        std::map<std::pair<int, int>, Chunk*> chunkMap;

        ChunkManager(){
        }
    private:
        // These shouldn't really be inside ChunkManager, but in a meshing class, couldnt fix that so now its here for now
        int getBlockValueFromPosition(Octree &octree, int x, int y, int z);
        void makeVoxelAccountedFor(bool bitset[], int x, int y, int z);
        bool isAirBlock(Octree &octree, int x, int y, int z);
        bool isAccountedFor(bool accountedVoxels[], int x, int y, int z);
        bool isFacingAirblock(Octree &octree, int x, int y, int z, int reverseConstant, int constantPos);
        void getTextureCoordinates(int textureValue, float &u, float &v);
};

void ChunkManager::appendChunk(Chunk _chunk){
    chunkMap[std::make_pair(_chunk.xCoordinate, _chunk.zCoordinate)] = new Chunk(_chunk.xCoordinate, _chunk.zCoordinate);
}

Chunk* ChunkManager::getChunk(int x, int z){
    try{
        return chunkMap.at(std::make_pair(x, z));
    }
    catch (const std::out_of_range& oor){
        return nullptr;
    }
}

////////////////////////////////////////////////////  MESHING PART ///////////////////////////////////////////////////////////////////////////////

inline void ChunkManager::makeVoxelAccountedFor(bool bitset[], int x, int y, int z){
    bitset[x + 32 * z + 32 * 32 * y] = true;
}

int ChunkManager::getBlockValueFromPosition(Octree &octree, int x, int y, int z){
    if (y >= 32) return 0;
    if (y <= -1) return 0;
    if (x >= 32) return 0;
    if (x <= -1) return 0;
    if (z >= 32) return 0;
    if (z <= -1) return 0;
    return octree.getNodeFromPosition(x, y, z)->blockValue;
}

bool ChunkManager::isAccountedFor(bool accountedVoxels[], int x, int y, int z){
    return accountedVoxels[x + 32 * z + 32 * 32 * y];
}

bool ChunkManager::isAirBlock(Octree &octree, int x, int y, int z){
    return getBlockValueFromPosition(octree, x, y, z) == 0;
}

bool ChunkManager::isFacingAirblock(Octree &octree, int x, int y, int z, int reverseConstant, int constantPos){
    if (constantPos == 1) x = x + reverseConstant;
    if (constantPos == 2) y = y + reverseConstant;
    if (constantPos == 3) z = z + reverseConstant;
    return getBlockValueFromPosition(octree, x, y, z) == 0;
}

void ChunkManager::getTextureCoordinates(int textureValue, float &u, float &v){
    const float textureBlockSize = 0.0625f;
    textureValue -= 1;
    u = textureBlockSize * (textureValue % 16);
    v = textureBlockSize * (textureValue - textureValue % 16)/16; 
}

Mesh ChunkManager::getBufferArray(Chunk* _chunk, Shader shader){
    Octree octree = _chunk->octree;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    int reverseConstant = 1;
    for (int w = 0; w < 6; w++){
        std::vector<voxelFace> faces;
        bool accountedVoxels[32 * 32 * 32] = {false};

        for (int y = 0; y < 32; y++){
        for (int z = 0; z < 32; z++){
        for (int x = 0; x < 32; x++){
            reverseConstant = 1;
            if (w == 1 || w == 3 || w == 5) reverseConstant = -1;
            
            // TOP & BOTTOM FACE
            if (w == 0 || w == 1){
                if (!isAccountedFor(accountedVoxels, x, y, z) && 
                     isFacingAirblock(octree, x, y, z, reverseConstant, 2) && 
                    !isAirBlock(octree, x, y, z)){

                    voxelFace face(x, y, z);
                    face.texture = octree.getNodeFromPosition(x, y, z)->blockValue;

                    for (int dx = x; dx < 32; dx++){
                        if (!isAccountedFor(accountedVoxels, dx, y, z) &&
                             isFacingAirblock(octree, dx, y, z, reverseConstant, 2) && 
                            !isAirBlock(octree, dx, y, z) && 
                             octree.getNodeFromPosition(dx, y, z)->blockValue == face.texture){

                            accountedVoxels[dx + 32 * z + 32 * 32 * y] = true;
                            face.width += 1;
                        } else{
                            break;
                        }
                    }

                    bool shouldBreak = false;
                    for (int dz = z + 1; dz < 32; dz++){
                        for (int dx = x; dx < x + face.width; dx++){
                            if (!isAccountedFor(accountedVoxels, dx, y, dz) &&
                                 isFacingAirblock(octree, dx, y, dz, reverseConstant, 2) && 
                                !isAirBlock(octree, dx, y, dz) && 
                                 octree.getNodeFromPosition(dx, y, dz)->blockValue == face.texture){}
                            else{
                                shouldBreak = true;
                                break;
                            }
                        }

                        if (shouldBreak) break;
                        face.height += 1;
                        for (int dx = x; dx < x + face.width; dx++){
                            accountedVoxels[dx + 32 * dz + 32 * 32 * y] = true;
                        }
                        
                    }

                int vertexCount = vertices.size();
                float u, v;
                getTextureCoordinates(face.texture, u, v);
                if (w == 0){
                    vertices.push_back(Vertex(face.x,              face.y + 1, face.z,               u,              v)); 
                    vertices.push_back(Vertex(face.x + face.width, face.y + 1, face.z + face.height, u + face.width, v + face.height));
                    vertices.push_back(Vertex(face.x + face.width, face.y + 1, face.z,               u + face.width, v));
                    vertices.push_back(Vertex(face.x,              face.y + 1, face.z + face.height, u,              v + face.height));

                    indices.push_back(vertexCount + 3);
                    indices.push_back(vertexCount + 1);
                    indices.push_back(vertexCount + 2);
                    indices.push_back(vertexCount + 3);
                    indices.push_back(vertexCount + 2);
                    indices.push_back(vertexCount + 0);
                } else {
                    vertices.push_back(Vertex(face.x,              face.y, face.z,               u,              v)); 
                    vertices.push_back(Vertex(face.x + face.width, face.y, face.z,               u + face.width, v));
                    vertices.push_back(Vertex(face.x + face.width, face.y, face.z + face.height, u + face.width, v + face.height));
                    vertices.push_back(Vertex(face.x,              face.y, face.z + face.height, u,              v + face.height));

                    indices.push_back(vertexCount + 2);
                    indices.push_back(vertexCount + 3);
                    indices.push_back(vertexCount + 0);
                    indices.push_back(vertexCount + 2);
                    indices.push_back(vertexCount + 0);
                    indices.push_back(vertexCount + 1);
                }
            }
        }

        // LEFT & RIGHT FACE
        if (w == 2 || w == 3){
            if (!isAccountedFor(accountedVoxels, x, y, z) &&
                 isFacingAirblock(octree, x, y, z, reverseConstant, 1) && 
                !isAirBlock(octree, x, y, z)){

                voxelFace face(x, y, z);
                face.texture = octree.getNodeFromPosition(x, y, z)->blockValue;

                for (int dz = z; dz < 32; dz++){
                    if (!isAccountedFor(accountedVoxels, x, y, dz) &&
                         isFacingAirblock(octree, x, y, dz, reverseConstant, 1) && 
                        !isAirBlock(octree, x, y, dz) && 
                         octree.getNodeFromPosition(x, y, dz)->blockValue == face.texture){

                        accountedVoxels[x + 32 * dz + 32 * 32 * y] = true;
                        face.width += 1;
                    } else{
                        break;
                    }
                }

                bool shouldBreak = false;
                for (int dy = y + 1; dy < 32; dy++){
                    for (int dz = z; dz < z + face.width; dz++){
                        if (!isAccountedFor(accountedVoxels, x, dy, dz) &&
                             isFacingAirblock(octree, x, dy, dz, reverseConstant, 1) && 
                            !isAirBlock(octree, x, dy, dz) && 
                             octree.getNodeFromPosition(x, dy, dz)->blockValue == face.texture){}
                        else{
                            shouldBreak = true;
                            break;
                        }
                    }

                    if (shouldBreak) break;
                    face.height += 1;
                    for (int dz = z; dz < z + face.width; dz++){
                        accountedVoxels[x + 32 * dz + 32 * 32 * dy] = true;
                    }
                    
                }

                int vertexCount = vertices.size();
                float u, v;
                getTextureCoordinates(face.texture, u, v);
                if (w == 3){
                    vertices.push_back(Vertex(face.x, face.y + face.height, face.z + face.width, u + face.width, v + face.height));
                    vertices.push_back(Vertex(face.x, face.y + face.height, face.z,              u,              v + face.height));
                    vertices.push_back(Vertex(face.x, face.y,               face.z,              u,              v));
                    vertices.push_back(Vertex(face.x, face.y,               face.z + face.width, u + face.width, v));

                    indices.push_back(vertexCount + 2);
                    indices.push_back(vertexCount + 3);
                    indices.push_back(vertexCount + 0);
                    indices.push_back(vertexCount + 2);
                    indices.push_back(vertexCount + 0);
                    indices.push_back(vertexCount + 1);
                } else {
                    vertices.push_back(Vertex(face.x + 1, face.y + face.height, face.z + face.width, u,              v + face.height));
                    vertices.push_back(Vertex(face.x + 1, face.y,               face.z,              u + face.width, v));
                    vertices.push_back(Vertex(face.x + 1, face.y + face.height, face.z,              u + face.width, v + face.height));
                    vertices.push_back(Vertex(face.x + 1, face.y,               face.z + face.width, u,              v));

                    indices.push_back(vertexCount + 3);
                    indices.push_back(vertexCount + 1);
                    indices.push_back(vertexCount + 2);
                    indices.push_back(vertexCount + 3);
                    indices.push_back(vertexCount + 2);
                    indices.push_back(vertexCount + 0);
                }
            }
        }

        // FRONT & BACK FACE
        if (w == 4 || w == 5){
            if (!isAccountedFor(accountedVoxels, x, y, z) &&
                 isFacingAirblock(octree, x, y, z, reverseConstant, 3) && 
                !isAirBlock(octree, x, y, z)){

                voxelFace face(x, y, z);
                face.texture = octree.getNodeFromPosition(x, y, z)->blockValue;

                for (int dx = x; dx < 32; dx++){
                    if (!isAccountedFor(accountedVoxels, dx, y, z) &&
                         isFacingAirblock(octree, dx, y, z, reverseConstant, 3) && 
                        !isAirBlock(octree, dx, y, z) && 
                         octree.getNodeFromPosition(dx, y, z)->blockValue == face.texture){

                        accountedVoxels[dx + 32 * z + 32 * 32 * y] = true;
                        face.width += 1;
                    } else{
                        break;
                    }
                }

                bool shouldBreak = false;
                for (int dy = y + 1; dy < 32; dy++){
                    for (int dx = x; dx < x + face.width; dx++){
                        if (!isAccountedFor(accountedVoxels, dx, dy, z) &&
                             isFacingAirblock(octree, dx, dy, z, reverseConstant, 3) && 
                            !isAirBlock(octree, dx, dy, z) && 
                             octree.getNodeFromPosition(dx, dy, z)->blockValue == face.texture){}

                        else{
                            shouldBreak = true;
                            break;
                        }
                    }

                    if (shouldBreak) break;
                    face.height += 1;
                    for (int dx = x; dx < x + face.width; dx++){
                        accountedVoxels[dx + 32 * z + 32 * 32 * dy] = true;
                    }
                    
                }
            
            int vertexCount = vertices.size();
            float u, v;
            getTextureCoordinates(face.texture, u, v);
            if (w == 4){
                vertices.push_back(Vertex(face.x,              face.y,               face.z + 1, u,              v)); 
                vertices.push_back(Vertex(face.x + face.width, face.y,               face.z + 1, u + face.width, v));
                vertices.push_back(Vertex(face.x + face.width, face.y + face.height, face.z + 1, u + face.width, v + face.height));
                vertices.push_back(Vertex(face.x,              face.y + face.height, face.z + 1, u,              v + face.height));

                indices.push_back(vertexCount + 0);
                indices.push_back(vertexCount + 1);
                indices.push_back(vertexCount + 2);
                indices.push_back(vertexCount + 0);
                indices.push_back(vertexCount + 2);
                indices.push_back(vertexCount + 3);
            } else {
                vertices.push_back(Vertex(face.x,              face.y,               face.z, u,              v)); 
                vertices.push_back(Vertex(face.x + face.width, face.y + face.height, face.z, u + face.width, v + face.height));
                vertices.push_back(Vertex(face.x + face.width, face.y,               face.z, u + face.width, v));
                vertices.push_back(Vertex(face.x,              face.y + face.height, face.z, u,              v + face.height));

                indices.push_back(vertexCount + 0);
                indices.push_back(vertexCount + 1);
                indices.push_back(vertexCount + 2);
                indices.push_back(vertexCount + 0);
                indices.push_back(vertexCount + 3);
                indices.push_back(vertexCount + 1);
                }
            }
        }
    }}}}

    return Mesh(vertices, indices);
}

