cmake_minimum_required(VERSION 3.0)
project(Skiplist)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)   # 强制使用指定的C++标准
add_compile_options(-Wall) # 编译选项

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


# 设置全局链接库
set(LIBS
    pthread
)

# 编译所有测试文件
add_executable(test_skiplist "test/test_skiplist.cpp")
add_executable(test_insert "test/test_insert.cpp")
add_executable(test_file "test/test_file.cpp")
add_executable(stress_test "test/stress_test.cpp")


target_link_libraries(test_skiplist ${LIBS})
target_link_libraries(test_insert ${LIBS})
target_link_libraries(test_file ${LIBS})
target_link_libraries(stress_test ${LIBS})

