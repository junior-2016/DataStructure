//
// Created by junior on 19-6-16.
//

#ifndef DATASTRUCTURE_CLOSURE_H
#define DATASTRUCTURE_CLOSURE_H

#include "functional"

namespace DS {
/**
 * 用C++ lambda表达式模拟 js 的闭包
 * 1. 闭包: 在函数A里返回一个新定义的函数R,并且这个返回函数R利用了定义它的函数A内的某些变量或者对象,
 * 2. 使用lambda捕获的作用域问题,有两种捕获方式将变量带出:
 * 一种是复制变量的值后将值带出(用[=]), 一种是捕获变量的引用后将引用带出(相当于带出变量的常量指针,用[&]).
 * 这里只能采用复制值的方式,而不能采用引用方法,因为当函数A调用结束后,函数A内的变量也会释放掉内存,
 * 接着你调用获得的返回函数R,去访问变量的引用,相对于访问一个野指针,得到的结果是未定义的.
 * 参考知乎讨论: https://www.zhihu.com/question/43643243
 */
    template<typename T>
    std::function<T()> closure_value() {
        int a = 1;
        return [=]() {
            return a;
        };
    }

    template<typename T, size_t N>
    std::function<T *()> closure_pointer() {
        T *array = new T[N];
        for (size_t j = 0; j < N; j++) {
            array[j] = j;
        }
        auto f = [=]() { return array; };
        array[0] = (T) N;
        return f;
    }

    template<typename T>
    std::function<std::vector<T>()> closure_vector() {
        std::vector<T> vector = {(T) 1, (T) 2, (T) 3};
        auto func = [=]() {
            return vector;
        };
        vector.push_back(4);
        return func;
    }
}
#endif //DATASTRUCTURE_CLOSURE_H
