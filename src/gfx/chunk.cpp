#include "mesh.hpp"
#include "../shaders/shaders.hpp"
#include "../structures/octree.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Chunk{
    public:
        static const int CHUNK_SIZE = 32;
        int xCoordinate, zCoordinate;

        Mesh mesh;
        Octree octree;

        void draw(const Shader &shader);
};



void Chunk::draw(const Shader &shader){
    mesh.draw(shader, xCoordinate, zCoordinate);
}