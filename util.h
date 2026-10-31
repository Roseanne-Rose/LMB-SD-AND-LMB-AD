#pragma once
#ifndef UTIL_H
#define UTIL_H
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <time.h>
#include <vector>
#include "record.h"
#include "node.h"

using namespace std;
//{4,6,8,10,12}
//{4095,511,127,31,15}
#define BUCKET_NUMBER 8 // ·ÖÍ°ÊýÁ¿
#define MAX_LENGTH 127
#define WORD_SIZE (sizeof(unsigned long long) * 8)
bool compareByElem(const int& a, const int& b);

bool compareByLength(const Record* a, const Record* b);
bool compareRecords(const Record* a, const Record* b);
bool compareByValue(const pair<int, int>& a, const pair<int, int>& b);
char* generateDatafile(const char database[]);
char* generateQueryfile(const char database[]);
unsigned long long generateMask(int num_bits);
int judgeRecords(const Record* rec1, const Record* rec2);
int judgeLongRecords(const Record* rec1, const Record* rec2);
bool areNodesEqual(Node* node1, Node* node2);

#endif //UTIL_H

