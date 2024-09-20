#include "mesh.hpp"
#include "../shaders/shaders.hpp"
#include "../structures/octree.hpp"
#include "chunk.hpp"

void Chunk::draw(const Shader &shader){
    mesh.draw(shader, xCoordinate, yCoordinate, zCoordinate);
}

void Chunk::updateMesh(){
    if (mesh.bufferExists){
        mesh.updateMesh();
    } else {
        mesh.bindMesh();
    }
}