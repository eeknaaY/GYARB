#include "chunk.hpp"
#include "renderer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>
#include <numeric>


void Renderer::renderVisibleChunks(const Shader &shader, const Camera& camera){


    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& corner : camera.getFrustumCornersWorldSpace())
    {
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
            drawChunkVector(chunkPositionX - renderSize,          chunkPositionZ - renderSize + offSet, shader, frustumExtremeValues);
            drawChunkVector(chunkPositionX - renderSize + offSet, chunkPositionZ + renderSize,          shader, frustumExtremeValues);
            drawChunkVector(chunkPositionX + renderSize - offSet, chunkPositionZ - renderSize,          shader, frustumExtremeValues);
            drawChunkVector(chunkPositionX + renderSize,          chunkPositionZ + renderSize - offSet, shader, frustumExtremeValues);
        }

        for (int offSet = 0; offSet <= renderSize; offSet += 1){
            drawChunkVector(chunkPositionX - renderSize + offSet, chunkPositionZ - renderSize,          shader, frustumExtremeValues);
            drawChunkVector(chunkPositionX - renderSize,          chunkPositionZ + renderSize - offSet, shader, frustumExtremeValues);
            drawChunkVector(chunkPositionX + renderSize,          chunkPositionZ - renderSize + offSet, shader, frustumExtremeValues);
            drawChunkVector(chunkPositionX + renderSize - offSet, chunkPositionZ + renderSize         , shader, frustumExtremeValues);
        }
    }

    drawChunkVector(chunkPositionX, chunkPositionZ, shader, frustumExtremeValues);
}

void Renderer::drawChunkVector(int x, int z, Shader shader, const std::vector<float>& frustumExtremeValues){
    // If a chunk is outside of the frustum, dont render.
    // int errorMargain = 1;
    // if ((x + errorMargain) * Chunk::CHUNK_SIZE < frustumExtremeValues[0]) return;
    // if ((x - errorMargain) * Chunk::CHUNK_SIZE > frustumExtremeValues[1]) return;
    // if ((z + errorMargain) * Chunk::CHUNK_SIZE < frustumExtremeValues[4]) return;
    // if ((z - errorMargain) * Chunk::CHUNK_SIZE > frustumExtremeValues[5]) return;

    for (Chunk* chunk : chunkManager->getChunkVector(x, z)){
        // if ((chunk->yCoordinate + errorMargain) * Chunk::CHUNK_SIZE < frustumExtremeValues[2]) return;
        // if ((chunk->yCoordinate - errorMargain) * Chunk::CHUNK_SIZE > frustumExtremeValues[3]) return;


        chunk->draw(shader);
    }
}

void Renderer::updataChunkData(){
    while (chunkManager->finishedMeshesth1.size() != 0){
        Chunk* chunk = chunkManager->finishedMeshesth1[0];
        if (chunk->mesh.solid_vertices.size() != 0){
            chunk->updateMesh();
        }
        chunkManager->finishedMeshesth1.erase(chunkManager->finishedMeshesth1.begin());
    }
    
    while (chunkManager->finishedMeshesth2.size() != 0){
        Chunk* chunk = chunkManager->finishedMeshesth2[0];
        if (chunk->mesh.solid_vertices.size() != 0){
            chunk->updateMesh();
        }
        chunkManager->finishedMeshesth2.erase(chunkManager->finishedMeshesth2.begin());
    }
}