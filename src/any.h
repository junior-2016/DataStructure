//
// Created by junior on 19-4-16.
//

#ifndef DATASTRUCTURE_ANY_H
#define DATASTRUCTURE_ANY_H

#include <memory>
#include "Util.h"

namespace DS {
    /**
     * 仿照 std::any 实现 DS::any.
     */
    class Base {
    public:
        virtual std::string type() = 0;
    };

    template<typename T>
    class Child : public Base {
        std::string type_info;
        T value;
    public:
        std::string type() override { return type_info; }

        explicit Child(T t) : value(t), type_info(DS::type_name<decltype(t)>()) {}

        T getValue() const { return value; }
    };

    class any {
    private:
        std::shared_ptr<Base> ptr;
    public:
        auto getPtr() const { return ptr; }

        template<typename T>
        explicit any(T t) {
            auto child_ptr = std::make_shared<Child<T>>(Child<T>(t));
            ptr = std::static_pointer_cast<Base>(child_ptr);
            // 将子类指针转为父类shared_ptr(go up class hierarchy),用static_pointer_cast.
        }

        std::string type() const { return ptr->type(); }
    };

    class any_cast_error : public std::logic_error {
    public:
        explicit any_cast_error(const std::string &error) : logic_error(error) {}
    };

    template<typename T>
    T any_cast(const any &any) {
        auto cast_ret = std::dynamic_pointer_cast<Child<T>>(any.getPtr());
        // 将父类指针安全转为子类指针(go down class hierarchy safety),用dynamic_pointer_cast
        if (cast_ret != nullptr) {
            return cast_ret->getValue();
        } else {
            throw any_cast_error("Dynamic Cast Error. The actual value type is : " + any.type());
        }
    }
}
#endif //DATASTRUCTURE_ANY_H
