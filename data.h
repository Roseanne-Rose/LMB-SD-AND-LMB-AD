#pragma once
#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <set>
#include <vector>
#include "record.h"
#include "util.h"
#include <algorithm>
#include <string>

using namespace std;

class Data
{
public:
    ~Data();
    Data(char* datafile, char* queryfile);
    Record* readNextRecord();
    void readData();

public:
    FILE* in;
    char* datafile;
    char* queryfile;
    int maxElem;
    int minElem;
    int minLen;
    int maxLen;

    vector<Record*> vec_trans;
    vector<Record*> vec_querys;
    vector<Record*> vec_longset;
};

#endif //DATA_H

