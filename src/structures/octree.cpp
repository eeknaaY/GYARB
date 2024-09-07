#include <vector>
#include <map>
#include <algorithm> 
#include <math.h>
#include <memory>

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

        ~Node();
        Node(){
            blockValue = -1;
            isEndNode = true;
            parent = nullptr;
        }
        Node(int value, int depth, Node* parentPtr, bool _isEndNode, bool initChildren){
            blockValue = value;
            isEndNode = _isEndNode;
            parent = parentPtr;


            if (initChildren && !isEndNode){
                if (depth < 4) {
                    for (int i = 0; i < 8; i++){
                        children.push_back(new Node(value, depth + 1, this, false, initChildren));
                    }
                } else {
                    for (int i = 0; i < 8; i++){
                        children.push_back(new Node(value, depth + 1, this, true, false));
                    }
                }
            } 
        }
};

Node::~Node(){
    for (Node* child : children){
        delete child;
        child = 0;
    }

    children.clear();
    children.shrink_to_fit();

    if (parent != nullptr){
        isEndNode = true;
    }
}

void Node::makeNodeEndPoint(){
    blockValue = this->children[0]->blockValue;

    if (!isEndNode){
        for (int i = 0; i < 8; i++){
            delete children[i];
            children[i] = 0;
        }
    }

    isEndNode = true;
    children.clear();
    children.shrink_to_fit();
}

int Node::getAverageBlockValueFromChildren(){
    // Node has no children
    if (this->isEndNode) return this->blockValue;

    std::map<int, int> mp;
    int curMaxKey = -1;
    int curMaxValue = 0;
    for (int i = 0; i < 8; i++){
        int blockValue = this->children[i]->blockValue;
        if (mp.find(blockValue) == mp.end()) {
            // not found
            mp[blockValue] = 1;
            if (std::max(curMaxValue, mp[blockValue]) == mp[blockValue] && curMaxKey != blockValue){
                curMaxKey = blockValue;
                curMaxValue = mp[blockValue];
            }

        } else {
            // found
            mp[blockValue] += 1;
            if (std::max(curMaxValue, mp[blockValue]) == mp[blockValue]){
                curMaxKey = blockValue;
                curMaxValue = mp[blockValue];
            }
        }

        // Prioritize non-air blocks if its a 50/50
        if (curMaxKey != 0 && curMaxValue >= 4) return curMaxKey;
        if (curMaxValue >= 5) return curMaxKey;
    }

    return curMaxKey;
}

bool Node::allChildrenAreEqual(){
    int firstVal = this->children[0]->blockValue;
    for (Node* node : this->children){
        if (node->blockValue != firstVal) return false;
    }
    
    return true;
}

bool Node::aChildIsNotAnEndpoint(){
    for (Node* node : this->children){
        if (!node->isEndNode) return true;
    }
    
    return false;
}

// Depth 0 = Main Node, 1 = First 8 partions, 2 = 64 partions...
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
        int LoD;
        int chunk_xcoord, chunk_zcoord;
};

Octree::Octree(){
    mainNode = new Node();
}

Octree::Octree(int _chunk_xcoord, int _chunk_zcoord){
    mainNode = new Node(-1, 0, nullptr, false, true);
    chunk_xcoord = _chunk_xcoord;
    chunk_zcoord = _chunk_zcoord;
    TEMP_setBlockValues(_chunk_xcoord, _chunk_zcoord);
    TEMP_optimizeTree();
}

Octree::~Octree(){
    delete mainNode;
}

Node* Octree::getNodeFromPosition(int _x, int _y, int _z, int _depth){
    short non_use;
    return getNodeFromPosition(_x, _y, _z, non_use, _depth);
}

#include <iostream>

Node* Octree::getNodeFromPosition(int _x, int _y, int _z, short &width, int _depth){
    Node* currentNode = mainNode;

    if (currentNode->parent != nullptr){
        return nullptr;
        std::cout << "Trying to get position of invalid node // Octree.getNodeFromPosition()" << std::endl;
    }

    for (int depth = 1; depth <= _depth; depth++){
        // If current node doesnt have children -> return
        if (currentNode->isEndNode) return currentNode;

        // Since we only need reduction of power of 2 we can use bit shift for fast calculations
        int midLine = 1 << (5 - depth);
        int positionReduction = midLine;
        width = midLine;

if (_x < midLine){
            if (_y < midLine){
                if (_z < midLine){           
                    currentNode = currentNode->children[BottomLeftFront];
                    continue;
                } 
                else{          
                    currentNode = currentNode->children[BottomLeftBack];
                    _z -= positionReduction;
                    continue;
                }
            }
            else{
                if (_z < midLine){             
                    currentNode = currentNode->children[TopLeftFront];
                    _y -= positionReduction;
                    continue;
                }
                else{               
                    currentNode = currentNode->children[TopLeftBack];
                    _y -= positionReduction;
                    _z -= positionReduction;
                    continue;
                }
            }
        } else{
            if (_y < midLine){
                if (_z < midLine){                   
                    currentNode = currentNode->children[BottomRightFront];
                    _x -= positionReduction;
                    continue;
                } 
                else{   
                    currentNode = currentNode->children[BottomRightBack];
                    _z -= positionReduction;
                    _x -= positionReduction;
                    continue;
                }
            }
            else{
                if (_z < midLine){                
                    currentNode = currentNode->children[TopRightFront];
                    _y -= positionReduction;
                    _x -= positionReduction;
                    continue;
                }
                else{      
                    currentNode = currentNode->children[TopRightBack];
                    _y -= positionReduction;
                    _z -= positionReduction;
                    _x -= positionReduction;
                    continue;
                }
            }
        }
    }

    return currentNode;
}

void Octree::TEMP_setBlockValues(int _chunk_xcoord, int _chunk_zcoord){
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(3);
    
    for (int x = 0; x < 32; x++){
        for (int z = 0; z < 32; z++){
            float noiseVal = noise.GetNoise((float)(x + 32 * _chunk_xcoord), (float)(z + 32 * _chunk_zcoord));
            for (int y = 0; y < 32; y++){
                getNodeFromPosition(x, y, z)->blockValue = TEMP_blockDeterminationFunc(y, 16 + (int)(16.f * noiseVal));;
            }
        }
    }
}

int Octree::TEMP_blockDeterminationFunc(int y, int maxHeight){
    if (y == 0) return 2;
    if (y > maxHeight) return 0;
    if (y > (maxHeight - 2)) return 1;
    return 2;
}

void Octree::TEMP_optimizeTree(){
    for (Node* node_1 : mainNode->children){
        for (Node* node_2 : node_1->children){
            for (Node* node_3 : node_2->children){
                for (Node* node_4 : node_3->children){
                    if (node_4->allChildrenAreEqual() && !node_4->aChildIsNotAnEndpoint()){
                        node_4->makeNodeEndPoint();
                    } else {
                        node_4->blockValue = node_4->getAverageBlockValueFromChildren();
                    }

                }

                if (node_3->allChildrenAreEqual() && !node_3->aChildIsNotAnEndpoint()){
                    node_3->makeNodeEndPoint();
                } else {
                    node_3->blockValue = node_3->getAverageBlockValueFromChildren();
                }
            }

            if (node_2->allChildrenAreEqual() && !node_2->aChildIsNotAnEndpoint()){
                node_2->makeNodeEndPoint();
            } else {
                node_2->blockValue = node_2->getAverageBlockValueFromChildren();
            }
        }

        if (node_1->allChildrenAreEqual() && !node_1->aChildIsNotAnEndpoint()){
            node_1->makeNodeEndPoint();
        } else {
            node_1->blockValue = node_1->getAverageBlockValueFromChildren();
        }
    }
}

int Octree::nodeAmount(){
    int n = 0;
    for (Node* node_1 : mainNode->children){
        n++;
        for (Node* node_2 : node_1->children){
            n++;
            for (Node* node_3 : node_2->children){
                n++;
                for (Node* node_4 : node_3->children){
                    n++;
                }
            }
        }
    }
    return n;
}

