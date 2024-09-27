#include "chunk.hpp"
#include "renderer.hpp"

void Renderer::renderVisibleChunks(const Shader &shader, const Camera& camera){
    for (auto const& [key, val] : this->chunkManager->chunkMap){
        for (Chunk* chunk : val){
            chunk->draw(shader);
        }
    }
}

void Renderer::updataChunkData(){
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