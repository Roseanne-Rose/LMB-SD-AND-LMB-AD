#include "util.h"
bool compareByElem(const int& a, const int& b) {
    return a < b;
}

bool compareByLength(const Record* a, const Record* b)
{
    return a->elems.size() < b->elems.size();
}

bool compareRecords(const Record* a, const Record* b) {
    return a->key < b->key;
}

bool compareByValue(const pair<int, int>& a, const pair<int, int>& b) {
    return a.second > b.second;
}

int judgeRecords(const Record* rec1, const Record* rec2) {
    // 首先检查key是否相等
    if (rec1->key > rec2->key || rec1->elems.size() > rec2->elems.size())
        return 1;
    if (rec1->key < rec2->key || rec1->elems.size() < rec2->elems.size())
        return -1;
    for (int i = 0; i < rec1->elems.size(); i++)
    {
        if (rec1->elems[i] > rec2->elems[i])
            return 1;
        else if (rec2->elems[i] < rec2->elems[i])
            return -1;
        else
            continue;
    }
    return 0;
}

int judgeLongRecords(const Record* rec1, const Record* rec2) {
    // 首先检查key是否相等
    if (rec1->elems.size() > rec2->elems.size())
        return 1;
    if (rec1->elems.size() < rec2->elems.size())
        return -1;
    for (int i = 0; i < rec1->elems.size(); i++)
    {
        if (rec1->elems[i] > rec2->elems[i])
            return 1;
        else if (rec2->elems[i] < rec2->elems[i])
            return -1;
        else
            continue;
    }
    return 0;
}

// 比较两个Node*是否相等
bool areNodesEqual(Node* node1, Node* node2) {
    // 首先比较基本类型成员变量
    if (node1->isLeaf != node2->isLeaf ||
        node1->numberOfKeys != node2->numberOfKeys) {
        return false;
    }

    // 比较keys向量
    for (int i = 0; i < node1->numberOfKeys; i++)
    {
        if (judgeRecords(node1->keys[i], node2->keys[i]))
            return false;
    }
    return true;
}

char* generateFile(const char database[], const char file_extension[]) {
    // 检查 database 是否为 NULL
    if (database == nullptr) {
        std::cerr << "Error: database is null." << std::endl;
        return nullptr;
    }

    // 计算总长度
    const int base_path_len = strlen("/home/z/Lss/ZcurveDataset1/");
    const int query_path_len = strlen("/query/");
    const int file_ext_len = strlen(file_extension);
    const int total_len = base_path_len + strlen(database) * 2 + query_path_len + file_ext_len + 1; // +1 for null terminator

    // 动态分配内存
    char* datafile = new char[total_len];

    // 构建路径
    snprintf(datafile, total_len, "/home/z/Lss/ZcurveDataset1/%s/query/%s%s", database, database, file_extension);

    // 返回结果
    return datafile;
}

//char* generateFile(const char database[], const char file_extension[]) {
//    // 检查 database 是否为 NULL
//    if (database == nullptr) {
//        std::cerr << "Error: database is null." << std::endl;
//        return nullptr;
//    }
//
//    // 计算总长度
//    const int base_path_len = strlen("D:\\长度分区的LiB索引\\数据集\\");
//    const int file_ext_len = strlen(file_extension);
//    const int total_len = base_path_len + 2*strlen(database) + file_ext_len + 10; // +1 for null terminator
//
//    //// 动态分配内存
//    char* datafile = new char[total_len];
//
//    //// 构建路径
//    snprintf(datafile, total_len, "D:\\长度分区的LiB索引\\数据集\\%s\\query\\%s%s", database, database, file_extension);
//
//    //// 返回结果
//    return datafile;
//}


char* generateDatafile(const char database[]) {
    return generateFile(database, ".txt");
}

char* generateQueryfile(const char database[]) {
    return generateFile(database, "_query.txt");
}

unsigned long long generateMask(int num_bits) {
    unsigned long long mask = (1ULL << num_bits) - 1;
    return mask;
}
