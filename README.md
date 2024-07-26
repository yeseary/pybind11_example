# pybind11 教程

## 用途

通过pybind11可以实现以下功能： 
1. 将python中实现慢的逻辑，使用C++代码重写，从而提升程序效率
2. 将现有的C++代码编译为python模块，减少重复开发，方便在python中集成

本次阐述通过pybind11实现以下几个功能：
1. C++中的对象或方法如何在python中使用
2. pybind11中的gil锁
3. python对象如何在C++中使用


## 安装方式
可以通过pip的方式安装：`pip install pybind11`
安装完成之后，可以执行`pybind11-config --cmakedir`确认CMake的配置

以下代码都已开源到github上：https://github.com/yeseary/pybind11_example

## 执行方式
```
# 如果有conda，可以先切换环境
conda activate py36
cd example_py
cmake .. && make -j
python example.py  # 最简单的实例代码
python example_gil.py  # gil锁
python example_multi.py  # 多线程
python example_object.py  # 对象传递

```


## CMakeLists的配置实例
以下是一个CMakeLists.txt的配置示例：
``` CMake
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
pybind11_add_module(example_gil example_gil.cpp)
pybind11_add_module(example_multi example_multi.cpp)
pybind11_add_module(example_object example_object.cpp)

```

## C++中的对象或方法如何在python中使用

example.cpp文件如下：
``` C++
#include <pybind11/pybind11.h>

namespace py = pybind11;

// 一个简单的 C++ 函数
int add(int i, int j) {
    return i + j;
}

// 一个简单的 C++ 类
class Pet {
public:
    Pet(const std::string &name) : name(name) {}

    void setName(const std::string &name_) { name = name_; }
    const std::string &getName() const { return name; }

private:
    std::string name;
};

// 绑定代码
PYBIND11_MODULE(example, m) {
    m.doc() = "pybind11 example plugin"; // 可选的模块文档字符串

    m.def("add", &add, "A function which adds two numbers");

    py::class_<Pet>(m, "Pet")
        .def(py::init<const std::string &>())
        .def("setName", &Pet::setName)
        .def("getName", &Pet::getName);
}
```

example.py文件如下：
``` python
import example

# 调用 C++ 函数
print(example.add(1, 2))  # 输出: 3

# 使用 C++ 类
pet = example.Pet("Milo")
print(pet.getName())  # 输出: Milo
pet.setName("Otis")
print(pet.getName())  # 输出: Otis
```

## pybind11中的gil锁
GIL是python中的一个底层锁，它是python解释器的全局锁，在python中，只有一个线程可以执行python的代码，所以python的多线程是无法利用多核CPU的，所以python的多线程是无法利用多核CPU的。
但是通过pybind11可以灵活的摆脱GIL锁的限制，使用多线程。

example_gil.cpp代码如下：
``` C++
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <chrono>
#include <thread>
#include "Python.h"
#include <iostream>

namespace py = pybind11;


// 一个模拟长时间运行的计算函数
void long_computation(bool no_gil) {
    // 释放 GIL
    // std::cout<<"no gil:"<<no_gil<<std::endl;
    if(no_gil){
        py::gil_scoped_release release;
         // 模拟长时间计算
        std::this_thread::sleep_for(std::chrono::seconds(5));
        py::gil_scoped_acquire acquire;
    } else{
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    
}

// 一个模拟计算密集型任务的函数
void compute(int thread_id, int num_iterations) {
    for (int i = 0; i < num_iterations; ++i) {
        // 模拟一些计算
        double result = 0.0;
        for (int j = 0; j < 10000; ++j) {
            result += j * 0.001;
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

// 一个多线程计算函数
void parallel_compute(int num_threads, int num_iterations) {
    // 释放 GIL
    py::gil_scoped_release release;

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(compute, i, num_iterations);
    }

    for (auto& t : threads) {
        t.join();
    }

    // 重新获取 GIL
    // py::gil_scoped_acquire acquire;
}

// 绑定代码
PYBIND11_MODULE(example_gil, m) {
    m.doc() = "pybind11 example plugin"; // 可选的模块文档字符串

    m.def("long_computation", &long_computation, "A function that performs a long computation", py::arg("no_gil"));
    m.def("parallel_compute", &parallel_compute, "A function that performs parallel computation",
          py::arg("num_threads"), py::arg("num_iterations"));
}

```

example_gil.py代码如下：
```
import time
import example_gil
import threading

def run_task(no_gil:bool):

    n = 3
    def run_computation(i):
        print("开始长耗时任务计算...")
        example_gil.long_computation(no_gil)
        print("开始长耗时任务计算结束.")

    start_time = time.time()
    # 创建并启动多个线程
    threads = [threading.Thread(target=run_computation, args=(i,)) for i in range(n)]
    for t in threads:
        t.start()

    # 等待所有线程完成
    for t in threads:
        t.join()
    end_time = time.time()
    print(f"释放GIL锁:{no_gil},时间: {end_time - start_time} seconds")

run_task(no_gil=True)
run_task(no_gil=False)

num_threads = 4
num_iterations = 10000

start_time = time.time()
example_gil.parallel_compute(num_threads, num_iterations)
end_time = time.time()

print(f"Parallel computation with {num_threads} threads took {end_time - start_time} seconds")
```

执行结果如下：
```
开始长耗时任务计算...
开始长耗时任务计算...
开始长耗时任务计算...
开始长耗时任务计算结束.
开始长耗时任务计算结束.
开始长耗时任务计算结束.
释放GIL锁:True,时间: 5.005779027938843 seconds
开始长耗时任务计算...
开始长耗时任务计算...
开始长耗时任务计算结束.
开始长耗时任务计算...
开始长耗时任务计算结束.
开始长耗时任务计算结束.
释放GIL锁:False,时间: 15.013175964355469 seconds
Parallel computation with 4 threads took 5.391555070877075 seconds
```

在C++中代码中，可以通过pybind11的接口进行GIL锁的管理，对于以下场景：
1. python单线程，C++多线程，可以利用多核；
2. python多线程，C++单线程，如果不释放GIL锁，串行执行，释放GIL锁后可以并行；
3. python多线程，C++多线程，同2；

## python对象如何在C++中使用

以下代码展示了如何将python中的列表、字典、函数通过pybind11传递到C++中使用

example_object.cpp的代码如下：
``` cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <iostream>

namespace py = pybind11;

// 一个函数，接受一个 Python 列表并返回其平方
std::vector<int> square_list(const std::vector<int>& input_list) {
    std::vector<int> result;
    for (int value : input_list) {
        result.push_back(value * value);
    }
    return std::move(result);
}

uint64_t pass_vec(const std::vector<int>& input_list) {
    return 0;
    // uint64_t result = 0;
    // for (int value : input_list) {
    //     result += value;
    // }
    // return result;
}

uint64_t pass_list(const py::list& input_list) {
    return 0;
    // uint64_t result = 0;
    // for (int value : input_list) {
    //     result += value;
    // }
    // return result;
}

// 一个函数，接受一个 Python 字典并打印其内容
void print_dict(const py::dict& input_dict) {
    // auto d = py::dict();
    // d["aaa"] = 1;
    // input_dict.attr("update")(d);
    // for (auto item : input_dict) {
    //     std::cout << "Key: " << py::str(item.first) << ", Value: " << py::str(item.second) << std::endl;
    // }
    // std::cout << "input_dict size: " << input_dict.size() << std::endl;
}

// 一个函数，接受一个 Python 函数并调用它
void call_python_function(const py::function& func, const py::dict& input_dict) {
    // 调用 Python 函数并获取结果
    py::object result = func(input_dict);
    std::cout << "Result from Python function: " << py::str(result) << std::endl;
}

// 绑定代码
PYBIND11_MODULE(example_object, m) {
    m.doc() = "pybind11 example plugin"; // 可选的模块文档字符串

    m.def("square_list", &square_list, "A function that squares a list of integers",
          py::arg("input_list"));
    m.def("pass_list", &pass_list, "pass list",
          py::arg("input_list"));
    m.def("pass_vec", &pass_vec, "pass vector",
          py::arg("input_list"));

    m.def("print_dict", &print_dict, "A function that prints a dictionary",
          py::arg("input_dict"));

    m.def("call_python_function", &call_python_function, "A function that calls a Python function",
          py::arg("func"), py::arg("input_dict"));
}

```

python代码`example_object.py`如下：
``` python
import time
import example_object as example
import timeit

# 测试 square_list 函数
start_time = time.time()
n = int(1e4)
input_list = list(range(n))

# 参数传递耗时
for _ in range(n):
    squared_list = example.pass_list(input_list)
time_cost = time.time() - start_time
print("run time:{}s".format(time_cost))
print("avg pass list time cost:{}ms".format(time_cost / n*1000))

for _ in range(n):
    squared_list = example.pass_vec(input_list)
time_cost = time.time() - start_time
print("run time:{}s".format(time_cost))
print("avg pass vec time cost:{}ms".format(time_cost / n*1000))


large_dict = {f"key{i}": i for i in range(10000000)}

def run():
    start_time = time.time()
    
    for _ in range(1000000):
        example.print_dict(large_dict)
    time_cost = time.time() - start_time
    print("run time:{}s".format(time_cost))
    print("avg time cost:{}ms".format(time_cost / 1000000*1000))

run()
# 测试 call_python_function 函数
def my_python_function():
    return "Hello from Python!"

input_dict = dict(a=1, b=2, c=3)
example.call_python_function(id, input_dict)
print("input_dict:", id(input_dict))

```
执行结果如下：
```
run time:0.0034627914428710938s
avg pass list time cost:0.00034627914428710934ms
run time:1.315690040588379s
avg pass vec time cost:0.1315690040588379ms
run time:0.30083274841308594s
avg time cost:0.00030083274841308596ms
Result from Python function: 140240346308040
input_dict: 140240346308040
```

**需要注意如果数据类型不一致的情况下，pybind11会进行类型转换，当数据量非常大的时候，这个转换耗时将不可忽略。因此，如果涉及到大量的数据转换，最好避免类型转换。**

## 参考
1. [给Python算法插上性能的翅膀——pybind11落地实践](https://cloud.tencent.com/developer/article/1919872)