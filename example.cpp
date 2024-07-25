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