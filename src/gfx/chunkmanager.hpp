#pragma once

#include <map>
#include "mesh.hpp"
#include "camera.hpp"
#include <thread>
#include "FastNoiseLite.h"
#include "chunk.hpp"

struct voxelFace{
    int x, y, z, width, height, texture;

    voxelFace(short _x, short _y, short _z){
        x = _x;
        y = _y;
        z = _z;
        width = 0;
        height = 1;
    }
};

enum voxelFaces{
    TOP_FACE,
    BOTTOM_FACE,
    FRONT_FACE,
    BACK_FACE,
    LEFT_FACE,
    RIGHT_FACE
};

class ChunkManager{
    public:
        std::vector<Chunk*> finishedMeshesth1;
        std::vector<Chunk*> finishedMeshesth2;
        std::vector<std::pair<int, int>> chunksToRemoveth1;
        std::vector<std::pair<int, int>> chunksToRemoveth2;

        std::map<std::pair<int, int>, std::vector<Chunk*>> chunkMap;

        void appendChunk(int x, int z, int LoD);
        void appendChunk(Chunk* ptr);
        void removeChunk(int x, int z);
        Chunk* getChunk(int x, int y, int z);
        std::vector<Chunk*> getChunkVector(int x, int z);
        Mesh buildMesh(Chunk* _chunk, Camera gameCamera);
        void updateChunkMesh(Chunk* _chunk, Camera gameCamera);
        void startMeshingThreads(Camera* gameCamera);
        void updateTerrain(Camera* gameCamera, int threadMultiplier);
        void setBlockValue(int x, int y, int z, int blockValue);
        int getBlockValue(float x, float y, float z);
        int getBlockValue(int x, int y, int z);
        void updateBlockValueAndMesh(int x, int y, int z, int blockValue, const Camera& camera);

        ChunkManager(){}
    private:
        std::thread meshingThread1;
        std::thread meshingThread2;
        
        void updateChunkMesh_MT(Chunk* _chunk, Camera gameCamera);

        // Used for meshing.
        int getBlockValueFromPosition(Chunk* chunk, int x, int y, int z, int LoD);
        void makeVoxelAccountedFor(bool accountedVoxels[], int x, int y, int z);
        bool isAirBlock(int voxelValues[], int x, int y, int z, int LoD);
        bool isAccountedFor(bool accountedVoxels[], int x, int y, int z);
        bool isFacingAirblock(int voxelValues[], int x, int y, int z, int reverseConstant, int constantPos, int LoD);
        void updateTextureValue(int& textureValue, voxelFaces face, int LoD);
        void buildVoxelValueArray(int voxelValues[], Chunk* chunk, int LoD);
        int getVoxelValue(int voxelValues[], int x, int y, int z);
};