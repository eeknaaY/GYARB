#include "mesh.hpp"
#include "../shaders/shaders.hpp"
#include "../structures/octree.hpp"
#include "chunk.hpp"

void Chunk::draw(const Shader &shader){
    mesh.drawChunk(shader, xCoordinate, yCoordinate, zCoordinate);
}

void Chunk::updateMesh(){
    if (mesh.bufferExists){
        mesh.updateMesh();
    } else {
        mesh.bindMesh();
    }
}

void Chunk::updateBlockValue(int x, int y, int z, int blockValue){
    if (!this) return;
    octree->updateNodeValueFromPosition(x, y, z, blockValue);
}

int Chunk::getBlockValue(int x, int y, int z){
    if (!this) return 0;
    return octree->getNodeFromPosition(x, y, z)->blockValue;
}