#include "src/DataStructure.h"
#include <iostream>
#include <variant>

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
               {{4}, {1, 2}}, 88, {1, 5}, {1, 3, std::string("Hello World"),"My favorite"}, 7});
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

    cout << "---------------------------------------------\n\n";
}

int main() {
    TEST_LIST();
    return 0;
}