//
// Created by junior on 19-4-17.
//
#ifndef DATASTRUCTURE_DATASTRUCTUREIMPL_H
#define DATASTRUCTURE_DATASTRUCTUREIMPL_H

#include "DataStructure.h"

namespace DS {
    /**
     * 测试要点:
     * 1. 测试时为了防止名称空间发生污染(比如std::to_string和DS::to_string),不使用using指令;
     */

    void TEST_MULTI_ARRAY() {
        std::cout << "--------------TEST MULTI ARRAY----------------\n";
        DS::MultiArray<int, 4, 5, 6, 7, 8, 10, 11> array1;
        std::cout << array1[2][3][4][5][6][4][8] << std::endl;
        std::cout << "----------------------------------------------\n\n";
    }

    void TEST_LIST() {
        std::cout << "-------------TEST LIST--------------------\n";

        DS::List list({{{1, 2, 3}, {1, 2, 3.1415926535}}, 5, {6, 7, {1, 4, 5}, {33, {24, 5}}},
                       {{4}, {1, 2}}, 88, {1, 5}, {1, 3, std::string("Hello World"), "My favorite"}, 7});
        DS::List list1({1, {"25", 1.23}, std::vector<int>({1, 2})});
        std::cout << list1 << list1.size() << "\n";

        std::cout << DS::List::flat(list1);

        std::cout << list << list.size() << "\n";

        DS::List flat_list = DS::List::flat(list);
        std::cout << flat_list << flat_list.size() << "\n";

        for (const auto &i : list) { // for-ranged loop
            std::cout << i;
        }

        list.append(1);
        std::cout << list << list.size() << "\n";

        list.append(List(6));
        std::cout << list << list.size() << "\n";

        list.append({5, 5, {7, 8}});
        std::cout << list << list.size() << "\n";

        list.append(flat_list);
        std::cout << list << list.size() << "\n";

        list.insert(list.cbegin() + 2, List({"insert list in here"}));
        std::cout << list << list.size() << "\n";

        list.insert(list.cbegin(), List(std::string("insert string in here")));
        std::cout << list << list.size() << "\n";

        std::cout << DS::List::flat(list).size() << "\n";

        std::cout << "---------------------------------------------\n\n";
    }

    void TEST_ANY() {
        std::cout << "---------------TEST ANY -----------------------\n";
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
        std::cout << "-------------------------------------------------\n\n";
    }

    void TEST_VARIANT() {
        std::cout << "--------------TEST VARIANT------------------------\n";
        DS::variant<int, double, std::string, std::vector<int>, std::vector<std::string>> variant;
        try {
            variant.set<int>(5);
            std::cout << DS::get<int>(variant) << "\n";

            variant.set<double>(5.23);
            std::cout << DS::get<double>(variant) << "\n";

            variant.set<std::string>("Hello world");
            std::cout << DS::get<std::string>(variant) << "\n";

            variant.set<std::vector<int>>(std::vector<int>{5, 4, 3, 7, 10});
            std::cout << DS::to_string(DS::get<std::vector<int>>(variant)) << "\n";

            variant.set<std::vector<std::string>>(std::vector<std::string>{"Hello world", "Wo hhh"});
            std::cout << DS::to_string(DS::get<std::vector<std::string>>(variant)) << "\n";

            variant.set<double>(3.14);
            std::cout << DS::to_string(DS::get<std::vector<int>>(variant)) << "\n"; // 这里会抛出异常....
        } catch (DS::variant_access_error &e) {
            std::cout << e.what() << "\n";
        }
        std::cout << "---------------------------------------------------\n";
    }

    void TEST_CONTAINER_TO_STRING() {
        std::cout << "--------------TEST CONTAINER TO STRING--------------\n";
        std::vector<int> vector = {1, 2};
        std::vector<int> empty_vector;
        std::vector<int> single_vector = {1};
        std::cout << DS::to_string(vector) << "\n";
        std::cout << DS::to_string(vector, ";") << "\n";
        std::cout << DS::to_string(empty_vector) << "\n";
        std::cout << DS::to_string(single_vector) << "\n";
        std::cout << "---------------------------------------------------\n\n";
    }

    void test() {
        TEST_CONTAINER_TO_STRING();
        TEST_MULTI_ARRAY();
        TEST_LIST();
        TEST_ANY();
        TEST_VARIANT();
    }
}
#endif //DATASTRUCTURE_DATASTRUCTUREIMPL_H
