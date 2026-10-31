#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>
#include <queue>

#include "node.h"
#include "data.h"

class BPtree {
public:
    // 常量定义
    static const int IDCOLUMN = 0;
    int treeOrder;
    int minN;// (treeOrder - 1) / 2
    int maxN;// treeOrder - 1

    // 成员函数的声明
    void bulkLoadingInsert(Node*& tree, std::vector<Record*> data);
    void upFirsts(std::vector<KeyType>& keys, std::vector<Node*>& pointers, int treeOrder, Node*& tree);
    float getTreeSize(Node* tree);

    int getTreeHeight(Node* tree);

    Node* findLeafNode(Node* T, unsigned long long searchKey);

    bool insertKey(Node*& tree, KeyType key, long int* splitNumPtr);

    bool splitNode(Node* node, long int* splitNumPtr);
    bool insertIntoParent(Node* oldNode, Node* newNode, Record* newKey, long int* splitNumPtr);
    void printTree(Node* node, int level);
    void deleteKey(Node*& tree, KeyType key);
    bool borrowFromSibling(Node*& node);
    int getParentId(Node* node);
    bool deleteFromParent(Node*& parent, KeyType key);
    bool mergeWithSibling(Node*& node);
};