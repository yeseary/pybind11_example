import example

# 调用 C++ 函数
print(example.add(1, 2))  # 输出: 3

# 使用 C++ 类
pet = example.Pet("Milo")
print(pet.getName())  # 输出: Milo
pet.setName("Otis")
print(pet.getName())  # 输出: Otis