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