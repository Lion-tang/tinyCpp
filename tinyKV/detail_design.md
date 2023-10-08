## 2 项目代码
所有实现代码位于 `skiplist.h` 头文件中，CPP 文件中 include 它就可以使用。内部关键在于定义了 `SkipList` 结构，以及 `Node` 结构。

### 2.1 头文件

```c++

#include <iostream> 

#include <cstdlib>

#include <cmath>

#include <cstring>

#include <mutex>

#include <fstream>

```

------

### 2.2 基本定义


```c++

std::mutex mtx, mtx1;  

std::string delimiter = ":";

```

**说明** :sun_with_face:

:small_blue_diamond: 宏定义数据落盘位置，以及加载数据位置；

:small_orange_diamond: 定义互斥量锁，用于写操作中的临界区；

:small_blue_diamond: 定义分隔符，用于加载数据是识别 *************Key************* 和 *************Value************* 。

------

### 2.3 Node

#### 2.3.1 Node 定义

`Node` 结构是 `SkipList` 实现的基础。其定义如下：

```c++

template<typename K, typename V> 
class Node {
**// ~ Node 节点定义**
public:

  Node(){}; **// default constructor**

  Node(const K& k, const V& v, int);

  ~Node();

  K getKey() const;

  V getValue() const;

  void setValue(V v);

  Node<K, V> ***forward; *// 前向指针数组**

  int node_level; **//当前节点的层数**

 private:

  K key;

  V value;

};

```

**说明** :sun_with_face:

:small_blue_diamond: 私有数据成员 **key** ， **value** ，公有数据成员 **forward** ，**node_level**

:small_orange_diamond: 成员函数：构造函数，析构函数，键值对操作函数

**注意** :raising_hand_man:

**forward** 是一个指针数组，其元素是指向 **Node** 的指针。
------

#### 2.3.2 构造函数和析构函数

```c++
template<typename K, typename V> 
Node<K, V>::Node(const K k, const V v, int level) {
​    this->key = k;
​    this->value = v;
​    this->node_level = level;    
​    // forward 大小为 level + 1
​    this->forward = new Node<K, V>**[level + 1]; *// 分配内存**
​    memset(this->forward, 0, sizeof(Node<K, V>**)**(level + 1)); **// 初始化**
};

template<typename K, typename V> 
Node<K, V>::~Node() {
​    delete []forward;  **// 释放内存**
};

```
------

#### 2.3.3 键值对操作函数

```c++
template <typename K, typename V> K Node<K, V>::getKey() const {
  return this->key;
}

template <typename K, typename V> V Node<K, V>::getValue() const {
  return this->value;
}

template <typename K, typename V> void Node<K, V>::setValue(V v) {
  this->value = v;
}

```
------

### 2.4 SkipList

#### 2.4.1 SkipList 定义

```c++
template <typename K, typename V> 
class SkipList {
public:
  SkipList(int);

  ~SkipList();

  int getRandomLevel();

  Node<K, V> *createNode(K, V, int);

  int putElement(K, V);

  void displayList();

  bool searchElement(K k);

  void deleteElement(K k);

  void clear();

  int size() const;

private:

  Node<K, V> *head;

  int maxLevel;      **// maximum level of this skip list, or named highest limit**

  int skipListLevel; **// current highest level**

  **// file read and write**

  std::ofstream fileWriter;

  std::ifstream fileReader;

  int elementCount; **// current element count**

};

```

**说明** :sun_with_face:

:small_blue_diamond: 私有数据成员 **maxLevel, skipListLevel, elementCount, head, fileWriter, fileReader**

:small_orange_diamond: 成员函数：构造函数，析构函数，创建节点函数，增删改查操作函数，清空跳表，数据落盘和加载数据及其相关函数，打印跳表函数，返回跳表节点数函数，获得随机层高函数 。

------

#### 2.4.2 构造函数、析构函数、节点构造函数

```c++

template <typename K, typename V> SkipList<K, V>::SkipList(int maxLevel) {
  this->maxLevel = maxLevel;
  this->elementCount = 0;
  this->skipListLevel = 0;
  // create head node with initial key and value
  K k;
  V v;
  this->head = createNode(k, v, maxLevel);
}

template <typename K, typename V> SkipList<K, V>::~SkipList() {
  Node<K, V> *current = head;
  while (current != NULL) {
​    Node<K, V> *next = current->forward[0];
​    delete current;
​    current = next;
  }
}

template <typename K, typename V>
Node<K, V> *SkipList<K, V>::createNode(K key, V value, int level) {
  Node<K, V> *node = new Node<K, V>(key, value, level);
  return node;
}

```

------

#### 2.4.3 获得随机层高函数以及返回跳表大小函数

```c++
template <typename K, typename V> int SkipList<K, V>::getRandomLevel() {
  int level = 1;
  while (rand() % 2) {
​    level++;
  }
  level = level > this->maxLevel ? this->maxLevel : level;
  return level;
}

template<typename K, typename V> 
int SkipList<K, V>::size() { 
​    return _element_count;  **// 会随着节点的添加, 删除, 更新改变**
}

```
**注意** :raising_hand: 层高的选择一般根据 **幂次定律** （power law），越大的数出现的概率越小 。

------

#### 2.4.4 put_element()

```c++

template <typename K, typename V>

int SkipList<K, V>::putElement(K key, V value) {
  mtx1.lock();
  Node<K, V> *current = head;
  Node<K, V> *update[maxLevel + 1];
  memset(update, 0, sizeof(Node<K, V> *) * (maxLevel + 1));

  // record frontend node of param key in each level

  for (int i = skipListLevel; i >= 0; i--) {
​    while (current->forward[i] != NULL && current->forward[i]->getKey() < key) {
​      current = current->forward[i];
​    }
​    update[i] = current;
  }
  // the first node whose key is ge than param key
  current = current->forward[0];
  // 1. update direcctly, return 1
  if (current != NULL && current->getKey() == key) {
​    std::cout << "update key: " << key << ", old value: " << current->getValue()<< " ,new value: " << value << std::endl;
​    current->setValue(value);
​    mtx1.unlock();
​    return 1;
  }

  // 2. key does not exist, create and insert new node
  int newNodeLevel = getRandomLevel();
  if(newNodeLevel > skipListLevel){
​    for(int i = skipListLevel + 1; i <= newNodeLevel; i++){
​      update[i] = head;
​    }
​    skipListLevel = newNodeLevel;
  }
  Node<K, V>* newNode = createNode(key, value, newNodeLevel);

  // insert node
  // the operation is similar to insert a node into a list**
  // which is like: cur -> next = pre -> next; pre->next = cur; pre = cur; cur = cur->next;** 
  // but need to insert from newNode-level to 0-level
  // insert given by key and value into skip list
  /**** *example:**
​                            **+------------+**
​                            **|  insert 50 |**
​                            **+------------+**
  **level 4     +-->1+                                                      100**
​                  **|**
​                  **|                       insert +----+**
  **level 3         1+-------->10+---------------> | 50 |          70       100**
​                                                 **|    |**
​                                                 **|    |**
  **level 2         1          10         30       | 50 |          70       100**
​                                                 **|    |**
​                                                 **|    |**
  **level 1         1    4     10         30       | 50 |          70       100**
​                                                 **|    |**
​                                                 **|    |**
  **level 0         1    4   9 10         30   40  | 50 |  60      70       100**
*/


​                                               
  for(int i = newNodeLevel; i >= 0; i--){
​    newNode -> forward[i] = update[i] -> forward[i];
​    update[i]->forward[i] = newNode;
  }
  elementCount++;
  mtx1.unlock();
  return 0;
}

template <typename K, typename V> int SkipList<K, V>::size() const {
  return elementCount;
}

template <typename K, typename V> void SkipList<K, V>::clear() {
  std::cout << "clear ..." << std::endl;
  Node<K, V> *current = head->forward[0];
  // delete each node except for dummy head
  while (current != NULL) {
​    Node<K, V>* tmp = current->forward[0];
​    delete current;
​    current = tmp;
  }
  // pivot head forward pointer to NULL
  for (int i = skipListLevel; i >= 0; i--) {
​    head->forward[i] = NULL;
  }
  // reset level and count to 0
  skipListLevel = 0;
  elementCount = 0;
}

```
------

#### 2.4.5 delete_element()


```c++
template <typename K, typename V> void SkipList<K, V>::deleteElement(K key) {
  mtx.lock();
  Node<K, V> *current = head;
  Node<K, V> *update[maxLevel + 1];
  memset(update, 0, sizeof(Node<K, V> *) * (maxLevel + 1));
  for (int i = skipListLevel; i >= 0; i--) {
​    while (current->forward[i] != NULL && current->forward[i]->getKey() < key) {
​      current = current->forward[i];
​    }
​    update[i] = current;
  }
  current = current->forward[0];
  if (current != NULL && current->getKey() != key) {
​    std::cout << "key: " << key << " does not exist, do not need to delete" << std::endl;
​    mtx.unlock();
​    return;

  }
  // now current node is found node with param key or NULL
  // when current node is NULL, do nothing
  if(current == NULL){
​    std::cout << "key: " << key << " does not exist, do not need to delete" << std::endl;
  }
  // when current node is not NULL, node is founded, delete link from currentLevel to level0
  else {
​    int currentLevel = current->node_level;
​    for (int i = currentLevel; i >= 0; i--) {
​      update[i]->forward[i] = current -> forward[i];
​    }
​    delete current;
​    std::cout << "Successfully delete key: " << key << std::endl;
​    elementCount--;
​    **// if skipListLevel only have node needed to be deleted, decrease skipListLevel gradually**
​    while(skipListLevel > 0 && head -> forward[skipListLevel] == NULL){
​      skipListLevel--;
​    }
  }
  mtx.unlock();
}

```

------

#### 2.4.7 search_element()

```c++

template <typename K, typename V> bool SkipList<K, V>::searchElement(K key) {
  Node<K, V> *current = head;
  for (int i = skipListLevel; i >= 0; i--) {
​    while (current->forward[i] != NULL && current->forward[i]->getKey() < key) {
​      current = current->forward[i];
​    }
  }
  current = current->forward[0];
  if (current != NULL && current->getKey() == key) {
​    std::cout << "key: " << key << " is found, value: " << current->getValue()<< std::endl;
​    return true;
  }
  std::cout << "key: " << key << " is not found" << std::endl;
  return false;
}

```
**说明** :sun_with_face: ：代码详细解释见注释。

------

#### 2.4.8 打印跳表

```c++
template <typename K, typename V> void SkipList<K, V>::displayList() {
  Node<K, V> *current;
  for (int i = skipListLevel; i >= 0; i--) {
​    // std::cout << "---------" << i << "th level: ---------" << std::endl;
​    current = head->forward[i];
​    int cnt = 1;
​    while (current != NULL) {
​      std::cout << cnt++ << "th Node, key: " << current->getKey() << " , value: " << current->getValue() << std::endl;
​      current = current->forward[i];
​    }
  }

}

```

------

#### 2.4.10 clear()


```c++

template <typename K, typename V> void SkipList<K, V>::clear() {
  std::cout << "clear ..." << std::endl;
  Node<K, V> *current = head->forward[0];
  // delete each node except for dummy head

  while (current != NULL) {
​    Node<K, V>* tmp = current->forward[0];
​    delete current;
​    current = tmp;
  }
  // pivot head forward pointer to NULL
  for (int i = skipListLevel; i >= 0; i--) {
​    head->forward[i] = NULL;
  }
  // reset level and count to 0
  skipListLevel = 0;
  elementCount = 0;
}
```

**说明** :sun_with_face: ：代码详细解释见注释。

**注意** :raising_hand_man: ：清空除了头节点以外的节点并且头节点 *************forward************* 初始化；
------
## 3 项目测试

### 3.1 `skiplist.h` 测试

针对所有提供的 API 进行测试：

```c++
#include <iostream>
#include <iterator>
#include "skiplist.h"

int main() {
​    SkipList<std::string, std::string> skipList(6);
​    **// insert test**
​    std::cout << "--- insert test ---" << std::endl;
​    skipList.putElement("1", " one"); 
​    skipList.putElement("2", " two"); 
​    skipList.putElement("3", " three"); 
​    skipList.putElement("abc", " test1"); 
​    skipList.putElement("中文", " test2"); 
​    std::cout << "skipList size after putElement(): " << skipList.size() << std::endl; **// 5**
​    **// search test**
​    std::cout << "--- search test ---" << std::endl;
​    skipList.searchElement("3");
​    skipList.searchElement("中文");
​    skipList.searchElement("4");
​    **// delete test**
​    std::cout << "--- delete test ---" << std::endl;
​    skipList.deleteElement("3");  **// 成功**
​    skipList.deleteElement("5");  **// 失败**
​    std::cout << "skipList size after deleteElement(): " << skipList.size() << std::endl; **// 4**
​    // display test
​    std::cout << "--- display test ---" << std::endl;
​    skipList.displayList();
​    // clear test
​    std::cout << "--- clear test ---" << std::endl;

​    skipList.clear();
​    std::cout << "skipList size after clear(): " << skipList.size() << std::endl; **// 0**
​    return 0;

}


```

https://github.com/youngyangyang04/Skiplist-CPP