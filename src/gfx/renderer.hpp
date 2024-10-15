#pragma once
#include "chunkmanager.hpp"

class Renderer{
    public: 
        ChunkManager* chunkManager;
        void renderVisibleChunks(const Shader &shader, const Camera& camera);
        void drawChunkVector(int x, int z, Shader shader, bool cameraInChunk);
        void updataChunkData();
};