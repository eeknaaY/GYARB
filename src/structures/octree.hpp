#pragma once
#include <vector>
#include "FastNoiseLite.h"

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
        Octree();
        Octree(int _chunk_xcoord, int _chunk_ycoord, int _chunk_zcoord, FastNoiseLite noise);
        ~Octree();
        Node* mainNode;
        Node* getNodeFromPosition(int _x, int _y, int _z, int _depth = 5);
        Node* getNodeFromPosition(int _x, int _y, int _z, short &width, int _depth = 5);
        
        int nodeAmount();
        void TEMP_setBlockValues(int _chunk_xcoord, int _chunk_ycoord, int _chunk_zcoord, FastNoiseLite noise);
        void TEMP_optimizeTree();
        int TEMP_blockDeterminationFunc(int y, int localMaxHeight, int globalMaxHeight, int waterLevel);
        void buildAMinecraftTree(int x, int y, int z);
        void updateNodeValueFromPosition(int x, int y, int z, int blockValue);
    private:
        int LoD;
        int chunk_xcoord, chunk_zcoord;
};
