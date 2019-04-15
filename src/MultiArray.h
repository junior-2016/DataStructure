//
// Created by junior on 19-4-13.
//

#ifndef DATASTRUCTURE_MULTIARRAY_H
#define DATASTRUCTURE_MULTIARRAY_H
/**
 * C++17 模板类实现任意维度数组..
 * 内部数据指针只用一维,访问通过计算偏移实现.
 * 多维数组的每一层偏移通过模板逐层推导实现(需要变长模板)
 * 参考:
 * [1] https://www.zhihu.com/question/289187231
 * [2] fold expression https://zh.cppreference.com/w/cpp/language/fold
 *
 */
#include <cctype>
#include <type_traits>
#include <vector>

template<typename T, size_t ... Ns>
class MultiArrayPointer;

template<typename T, size_t N>
class MultiArrayPointer<T, N> {
private:
    T *data;
public:
    explicit MultiArrayPointer(T *data) : data(data) {}

    constexpr T &operator[](const size_t &idx) const {
        // assert idx < N
        return data[idx];
    }
};

template<typename T, size_t N, size_t ...Ns>
class MultiArrayPointer<T, N, Ns...> {
private:
    T *data;
    constexpr static size_t stride = (1* ... *Ns);
public:
    explicit MultiArrayPointer(T *data) : data(data) {}

    constexpr auto operator[](const size_t &idx) const {
        // assert idx < N
        return MultiArrayPointer<T, Ns...>(data + idx * stride);
    }
};

template<typename T, size_t N, size_t ...Ns>
class MultiArray {
private:
    static constexpr size_t stride = (1 * ... * Ns);
    T *data = new T[(N * ... * Ns)];
public:

    MultiArray() {
        for (size_t i = 0; i < (N * ... * Ns); i++) {
            data[i] = (T) i;
        }
    }

    constexpr auto operator[](const size_t &idx) const {
        // assert idx < N
        return MultiArrayPointer<T, Ns...>(data + idx * stride);
    }
};

#endif //DATASTRUCTURE_MULTIARRAY_H
