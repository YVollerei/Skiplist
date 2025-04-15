#include <iostream>
#include <string>
#include "../skiplist.h"


template<typename K, typename V>
class SkipListOperation{
public:
    virtual void run(SkipList<K, V>&) = 0;
};

template<typename K, typename V>
class InsertOperation: public SkipListOperation<K, V>{
public:
    void run(SkipList<K, V>& sl) override {
        K key;
        V val;
        std::cin >> key >> val;
        int rt = sl.insertNode(key, val);
        if(rt){
            std::cout << "Insert failed, Node existed!" << std::endl;
        } else {
            std::cout << "Insert success: key=" << key << ", value=" << val << std::endl;
        }
    }
};

template<typename K, typename V>
class deleteOperation: public SkipListOperation<K, V>{
public:
    void run(SkipList<K, V>& sl) override {
        K key;
        std::cin >> key;
        sl.deleteNode(key);
    }
};

template<typename K, typename V>
class SearchOperation: public SkipListOperation<K, V>{
public:
    void run(SkipList<K, V>& sl) override {
        K key;
        std::cin >> key;
        bool rt = sl.searchNode(key);
        if(rt){
            std::cout << "Found Key: " << key << std::endl;
        } else {
            std::cout << "Not Found Key: " << key << std::endl;
        }
    }
};

template<typename K, typename V>
class DisplayOperation: public SkipListOperation<K, V>{
public:
    void run(SkipList<K, V>& sl) override {
        sl.displayList();
    }
};

template<typename K, typename V>
class DumpOperation: public SkipListOperation<K, V>{
public:
    void run(SkipList<K, V>& sl) override {
        sl.dumpFile();
    }
};

template<typename K, typename V>
class LoadOperation: public SkipListOperation<K, V>{
public:
    void run(SkipList<K, V>& sl) override {
        sl.loadFile();
    }
};

template<typename K, typename V>
class OperationFactory{
public:
    SkipListOperation<K, V>* createOperation(const std::string& opname){
        if(opname == "insert" || opname == "i"){
            return new InsertOperation<K, V>();
        } else if(opname == "delete" || opname == "del"){
            return new deleteOperation<K, V>();
        } else if(opname == "search" || opname == "s"){
            return new SearchOperation<K, V>();
        } else if(opname == "display" || opname == "d"){
            return new DisplayOperation<K, V>();
        } else if(opname == "dump"){
            return new DumpOperation<K, V>();
        } else if(opname == "load"){
            return new LoadOperation<K, V>();
        } else {
            std::cout << "Invalid Operation!" << std::endl;
        }
        return nullptr;
    };
};


int main(){
    SkipList<int, int> sl;
    OperationFactory<int, int> factory;

    while(true){
        std::string opname;
        std::cin >> opname;
        if(opname == "quit" || opname == "q"){
            break;
        }
        auto operation = factory.createOperation(opname);
        if(operation){
            operation->run(sl);
        }
    }

    return 0;
}