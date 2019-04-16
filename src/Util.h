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

}
#endif //DATASTRUCTURE_UTIL_H
