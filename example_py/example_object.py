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
