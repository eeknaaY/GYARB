#pragma once
#include <vector>

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
        
        ~Node();
        Node();
        Node(int value, int depth, Node* parentPtr, bool _isEndNode, bool initChildren);
};

class Octree{
    public:
        Octree();
        Octree(int chunk_xcoord, int chunk_zcoord);
        ~Octree();
        Node* mainNode;
        Node* getNodeFromPosition(int _x, int _y, int _z, int _depth = 5);
        Node* getNodeFromPosition(int _x, int _y, int _z, short &width, int _depth = 5);
        
        int nodeAmount();
        void TEMP_setBlockValues(int chunk_xcoord, int chunk_zcoord);
        void TEMP_optimizeTree();
        int TEMP_blockDeterminationFunc(int y, int maxHeight);
    private:
        int chunk_xcoord, chunk_zcoord;
};
