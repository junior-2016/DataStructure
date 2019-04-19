//
// Created by junior on 19-4-16.
//

#ifndef DATASTRUCTURE_VARIANT_H
#define DATASTRUCTURE_VARIANT_H

#include <type_traits>
#include <typeinfo>
#include <utility>
#include <string>
#include <set>
#include "Util.h"

/**
 * 仿照 std::variant 实现 DS::variant (variant是C++上union的替代品)
 * std::variant 实现代码: https://github.com/mpark/variant
 * 一个简易版本的实现: https://gist.github.com/tibordp/6909880
 */
namespace DS {
    // 模板递归求变长模板参数值(size_t类型)的最大值
    template<size_t S, size_t ...Ss>
    struct max;
    template<size_t S>
    struct max<S> {
        static const size_t size_ = S;
    };
    template<size_t S1, size_t S2, size_t ... Ss>
    struct max<S1, S2, Ss...> {
        static const size_t size_ = (S1 >= S2) ? max<S1, Ss...>::size_ : max<S2, Ss...>::size_;
    };

    // 模板递归,构造集合 {typeid(t).hash_code()}(for t in Ts...).
    // 这是为了检查后面variant.set<T>的类型T有没有超出variant的范围.
    /*
    template<typename...T>
    struct type_set;

    template<typename T, typename... Ts>
    struct type_set<T, Ts...> {
        static std::set<size_t> set;

        type_set() {
            set.insert(typeid(T).hash_code());
            set.insert(type_set<Ts...>::set.begin(), type_set<Ts...>::set.end());
        }
    };

    template<>
    struct type_set<> {
        static std::set<size_t> set;
    };
     */
    // 修正:使用模板递归bool值来判断,而不是set.
    template<typename...>
    struct is_one_of;
    template<typename T, typename S, typename... Ts>
    struct is_one_of<T, S, Ts...> {
        static constexpr bool value = std::is_same<T, S>::value || is_one_of<T, Ts...>::value;
    };
    template<typename T>
    struct is_one_of<T> {
        static constexpr bool value = false;
    };

    // 模板递归解决内存分配回收
    template<typename ... Ts>
    struct variant_helper;

    template<typename T, typename ... Ts>
    struct variant_helper<T, Ts...> {
        // 析构
        inline static void destroy(size_t type_id, void *data) {
            if (type_id == typeid(T).hash_code()) {
                reinterpret_cast<T *>(data)->~T();
            } else {
                variant_helper<Ts...>::destroy(type_id, data);
            }
        }

        inline static void move(size_t type_id, void *old_data, void *new_data) {
            if (type_id == typeid(T).hash_code()) {
                // 使用 placement new 特性.
                // new (new_data /* new_data所指向的空间必须满足T对象的对齐大小(预先开辟) */) T(construct func)
                new(new_data)T(std::move(*(reinterpret_cast<T *>(old_data))));
                // 使用std::move()将对象*((T*)old_data)变为右值,调用T(T&&t)的右值版本构造.
                // 并且old_data在执行std::move就失去对原来内存的管理
            } else {
                variant_helper<Ts...>::move(type_id, old_data, new_data);
            }
        }

        inline static void copy(size_t type_id, void *old_data, void *new_data) {
            if (type_id == typeid(T).hash_code()) {
                new(new_data)T(*(reinterpret_cast<const T *>(old_data))); // 注意这里cast的时候cast为const T*.
                // 跟move()不同在于,copy方法不会让old_data丢失对原来内存的管理,因为cast得到的对象是:*((const T*)old_data),
                // new_data所在的对象是通过T(T&t)的左值版本构建的.
            } else {
                variant_helper<Ts...>::copy(type_id, old_data, new_data);
            }
        }
    };

    // 当所有类型都匹配不上type_id时才会调用到这个模板. 可以认为是传递的类型不在DS::variant定义的范围内.
    template<>
    struct variant_helper<> {
        inline static void destroy(size_t, void *) {}

        inline static void move(size_t, void *, void *) {}

        inline static void copy(size_t, void *, void *) {}
    };

    template<typename ...Ts>
    class variant {
    private:
        // 计算variant内最大的类型字节大小.
        // 这里 sizeof(Ts)... 可以认为是对所有的类型计算字节大小得到的新的变长模板参数.
        // 这是一种 op(type_list)... 的语法,即对模板参数列表type_list的所有参数施加一样的操作op.
        static const size_t size = max<sizeof(Ts)...>::size_;
        // 计算需要的最大对齐大小
        static const size_t align = max<alignof(Ts)...>::size_;

        // 使用std::aligned_storage<类型字节大小,对齐大小>::type 创建一个要求大小size,对齐align的类型
        // 注意加上typename前缀,因为这个类型是需要推断的(size,align都是需要推断计算得到的)
        using data_t = typename std::aligned_storage<size, align>::type;

        // 这里不需要typename前缀,因为variant_helper<Ts...>里的Ts...不需要推断(创建variant对象的时候Ts就确定了)
        using helper_t = variant_helper<Ts...>;

        // 无效类型ID
        static inline size_t invalid_type() {
            return typeid(void).hash_code();
        }

        size_t type_id;
        data_t data;
    public:
        variant() : type_id(invalid_type()) {}

        variant(const variant<Ts...> &old) : type_id(old.type_id) {
            helper_t::copy(type_id, &old.data, &data);
        }

        variant(variant<Ts...> &&old) noexcept : type_id(old.type_id) {
            helper_t::move(type_id, &old.data, &data);
        }

        // 赋值构造,不使用左值引用参数,而是普通的左值参数,这样传递参数的时候,
        // 会copy构造一个对象old,后面我们直接swap这个old对象的内存空间到自己的内存空间即可.
        // 当然你也可以使用 T& operator=(T&t){ copy() ;},这样传递参数的时候不会有多余构造,
        // 而是显式地在operator=函数体里面执行copy,两种开销一样.
        variant<Ts...> &operator=(variant<Ts...> old) {
            std::swap(type_id, old.type_id);
            std::swap(data, old.data);
            return *this;
        }

        ~variant() {
            helper_t::destroy(type_id, &data);
        }

        /*
         * 使用std::enable_if<is_one_of<T,Ts...>::value,void>::type,
         * 在编译期就可以判断set<T>(Args&&...args)的类型T有没有超过variant规定的范围.
         */
        template<typename T, typename ... Args,
                typename = typename std::enable_if<is_one_of<T, Ts...>::value, void>::type>
        void set(Args &&... args) {
            // 这里是需要推断类型(需要推断Args所有类型)的右值引用参数列表,实际上是通用引用参数列表,
            // 既可以接受左值参数也可以接收右值参数,因此后面将args转发给T的构造函数时,
            // 需要先用std::forward<T>处理一下,确保转发给构造函数的时候右值参数还是右值参数,左值参数还是左值参数,
            // 如果没有std::forward处理,就会被一律当做左值参数转发给T的构造函数..
            helper_t::destroy(type_id, &data);
            new(&data)T(std::forward<Args>(args)...);//注意这里要写成std::forward<Args>(args)... 这属于op(TypeList)...语法
            type_id = typeid(T).hash_code();
        }

        // 注意下面三个函数全部const,然后data_ptr返回const T*
        template<typename T>
        bool is_type() const {
            return (type_id == typeid(T).hash_code());
        }

        template<typename T>
        const T *data_ptr() const {
            return reinterpret_cast<const T *>(&data);
        }

        template<typename T>
        bool is_valid() const {
            return (type_id != invalid_type());
        }
    };

    class variant_access_error : public std::logic_error {
    public:
        explicit variant_access_error(const std::string &str) : logic_error(str.c_str()) {}
    };


    // 通过 std::enable_if 加上 is_one_of<T,Ts...> 可以判断T是否符合范围Ts...
    // 调用get时,只需要显式提供类型T,至于Ts...可以通过参数推断.
    // 即 get<int>(variant), 显式提供类型T=int, 而Ts...取决于参数variant持有的类型范围
    template<typename T, typename ...Ts,
            typename = typename std::enable_if<is_one_of<T, Ts...>::value, void>::type>
    const T &get(const variant<Ts...> &v) {
        // 注意这里调用的时候需要加上template前缀, 跟typename前缀类似.
        // 当你调用某个模板函数f<T>,但是模板函数的类型T是需要推断出来时,就需要加上这个template前缀,
        // 比如这里的variant在调用模板成员函数is_type<T>()时,传递给它的类型T是需要推断的(只有调用get<T>(variant)的时候才能得到),
        // 所以要加上template前缀.
        if (v.template is_type<T>()) {
            return *(v.template data_ptr<T>()); // data_ptr返回const T*,解绑后返回const T&
        } else {
            // 虽然已经检查了T在variant的范围Ts...内,但这里还是需要检查当前variant对象的类型是否为T,如果不是就需要抛出异常
            throw DS::variant_access_error("The type of variant is not [" + DS::type_name<T>() + "] now!!!");
        }
    }

}
#endif //DATASTRUCTURE_VARIANT_H
