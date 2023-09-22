#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <ostream>
#include <string>
#include <type_traits>


std::mutex mtx, mtx1;

template <typename K, typename V> class Node {
public:
  Node(){}; // default constructor

  Node(const K& k, const V& v, int);

  ~Node();

  K getKey() const;

  V getValue() const;

  void setValue(V v);

  Node<K, V> **forward; // 前向指针数组

  int node_level; //当前节点的层数

private:
  K key;
  V value;
};

template <typename K, typename V>
Node<K, V>::Node(const K& k, const V& v, int level) {
  this->key = k;
  this->value = v;
  this->node_level = level;
  this->forward = new Node<K, V> *[level + 1];
  memset(this->forward, 0, sizeof(Node<K, V> *) * (level + 1));
};

template <typename K, typename V> Node<K, V>::~Node() { delete[] forward; }

template <typename K, typename V> K Node<K, V>::getKey() const {
  return this->key;
}

template <typename K, typename V> V Node<K, V>::getValue() const {
  return this->value;
}

template <typename K, typename V> void Node<K, V>::setValue(V v) {
  this->value = v;
}

template <typename K, typename V> class SkipList {
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
  int maxLevel;      // maximum level of this skip list, or named highest limit
  int skipListLevel; // current highest level
  // file read and write
  std::ofstream fileWriter;
  std::ifstream fileReader;
  int elementCount; // current element count
};

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
    Node<K, V> *next = current->forward[0];
    delete current;
    current = next;
  }
}
/*
  generate a random level，level - 0 included posibility is 100%, and level - 1
  is 50%， according to this，it can scatter data from 0th to nth level in
  1,0.5,0.25 distribution
*/
template <typename K, typename V> int SkipList<K, V>::getRandomLevel() {
  int level = 1;
  while (rand() % 2) {
    level++;
  }
  level = level > this->maxLevel ? this->maxLevel : level;
  return level;
}

template <typename K, typename V>
Node<K, V> *SkipList<K, V>::createNode(K key, V value, int level) {
  Node<K, V> *node = new Node<K, V>(key, value, level);
  return node;
}


template <typename K, typename V> void SkipList<K, V>::displayList() {
  Node<K, V> *current;
  for (int i = skipListLevel; i >= 0; i--) {
    std::cout << "---------" << i << "th level: ---------" << std::endl;
    current = head->forward[i];
    int cnt = 1;
    while (current != NULL) {
      std::cout << cnt++ << "th Node, key: " << current->getKey()
                << " , value: " << current->getValue() << std::endl;
      current = current->forward[i];
    }
  }
}

/*
  flag, indicate operation when k does not exist，
  1 if key exists, update directly, return 1
  2 if key does not exist, and flag is true, create new node, return 0
*/
template <typename K, typename V>
int SkipList<K, V>::putElement(K key, V value) {
  mtx1.lock();
  Node<K, V> *current = head;
  Node<K, V> *update[maxLevel + 1];
  memset(update, 0, sizeof(Node<K, V> *) * (maxLevel + 1));
  // record frontend node of param key in each level
  for (int i = skipListLevel; i >= 0; i--) {
    while (current->forward[i] != NULL && current->forward[i]->getKey() < key) {
      current = current->forward[i];
    }
    update[i] = current;
  }
  // the first node whose key is ge than param key
  current = current->forward[0];
  // 1. update direcctly, return 1
  if (current != NULL && current->getKey() == key) {
    std::cout << "update key: " << key << ", old value: " << current->getValue()
              << " ,new value: " << value << std::endl;
    current->setValue(value);
    mtx1.unlock();
    return 1;
  }
  // 2. key does not exist, create and insert new node
  int newNodeLevel = getRandomLevel();
  if(newNodeLevel > skipListLevel){
    for(int i = skipListLevel + 1; i <= newNodeLevel; i++){
      update[i] = head;
    }
    skipListLevel = newNodeLevel;
  }
  
  Node<K, V>* newNode = createNode(key, value, newNodeLevel);
  // insert node
  // the operation is similar to insert a node into a list
  // which is like: cur -> next = pre -> next; pre->next = cur; pre = cur; cur = cur->next; 
  // but need to insert from newNode-level to 0-level
  // insert given by key and value into skip list
  /* example:
                            +------------+
                            |  insert 50 |
                            +------------+
  level 4     +-->1+                                                      100
                  |
                  |                       insert +----+
  level 3         1+-------->10+---------------> | 50 |          70       100
                                                 |    |
                                                 |    |
  level 2         1          10         30       | 50 |          70       100
                                                 |    |
                                                 |    |
  level 1         1    4     10         30       | 50 |          70       100
                                                 |    |
                                                 |    |
  level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                                 +----+
  */
  for(int i = newNodeLevel; i >= 0; i--){
    newNode -> forward[i] = update[i] -> forward[i];
    update[i]->forward[i] = newNode;
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
    Node<K, V>* tmp = current->forward[0];
    delete current;
    current = tmp;
  }
  // pivot head forward pointer to NULL
  for (int i = skipListLevel; i >= 0; i--) {
    head->forward[i] = NULL;
  }
  // reset level and count to 0
  skipListLevel = 0;
  elementCount = 0;
}

/*
  return 1 if key exsits, otherwise return 0
*/
template <typename K, typename V> bool SkipList<K, V>::searchElement(K key) {
  Node<K, V> *current = head;
  for (int i = skipListLevel; i >= 0; i--) {
    while (current->forward[i] != NULL && current->forward[i]->getKey() < key) {
      current = current->forward[i];
    }
  }
  current = current->forward[0];
  if (current != NULL && current->getKey() == key) {
    std::cout << "key: " << key << " is found, value: " << current->getValue()
              << std::endl;
    return true;
  }
  std::cout << "key: " << key << " is not found" << std::endl;
  return false;
}

template <typename K, typename V> void SkipList<K, V>::deleteElement(K key) {
  mtx.lock();
  Node<K, V> *current = head;
  Node<K, V> *update[maxLevel + 1];
  memset(update, 0, sizeof(Node<K, V> *) * (maxLevel + 1));
  for (int i = skipListLevel; i >= 0; i--) {
    while (current->forward[i] != NULL && current->forward[i]->getKey() < key) {
      current = current->forward[i];
    }
    update[i] = current;
  }
  current = current->forward[0];
  if (current != NULL && current->getKey() != key) {
    std::cout << "key: " << key << " does not exist, do not need to delete" << std::endl;
    mtx.unlock();
    return;
  }
  // now current node is found node with param key or NULL
  // when current node is NULL, do nothing
  if(current == NULL){
    std::cout << "key: " << key << " does not exist, do not need to delete" << std::endl;
  }
  // when current node is not NULL, node is founded, delete link from currentLevel to level0
  else {
    int currentLevel = current->node_level;
    for (int i = currentLevel; i >= 0; i--) {
      update[i]->forward[i] = current -> forward[i];
    }
    delete current;
    std::cout << "Successfully delete key: " << key << std::endl;
    elementCount--;
    // if skipListLevel only have node needed to be deleted, decrease skipListLevel gradually
    while(skipListLevel > 0 && head -> forward[skipListLevel] == NULL){
      skipListLevel--;
    }
  }
  mtx.unlock();
}