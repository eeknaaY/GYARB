#include "chunk.hpp"
#include "renderer.hpp"

void Renderer::renderVisibleChunks(const Shader &shader, const Camera& camera){
    for (auto const& [key, val] : this->chunkManager->chunkMap){
        if (key.first > camera.currentChunk_x + 12 || key.first < camera.currentChunk_x - 12 || key.second > camera.currentChunk_z + 12 || key.second < camera.currentChunk_z - 12){
            //chunkManager->removeChunk(key.first, key.second);
            continue;
        }

        for (Chunk* chunk : val){

            chunk->draw(shader);
        }
    }
}

void Renderer::updataChunkData(){
    while (chunkManager->chunksToRemoveth1.size() != 0){
        std::pair<int, int> pair = chunkManager->chunksToRemoveth1[0];
        chunkManager->removeChunk(pair.first, pair.second);
        chunkManager->chunksToRemoveth1.erase(chunkManager->chunksToRemoveth1.begin());
    }

    while (chunkManager->chunksToRemoveth2.size() != 0){
        std::pair<int, int> pair = chunkManager->chunksToRemoveth2[0];
        chunkManager->removeChunk(pair.first, pair.second);
        chunkManager->chunksToRemoveth2.erase(chunkManager->chunksToRemoveth2.begin());
    }

    while (chunkManager->finishedMeshesth1.size() != 0){
        Chunk* chunk = chunkManager->finishedMeshesth1[0];
        if (chunk->mesh.vertices.size() != 0){
            chunk->updateMesh();
        }
        chunkManager->finishedMeshesth1.erase(chunkManager->finishedMeshesth1.begin());
    }
    
    while (chunkManager->finishedMeshesth2.size() != 0){
        Chunk* chunk = chunkManager->finishedMeshesth2[0];
        if (chunk->mesh.vertices.size() != 0){
            chunk->updateMesh();
        }
        chunkManager->finishedMeshesth2.erase(chunkManager->finishedMeshesth2.begin());
    }
}