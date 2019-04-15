//
// Created by junior on 19-4-15.
//

#ifndef DATASTRUCTURE_UTIL_H
#define DATASTRUCTURE_UTIL_H

#include <string>
#include <sstream>

namespace DS {
    /*
     * 注意下面的 to_string()在调用时,不需要 to_string<Type>(variable),
     * 直接to_string(variable)就行,因为通过参数就可以确定T的类型;
     * 但如果是 template<typename T> void f(){} 这样的函数,调用时就必须明确是: f<Type>();
     */

    // 对象类型,输出对象字节字符串,现在有点问题(const uint8_t*pointer的转化那里),暂时改成输出对象地址
    template<class T>
    std::string to_string(const T &t) {
        std::ostringstream stream;
        /*
        const uint8_t *pointer = &t;
        for (size_t i = 0; i < sizeof(T); ++i) {
            stream << "0x" << std::hex << pointer[i];
        }*/
        stream << &t;
        return stream.str();
    }

    // 模板特例化转化为std::string
    template<>
    std::string to_string(const int &t) { return std::to_string(t); }

    template<>
    std::string to_string(const long &t) { return std::to_string(t); }

    template<>
    std::string to_string(const long long &t) { return std::to_string(t); }

    template<>
    std::string to_string(const unsigned &t) { return std::to_string(t); }

    template<>
    std::string to_string(const unsigned long &t) { return std::to_string(t); }

    template<>
    std::string to_string(const unsigned long long &t) { return std::to_string(t); }

    template<>
    std::string to_string(const float &t) { return std::to_string(t); }

    template<>
    std::string to_string(const double &t) { return std::to_string(t); }
}
#endif //DATASTRUCTURE_UTIL_H
