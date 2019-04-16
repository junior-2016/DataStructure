//
// Created by junior on 19-4-16.
//

#ifndef DATASTRUCTURE_ANY_VARIANT_H
#define DATASTRUCTURE_ANY_VARIANT_H

#include <memory>
/**
 std::any 实现方法:
 class Base{
     public:
         virtual std::string name() const;
 }
 template<typename T>
 class Child:Base{
     private:
         T value;
         std::string type_info;
     public:
         std::string name () const override {return type_info;}
         Child(const T&t){value = t; getTypeInfo(type_info); }
         T getValue() const {return value;}
 }
 class any{
     private:
        std::shared_ptr<Base> ptr;
     public:
        template<typename T> any(const T& t){
            ptr = std::static_pointer_cast<Child<T>(t)>;
            // 将子类指针转为父类shared_ptr(即:go up class hierarchy),用static_pointer_cast.
        }
        std::string name() const { return ptr->name(); }
        auto getPtr() const { return ptr;}
 }
 template<typename T>
 T any_cast(const any& a){
     return std::dynamic_pointer_cast<Child<T>>(a.getPtr())->getValue();
     // 将父类指针安全转为子类指针,用dynamic_pointer_cast
 }
 */

namespace DS {
    class Base {
    public:
        virtual std::string name() { return "Base"; }
    };

    template<typename T>
    class Child : public Base {
        std::string type_info;
        T value;
    public:
        std::string name() override { return type_info; }

        Child(const T &t) : value(t) { /*getTypeInfo(type_info);*/ }

        T getValue() const { return value; }
    };

    class any {
    private:
        std::shared_ptr<Base> ptr;
    public:
        auto getPtr() const { return ptr; }

        template<typename T>
        any(const T &t) {
            auto child_ptr = std::make_shared<Child<T>>(Child<T>(t));
            ptr = std::static_pointer_cast<Base>(child_ptr);
        }

        std::string name() const { return ptr->name(); }
    };

    template<typename T>
    T any_cast(const any &any) {
        return std::dynamic_pointer_cast<Child<T>>(any.getPtr())->getValue();
    }
}
#endif //DATASTRUCTURE_ANY_VARIANT_H
