#include "../skiplist.h"
#include <iostream>




int main(){
    SkipList<int, int> sl1;

    sl1.insertNode(1, 2);
    sl1.insertNode(3, 2);
    sl1.insertNode(5, 2);
    sl1.insertNode(2, 2);
    sl1.insertNode(6, 2);

    std::cout << "-----SkipList 1-----" << std::endl;
    sl1.displayList();
    sl1.dumpFile();

    SkipList<int, int> sl2;
    sl2.loadFile();
    std::cout << "\n-----SkipList 2-----" << std::endl;
    sl2.displayList();
}