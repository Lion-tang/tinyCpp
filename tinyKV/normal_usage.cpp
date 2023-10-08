#include <iostream>
#include <iterator>
#include "skiplist.h"

int main() {

    SkipList<std::string, std::string> skipList(6);
    // insert test
    std::cout << "--- insert test ---" << std::endl;
	skipList.putElement("1", " one"); 
	skipList.putElement("2", " two"); 
	skipList.putElement("3", " three"); 
	skipList.putElement("abc", " test1"); 
    skipList.putElement("中文", " test2"); 

    std::cout << "skipList size after putElement(): " << skipList.size() << std::endl; // 5
 
    // search test
    std::cout << "--- search test ---" << std::endl;
    skipList.searchElement("3");
    skipList.searchElement("中文");
    skipList.searchElement("4");

    // delete test
    std::cout << "--- delete test ---" << std::endl;
    skipList.deleteElement("3");  // 成功
    skipList.deleteElement("5");  // 失败
    std::cout << "skipList size after deleteElement(): " << skipList.size() << std::endl; // 4

    // display test
    std::cout << "--- display test ---" << std::endl;
    skipList.displayList();
    
    // clear test
    std::cout << "--- clear test ---" << std::endl;
    skipList.clear();
    std::cout << "skipList size after clear(): " << skipList.size() << std::endl; // 0
    return 0;
}
