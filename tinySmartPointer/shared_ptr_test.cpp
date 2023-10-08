#include "my_minishared_ptr.hpp"
#include <iostream>
#include <string>
using namespace std;

class B;

class A {
public:
  A(myMiniSharedPtr<B> &mb) : b(mb){};
  A(myMiniWeakPtr<B> &mb) : b(mb){};
  A():b(){};
  // myMiniSharedPtr<B> b; memory leak
  myMiniWeakPtr<B> b;
};

class B {
public:
  B(myMiniSharedPtr<A> &ma) : a(ma){};
  B(myMiniWeakPtr<A> &ma) : a(ma){};
  B():a(){};
  // myMiniSharedPtr<A> a; memory leak
  myMiniWeakPtr<A> a;
};

int main() {
  A *pa = new A;
  cout << "a address : " << pa << endl;
  B *pb = new B;
  cout << "b address : " << pb << endl;
  myMiniSharedPtr<A> sharedPA(pa);
  myMiniSharedPtr<B> sharedPB(pb);
  sharedPA->b = sharedPB;
  sharedPB->a = sharedPA;
}