//
// Created by junior on 19-4-16.
//

#ifndef DATASTRUCTURE_VARIANT_H
#define DATASTRUCTURE_VARIANT_H

#include <type_traits>
#include <typeinfo>
#include <utility>
#include <string>

/**
 * 仿照 std::variant 实现 DS::variant (variant是C++上union的替代品)
 * std::variant 实现代码: https://github.com/mpark/variant
 * 一个简易版本的实现: https://gist.github.com/tibordp/6909880
 */
namespace DS {
    // 模板递归求变长模板参数值(size_t类型)的最大值
    template<size_t ... Ss>
    struct max;
    template<size_t S>
    struct max<S> {
        static const size_t size_ = S;
    };

    template<typename ...Ts>
    class variant {
    private:

    };
}
#endif //DATASTRUCTURE_VARIANT_H
