#include "src/Test.h"

struct S {
    bool operator()(int a, char c) { return false; }
};

void example1() {
    decltype(S()(1, 'c')) s1 = true;
    std::result_of<S(int, char)>::type s2 = true;
    std::cout << std::boolalpha << s1 << " " << s2 << "\n";
}

void example2() {
    auto f = [](int a) { return a; };
    decltype(f(1)) a1 = 1;
    std::result_of<decltype(f)(int)>::type a2 = 1;
    std::cout << a1 << " " << a2 << "\n";
}

template<typename F, typename ... Args>
void example(F &&f, Args &&... args) {
    using return_type_1 = typename std::result_of<F(Args...)>::type;
    using return_type_2 = decltype(f(args...));
    std::cout << std::boolalpha << std::is_same<return_type_1, return_type_2>::value << "\n";
}

auto main() -> int {
    example1();
    example2();
    example(S(), 1, 'c');
    example([](int, char) { return std::string("str"); }, 1, 'c');
    // DS::test();
    return 0;
}