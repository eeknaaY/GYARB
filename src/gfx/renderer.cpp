#include "chunk.hpp"
#include "renderer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>


void Renderer::renderVisibleChunks(const Shader &shader, const Camera& camera){
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const glm::vec3& corner : camera.getFrustumCornersWorldSpace()){
        minX = std::min(minX, corner.x);
        maxX = std::max(maxX, corner.x);
        minY = std::min(minY, corner.y);
        maxY = std::max(maxY, corner.y);
        minZ = std::min(minZ, corner.z);
        maxZ = std::max(maxZ, corner.z);
    }

    std::vector<float> frustumExtremeValues = {minX, maxX, minY, maxY, minZ, maxZ};

    // Draw them far -> near
    int chunkPositionX = camera.currentChunk_x;
    int chunkPositionZ = camera.currentChunk_z;
    int renderDistance = camera.renderDistance;
    for (int renderSize = renderDistance; renderSize > 0; renderSize--){
        for (int offSet = 0; offSet < renderSize; offSet += 1){
            drawChunkVector(chunkPositionX - renderSize,          chunkPositionZ - renderSize + offSet, camera, shader, frustumExtremeValues);
            drawChunkVector(chunkPositionX - renderSize + offSet, chunkPositionZ + renderSize,          camera, shader, frustumExtremeValues);
            drawChunkVector(chunkPositionX + renderSize - offSet, chunkPositionZ - renderSize,          camera, shader, frustumExtremeValues);
            drawChunkVector(chunkPositionX + renderSize,          chunkPositionZ + renderSize - offSet, camera, shader, frustumExtremeValues);
        }

        for (int offSet = 1; offSet <= renderSize; offSet += 1){
            drawChunkVector(chunkPositionX - renderSize + offSet, chunkPositionZ - renderSize,          camera, shader, frustumExtremeValues);
            drawChunkVector(chunkPositionX - renderSize,          chunkPositionZ + renderSize - offSet, camera, shader, frustumExtremeValues);
            drawChunkVector(chunkPositionX + renderSize,          chunkPositionZ - renderSize + offSet, camera, shader, frustumExtremeValues);
            drawChunkVector(chunkPositionX + renderSize - offSet, chunkPositionZ + renderSize         , camera, shader, frustumExtremeValues);
        }
    }

    drawChunkVector(chunkPositionX, chunkPositionZ, camera, shader, frustumExtremeValues);
}

void Renderer::drawChunkVector(int x, int z, const Camera& camera, const Shader& shader){
    bool playerInChunkVector = camera.currentChunk_x == x && camera.currentChunk_z == z;

    std::vector<Chunk*> chunkVector = chunkManager->getChunkVector(x, z);
    for (Chunk* chunk : chunkVector){
        if (playerInChunkVector){
            if(camera.hasChangedBlock())chunk->sortTransparentFaces(camera);
        } else {
            if(camera.hasChangedChunk()) chunk->sortTransparentFaces(camera);
        }

        if(chunk->yCoordinate == camera.currentChunk_y) break;

        chunk->draw(shader);
    }

    std::reverse(chunkVector.begin(), chunkVector.end());
    for (Chunk* chunk : chunkVector){
        if (playerInChunkVector){
            if(camera.hasChangedBlock())chunk->sortTransparentFaces(camera);
        } else {
            if(camera.hasChangedChunk()) chunk->sortTransparentFaces(camera);
        }

        if(chunk->yCoordinate == camera.currentChunk_y - 1) break;

        chunk->draw(shader);
    }
}

void Renderer::drawChunkVector(int x, int z, const Camera& camera, const Shader& shader, const std::vector<float>& frustumExtremeValues){
    // If a chunk is outside of the frustum, dont render.
    int errorMargain = 1;
    if ((x + errorMargain) * Chunk::CHUNK_SIZE < frustumExtremeValues[0]) return;
    if ((x - errorMargain) * Chunk::CHUNK_SIZE > frustumExtremeValues[1]) return;
    if ((z + errorMargain) * Chunk::CHUNK_SIZE < frustumExtremeValues[4]) return;
    if ((z - errorMargain) * Chunk::CHUNK_SIZE > frustumExtremeValues[5]) return;

    bool playerInChunkVector = camera.currentChunk_x == x && camera.currentChunk_z == z;

    std::vector<Chunk*> chunkVector = chunkManager->getChunkVector(x, z);
    for (Chunk* chunk : chunkVector){
        if(chunk->yCoordinate == camera.currentChunk_y) break;
        
        if (playerInChunkVector){
            if(camera.hasChangedBlock()) chunk->sortTransparentFaces(camera);
        } else {
            if(camera.hasChangedChunk()) chunk->sortTransparentFaces(camera);
        }

        if ((chunk->yCoordinate + errorMargain) * Chunk::CHUNK_SIZE < frustumExtremeValues[2]) continue;
        if ((chunk->yCoordinate - errorMargain) * Chunk::CHUNK_SIZE > frustumExtremeValues[3]) continue;


        chunk->draw(shader);
    }

    std::reverse(chunkVector.begin(), chunkVector.end());
    for (Chunk* chunk : chunkVector){
        if(chunk->yCoordinate == camera.currentChunk_y - 1) break;

        if (playerInChunkVector){
            if(camera.hasChangedBlock()) chunk->sortTransparentFaces(camera);
        } else {
            if(camera.hasChangedChunk()) chunk->sortTransparentFaces(camera);
        }

        if ((chunk->yCoordinate + errorMargain) * Chunk::CHUNK_SIZE < frustumExtremeValues[2]) continue;
        if ((chunk->yCoordinate - errorMargain) * Chunk::CHUNK_SIZE > frustumExtremeValues[3]) continue;


        chunk->draw(shader);
    }
}

void Renderer::updataChunkData(){
    while (chunkManager->finishedMeshesth1.size() != 0){
        Chunk* chunk = chunkManager->finishedMeshesth1[0];
        if (chunk->mesh.opaqueVertices.size() != 0){
            chunk->updateMesh();
        }
        chunkManager->finishedMeshesth1.erase(chunkManager->finishedMeshesth1.begin());
    }
    
    while (chunkManager->finishedMeshesth2.size() != 0){
        Chunk* chunk = chunkManager->finishedMeshesth2[0];
        if (chunk->mesh.opaqueVertices.size() != 0){
            chunk->updateMesh();
        }
        chunkManager->finishedMeshesth2.erase(chunkManager->finishedMeshesth2.begin());
    }
}