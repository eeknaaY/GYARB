#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <vector>
#include <map>
#include <tuple>
#include "voxelobject.hpp"
#include <glad/glad.h>
#include <stdexcept>      // std::out_of_range
#include "../shaders/shaders.hpp"
#include "mesh.hpp"
#include "../structures/octree.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Chunk{
    public:
        static const int CHUNK_SIZE = 32;
        int xCoordinate, zCoordinate;

        Mesh mesh;
        Octree* octree = new Octree();

        void draw(const Shader &shader);

        Chunk(int _xCoordinate, int _zCoordinate){
            this->xCoordinate = _xCoordinate;
            this->zCoordinate = _zCoordinate;
        }
        ~Chunk();
        Chunk() = default;
};

Chunk::~Chunk(){
    delete octree;
}


void Chunk::draw(const Shader &shader){
    mesh.draw(shader, xCoordinate, zCoordinate);
}