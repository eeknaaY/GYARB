#pragma once
#include "chunkmanager.hpp"
#include <numeric>

class Renderer{
    public:
        Renderer(ChunkManager* chunkManager){
            this->chunkManager = chunkManager;
        }

        ChunkManager* chunkManager;
        void renderVisibleChunks(const Shader &shader, const Camera& camera);
        void drawChunkVector(int x, int z, const Camera& camera, const Shader& shader);
        void drawChunkVector(int x, int z, const Camera& camera, const Shader& shader, const std::vector<float>& frustumExtremeValues);
        void updataChunkData(const Shader& shader);
};