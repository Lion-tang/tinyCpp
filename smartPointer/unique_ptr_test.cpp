#include "my_miniunique_ptr.hpp"
#include <string>
#include <iostream>
#include <utility>
using namespace std;
class student
{
public:
    student(string n, int a) : name(n), age(a) { cout << "构造student name" << name << " age:" << age << endl; }
    ~student()
    {
        cout << "析构student name" << name << " age:" << age << endl;
    }
    string getName() const
    {
        return name;
    }
    void older(int n)
    {
        age += n;
    }

private:
    string name;
    int age;
};

int main()
{
    myMiniUniquePtr<student> autos;
    // miniunique_ptr<student> autos = miniunique_ptr<student>(new student("稳健",88));
    myMiniUniquePtr<student> autos2(new student("大佬", 66));
    myMiniUniquePtr<student> unique1;
    // unique1 = autos;//禁止左值赋值
    unique1 = myMiniUniquePtr<student>(new student("右值稳健", 88));
    // auto au3(autos);//禁止左值复制构造
    auto au3(myMiniUniquePtr<student>(new student("稳健", 88)));
    // cout<<"------------------"<<endl;
    // cout<<autos.get()<<endl;
}