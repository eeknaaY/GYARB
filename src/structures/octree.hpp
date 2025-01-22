#pragma once
#include <vector>
#include "FastNoiseLite.h"
#include "../gfx/biomes.hpp"
#include "../gfx/mesh.hpp"

enum nodePositions{
    BottomLeftFront,
    BottomLeftBack,
    BottomRightFront,
    BottomRightBack,
    TopLeftFront,
    TopLeftBack,
    TopRightFront,
    TopRightBack
};

class Node{
    public:
        int blockValue;
        bool isEndNode;
        Node* parent;
        std::vector<Node*> children;
        
        void makeNodeEndPoint();
        int getAverageBlockValueFromChildren();
        bool allChildrenAreEqual();
        bool aChildIsNotAnEndpoint();
        void createIdenticalChildren();

        ~Node();
        Node();
        Node(Node* parentPtr);
        Node(int value, int depth, Node* parentPtr, bool _isEndNode, bool initChildren);        
};

class Octree{
    public:
        Node* mainNode;
        
        Node* getNodeFromPosition(int _x, int _y, int _z, int _depth = 5);
        Node* getNodeFromPosition(int _x, int _y, int _z, short &width, int _depth = 5);
        size_t nodeAmount();
        void setInitialBlockValues(int _chunk_xcoord, int _chunk_ycoord, int _chunk_zcoord, Biome* biomeType);
        void optimizeTree();
        void buildAMinecraftTree(int x, int y, int z);
        void updateNodeValueFromPosition(int x, int y, int z, int blockValue);
        void buildOctreeLineMesh(LineMesh& mesh);

        Octree();
        Octree(int _chunk_xcoord, int _chunk_ycoord, int _chunk_zcoord, Biome* biomeType);
        Octree(int initValue);
        ~Octree();
    private:
        void putNodeEdgesIntoLineMesh(int _x, int _y, int _z, short &width, int _depth, LineMesh& mesh);
        int LoD;
        int chunk_xcoord, chunk_zcoord;
};
