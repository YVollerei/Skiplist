#pragma once

#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <cstring>
#include <mutex>
#include <boost/lexical_cast.hpp>


// 跳表落盘的路径
#define STORE_FILE "../store/skiplistDumpFile"

// 全局互斥量
std::mutex mtx;
// 连接键值的分隔符
std::string delimiter = ":";



template<typename K, typename V>
class Node{
public:
    Node(const K& k, const V& v, int level)
        : m_key(k)
        , m_value(v)
        , m_level(level){
        // 层数的索引范围为[0, level]
        forward = std::vector<Node<K, V>*>(level + 1, nullptr);
    }

    ~Node(){
        // forward数组中节点的删除由SkipList的析构函数处理，这里什么都不用做
    }

    K getKey() const { return m_key; }

    V getValue() const { return m_value; }

    void setValue(const V& v) { m_value = v; }

    int getLevel() const { return m_level; }

public:
    // 指针数组，存储每一层的下一节点
    std::vector<Node<K, V>*> forward;

private:
    K m_key;
    V m_value;
    int m_level;    // 节点的最高层级
};


template<typename K, typename V>
class SkipList{
public:
    // 构造函数
    SkipList(int maxLevel = 16, double p = 0.5);
    
    // 析构函数
    ~SkipList();

    // 向跳表中插入节点，如果节点已存在则返回1，否则插入节点并返回0
    int insertNode(K key, V val);

    // 打印所有节点
    void displayList() const;

    // 搜索节点是否存在
    bool searchNode(K key) const;

    // 删除节点
    void deleteNode(K key);

    // 将跳表持久化到文件
    void dumpFile();

    // 从文件中载入跳表
    void loadFile();

    // 递归地删除指定节点后的所有节点（包括指定节点）
    void clear(Node<K, V>* node);

    // 获取跳表中的节点数
    int size() const { return m_count; }

private:
    // 随机获取节点层数
    int getRandomLevel();

    // 创建跳表节点
    Node<K, V>* createNode(K key, V val, int level);

private:
    // 跳表允许的最大层数
    int m_maxLevel;
    // 跳表的当前实际的最大层数
    int m_curLevel;
    // 节点升层的概率，默认为0.5
    double m_p;

    // 跳表的头节点
    Node<K, V>* m_header;
    // 跳表的当前节点数
    int m_count;

    // 文件写入器
    std::ofstream m_fileWriter;
    // 文件读取器
    std::ifstream m_fileReader;
};


template<typename K, typename V>
SkipList<K, V>::SkipList(int maxLevel, double p)
    : m_maxLevel(maxLevel)
    , m_curLevel(0)
    , m_count(0){
    if(p < 0 || p > 1){
        throw std::invalid_argument("p should in range[0, 1]");
    }
    m_p = p;

    // 以默认值初始化头节点，头节点的层数为最大层数
    K key;
    V val;
    m_header = new Node<K, V>(key, val, maxLevel);
}

template<typename K, typename V>
SkipList<K, V>::~SkipList(){
    if(m_fileReader.is_open()){
        m_fileReader.close();
    }
    if(m_fileWriter.is_open()){
        m_fileWriter.close();
    }

    // 递归地删除所有节点
    if(m_header->forward[0]){
        clear(m_header->forward[0]);
    }
    delete(m_header);
}

template<typename K, typename V>
int SkipList<K, V>::getRandomLevel(){
    // 高效的随机数生成器
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<double> dis(0.0, 1.0);

    int level = 1;
    while(level < m_maxLevel && dis(gen) <= m_p){
        ++level;
    }

    return level;
}

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::createNode(K key, V val, int level){
    return new Node<K, V>(key, val, level);
}

template<typename K, typename V>
bool SkipList<K, V>::searchNode(K key) const {
    // 从头节点开始遍历
    Node<K, V>* current = m_header;

    // 从最高层开始搜索
    for(int i = m_curLevel; i >= 0; --i){
        // 判断是否要移动到同层的下一节点
        while(current->forward[i] && current->forward[i]->getKey() < key){
            current = current->forward[i];
        }
        // 无法移动到下一节点了，选择降层操作（--i），直到降到最底层
    }

    current = current->forward[0];
    if(current && current->getKey() == key){
        // std::cout << "Found Key: " << key << ", value=" << current->getValue() << std::endl;
        return true;
    }

    // std::cout << "Not Found Key: " << key << std::endl;
    return false;
}


template<typename K, typename V>
int SkipList<K, V>::insertNode(K key, V val){
    mtx.lock();

    // <搜索插入位置>
    Node<K, V>*  current = m_header;
    // 存储插入位置每一层的上一节点
    std::vector<Node<K, V>*> update(m_maxLevel + 1, nullptr);

    // 从最高层开始搜索
    for(int i = m_curLevel; i >= 0; --i){
        while(current->forward[i] && current->forward[i]->getKey() < key){
            // 同层移动
            current = current->forward[i];
        }
        // 降层前先记录节点
        update[i] = current;
    }

    current = current->forward[0];
    if(current && current->getKey() == key){
        mtx.unlock();
        return 1;
    }

    // <在对应位置每层插入节点>
    // 随机获取层数
    int level = getRandomLevel();
    // std::cout << "Insert: random level = " << level << std::endl;

    // 更新m_curLevel
    if(level > m_curLevel){
        for(int i = m_curLevel + 1; i <= level; ++i){
            update[i] = m_header;
        }
        m_curLevel = level;
    }

    Node<K, V>* inserted_node = createNode(key, val, level);
    // 逐层插入节点
    for(int i = 0; i <= level; ++i){
        inserted_node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = inserted_node;
    }
    ++m_count;

    mtx.unlock();
    return 0;
}


template<typename K, typename V>
void SkipList<K, V>::deleteNode(K key){
    mtx.lock();

    Node<K, V>* current = m_header;
    std::vector<Node<K, V>*> update(m_maxLevel + 1, nullptr);

    for(int i = m_curLevel; i >= 0; --i){
        while(current->forward[i] && current->forward[i]->getKey() < key){
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    // 确认指定节点确实在跳表中
    if(current && current->getKey() != key){
        mtx.unlock();
        return;
    }

    // 逐层移除节点
    for(int i = 0; i <= current->getLevel(); ++i){
        update[i]->forward[i] = current->forward[i];
    }

    // 更新m_curLevel
    while(m_curLevel > 0 && !m_header->forward[m_curLevel]){
        // 头节点该层forward指向空，表示该层没有节点
        --m_curLevel;
    }

    // 释放被移除节点的内存
    delete current;
    --m_count;

    mtx.unlock();
}

template<typename K, typename V>
void SkipList<K, V>::displayList() const {
    for(int i = m_curLevel; i >= 0; --i){
        auto node = m_header->forward[i];
        std::cout << "Level " << i << ": ";

        while(node){
            std::cout << node->getKey() << ":" << node->getValue();
            if(node->forward[i]){
                std::cout << " -> ";
            }
            node = node->forward[i];
        }
        std::cout << '\n';
    }
    std::cout.flush();
}

template<typename K, typename V>
void SkipList<K, V>::dumpFile(){
    m_fileWriter.open(STORE_FILE);
    // std::cout << "dumpFile: open status: " << (m_fileWriter.is_open() ? "Success" : "Failed") << std::endl;

    auto node = m_header->forward[0];

    while(node){
        m_fileWriter << node->getKey() << delimiter << node->getValue() << '\n';
        node = node->forward[0];
    }

    m_fileWriter.flush();
    m_fileWriter.close();
}


// 抽象的数据转换方法，用于读取数据时将读取string转换为目标类型，可通过偏特化支持任意类型
template<typename F, typename T>
class Lexical_cast{
public:
    T operator()(const F& val) const {
        return boost::lexical_cast<T>(val);
    }
};

template<typename K, typename V>
void SkipList<K, V>::loadFile(){
    m_fileReader.open(STORE_FILE);

    std::string line;
    std::string key;
    std::string val;

    while(getline(m_fileReader, line)){
        auto it = line.find(delimiter);
        if(it == std::string::npos){
            // 本行数据不合法（空字符串或没有分隔符），跳过本行
            continue;
        }
        key = line.substr(0, it);
        val = line.substr(it + 1);
        if(key.empty() || val.empty()){
            // 键或值为空，跳过本行
            continue;
        }

        // 向跳表中插入节点
        insertNode(Lexical_cast<std::string, K>()(key), Lexical_cast<std::string, V>()(val));

        // std::cout << "loadFile: key: " << key << ", value: " << val << std::endl;
    }

    m_fileReader.close();
}

template<typename K, typename V>
void SkipList<K, V>::clear(Node<K, V>* node){
    if(node->forward[0]){
        clear(node->forward[0]);
    }
    delete node;
}

