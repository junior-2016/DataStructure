//
// Created by junior on 19-4-14.
//

#ifndef DATASTRUCTURE_LIST_H
#define DATASTRUCTURE_LIST_H

#include <vector>
#include <iostream>
#include <functional>
#include <numeric>
#include <variant>
#include <memory>
#include <sstream>
#include "Util.h"

namespace DS {
    /**
     * 实现多重嵌套多类型List.
     * eg: List list = { {1,2,3}, 4, {5,6,7,4.56}, std::string("c++ string"), "c string" }
     * 解析过程为: List list = List{ List({1,2,3}), List(4), List({5,6,7,4.56}),
     *                        List(std::string("c++ string")), List("c string") };
     * 这其中 List(const T& t) 是最重要最基础的构造函数.
     *
     * List要求储存各种类型数据,所以List的class本身不需要模板,取而代之的是在List的构造函数上使用模板,即:
     * template<typename T> List(const T& t), 这样就可以接收任意类型作为List的构造参数了.
     * 并且这里的模板构造函数调用时,不需要写成 List<Type> list(Arg), 原因很简单,直接通过传递的参数Arg就可以得到类型T的信息,不需要显式写出,
     * 显式写出反而是错误的(只有一种模板函数调用时需要声明类型信息: template<typename T> f(){...} 即没有利用T做参数的时候,但是这种情况基本可以忽略).
     * 因此, 正确的构造方式可以是 List list(5); List list(5.25); List list(std::string("list")); 等等
     * 但是,这样得到的多种类型数据怎么存在同一个容器里? 这时就要使用 std::any(C++17) 或者 boost::any 来管理.
     * 另一个问题是如何正确解析出容器里的每一个值,采用的方法是 std::any_cast<element_type>(element),
     * 但是 element_type 如何获得,这个就很麻烦了, 因为std::any只有一个type_info信息,完全不能基于type_info得到类型
     * (type_info只能得到类型的字符串名称), 那唯一的方法只能采用判断的的方法,限定在几个特殊的类型里:
     * eg: if ( std::any.type() == typeid(int) ) return int_type;
     *
     * 如果限制了范围,还可以用std::variant来实现,这样在构造List的时候也能进行限制.
     */

    class List {
    public:
        typedef std::variant<int, double, float, char, long, long long, long double,
                unsigned, unsigned long, unsigned long long, std::string, const char *,
                std::vector<int> > type;
    private:
        static std::string to_string(const type &t) {
            try {
                switch (t.index()) {
                    case 0:
                        return std::to_string(std::get<int>(t));
                    case 1:
                        return std::to_string(std::get<double>(t));
                    case 2:
                        return std::to_string(std::get<float>(t));
                    case 3:
                        return std::string("") + std::get<char>(t);
                    case 4:
                        return std::to_string(std::get<long>(t));
                    case 5:
                        return std::to_string(std::get<long long>(t));
                    case 6:
                        return std::to_string(std::get<long double>(t));
                    case 7:
                        return std::to_string(std::get<unsigned>(t));
                    case 8:
                        return std::to_string(std::get<unsigned long>(t));
                    case 9:
                        return std::to_string(std::get<unsigned long long>(t));
                    case 10:
                        return std::get<std::string>(t);
                    case 11:
                        return std::string(std::get<const char *>(t));
                    case 12:
                        auto &v = std::get<std::vector<int>>(t);
                        std::string s = "vector<int>[";
                        for (auto i = v.begin(); i != v.end(); ++i) {
                            s += std::to_string(*i) + (i + 1 == v.end() ? "" : ",");
                        }
                        return s += "]";
                }
            } catch (const std::bad_variant_access &e) {
                std::cerr << e.what() << "\n";
            }
        }

        std::vector<type> data; // 要求容器里塞各种类型的值,使用std::any对象管理.
        std::vector<List> lists;
        bool is_single = false;
        std::string flat_string;

        void getFlatString(const List &list) {
            if (!list.data.empty()) {
                if (list.data.size() == 1 && list.is_single) {
                    flat_string += (to_string(list.data[0]));
                } else {
                    // 去掉accumulate的写法,因为不好阅读和调试
                    /*
                    flat_string += ("{")
                                   + (std::accumulate(
                            std::next(list.data.begin()),
                            list.data.end(),
                            to_string(list.data[0]),
                            [](std::string a, type b) -> std::string {
                                // 注意聚合函数的参数:首先把 list.data[0] 变为 std::string.
                                // 然后根据聚合函数参数,依次执行:
                                // f (init:string, data[1]:type) => result1:string;
                                // f (result1:string, data[2]:type) => result2:string;
                                // f (result2:string, data[3]:type) => result3:string;
                                // .......
                                // 如果聚合函数参数类型没有写对,会报错.
                                return std::move(a) + ',' + to_string(b);
                            }))
                                   + ('}');
                    */
                    flat_string += "{";
                    for (auto i = list.data.begin(); i != list.data.end(); ++i) {
                        flat_string += to_string(*i) + (i + 1 == list.data.end() ? "" : ",");
                    }
                    flat_string += "}";
                }
                return;
            }
            flat_string += ("{");
            for (auto i = list.lists.begin(); i != list.lists.end(); ++i) {
                getFlatString(*i);
                flat_string += (i + 1 == list.lists.end() ? "" : ",");
            }
            flat_string += ("}");
        }

        void refresh_flat_string() {
            flat_string = "";
            getFlatString(*this);
        }

    public:
        // 在 List 里实现 range-for 循环.
        // 实现方式有两种:
        // 1.如果class内部已经持有vector/map/...的容器类,直接包装一次begin()/end()/begin() const/end() const/cbegin()/cend()就可以了.
        // 2.如果class内部是原生指针 T*data,就需要自己实现一个iterator.见:
        // https://stackoverflow.com/questions/8164567/how-to-make-my-custom-type-to-work-with-range-based-for-loops

        /*
         * 注意下面的类型用typedef重命名时,需要在类型前面加上typename.
         * 因为这里的类型带有<T>, 如果不加上,就会报 missing 'typename' prior to dependent type name 错误.
         * 这个特性称为 dependent names, 类似的还有一个:
         * 例如 T *t = template variable.template_function<T>(Args ...);
         * 调用某个模板函数,但是这个模板函数的类型是<T>,是未确定的(依赖于具体的运行情况),所以需要加上 template 前缀.
         */
        typedef typename std::vector<List>::const_iterator const_iterator;
        typedef typename std::vector<List>::iterator iterator;

        // 可修改的begin迭代器和end迭代器
        iterator begin() {
            return lists.begin();
        }

        iterator end() {
            return lists.end();
        }

        // 依旧调用vector的begin()和end()方法获取迭代器,但是加上const方法声明,从而得到const迭代器.
        // 注意从这里我们可以看出,在C++11里,函数签名除了看:函数名,参数列表外,还会看函数的const声明,
        // 因此这里的 [return-type] begin() const 与前面的 [return-type] begin() 不冲突.
        const_iterator begin() const noexcept {
            return lists.begin();
        }

        const_iterator end() const noexcept {
            return lists.end();
        }

        // 显式提供 const 迭代器
        const_iterator cbegin() const noexcept {
            return lists.cbegin();
        }

        const_iterator cend() const noexcept {
            return lists.cend();
        }

        List() = delete;

        template<typename T>
        List(const T &t) { // 解析由单个type组成的list
            data.push_back(type(t)); // 这里将 T&t 变为type类型(即std::variant),如果没有处于type的类型范围,编译的时候就会报错
            is_single = true;
            lists.push_back(*this);
            refresh_flat_string();
        }

        explicit List(std::vector<type> &vector) : data(vector) {
            for (auto &item:vector) {
                lists.emplace_back(item);
            }
            refresh_flat_string();
        }

        template<typename T>
        List(std::initializer_list<T> list) { // 解析由多个type组成的List对象
            for (auto &item:list) {
                data.push_back(type(item));
                lists.emplace_back(item);
            }
            refresh_flat_string();
        }

        List(std::initializer_list<List> list) { // 解析由多个list组成的List对象
            for (auto &item:list) {
                lists.push_back(item);
            }
            refresh_flat_string();
        }

        void append(const List &list) {
            lists.push_back(list);
            refresh_flat_string();
        }

        // TODO: 加入与vector对应的 insert 接口


        static List flat(const List &list) {
            std::vector<type> record;

            // 带auto-推导的递归lambda函数,因为类型推导需要解释整一个lambda表达式才能确定,
            // 因此无法用[&f1]捕获f1,故而无法直接在函数里递归调用f1.
            // 采用的策略时将函数本身作为参数,这样调用的时候需要多一个函数参数.
            // PS:使用[&]对所有外部变量捕获虽然方便,但是可能捕获到我们不要的变量
            // (比如这里的list外部变量),因此最好明确要捕获的变量.
            /*
            auto f_auto = [&record](auto &&self, const List<T> &list) -> void {
                if (list.lists.empty()) {
                    for (auto &item:list.data) {
                        record.push_back(item);
                    }
                }
                for (auto &item:list.lists) {
                    self(self, item);
                }
            };
            f_auto(f_auto,list);
            */

            // 直接写明函数类型的lambda函数,这种情况下可以直接用 [&f] 捕获函数变量,
            // 然后在函数体里直接递归调用.
            std::function<void(const List &)> f = [&f, &record](const List &list) -> void {
                if (!list.data.empty()) {
                    for (auto &item:list.data) {
                        record.push_back(item);
                    }
                    return;
                }
                for (auto &item:list.lists) {
                    f(item);
                }
            };
            f(list);
            return List(record);
        }

        // 展开输出 List
        friend std::ostream &operator<<(std::ostream &out, const List &list) {
            out << list.flat_string << "\n";
            return out;
        }

        auto operator[](const size_t &idx) const {
            return lists.at(idx);
        }

        auto size() const {
            return lists.size();
        }

    };
}
#endif //DATASTRUCTURE_LIST_H
