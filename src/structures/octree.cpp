#include <vector>
#include <map>
#include <algorithm> 
#include <math.h>
#include <memory>

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
        
        void makeNodeEndPoint(int value);
        ~Node();
        Node(){
            blockValue = -1;
            isEndNode = true;
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

    if (parent != nullptr){
        isEndNode = true;
    }
}

void Node::makeNodeEndPoint(int value){
    blockValue = value;

    if (!isEndNode){
        for (int i = 0; i < 8; i++){
            delete children[i];
            children[i] = 0;
        }
    }

    isEndNode = true;
    children.clear();
}

// Depth 0 = Main Node, 1 = First 8 partions, 2 = 64 partions...
class Octree{
    public:
        Octree();
        ~Octree();
        Node* mainNode;
        Node* getNodeFromPosition(int _x, int _y, int _z, int &width, int _depth = 5);
        Node* getNodeFromPosition(int _x, int _y, int _z);
        int getAverageBlockValueFromChildren(Node* parentNode);
        bool allChildrenAreEqual(Node* parentNode);
        bool aChildIsNotAnEndpoint(Node* parentNode);
        
        int nodeAmount();
        void TEMP_setBlockValues();
        void TEMP_optimizeTree();
        int blockDeterminationFunc(int x, int y, int z);
};

Octree::Octree(){
    mainNode = new Node(-1, 0, nullptr, false, true);
    TEMP_setBlockValues();
    TEMP_optimizeTree();
}

Octree::~Octree(){
    delete mainNode;
}

Node* Octree::getNodeFromPosition(int _x, int _y, int _z){
    int non_use;
    return getNodeFromPosition(_x, _y, _z, non_use);
}

Node* Octree::getNodeFromPosition(int _x, int _y, int _z, int &width, int _depth){
    Node* currentNode = mainNode;
    for (int depth = 1; depth <= _depth; depth++){

        // Since we only need reduction of power of 2 we can use bit shift for fast calculations
        int midLine = 1 << (5 - depth);
        int positionReduction = midLine;
        width = midLine;

        // If current node doesnt have children -> return
        if (currentNode->isEndNode) return currentNode;

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

int Octree::getAverageBlockValueFromChildren(Node* parentNode){
    // Node has no children
    if (parentNode->isEndNode) return parentNode->blockValue;

    std::map<int, int> mp;
    int curMaxKey = -1;
    int curMaxValue = 0;
    for (int i = 0; i < 8; i++){
        int blockValue = parentNode->children[i]->blockValue;
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
            if (std::max(curMaxValue, mp[blockValue]) == mp[blockValue] && curMaxKey != blockValue){
                curMaxKey = blockValue;
                curMaxValue = mp[blockValue];
            }
        }

        if (curMaxValue >= 4) return curMaxKey;
    }

    return curMaxKey;
}

bool Octree::allChildrenAreEqual(Node* parentNode){
    int firstVal = parentNode->children[0]->blockValue;
    for (Node* node : parentNode->children){
        if (node->blockValue != firstVal) return false;
    }
    
    return true;
}

bool Octree::aChildIsNotAnEndpoint(Node* parentNode){
    for (Node* node : parentNode->children){
        if (!node->isEndNode) return true;
    }
    
    return false;
}

#include <ctime>

void Octree::TEMP_setBlockValues(){
    for (int x = 0; x < 32; x++){
        for (int y = 0; y < 32; y++){
            for (int z = 0; z < 32; z++){
                getNodeFromPosition(x, y, z)->blockValue = blockDeterminationFunc(x, y, z);
            }
        }
    }
}

int Octree::blockDeterminationFunc(int x, int y, int z){
    if (y < 10) return 2;
    if (y == 10) return 1;
    if (y > 10) return 0;
    return -1;
}

void Octree::TEMP_optimizeTree(){
    for (Node* node_1 : mainNode->children){
        for (Node* node_2 : node_1->children){
            for (Node* node_3 : node_2->children){
                for (Node* node_4 : node_3->children){
                    if (this->allChildrenAreEqual(node_4) && !this->aChildIsNotAnEndpoint(node_4)){
                        node_4->makeNodeEndPoint(node_4->children[0]->blockValue);
                    } else {
                        node_4->blockValue = this->getAverageBlockValueFromChildren(node_4);
                    }

                }

                if (this->allChildrenAreEqual(node_3) && !this->aChildIsNotAnEndpoint(node_3)){
                    node_3->makeNodeEndPoint(node_3->children[0]->blockValue);
                } else {
                    node_3->blockValue = this->getAverageBlockValueFromChildren(node_3);
                }
            }

            if (this->allChildrenAreEqual(node_2) && !this->aChildIsNotAnEndpoint(node_2)){
                node_2->makeNodeEndPoint(node_2->children[0]->blockValue);
            } else {
                node_2->blockValue = this->getAverageBlockValueFromChildren(node_2);
            }
        }

        if (this->allChildrenAreEqual(node_1) && !this->aChildIsNotAnEndpoint(node_1)){
            node_1->makeNodeEndPoint(node_1->children[0]->blockValue);
        } else {
            node_1->blockValue = this->getAverageBlockValueFromChildren(node_1);
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

