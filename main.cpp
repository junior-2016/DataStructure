#include "src/DataStructure.h"
#include <iostream>

using namespace std;

void TEST_MULTI_ARRAY() {
    cout << "--------------TEST MULTI ARRAY----------------\n";
    MultiArray<int, 4, 5, 6, 7, 8, 10, 11> array1;
    std::cout << array1[2][3][4][5][6][4][8] << std::endl;
    cout << "----------------------------------------------\n\n";
}

void TEST_LIST() {
    cout << "-------------TEST LIST--------------------\n";

    List<int> list({{{1, 2, 3}, {1, 2, 3}}, 5, {6, 7, {1, 4, 5}, {33, {24, 5}}},
                    {{4}, {1, 2}}, 88, {1, 5}, {1, 3}, 7});

    List<int> flat_list = List<int>::flat(list);

    cout << list << list.size() << "\n";
    cout << flat_list << flat_list.size() << "\n";


    for (const auto &i : list) { // for-ranged loop
        cout << i;
    }

    cout << "---------------------------------------------\n\n";
}

int main() {
    TEST_LIST();
    return 0;
}