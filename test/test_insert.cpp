#include <iostream>
#include "../skiplist.h"



int main(){
    SkipList<int, int> sl;

    sl.insertNode(1, 2);
    sl.insertNode(2, 3);
    std::cout << "Insert complete" << std::endl;

    sl.displayList();

    return 0;
}