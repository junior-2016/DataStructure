//
// Created by junior on 19-6-17.
//

#ifndef DATASTRUCTURE_LOOPCONSTEXPR_H
#define DATASTRUCTURE_LOOPCONSTEXPR_H

#include <type_traits>
#include <functional>
#include <utility>

namespace DS {
    template<size_t N>
    struct num {
        constexpr static int value = N;
    };

    /**
     * 在可变参数模板中处理各个参数
     */
    template<typename Func, typename ... Args, size_t ... Is>
    constexpr void for_b(Func func, const std::tuple<Args...> &tuple, std::index_sequence<Is...>) {
        // 这里第二个参数必须用 std::index_sequence<Is...> 作为参数.
        // 本来是可以不用的,因为我们下面的调用,只要存在模板参数 size_t ... Is 就可以了呀,
        // 问题是调用 for_b 函数的 for_a => 它的模板参数只有一个 size_t N, 必须用 make_index_sequence 才能生成序列,
        // 所以也就不得不要求 for_b 的第二个参数必须是index_sequence了.
        (func(num<Is>{}, tuple), ...);
        // 这里用了逗号表达式(严格上说应该是折叠表达式)顺序处理完所有对应序号上的参数,每个参数被func执行一次
    }

    template<size_t N, typename Func, typename ... Args>
    constexpr void for_a(Func func, const std::tuple<Args...> &tuple) {
        for_b(func, tuple, std::make_index_sequence<N>());
    }

    // 用 func 依次处理每一个参数,func只接收一个参数,因此可以在func里对特别的参数做特别处理,处理过程可以是不一样的.
    template<typename Func, typename ... Args>
    constexpr void process_arguments(Func func, const std::tuple<Args...> &tuple) {
        // sizeof(args) ... 得到包含各个参数size的列表;
        // sizeof ... (Args) 得到 Args 参数列表的长度　
        for_a<sizeof ... (Args)>(func, tuple);
    }

    // 先将参数变成tuple,再用 std::apply 一次性处理掉所有的参数,
    // 这个跟上面不同在于,这里func的参数必须是 auto&& ... args,
    // 即一次性吃下所有的参数然后处理掉,并且每一个参数的执行过程都是一样的,相对简单但是不够灵活.
    template<typename Func, typename ... Args>
    void process_arguments_all(Func func, const std::tuple<Args...> &tuple) {
        std::apply(func, tuple);
    }

    void test_A() {
        std::cout << "sum of even order numbers : ";
        int sum = 0;
        auto func = [&](auto num_object, const auto &tuple_object) {
            if (num_object.value % 2 == 0) { // 只有序号为偶数的参数才进行求和
                decltype(auto) v = std::get<num_object.value>(tuple_object);
                std::cout << v << " ";
                sum += v;
            }
        };
        process_arguments(func, std::forward_as_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
        std::cout << "\nThe result is " << sum << std::endl;
    }

    void test_B() {
        std::cout << "sum of all numbers : ";
        int sum = 0;
        auto func = [&](auto const &... args) {
            ((std::cout << args << " "), ...);
            sum = (0 + ... + args); // 用折叠表达式计算
        };
        process_arguments_all(func, std::forward_as_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
        std::cout << "\nThe result is " << sum << std::endl;
    }
}
#endif //DATASTRUCTURE_LOOPCONSTEXPR_H
