#include "mesh.hpp"
#include "../shaders/shaders.hpp"
#include "../structures/octree.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Chunk{
    public:
        static const int CHUNK_SIZE = 32;
        int xCoordinate, zCoordinate;

        Mesh mesh;
        Octree octree = Octree();

        void draw(const Shader &shader);

        Chunk(int _xCoordinate, int _zCoordinate){
            this->xCoordinate = _xCoordinate;
            this->zCoordinate = _zCoordinate;
        }
        Chunk() = default;
};



void Chunk::draw(const Shader &shader){
    mesh.draw(shader, xCoordinate, zCoordinate);
}