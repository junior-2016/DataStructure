//
// Created by junior on 19-4-15.
//

#ifndef DATASTRUCTURE_UTIL_H
#define DATASTRUCTURE_UTIL_H

#include <type_traits>
#include <typeinfo>

#ifndef _MSC_VER

#   include <cxxabi.h>

#endif

#include <memory>
#include <string>
#include <cstdlib>


namespace DS {
    /**
     * 根据类型打印完整的类型名,见:
     * https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/20170989
     * usage: auto type_name = typename<decltype(variable)>();
     */
    template<class T>
    std::string
    type_name() {
        typedef typename std::remove_reference<T>::type TR;
        std::unique_ptr<char, void (*)(void *)> own
                (
#ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                    nullptr, nullptr),
#else
                nullptr,
#endif
                std::free
        );
        std::string r = own != nullptr ? own.get() : typeid(TR).name();
        if (std::is_const<TR>::value)
            r += " const";
        if (std::is_volatile<TR>::value)
            r += " volatile";
        if (std::is_lvalue_reference<T>::value)
            r += "&";
        else if (std::is_rvalue_reference<T>::value)
            r += "&&";
        return r;
    }

    std::string to_string(const int &t) { return std::to_string(t); }

    std::string to_string(const unsigned &t) { return std::to_string(t); }

    std::string to_string(const long &t) { return std::to_string(t); }

    std::string to_string(const long long &t) { return std::to_string(t); }

    std::string to_string(const unsigned long &t) { return std::to_string(t); }

    std::string to_string(const unsigned long long &t) { return std::to_string(t); }

    std::string to_string(const float &t) { return std::to_string(t); }

    std::string to_string(const double &t) { return std::to_string(t); }

    std::string to_string(const long double &t) { return std::to_string(t); }

    std::string to_string(const std::string &string) { return string; }

    std::string to_string(const char *str) { return std::string(str); }

    // 使用 std::accumulate 实现容器元素拼接为字符串.默认的中间分割串为","
    template<typename Container>
    std::string to_string(const Container &container, const std::string &delimiter = ",") {
        if (container.empty()) return "[]";
        if (container.size() == 1) return "[" + DS::to_string(*container.begin()) + "]";

        return std::string("[") +
               std::accumulate(std::next(container.begin()), container.end(),
                               DS::to_string(*container.begin()),
                               [&delimiter](std::string a, typename Container::value_type b)
                                       -> std::string {
                                   return std::move(a) + delimiter + DS::to_string(b);
                               }) + std::string("]");
    }
}
#endif //DATASTRUCTURE_UTIL_H
