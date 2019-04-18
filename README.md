#### DataStructure

#### Build and Run
Compiler: g++-8 / clang-7; C++ Standard: C++17; OS: Linux Mint19

##### List
提供多重嵌套链表,用initialize-list实现显式的多重嵌套链表
赋值、合并、展开操作,比如List<int> list = {{4,5,{6,7}},{7,8}};
list.append(5);list.append({4,{5}});
list = List<int>::flat(list)等.

#### MultiArray
用模板实现的任意维度数组,尚未完成基本功能