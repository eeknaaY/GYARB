#include "mesh.hpp"
#include "../shaders/shaders.hpp"
#include "../structures/octree.hpp"
#include "chunk.hpp"
#include <algorithm>

void Chunk::sortTransparentFaces(const Camera& camera){
    struct VoxelFace{
        Vertex faceVertices[4];
        unsigned int faceIndices[6];

        VoxelFace(){};
        float distanceFromCamera = 0;
        float distanceFromFurthestPoint = 0;

        void setDistanceFromCamera(const Camera& camera, Chunk* currentChunk){
            glm::vec3 sumPosition = glm::vec3(0);

            for (Vertex vertex : faceVertices){
                int zPosition = (vertex.bitPackedData1 & 0x3F) + CHUNK_SIZE * currentChunk->zCoordinate;
                int yPosition = ((vertex.bitPackedData1 >> 6) & 0x3F) + CHUNK_SIZE * currentChunk->yCoordinate;
                int xPosition = ((vertex.bitPackedData1 >> 12) & 0x3F) + CHUNK_SIZE * currentChunk->xCoordinate;
                sumPosition += glm::vec3(xPosition, yPosition, zPosition);
                distanceFromFurthestPoint = std::max(distanceFromFurthestPoint, camera.distanceFromCamera(xPosition, yPosition, zPosition));
            }

            distanceFromCamera = camera.distanceFromCamera(sumPosition.x / 4.f, sumPosition.y / 4.f, sumPosition.z / 4.f, false);
        }
    };

    if (this == nullptr) return;
    if (this->mesh.transparent_indices.size() == 0) return;

    std::vector<VoxelFace> transparentFaces;
    for (int faceCount = 0; faceCount < this->mesh.transparent_vertices.size() / 4; faceCount++){
        VoxelFace face = VoxelFace();
        // std::copy(this->mesh.transparent_vertices[4 * faceCount], this->mesh.transparent_vertices[4 * faceCount + 3], face.faceVertices);
        // std::copy(this->mesh.transparent_indices[6 * faceCount], this->mesh.transparent_indices[6 * faceCount + 5], face.faceIndices);
        face.faceVertices[0] = this->mesh.transparent_vertices[4 * faceCount];
        face.faceVertices[1] = this->mesh.transparent_vertices[4 * faceCount + 1];
        face.faceVertices[2] = this->mesh.transparent_vertices[4 * faceCount + 2];
        face.faceVertices[3] = this->mesh.transparent_vertices[4 * faceCount + 3];
        
        face.faceIndices[0] = this->mesh.transparent_indices[6 * faceCount] % 4;
        face.faceIndices[1] = this->mesh.transparent_indices[6 * faceCount + 1] % 4;
        face.faceIndices[2] = this->mesh.transparent_indices[6 * faceCount + 2] % 4;
        face.faceIndices[3] = this->mesh.transparent_indices[6 * faceCount + 3] % 4;
        face.faceIndices[4] = this->mesh.transparent_indices[6 * faceCount + 4] % 4;
        face.faceIndices[5] = this->mesh.transparent_indices[6 * faceCount + 5] % 4;

        face.setDistanceFromCamera(camera, this);

        transparentFaces.push_back(face);
    }

    std::sort(transparentFaces.begin(), transparentFaces.end(), [](const VoxelFace& left, const VoxelFace& right){
        return left.distanceFromFurthestPoint == right.distanceFromFurthestPoint ? left.distanceFromCamera > right.distanceFromCamera : left.distanceFromFurthestPoint > right.distanceFromFurthestPoint;
    });

    this->mesh.transparent_vertices.clear();
    this->mesh.transparent_indices.clear();

    for (VoxelFace face : transparentFaces){
        //this->mesh.transparent_vertices.insert(this->mesh.transparent_vertices.end(), face.faceVertices, face.faceVertices + 4);
        this->mesh.transparent_vertices.push_back(face.faceVertices[0]);
        this->mesh.transparent_vertices.push_back(face.faceVertices[1]);
        this->mesh.transparent_vertices.push_back(face.faceVertices[2]);
        this->mesh.transparent_vertices.push_back(face.faceVertices[3]);

        int arraySize = this->mesh.transparent_vertices.size() - 4;
        this->mesh.transparent_indices.push_back(face.faceIndices[0] + arraySize);
        this->mesh.transparent_indices.push_back(face.faceIndices[1] + arraySize);
        this->mesh.transparent_indices.push_back(face.faceIndices[2] + arraySize);
        this->mesh.transparent_indices.push_back(face.faceIndices[3] + arraySize);
        this->mesh.transparent_indices.push_back(face.faceIndices[4] + arraySize);
        this->mesh.transparent_indices.push_back(face.faceIndices[5] + arraySize);
    }

    updateTransparentMesh();
}

void Chunk::draw(const Shader &shader){
    mesh.drawChunk(shader, xCoordinate, yCoordinate, zCoordinate);
}

void Chunk::updateTransparentMesh(){
    if (mesh.bufferExists){
        mesh.updateTransparentMesh();
    } 
}

void Chunk::updateMesh(){
    if (mesh.bufferExists){
        mesh.updateMesh();
    } else {
        mesh.bindMesh();
    }
}

void Chunk::setBlockValue(int x, int y, int z, int blockValue){
    if (!this) return;
    octree->updateNodeValueFromPosition(x, y, z, blockValue);
}

int Chunk::getBlockValue(int x, int y, int z){
    if (!this) return 0;
    return octree->getNodeFromPosition(x, y, z)->blockValue;
}