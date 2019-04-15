//
// Created by junior on 19-4-14.
//

#ifndef DATASTRUCTURE_LIST_H
#define DATASTRUCTURE_LIST_H

#include <vector>
#include <iostream>
#include <functional>
#include <numeric>

/**
 * 实现多重嵌套List,以及用于解开多重嵌套的flat方法.
 * eg: List<int> list = { {1,2,3}, 4, {5,6,7} }
 * 解析过程为: List<int> list = List<int>{ List<int>{1,2,3}, List<int>(4), List<int>{5,6,7} }
 */

// TODO: 下一步的实现是混合类型 List. 例如 List list = {1.23, "string", {1, 2, {'\n', 3.14}}}
// eg: 带混合类型的堆栈
/*  template<typename Allocator> // if need
 *  class Stack {
 *    private:
 *      char* top;
 *      char* end;
 *    public:
 *      template<typename T>
 *      FORCE_INLINE T* push(size_t count = 1){
 *          if( top+sizeof(T)*count >= end) { expand_stack(); }
 *          T * ret = reinterpret_cast<T>(top);
 *          top += sizeof(T)*count;
 *          return ret;
 *      }
 *      FORCE_INLINE template<typename T> T* push(size_t count){...}
 *      FORCE_INLINE template<typename T> T* pop(size_t count=1){..}
 *      FORCE_INLINE template<typename T> T* pop(size_t count){...}
 *      ~Stack(){
 *          //...
 *      }
 *  }
 *  usage:
 *  Stack<Allocator> stack_();
 *  // 泛型push
 *  bool addValue(){
 *      // placement new 就是不分配内存，由使用者给予内存空间来构建对象
 *      // 即: new (使用者提供的内存空间指针) construct_func(Args ...) ;
 *      new (stack_.template push<ValueType>()) ValueType (Args ...);
 *      return true;
 *  }
 *  // 一般push
 *  *(stack_.push<int>()) = 1;
 *  *(stack_.push<char>()) = 'c';
 *  .... 考虑到内存对齐,可能需要用 memcpy 把数据拷贝到push()提供的内存地址上.
 *
 *  // 释放:
 *  while(!stack_.empty()) { (stack_.template pop<ValueType>())->~ValueType(); }
 */

template<typename T>
struct List {
private:
    std::vector<T> data;
    std::vector<List<T>> lists;
    bool is_single = false;
    std::string flat_string;

    void getFlatString(const List<T> &list) {
        if (!list.data.empty()) {
            if (list.data.size() == 1 && list.is_single) {
                flat_string += (std::to_string(list.data[0]));
            } else {
                flat_string += ("{")
                               + (std::accumulate(
                        std::next(list.data.begin()),
                        list.data.end(),
                        std::to_string(list.data[0]),
                        [](std::string a, T b) -> std::string {
                            // 注意聚合函数的参数:首先把 list.data[0] 变为 std::string.
                            // 然后根据聚合函数参数,依次执行:
                            // f (init:string, data[1]:T) => result1:string;
                            // f (result1:string, data[2]:T) => result2:string;
                            // f (result2:string, data[3]:T) => result3:string;
                            // .......
                            // 如果聚合函数参数类型没有写对,会报错.
                            return std::move(a) + ',' + std::to_string(b);
                        }))
                               + ('}');
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
    typedef typename std::vector<List<T>>::const_iterator const_iterator;
    typedef typename std::vector<List<T>>::iterator iterator;

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

    List(T t) { // 解析由单个int形成的List对象,即0维度
        data.push_back(t);
        is_single = true;
        lists.push_back(*this);
        refresh_flat_string();
    }

    explicit List(std::vector<T> &vector) : data(vector) {
        for (auto &item:vector) {
            lists.push_back(List<T>(item));
        }
        refresh_flat_string();
    }

    List(std::initializer_list<T> list) { // 解析由多个int组成的List对象,即1维度
        for (auto &item:list) {
            data.push_back(item);
            lists.push_back(List<T>(item));
        }
        refresh_flat_string();
    }

    List(std::initializer_list<List<T>> list) { // 解析由多个list组成的List对象
        for (auto &item:list) {
            lists.push_back(item);
        }
        refresh_flat_string();
    }

    void append(const List<T> &list) {
        lists.push_back(list);
        refresh_flat_string();
    }

    static List<T> flat(const List<T> &list) {

        std::vector<T> record;

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
        std::function<void(const List<T> &)> f = [&f, &record](const List<T> &list) -> void {
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
        return List<T>(record);
    }

    // 展开输出 List
    friend std::ostream &operator<<(std::ostream &out, const List<T> &list) {
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

#endif //DATASTRUCTURE_LIST_H
