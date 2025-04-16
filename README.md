本项目是使用 C++ 开发、基于跳表实现的轻量级键值数据库。实现了插入数据、删除数据、查询数据、数据展示、生成持久化文件、恢复数据以及数据库大小显示等功能。

## 项目文件

- skiplist.h：跳表核心实现
- README：介绍文档
- bin：生成可执行目录文件
- CMakeLists.txt：CMake编译文件
- store：存储跳表数据落盘的文件
- test：存放测试文件
    - test_insert.cpp：insertNode与displayList接口测试程序
    - test_file.cpp：dumpFile与loadFile接口测试程序
    - test_skiplist.cpp：命令行交互式接口测试程序
    - stress_test：压力测试程序

## 提供接口

- insertNode：插入数据
- deleteNode：删除数据
- searchNode：查询数据
- displayList：展示已存数据
- dumpFile：数据落盘
- loadFile：加载数据
- size：返回数据规模

## 项目运行方式

### 直接使用

如果想自己写程序使用这个kv存储引擎，只需要在你的CPP文件中include skiplist.h 就可以了。

```cpp
#include "skiplist.h"

// 默认构造
SkipList skiplist1;

// 指定跳表层高与升层概率
SkipList skiplist2(32, 0.25);
```

### 运行测试

```bash
# cmake编译
mkdir build && cd build
cmake..
make
```

- 交互式测试
    
    ```bash
    cd bin
    ./test_skiplist
    
    # 测试insertNode: insert/i key value
    insert 1 2
    i 2 3
    i 3 4
    
    # 测试deleteNode: delete/del key
    delete 1
    del 2
    
    # 测试display: display/d
    display
    d
    
    # 测试dumpFile: dump
    dump
    
    # 测试loadFile: load
    load
    ```
    
- 压力测试
    
    ```bash
    cd bin
    ./stress_test
    ```
    

## 性能表现

- 测试环境
    - 测试平台：阿里云服务器
    - 操作系统：Ubuntu 22.04
    - CPU：2核
    - 内存：2G
    - 硬盘：40G
- 测试配置：
    - 跳表层高：32
    - 跳表升层概率：0.25
    - 并发线程数：8

### 插入操作

| 数据规模（万条） | 耗时（秒） |
| --- | --- |
| 10 | 0.286855 |
| 50 | 2.05853 |
| 100 | 4.99911 |

每秒可处理写请求数（QPS）：24.289w

### 搜索操作

| 数据规模（万条） | 耗时（秒） |
| --- | --- |
| 10 | 0.0800299 |
| 50 | 0.689983 |
| 100 | 1.67086 |

每秒可处理写请求数（QPS）：72.465w

## 项目参考

参考项目：https://github.com/youngyangyang04/Skiplist-CPP

跳表学习：[Redis 数据结构 | 小林coding](https://xiaolincoding.com/redis/data_struct/data_struct.html#%E8%B7%B3%E8%A1%A8)