#include "chunk.hpp"
#include <vector>
#include <stdexcept>      // std::out_of_range
#include "chunkmanager.hpp"
#include "biomes.hpp"

////////////////////////////////////////////////////  CHUNK HANDLING  ///////////////////////////////////////////////////////////////////////////////

void ChunkManager::appendChunk(int x, int z, int LoD){
    if (getChunkVector(x, z).size() == 0){
        chunkMap[std::make_pair(x, z)] = std::vector<Chunk*>{new Chunk(x, 0, z, LoD, noise)};
    } else {
        chunkMap[std::make_pair(x, z)].push_back(new Chunk(x, 0, z, LoD, noise));
    }

    Biome* currentBiome = BiomeHandler::getBiome(Biomes::Forest);
    for (int _x = 0; _x < 32; _x++){
        for (int _z = 0; _z < 32; _z++){
            float noiseVal = currentBiome->noise.GetNoise((float)(_x + 32 * x), (float)(_z + 32 * z));
            // Change height here and in octree
            int maxHeight = currentBiome->averageHeightValue + Chunk::CHUNK_SIZE + (int)((currentBiome->heightOffsetValue) * noiseVal);
            if (maxHeight - getChunkVector(x, z).size() * Chunk::CHUNK_SIZE > Chunk::CHUNK_SIZE){
                appendChunk(new Chunk(x, getChunkVector(x, z).size(), z, LoD, noise));
                return;
            }
        }
    }
}

void ChunkManager::appendChunk(Chunk* ptr){
    if (getChunkVector(ptr->xCoordinate, ptr->zCoordinate).size() == 0){
        chunkMap[std::make_pair(ptr->xCoordinate, ptr->zCoordinate)] = std::vector<Chunk*>{ptr};
    } else {
        chunkMap[std::make_pair(ptr->xCoordinate, ptr->zCoordinate)].push_back(ptr);
    }

    Biome* currentBiome = BiomeHandler::getBiome(Biomes::Mountain);
    for (int x = 0; x < 32; x++){
        for (int z = 0; z < 32; z++){
            float noiseVal = currentBiome->getNoiseValue(32 * ptr->xCoordinate + x, 32 * ptr->zCoordinate + z);
            // Change height here and in octree
            int maxHeight = currentBiome->averageHeightValue + Chunk::CHUNK_SIZE + (int)((currentBiome->heightOffsetValue) * noiseVal);

            if (maxHeight - (ptr->yCoordinate + 1) * Chunk::CHUNK_SIZE > Chunk::CHUNK_SIZE){
                appendChunk(new Chunk(ptr->xCoordinate, ptr->yCoordinate + 1, ptr->zCoordinate, ptr->currentLoD, noise));
                return;
            }
        }
    }
}

void ChunkManager::removeChunk(int x, int z){
    for (Chunk* chunkptr : getChunkVector(x, z)){
        delete chunkptr;
        chunkptr = 0;
    }

    chunkMap.erase(std::make_pair(x, z));
}

Chunk* ChunkManager::getChunk(int x, int y, int z){
    if (x > 25 || z > 25 || x < -25 || z < -25) return nullptr;
    try{
        if (y > chunkMap.at(std::make_pair(x, z)).size() - 1) return nullptr;
        return chunkMap.at(std::make_pair(x, z))[y];
    }
    catch (const std::out_of_range& oor){
        return nullptr;
    }
}

std::vector<Chunk*> ChunkManager::getChunkVector(int x, int z){
    try{
        return chunkMap.at(std::make_pair(x, z));
    }
    catch (const std::out_of_range& oor){
        return std::vector<Chunk*>{};
    }
}

void ChunkManager::updateChunkMesh(Chunk* _chunk, Camera gameCamera){
    if (!_chunk) return;
    Mesh updatedMesh = buildMesh(_chunk, gameCamera);
    _chunk->mesh.solid_vertices = updatedMesh.solid_vertices;
    _chunk->mesh.solid_indices = updatedMesh.solid_indices;
    _chunk->mesh.transparent_vertices = updatedMesh.transparent_vertices;
    _chunk->mesh.transparent_indices = updatedMesh.transparent_indices;
    _chunk->updateMesh();
}

void ChunkManager::updateChunkMesh_MT(Chunk* _chunk, Camera gameCamera){
    Mesh updatedMesh = buildMesh(_chunk, gameCamera);
    _chunk->mesh.solid_vertices = updatedMesh.solid_vertices;
    _chunk->mesh.solid_indices = updatedMesh.solid_indices;
    _chunk->mesh.transparent_vertices = updatedMesh.transparent_vertices;
    _chunk->mesh.transparent_indices = updatedMesh.transparent_indices;
}

void ChunkManager::startMeshingThreads(Camera* gameCamera){
    // if (meshingThread1.joinable()) meshingThread1.join();
    // if (meshingThread2.joinable()) meshingThread2.join();

    while (chunksToRemoveth1.size() != 0){
        std::pair<int, int> pair = chunksToRemoveth1[0];
        removeChunk(pair.first, pair.second);
        chunksToRemoveth1.erase(chunksToRemoveth1.begin());
    }

    while (chunksToRemoveth2.size() != 0){
        std::pair<int, int> pair = chunksToRemoveth2[0];
        removeChunk(pair.first, pair.second);
        chunksToRemoveth2.erase(chunksToRemoveth2.begin());
    }

    // meshingThread1 = std::thread(updateTerrain, this, gameCamera, 0);
    // meshingThread2 = std::thread(updateTerrain, this, gameCamera, 1);
}

void ChunkManager::updateTerrain(Camera* gameCamera, int threadMultiplier){
    int c_dXPos = gameCamera->currentChunk_x - gameCamera->oldChunk_x;
    int c_dZPos = gameCamera->currentChunk_z - gameCamera->oldChunk_z;
    int xPos = gameCamera->currentChunk_x;
    int zPos = gameCamera->currentChunk_z;

    std::vector<Chunk*>* finishedMeshes;
    std::vector<std::pair<int, int>>* chunksToRemove;

    int renderDistance = gameCamera->renderDistance;

    if (threadMultiplier == 0){
        finishedMeshes = &this->finishedMeshesth1;
        chunksToRemove = &this->chunksToRemoveth1;
    } else {
        finishedMeshes = &this->finishedMeshesth2;
        chunksToRemove = &this->chunksToRemoveth2;
    }

    if (c_dXPos != 0){
        for (int dz = zPos - renderDistance * (1 - threadMultiplier); dz <= zPos + renderDistance * threadMultiplier + (threadMultiplier - 1); dz++){
            for (int dx = xPos - renderDistance; dx <= xPos + renderDistance; dx++){
                Chunk* chunk = getChunk(dx, 0, dz);

                if (!chunk){
                    int LoD = 5;
                    if (abs(dx - xPos) > 10 || abs(dz - zPos) > 10) LoD = 4;
                    Chunk* _chunk = new Chunk(dx, 0, dz, LoD, noise);
                    appendChunk(_chunk);
                    continue;
                }

                if (dx == xPos - c_dXPos * renderDistance){
                    chunksToRemove->push_back(std::make_pair(dx, dz));
                    continue;
                }

                if (dx == xPos + c_dXPos * 4 || dx == xPos + c_dXPos * (renderDistance - 1)){
                    for (Chunk* _chunkptr : getChunkVector(dx, dz)){
                        updateChunkMesh_MT(_chunkptr, *gameCamera);
                        finishedMeshes->push_back(_chunkptr);
                    }
                }
                
                if (dx == xPos + c_dXPos * 4 || dx == xPos + c_dXPos * (renderDistance - 1)){
                    for (Chunk* _chunkptr : getChunkVector(dx, dz)){
                        if (_chunkptr->mesh.solid_vertices.size() == 0){
                            int LoD = 5;
                            if (abs(dx - xPos) > 8 || abs(dz - zPos) > 8) LoD = 4;
                            _chunkptr->currentLoD = LoD;
                            updateChunkMesh_MT(_chunkptr, *gameCamera);
                            finishedMeshes->push_back(_chunkptr);
                        }
                    }
                }
            }
        }
    }

    if (c_dZPos != 0){
        for (int dx = xPos - renderDistance * (1 - threadMultiplier); dx <= xPos + renderDistance * threadMultiplier + (threadMultiplier - 1); dx++){
            for (int dz = zPos - renderDistance; dz <= zPos + renderDistance; dz++){
                Chunk* chunk = getChunk(dx, 0, dz);

                if (!chunk){
                    int LoD = 5;
                    if (abs(dx - xPos) > 10 || abs(dz - zPos) > 10) LoD = 4;
                    Chunk* _chunk = new Chunk(dx, 0, dz, LoD, noise);
                    appendChunk(_chunk);
                    continue;
                }

                if (dz == zPos - c_dZPos * renderDistance){
                    chunksToRemove->push_back(std::make_pair(dx, dz));
                    continue;
                }

                if (dz == zPos + c_dZPos * 4 || dz == zPos + c_dZPos * (renderDistance - 1)){
                    for (Chunk* _chunkptr : getChunkVector(dx, dz)){
                        int LoD = 5;
                        if (abs(dx - xPos) > 8 || abs(dz - zPos) > 8) LoD = 4;
                        _chunkptr->currentLoD = LoD;
                        updateChunkMesh_MT(_chunkptr, *gameCamera);
                        finishedMeshes->push_back(_chunkptr);
                    }
                }
            }
        }
    }
}


////////////////////////////////////////////////////  MESHING  ///////////////////////////////////////////////////////////////////////////////

void ChunkManager::makeVoxelAccountedFor(bool accountedVoxels[], int x, int y, int z){
    accountedVoxels[x + 32 * z + 32 * 32 * y] = true;
}

int ChunkManager::getBlockValueFromPosition(Chunk* chunk, int x, int y, int z, int LoD){
    bool chunkChanged = false;
    Chunk* _chunk;
    if (y >= 32){
        chunkChanged = true;
        _chunk = getChunk(chunk->xCoordinate, chunk->yCoordinate + 1, chunk->zCoordinate);   
        y-=32;
    }
    if (y <= -1){
        chunkChanged = true;
        _chunk = getChunk(chunk->xCoordinate, chunk->yCoordinate - 1, chunk->zCoordinate); 
        y+=32;
    } 
    if (x >= 32){
        chunkChanged = true;
        _chunk = getChunk(chunk->xCoordinate + 1, chunk->yCoordinate, chunk->zCoordinate);   
        x-=32;
    }
    if (x <= -1){
        chunkChanged = true;
        _chunk = getChunk(chunk->xCoordinate - 1, chunk->yCoordinate, chunk->zCoordinate); 
        x+=32;
    } 
    if (z >= 32){
        chunkChanged = true;
        _chunk = getChunk(chunk->xCoordinate, chunk->yCoordinate, chunk->zCoordinate + 1); 
        z-=32;
    } 
    if (z <= -1){
        chunkChanged = true;
        _chunk = getChunk(chunk->xCoordinate, chunk->yCoordinate, chunk->zCoordinate - 1); 
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

bool ChunkManager::isAirBlock(int voxelVal[], int x, int y, int z, int LoD){
    return getVoxelValue(voxelVal, x, y, z) == 0;
}

bool ChunkManager::isFacingAirblock(int voxelVal[], int x, int y, int z, int reverseConstant, int constantPos, int LoD){
    int currentBlockValue = getVoxelValue(voxelVal, x, y, z);
    
    switch (constantPos)
    {
    case 1:
        x += reverseConstant;
        break;
    case 2:
        y += reverseConstant;
        break;
    case 3:
        z += reverseConstant;
        break;

    default:
        break;
    }

    int blockValue = getVoxelValue(voxelVal, x, y, z);

    if (currentBlockValue == 17){
        if (reverseConstant == 1 && constantPos == 2) return 1;
        return blockValue == 0;
    }

    // Facing water or air
    return (blockValue == 0 || blockValue == (int)Block::Water || blockValue == (int)Block::Glass);
}

int ChunkManager::getVoxelValue(int voxelValues[], int x, int y, int z){
    return voxelValues[(x + 1) + (z + 1) * 34 + (y + 1) * 34 * 34];
}

void ChunkManager::buildVoxelValueArray(int voxelValues[], Chunk* chunk, int LoD){
    int jumpLength = 1 << (5 - LoD);
    for (int x = -1; x < 33; x++){
        for (int y = -1; y < 33; y++){
            for (int z = -1; z < 33; z++){
                voxelValues[(x + 1) + (z + 1) * 34 + (y + 1) * 34 * 34] = getBlockValueFromPosition(chunk, x, y, z, LoD);
            }
        }
    }
}

void ChunkManager::getTextureCoordinates(int textureValue, float &u, float &v, voxelFaces face, int LoD){ // Replace sides of a grass block to semi dirt + semi grass
    Block blockType = (Block)textureValue;
    switch (blockType)
    {
    case Block::Grass:
        if (face == BOTTOM_FACE) blockType = Block::Dirt;
        if (face != TOP_FACE && LoD == 5) blockType = Block::Grassy_Dirt;
        break;
    
    case Block::Grassy_Dirt:
        if (face == TOP_FACE) blockType = Block::Grass;
        if (face == BOTTOM_FACE) blockType = Block::Dirt;
        
    default:
        break;
    }

    const float textureBlockSize = 0.0625f;

    // Since air is the value 0, we need to remove one value so textures begin at 0.
    int _textureValue = (int)blockType - 1;


    u = textureBlockSize * (_textureValue % 16);
    v = textureBlockSize * (_textureValue - _textureValue % 16)/16; 
}

#include <chrono>

Mesh ChunkManager::buildMesh(Chunk* _chunk, Camera gameCamera){
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<Vertex> solid_vertices;
    std::vector<unsigned int> solid_indices;
    std::vector<Vertex> transparent_vertices;
    std::vector<unsigned int> transparent_indices;
    short voxelWidth;

    int LoD = _chunk->currentLoD;
    int jumpLength = 1 << (5 - LoD);

    int chunk_dx = _chunk->xCoordinate - gameCamera.currentChunk_x;
    int chunk_dz = _chunk->zCoordinate - gameCamera.currentChunk_z;
    int chunk_dy = _chunk->yCoordinate - gameCamera.currentChunk_y;

    int voxelValues[34 * 34 * 34];
    buildVoxelValueArray(voxelValues, _chunk, LoD);

    for (int currentFace = TOP_FACE; currentFace <= RIGHT_FACE; currentFace++){
        if (chunk_dx > 5 && currentFace == FRONT_FACE) continue;
        if (chunk_dx < -5 && currentFace == BACK_FACE) continue;
        if (chunk_dz < -5 && currentFace == RIGHT_FACE) continue;
        if (chunk_dz > 5 && currentFace == LEFT_FACE) continue;
        if (chunk_dy > 5 && currentFace == TOP_FACE) continue;
        //if (chunk_dy < 5 && currentFace == BOTTOM_FACE) continue;

        bool accountedVoxels[32 * 32 * 32] = {false};

        int reverseConstant = jumpLength;
        if (currentFace == BOTTOM_FACE || currentFace == BACK_FACE || currentFace == RIGHT_FACE) reverseConstant = -1;

        for (int y = 0; y < 32; y += jumpLength){
        for (int z = 0; z < 32; z += jumpLength){
        for (int x = 0; x < 32; x += jumpLength){
            if (isAccountedFor(accountedVoxels, x, y, z)) continue;
            if (isAirBlock(voxelValues, x, y, z, LoD)) continue;

            // TOP & BOTTOM FACE
            if (currentFace == TOP_FACE || currentFace == BOTTOM_FACE){
                if (isFacingAirblock(voxelValues, x, y, z, reverseConstant, 2, LoD)){

                    voxelFace face(x, y, z);
                    face.texture = getVoxelValue(voxelValues, x, y, z);

                    for (int dx = x; dx < 32; dx++){
                        if (!isAccountedFor(accountedVoxels, dx, y, z) &&
                            !isAirBlock(voxelValues, dx, y, z, LoD) && 
                             isFacingAirblock(voxelValues, dx, y, z, reverseConstant, 2, LoD) && 
                             getVoxelValue(voxelValues, dx, y, z) == face.texture){
                                
                            face.width += 1;
                        } else {
                            break;
                        }
                    }

                    bool shouldBreak = false;
                    for (int dz = z + 1; dz < 32; dz++){
                        for (int dx = x; dx < x + face.width; dx++){
                            if (!isAccountedFor(accountedVoxels, dx, y, dz) &&
                                !isAirBlock(voxelValues, dx, y, dz, LoD) && 
                                 isFacingAirblock(voxelValues, dx, y, dz, reverseConstant, 2, LoD) && 
                                 getVoxelValue(voxelValues, dx, y, dz) == face.texture){
                                    //dx += voxelWidth - 1;
                            } else {
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

                std::vector<Vertex>* vertices = &solid_vertices;
                std::vector<unsigned int>* indices = &solid_indices;

                if (face.texture == (int)Block::Water || face.texture == (int)Block::Glass){ 
                    vertices = &transparent_vertices;
                    indices = &transparent_indices;
                }

                int vertexCount = vertices->size();
                float u, v;

                if (currentFace == TOP_FACE){
                    getTextureCoordinates(face.texture, u, v, TOP_FACE, LoD);

                    vertices->push_back(Vertex(face.x,              face.y + jumpLength, face.z,               u,              v,               TOP_FACE, face.texture)); 
                    vertices->push_back(Vertex(face.x + face.width, face.y + jumpLength, face.z + face.height, u + face.width, v + face.height, TOP_FACE, face.texture));
                    vertices->push_back(Vertex(face.x + face.width, face.y + jumpLength, face.z,               u + face.width, v,               TOP_FACE, face.texture));
                    vertices->push_back(Vertex(face.x,              face.y + jumpLength, face.z + face.height, u,              v + face.height, TOP_FACE, face.texture));

                    indices->push_back(vertexCount + 3);
                    indices->push_back(vertexCount + 1);
                    indices->push_back(vertexCount + 2);
                    indices->push_back(vertexCount + 3);
                    indices->push_back(vertexCount + 2);
                    indices->push_back(vertexCount + 0);
                } else {
                    getTextureCoordinates(face.texture, u, v, BOTTOM_FACE, LoD);

                    vertices->push_back(Vertex(face.x,              face.y, face.z,               u,              v,               BOTTOM_FACE, face.texture)); 
                    vertices->push_back(Vertex(face.x + face.width, face.y, face.z,               u + face.width, v,               BOTTOM_FACE, face.texture));
                    vertices->push_back(Vertex(face.x + face.width, face.y, face.z + face.height, u + face.width, v + face.height, BOTTOM_FACE, face.texture));
                    vertices->push_back(Vertex(face.x,              face.y, face.z + face.height, u,              v + face.height, BOTTOM_FACE, face.texture));

                    indices->push_back(vertexCount + 2);
                    indices->push_back(vertexCount + 3);
                    indices->push_back(vertexCount + 0);
                    indices->push_back(vertexCount + 2);
                    indices->push_back(vertexCount + 0);
                    indices->push_back(vertexCount + 1);
                }
            }
        }

        // FRONT & BACK FACE
        if (currentFace == FRONT_FACE || currentFace == BACK_FACE){
            if (isFacingAirblock(voxelValues, x, y, z, reverseConstant, 1, LoD)){

                voxelFace face(x, y, z);
                face.texture = getVoxelValue(voxelValues, x, y, z);

                for (int dz = z; dz < 32; dz++){
                    if (!isAccountedFor(accountedVoxels, x, y, dz) &&
                        !isAirBlock(voxelValues, x, y, dz, LoD) && 
                         isFacingAirblock(voxelValues, x, y, dz, reverseConstant, 1, LoD) && 
                         getVoxelValue(voxelValues, x, y, dz) == face.texture){

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
                            !isAirBlock(voxelValues, x, dy, dz, LoD) && 
                             isFacingAirblock(voxelValues, x, dy, dz, reverseConstant, 1, LoD) && 
                             getVoxelValue(voxelValues, x, dy, dz) == face.texture){
                                    //dz += voxelWidth - 1;
                                    //dz += 1;
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

                std::vector<Vertex>* vertices = &solid_vertices;
                std::vector<unsigned int>* indices = &solid_indices;

                if (face.texture == (int)Block::Water || face.texture == (int)Block::Glass){ // If texture is water.
                    vertices = &transparent_vertices;
                    indices = &transparent_indices;
                }

                int vertexCount = vertices->size();
                float u, v;

                if (currentFace == BACK_FACE){
                    getTextureCoordinates(face.texture, u, v, FRONT_FACE, LoD);

                    vertices->push_back(Vertex(face.x, face.y + face.height, face.z + face.width, u + face.width, v + face.height, FRONT_FACE, face.texture));
                    vertices->push_back(Vertex(face.x, face.y + face.height, face.z,              u,              v + face.height, FRONT_FACE, face.texture));
                    vertices->push_back(Vertex(face.x, face.y,               face.z,              u,              v,               FRONT_FACE, face.texture));
                    vertices->push_back(Vertex(face.x, face.y,               face.z + face.width, u + face.width, v,               FRONT_FACE, face.texture));

                    indices->push_back(vertexCount + 2);
                    indices->push_back(vertexCount + 3);
                    indices->push_back(vertexCount + 0);
                    indices->push_back(vertexCount + 2);
                    indices->push_back(vertexCount + 0);
                    indices->push_back(vertexCount + 1);
                } else {
                    getTextureCoordinates(face.texture, u, v, BACK_FACE, LoD);

                    vertices->push_back(Vertex(face.x + jumpLength, face.y + face.height, face.z + face.width, u,              v + face.height, BACK_FACE, face.texture));
                    vertices->push_back(Vertex(face.x + jumpLength, face.y,               face.z,              u + face.width, v,               BACK_FACE, face.texture));
                    vertices->push_back(Vertex(face.x + jumpLength, face.y + face.height, face.z,              u + face.width, v + face.height, BACK_FACE, face.texture));
                    vertices->push_back(Vertex(face.x + jumpLength, face.y,               face.z + face.width, u,              v,               BACK_FACE, face.texture));

                    indices->push_back(vertexCount + 3);
                    indices->push_back(vertexCount + 1);
                    indices->push_back(vertexCount + 2);
                    indices->push_back(vertexCount + 3);
                    indices->push_back(vertexCount + 2);
                    indices->push_back(vertexCount + 0);
                }
            }
        }

        // RIGHT & LEFT FACE
        if (currentFace == LEFT_FACE || currentFace == RIGHT_FACE){
            if (isFacingAirblock(voxelValues, x, y, z, reverseConstant, 3, LoD)){

            voxelFace face(x, y, z);
            face.texture = getVoxelValue(voxelValues, x, y, z);

            for (int dx = x; dx < 32; dx++){
                if (!isAccountedFor(accountedVoxels, dx, y, z) &&
                    !isAirBlock(voxelValues, dx, y, z, LoD) && 
                     isFacingAirblock(voxelValues, dx, y, z, reverseConstant, 3, LoD) && 
                     getVoxelValue(voxelValues, dx, y, z) == face.texture){

                    face.width += 1;
                } else{
                    break;
                }
            }

            bool shouldBreak = false;
            for (int dy = y + 1; dy < 32; dy++){
                for (int dx = x; dx < x + face.width; dx++){
                    if (!isAccountedFor(accountedVoxels, dx, dy, z) &&
                        !isAirBlock(voxelValues, dx, dy, z, LoD) && 
                         isFacingAirblock(voxelValues, dx, dy, z, reverseConstant, 3, LoD) && 
                         getVoxelValue(voxelValues, dx, dy, z) == face.texture){
                            //dx += voxelWidth - 1;
                            //dx += 1;
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

            std::vector<Vertex>* vertices = &solid_vertices;
            std::vector<unsigned int>* indices = &solid_indices;

            if (face.texture == (int)Block::Water || face.texture == (int)Block::Glass){ // If texture is water.
                vertices = &transparent_vertices;
                indices = &transparent_indices;
            }

            int vertexCount = vertices->size();
            float u, v;
            
            if (currentFace == LEFT_FACE){
                getTextureCoordinates(face.texture, u, v, LEFT_FACE, LoD);

                vertices->push_back(Vertex(face.x,              face.y,               face.z + jumpLength, u,              v,               LEFT_FACE, face.texture)); 
                vertices->push_back(Vertex(face.x + face.width, face.y,               face.z + jumpLength, u + face.width, v,               LEFT_FACE, face.texture));
                vertices->push_back(Vertex(face.x + face.width, face.y + face.height, face.z + jumpLength, u + face.width, v + face.height, LEFT_FACE, face.texture));
                vertices->push_back(Vertex(face.x,              face.y + face.height, face.z + jumpLength, u,              v + face.height, LEFT_FACE, face.texture));

                indices->push_back(vertexCount + 0);
                indices->push_back(vertexCount + 1);
                indices->push_back(vertexCount + 2);
                indices->push_back(vertexCount + 0);
                indices->push_back(vertexCount + 2);
                indices->push_back(vertexCount + 3);
            } else {
                getTextureCoordinates(face.texture, u, v, RIGHT_FACE, LoD);

                vertices->push_back(Vertex(face.x,              face.y,               face.z, u,              v,               RIGHT_FACE, face.texture)); 
                vertices->push_back(Vertex(face.x + face.width, face.y + face.height, face.z, u + face.width, v + face.height, RIGHT_FACE, face.texture));
                vertices->push_back(Vertex(face.x + face.width, face.y,               face.z, u + face.width, v,               RIGHT_FACE, face.texture));
                vertices->push_back(Vertex(face.x,              face.y + face.height, face.z, u,              v + face.height, RIGHT_FACE, face.texture));

                indices->push_back(vertexCount + 0);
                indices->push_back(vertexCount + 1);
                indices->push_back(vertexCount + 2);
                indices->push_back(vertexCount + 0);
                indices->push_back(vertexCount + 3);
                indices->push_back(vertexCount + 1);
                }
            }
        }
    }}}}

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop-start);
    std::cout << "Meshing took: " << duration.count() << "ms \n";
    return Mesh(solid_vertices, solid_indices, transparent_vertices, transparent_indices);
}

////////////////////////////////////////////////////////////////////////////// Octree stuff ////////////////////////////////////////////////////

int ChunkManager::getBlockValue(float x, float y, float z){
    return getBlockValue((int)x, (int)y, (int)z);
}

int ChunkManager::getBlockValue(int x, int y, int z){
    int chunkPosX = (int)((round(x) - (int)round(x) % Chunk::CHUNK_SIZE) / Chunk::CHUNK_SIZE);
    int chunkPosY = (int)((round(y) - (int)round(y) % Chunk::CHUNK_SIZE) / Chunk::CHUNK_SIZE);
    int chunkPosZ = (int)((round(z) - (int)round(z) % Chunk::CHUNK_SIZE) / Chunk::CHUNK_SIZE);

    // We got the chunk, now make coords local.
    x = x % Chunk::CHUNK_SIZE;
    y = y % Chunk::CHUNK_SIZE;
    z = z % Chunk::CHUNK_SIZE;

    if (x < 0) {
        chunkPosX--;
        x += Chunk::CHUNK_SIZE;
    }

    if (z < 0) {
        chunkPosZ--;
        z += Chunk::CHUNK_SIZE;
    }

    return getChunk(chunkPosX, chunkPosY, chunkPosZ)->getBlockValue(x, y, z);
}


void ChunkManager::updateBlockValue(int x, int y, int z, int blockValue){
    // Coords are in global scope, find the chunk and make them local.
    int chunkPosX = (int)((round(x) - (int)round(x) % Chunk::CHUNK_SIZE) / Chunk::CHUNK_SIZE);
    int chunkPosY = (int)((round(y) - (int)round(y) % Chunk::CHUNK_SIZE) / Chunk::CHUNK_SIZE);
    int chunkPosZ = (int)((round(z) - (int)round(z) % Chunk::CHUNK_SIZE) / Chunk::CHUNK_SIZE);

    // We got the chunk, now make coords local.
    x = x % Chunk::CHUNK_SIZE;
    y = y % Chunk::CHUNK_SIZE;
    z = z % Chunk::CHUNK_SIZE;

    if (x < 0) {
        chunkPosX--;
        x += Chunk::CHUNK_SIZE;
    }

    if (z < 0) {
        chunkPosZ--;
        z += Chunk::CHUNK_SIZE;
    }

    getChunk(chunkPosX, chunkPosY, chunkPosZ)->updateBlockValue(x, y, z, blockValue);
}

void ChunkManager::updateBlockValueAndMesh(int x, int y, int z, int blockValue, Camera camera){
    updateBlockValue(x, y, z, blockValue);

    int chunkPosX = (int)((round(x) - (int)round(x) % Chunk::CHUNK_SIZE) / Chunk::CHUNK_SIZE);
    int chunkPosY = (int)((round(y) - (int)round(y) % Chunk::CHUNK_SIZE) / Chunk::CHUNK_SIZE);
    int chunkPosZ = (int)((round(z) - (int)round(z) % Chunk::CHUNK_SIZE) / Chunk::CHUNK_SIZE);
    
    x = x % Chunk::CHUNK_SIZE;
    y = y % Chunk::CHUNK_SIZE;
    z = z % Chunk::CHUNK_SIZE;

    if (x < 0) {
        chunkPosX--;
        x += Chunk::CHUNK_SIZE;
    }

    if (z < 0) {
        chunkPosZ--;
        z += Chunk::CHUNK_SIZE;
    }

    updateChunkMesh(getChunk(chunkPosX, chunkPosY, chunkPosZ), camera);

    if (x == 0){
        updateChunkMesh(getChunk(chunkPosX - 1, chunkPosY, chunkPosZ), camera);
    }

    if (x == 31){
        updateChunkMesh(getChunk(chunkPosX + 1, chunkPosY, chunkPosZ), camera);
    }

    if (y == 0){
        updateChunkMesh(getChunk(chunkPosX, chunkPosY - 1, chunkPosZ), camera);
    }

    if (y == 31){
        Chunk* _chunk = getChunk(chunkPosX, chunkPosY + 1, chunkPosZ);
        if (_chunk){
            updateChunkMesh(getChunk(chunkPosX, chunkPosY + 1, chunkPosZ), camera);
        } else {
            // Initialize new chunk
            Chunk* newChunk = new Chunk(chunkPosX, chunkPosY + 1, chunkPosZ);
            appendChunk(newChunk);
            updateBlockValue(x, y, z, blockValue);
        }
    }

    if (z == 0){
        updateChunkMesh(getChunk(chunkPosX, chunkPosY, chunkPosZ - 1), camera);
    }

    if (z == 31){
        updateChunkMesh(getChunk(chunkPosX, chunkPosY, chunkPosZ + 1), camera);
    }
}

