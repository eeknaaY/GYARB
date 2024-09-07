#include "mesh.hpp"
#include "../shaders/shaders.hpp"
#include "../structures/octree.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Chunk{
    public:
        static const int CHUNK_SIZE = 32;
        int xCoordinate, zCoordinate;
        int LoD;

        Mesh mesh;
        Octree* octree;

        void draw(const Shader &shader);
        void updateMesh();
};

void Chunk::draw(const Shader &shader){
    mesh.draw(shader, xCoordinate, zCoordinate);
}

void Chunk::updateMesh(){
    if (mesh.bufferExists){
        mesh.updateMesh();
    } else {
        mesh.bindMesh();
    }
}