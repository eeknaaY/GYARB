#pragma once
#include <vector>
#include "FastNoiseLite.h"
#include "../gfx/biomes.hpp"

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
        void optimizeTree();
        void updateNodeValueFromPosition(int x, int y, int z, int blockValue);

        Octree();
        Octree(int initValue);
        ~Octree();
    private:
        int LoD;
        int chunk_xcoord, chunk_zcoord;
};
