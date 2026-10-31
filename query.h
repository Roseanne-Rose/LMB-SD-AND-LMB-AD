#pragma once
#include <map>
#include <chrono>
#include "BPtree.h"
#include "node.h"

class subTree {
public:
    int length;
    vector<Record*> sub_trains;
    Node* sub_tree;
};

class LMBQ {
private:
    Data* data;
    vector<subTree*> multiTree;
    int maxBits = WORD_SIZE / BUCKET_NUMBER;
    int treeOrder = 16;
    int* elemsBuckets;
    BPtree bptree;
    int* bucketsCumulativeFrequencies;

    void init();

    void FB_GF();

    void HB_GF();

    void HB_HF();

    void CountSubtreesHeight();

    vector<Record*> getHalfTrans();

    void testSets();

    int* getElemsBucketsByZorder();
    int* getElemsBucketsByZorderCountCF();
    void generateKeyNoLen(vector<int> elems, unsigned long long& res_vec);
    int* getCountBucketNumber(unsigned long long key);
    Node* findHeadNode(unsigned long long searchKey, int curLen);
    Node* findRearNode(unsigned long long searchKey, int curLen);

public:
    // 构造函数
    LMBQ(const Data& data) : data(new Data(data)) { // 深拷贝
        init();
    }
    unsigned long long getHeadKey(unsigned long long queryKey, int difValue, int currentLen);
    unsigned long long getRearKey(unsigned long long queryKey, int difValue, int currentLen);
    unsigned long long getHeadKeyNewAD(unsigned long long queryKey, int difValue);
    unsigned long long getRearKeyNewAD(unsigned long long queryKey, int difValue);
    unsigned long long getHeadKeyNew(unsigned long long queryKey, int queryLen, int difValue, int currentLen);
    unsigned long long getRearKeyNew(unsigned long long queryKey, int queryLen, int difValue, int currentLen);
    double getJaccard(Record* rec, Record* que);
    bool getJaccard(Record* rec, Record* que, double delta);
    vector<Record*> LMBQ_SD_PV_NONE(double delta);
    vector<Record*> LMBQ_SD(double delta);
    vector<Record*> LMBQ_SD_PV(double delta);
    vector<Record*> LMBQ_UD(double delta);
    vector<Record*> LMBQ_UD_PV(double delta);
    vector<Record*> LMBQ_Test(double delta);
    vector<Record*> LMBQ_Test_PV(double delta);
    vector<Record*> LMBQ_Test_PV_NONE(double delta);
    vector<Record*> LeBQP_CBDF_SingQuery(Record* query, double delta);
    bool Cosine(Record* que, Record* rec, double delta);
    bool Dice(Record* que, Record* rec, double delta);
    vector<Record*> LeBQP_CBDF_Cosine(double delta);
    vector<Record*> LeBQP_CBDF_Dice(double delta);
    void insertQuery();
    vector<Record*> searchAfterInsert(double delta);
    void testBInsert();
    void testBDelete();
    void deleteQuery();
    vector<Record*> searchAfterDelete(double delta);
    void InsertAfterBuild(vector<Record*> iRecords);
};

