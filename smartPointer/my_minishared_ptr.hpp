#include <iostream>
#include <utility>

class counter {
public:
  counter() : sharedCount(0), weakCount(0){};
  int sharedCount;
  int weakCount;
};

template <typename T> class myMiniWeakPtr;

template <typename T> class myMiniSharedPtr {
public:
  friend class myMiniWeakPtr<T>;

  explicit myMiniSharedPtr(T *p = nullptr) : pointer(p) {
    countPtr = new counter();
    if (p) {
      countPtr->sharedCount = 1;
    }
  }

  myMiniSharedPtr(myMiniSharedPtr<T> &p)
      : pointer(p.pointer), countPtr(p.countPtr) {
    countPtr->sharedCount++;
  }

  myMiniSharedPtr(myMiniWeakPtr<T> &p)
      : pointer(p.pointer), countPtr(p.countPtr) {
    countPtr->weakCount++;
  }

  // deconstruction func
  ~myMiniSharedPtr() { _release(); }

  T *operator->() { return pointer; }

  T &operator*() { return *pointer; }

  // dangerous operation, which directly gets original pointer
  T *get() const { return pointer; }

  void reset(T *p = nullptr) {
    if (p != pointer) {
      _release();
      pointer = p;
      countPtr = new counter();
      countPtr->sharedCount++;
    }
  }

  // allow left value and right value assignment
  myMiniSharedPtr<T> &operator=(myMiniSharedPtr<T> &p) {
    if (p.pointer != pointer) {
      _release();
      pointer = p.pointer;
      countPtr = p.countPtr;
      countPtr->sharedCount++;
    }
    return *this;
  }

  int useCount() { return countPtr->sharedCount; }

  bool unique() { return useCount() == 1; }

private:
  T *pointer;
  counter *countPtr;
  void _release() {
    std::cout << "mini shared ptr 析构前, shared count: "
              << countPtr->sharedCount << " weak count: " << countPtr->weakCount
              << " ------ ";
    countPtr->sharedCount--;
    std::cout << "mini shared ptr 析构后, shared count: "
              << countPtr->sharedCount << " weak count: " << countPtr->weakCount
              << std::endl;
    if (countPtr->sharedCount < 1) {
      delete pointer;
      pointer = nullptr;
      if (countPtr->weakCount < 1) {
        delete countPtr;
        countPtr = nullptr;
      }
    }
  }
};

template <typename T> class myMiniWeakPtr {
public:
  friend class myMiniSharedPtr<T>;

  explicit myMiniWeakPtr() : pointer(nullptr), countPtr(nullptr) {}

  myMiniWeakPtr(myMiniSharedPtr<T> &p)
      : pointer(p.pointer), countPtr(p.countPtr) {
    countPtr->weakCount++;
  }

  myMiniWeakPtr(myMiniWeakPtr<T> &p)
      : pointer(p.pointer), countPtr(p.countPtr) {
    countPtr->weakCount++;
  }

  ~myMiniWeakPtr() { _release(); }

  T *operator->() const { return pointer; }

  T &operator*() const { return *pointer; }

  // dangerous operation, which directly gets originial pointer
  T *get() const { return pointer; }

  void reset(T *p = nullptr) {
    if (p != pointer) {
      delete pointer;
      pointer = p;
    }
  }

  myMiniWeakPtr<T> &operator=(myMiniSharedPtr<T> &p) {
    if (p.pointer != pointer) {
      _release();
      pointer = p.pointer;
      countPtr = p.countPtr;
      countPtr->weakCount++;
    }
    return *this;
  }

  myMiniWeakPtr<T> &operator=(myMiniWeakPtr<T> &p) {
    if (p.pointer != pointer) {
      _release();
      pointer = p.pointer;
      countPtr = p.countPtr;
      countPtr->weakCount++;
    }
    return *this;
  }

  myMiniSharedPtr<T> lock() { return myMiniSharedPtr<T>(*this); }

  int useCount() { return countPtr->weakCount; }

  bool unique() { return useCount() == 1; }

  bool exipire() { return useCount() == 0; }

private:
  counter *countPtr;
  T *pointer;
  void _release() {
    if (!countPtr) {
      return;
    }
    std::cout << "mini weak ptr 析构前, shared count: " << countPtr->sharedCount
              << " weak count: " << countPtr->weakCount << " ------ ";
    countPtr->weakCount--;
    std::cout << "mini weak ptr 析构后, shared count: " << countPtr->sharedCount
              << " weak count: " << countPtr->weakCount << std::endl;
    if (countPtr) {
      if (countPtr->sharedCount < 1 && countPtr->weakCount < 1) {
        delete countPtr;
        countPtr = nullptr;
      }
    }
  }
};