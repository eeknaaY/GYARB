#include "chunk.hpp"
#include "renderer.hpp"


void Renderer::renderVisibleChunks(const Shader &shader, const Camera& camera){
    // Draw them far -> near
    int chunkPositionX = camera.currentChunk_x;
    int chunkPositionZ = camera.currentChunk_z;
    int renderDistance = 12;
    for (int renderSize = 12; renderSize > 0; renderSize--){
        for (int offSet = 0; offSet < renderSize; offSet += 1){
            drawChunkVector(chunkPositionX - renderSize,          chunkPositionZ - renderSize + offSet, shader);
            drawChunkVector(chunkPositionX - renderSize + offSet, chunkPositionZ + renderSize,          shader);
            drawChunkVector(chunkPositionX + renderSize - offSet, chunkPositionZ - renderSize,          shader);
            drawChunkVector(chunkPositionX + renderSize,          chunkPositionZ + renderSize - offSet, shader);
        }

        for (int offSet = 0; offSet <= renderSize; offSet += 1){
            drawChunkVector(chunkPositionX - renderSize + offSet, chunkPositionZ - renderSize,          shader);
            drawChunkVector(chunkPositionX - renderSize,          chunkPositionZ + renderSize - offSet, shader);
            drawChunkVector(chunkPositionX + renderSize,          chunkPositionZ - renderSize + offSet, shader);
            drawChunkVector(chunkPositionX + renderSize - offSet, chunkPositionZ + renderSize         , shader);
        }
    }

    drawChunkVector(chunkPositionX, chunkPositionZ, shader);
}

void Renderer::drawChunkVector(int x, int z, Shader shader){
    for (Chunk* chunk : chunkManager->getChunkVector(x, z)){
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