cmake_minimum_required(VERSION 3.4...3.18)
project(example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILEON ON)


# 设置编译器
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")  # 设置c++编译器
set(CMAKE_OSX_ARCHITECTURES "x86_64")  #如果是mac-m1芯片，需要设置


# 将pybind11的cmake目录添加到cmake的搜索路径中
execute_process(
    COMMAND pybind11-config --cmakedir
    OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
list(APPEND CMAKE_PREFIX_PATH ${PYBIND11_CMAKE_DIR})
message(CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}")
find_package(pybind11 REQUIRED)

# 生产环境使用
add_compile_options(-g -O2)
set(CMAKE_BUILD_TYPE "Release")

# 设置debug模式
# set(CMAKE_BUILD_TYPE "Debug")


# 导出pybind11模块
pybind11_add_module(example example.cpp)
pybind11_add_module(example_gil example_gil.cpp)
pybind11_add_module(example_multi example_multi.cpp)
pybind11_add_module(example_object example_object.cpp)
