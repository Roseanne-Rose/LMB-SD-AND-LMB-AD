#include "BPtree.h"

void BPtree::bulkLoadingInsert(Node*& tree, std::vector<Record*> data) {  //用于将一组记录批量插入到B树中
	sort(data.begin(), data.end(), compareRecords);
	size_t count = 0;    /// 用于记录已处理的记录数量
	vector<Node*> nodes;  // 存储创建的叶节点
	vector<KeyType> upperKeys;// 存储非叶节点中的键
	upperKeys.clear();
	vector<Node*> upperPointers;// 存储非叶节点的子节点指针
	upperPointers.clear();
	// 循环处理所有记录
	while (count < data.size()) {
		Node* temporaryNode = new Node(treeOrder, true); // 创建一个新的叶节点
		temporaryNode->paths.clear();
		try {
			KeyType tempKey;
			tempKey = data[count];   // 获取当前记录
			// 循环直到叶节点的键数目满足最小值   或   全部记录处理完毕
			while (temporaryNode->numberOfKeys < (treeOrder - 1) / 2 && count < data.size()) {
				tempKey = data[count];
				// 如果当前节点不是空的，并且新键不大于最后一个键
				if (temporaryNode->numberOfKeys > 0 && !(data[count]->key >
					temporaryNode->keys[temporaryNode->numberOfKeys - 1]->key)) {
					//temporaryNode->paths[temporaryNode->paths.size() - 1] += "-" + table[count][0];
					temporaryNode->paths[temporaryNode->paths.size() - 1].push_back(tempKey);   // 将记录添加到最后一个键的倒排列表中
				}
				else {//否则，创建一个新的键和其倒排列表，并放到叶节点中
					vector<KeyType> newVector;
					temporaryNode->keys.push_back(tempKey);
					newVector.push_back(tempKey);
					temporaryNode->paths.push_back(newVector);
					temporaryNode->numberOfKeys++;
				}
				count++;//处理下一个记录
			}
			//此时应该处理完一个节点（即节点的键数量大于最小值）  或 全部记录处理完毕（最后一个叶节点可能不满足最小值）
			if (nodes.size() > 0) {//叶节点的前后指针
				nodes[nodes.size() - 1]->nextList = temporaryNode;
				temporaryNode->lastList = nodes[nodes.size() - 1];
			}
			nodes.push_back(temporaryNode);//nodes为空，则直接push

			//查看后面的记录是否可以放进当前节点的倒排列表中
			while (count < data.size() && nodes.size() > 0 && !(data[count]->key > nodes[nodes.size() - 1]
				->keys[nodes[nodes.size() - 1]->numberOfKeys - 1]->key)) {
				tempKey = data[count];
				nodes[nodes.size() - 1]->paths[temporaryNode->paths.size() - 1].push_back(tempKey);
				count++;
			}
		}
		catch (...) {
			cout << "[Exception] function 'bulkloading': error during tree construct. this "
				"column doesn't exist in line :"
				<< count << "\n";
			return;
		}
	}
	//此时处理完所有记录，所有记录均存放在叶节点中，并且每个叶节点满足最小值，最后一个叶节点可能不满足
	// 如果第一个节点不满，后面肯定是没有节点了，则直接设置为根节点
	if (nodes[0]->numberOfKeys < (treeOrder - 1) / 2) {
		tree = nodes[0];
		tree->parent = NULL;
		return;
	}
	// 如果最后一个节点不满，则将其合并到前一个节点
	if (nodes[nodes.size() - 1]->numberOfKeys < (treeOrder - 1) / 2) {
		for (int i = 0; i < nodes[nodes.size() - 1]->numberOfKeys; i++) {
			nodes[nodes.size() - 2]->keys.push_back(nodes[nodes.size() - 1]->keys[i]);
			nodes[nodes.size() - 2]->paths.push_back(
				nodes[nodes.size() - 1]->paths[i]);
			nodes[nodes.size() - 2]->numberOfKeys++;
		}
		nodes[nodes.size() - 2]->nextList = NULL;  //设置指针
		nodes.erase(nodes.begin() + nodes.size() - 1);  //删除最后一个节点
	}
	// 合并完后，如果只有一个节点，则直接设置为根节点，没问题，因为每个叶节点都是满足最小值，但是没有满，就算合并了，也是不会超界的
	if (nodes.size() == 1) {
		tree = nodes[0];
		tree->parent = NULL;
		return;
	}
	// 准备非叶节点的键和指针
	for (size_t i = 1; i < nodes.size(); i++) {
		upperKeys.push_back(nodes[i]->keys[0]);//从  一  开始处理，因为  零  不用存储到非叶节点
		upperPointers.push_back(nodes[i - 1]);//零  的指针需要存储的
	}
	upperPointers.push_back(nodes[nodes.size() - 1]);//存放最后一个指针
	// 调用 upFirsts 来创建非叶节点
	upFirsts(upperKeys, upperPointers, treeOrder, tree);
}

//递归函数，自下而上创建非叶节点
void BPtree::upFirsts(std::vector<KeyType>& keys, std::vector<Node*>& pointers, int treeOrder, Node*& tree) {
	vector<KeyType> upperKeys; // 存储上级非叶节点的键
	vector<Node*> upperPointers; // 存储上级非叶节点的子节点指针
	vector<Node*> level; // 存储当前层的非叶节点

	size_t count = 0;//计数
	// 循环直到处理完所有的键
	while (count < keys.size()) {
		Node* temporaryNode = new Node(treeOrder); // 创建一个新的非叶节点
		//// 循环直到当前非叶节点的键数目满足最小值   或   全部键处理完毕
		while (temporaryNode->numberOfKeys < (treeOrder - 1) / 2 &&
			count < keys.size()) {
			temporaryNode->keys.push_back(keys[count]); // 将键key[count]和对应的指针添加到节点中
			temporaryNode->pointers.push_back(pointers[count]);//这是键key[count]的左指针
			pointers[count]->parent = temporaryNode;// 设置子节点的父节点
			temporaryNode->numberOfKeys++;
			count++;
		}

		pointers[count]->parent = temporaryNode;// 设置最后一个指针的父节点
		temporaryNode->pointers.push_back(pointers[count]);// 将最后一个指针添加到当前非叶节点的指针列表中，因为指针比键数目第一个，但是key[count]还没有处理
		level.push_back(temporaryNode);// 将节点添加到当前级别的列表中

		// 如果剩余的键不可以放入最后一个节点中，则准备分裂，只进行一次判断和处理
		if (keys.size() - count > ((size_t)treeOrder - 1) / 2) {
			upperKeys.push_back(keys[count]);// 添加中间键到上级节点的键列表中，key[count]需要放到上级节点中
			upperPointers.push_back(level[level.size() - 1]);// 添加当前节点到上级节点的指针列表中
			count++;
		}
		else {//剩余的键可以放入最后一个节点中
			// 处理剩余的键和指针
			while (count < keys.size()) {
				level[level.size() - 1]->keys.push_back(keys[count]);// 将剩余的键添加到最后一个节点中
				level[level.size() - 1]->numberOfKeys++;
				level[level.size() - 1]->pointers.push_back(pointers[count + 1]); // 将剩余的指针添加到最后一个节点的指针列表中
				pointers[count + 1]->parent = level[level.size() - 1];// 设置子节点的父节点
				count++;
			}
		}
	}

	upperPointers.push_back(level[level.size() - 1]);

	// 如果当前层只创建了一个节点，则它是根节点
	if (level.size() == 1) {
		tree = level[0];
		tree->parent = NULL;
		return;
	}
	// 递归调用 upFirsts 来创建更高级别的节点
	upFirsts(upperKeys, upperPointers, treeOrder, tree);
}

float BPtree::getTreeSize(Node* tree) {
	if (!tree) return 0;
	float result = 0;
	queue<Node*> q;
	q.push(tree);
	while (!q.empty()) {
		Node* current = q.front();
		q.pop();

		//   ?         
		if (!current->isLeaf) {
			result += 8;//父指针
			result += current->keys.size() * 8;
			result += current->pointers.size() * 8;
			for (Node* child : current->pointers) {
				if (child) {
					q.push(child);
				}
			}
		}
		else {
			result += 8 * 3;//父指针，前后指针
			result += current->keys.size() * 8 * 2;
			for (int i = 0; i < current->paths.size(); i++) {
				result += current->paths[i].size() * 8;
			}
		}
	}
	return result / (1024 * 1024);
}

int BPtree::getTreeHeight(Node* tree) {
	if (!tree) return 0; // 空树的高度为0

	int height = 0;
	Node* current = tree;

	// 从根节点向下走到叶子节点
	while (!current->isLeaf) {
		current = current->pointers[0]; // 选择第一个子节点向下走
		height++;
	}

	// 加上叶子节点的高度
	return height + 1;
}

Node* BPtree::findLeafNode(Node* T, unsigned long long searchKey) {
	if (T == nullptr) {
		return nullptr; // 处理空树的情况
	}

	Node* searchNode = T;
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

//============================================插入=================================

// 在 B+ 树中插入一个 Record* 数据并更新树结构
bool BPtree::insertKey(Node*& tree, KeyType key, long int* splitNumPtr) {
	if (!tree)
	{
		tree = new Node(treeOrder, true); // 假设最后一个参数表示是否是叶子节点
		if (!tree)
		{
			cout << "Build new tree root node failed!\n";
			return false;
		}
		vector<KeyType> newVector;
		tree->keys.push_back(key);
		newVector.push_back(key);
		tree->paths.push_back(newVector);
		tree->parent = NULL;
		tree->numberOfKeys = 1;
		return true;
	}
	// 1. 查找正确的叶子节点以插入新记录
	Node* leafNode = findLeafNode(tree, key->key); // 假设我们从根节点开始查找
	if (!leafNode)
	{
		cout << "Cannot find the leaf node need to inserted!\n";
		return false;
	}

	// 2. 插入新记录到叶子节点
	bool inserted = false;
	for (int i = 0; i < leafNode->keys.size(); ++i) {

		if (leafNode->keys[i]->key == key->key) {
			//// 如果找到相同键的记录，则将新记录添加到 paths 中
			//leafNode->paths[i].push_back(key);
			//inserted = true;
			//return true;
			try {
				// 如果找到相同键的记录，则将新记录添加到 paths 中
				leafNode->paths[i].push_back(key);
				inserted = true;
				return true;
			}
			catch (std::bad_alloc&) {
				// 处理内存不足的情况
				std::cerr << "Failed to allocate memory for paths[" << std::endl;
				return false;
			}
		}
		if (leafNode->keys[i]->key > key->key) {
			// 在正确的位置插入新键和新记录
			leafNode->keys.insert(leafNode->keys.begin() + i, key);
			vector<KeyType> newVector;
			newVector.push_back(key);
			leafNode->paths.insert(leafNode->paths.begin() + i, newVector);
			leafNode->numberOfKeys++;
			inserted = true;
			break;
		}
	}

	if (!inserted) {
		// 如果新记录是最大的，或者叶子节点为空，则添加到末尾
		leafNode->keys.push_back(key);
		vector<KeyType> newVector;
		newVector.push_back(key);
		leafNode->paths.push_back(newVector);
		leafNode->numberOfKeys++;
		inserted = true;
	}

	// 3. 检查是否需要分裂叶子节点
	if (leafNode->numberOfKeys > (treeOrder - 1)) {
		bool flag = splitNode(leafNode, splitNumPtr);
		while (tree->parent)
			tree = tree->parent;
		return flag;
	}

	return true;
}

// 分裂函数，可能是叶节点，也可能是非叶节点
bool BPtree::splitNode(Node* node, long int* splitNumPtr) {
	// 创建新的节点
	Node* newNode = new Node(treeOrder, node->isLeaf);
	*splitNumPtr = *splitNumPtr + 1;
	// 计算分裂点
	int splitPoint = node->numberOfKeys / 2;
	int AllN = node->numberOfKeys;

	if (node->isLeaf)
	{
		newNode->keys.insert(newNode->keys.begin(), node->keys.begin() + splitPoint, node->keys.end());
		//newNode->numberOfKeys = node->numberOfKeys - splitPoint;
		newNode->numberOfKeys = newNode->keys.size();
		// 从原节点中移除后半部分的键和路径
		node->keys.erase(node->keys.begin() + splitPoint, node->keys.end());
		//node->numberOfKeys = splitPoint;
		node->numberOfKeys = node->keys.size();
		newNode->paths.insert(newNode->paths.begin(), node->paths.begin() + splitPoint, node->paths.end());
		node->paths.erase(node->paths.begin() + splitPoint, node->paths.end());
		if (node->numberOfKeys != node->paths.size() || newNode->numberOfKeys != newNode->paths.size() || (node->numberOfKeys + newNode->numberOfKeys) != AllN)
		{
			cout << "split leaf node failed! split!\n";
			return false;
		}
		// 插入新节点到父节点
		return insertIntoParent(node, newNode, newNode->keys[0], splitNumPtr);
	}
	else
	{
		Record* newKey = node->keys[splitPoint];
		//不包括中间的节点
		newNode->keys.insert(newNode->keys.begin(), node->keys.begin() + splitPoint + 1, node->keys.end());
		//newNode->numberOfKeys = node->numberOfKeys - splitPoint-1;
		newNode->numberOfKeys = newNode->keys.size();
		// 从原节点中移除后半部分的键和路径
		node->keys.erase(node->keys.begin() + splitPoint, node->keys.end());
		//node->numberOfKeys = splitPoint;
		node->numberOfKeys = node->keys.size();
		newNode->pointers.insert(newNode->pointers.begin(), node->pointers.begin() + splitPoint + 1, node->pointers.end());
		node->pointers.erase(node->pointers.begin() + splitPoint + 1, node->pointers.end());
		if (node->numberOfKeys != (node->pointers.size() - 1) || newNode->numberOfKeys != (newNode->pointers.size() - 1) || (node->numberOfKeys + newNode->numberOfKeys + 1) != AllN)
		{
			cout << "split internal node failed! split!\n";
			//cout << "node info: " << node->numberOfKeys << " " << node->pointers.size() << " newNode info: " << newNode->numberOfKeys << " " << newNode->pointers.size() << " ALLN: " << AllN << endl;
			return false;
		}
		// 插入新节点到父节点
		return insertIntoParent(node, newNode, newKey, splitNumPtr);
	}

}

// 将新节点插入到父节点的函数，一定是非叶节点
bool BPtree::insertIntoParent(Node* oldNode, Node* newNode, Record* newKey, long int* splitNumPtr) {
	Node* parent = oldNode->parent;
	// 如果 oldNode 是根节点，则创建新的根节点
	if (parent == nullptr) {
		Node* newRoot = new Node(treeOrder, false);//创建一个新的非叶节点作为根节点
		newRoot->keys.push_back(newKey);
		newRoot->pointers.push_back(oldNode);
		newRoot->pointers.push_back(newNode);
		newRoot->numberOfKeys = 1;
		newRoot->parent = NULL;
		oldNode->parent = newRoot;
		newNode->parent = newRoot;//更换了树的根节点
		if (oldNode->isLeaf)
		{
			newNode->nextList = oldNode->nextList;
			newNode->lastList = oldNode;
			oldNode->nextList = newNode;
		}
		return true;
	}

	int flag = 0;
	for (int i = 0; i < parent->numberOfKeys; i++)
	{
		if (parent->keys[i]->key > newKey->key)
		{
			parent->keys.insert(parent->keys.begin() + i, newKey);
			parent->pointers.insert(parent->pointers.begin() + i + 1, newNode);
			parent->numberOfKeys++;
			newNode->parent = parent;
			flag = 1;
			break;
		}
		if (parent->keys[i]->key == newKey->key)
		{
			cout << "Insert to Parent Error!" << " " << newKey->key << endl;
			/*cout << oldNode->isLeaf << endl;
			cout << newNode->parent << endl;*/
			/*cout << parent->pointers[i + 1]->numberOfKeys << " ";
			for (int j = 0; j < parent->pointers[i + 1]->numberOfKeys; j++)
				cout << dec << parent->pointers[i + 1]->keys[j]->key << " ";
			cout << endl;
			cout << newNode->numberOfKeys << " ";
			for (int j = 0; j < newNode->numberOfKeys; j++)
				cout << dec << newNode->keys[j]->key << " ";
			cout << endl;*/
			return false;
		}
	}
	if (!flag)
	{
		parent->keys.push_back(newKey);
		parent->pointers.push_back(newNode);
		parent->numberOfKeys++;
		newNode->parent = parent;
	}
	if (oldNode->isLeaf)
	{
		newNode->nextList = oldNode->nextList;
		newNode->lastList = oldNode;
		oldNode->nextList = newNode;
	}
	// 如果父节点满了，则分裂
	if (parent->numberOfKeys > (treeOrder - 1)) {
		return splitNode(parent, splitNumPtr);
	}
	return true;
}

void BPtree::printTree(Node* T, int level) {
	if (T == nullptr) {
		cout << "The tree is NULL!\n" << endl;
		return;
	}

	// 使用制表符或空格来表示层级，增加可读性
	for (int i = 0; i < level; ++i) std::cout << "\t";

	// 打印当前节点的键
	std::cout << level << " Level: [";
	for (int i = 0; i < T->numberOfKeys; ++i) {
		std::cout << T->keys[i]->key;
		if (i < T->numberOfKeys - 1) std::cout << ","; // 避免在最后一个键后打印逗号
	}
	std::cout << "]" << std::endl;

	if (!T->isLeaf) {
		std::cout << "Child Num:" << T->pointers.size() << std::endl;
		for (Node* child : T->pointers) {
			printTree(child, level + 1);
		}
	}
}

//=====================================================删除================================
//叶节点中的删除
void BPtree::deleteKey(Node*& tree, KeyType key) {
	if (!tree)
		return;
	Node* leafNode = findLeafNode(tree, key->key);//找到叶节点
	if (!leafNode) return;
	int uflag = 0;
	bool deleted = false;
	for (int i = 0; i < leafNode->keys.size(); ++i) {
		if (leafNode->keys[i]->key == key->key) {
			for (int j = 0; j < leafNode->paths[i].size(); j++)
			{
				if (judgeRecords(leafNode->paths[i][j], key) == 0)  //比较两个Record*是否相等，需要实现
				{
					leafNode->paths[i].erase(leafNode->paths[i].begin() + j);
					if (leafNode->paths[i].size() == 0)
					{
						if (i == 0)
							uflag = 1;
						leafNode->keys.erase(leafNode->keys.begin() + i);
						leafNode->paths.erase(leafNode->paths.begin() + i);
						leafNode->numberOfKeys--;
					}
					deleted = true;
					break;
				}
			}
			if (deleted)
				break;
		}
	}

	if (!deleted) return;//没有删除成功，直接返回
	if (leafNode == tree)//删除的节点是根节点，直接返回
	{
		if (leafNode->numberOfKeys == 0)//根节点空了
		{
			delete leafNode;
			tree = NULL;
		}
		return;
	}
	if (leafNode->parent == tree && leafNode->numberOfKeys == 0)//两层，并且删除记录后，叶节点空了一个
	{
		int nodeParentId = getParentId(leafNode);
		if (nodeParentId == 0)//空节点是父节点的第一个指针
		{
			if (tree->numberOfKeys == 1)//兄弟结点直接变成父节点
			{
				Node* tempTree = tree;
				tree = tree->pointers[1];
				tree->parent = tree->lastList = tree->nextList = NULL;
				tree->isLeaf = true;
				delete leafNode;
				delete tempTree;
			}
			else//删除父节点中的第一个键和指针
			{
				tree->keys.erase(tree->keys.begin());
				tree->pointers.erase(tree->pointers.begin());
				tree->numberOfKeys--;
				leafNode->nextList->lastList = NULL;
				delete leafNode;
			}
			return;
		}
		else//空节点不是父节点的第一个指针
		{
			if (tree->numberOfKeys == 1)
			{
				Node* tempTree = tree;
				tree = tree->pointers[0];
				tree->parent = tree->lastList = tree->nextList = NULL;
				tree->isLeaf = true;
				delete leafNode;
				delete tempTree;
			}
			else
			{
				tree->keys.erase(tree->keys.begin() + nodeParentId - 1);
				tree->pointers.erase(tree->pointers.begin() + nodeParentId);
				tree->numberOfKeys--;
				if (leafNode->lastList && leafNode->nextList)
				{
					leafNode->lastList->nextList = leafNode->nextList;
					leafNode->nextList->lastList = leafNode->lastList;
				}
				else if (leafNode->lastList)
				{
					leafNode->lastList->nextList = NULL;
				}
				else if (leafNode->nextList)
				{
					leafNode->nextList->lastList = NULL;
				}
				else
				{
					cout << " Error!\n";
				}
				delete leafNode;
			}
			return;
		}
		return;
	}
	if (uflag && leafNode->numberOfKeys > 0 && leafNode->parent)//更新父节点中的键
	{
		int nodeParentId = getParentId(leafNode);
		if (nodeParentId > 0)
			leafNode->parent->keys[nodeParentId - 1] = leafNode->keys[0];
	}

	if (leafNode->numberOfKeys >= minN)//满足条件，不用处理
	{
		return;
	}

	if (!borrowFromSibling(leafNode)) {
		if (mergeWithSibling(leafNode))
		{
			if (tree->numberOfKeys == 0)
			{
				if (tree->pointers.size() == 0)
				{
					delete tree;
					tree = NULL;
				}
				else
				{
					Node* oldParent = tree;
					tree = tree->pointers[0]; // 更新父节点指针
					delete oldParent; // 删除原来的父节点
				}
			}
		}
	}
}

bool BPtree::borrowFromSibling(Node*& node) {
	Node* sibling = nullptr;
	Node* parent = node->parent;//父节点
	if (!parent)//没有兄弟节点
		return false;
	int siblingIndex = -1;
	int nodeParentId = getParentId(node);
	if (nodeParentId > 0)
	{
		sibling = parent->pointers[nodeParentId - 1];
		if (sibling->numberOfKeys > minN)
			siblingIndex = 0;
	}
	if (siblingIndex == -1 && (nodeParentId + 1) < parent->pointers.size()) {

		sibling = parent->pointers[nodeParentId + 1];//右兄弟
		if (sibling->numberOfKeys > minN)
			siblingIndex = 1;
	}
	if (siblingIndex == -1)
		return false;
	// 从兄弟节点借键
	if (siblingIndex == 0) { // 从左兄弟借键,父节点下来，左节点最后一个上去父节点
		Record* oldKey = parent->keys[nodeParentId - 1];
		node->keys.insert(node->keys.begin(), oldKey);
		parent->keys[nodeParentId - 1] = sibling->keys.back();
		sibling->keys.pop_back();
		sibling->numberOfKeys--;
		node->numberOfKeys++;
		if (node->isLeaf)
		{
			node->paths.insert(node->paths.begin(), sibling->paths.back());
			sibling->paths.pop_back();
		}
		else
		{
			node->pointers.insert(node->pointers.begin(), sibling->pointers.back());
			sibling->pointers.back()->parent = node;
			sibling->pointers.pop_back();
		}

		return true;
		// 更新父节点的键值
		//return updateParentKey(parent, oldKey, node->keys[0]);
	}
	else { // 从右兄弟借键，父节点下来，右兄弟第一个key上去
		Record* oldKey = parent->keys[nodeParentId];
		node->keys.push_back(oldKey);
		parent->keys[nodeParentId] = sibling->keys[0];
		sibling->keys.erase(sibling->keys.begin());
		sibling->numberOfKeys--;
		node->numberOfKeys++;
		if (node->isLeaf)
		{
			node->paths.push_back(sibling->paths.front());
			sibling->paths.erase(sibling->paths.begin());
		}
		else
		{
			node->pointers.push_back(sibling->pointers.front());
			sibling->pointers.front()->parent = node;
			sibling->pointers.erase(sibling->pointers.begin());
		}

		return true;
		// 更新父节点的键值
		//return updateParentKey(parent, oldKey, sibling->keys[0]);
	}
	return false;
}

int BPtree::getParentId(Node* node)
{
	if (!node->parent) return -1;
	for (size_t i = 0; i < node->parent->pointers.size(); ++i) {
		if (areNodesEqual(node->parent->pointers[i], node)) {
			return i;
		}
	}
	return -1;
}

//叶节点
bool BPtree::mergeWithSibling(Node*& node) {
	Node* parent = node->parent;
	if (!parent)
		return false;
	Node* sibling;
	int nodeParentId = getParentId(node);
	if (nodeParentId > 0)//存在左兄弟
	{
		sibling = parent->pointers[nodeParentId - 1];
		Record* oldKey = parent->keys[nodeParentId - 1];
		if (node->isLeaf)
		{
			sibling->keys.insert(sibling->keys.end(), node->keys.begin(), node->keys.end());
			node->keys.erase(node->keys.begin(), node->keys.end());
			node->numberOfKeys = 0;
			sibling->numberOfKeys = sibling->keys.size();
			sibling->paths.insert(sibling->paths.end(), node->paths.begin(), node->paths.end());
			node->paths.erase(node->paths.begin(), node->paths.end());

			sibling->nextList = node->nextList;
			if (node->nextList)
				node->nextList->lastList = sibling;
		}
		else//父节点需要下移
		{
			sibling->keys.push_back(oldKey);
			sibling->keys.insert(sibling->keys.end(), node->keys.begin(), node->keys.end());
			node->keys.erase(node->keys.begin(), node->keys.end());
			node->numberOfKeys = 0;
			sibling->numberOfKeys = sibling->keys.size();
			sibling->pointers.insert(sibling->pointers.end(), node->pointers.begin(), node->pointers.end());

			for (int i = 0; i < node->pointers.size(); i++)
			{
				node->pointers[i]->parent = sibling;
			}
			node->pointers.erase(node->pointers.begin(), node->pointers.end());
		}
		deleteFromParent(parent, oldKey);
		if (sibling->numberOfKeys > maxN)
		{
			//splitNode(sibling);
		}
		// 删除原来的叶子节点
		delete node;
		return true;
	}
	if ((nodeParentId + 1) < parent->pointers.size()) { // 合并右兄弟

		sibling = parent->pointers[nodeParentId + 1];
		Record* oldKey = parent->keys[nodeParentId];
		if (node->isLeaf)
		{
			node->keys.insert(node->keys.end(), sibling->keys.begin(), sibling->keys.end());
			sibling->keys.erase(sibling->keys.begin(), sibling->keys.end());
			sibling->numberOfKeys = 0;
			node->numberOfKeys = node->keys.size();
			node->paths.insert(node->paths.end(), sibling->paths.begin(), sibling->paths.end());
			sibling->paths.erase(sibling->paths.begin(), sibling->paths.end());

			node->nextList = sibling->nextList;
			if (sibling->nextList)
				sibling->nextList->lastList = node;
		}
		else//父节点需要下移
		{
			node->keys.push_back(oldKey);
			node->keys.insert(node->keys.end(), sibling->keys.begin(), sibling->keys.end());
			sibling->keys.erase(sibling->keys.begin(), sibling->keys.end());
			sibling->numberOfKeys = 0;
			node->numberOfKeys = node->keys.size();
			node->pointers.insert(node->pointers.end(), sibling->pointers.begin(), sibling->pointers.end());
			for (int i = 0; i < sibling->pointers.size(); i++)
			{
				sibling->pointers[i]->parent = node;
			}
			sibling->pointers.erase(sibling->pointers.begin(), sibling->pointers.end());
		}

		// 从父节点中删除对应的键值
		deleteFromParent(parent, oldKey);
		if (node->numberOfKeys > maxN)
		{
			//splitNode(node);
		}
		// 删除原来的兄弟节点
		delete sibling;
		return true;
	}
	return false;
}

//非叶节点
bool BPtree::deleteFromParent(Node*& parent, KeyType key) {
	// 从父节点中删除键值，并更新指针
	if (parent == nullptr) return false; // 没有父节点

	for (int i = 0; i < parent->keys.size(); ++i) {
		if (parent->keys[i]->key == key->key) {
			parent->keys.erase(parent->keys.begin() + i);
			parent->pointers.erase(parent->pointers.begin() + i + 1); // 删除键值对应的子节点指针
			parent->numberOfKeys--;
			break;
		}
	}
	if (!parent->parent)//根节点
	{
		//if (parent->numberOfKeys == 0 && parent->pointers.size() == 0)
		//{
		//	delete parent;
		//	parent = NULL;
		//	return true;
		//}
		//if (parent->numberOfKeys == 0 && parent->pointers.size() != 0)
		//{
		//	//parent = parent->pointers[0];
		//	Node* oldParent = parent;
		//	parent = parent->pointers[0]; // 更新父节点指针
		//	delete oldParent; // 删除原来的父节点
		//	/*Node* tempTree = parent->pointers[0];
		//	tempTree->parent = NULL;
		//	delete parent;
		//	parent = tempTree;*/
		//	return true;
		//}
		return true;
	}


	// 如果父节点键值数量不足，需要递归处理父节点
	if (parent->numberOfKeys < minN) {
		if (borrowFromSibling(parent)) {
			// 借键成功，结束调整
			return true;
		}
		if (mergeWithSibling(parent))
		{
			// 借键失败，合并节点
			return true;
			// 如果合并后父节点为空，或者父节点的键的数量满足条件，则结束调
		}
		return false;
	}
	return true;
}
