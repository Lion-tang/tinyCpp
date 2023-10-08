template <typename T> class myMiniUniquePtr {
public:
  // 管理裸指针
  myMiniUniquePtr<T>(T *p = nullptr) : pointer(p){};
  // 禁止左值拷贝
  myMiniUniquePtr<T>(myMiniUniquePtr<T> &p) = delete;
  // 右值引用拷贝构造
  myMiniUniquePtr<T>(myMiniUniquePtr<T> &&p) { pointer = p.release(); }
  ~myMiniUniquePtr<T>() { delete pointer; }
  T *operator->() const { return pointer; }
  T &operator*() const { return *pointer; }
  // 赋值时使用，不会delete掉指针管理的数据，对原指针失去管理
  T *release() {
    T *oldPtr = pointer;
    pointer = nullptr;
    return oldPtr;
  }
  // 注意自己控制自己，重新设置问题
  void reset(T *p = nullptr) {
    if (p != pointer) {
      delete pointer;
      pointer = p;
    }
  }
  // 禁止左值赋值
  myMiniUniquePtr<T> &operator=(myMiniUniquePtr<T> &p) = delete;
  // 右值赋值
  myMiniUniquePtr<T> &operator=(myMiniUniquePtr<T> &&p) {
    if (p.pointer != pointer) {
      reset(p.release());
    }
    return *this;
  }

private:
  T *pointer;
};