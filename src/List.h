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
     * 并且这里的模板构造函数调用时,不需要写成 List<Type> list(Arg), 原因很简单,
     * 直接通过传递的参数Arg就可以得到类型T的信息,不需要显式写出,显式写出反而是错误的
     * ( PS: 只有一种情况,模板函数调用时需要声明类型信息,即没有利用 T 做参数的时候,例如下面:
     *   template<typename T> void f(){...} 或者 template<typename T> T f() {...} )
     * 因此, 正确的构造方式可以是 List list(5); List list(5.25); List list(std::string("list")); 等等
     * 但是,这样得到的多种类型数据怎么存在同一个容器里? 这时就要使用 std::any(C++17) 或者 boost::any 来管理.
     * 另一个问题是如何正确解析出容器里的每一个值,采用的方法是 std::any_cast<element_type>(element),
     * 但是 element_type 如何获得,这个就很麻烦了, 因为std::any只有一个type_info信息,完全不能基于type_info得到类型
     * (type_info只能得到类型的字符串名称), 那唯一的方法只能采用判断的的方法,限定在几个特殊的类型里:
     * eg: if ( std::any.type() == typeid(int) ) return int_type;
     *
     * 如果限制了范围,还可以用std::variant来实现,这样在构造List的时候也能进行限制.
     *
     * 后续更新:
     * 删除 std::initializer_list<T> 以及用于判断是由List({x})生成的还是List(x)生成的is_single.
     * 因为利用 List(std::initializer<List>) 构造器 和 接收任意类型的单一元素构造器List(T&t)就可以处理任何情况了,
     * 而限制同一类型的初始化列表作为参数的构造器是没有必要出现的.
     * 并且这样修改后,data容器持有的数据最多只有一个,所有持有数据的List都是单一元素List,那么
     * 后面需要修改data为: std::shared_ptr<type_t> data_ptr,
     * 然后通过data_ptr是否为null来判断List有没有持有数据.
     *
     * 通过这样的修改,可以明确List的数据结构其实就是一个支持任意多种类型数据作为叶子的多叉树,
     * List list = { {"a", {'b', 77) },{ 5, 6.36 }, 3.14f}
     *                       list
     *             A           B          C
     *        "a"     D     5    6.36    3.14f
     *            'b'   77
     * 构造函数返回的List对象就是多叉树的根,然后往下递归构造的List对象是多叉树的中间节点,最后递归
     * 构造的List(T&t)生成的List对象是叶子节点(叶子节点所在的List对象,其data字段非空指针,lists字段则没有任何元素).
     * List 的构造就是将序列化的字符串(类似Json字符串)转为多叉树数据结构,并且支持叶子节点为多种类型;
     * List 储存的flat_string就是遍历整个多叉树后形成的序列(树的序列化操作,但是生成的序列还加入了其他信息,和原序列有些不同了),
     * List 的静态方法flat其实就是对多叉树的一种遍历过程,并且返回一个只有两层的多叉树list(顶层为list,第二层都是叶子节点),
     * 你可以认为flat把所有的叶子收集起来,统一放在第二层.
     * List 的append方法是将一颗多叉树插在最后一个叶子节点上变成一个子树; insert 是往多叉树某个节点上插入一个新的子树
     *
     * 如果将List作为多叉树来看, 后面可以引入的新接口还有不少:
     * TODO: 引入多种多叉树遍历接口(先序/后序)
     *
     * 这样支持多种类型做叶子的多叉树有几种用途:
     * 1. 引入B+的设计,做成数据库储存结构(支持叶子节点多类型是基本要求) => 进一步做成数据库
     * 2. 浏览器或者文件系统的 Dom(Document) 文件树结构
     * 3. Json字符串解析后的解析树.
     * TODO: 考虑实现上面三条应用.
     */

    class List {
    public:
        /*
        typedef std::variant<int, double, float, char, long, long long, long double,
                unsigned, unsigned long, unsigned long long, std::string, const char *,
                std::vector<int> > type;*/
        // 使用 using xxx = type 来代替C语言的 typedef(类型别名)
        using type_t = std::variant<int, double, float, char, long, long long, long double,
                unsigned, unsigned long, unsigned long long, std::string, const char *,
                std::vector<int> >;
    private:
        static std::string to_string(const type_t &t) {
            try {
                switch (t.index()) {
                    case 0:
                        return std::to_string(std::get<int>(t));
                    case 1:
                        return std::to_string(std::get<double>(t));
                    case 2:
                        return std::to_string(std::get<float>(t));
                    case 3:
                        return std::string("\'") + std::get<char>(t) + std::string("\'");
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
                        return "std::string[\"" + std::get<std::string>(t) + "\"]";
                    case 11:
                        return "\"" + std::string(std::get<const char *>(t)) + "\"";
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
            return "";
        }

        std::shared_ptr<type_t> data = nullptr; // 当前List对象储存的数据指针,只有叶子节点该字段非空
        std::vector<List> lists;  // 当前List对象的所有子节点对象
        std::string flat_string;  // 以当前List对象为根节点的多叉树,序列化后生成的字符串
        size_t size_;              // 以当前List对象为根节点的多叉树中,所有叶子节点的数量

        void getFlatString(const List &list) {
            if (list.data != nullptr) {
                flat_string += to_string(*list.data);
                size_++; // 顺便更新size
                return;
            }
            flat_string += ("{");
            for (auto i = list.lists.begin(); i != list.lists.end(); ++i) {
                getFlatString(*i);
                flat_string += (i + 1 == list.lists.end() ? "" : ",");
            }
            flat_string += ("}");
        }

        void refresh() {
            flat_string = "";
            size_ = 0;
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
        /*
        typedef typename std::vector<List>::const_iterator const_iterator;
        typedef typename std::vector<List>::iterator iterator;*/
        // 使用 using xxx = type 来代替C语言的 typedef(类型别名)
        using const_iterator = typename std::vector<List>::const_iterator;
        using iterator = typename std::vector<List>::iterator;

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
        List(const T &t) { // 解析由单个值组成的list,这里可以接收任意类型,但是下面会通过type_t(t)在编译期检查是否合法
            data = std::make_shared<type_t>(type_t(t));// 这里将T&t变为type_t类型(即std::variant),如果没有处于type的类型范围,编译的时候就会报错
            // lists.push_back(*this); // 这里构造的List为多叉树叶子节点,所以lists容器为空
            refresh();
        }

        List(std::initializer_list<List> list) { // 解析由多个list组成的List对象,可以认为是解析多叉树中间节点
            for (auto &item:list) {
                lists.push_back(item);
            }
            refresh();
        }

        explicit List(std::vector<type_t> &vector) {
            for (auto &item:vector) {
                lists.emplace_back(item);
            }
            refresh();
        }

//        template<typename T>
//        List(std::initializer_list<T> list) {
//            for (auto &item:list) {
//                data.push_back(type_t(item));
//                lists.emplace_back(item);
//            }
//            refresh_flat_string();
//        }

        void append(const List &list) {
            lists.push_back(list);
            refresh();
        }

        iterator insert(iterator pos, const List &list) {
            auto ret = lists.insert(pos, list);
            refresh();
            return ret;
        }

        iterator insert(const_iterator pos, const List &list) {
            auto ret = lists.insert(pos, list);
            refresh();
            return ret;
        }

        iterator insert(const_iterator pos, List &&list) {
            auto ret = lists.insert(pos, list);
            refresh();
            return ret;
        }

        void insert(iterator pos, size_t count, const List &list) {
            lists.insert(pos, count, list);
            refresh();
        }

        iterator insert(const_iterator pos, size_t count, const List &list) {
            auto ret = lists.insert(pos, count, list);
            refresh();
            return ret;
        }

        iterator insert(const_iterator pos, std::initializer_list<List> llist) {
            auto ret = lists.insert(pos, llist);
            refresh();
            return ret;
        }

        template<class InputItr>
        void insert(iterator pos, InputItr first, InputItr last) {
            lists.insert(pos, first, last);
            refresh();
        }

        template<class InputItr>
        iterator insert(const_iterator pos, InputItr first, InputItr last) {
            auto ret = lists.insert(pos, first, last);
            refresh();
            return ret;
        }

        static List flat(const List &list) {
            std::vector<type_t> record;

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
                if (list.data != nullptr) {
                    record.push_back(*list.data);
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
            return size_;
        }

        auto empty() const {
            return size_ == 0;
        }
    };
}
#endif //DATASTRUCTURE_LIST_H
