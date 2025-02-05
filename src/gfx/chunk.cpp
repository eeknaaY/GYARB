#include "mesh.hpp"
#include "../shaders/shaders.hpp"
#include "../structures/octree.hpp"
#include "chunk.hpp"
#include <algorithm>

void Chunk::sortTransparentFaces(const Camera& camera){
    return;
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
    if (this->mesh.transparentIndices.size() == 0) return;

    std::vector<VoxelFace> transparentFaces;
    for (int faceCount = 0; faceCount < this->mesh.transparentVertices.size() / 4; faceCount++){
        VoxelFace face = VoxelFace();

        for (int i = 0; i < 4; i++){
            face.faceVertices[i] = this->mesh.transparentVertices[4 * faceCount + i];
        }

        for (int i = 0; i < 6; i++){
            face.faceIndices[i] = this->mesh.transparentIndices[6 * faceCount + i] % 4;
        }

        face.setDistanceFromCamera(camera, this);

        transparentFaces.push_back(face);
    }

    std::sort(transparentFaces.begin(), transparentFaces.end(), [](const VoxelFace& left, const VoxelFace& right){
        return left.distanceFromFurthestPoint == right.distanceFromFurthestPoint ? left.distanceFromCamera > right.distanceFromCamera : left.distanceFromFurthestPoint > right.distanceFromFurthestPoint;
    });

    this->mesh.transparentVertices.clear();
    this->mesh.transparentIndices.clear();

    for (VoxelFace face : transparentFaces){
        //this->mesh.transparentVertices.insert(this->mesh.transparentVertices.end(), face.faceVertices, face.faceVertices + 4);
        this->mesh.transparentVertices.push_back(face.faceVertices[0]);
        this->mesh.transparentVertices.push_back(face.faceVertices[1]);
        this->mesh.transparentVertices.push_back(face.faceVertices[2]);
        this->mesh.transparentVertices.push_back(face.faceVertices[3]);

        int arraySize = this->mesh.transparentVertices.size() - 4;
        this->mesh.transparentIndices.push_back(face.faceIndices[0] + arraySize);
        this->mesh.transparentIndices.push_back(face.faceIndices[1] + arraySize);
        this->mesh.transparentIndices.push_back(face.faceIndices[2] + arraySize);
        this->mesh.transparentIndices.push_back(face.faceIndices[3] + arraySize);
        this->mesh.transparentIndices.push_back(face.faceIndices[4] + arraySize);
        this->mesh.transparentIndices.push_back(face.faceIndices[5] + arraySize);
    }

    updateTransparentMesh();
}

void Chunk::draw(const Shader &shader){
    mesh.draw(shader, xCoordinate, yCoordinate, zCoordinate);
}

void Chunk::drawTransparent(const Shader &shader){
    mesh.drawTransparentChunk(shader, xCoordinate, yCoordinate, zCoordinate);
}

void Chunk::drawOpaque(const Shader &shader){
    mesh.drawOpaqueChunk(shader, xCoordinate, yCoordinate, zCoordinate);
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

void Chunk::useBackupMesh(){
    mesh.opaqueVertices = backupMesh.opaqueVertices;
    mesh.opaqueIndices = backupMesh.opaqueIndices;
    mesh.transparentVertices = backupMesh.transparentVertices;
    mesh.transparentIndices = backupMesh.transparentIndices;
    updateMesh();
}

void Chunk::setBlockValue(int x, int y, int z, int blockValue){
    if (!this) return;
    octree->updateNodeValueFromPosition(x, y, z, blockValue);
}

int Chunk::getBlockValue(int x, int y, int z){
    if (!this) return 0;
    return octree->getNodeFromPosition(x, y, z)->blockValue;
}

void Chunk::setInitialBlockValues(){
    Biome* currentBiome = biomeType;

    for (int x = 0; x < 32; x++){
        for (int z = 0; z < 32; z++){

            float globalMaxHeight = BiomeHandler::getHeightValue(32 * xCoordinate + x, 32 * zCoordinate + z);
            int localMaxHeight = globalMaxHeight - 32 * yCoordinate;

            if (localMaxHeight < 0 && yCoordinate == 0) localMaxHeight = 0;
            int waterLevel = biomeType->waterLevel;

            for (int y = 31; y >= 0; y--){
                int blockValue = biomeType->getBlockValue(y + 32 * yCoordinate, globalMaxHeight);
                // This stops the function overriding any non-air blocks when it wants them to be air, fucks trees over.
                if (blockValue == 0 && octree->getNodeFromPosition(x, y, z)->blockValue > 0) continue;
                
                octree->getNodeFromPosition(x, y, z)->blockValue = blockValue;

                if (x > 2 && x < 30 && z > 2 && z < 30 && y < 24){
                    if (biomeType->shouldGenerateTree(y + 32 * yCoordinate, globalMaxHeight)){
                        mesh.addTree(octree, x, y + 1, z);
                    }
                }
            }
        }
    }


}