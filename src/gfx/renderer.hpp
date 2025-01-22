#pragma once
#include "chunkmanager.hpp"
#include <numeric>

class Renderer{
    enum class RenderOptions{
        OPAQUE,
        TRANSPARENT
    };

    public:
        Renderer(ChunkManager* chunkManager){
            this->chunkManager = chunkManager;
        }

        ChunkManager* chunkManager;

        void renderVisibleChunks(const Shader &shader, const Camera& camera);

        void drawChunkVector(int x, int z, const Camera& camera, const Shader& shader);
        void drawChunkVector(int x, int z, const Camera& camera, const Shader& shader, const std::vector<float>& frustumExtremeValues, RenderOptions renderType = RenderOptions::OPAQUE);
        void updataChunkData(const Shader& shader);

    private:
        void renderTransparentChunks(const Shader& shader, const Camera& camera, const std::vector<float>& frustumExtremeValues);
        void renderOpaqueChunks(const Shader& shader, const Camera& camera, const std::vector<float>& frustumExtremeValues);
};