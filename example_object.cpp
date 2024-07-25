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