#include <map>
#include <vector>
#include "chunk.hpp"
#include "mesh.hpp"
#include <stdexcept>      // std::out_of_range
#include "../structures/octree.hpp"
#include "../shaders/shaders.hpp"
#include "camera.hpp"
#include <iostream>
#include <thread>

struct voxelFace{
    short x, y, z, width = 0, height = 1, texture;

    voxelFace(short _x, short _y, short _z){
        x = _x;
        y = _y;
        z = _z;
    }
};

enum voxelFaces{
    TOP_FACE,
    BOTTOM_FACE,
    FRONT_FACE,
    BACK_FACE,
    LEFT_FACE,
    RIGHT_FACE
};

class ChunkManager{
    public:
        std::vector<Chunk*> finishedMeshes;
        std::vector<std::pair<int, int>> chunksToRemove;

        std::map<std::pair<int, int>, Chunk*> chunkMap;

        void appendChunk(int x, int z, int LoD);
        void appendChunk(Chunk* ptr);
        void removeChunk(int x, int z);
        Chunk* getChunk(int x, int z);
        Mesh buildMesh(Chunk* _chunk, Camera gameCamera);
        void updateChunkMesh(Chunk* _chunk, Camera gameCamera);
        void updateChunkMesh_MT(Chunk* _chunk, Camera gameCamera);
        void testStartMT(Camera* gameCamera);
        void updateTerrain(Camera* gameCamera);

        ChunkManager(){}
    private:
        std::thread testThread;
        
        // These shouldn't really be inside ChunkManager, but in a meshing class, couldnt fix that so now its here for now
        int getBlockValueFromPosition(Chunk* chunk, int x, int y, int z, int LoD);
        void makeVoxelAccountedFor(bool accountedVoxels[], int x, int y, int z);
        bool isAirBlock(Chunk* chunk, int x, int y, int z, int LoD);
        bool isAccountedFor(bool accountedVoxels[], int x, int y, int z);
        bool isFacingAirblock(Chunk* chunk, int x, int y, int z, int reverseConstant, int constantPos, int LoD);
        void getTextureCoordinates(int textureValue, float &u, float &v, voxelFaces face, int LoD);
};

////////////////////////////////////////////////////  CHUNK HANDLING  ///////////////////////////////////////////////////////////////////////////////

void ChunkManager::appendChunk(int x, int z, int LoD){
    chunkMap[std::make_pair(x, z)] = new Chunk(x, z, LoD);
}

void ChunkManager::appendChunk(Chunk* ptr){
    chunkMap[std::make_pair(ptr->xCoordinate, ptr->zCoordinate)] = ptr;
}

void ChunkManager::removeChunk(int x, int z){
    Chunk* ptr = getChunk(x, z);
    delete ptr;
    ptr = 0;
    chunkMap.erase(std::make_pair(x, z));
}

Chunk* ChunkManager::getChunk(int x, int z){
    try{
        return chunkMap.at(std::make_pair(x, z));
    }
    catch (const std::out_of_range& oor){
        return nullptr;
    }
}

void ChunkManager::updateChunkMesh(Chunk* _chunk, Camera gameCamera){
    Mesh updatedMesh = buildMesh(_chunk, gameCamera);
    _chunk->mesh.vertices = updatedMesh.vertices;
    _chunk->mesh.indices = updatedMesh.indices;
    _chunk->updateMesh();
}

void ChunkManager::updateChunkMesh_MT(Chunk* _chunk, Camera gameCamera){
    Mesh updatedMesh = buildMesh(_chunk, gameCamera);
    _chunk->mesh.vertices = updatedMesh.vertices;
    _chunk->mesh.indices = updatedMesh.indices;
}

void ChunkManager::testStartMT(Camera* gameCamera){
    if (testThread.joinable()) testThread.join();
    testThread = std::thread(updateTerrain, this, gameCamera);

}

void ChunkManager::updateTerrain(Camera* gameCamera){
    int c_dXPos = gameCamera->currentChunk_x - gameCamera->oldChunk_x;
    int c_dZPos = gameCamera->currentChunk_z - gameCamera->oldChunk_z;
    int xPos = gameCamera->currentChunk_x;
    int zPos = gameCamera->currentChunk_z;
    if (c_dXPos != 0){
        for (int dz = zPos - 12; dz <= zPos + 12; dz++){
            for (int dx = xPos - 12; dx <= xPos + 12; dx++){
                Chunk* chunk = getChunk(dx, dz);

                if (!chunk){
                    Chunk* _chunk = new Chunk(dx, dz, 5);
                    this->finishedMeshes.push_back(_chunk);
                    continue;
                }

                if (dx == xPos - c_dXPos * 12){
                    removeChunk(dx, dz);
                    continue;
                }

                if (dx == xPos + c_dXPos || dx == xPos || dx == xPos + c_dXPos * 11){
                    updateChunkMesh_MT(chunk, *gameCamera);
                    this->finishedMeshes.push_back(chunk);
                }
            }
        }
    }

    if (c_dZPos != 0){
        for (int dz = zPos - 12; dz <= zPos + 12; dz++){
            for (int dx = xPos - 12; dx <= xPos + 12; dx++){
                Chunk* chunk = getChunk(dx, dz);

                if (!chunk){
                    Chunk* _chunk = new Chunk(dx, dz, 5);
                    this->finishedMeshes.push_back(_chunk);
                    continue;
                }

                if (dz == zPos - c_dZPos * 12){
                    removeChunk(dx, dz);
                    continue;
                }

                if (dz == zPos + c_dZPos || dz == zPos || dz == zPos + c_dZPos * 11){
                    updateChunkMesh_MT(chunk, *gameCamera);
                    this->finishedMeshes.push_back(chunk);
                }
            }
        }
    }
}


////////////////////////////////////////////////////  MESHING  ///////////////////////////////////////////////////////////////////////////////

inline void ChunkManager::makeVoxelAccountedFor(bool accountedVoxels[], int x, int y, int z){
    accountedVoxels[x + 32 * z + 32 * 32 * y] = true;
}

int ChunkManager::getBlockValueFromPosition(Chunk* chunk, int x, int y, int z, int LoD){
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

    if (chunkChanged){
        if (!_chunk) return 0;
        if (chunk->currentLoD < _chunk->currentLoD) return 0;

        Node* node = _chunk->octree->getNodeFromPosition(x, y, z, LoD);
        return node->blockValue;
    }

    Node* node = chunk->octree->getNodeFromPosition(x, y, z, LoD);
    return node->blockValue;
}

bool ChunkManager::isAccountedFor(bool accountedVoxels[], int x, int y, int z){
    if (x > 31) return true;
    if (y > 31) return true;
    if (z > 31) return true;
    if (x < 0) return true;
    if (y < 0) return true;
    if (z < 0) return true;
    return accountedVoxels[x + 32 * z + 32 * 32 * y];
}

bool ChunkManager::isAirBlock(Chunk* chunk, int x, int y, int z, int LoD){
    return getBlockValueFromPosition(chunk, x, y, z, LoD) == 0;
}

bool ChunkManager::isFacingAirblock(Chunk* chunk, int x, int y, int z, int reverseConstant, int constantPos, int LoD){
    switch (constantPos)
    {
    case 1:
        x = x + reverseConstant;
        break;
    case 2:
        y = y + reverseConstant;
        break;
    case 3:
        z = z + reverseConstant;
        break;

    default:
        break;
    }

    return getBlockValueFromPosition(chunk, x, y, z, LoD) == 0;
}

void ChunkManager::getTextureCoordinates(int textureValue, float &u, float &v, voxelFaces face, int LoD){ // Replace sides of a grass block to semi dirt + semi grass
    switch (textureValue)
    {
    case 1:
        if (face != TOP_FACE && LoD == 5) textureValue = 4;
        break;
    
    default:
        break;
    }

    constexpr float textureBlockSize = 0.0625f;

    // Since air is the value 0, we need to remove one value so textures begin at 0.
    textureValue -= 1;


    u = textureBlockSize * (textureValue % 16);
    v = textureBlockSize * (textureValue - textureValue % 16)/16; 
}

#include <chrono>

Mesh ChunkManager::buildMesh(Chunk* _chunk, Camera gameCamera){
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    short voxelWidth;

    int LoD = _chunk->currentLoD;
    int jumpLength = 1 << (5 - LoD);

    int chunk_dx = _chunk->xCoordinate - gameCamera.currentChunk_x;
    int chunk_dz = _chunk->zCoordinate - gameCamera.currentChunk_z;

    for (int currentFace = TOP_FACE; currentFace <= RIGHT_FACE; currentFace++){
        if (chunk_dx > 0 && currentFace == FRONT_FACE) continue;
        if (chunk_dx < 0 && currentFace == BACK_FACE) continue;
        if (chunk_dz < 0 && currentFace == RIGHT_FACE) continue;
        if (chunk_dz > 0 && currentFace == LEFT_FACE) continue;
        if (chunk_dx != 0 && chunk_dz != 0){
            if (chunk_dx == 0 && (currentFace == FRONT_FACE || currentFace == BACK_FACE)) continue;
            if (chunk_dz == 0 && (currentFace == RIGHT_FACE || currentFace == LEFT_FACE)) continue;
        }

        bool accountedVoxels[32 * 32 * 32] = {false};

        int reverseConstant = jumpLength;
        if (currentFace == BOTTOM_FACE || currentFace == BACK_FACE || currentFace == RIGHT_FACE) reverseConstant = -jumpLength;

        for (int y = 0; y < 32; y += jumpLength){
        for (int z = 0; z < 32; z += jumpLength){
        for (int x = 0; x < 32; x += jumpLength){
            
            // TOP & BOTTOM FACE
            if (currentFace == TOP_FACE || currentFace == BOTTOM_FACE){
                if (!isAccountedFor(accountedVoxels, x, y, z) && 
                     isFacingAirblock(_chunk, x, y, z, reverseConstant, 2, LoD) && 
                    !isAirBlock(_chunk, x, y, z, LoD)){

                    voxelFace face(x, y, z);
                    face.texture = _chunk->octree->getNodeFromPosition(x, y, z, voxelWidth, LoD)->blockValue;
                    // face.width = voxelWidth - (x % voxelWidth);

                    // for (int dx = x; dx < x + face.width; dx++){
                    //     if (isAccountedFor(accountedVoxels, dx, y, z) || !isFacingAirblock(_chunk, dx, y, z, reverseConstant, 2, LoD)){
                    //         face.width = dx - x;
                    //         break;
                    //     }
                    // }

                    for (int dx = x; dx < 32; dx++){
                        if (!isAccountedFor(accountedVoxels, dx, y, z) &&
                             isFacingAirblock(_chunk, dx, y, z, reverseConstant, 2, LoD) && 
                            !isAirBlock(_chunk, dx, y, z, LoD) && 
                             _chunk->octree->getNodeFromPosition(dx, y, z, voxelWidth, LoD)->blockValue == face.texture){
                                
                            face.width += 1;
                        } else {
                            break;
                        }
                    }

                    bool shouldBreak = false;
                    for (int dz = z + 1; dz < 32; dz++){
                        for (int dx = x; dx < x + face.width; dx++){
                            if (!isAccountedFor(accountedVoxels, dx, y, dz) &&
                                 isFacingAirblock(_chunk, dx, y, dz, reverseConstant, 2, LoD) && 
                                !isAirBlock(_chunk, dx, y, dz, LoD) && 
                                 _chunk->octree->getNodeFromPosition(dx, y, dz, voxelWidth, LoD)->blockValue == face.texture){
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
                        makeVoxelAccountedFor(accountedVoxels, dx, y, dz);
                    }
                }

                int vertexCount = vertices.size();
                float u, v;

                if (currentFace == TOP_FACE){
                    getTextureCoordinates(face.texture, u, v, TOP_FACE, LoD);

                    vertices.push_back(Vertex(face.x,              face.y + jumpLength, face.z,               u,              v)); 
                    vertices.push_back(Vertex(face.x + face.width, face.y + jumpLength, face.z + face.height, u + face.width, v + face.height));
                    vertices.push_back(Vertex(face.x + face.width, face.y + jumpLength, face.z,               u + face.width, v));
                    vertices.push_back(Vertex(face.x,              face.y + jumpLength, face.z + face.height, u,              v + face.height));

                    indices.push_back(vertexCount + 3);
                    indices.push_back(vertexCount + 1);
                    indices.push_back(vertexCount + 2);
                    indices.push_back(vertexCount + 3);
                    indices.push_back(vertexCount + 2);
                    indices.push_back(vertexCount + 0);
                } else {
                    getTextureCoordinates(face.texture, u, v, BOTTOM_FACE, LoD);

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

        // FRONT & BACK FACE
        if (currentFace == FRONT_FACE || currentFace == BACK_FACE){
            if (!isAccountedFor(accountedVoxels, x, y, z) &&
                 isFacingAirblock(_chunk, x, y, z, reverseConstant, 1, LoD) && 
                !isAirBlock(_chunk, x, y, z, LoD)){

                voxelFace face(x, y, z);
                face.texture = _chunk->octree->getNodeFromPosition(x, y, z, voxelWidth, LoD)->blockValue;
                //face.width = voxelWidth - (z % voxelWidth);

                // for (int dz = z; dz < z + face.width; dz++){
                //     if (isAccountedFor(accountedVoxels, x, y, dz) || !isFacingAirblock(_chunk, x, y, dz, reverseConstant, 1, LoD)){
                //         face.width = dz - z;
                //         break;
                //     }
                // }

                for (int dz = z; dz < 32; dz++){
                    if (!isAccountedFor(accountedVoxels, x, y, dz) &&
                         isFacingAirblock(_chunk, x, y, dz, reverseConstant, 1, LoD) && 
                        !isAirBlock(_chunk, x, y, dz, LoD) && 
                         _chunk->octree->getNodeFromPosition(x, y, dz, voxelWidth, LoD)->blockValue == face.texture){

                        face.width += 1;
                        //dz += voxelWidth - 1;
                    } else{
                        break;
                    }
                }

                bool shouldBreak = false;
                for (int dy = y + 1; dy < 32; dy++){
                    for (int dz = z; dz < z + face.width; dz++){
                        if (!isAccountedFor(accountedVoxels, x, dy, dz) &&
                             isFacingAirblock(_chunk, x, dy, dz, reverseConstant, 1, LoD) && 
                            !isAirBlock(_chunk, x, dy, dz, LoD) && 
                             _chunk->octree->getNodeFromPosition(x, dy, dz, voxelWidth, LoD)->blockValue == face.texture){
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
                        makeVoxelAccountedFor(accountedVoxels, x, dy, dz);
                    }
                }

                int vertexCount = vertices.size();
                float u, v;

                if (currentFace == BACK_FACE){
                    getTextureCoordinates(face.texture, u, v, FRONT_FACE, LoD);

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
                    getTextureCoordinates(face.texture, u, v, BACK_FACE, LoD);

                    vertices.push_back(Vertex(face.x + jumpLength, face.y + face.height, face.z + face.width, u,              v + face.height));
                    vertices.push_back(Vertex(face.x + jumpLength, face.y,               face.z,              u + face.width, v));
                    vertices.push_back(Vertex(face.x + jumpLength, face.y + face.height, face.z,              u + face.width, v + face.height));
                    vertices.push_back(Vertex(face.x + jumpLength, face.y,               face.z + face.width, u,              v));

                    indices.push_back(vertexCount + 3);
                    indices.push_back(vertexCount + 1);
                    indices.push_back(vertexCount + 2);
                    indices.push_back(vertexCount + 3);
                    indices.push_back(vertexCount + 2);
                    indices.push_back(vertexCount + 0);
                }
            }
        }

        // RIGHT & LEFT FACE
        if (currentFace == LEFT_FACE || currentFace == RIGHT_FACE){
            if (!isAccountedFor(accountedVoxels, x, y, z) &&
                 isFacingAirblock(_chunk, x, y, z, reverseConstant, 3, LoD) && 
                !isAirBlock(_chunk, x, y, z, LoD)){

            voxelFace face(x, y, z);
            face.texture = _chunk->octree->getNodeFromPosition(x, y, z, voxelWidth, LoD)->blockValue;
            //face.width = voxelWidth - (x % voxelWidth);

            // for (int dx = x; dx < x + face.width; dx++){
            //     if (isAccountedFor(accountedVoxels, dx, y, z) || !isFacingAirblock(_chunk, dx, y, z, reverseConstant, 3, LoD)){
            //         face.width = dx - x;
            //         break;
            //     }
            // }

            for (int dx = x; dx < 32; dx++){
                if (!isAccountedFor(accountedVoxels, dx, y, z) &&
                     isFacingAirblock(_chunk, dx, y, z, reverseConstant, 3, LoD) && 
                    !isAirBlock(_chunk, dx, y, z, LoD) && 
                     _chunk->octree->getNodeFromPosition(dx, y, z, voxelWidth, LoD)->blockValue == face.texture){

                    face.width += 1;
                } else{
                    break;
                }
            }

            bool shouldBreak = false;
            for (int dy = y + 1; dy < 32; dy++){
                for (int dx = x; dx < x + face.width; dx++){
                    if (!isAccountedFor(accountedVoxels, dx, dy, z) &&
                         isFacingAirblock(_chunk, dx, dy, z, reverseConstant, 3, LoD) && 
                        !isAirBlock(_chunk, dx, dy, z, LoD) && 
                         _chunk->octree->getNodeFromPosition(dx, dy, z, voxelWidth, LoD)->blockValue == face.texture){
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

            for (int dy = y; dy < y + face.height; dy++){
                for (int dx = x; dx < x + face.width; dx++){
                    makeVoxelAccountedFor(accountedVoxels, dx, dy, z);
                }
            }

            int vertexCount = vertices.size();
            float u, v;
            
            if (currentFace == LEFT_FACE){
                getTextureCoordinates(face.texture, u, v, LEFT_FACE, LoD);

                vertices.push_back(Vertex(face.x,              face.y,               face.z + jumpLength, u,              v)); 
                vertices.push_back(Vertex(face.x + face.width, face.y,               face.z + jumpLength, u + face.width, v));
                vertices.push_back(Vertex(face.x + face.width, face.y + face.height, face.z + jumpLength, u + face.width, v + face.height));
                vertices.push_back(Vertex(face.x,              face.y + face.height, face.z + jumpLength, u,              v + face.height));

                indices.push_back(vertexCount + 0);
                indices.push_back(vertexCount + 1);
                indices.push_back(vertexCount + 2);
                indices.push_back(vertexCount + 0);
                indices.push_back(vertexCount + 2);
                indices.push_back(vertexCount + 3);
            } else {
                getTextureCoordinates(face.texture, u, v, RIGHT_FACE, LoD);

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

