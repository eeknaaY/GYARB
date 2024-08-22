#pragma once
#include <vector>

class Node{
    public:
        int blockValue;
        bool isEndNode;
        Node* parent;
        std::vector<Node*> children;
        
        void makeNodeEndPoint(int value);
        ~Node();
        Node();
        Node(int value, int depth, Node* parentPtr, bool _isEndNode, bool initChildren);
};

class Octree{
    public:
        Octree();
        ~Octree();
        Node* mainNode;
        Node* getNodeFromPosition(int _x, int _y, int _z, int _depth = 4);
        int getAverageBlockValueFromChildren(Node* parentNode);
        bool allChildrenAreEqual(Node* parentNode);
        bool aChildIsNotAnEndpoint(Node* parentNode);
        
        int nodeAmount();
        void TEMP_setBlockValues();
        void TEMP_optimizeTree();
        int blockDeterminationFunc(int x, int y, int z);
};
