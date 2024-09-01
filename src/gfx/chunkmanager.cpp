#include <map>
#include <vector>
#include "chunk.hpp"
#include "voxelobject.hpp"
#include "mesh.hpp"
#include <stdexcept>      // std::out_of_range
#include "../structures/octree.hpp"
#include "../shaders/shaders.hpp"

struct voxelFace{
    short x, y, z, width = 1, height = 1, texture;

    voxelFace(short _x, short _y, short _z){
        x = _x;
        y = _y;
        z = _z;
    }
};

class ChunkManager{
    public:
        void appendChunk(int x, int z);
        Chunk* getChunk(int x, int z);

        Mesh getBufferArray(Chunk* _chunk, Shader shader);
        std::map<std::pair<int, int>, Chunk*> chunkMap;

        ChunkManager(){
        }
    private:
        // These shouldn't really be inside ChunkManager, but in a meshing class, couldnt fix that so now its here for now
        int getBlockValueFromPosition(Chunk* chunk, int x, int y, int z);
        void makeVoxelAccountedFor(bool bitset[], int x, int y, int z);
        bool isAirBlock(Chunk* chunk, int x, int y, int z);
        bool isAccountedFor(bool accountedVoxels[], int x, int y, int z);
        bool isFacingAirblock(Chunk* chunk, int x, int y, int z, int reverseConstant, int constantPos);
        void getTextureCoordinates(int textureValue, float &u, float &v);
};

void ChunkManager::appendChunk(int x, int z){
    chunkMap[std::make_pair(x, z)] = new Chunk(x, z);
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

int ChunkManager::getBlockValueFromPosition(Chunk* chunk, int x, int y, int z){
    if (y >= 32) return 0;
    if (y <= -1) return 0;

    bool chunkChanged = false;
    Chunk* _chunk;
    if (x >= 32){
        chunkChanged = true;
        _chunk = getChunk(chunk->xCoordinate + 1, chunk->zCoordinate);   
        x-=32;
    }
    if (x <= -1){
        chunkChanged = true;
        _chunk = getChunk(chunk->xCoordinate - 1, chunk->zCoordinate); 
        x+=32;
    } 
    if (z >= 32){
        chunkChanged = true;
        _chunk = getChunk(chunk->xCoordinate, chunk->zCoordinate + 1); 
        z-=32;
    } 
    if (z <= -1){
        chunkChanged = true;
        _chunk = getChunk(chunk->xCoordinate, chunk->zCoordinate - 1); 
        z+=32;
    }


    if (chunkChanged && !_chunk) return 0;
    if (chunkChanged) return _chunk->octree.getNodeFromPosition(x, y, z)->blockValue;
    return chunk->octree.getNodeFromPosition(x, y, z)->blockValue;
}

bool ChunkManager::isAccountedFor(bool accountedVoxels[], int x, int y, int z){
    return accountedVoxels[x + 32 * z + 32 * 32 * y];
}

bool ChunkManager::isAirBlock(Chunk* chunk, int x, int y, int z){
    return getBlockValueFromPosition(chunk, x, y, z) == 0;
}

bool ChunkManager::isFacingAirblock(Chunk* chunk, int x, int y, int z, int reverseConstant, int constantPos){
    if (constantPos == 1) x = x + reverseConstant;
    if (constantPos == 2) y = y + reverseConstant;
    if (constantPos == 3) z = z + reverseConstant;
    return getBlockValueFromPosition(chunk, x, y, z) == 0;
}

void ChunkManager::getTextureCoordinates(int textureValue, float &u, float &v){
    const float textureBlockSize = 0.0625f;
    textureValue -= 1;
    u = textureBlockSize * (textureValue % 16);
    v = textureBlockSize * (textureValue - textureValue % 16)/16; 
}

#include <chrono>
#include <iostream>

Mesh ChunkManager::getBufferArray(Chunk* _chunk, Shader shader){
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    int reverseConstant = 1;
    for (int w = 0; w < 6; w++){
        std::vector<voxelFace> faces;
        bool accountedVoxels[32 * 32 * 32] = {false};
        int voxelWidth = 1;

        for (int y = 0; y < 32; y++){
        for (int z = 0; z < 32; z++){
        for (int x = 0; x < 32; x++){
            reverseConstant = 1;
            if (w == 1 || w == 3 || w == 5) reverseConstant = -1;
            
            // TOP & BOTTOM FACE
            if (w == 0 || w == 1){
                if (!isAccountedFor(accountedVoxels, x, y, z) && 
                     isFacingAirblock(_chunk, x, y, z, reverseConstant, 2) && 
                    !isAirBlock(_chunk, x, y, z)){

                    voxelFace face(x, y, z);
                    face.texture = _chunk->octree.getNodeFromPosition(x, y, z, voxelWidth)->blockValue;
                    face.width = voxelWidth; 

                    for (int dx = x + voxelWidth; dx < 32; dx++){
                        if (!isAccountedFor(accountedVoxels, dx, y, z) &&
                             isFacingAirblock(_chunk, dx, y, z, reverseConstant, 2) && 
                            !isAirBlock(_chunk, dx, y, z) && 
                             _chunk->octree.getNodeFromPosition(dx, y, z, voxelWidth)->blockValue == face.texture){

                            face.width += voxelWidth;
                            dx += voxelWidth - 1;
                        } else{
                            break;
                        }
                    }

                    bool shouldBreak = false;
                    for (int dz = z + 1; dz < 32; dz++){
                        for (int dx = x; dx < x + face.width; dx++){
                            if (!isAccountedFor(accountedVoxels, dx, y, dz) &&
                                 isFacingAirblock(_chunk, dx, y, dz, reverseConstant, 2) && 
                                !isAirBlock(_chunk, dx, y, dz) && 
                                 _chunk->octree.getNodeFromPosition(dx, y, dz, voxelWidth)->blockValue == face.texture){
                                    dx += voxelWidth - 1;
                                 }
                            else{
                                shouldBreak = true;
                                break;
                            }
                        }

                        if (shouldBreak) break;
                        face.height += 1;
                    }

                for (int dx = x; dx < x + face.width; dx++){
                    for (int dz = z; dz < z + face.height; dz++){
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
                 isFacingAirblock(_chunk, x, y, z, reverseConstant, 1) && 
                !isAirBlock(_chunk, x, y, z)){

                voxelFace face(x, y, z);
                face.texture = _chunk->octree.getNodeFromPosition(x, y, z, voxelWidth)->blockValue;
                face.width = voxelWidth; 

                for (int dz = z + voxelWidth; dz < 32; dz++){
                    bool a = !isAccountedFor(accountedVoxels, x, y, dz);
                    bool b = isFacingAirblock(_chunk, x, y, dz, reverseConstant, 1);
                    bool c = !isAirBlock(_chunk, x, y, dz);
                    if (!isAccountedFor(accountedVoxels, x, y, dz) &&
                         isFacingAirblock(_chunk, x, y, dz, reverseConstant, 1) && 
                        !isAirBlock(_chunk, x, y, dz) && 
                         _chunk->octree.getNodeFromPosition(x, y, dz, voxelWidth)->blockValue == face.texture){

                        face.width += voxelWidth;
                        dz += voxelWidth - 1;
                    } else{
                        break;
                    }
                }

                bool shouldBreak = false;
                for (int dy = y + 1; dy < 32; dy++){
                    for (int dz = z; dz < z + face.width; dz++){
                        if (!isAccountedFor(accountedVoxels, x, dy, dz) &&
                             isFacingAirblock(_chunk, x, dy, dz, reverseConstant, 1) && 
                            !isAirBlock(_chunk, x, dy, dz) && 
                             _chunk->octree.getNodeFromPosition(x, dy, dz, voxelWidth)->blockValue == face.texture){
                                dz += voxelWidth - 1;
                             }
                        else{
                            shouldBreak = true;
                            break;
                        }
                    }

                    if (shouldBreak) break;
                    face.height += 1;
                }

                for (int dy = y; dy < y + face.height; dy++){
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
                 isFacingAirblock(_chunk, x, y, z, reverseConstant, 3) && 
                !isAirBlock(_chunk, x, y, z)){

                voxelFace face(x, y, z);
                face.texture = _chunk->octree.getNodeFromPosition(x, y, z, voxelWidth)->blockValue;
                face.width = voxelWidth; 

                for (int dx = x + voxelWidth; dx < 32; dx++){
                    if (!isAccountedFor(accountedVoxels, dx, y, z) &&
                         isFacingAirblock(_chunk, dx, y, z, reverseConstant, 3) && 
                        !isAirBlock(_chunk, dx, y, z) && 
                         _chunk->octree.getNodeFromPosition(dx, y, z, voxelWidth)->blockValue == face.texture){

                        face.width += voxelWidth;
                        dx += voxelWidth - 1;
                    } else{
                        break;
                    }
                }

                bool shouldBreak = false;
                for (int dy = y + 1; dy < 32; dy++){
                    for (int dx = x; dx < x + face.width; dx++){
                        if (!isAccountedFor(accountedVoxels, dx, dy, z) &&
                             isFacingAirblock(_chunk, dx, dy, z, reverseConstant, 3) && 
                            !isAirBlock(_chunk, dx, dy, z) && 
                             _chunk->octree.getNodeFromPosition(dx, dy, z, voxelWidth)->blockValue == face.texture){
                                    dx += voxelWidth - 1;
                             }

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

            for (int dy = y; dy < y + face.height; dy++){
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

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop-start);
    std::cout << "Meshing took: " << duration.count() << "ms \n";
    return Mesh(vertices, indices);
}

