#include "src/DataStructure.h"
#include <iostream>

using namespace std;

void TEST_MULTI_ARRAY() {
    cout << "--------------TEST MULTI ARRAY----------------\n";
    DS::MultiArray<int, 4, 5, 6, 7, 8, 10, 11> array1;
    std::cout << array1[2][3][4][5][6][4][8] << std::endl;
    cout << "----------------------------------------------\n\n";
}

void TEST_LIST() {
    using namespace DS;
    cout << "-------------TEST LIST--------------------\n";

    List list({{{1, 2, 3}, {1, 2, 3.1415926535}}, 5, {6, 7, {1, 4, 5}, {33, {24, 5}}},
               {{4}, {1, 2}}, 88, {1, 5}, {1, 3, std::string("Hello World"), "My favorite"}, 7});
    List list1({1, {"25", 1.23}, std::vector<int>({1, 2})});
    cout << list1 << list1.size() << "\n";

    cout << List::flat(list1);

    cout << list << list.size() << "\n";

    List flat_list = List::flat(list);
    cout << flat_list << flat_list.size() << "\n";

    for (const auto &i : list) { // for-ranged loop
        cout << i;
    }

    list.append(1);
    cout << list << list.size() << "\n";

    list.append(List(6));
    cout << list << list.size() << "\n";

    list.append({5, 5, {7, 8}});
    cout << list << list.size() << "\n";

    list.append(flat_list);
    cout << list << list.size() << "\n";

    list.insert(list.cbegin() + 2, List({"insert list in here"}));
    cout << list << list.size() << "\n";

    list.insert(list.cbegin(), List(std::string("insert string in here")));
    cout << list << list.size() << "\n";

    cout << List::flat(list).size() << "\n";

    cout << "---------------------------------------------\n\n";
}

void TEST_ANY_VARIANT() {
    cout << "---------------TEST ANY VARIANT------------------\n";
    DS::any any(std::vector<int>{1, 2, 3});
    DS::any any1(2.36f);
    const double &d = 6.66;
    DS::any any2(d);
    std::cout << any.type() << "\n";
    std::cout << any1.type() << "\n";
    std::cout << any2.type() << "\n";
    try {
        std::cout << DS::any_cast<std::vector<int, std::allocator<int>>>(any).size() << "\n";
        std::cout << DS::any_cast<float>(any1) << "\n";
        std::cout << DS::any_cast<double>(any2) << "\n";
    } catch (const DS::any_cast_error &e) {
        std::cerr << e.what() << "\n";
    }
    cout << "-------------------------------------------------\n\n";
}

int main() {
    TEST_MULTI_ARRAY();
    TEST_ANY_VARIANT();
    TEST_LIST();
    return 0;
}