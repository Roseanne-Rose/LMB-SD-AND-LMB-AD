#define _CRT_SECURE_NO_WARNINGS
#include "data.h"
#include <fstream>
#include <iostream>
#include <cstdlib> // 对于 exit()
using namespace std;

Data::~Data()
{
    if (in != NULL) fclose(in);
}

Data::Data(char* datafile, char* queryfile)
{
    this->datafile = datafile;
    this->queryfile = queryfile;
    maxElem = maxLen = 0;
    minElem = minLen = 2147483647;
}

Record* Data::readNextRecord() {
    // if end of file is reached, rewind to beginning for next pass
    if (feof(in)) {
        rewind(in);
        return NULL;
    }

    //    set<int>::iterator it;
    Record* rec = new Record();
    char c;
    do {
        int item = 0, pos = 0;
        c = getc(in);
        while ((c >= '0') && (c <= '9')) {
            item *= 10;
            item += int(c) - int('0');
            c = getc(in);
            pos++;
        }
        if (pos) rec->elems.push_back(item);
        if (item > maxElem) maxElem = item;
        if (item < minElem) minElem = item;

    } while (c != '\n' && !feof(in));

    sort(rec->elems.begin(), rec->elems.end(), compareByElem);//集合元素从小到大升序排序
    return rec;
}

void Data::readData() {
    //    int id = 0;
    Record* rec;

    // 打开 datafile
    in = fopen(datafile, "rt");
    if (in == NULL) {
        perror("Failed to open datafile");
        return;
    }

    // 读取 datafile 内容
    while ((rec = readNextRecord()) != NULL) {
        //        rec->id = id++;
        vec_trans.push_back(rec);
        if (rec->elems.size() > maxLen) maxLen = rec->elems.size();
        if (rec->elems.size() < minLen) minLen = rec->elems.size();
    }

    fclose(in);
    in = NULL;

    sort(vec_trans.begin(), vec_trans.end(), compareByLength);//按照集合长度升序排序

    // 过滤 vec_trans
    vector<Record*> vec_trans_temp;
    for (int i = 0; i < vec_trans.size(); i++) {
        if (vec_trans[i]->elems.size() > MAX_LENGTH) {
            vec_longset.push_back(vec_trans[i]);
            continue;
        }
        vec_trans_temp.push_back(vec_trans[i]);
    }
    vec_trans = vec_trans_temp;

    // 重置状态
//    id = 0;
    maxElem = maxLen = 0;
    minElem = minLen = 2147483647;

    // 打开 queryfile
    in = fopen(queryfile, "rt");
    if (in == NULL) {
        perror("Failed to open queryfile");
        return;
    }

    // 读取 queryfile 内容
    while ((rec = readNextRecord()) != NULL) {
        //        rec->id = id++;
        vec_querys.push_back(rec);
        if (rec->elems.size() > maxLen) maxLen = rec->elems.size();
        if (rec->elems.size() < minLen) minLen = rec->elems.size();
    }

    fclose(in);
    in = NULL;

    sort(vec_querys.begin(), vec_querys.end(), compareByLength);//按照集合长度升序排序
}