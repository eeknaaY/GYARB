#ifndef CHUNKMANAGER_HPP
#define CHUNKMANAGER_HPP

#include <map>
#include "mesh.hpp"
#include "../shaders/shaders.hpp"
#include "camera.hpp"
#include <thread>

enum voxelFaces{
    TOP_FACE,
    BOTTOM_FACE,
    LEFT_FACE,
    RIGHT_FACE,
    FRONT_FACE,
    BACK_FACE
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

#endif