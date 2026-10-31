#pragma once
#ifndef NODE_H
#define NODE_H

#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include "record.h"

#define _IDCOLUMN 0

using namespace std;

typedef Record* KeyType;

class Node {

public:
    vector<KeyType> keys;
    vector<Node*> pointers;
    vector<vector<KeyType>> paths;
    Node* nextList;
    Node* lastList;
    bool isLeaf;
    int numberOfKeys;
    Node* parent;

    Node(int order, bool leaf) {
        keys.resize(order - 1);
        numberOfKeys = 0;
        nextList = NULL;
        lastList = NULL;
        isLeaf = leaf ? true : false;
        keys.clear();
        numberOfKeys = 0;
        if (isLeaf) {
            paths.resize(order - 1);
            paths.clear();
        }
        else {
            pointers.resize(order);
            pointers.clear();
        }
    }

    Node(int order) {
        keys.resize(order - 1);
        pointers.resize(order);
        nextList = NULL;
        lastList = NULL;
        numberOfKeys = 0;
        isLeaf = false;
        keys.clear();
        pointers.clear();
    }

    void sortKeys() {
        sort(this->keys.begin(), this->keys.begin() + this->numberOfKeys);
    }
};


#endif //NODE_H

