#include "src/DataStructure.h"

template<typename T>
class A {
public:
    A(T &t) {}

    template<typename T2>
    explicit A(const T2 &a) {}
};

int main() {
    std::string s;
    A<int> a(std::move(s));
    DS::test();
    return 0;
}