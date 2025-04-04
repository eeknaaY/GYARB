#include <map>
#include "octree.hpp"

Node::Node(){
    blockValue = -1;
    isEndNode = true;
    parent = nullptr;
}

Node::Node(int value, int depth, Node* parentPtr, bool _isEndNode, bool initChildren){
    blockValue = value;
    isEndNode = _isEndNode;
    parent = parentPtr;


    if (initChildren && !isEndNode){
        this->children.reserve(8);
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

Node::Node(Node* parentPtr){
    this->blockValue = parentPtr->blockValue;
    isEndNode = true;
    parent = parentPtr;
    this->children.clear();
}

Node::~Node(){
    for (Node* child : children){
        delete child;
        child = 0;
    }

    if (parent != nullptr){
        isEndNode = true;
    }
}

void Node::createIdenticalChildren(){
    this->isEndNode = false;
    for (int i = 0; i < 8; i++){
        this->children.push_back(new Node(this));
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

Octree::Octree(){
    mainNode = new Node(0, 0, nullptr, false, true);
}

Octree::Octree(int initValue){
    mainNode = new Node(initValue, 0, nullptr, true, false);
}

Octree::~Octree(){
    delete mainNode;
    mainNode = 0;
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
        } 
        else{
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

void Octree::optimizeTree(){
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

size_t Octree::nodeAmount(){
    size_t size = 0;
    size++;
    // size+=sizeof(Node);
    // size+=mainNode->children.capacity() * 8;
    for (Node* node_1 : mainNode->children){
        size++;
        // size+=sizeof(Node);
        // size+=node_1->children.capacity() * 8;
        for (Node* node_2 : node_1->children){
            size++;
            // size+=sizeof(Node);
            // size+=node_2->children.capacity() * 8;
            for (Node* node_3 : node_2->children){
                size++;
                // size+=sizeof(Node);
                // size+=node_3->children.capacity() * 8;
                for (Node* node_4 : node_3->children){
                    size++;
                    // size+=sizeof(Node);
                    // size+=node_4->children.capacity() * 8;
                    for (Node* node_5 : node_4->children){
                        size++;
                        // size+=sizeof(Node);
                        // size+=node_5->children.capacity() * 8;
                    }
                }
            }
        }
    }
    return size;
}

void Octree::updateNodeValueFromPosition(int x, int y, int z, int blockValue){
    // When the width is 1, we have reached the end.
    short voxelWidth;
    Node* curNode = getNodeFromPosition(x, y, z, voxelWidth);

    // Loop through until we've reached end of tree
    if (voxelWidth != 1){
        curNode->createIdenticalChildren();
        updateNodeValueFromPosition(x, y, z, blockValue);
        return;
    } else {
        curNode->blockValue = blockValue;
    }
}

