# miniSmartpointers
实现简单版本的C++智能指针
> 智能指针最朴素的想法是利用类的析构函数和函数栈的自动释放机制来自动管理指针，即用户只要按需分配堆空间，堆空间的释放由智能指针帮你完成。

1. 实现了 mini_shared_ptr, mini_weak_ptr(在 `mini_shared_ptr.hpp` 中), mini_unique_ptr
2. `shared_ptr_test.cpp` 测试了循环依赖情况下的智能指针释放，使用 weak_ptr 可以解决这类问题