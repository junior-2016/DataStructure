#### DataStructure

#### Build and Run
Compiler: g++-8 / clang-7; C++ Standard: C++17; OS: Linux Mint19

##### List
提供多类型数据的多重嵌套链表,用initialize-list实现显式的嵌套链表
赋值、合并、展开操作.  
Example:
```c++
void test() {
    DS::List list({{{1, 2, 3}, {1, 2.14, 3.1415926535}}, 5, {6, {1, 5}, {33, {24, 5}}},
                   {{4}, {1, 2}}, 88, {1, 5}, {1, 3, std::string("Hello World"), "My favorite"}, 7,
                   {1, {"25", 1.23}, std::vector<int>({1, 2, 3})}});

    std::cout << list << list.size() << "\n";

    DS::List flat_list = DS::List::flat(list);

    std::cout << flat_list << flat_list.size() << "\n";

    for (const auto &i : list) { // for-ranged loop
        std::cout << i;
    }

    list.append(1);
    std::cout << list << list.size() << "\n";

    list.append(DS::List(6));
    std::cout << list << list.size() << "\n";

    list.append({5, 5, {7, 8}});
    std::cout << list << list.size() << "\n";

    list.insert(list.cbegin() + 2, DS::List({"insert list in here"}));
    std::cout << list << list.size() << "\n";

    list.insert(list.cbegin(), DS::List(std::string("insert string in here")));
    std::cout << list << list.size() << "\n";
}
```
```output
{{{1,2,3},{1,2.140000,3.141593}},5,{6,{1,5},{33,{24,5}}},{{4},{1,2}},88,{1,5},{1,3,Hello World,"My favorite"},7,{1,{"25",1.230000},[1,2,3]}}
28
{1,2,3,1,2.140000,3.141593,5,6,1,5,33,24,5,4,1,2,88,1,5,1,3,Hello World,"My favorite",7,1,"25",1.230000,[1,2,3]}
28
{{1,2,3},{1,2.140000,3.141593}}
5
{6,{1,5},{33,{24,5}}}
{{4},{1,2}}
88
{1,5}
{1,3,Hello World,"My favorite"}
7
{1,{"25",1.230000},[1,2,3]}
{{{1,2,3},{1,2.140000,3.141593}},5,{6,{1,5},{33,{24,5}}},{{4},{1,2}},88,{1,5},{1,3,Hello World,"My favorite"},7,{1,{"25",1.230000},[1,2,3]},1}
29
{{{1,2,3},{1,2.140000,3.141593}},5,{6,{1,5},{33,{24,5}}},{{4},{1,2}},88,{1,5},{1,3,Hello World,"My favorite"},7,{1,{"25",1.230000},[1,2,3]},1,6}
30
{{{1,2,3},{1,2.140000,3.141593}},5,{6,{1,5},{33,{24,5}}},{{4},{1,2}},88,{1,5},{1,3,Hello World,"My favorite"},7,{1,{"25",1.230000},[1,2,3]},1,6,{5,5,{7,8}}}
34
{{{1,2,3},{1,2.140000,3.141593}},5,{"insert list in here"},{6,{1,5},{33,{24,5}}},{{4},{1,2}},88,{1,5},{1,3,Hello World,"My favorite"},7,{1,{"25",1.230000},[1,2,3]},1,6,{5,5,{7,8}}}
35
{insert string in here,{{1,2,3},{1,2.140000,3.141593}},5,{"insert list in here"},{6,{1,5},{33,{24,5}}},{{4},{1,2}},88,{1,5},{1,3,Hello World,"My favorite"},7,{1,{"25",1.230000},[1,2,3]},1,6,{5,5,{7,8}}}
36
```
#### MultiArray
用模板实现的任意维度数组