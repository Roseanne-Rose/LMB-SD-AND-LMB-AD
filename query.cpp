
#include "query.h"
#include "math.h"
#include "util.h"
#include <set>
#include <numeric>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

void LMBQ::init() {
    //testBInsert();
    //testBDelete();
    bptree.treeOrder = treeOrder;
    bptree.minN = (bptree.treeOrder - 1) / 2;
    bptree.maxN = bptree.treeOrder - 1;

    auto start1 = chrono::high_resolution_clock::now();

    for (int i = 0; i <= MAX_LENGTH; i++)
    {
        subTree* subtree = new subTree();
        subtree->length = i;
        multiTree.push_back(subtree);
    }

    auto end1 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration1 = end1 - start1;

    cout << "Init time: " << duration1.count() << "ms" << endl;

    FB_GF();
    //HB_GF();
    //HB_HF();
}

void LMBQ::FB_GF()
{
    auto start2 = chrono::high_resolution_clock::now();
    elemsBuckets = getElemsBucketsByZorder();

    int lastLen = data->vec_trans[0]->elems.size();
    int lastId = 0;
    for (int i = 0; i < data->vec_trans.size(); i++) {
        //generateKey(data->vec_trans[i]->elems, data->vec_trans[i]->key);
        generateKeyNoLen(data->vec_trans[i]->elems, data->vec_trans[i]->key);
        int length = data->vec_trans[i]->elems.size();
        if (length > lastLen)
        {
            for (int j = lastId; j < i; j++)
            {
                Record* newRecord = new Record(*data->vec_trans[j]);
                multiTree[lastLen]->sub_trains.push_back(newRecord);
            }
            //sort(multiTree[lastLen]->sub_trains.begin(), multiTree[lastLen]->sub_trains.end(), compareRecords);
            bptree.bulkLoadingInsert(multiTree[lastLen]->sub_tree, multiTree[lastLen]->sub_trains);
            lastId = i;
            lastLen = length;
        }
    }
    //处理最后的部分
    for (int j = lastId; j < data->vec_trans.size(); j++)
    {
        Record* newRecord = new Record(*data->vec_trans[j]);
        multiTree[lastLen]->sub_trains.push_back(newRecord);
    }
    bptree.bulkLoadingInsert(multiTree[lastLen]->sub_tree, multiTree[lastLen]->sub_trains);

    auto end2 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration2 = end2 - start2;

    float treeSize = 0;
    for (int i = 1; i <= MAX_LENGTH; i++)
    {
        treeSize += bptree.getTreeSize(multiTree[i]->sub_tree);
    }
    float subTreeSize = sizeof(subTree*);
    treeSize += subTreeSize * MAX_LENGTH / (1024 * 1024);
    cout << "Index size: " << treeSize << "MB" << endl;
    cout << "Index construction time: " << duration2.count() << "ms" << endl<< endl;
}

void LMBQ::HB_GF()
{
    auto start2 = chrono::high_resolution_clock::now();

    elemsBuckets = getElemsBucketsByZorder();
    //插入
    vector<Record*> insertData = getHalfTrans();

    int lastLen = data->vec_trans[0]->elems.size();
    int lastId = 0;
    for (int i = 0; i < data->vec_trans.size(); i++) {
        //generateKey(data->vec_trans[i]->elems, data->vec_trans[i]->key);
        generateKeyNoLen(data->vec_trans[i]->elems, data->vec_trans[i]->key);
        int length = data->vec_trans[i]->elems.size();
        if (length > lastLen)
        {
            for (int j = lastId; j < i; j++)
            {
                Record* newRecord = new Record(*data->vec_trans[j]);
                multiTree[lastLen]->sub_trains.push_back(newRecord);
            }
            //sort(multiTree[lastLen]->sub_trains.begin(), multiTree[lastLen]->sub_trains.end(), compareRecords);
            bptree.bulkLoadingInsert(multiTree[lastLen]->sub_tree, multiTree[lastLen]->sub_trains);
            lastId = i;
            lastLen = length;
        }
    }
    //处理最后的部分
    for (int j = lastId; j < data->vec_trans.size(); j++)
    {
        Record* newRecord = new Record(*data->vec_trans[j]);
        multiTree[lastLen]->sub_trains.push_back(newRecord);
    }
    bptree.bulkLoadingInsert(multiTree[lastLen]->sub_tree, multiTree[lastLen]->sub_trains);

    auto end2 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration2 = end2 - start2;

    float treeSize = 0;
    for (int i = 1; i <= MAX_LENGTH; i++)
    {
        treeSize += bptree.getTreeSize(multiTree[i]->sub_tree);
    }
    float subTreeSize = sizeof(subTree*);
    treeSize += subTreeSize * MAX_LENGTH / (1024 * 1024);
    cout << "Index of LeB: " << treeSize << "MB" << endl;
    cout << "Index Time: " << duration2.count() << "ms" << endl;
    cout << "Initialization successful !" << endl << endl;

    //插入
    InsertAfterBuild(insertData);
}

void LMBQ::HB_HF()
{
    auto start2 = chrono::high_resolution_clock::now();
    //插入
    vector<Record*> insertData = getHalfTrans();

    elemsBuckets = getElemsBucketsByZorderCountCF();

    int lastLen = data->vec_trans[0]->elems.size();
    int lastId = 0;
    for (int i = 0; i < data->vec_trans.size(); i++) {
        //generateKey(data->vec_trans[i]->elems, data->vec_trans[i]->key);
        generateKeyNoLen(data->vec_trans[i]->elems, data->vec_trans[i]->key);
        int length = data->vec_trans[i]->elems.size();
        if (length > lastLen)
        {
            for (int j = lastId; j < i; j++)
            {
                Record* newRecord = new Record(*data->vec_trans[j]);
                multiTree[lastLen]->sub_trains.push_back(newRecord);
            }
            //sort(multiTree[lastLen]->sub_trains.begin(), multiTree[lastLen]->sub_trains.end(), compareRecords);
            bptree.bulkLoadingInsert(multiTree[lastLen]->sub_tree, multiTree[lastLen]->sub_trains);
            lastId = i;
            lastLen = length;
        }
    }
    //处理最后的部分
    for (int j = lastId; j < data->vec_trans.size(); j++)
    {
        Record* newRecord = new Record(*data->vec_trans[j]);
        multiTree[lastLen]->sub_trains.push_back(newRecord);
    }
    bptree.bulkLoadingInsert(multiTree[lastLen]->sub_tree, multiTree[lastLen]->sub_trains);

    auto end2 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration2 = end2 - start2;

    float treeSize = 0;
    for (int i = 1; i <= MAX_LENGTH; i++)
    {
        treeSize += bptree.getTreeSize(multiTree[i]->sub_tree);
    }
    float subTreeSize = sizeof(subTree*);
    treeSize += subTreeSize * MAX_LENGTH / (1024 * 1024);
    cout << "Index of LeB: " << treeSize << "MB" << endl;
    cout << "Index Time: " << duration2.count() << "ms" << endl;
    cout << "Initialization successful !" << endl << endl;

    //插入
    InsertAfterBuild(insertData);
}

void LMBQ::CountSubtreesHeight() {
    int allHeight = 0;
    int maxTreeHeight = 0;
    int minTreeHeight = std::numeric_limits<int>::max(); // 初始化为最大可能的int值
    int n = 0;

    // 打开文件用于写入
    std::ofstream outFile("tree_heights_kosarak.txt");
    if (!outFile.is_open()) {
        std::cerr << "无法打开文件用于写入。" << std::endl;
        return; // 添加返回语句以避免进一步执行
    }

    for (int i = 1; i <= MAX_LENGTH; i++) {
        int currentHeight = bptree.getTreeHeight(multiTree[i]->sub_tree);
        if (currentHeight) {
            allHeight += currentHeight;
            n++;
        }

        // 写入当前长度和子树高度
        outFile << i << " " << currentHeight << '\n'; // 使用'\n'代替std::endl

        // 更新最大和最小子树高度
        if (currentHeight > maxTreeHeight) {
            maxTreeHeight = currentHeight;
        }
        if (currentHeight < minTreeHeight) {
            minTreeHeight = currentHeight;
        }
    }

    // 写入最大和最小子树高度
    outFile << "Max tree height: " << maxTreeHeight << '\n';
    outFile << "Min tree height: " << minTreeHeight << '\n';

    outFile << "MAX_LENGTH: " << MAX_LENGTH << '\n';
    outFile << "The number of non-empty subtrees: " << n << '\n';
    outFile << "The heights of all non-empty subtrees: " << allHeight << '\n';

    // 关闭文件
    outFile.close();
}

vector<Record*> LMBQ::getHalfTrans()
{
    std::vector<Record*> rRecords;
    int n = data->vec_trans.size() / 2; // 计算一半的大小

    // 创建一个随机数生成器
    std::random_device rd;
    std::mt19937 g(rd());

    // 打乱vec_trans以随机选择元素
    std::shuffle(data->vec_trans.begin(), data->vec_trans.end(), g);

    // 选择前一半的元素
    for (int i = 0; i < n; ++i) {
        rRecords.push_back(data->vec_trans[i]);
    }

    // 从vec_trans中删除这些元素
    data->vec_trans.erase(data->vec_trans.begin(), data->vec_trans.begin() + n);

    sort(data->vec_trans.begin(), data->vec_trans.end(), compareByLength);
    //sort(rRecords.begin(), rRecords.end(), compareByLength);
    return rRecords;
}

void LMBQ::InsertAfterBuild(vector<Record*> iRecords)
{
    auto start = chrono::high_resolution_clock::now();
    int n = iRecords.size();
    long int splitNum = 0;
    for (int i = 0; i < n; i++)
    {
        Record* iR = iRecords[i];
        int iRLength = iR->elems.size();
        generateKeyNoLen(iR->elems, iR->key);
        if (bptree.insertKey(multiTree[iRLength]->sub_tree, iR, &splitNum) == false)
        {
            cout << "insert error!" << iR->key << endl;
        }
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "Insert Time: " << duration.count() << "ms" << endl;
    cout << "split Num: " << splitNum << endl;

    float treeSize = 0;
    for (int i = 0; i <= MAX_LENGTH; i++)
    {
        treeSize += bptree.getTreeSize(multiTree[i]->sub_tree);
    }
    float subTreeSize = sizeof(subTree*);
    treeSize += subTreeSize * MAX_LENGTH / (1024 * 1024);
    cout << "Index of LeB: " << treeSize << "MB" << endl << endl;
}

int* LMBQ::getElemsBucketsByZorder() {

    map<int, int> countMap;

    for (int i = 0; i < data->vec_trans.size(); i++) {
        for (int j = 0; j < data->vec_trans[i]->elems.size(); j++) {
            countMap[data->vec_trans[i]->elems[j]]++;
        }
    }
    vector<pair<int, int>> countVec(countMap.begin(), countMap.end());
    sort(countVec.begin(), countVec.end(), compareByValue);
    int* elemBucket = new int[100000000];
    int bucketIndex = 0;
    bool reverse = false;

    for (int i = 0; i < countVec.size(); i++) {
        elemBucket[countVec[i].first] = bucketIndex;
        if (!reverse) {
            bucketIndex++;
            if (bucketIndex == BUCKET_NUMBER) {
                bucketIndex = BUCKET_NUMBER - 1;
                reverse = true;
            }
        }
        else {
            bucketIndex--;
            if (bucketIndex == -1) {
                bucketIndex = 0;
                reverse = false;
            }
        }
    }
    return elemBucket;
}

int* LMBQ::getElemsBucketsByZorderCountCF() {

    map<int, int> countMap;

    for (int i = 0; i < data->vec_trans.size(); i++) {
        for (int j = 0; j < data->vec_trans[i]->elems.size(); j++) {
            countMap[data->vec_trans[i]->elems[j]]++;
        }
    }
    vector<pair<int, int>> countVec(countMap.begin(), countMap.end());
    sort(countVec.begin(), countVec.end(), compareByValue);
    int* elemBucket = new int[100000000];
    int bucketIndex = 0;
    bool reverse = false;

    bucketsCumulativeFrequencies = new int[BUCKET_NUMBER];
    for (int i = 0; i < BUCKET_NUMBER; i++)
        bucketsCumulativeFrequencies[i] = 0;

    for (int i = 0; i < countVec.size(); i++) {
        elemBucket[countVec[i].first] = bucketIndex;
        bucketsCumulativeFrequencies[bucketIndex] += countVec[i].second;
        if (!reverse) {
            bucketIndex++;
            if (bucketIndex == BUCKET_NUMBER) {
                bucketIndex = BUCKET_NUMBER - 1;
                reverse = true;
            }
        }
        else {
            bucketIndex--;
            if (bucketIndex == -1) {
                bucketIndex = 0;
                reverse = false;
            }
        }
    }
    return elemBucket;
}

void LMBQ::generateKeyNoLen(vector<int> elems, unsigned long long& res_vec) {
    int countArr[BUCKET_NUMBER] = { 0 };
    int curLen = 0;
    unsigned long long word = 0;
    int blankNum = WORD_SIZE - maxBits * BUCKET_NUMBER;

    for (int i = 0; i < elems.size(); i++) {
        int no = elemsBuckets[elems[i]];
        if (no < 0) {//没有元素和桶的映射关系
            int minFC = bucketsCumulativeFrequencies[0];
            no = 0;
            for (int j = 1; j < BUCKET_NUMBER; j++)
            {
                if (bucketsCumulativeFrequencies[j] < minFC)
                {
                    minFC = bucketsCumulativeFrequencies[j];
                    no = j;
                }
            }
            elemsBuckets[elems[i]] = no;
            bucketsCumulativeFrequencies[no]++;
        }
        countArr[no]++;
    }

    for (int i = 0; i < BUCKET_NUMBER; i++) {
        if (curLen + maxBits <= WORD_SIZE) {
            word <<= maxBits;
            word |= countArr[i];
            curLen += maxBits;
        }
        else {
            int reminder = WORD_SIZE - curLen;
            word <<= reminder;
            word |= (countArr[i] >> (maxBits - reminder));
            res_vec = word;
            word = countArr[i] << (WORD_SIZE - maxBits + reminder) >> (WORD_SIZE - maxBits + reminder);
            curLen = maxBits - reminder;
        }
    }
    if (curLen != 0)
        res_vec = word << blankNum;
}

int* LMBQ::getCountBucketNumber(unsigned long long key) {
    int bucketBit = WORD_SIZE / BUCKET_NUMBER;
    unsigned long long mask = generateMask(bucketBit) << (WORD_SIZE - bucketBit);
    int* countBucketNumber = (int*)malloc(sizeof(int) * BUCKET_NUMBER);

    if (countBucketNumber == NULL) {
        return NULL;
    }
    for (int i = 0; i < BUCKET_NUMBER; i++) {
        int num = (key & mask) >> (WORD_SIZE - bucketBit * (i + 1));
        countBucketNumber[i] = num;
        mask >>= bucketBit;
    }
    return countBucketNumber;
}

unsigned long long LMBQ::getHeadKey(unsigned long long queryKey, int difValue, int currentLen) {
    int blankNum = WORD_SIZE - maxBits * BUCKET_NUMBER;
    int* countBucketNumber = getCountBucketNumber(queryKey);

    int n = 0;
    while (difValue > 0 && n < BUCKET_NUMBER) {
        if (countBucketNumber[n] >= difValue) {
            countBucketNumber[n] -= difValue;
            difValue = 0; // ?   difValue       
        }
        else {
            difValue -= countBucketNumber[n];
            countBucketNumber[n] = 0;
            n++;
        }
    }

    unsigned long long word = 0;
    int curLen = 0;
    for (int i = 0; i < BUCKET_NUMBER; i++) {
        word <<= maxBits;
        word |= countBucketNumber[i];
        curLen += maxBits;
    }
    free(countBucketNumber);

    return  word << blankNum;
}

unsigned long long LMBQ::getRearKey(unsigned long long queryKey, int difValue, int currentLen) {
    int blankNum = WORD_SIZE - maxBits * BUCKET_NUMBER;
    int* countBucketNumber = getCountBucketNumber(queryKey);

    int n = 0;
    int maxCount = pow(2, maxBits) - 1;
    while (difValue > 0 && n < BUCKET_NUMBER) {
        if (countBucketNumber[n] + difValue >= maxCount) {
            difValue = maxCount - countBucketNumber[n];
            countBucketNumber[n] = maxCount;
            n++;
        }
        else {
            countBucketNumber[n] += difValue;
            difValue = 0;
            break;
        }
    }

    unsigned long long word = 0;
    int curLen = 0;
    for (int i = 0; i < BUCKET_NUMBER; i++) {
        word <<= maxBits;
        word |= countBucketNumber[i];
        curLen += maxBits;
    }
    free(countBucketNumber);

    return  word << blankNum;
}

unsigned long long LMBQ::getHeadKeyNewAD(unsigned long long queryKey, int difValue) {
    int blankNum = WORD_SIZE  - maxBits * BUCKET_NUMBER;
    int* countBucketNumber = getCountBucketNumber(queryKey);

    int n = 0;
    while (difValue > 0 && n < BUCKET_NUMBER) {
        if (countBucketNumber[n] >= difValue) {
            countBucketNumber[n] -= difValue;
            difValue = 0; // 确保 difValue 被归零
        }
        else {
            difValue -= countBucketNumber[n];
            countBucketNumber[n] = 0;
            n++;
        }
    }

    unsigned long long word = 0;
    int curLen = 0;
    for (int i = 0; i < BUCKET_NUMBER; i++) {
        word <<= maxBits;
        word |= countBucketNumber[i];
        curLen += maxBits;
    }
    free(countBucketNumber);

    return  word << blankNum;
}

unsigned long long LMBQ::getRearKeyNewAD(unsigned long long queryKey, int difValue) {
    int blankNum = WORD_SIZE  - maxBits * BUCKET_NUMBER;
    int* countBucketNumber = getCountBucketNumber(queryKey);

    int n = 0;
    int maxCount = pow(2, maxBits) - 1;
    while (difValue > 0 && n < BUCKET_NUMBER) {
        if (countBucketNumber[n] + difValue >= maxCount) {
            difValue = difValue - (maxCount - countBucketNumber[n]);
            countBucketNumber[n] = maxCount;
            n++;
        }
        else {
            countBucketNumber[n] += difValue;
            difValue = 0;
            break;
        }
    }

    unsigned long long word = 0;
    int curLen = 0;
    for (int i = 0; i < BUCKET_NUMBER; i++) {
        word <<= maxBits;
        word |= countBucketNumber[i];
        curLen += maxBits;
    }
    free(countBucketNumber);

    return  word << blankNum;
}

unsigned long long LMBQ::getHeadKeyNew(unsigned long long queryKey, int queryLen, int difValue, int currentLen) {
    int blankNum = WORD_SIZE  - maxBits * BUCKET_NUMBER;
    int* countBucketNumber = getCountBucketNumber(queryKey);

    //首减
    int n = 0;
    int d = difValue;
    while (difValue > 0 && n < BUCKET_NUMBER) {
        if (countBucketNumber[n] >= difValue) {
            countBucketNumber[n] -= difValue;
            difValue = 0; // 确保 difValue 被归零
        }
        else {
            difValue -= countBucketNumber[n];
            countBucketNumber[n] = 0;
            n++;
        }
    }
    //尾加
    int lenDif = currentLen - queryLen + d;
    n = BUCKET_NUMBER - 1;
    int maxCount = pow(2, maxBits) - 1;
    while (lenDif > 0 && n >= 0) {
        if (countBucketNumber[n] + lenDif >= maxCount) {
            lenDif = lenDif - (maxCount - countBucketNumber[n]);
            countBucketNumber[n] = maxCount;
            n--;
        }
        else {
            countBucketNumber[n] += lenDif;
            lenDif = 0;
            break;
        }
    }

    unsigned long long word = 0;
    int curLen = 0;
    for (int i = 0; i < BUCKET_NUMBER; i++) {
        word <<= maxBits;
        word |= countBucketNumber[i];
        curLen += maxBits;
    }
    free(countBucketNumber);

    return  word << blankNum;
}

unsigned long long LMBQ::getRearKeyNew(unsigned long long queryKey, int queryLen, int difValue, int currentLen) {
    int blankNum = WORD_SIZE - maxBits * BUCKET_NUMBER;
    int* countBucketNumber = getCountBucketNumber(queryKey);

    //尾减
    int n = BUCKET_NUMBER - 1;
    int d = difValue;
    while (difValue > 0 && n >= 0) {
        if (countBucketNumber[n] >= difValue) {
            countBucketNumber[n] -= difValue;
            difValue = 0; // 确保 difValue 被归零
        }
        else {
            difValue -= countBucketNumber[n];
            countBucketNumber[n] = 0;
            n--;
        }
    }
    //首加
    n = 0;
    int maxCount = pow(2, maxBits) - 1;
    int lenDif = currentLen - queryLen + d;
    while (lenDif > 0 && n < BUCKET_NUMBER) {
        if (countBucketNumber[n] + lenDif >= maxCount) {
            lenDif = lenDif - (maxCount - countBucketNumber[n]);
            countBucketNumber[n] = maxCount;
            n++;
        }
        else {
            countBucketNumber[n] += lenDif;
            lenDif = 0;
            break;
        }
    }

    unsigned long long word = 0;
    int curLen = 0;
    for (int i = 0; i < BUCKET_NUMBER; i++) {
        word <<= maxBits;
        word |= countBucketNumber[i];
        curLen += maxBits;
    }
    free(countBucketNumber);

    return  word << blankNum;
}


Node* LMBQ::findHeadNode(unsigned long long searchKey, int curLen) {
    Node* searchNode = multiTree[curLen]->sub_tree;
    while (!searchNode->isLeaf) {
        for (int i = 0; i < searchNode->keys.size(); i++) {
            if (searchNode->keys[i]->key > searchKey) {
                searchNode = searchNode->pointers[i];
                break;
            }
            if (i == searchNode->keys.size() - 1) {
                searchNode = searchNode->pointers[i + 1];
                break;
            }
        }
    }
    return searchNode;
}

Node* LMBQ::findRearNode(unsigned long long searchKey, int curLen) {
    Node* searchNode = multiTree[curLen]->sub_tree;
    // 当当前节点不是叶子节点时，继续向下搜索
    while (!searchNode->isLeaf) {
        // 遍历当前节点的所有键
        for (int i = 0; i < searchNode->keys.size(); i++) {
            // 如果当前键大于搜索键，则移动到对应的子节点
            if (searchNode->keys[i]->key > searchKey) {
                searchNode = searchNode->pointers[i];
                break;
            }
            // 如果当前键是最后一个键，并且没有大于搜索键的键，则移动到最大的子节点
            if (i == searchNode->keys.size() - 1) {
                searchNode = searchNode->pointers[i + 1];
                break;
            }
        }
    }
    return searchNode;
}

double LMBQ::getJaccard(Record* rec, Record* que) {
    vector<int> intersection;
    for (int elem : rec->elems) {
        if (find(que->elems.begin(), que->elems.end(), elem) != que->elems.end()) {
            intersection.push_back(elem);
        }
    }
    double union_size = rec->elems.size() + que->elems.size() - intersection.size();
    return (double)intersection.size() / union_size;
}

bool LMBQ::getJaccard(Record* rec, Record* que, double delta) {
    int recLen = rec->elems.size();
    int queryLen = que->elems.size();
    int T = (int)ceil(delta * (queryLen + recLen) / (1 + delta));
    int commonCount = 0;
    int i = 0, j = 0;
    while (i < queryLen && j < recLen) {
        while (i < queryLen && que->elems[i] < rec->elems[j]) {
            i++;
        }
        if (i == queryLen)
            break;
        while (j<recLen && que->elems[i] > rec->elems[j])
            j++;
        if (j == recLen)
            break;
        if (que->elems[i] == rec->elems[j]) {
            commonCount++;
            if (commonCount >= T) {
                return true;
            }
            i++;
            j++;
        }
        int remainingCount = min(queryLen - i, recLen - j);
        if (commonCount + remainingCount < T) {
            return false;
        }
    }
    return commonCount >= T;
}

//对称差
vector<Record*> LMBQ::LMBQ_SD(double delta) {
    auto start = chrono::high_resolution_clock::now();
    long keyNum = 0;
    long long CandidateNum = 0;
    long long oneKey = 0;
    double kFudgeFactor = .00000001;
    vector<Record*> result;
    for (int i = 0; i < data->vec_querys.size(); i++) {
        Record* query = data->vec_querys[i];
        int queryLength = query->elems.size();
        int minL = ceil(queryLength * delta);
        int maxL = floor(queryLength / delta);
        if (maxL > MAX_LENGTH) {
            for (int j = 0; j < data->vec_longset.size(); j++)
            {
                int curLen = data->vec_longset[j]->elems.size();
                if (curLen < minL)
                    continue;
                if (curLen > maxL)
                    break;
                CandidateNum++;
                if (getJaccard(data->vec_longset[j], query) >= delta - kFudgeFactor)
                    result.push_back(data->vec_longset[j]);
            }
        }
        if (minL > MAX_LENGTH)
            continue;
        vector<vector<Record*>> pathsResults;
        generateKeyNoLen(query->elems, query->key);
        for (int currentLen = minL; currentLen <= maxL; currentLen++) {
            if (currentLen <= 0) {
                continue;
            }
            if (currentLen > MAX_LENGTH)
                break;
            if (!multiTree[currentLen]->sub_tree)
                continue;
            int currentDif = (int)floor((1 - delta) * (queryLength + currentLen) / (1 + delta));
            unsigned long long headKey = getHeadKey(query->key, currentDif, currentLen);
            unsigned long long rearKey = getRearKey(query->key, currentDif, currentLen);
            Node* headNode = findHeadNode(headKey, currentLen);
            Node* rearNode = findRearNode(rearKey, currentLen);
            Node* searchNode = headNode;
            int* bucketArrOfQueryKey = getCountBucketNumber(query->key);
            while (searchNode != nullptr) {
                for (int j = 0; j < searchNode->keys.size(); j++) {
                    
                    if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
                        oneKey++;
                        int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
                        bool isCandidate = true;
                        int countDif = 0;
                        for (int n = 0; n < BUCKET_NUMBER; n++) {
                            countDif += abs(bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n]);
                            if (countDif > currentDif) {
                                isCandidate = false;
                                break;
                            }
                        }
                        if (isCandidate) {
                            pathsResults.push_back(searchNode->paths[j]);
                        }
                    }
                }
                if (searchNode == rearNode) {
                    break;
                }
                else {
                    searchNode = searchNode->nextList;
                }
            }
        }
        for (int j = 0; j < pathsResults.size(); j++) {
            keyNum++;
            CandidateNum += pathsResults[j].size();
            for (int k = 0; k < pathsResults[j].size(); k++) {
                if (getJaccard(pathsResults[j][k], query) >= delta - kFudgeFactor) {
                    result.push_back(pathsResults[j][k]);
                }
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "Average query time: " << duration.count() / data->vec_querys.size() << " ms" << endl;
    cout << "Average key: " << (float)keyNum / data->vec_querys.size() << endl;
    cout << "Average candidate: " << (float)CandidateNum /data->vec_querys.size() << endl;
    cout << result.size() << endl;
    cout << "Average one key: "<< (float)oneKey / data->vec_querys.size() << endl;
    return result;
}

//对称差-验证
vector<Record*> LMBQ::LMBQ_SD_PV(double delta) {
    auto start = chrono::high_resolution_clock::now();
    long keyNum = 0;
    long long CandidateNum = 0;
    long long oneKey = 0;
    vector<Record*> result;
    for (int i = 0; i < data->vec_querys.size(); i++) {
        Record* query = data->vec_querys[i];
        int queryLength = query->elems.size();
        int minL = ceil(queryLength * delta);
        int maxL = floor(queryLength / delta);
        if (maxL > MAX_LENGTH) {
            for (int j = 0; j < data->vec_longset.size(); j++)
            {
                int curLen = data->vec_longset[j]->elems.size();
                if (curLen < minL)
                    continue;
                if (curLen > maxL)
                    break;
                CandidateNum++;
                if (getJaccard(data->vec_longset[j], query, delta))
                    result.push_back(data->vec_longset[j]);
            }
        }
        if (minL > MAX_LENGTH)
            continue;
        vector<vector<Record*>> pathsResults;
        generateKeyNoLen(query->elems, query->key);
        for (int currentLen = minL; currentLen <= maxL; currentLen++) {
            if (currentLen <= 0) {
                continue;
            }
            if (currentLen > MAX_LENGTH)
                break;
            if (!multiTree[currentLen]->sub_tree)
                continue;
            int currentDif = (int)floor((1 - delta) * (queryLength + currentLen) / (1 + delta));
            unsigned long long headKey = getHeadKey(query->key, currentDif, currentLen);
            unsigned long long rearKey = getRearKey(query->key, currentDif, currentLen);
            Node* headNode = findHeadNode(headKey, currentLen);
            Node* rearNode = findRearNode(rearKey, currentLen);
            Node* searchNode = headNode;
            int* bucketArrOfQueryKey = getCountBucketNumber(query->key);
            while (searchNode != nullptr) {
                for (int j = 0; j < searchNode->keys.size(); j++) {
                    
                    if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
                        oneKey++;
                        int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
                        bool isCandidate = true;
                        int countDif = 0;
                        for (int n = 0; n < BUCKET_NUMBER; n++) {
                            countDif += abs(bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n]);
                            if (countDif > currentDif) {
                                isCandidate = false;
                                break;
                            }
                        }
                        if (isCandidate) {
                            pathsResults.push_back(searchNode->paths[j]);
                        }
                    }
                }
                if (searchNode == rearNode) {
                    break;
                }
                else {
                    searchNode = searchNode->nextList;
                }
            }
        }
        for (int j = 0; j < pathsResults.size(); j++) {
            keyNum++;
            CandidateNum += pathsResults[j].size();
            for (int k = 0; k < pathsResults[j].size(); k++) {
                if (getJaccard(pathsResults[j][k], query, delta)) {
                    result.push_back(pathsResults[j][k]);
                }
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "Average query time: " << duration.count() / data->vec_querys.size() << " ms" << endl;
    cout << "Average key: " << (float)keyNum / data->vec_querys.size() << endl;
    cout << "Average candidate: " << (float)CandidateNum / data->vec_querys.size() << endl;
    cout << result.size() << endl;
    cout << "Average one key: " << (float)oneKey / data->vec_querys.size() << endl;
    return result;
}

//对称差-没有过滤策略
vector<Record*> LMBQ::LMBQ_SD_PV_NONE(double delta) {
    auto start = chrono::high_resolution_clock::now();
    long keyNum = 0;
    long long CandidateNum = 0;
    long long oneKey = 0;
    double kFudgeFactor = .00000001;
    vector<Record*> result;
    for (int i = 0; i < data->vec_querys.size(); i++) {
        Record* query = data->vec_querys[i];
        int queryLength = query->elems.size();
        int minL = ceil(queryLength * delta);
        int maxL = floor(queryLength / delta);
        if (maxL > MAX_LENGTH) {
            for (int j = 0; j < data->vec_longset.size(); j++)
            {
                int curLen = data->vec_longset[j]->elems.size();
                if (curLen < minL)
                    continue;
                if (curLen > maxL)
                    break;
                CandidateNum++;
                //if (getJaccard(data->vec_longset[j], query) >= delta - kFudgeFactor)
                if (getJaccard(data->vec_longset[j], query, delta))
                    result.push_back(data->vec_longset[j]);
            }
        }
        if (minL > MAX_LENGTH)
            continue;
        vector<vector<Record*>> pathsResults;
        generateKeyNoLen(query->elems, query->key);
        for (int currentLen = minL; currentLen <= maxL; currentLen++) {
            if (currentLen <= 0) {
                continue;
            }
            if (currentLen > MAX_LENGTH)
                break;
            if (!multiTree[currentLen]->sub_tree)
                continue;
            int currentDif = (int)floor((1 - delta) * (queryLength + currentLen) / (1 + delta));
            unsigned long long headKey = getHeadKey(query->key, currentDif, currentLen);
            unsigned long long rearKey = getRearKey(query->key, currentDif, currentLen);
            Node* headNode = findHeadNode(headKey, currentLen);
            Node* rearNode = findRearNode(rearKey, currentLen);
            Node* searchNode = headNode;
            int* bucketArrOfQueryKey = getCountBucketNumber(query->key);
            while (searchNode != nullptr) {
                for (int j = 0; j < searchNode->keys.size(); j++) {

                    if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
                        oneKey++;
                        pathsResults.push_back(searchNode->paths[j]);
                    }
                }
                if (searchNode == rearNode) {
                    break;
                }
                else {
                    searchNode = searchNode->nextList;
                }
            }
        }
        for (int j = 0; j < pathsResults.size(); j++) {
            keyNum++;
            CandidateNum += pathsResults[j].size();
            for (int k = 0; k < pathsResults[j].size(); k++) {
                //if (getJaccard(pathsResults[j][k], query) >= delta - kFudgeFactor) 
                if (getJaccard(pathsResults[j][k], query, delta))
                {
                    result.push_back(pathsResults[j][k]);
                }
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "Average query time: " << duration.count() / data->vec_querys.size() << " ms" << endl;
    cout << "Average key: " << (float)keyNum / data->vec_querys.size() << endl;
    cout << "Average candidate: " << (float)CandidateNum / data->vec_querys.size() << endl;
    cout << result.size() << endl;
    cout << "Average one key: " << (float)oneKey / data->vec_querys.size() << endl;
    return result;
}

//SIGIR的非对称差
vector<Record*> LMBQ::LMBQ_UD(double delta) {

    auto start = chrono::high_resolution_clock::now();
    double kFudgeFactor = .00000001;
    long keyNum = 0;
    long long oneKey = 0;
    vector<Record*> result;
    long long CandidateNum = 0;
    for (int i = 0; i < data->vec_querys.size(); i++) {
        Record* query = data->vec_querys[i];
        int queryLength = query->elems.size();
        int minL = ceil(queryLength * delta);
        int maxL = floor(queryLength / delta);
        if (maxL > MAX_LENGTH) {
            for (int j = 0; j < data->vec_longset.size(); j++)
            {
                int curLen = data->vec_longset[j]->elems.size();
                if (curLen < minL)
                    continue;
                if (curLen > maxL)
                    break;
                CandidateNum++;
                if (getJaccard(data->vec_longset[j], query) >= delta - kFudgeFactor)
                    result.push_back(data->vec_longset[j]);
            }
        }
        if (minL > MAX_LENGTH)
            continue;
        vector<vector<Record*>> pathsResults;
        generateKeyNoLen(query->elems, query->key);
        for (int currentLen = minL; currentLen <= maxL; currentLen++) {
            if (currentLen <= 0) {
                continue;
            }
            if (currentLen > MAX_LENGTH)
                break;
            if (!multiTree[currentLen]->sub_tree)
                continue;
            //int currentDif = (int)floor((1 - delta) * (queryLength + currentLen) / (1 + delta)); //对称差
            int currentDif = (int)floor(((queryLength - delta * currentLen) / (1 + delta)) + kFudgeFactor);
            int currentDifTwo = (int)floor(((currentLen - delta * queryLength) / (1 + delta)) + kFudgeFactor);
            unsigned long long headKey = getHeadKeyNewAD(query->key, currentDif);
            unsigned long long rearKey = getRearKeyNewAD(query->key, currentDifTwo);

            Node* headNode = findHeadNode(headKey, currentLen);//查找叶节点，访问节点数就是树的高度
            Node* rearNode = findRearNode(rearKey, currentLen);//查找叶节点，访问节点数就是树的高度
            Node* searchNode = headNode;
            int* bucketArrOfQueryKey = getCountBucketNumber(query->key);
            while (searchNode != nullptr) {//访问的叶节点的数量可能不同
                for (int j = 0; j < searchNode->keys.size(); j++) {
                    
                    if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
                        oneKey++;
                        int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
                        bool isCandidate = true;
                        int countDif = 0;
                        int countDifTwo = 0;
                        for (int n = 0; n < BUCKET_NUMBER; n++) {
                            if (bucketArrOfQueryKey[n] == bucketArrOfLeafKey[n])
                                continue;
                            else if (bucketArrOfQueryKey[n] > bucketArrOfLeafKey[n])
                            {
                                countDif += bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n];
                                if (countDif > currentDif) {
                                    isCandidate = false;
                                    break;
                                }
                            }
                            else
                            {
                                countDifTwo += bucketArrOfLeafKey[n] - bucketArrOfQueryKey[n];
                                if (countDifTwo > currentDifTwo) {
                                    isCandidate = false;
                                    break;
                                }
                            }
                        }
                        if (isCandidate) {
                            pathsResults.push_back(searchNode->paths[j]);
                        }
                    }
                }
                if (searchNode == rearNode) {
                    break;
                }
                else {
                    searchNode = searchNode->nextList;
                }
            }
        }
        for (int j = 0; j < pathsResults.size(); j++) {
            keyNum++;
            CandidateNum += pathsResults[j].size();
            for (int k = 0; k < pathsResults[j].size(); k++) {
                if (getJaccard(pathsResults[j][k], query) >= delta - kFudgeFactor) {
                    result.push_back(pathsResults[j][k]);
                }
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "Average query time: " << duration.count() / data->vec_querys.size() << " ms" << endl;
    cout << "Average key: " << (float)keyNum / data->vec_querys.size() << endl;
    cout << "Average candidate: " << (float)CandidateNum / data->vec_querys.size() << endl;
    cout << result.size() << endl;
    cout << "Average one key: " << (float)oneKey / data->vec_querys.size() << endl;
    return result;
}

//SIGIR的非对称差-验证
vector<Record*> LMBQ::LMBQ_UD_PV(double delta) {

    auto start = chrono::high_resolution_clock::now();
    double kFudgeFactor = .00000001;
    long keyNum = 0;
    long oneKey = 0;
    vector<Record*> result;
    long long CandidateNum = 0;
    for (int i = 0; i < data->vec_querys.size(); i++) {
        Record* query = data->vec_querys[i];
        int queryLength = query->elems.size();
        int minL = ceil(queryLength * delta);
        int maxL = floor(queryLength / delta);
        if (maxL > MAX_LENGTH) {
            for (int j = 0; j < data->vec_longset.size(); j++)
            {
                int curLen = data->vec_longset[j]->elems.size();
                if (curLen < minL)
                    continue;
                if (curLen > maxL)
                    break;
                CandidateNum++;
                if (getJaccard(data->vec_longset[j], query, delta))
                    result.push_back(data->vec_longset[j]);
            }
        }
        if (minL > MAX_LENGTH)
            continue;
        vector<vector<Record*>> pathsResults;
        generateKeyNoLen(query->elems, query->key);
        for (int currentLen = minL; currentLen <= maxL; currentLen++) {
            if (currentLen <= 0) {
                continue;
            }
            if (currentLen > MAX_LENGTH)
                break;
            if (!multiTree[currentLen]->sub_tree)
                continue;
            //int currentDif = (int)floor((1 - delta) * (queryLength + currentLen) / (1 + delta));//对称差
            int currentDif = (int)floor(((queryLength - delta * currentLen) / (1 + delta)) + kFudgeFactor);
            int currentDifTwo = (int)floor(((currentLen - delta * queryLength) / (1 + delta)) + kFudgeFactor);
            unsigned long long headKey = getHeadKeyNewAD(query->key, currentDif);
            unsigned long long rearKey = getRearKeyNewAD(query->key, currentDifTwo);

            Node* headNode = findHeadNode(headKey, currentLen);//查找叶节点，访问节点数就是树的高度
            Node* rearNode = findRearNode(rearKey, currentLen);//查找叶节点，访问节点数就是树的高度
            Node* searchNode = headNode;
            int* bucketArrOfQueryKey = getCountBucketNumber(query->key);
            while (searchNode != nullptr) {//访问的叶节点的数量可能不同
                for (int j = 0; j < searchNode->keys.size(); j++) {
                    
                    if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
                        oneKey++;
                        int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
                        bool isCandidate = true;
                        int countDif = 0;
                        int countDifTwo = 0;
                        for (int n = 0; n < BUCKET_NUMBER; n++) {
                            if (bucketArrOfQueryKey[n] == bucketArrOfLeafKey[n])
                                continue;
                            else if (bucketArrOfQueryKey[n] > bucketArrOfLeafKey[n])
                            {
                                countDif += bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n];
                                if (countDif > currentDif) {
                                    isCandidate = false;
                                    break;
                                }
                            }
                            else
                            {
                                countDifTwo += bucketArrOfLeafKey[n] - bucketArrOfQueryKey[n];
                                if (countDifTwo > currentDifTwo) {
                                    isCandidate = false;
                                    break;
                                }
                            }
                        }
                        if (isCandidate) {
                            pathsResults.push_back(searchNode->paths[j]);
                        }
                    }
                }
                if (searchNode == rearNode) {
                    break;
                }
                else {
                    searchNode = searchNode->nextList;
                }
            }
        }
        for (int j = 0; j < pathsResults.size(); j++) {
            keyNum++;
            CandidateNum += pathsResults[j].size();
            for (int k = 0; k < pathsResults[j].size(); k++) {
                if (getJaccard(pathsResults[j][k], query, delta)) {
                    result.push_back(pathsResults[j][k]);
                }
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "Average query time: " << duration.count() / data->vec_querys.size() << " ms" << endl;
    cout << "Average key: " << (float)keyNum / data->vec_querys.size() << endl;
    cout << "Average candidate: " << (float)CandidateNum / data->vec_querys.size() << endl;
    cout << result.size() << endl;
    cout << "Average one key: " << (float)oneKey / data->vec_querys.size() << endl;
    return result;
}

//单向差
vector<Record*> LMBQ::LMBQ_Test(double delta) {

    auto start = chrono::high_resolution_clock::now();
    double kFudgeFactor = .00000001;
    long keyNum = 0;
    long oneKey = 0;
    vector<Record*> result;
    long long CandidateNum = 0;
    for (int i = 0; i < data->vec_querys.size(); i++) {
        Record* query = data->vec_querys[i];
        int queryLength = query->elems.size();
        int minL = ceil(queryLength * delta);
        int maxL = floor(queryLength / delta);
        if (maxL > MAX_LENGTH) {
            for (int j = 0; j < data->vec_longset.size(); j++)
            {
                int curLen = data->vec_longset[j]->elems.size();
                if (curLen < minL)
                    continue;
                if (curLen > maxL)
                    break;
                CandidateNum++;
                if (getJaccard(data->vec_longset[j], query) >= delta - kFudgeFactor)
                    result.push_back(data->vec_longset[j]);
            }
        }
        if (minL > MAX_LENGTH)
            continue;
        vector<vector<Record*>> pathsResults;
        generateKeyNoLen(query->elems, query->key);
        for (int currentLen = minL; currentLen <= maxL; currentLen++) {
            if (currentLen <= 0) {
                continue;
            }
            if (currentLen > MAX_LENGTH)
                break;
            if (!multiTree[currentLen]->sub_tree)
                continue;
            int currentDif = (int)floor(((queryLength - delta * currentLen) / (1 + delta)) + kFudgeFactor);
            unsigned long long headKey = getHeadKeyNew(query->key, queryLength, currentDif, currentLen);
            unsigned long long rearKey = getRearKeyNew(query->key, queryLength, currentDif, currentLen);

            Node* headNode = findHeadNode(headKey, currentLen);//查找叶节点，访问节点数就是树的高度
            Node* rearNode = findRearNode(rearKey, currentLen);//查找叶节点，访问节点数就是树的高度
            Node* searchNode = headNode;
            int* bucketArrOfQueryKey = getCountBucketNumber(query->key);
            while (searchNode != nullptr) {//访问的叶节点的数量可能不同
                for (int j = 0; j < searchNode->keys.size(); j++) {

                    if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
                        oneKey++;
                        int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
                        bool isCandidate = true;
                        int countDif = 0;
                        for (int n = 0; n < BUCKET_NUMBER; n++) {
                            countDif += max(bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n], 0);
                            if (countDif > currentDif) {
                                isCandidate = false;
                                break;
                            }
                        }
                        if (isCandidate) {
                            pathsResults.push_back(searchNode->paths[j]);
                        }
                    }
                }
                if (searchNode == rearNode) {
                    break;
                }
                else {
                    searchNode = searchNode->nextList;
                }
            }
        }
        for (int j = 0; j < pathsResults.size(); j++) {
            keyNum++;
            CandidateNum += pathsResults[j].size();
            for (int k = 0; k < pathsResults[j].size(); k++) {
                if (getJaccard(pathsResults[j][k], query) >= delta - kFudgeFactor) {
                    result.push_back(pathsResults[j][k]);
                }
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "Average query time: " << duration.count() / data->vec_querys.size() << " ms" << endl;
    cout << "Average key: " << (float)keyNum / data->vec_querys.size() << endl;
    cout << "Average candidate: " << (float)CandidateNum / data->vec_querys.size() << endl;
    cout << result.size() << endl;
    cout << "Average one key: " << (float)oneKey / data->vec_querys.size() << endl;
    return result;
}

//单向差-验证
vector<Record*> LMBQ::LMBQ_Test_PV(double delta) {

    auto start = chrono::high_resolution_clock::now();
    double kFudgeFactor = .00000001;
    long keyNum = 0;
    long oneKey = 0;
    vector<Record*> result;
    long long CandidateNum = 0;
    for (int i = 0; i < data->vec_querys.size(); i++) {
        Record* query = data->vec_querys[i];
        int queryLength = query->elems.size();
        int minL = ceil(queryLength * delta);
        int maxL = floor(queryLength / delta);
        if (maxL > MAX_LENGTH) {
            for (int j = 0; j < data->vec_longset.size(); j++)
            {
                int curLen = data->vec_longset[j]->elems.size();
                if (curLen < minL)
                    continue;
                if (curLen > maxL)
                    break;
                CandidateNum++;
                if (getJaccard(data->vec_longset[j], query, delta))
                    result.push_back(data->vec_longset[j]);
            }
        }
        if (minL > MAX_LENGTH)
            continue;
        vector<vector<Record*>> pathsResults;
        generateKeyNoLen(query->elems, query->key);
        for (int currentLen = minL; currentLen <= maxL; currentLen++) {
            if (currentLen <= 0) {
                continue;
            }
            if (currentLen > MAX_LENGTH)
                break;
            if (!multiTree[currentLen]->sub_tree)
                continue;
            int currentDif = (int)floor(((queryLength - delta * currentLen) / (1 + delta)) + kFudgeFactor);
            unsigned long long headKey = getHeadKeyNew(query->key, queryLength, currentDif, currentLen);
            unsigned long long rearKey = getRearKeyNew(query->key, queryLength, currentDif, currentLen);

            Node* headNode = findHeadNode(headKey, currentLen);//查找叶节点，访问节点数就是树的高度
            Node* rearNode = findRearNode(rearKey, currentLen);//查找叶节点，访问节点数就是树的高度
            Node* searchNode = headNode;
            int* bucketArrOfQueryKey = getCountBucketNumber(query->key);
            while (searchNode != nullptr) {//访问的叶节点的数量可能不同
                for (int j = 0; j < searchNode->keys.size(); j++) {
                    
                    if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
                        oneKey++;
                        int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
                        bool isCandidate = true;
                        int countDif = 0;
                        for (int n = 0; n < BUCKET_NUMBER; n++) {
                            countDif += max(bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n], 0);
                            if (countDif > currentDif) {
                                isCandidate = false;
                                break;
                            }
                        }
                        if (isCandidate) {
                            pathsResults.push_back(searchNode->paths[j]);
                        }
                    }
                }
                if (searchNode == rearNode) {
                    break;
                }
                else {
                    searchNode = searchNode->nextList;
                }
            }
        }
        for (int j = 0; j < pathsResults.size(); j++) {
            keyNum++;
            CandidateNum += pathsResults[j].size();
            for (int k = 0; k < pathsResults[j].size(); k++) {
                if (getJaccard(pathsResults[j][k], query, delta)) {
                    result.push_back(pathsResults[j][k]);
                }
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "Average query time: " << duration.count() / data->vec_querys.size() << " ms" << endl;
    cout << "Average key: " << (float)keyNum / data->vec_querys.size() << endl;
    cout << "Average candidate: " << (float)CandidateNum / data->vec_querys.size() << endl;
    cout << result.size() << endl;
    cout << "Average one key: " << (float)oneKey / data->vec_querys.size() << endl;
    return result;
}

//单向差-没有过滤策略
vector<Record*> LMBQ::LMBQ_Test_PV_NONE(double delta) {

    auto start = chrono::high_resolution_clock::now();
    double kFudgeFactor = .00000001;
    long keyNum = 0;
    long oneKey = 0;
    vector<Record*> result;
    long long CandidateNum = 0;
    for (int i = 0; i < data->vec_querys.size(); i++) {
        Record* query = data->vec_querys[i];
        int queryLength = query->elems.size();
        int minL = ceil(queryLength * delta);
        int maxL = floor(queryLength / delta);
        if (maxL > MAX_LENGTH) {
            for (int j = 0; j < data->vec_longset.size(); j++)
            {
                int curLen = data->vec_longset[j]->elems.size();
                if (curLen < minL)
                    continue;
                if (curLen > maxL)
                    break;
                CandidateNum++;
                //if (getJaccard(data->vec_longset[j], query) >= delta - kFudgeFactor)
                if (getJaccard(data->vec_longset[j], query, delta))
                    result.push_back(data->vec_longset[j]);
            }
        }
        if (minL > MAX_LENGTH)
            continue;
        vector<vector<Record*>> pathsResults;
        generateKeyNoLen(query->elems, query->key);
        for (int currentLen = minL; currentLen <= maxL; currentLen++) {
            if (currentLen <= 0) {
                continue;
            }
            if (currentLen > MAX_LENGTH)
                break;
            if (!multiTree[currentLen]->sub_tree)
                continue;
            int currentDif = (int)floor(((queryLength - delta * currentLen) / (1 + delta)) + kFudgeFactor);
            unsigned long long headKey = getHeadKeyNew(query->key, queryLength, currentDif, currentLen);
            unsigned long long rearKey = getRearKeyNew(query->key, queryLength, currentDif, currentLen);

            Node* headNode = findHeadNode(headKey, currentLen);//查找叶节点，访问节点数就是树的高度
            Node* rearNode = findRearNode(rearKey, currentLen);//查找叶节点，访问节点数就是树的高度
            Node* searchNode = headNode;
            int* bucketArrOfQueryKey = getCountBucketNumber(query->key);
            while (searchNode != nullptr) {//访问的叶节点的数量可能不同
                for (int j = 0; j < searchNode->keys.size(); j++) {

                    if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
                        oneKey++;
                        pathsResults.push_back(searchNode->paths[j]);
                    }
                }
                if (searchNode == rearNode) {
                    break;
                }
                else {
                    searchNode = searchNode->nextList;
                }
            }
        }
        for (int j = 0; j < pathsResults.size(); j++) {
            keyNum++;
            CandidateNum += pathsResults[j].size();
            for (int k = 0; k < pathsResults[j].size(); k++) {
                //if (getJaccard(data->vec_longset[j], query) >= delta - kFudgeFactor)
                if (getJaccard(pathsResults[j][k], query, delta)) 
                {
                    result.push_back(pathsResults[j][k]);
                }
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "Average query time: " << duration.count() / data->vec_querys.size() << " ms" << endl;
    cout << "Average key: " << (float)keyNum / data->vec_querys.size() << endl;
    cout << "Average candidate: " << (float)CandidateNum / data->vec_querys.size() << endl;
    cout << result.size() << endl;
    cout << "Average one key: " << (float)oneKey / data->vec_querys.size() << endl;
    return result;
}


bool LMBQ::Cosine(Record* que, Record* rec, double delta)
{
    vector<int> intersection;
    for (int elem : rec->elems) {
        if (find(que->elems.begin(), que->elems.end(), elem) != que->elems.end()) {
            intersection.push_back(elem);
        }
    }
    double union_size = sqrt(rec->elems.size()) * sqrt(que->elems.size());
    double t = (double)intersection.size() / union_size;
    return t >= delta;
}

bool LMBQ::Dice(Record* que, Record* rec, double delta)
{
    vector<int> intersection;
    for (int elem : rec->elems) {
        if (find(que->elems.begin(), que->elems.end(), elem) != que->elems.end()) {
            intersection.push_back(elem);
        }
    }
    double union_size = rec->elems.size() + que->elems.size();
    double t = 2 * (double)intersection.size() / union_size;
    return t >= delta;
}

//vector<Record*> LMBQ::LMBQ_SD_Cosine(double delta) {
//    auto start = chrono::high_resolution_clock::now();
//    long keyNum = 0;
//    vector<Record*> result;
//    long long CandidateNum = 0;
//    for (int i = 0; i < data->vec_querys.size(); i++) {
//        Record* query = data->vec_querys[i];
//        int queryLength = query->elems.size();
//        int minL = (int)ceil(delta * delta * queryLength);
//        int maxL = (int)floor(queryLength / (delta * delta));;
//        if (maxL > MAX_LENGTH) {
//            for (int j = 0; j < data->vec_longset.size(); j++)
//            {
//                int curLen = data->vec_longset[j]->elems.size();
//                if (curLen < minL)
//                    continue;
//                if (curLen > maxL)
//                    break;
//                CandidateNum++;
//                if (Cosine(data->vec_longset[j], query, delta))
//                    result.push_back(data->vec_longset[j]);
//            }
//        }
//        if (minL > MAX_LENGTH)
//            continue;
//        vector<vector<Record*>> pathsResults;
//        generateKeyNoLen(query->elems, query->key);
//        for (int currentLen = minL; currentLen <= maxL; currentLen++) {
//            if (currentLen <= 0) {
//                continue;
//            }
//            if (currentLen > MAX_LENGTH)
//                break;
//            if (!multiTree[currentLen]->sub_tree)
//                continue;
//            int currentDif = (int)floor(queryLength + currentLen - 2 * delta * sqrt(currentLen * queryLength));
//            unsigned long long headKey = getHeadKey(query->key, currentDif, currentLen);
//            unsigned long long rearKey = getRearKey(query->key, currentDif, currentLen);
//            Node* headNode = findHeadNode(headKey, currentLen);
//            Node* rearNode = findRearNode(rearKey, currentLen);
//            Node* searchNode = headNode;
//            int* bucketArrOfQueryKey = getCountBucketNumber(query->key);
//            while (searchNode != nullptr) {
//                for (int j = 0; j < searchNode->keys.size(); j++) {
//                    if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
//                        int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
//                        bool isCandidate = true;
//                        int countDif = 0;
//                        for (int n = 0; n < BUCKET_NUMBER; n++) {
//                            countDif += abs(bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n]);
//                            if (countDif > currentDif) {
//                                isCandidate = false;
//                                break;
//                            }
//                        }
//                        if (isCandidate) {
//                            pathsResults.push_back(searchNode->paths[j]);
//                        }
//                    }
//                }
//                if (searchNode == rearNode) {
//                    break;
//                }
//                else {
//                    searchNode = searchNode->nextList;
//                }
//            }
//        }
//        for (int j = 0; j < pathsResults.size(); j++) {
//            keyNum++;
//            CandidateNum += pathsResults[j].size();
//            for (int k = 0; k < pathsResults[j].size(); k++) {
//                if (Cosine(pathsResults[j][k], query, delta)) {
//                    result.push_back(pathsResults[j][k]);
//                }
//            }
//        }
//    }
//    auto end = chrono::high_resolution_clock::now();
//    chrono::duration<double, milli> duration = end - start;
//    cout << "Average query time: " << duration.count() / data->vec_querys.size() << " ms" << endl;
//    cout << "Average key number: " << (float)keyNum / data->vec_querys.size() << endl;
//    cout << "KeyNum: " << keyNum << endl;
//    cout << result.size() << endl;
//    cout << "CandidateNum: " << CandidateNum << endl;
//    return result;
//}

//vector<Record*> LMBQ::LeBQP_CBDF_Dice(double delta) {
//    auto start = chrono::high_resolution_clock::now();
//    long keyNum = 0;
//    vector<Record*> result;
//    long long CandidateNum = 0;
//    for (int i = 0; i < data->vec_querys.size(); i++) {
//        Record* query = data->vec_querys[i];
//        int queryLength = query->elems.size();
//        int minL = (int)ceil(delta * queryLength / (2 - delta));
//        int maxL = (int)floor(queryLength * (2 - delta) / delta);
//        if (maxL > MAX_LENGTH) {
//            for (int j = 0; j < data->vec_longset.size(); j++)
//            {
//                int curLen = data->vec_longset[j]->elems.size();
//                if (curLen < minL)
//                    continue;
//                if (curLen > maxL)
//                    break;
//                CandidateNum++;
//                if (Dice(data->vec_longset[j], query, delta))
//                    result.push_back(data->vec_longset[j]);
//            }
//        }
//        if (minL > MAX_LENGTH)
//            continue;
//        vector<vector<Record*>> pathsResults;
//        generateKeyNoLen(query->elems, query->key);
//        for (int currentLen = minL; currentLen <= maxL; currentLen++) {
//            if (currentLen <= 0) {
//                continue;
//            }
//            if (currentLen > MAX_LENGTH)
//                break;
//            if (!multiTree[currentLen]->sub_tree)
//                continue;
//            int currentDif = (int)floor((queryLength + currentLen) * (1 - delta));
//            unsigned long long headKey = getHeadKey(query->key, currentDif, currentLen);
//            unsigned long long rearKey = getRearKey(query->key, currentDif, currentLen);
//            Node* headNode = findHeadNode(headKey, currentLen);
//            Node* rearNode = findRearNode(rearKey, currentLen);
//            Node* searchNode = headNode;
//            int* bucketArrOfQueryKey = getCountBucketNumber(query->key);
//            while (searchNode != nullptr) {
//                for (int j = 0; j < searchNode->keys.size(); j++) {
//                    if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
//                        int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
//                        bool isCandidate = true;
//                        int countDif = 0;
//                        for (int n = 0; n < BUCKET_NUMBER; n++) {
//                            countDif += abs(bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n]);
//                            if (countDif > currentDif) {
//                                isCandidate = false;
//                                break;
//                            }
//                        }
//                        if (isCandidate) {
//                            pathsResults.push_back(searchNode->paths[j]);
//                        }
//                    }
//                }
//                if (searchNode == rearNode) {
//                    break;
//                }
//                else {
//                    searchNode = searchNode->nextList;
//                }
//            }
//        }
//        for (int j = 0; j < pathsResults.size(); j++) {
//            keyNum++;
//            CandidateNum += pathsResults[j].size();
//            for (int k = 0; k < pathsResults[j].size(); k++) {
//                if (Dice(pathsResults[j][k], query, delta)) {
//                    result.push_back(pathsResults[j][k]);
//                }
//            }
//        }
//    }
//    auto end = chrono::high_resolution_clock::now();
//    chrono::duration<double, milli> duration = end - start;
//    cout << "Average query time: " << duration.count() / data->vec_querys.size() << " ms" << endl;
//    cout << "Average key number: " << (float)keyNum / data->vec_querys.size() << endl;
//    cout << "KeyNum: " << keyNum << endl;
//    cout << result.size() << endl;
//    cout << "CandidateNum: " << CandidateNum << endl;
//    return result;
//}


//==============================更新==========================

void LMBQ::insertQuery()
{
    int longSetSign = 0;
    for (int i = 0; i < data->vec_querys.size(); i++) {
        Record* query = data->vec_querys[i];
        int queryLength = query->elems.size();
        if (queryLength > MAX_LENGTH)
        {
            data->vec_longset.push_back(query);
            longSetSign = 1;
            continue;
        }
        generateKeyNoLen(query->elems, query->key);
        //leb.insertKey(multiTree[queryLength]->sub_tree, query);
    }
    if (longSetSign)
    {
        sort(data->vec_longset.begin(), data->vec_longset.end(), compareByLength);
    }
}

vector<Record*> LMBQ::searchAfterInsert(double delta)
{
    //插入
    auto startInsert = chrono::high_resolution_clock::now();
    insertQuery();
    auto endInsert = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> durationInsert = endInsert - startInsert;
    cout << "Average insert time: " << durationInsert.count() / data->vec_querys.size() << " ms" << endl;

    //插入后查询
    long keyNum = 0;
    vector<Record*> result;
    //    int allCandidate = 0;
    auto startSearch = chrono::high_resolution_clock::now();
    for (int i = 0; i < data->vec_querys.size(); i++) {
        Record* query = data->vec_querys[i];
        int queryLength = query->elems.size();
        int minL = ceil(queryLength * delta);
        int maxL = ceil(queryLength / delta);
        if (maxL > MAX_LENGTH) {
            for (int j = 0; j < data->vec_longset.size(); j++)
            {
                int curLen = data->vec_longset[j]->elems.size();
                if (curLen < minL)
                    continue;
                if (curLen > maxL)
                    break;
                if (getJaccard(data->vec_longset[j], query, delta))
                    result.push_back(data->vec_longset[j]);
            }
        }
        if (minL > MAX_LENGTH)
            continue;
        vector<vector<Record*>> pathsResults;
        //        int lengthCandidates = 0;
        for (int currentLen = minL; currentLen <= maxL; currentLen++) {
            if (currentLen <= 0) {
                continue;
            }
            if (currentLen > MAX_LENGTH)
                break;
            if (!multiTree[currentLen]->sub_tree)
                continue;
            int currentDif = (int)floor((1 - delta) * (queryLength + currentLen) / (1 + delta));
            if (currentDif < 0)
                continue;
            unsigned long long headKey = getHeadKey(query->key, currentDif, currentLen);
            unsigned long long rearKey = getRearKey(query->key, currentDif, currentLen);
            //            outFile<<query->key<<" "<<headKey<<" "<<rearKey<<" "<<currentLen<<" "<<currentDif<<" ";
            Node* headNode = findHeadNode(headKey, currentLen);
            Node* rearNode = findRearNode(rearKey, currentLen);
            Node* searchNode = headNode;
            //            int firstCandidate = 0;
            int* bucketArrOfQueryKey = getCountBucketNumber(query->key);

            while (searchNode != nullptr) {
                for (int j = 0; j < searchNode->keys.size(); j++) {
                    if (searchNode->keys[j]->key > rearKey)
                        break;
                    if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
                        int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
                        bool isCandidate = true;
                        int countDif = 0;
                        //                        firstCandidate++;
                        for (int n = 0; n < BUCKET_NUMBER; n++) {
                            countDif += abs(bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n]);
                            if (countDif > currentDif) {
                                isCandidate = false;
                                break;
                            }
                        }
                        if (isCandidate) {
                            pathsResults.push_back(searchNode->paths[j]);
                        }
                    }
                }
                if (searchNode == rearNode) {
                    break;
                }
                else {
                    searchNode = searchNode->nextList;
                }
            }
            //            outFile<<firstCandidate<<endl;
            //            lengthCandidates += firstCandidate;
        }
        //        outFile<<"The key all candidate: "<<lengthCandidates<<endl;
        //        allCandidate += lengthCandidates;
        /*outFile2 <<query->key<<" ";
        int flag = false;*/
        for (int j = 0; j < pathsResults.size(); j++) {
            keyNum++;
            /*flag = false;*/
            for (int k = 0; k < pathsResults[j].size(); k++) {
                if (getJaccard(pathsResults[j][k], query, delta)) {
                    result.push_back(pathsResults[j][k]);
                    /*flag = true;*/
                }
                // if (getJaccard(pathsResults[j][k], query, delta)) {
                //     result.push_back(pathsResults[j][k]);
                // }
            }
            /*if(flag)
                outFile2<<pathsResults[j][0]->key<<" ";*/
        }
        /*outFile2<<endl;*/
    }
    //    outFile<<"All key the candidate: "<<allCandidate<<endl;
    //    outFile.close();
    //    outFile2.close();
    auto endSearch = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> durationSearch = endSearch - startSearch;
    cout << "Average query time: " << durationSearch.count() / data->vec_querys.size() << " ms" << endl;
    cout << "Average key number: " << (float)keyNum / data->vec_querys.size() << endl;
    cout << result.size() << endl;
    return result;
}

//vector<Record*> LeBQ::LeBQP_CBDF_New(double delta) {
//    //ofstream outFile("queryTableK.txt"); //          ?     д  
//    //if (!outFile) {
//    //    cerr << " ?    ?     д ?" << endl;
//    //    exit(0);
//    //}
//    auto start = chrono::high_resolution_clock::now();
//    double kFudgeFactor = .00000001;
//    long keyNum = 0;
//    vector<Record*> result;
//    //    int allCandidate = 0;
//    double indexTime = 0;
//    for (int i = 0; i < data->vec_querys.size(); i++) {
//        Record* query = data->vec_querys[i];
//        generateKeyNoLen(query->elems, query->key);
//        int queryLength = query->elems.size();
//        int* bucketArrOfQueryKey = getCountBucketNumber(query->key);
//
//        if (queryLength / delta > MAX_LENGTH) {
//            int t = (int)floor(queryLength * delta);
//            int minLen = t > MAX_LENGTH ? t : MAX_LENGTH;
//            int maxLen = (int)floor(queryLength / delta);
//            for (int j = 0; j < data->vec_longset.size(); j++) {
//                int recLen = data->vec_longset[j]->elems.size();
//                if (recLen < minLen || recLen > maxLen)
//                    continue;
//                //                if (getJaccard(data->vec_longset[j], query) >= delta - kFudgeFactor)   //?    
//                if (getJaccard(data->vec_longset[j], query, delta))
//                    result.push_back(data->vec_longset[j]);
//            }
//        }
//        if (queryLength * delta > MAX_LENGTH)
//            continue;
//        vector<vector<Record*>> pathsResults;
//        int dif = (int)floor((1 - delta) * queryLength / delta);
//        //        int lengthCandidates = 0;
//        for (int currentLen = queryLength - dif; currentLen <= queryLength + dif; currentLen++) {
//            if (currentLen < queryLength && (double)currentLen / queryLength < delta) {
//                continue;
//            }
//            if (currentLen <= 0) {
//                continue;
//            }
//            if (currentLen > MAX_LENGTH)
//                break;
//            int currentDif = (int)ceil(queryLength >= currentLen ? queryLength * delta : currentLen * delta);
//            currentDif = queryLength - currentDif;
//            if (currentDif < 0)
//                continue;
//            unsigned long long headKey = getHeadKeyNew(query->key, queryLength, currentDif, currentLen);
//            unsigned long long rearKey = getRearKeyNew(query->key, queryLength, currentDif, currentLen);
//            //            outFile<<query->key<<" "<<headKey<<" "<<rearKey<<" "<<currentLen<<" "<<currentDif<<" ";
//
//            Node* headNode = findHeadNode(headKey);
//            Node* rearNode = findRearNode(rearKey);
//            Node* searchNode = headNode;
//            currentDif = (int)floor((1 - delta) * (queryLength + currentLen) / (1 + delta));
//            //            int firstCandidate = 0;
//
//            while (searchNode != nullptr)
//            {
//                for (int j = 0; j < searchNode->keys.size(); j++) {
//                    if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
//                        int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
//                        bool isCandidate = true;
//                        int countDif = 0;
//                        //firstCandidate++;
//                        for (int n = 0; n < BUCKET_NUMBER; n++) {
//                            countDif += abs(bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n]);
//                            if (countDif > currentDif) {
//                                isCandidate = false;
//                                break;
//                            }
//                        }
//                        if (isCandidate) {
//                            pathsResults.push_back(searchNode->paths[j]);
//                        }
//                    }
//                }
//                if (searchNode == rearNode) {
//                    break;
//                }
//                else {
//                    searchNode = searchNode->nextList;
//                }
//            }
//
//            //            outFile<<firstCandidate<<endl;
//            //            lengthCandidates += firstCandidate;
//        }
//        //        outFile<<"The key all candidate: "<<lengthCandidates<<endl;
//        //        allCandidate += lengthCandidates;
//        /*outFile <<query->key<<" ";*/
//        /*auto startIndex = std::chrono::high_resolution_clock::now();*/
//        int flag = false;
//        for (int j = 0; j < pathsResults.size(); j++) {
//            keyNum++;
//            flag = false;
//            for (int k = 0; k < pathsResults[j].size(); k++) {
//                //                if (getJaccard(pathsResults[j][k], query) >= delta - kFudgeFactor)
//                if (getJaccard(pathsResults[j][k], query, delta))
//                {
//                    result.push_back(pathsResults[j][k]);
//                    flag = true;
//                }
//            }
//            /*if(flag)
//                outFile<<pathsResults[j][0]->key<<" ";*/
//        }
//        /*auto endIndex = std::chrono::high_resolution_clock::now();
//        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endIndex - startIndex);
//        indexTime += duration.count();*/
//        /*outFile<<endl;*/
//    }
//    //    outFile<<"All key the candidate: "<<allCandidate<<endl;
//    /*outFile.close();*/
//
//    auto end = chrono::high_resolution_clock::now();
//    chrono::duration<double, milli> duration = end - start;
//    cout << "Average query time: " << duration.count() / data->vec_querys.size() << " ms" << endl;
//    cout << "Average key number: " << (float)keyNum / data->vec_querys.size() << endl;
//    cout << result.size() << endl;
//    /* cout << "Average index time: " << indexTime / data->vec_querys.size() << " ms" << endl;*/
//    return result;
//}
//
//
//vector<Record*> LeBQ::LeBQP_CBDF_SingQuery(Record* query, double delta) {
//    ofstream outFile2("query582-1.txt"); //          ?     д  
//
//    long keyNum = 0;
//    vector<Record*> result;
//    //    int allCandidate = 0;
//    generateKeyNoLen(query->elems, query->key);
//    int queryLength = query->elems.size();
//    int* bucketArrOfQueryKey = getCountBucketNumber(query->key);
//
//    //if (queryLength / delta > MAX_LENGTH) {
//    //    int t = (int)floor(queryLength * delta);
//    //    int minLen = t > MAX_LENGTH ? t : MAX_LENGTH;
//    //    int maxLen = (int)floor(queryLength / delta);
//    //    for (int j = 0; j < data->vec_longset.size(); j++) {
//    //        int recLen = data->vec_longset[j]->elems.size();
//    //        if (recLen < minLen || recLen > maxLen)
//    //            continue;
//    //        //                if (getJaccard(data->vec_longset[j], query) >= delta - kFudgeFactor)   //?    
//    //        if (getJaccard(data->vec_longset[j], query, delta))
//    //            result.push_back(data->vec_longset[j]);
//    //    }
//    //}
//    //if (queryLength * delta > MAX_LENGTH)
//    //    return result;
//    vector<vector<Record*>> pathsResults;
//    int dif = (int)floor((1 - delta) * queryLength / delta);
//    //        int lengthCandidates = 0;
//    for (int currentLen = queryLength - dif; currentLen <= queryLength + dif; currentLen++) {
//        if (currentLen < queryLength && (double)currentLen / queryLength < delta) {
//            continue;
//        }
//        if (currentLen <= 0) {
//            continue;
//        }
//        if (currentLen > MAX_LENGTH)
//            break;
//        int currentDif = (int)ceil(queryLength >= currentLen ? queryLength * delta : currentLen * delta);
//        currentDif = queryLength - currentDif;
//        if (currentDif < 0)
//            continue;
//        unsigned long long headKey = getHeadKeyNew(query->key, queryLength, currentDif, currentLen);
//        unsigned long long rearKey = getRearKeyNew(query->key, queryLength, currentDif, currentLen);
//        outFile2 << query->key << " " << headKey << " " << rearKey << " " << currentLen << " " << currentDif << endl;
//        Node* headNode = findHeadNode(headKey);
//        Node* rearNode = findRearNode(rearKey);
//        Node* searchNode = headNode;
//        currentDif = (int)floor((1 - delta) * (queryLength + currentLen) / (1 + delta));
//        while (searchNode != nullptr)
//        {
//            for (int j = 0; j < searchNode->keys.size(); j++) {
//                if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
//                    int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
//                    bool isCandidate = true;
//                    int countDif = 0;
//                    for (int n = 0; n < BUCKET_NUMBER; n++) {
//                        countDif += abs(bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n]);
//                        if (countDif > currentDif) {
//                            isCandidate = false;
//                            break;
//                        }
//                    }
//                    if (isCandidate) {
//                        pathsResults.push_back(searchNode->paths[j]);
//                    }
//                }
//            }
//            if (searchNode == rearNode) {
//                break;
//            }
//            else {
//                searchNode = searchNode->nextList;
//            }
//        }
//    }
//    bool kflag = false;
//    for (int j = 0; j < pathsResults.size(); j++) {
//        keyNum++;
//        for (int k = 0; k < pathsResults[j].size(); k++) {
//            //                if (getJaccard(pathsResults[j][k], query) >= delta - kFudgeFactor)
//            if (getJaccard(pathsResults[j][k], query, delta))
//            {
//                result.push_back(pathsResults[j][k]);
//                outFile2 << pathsResults[j][k]->key << endl;
//                if (pathsResults[j][k]->key == 216172816473538561)
//                {
//                    outFile2 << endl;
//                    for (int s = 0; s < pathsResults[j][k]->elems.size(); s++)
//                        outFile2 << pathsResults[j][k]->elems[s] << " ";
//                    outFile2 << endl;
//                }
//            }
//        }
//    }
//    outFile2.close();
//    return result;
//}

void LMBQ::testBInsert() {
    BPtree bTree;
    Node* root = nullptr;
    bTree.treeOrder = 3;
    bTree.minN = (bTree.treeOrder - 1) / 2;
    bTree.maxN = bTree.treeOrder - 1;

    elemsBuckets = getElemsBucketsByZorder();

    for (int i = 0; i < data->vec_trans.size(); i++) {
        generateKeyNoLen(data->vec_trans[i]->elems, data->vec_trans[i]->key);
    }

    bTree.bulkLoadingInsert(root, data->vec_trans);

    // 打印初始树结构
    std::cout << "Initial B+ Tree Structure:" << std::endl;
    bTree.printTree(root, 0);

    // 插入新记录
    for (int i = 0; i < data->vec_querys.size(); i++)
    {
        Record* newRecord = data->vec_querys[i];
        if (newRecord->elems.size() > MAX_LENGTH)
        {
            data->vec_longset.push_back(newRecord);
            sort(data->vec_longset.begin(), data->vec_longset.end(), compareByLength);
            continue;
        }
        generateKeyNoLen(data->vec_querys[i]->elems, data->vec_querys[i]->key);
        //bTree.insertKey(root, newRecord);
        std::cout << "\nB+ Tree Structure after insert " << data->vec_querys[i]->key << ":" << std::endl;
        bTree.printTree(root, 0);
    }
}

void LMBQ::testBDelete()
{
    BPtree bTree;
    Node* root = nullptr;
    bTree.treeOrder = 3;
    bTree.minN = (bTree.treeOrder - 1) / 2;
    bTree.maxN = bTree.treeOrder - 1;

    elemsBuckets = getElemsBucketsByZorder();
    for (int i = 0; i < data->vec_trans.size(); i++) {
        generateKeyNoLen(data->vec_trans[i]->elems, data->vec_trans[i]->key);
    }

    bTree.bulkLoadingInsert(root, data->vec_trans);

    // 打印初始树结构
    std::cout << "Initial B+ Tree Structure:" << std::endl;
    bTree.printTree(root, 0);

    // 删除记录
    for (int i = 0; i < data->vec_querys.size(); i++)
    {

        Record* newRecord = data->vec_querys[i];
        if (newRecord->elems.size() > MAX_LENGTH)
        {
            for (int j = 0; j < data->vec_longset.size(); j++)
            {
                if (judgeLongRecords(data->vec_longset[j], newRecord) == 0)
                {
                    data->vec_longset.erase(data->vec_longset.begin() + j);
                    break;
                }
            }
            continue;
        }
        generateKeyNoLen(data->vec_querys[i]->elems, data->vec_querys[i]->key);
        std::cout << "\nB+ Tree Structure after delete " << data->vec_querys[i]->key << ":" << std::endl;
        bTree.deleteKey(root, newRecord);
        bTree.printTree(root, 0);
    }
}

void LMBQ::deleteQuery()
{
    for (int i = 0; i < data->vec_querys.size(); i++) {
        Record* query = data->vec_querys[i];
        int queryLength = query->elems.size();
        if (queryLength > MAX_LENGTH)
        {
            for (int j = 0; j < data->vec_longset.size(); j++)
            {
                if (judgeLongRecords(data->vec_longset[j], query) == 0)
                {
                    data->vec_longset.erase(data->vec_longset.begin() + j);
                    break;
                }
            }
            continue;
        }
        generateKeyNoLen(query->elems, query->key);
        bptree.deleteKey(multiTree[queryLength]->sub_tree, query);
    }
}

vector<Record*> LMBQ::searchAfterDelete(double delta)
{
    auto startDelete = chrono::high_resolution_clock::now();
    deleteQuery();
    auto endDelete = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> durationDelete = endDelete - startDelete;
    cout << "Average delete time: " << durationDelete.count() / data->vec_querys.size() << " ms" << endl;

    //插入后查询
    long keyNum = 0;
    vector<Record*> result;
    //    int allCandidate = 0;
    auto startSearch = chrono::high_resolution_clock::now();
    for (int i = 0; i < data->vec_querys.size(); i++) {
        Record* query = data->vec_querys[i];
        int queryLength = query->elems.size();
        int minL = ceil(queryLength * delta);
        int maxL = ceil(queryLength / delta);
        if (maxL > MAX_LENGTH) {
            for (int j = 0; j < data->vec_longset.size(); j++)
            {
                int curLen = data->vec_longset[j]->elems.size();
                if (curLen < minL)
                    continue;
                if (curLen > maxL)
                    break;
                if (getJaccard(data->vec_longset[j], query, delta))
                    result.push_back(data->vec_longset[j]);
            }
        }
        if (minL > MAX_LENGTH)
            continue;
        vector<vector<Record*>> pathsResults;
        //        int lengthCandidates = 0;
        for (int currentLen = minL; currentLen <= maxL; currentLen++) {
            if (currentLen <= 0) {
                continue;
            }
            if (currentLen > MAX_LENGTH)
                break;
            if (!multiTree[currentLen]->sub_tree)
                continue;
            int currentDif = (int)floor((1 - delta) * (queryLength + currentLen) / (1 + delta));
            if (currentDif < 0)
                continue;
            unsigned long long headKey = getHeadKey(query->key, currentDif, currentLen);
            unsigned long long rearKey = getRearKey(query->key, currentDif, currentLen);
            //            outFile<<query->key<<" "<<headKey<<" "<<rearKey<<" "<<currentLen<<" "<<currentDif<<" ";
            Node* headNode = findHeadNode(headKey, currentLen);
            Node* rearNode = findRearNode(rearKey, currentLen);
            Node* searchNode = headNode;
            //            int firstCandidate = 0;
            int* bucketArrOfQueryKey = getCountBucketNumber(query->key);

            while (searchNode != nullptr) {
                for (int j = 0; j < searchNode->keys.size(); j++) {
                    if (searchNode->keys[j]->key > rearKey)
                        break;
                    if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
                        int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
                        bool isCandidate = true;
                        int countDif = 0;
                        //                        firstCandidate++;
                        for (int n = 0; n < BUCKET_NUMBER; n++) {
                            countDif += abs(bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n]);
                            if (countDif > currentDif) {
                                isCandidate = false;
                                break;
                            }
                        }
                        if (isCandidate) {
                            pathsResults.push_back(searchNode->paths[j]);
                        }
                    }
                }
                if (searchNode == rearNode) {
                    break;
                }
                else {
                    searchNode = searchNode->nextList;
                }
            }
            //            outFile<<firstCandidate<<endl;
            //            lengthCandidates += firstCandidate;
        }
        //        outFile<<"The key all candidate: "<<lengthCandidates<<endl;
        //        allCandidate += lengthCandidates;
        /*outFile2 <<query->key<<" ";
        int flag = false;*/
        for (int j = 0; j < pathsResults.size(); j++) {
            keyNum++;
            /*flag = false;*/
            for (int k = 0; k < pathsResults[j].size(); k++) {
                if (getJaccard(pathsResults[j][k], query, delta)) {
                    result.push_back(pathsResults[j][k]);
                    /*flag = true;*/
                }
                // if (getJaccard(pathsResults[j][k], query, delta)) {
                //     result.push_back(pathsResults[j][k]);
                // }
            }
            /*if(flag)
                outFile2<<pathsResults[j][0]->key<<" ";*/
        }
        /*outFile2<<endl;*/
    }
    //    outFile<<"All key the candidate: "<<allCandidate<<endl;
    //    outFile.close();
    //    outFile2.close();
    auto endSearch = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> durationSearch = endSearch - startSearch;
    cout << "Average query time: " << durationSearch.count() / data->vec_querys.size() << " ms" << endl;
    cout << "Average key number: " << (float)keyNum / data->vec_querys.size() << endl;
    cout << result.size() << endl;
    return result;
}

vector<Record*> LMBQ::LeBQP_CBDF_SingQuery(Record* query, double delta)
{
    vector<Record*> result;
    int queryLength = query->elems.size();
    int minL = ceil(queryLength * delta);
    int maxL = ceil(queryLength / delta);
    if (maxL > MAX_LENGTH) {
        for (int j = 0; j < data->vec_longset.size(); j++)
        {
            int curLen = data->vec_longset[j]->elems.size();
            if (curLen < minL)
                continue;
            if (curLen > maxL)
                break;
            if (getJaccard(data->vec_longset[j], query, delta))
                result.push_back(data->vec_longset[j]);
        }
    }
    if (minL > MAX_LENGTH)
        return result;
    vector<vector<Record*>> pathsResults;
    generateKeyNoLen(query->elems, query->key);
    for (int currentLen = minL; currentLen <= maxL; currentLen++) {
        if (currentLen <= 0) {
            continue;
        }
        if (currentLen > MAX_LENGTH)
            break;
        if (!multiTree[currentLen]->sub_tree)
            continue;
        int currentDif = (int)floor((1 - delta) * (queryLength + currentLen) / (1 + delta));
        if (currentDif < 0)
            continue;
        unsigned long long headKey = getHeadKey(query->key, currentDif, currentLen);
        unsigned long long rearKey = getRearKey(query->key, currentDif, currentLen);

        Node* headNode = findHeadNode(headKey, currentLen);
        Node* rearNode = findRearNode(rearKey, currentLen);
        Node* searchNode = headNode;
        int* bucketArrOfQueryKey = getCountBucketNumber(query->key);

        while (searchNode != nullptr) {
            for (int j = 0; j < searchNode->keys.size(); j++) {
                if (searchNode->keys[j]->key > rearKey)
                    break;
                if (searchNode->keys[j]->key >= headKey && searchNode->keys[j]->key <= rearKey) {
                    int* bucketArrOfLeafKey = getCountBucketNumber(searchNode->keys[j]->key);
                    bool isCandidate = true;
                    int countDif = 0;
                    //                        firstCandidate++;
                    for (int n = 0; n < BUCKET_NUMBER; n++) {
                        countDif += abs(bucketArrOfQueryKey[n] - bucketArrOfLeafKey[n]);
                        if (countDif > currentDif) {
                            isCandidate = false;
                            break;
                        }
                    }
                    if (isCandidate) {
                        pathsResults.push_back(searchNode->paths[j]);
                    }
                }
            }
            if (searchNode == rearNode) {
                break;
            }
            else {
                searchNode = searchNode->nextList;
            }
        }
    }
    int flag = 0;
    double kFudgeFactor = .00000001;
    for (int j = 0; j < pathsResults.size(); j++) {
        if (pathsResults[j][0]->key == 1083691919051589131)
        {
            cout << pathsResults[j][0]->key << endl;
            flag = 1;
        }
        for (int k = 0; k < pathsResults[j].size(); k++) {
            /*if (flag)
            {
                for (int s = 0; s < pathsResults[j][k]->elems.size(); s++)
                {
                    cout << pathsResults[j][k]->elems[s] << " ";
                }
                cout << endl;
                flag = 0;
            }*/
            //if (getJaccard(pathsResults[j][k], query, delta))
            if (getJaccard(pathsResults[j][k], query) >= delta - kFudgeFactor)
            {
                result.push_back(pathsResults[j][k]);
                if (flag)
                {
                    cout << "Yes\n";
                    cout << pathsResults[j][k]->elems.size() << endl;
                    for (int s = 0; s < pathsResults[j][k]->elems.size(); s++)
                    {
                        cout << pathsResults[j][k]->elems[s] << " ";
                    }
                    cout << endl;
                }
            }
        }
        flag = 0;
    }
    cout << result.size() << endl;
    return result;
}
