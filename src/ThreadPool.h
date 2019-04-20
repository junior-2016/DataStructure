//
// Created by junior on 19-4-19.
//

#ifndef DATASTRUCTURE_THREADPOOL_H
#define DATASTRUCTURE_THREADPOOL_H
/**
 * C++11 线程池实现.
 * reference: https://github.com/progschj/ThreadPool
 */
#include <thread>
#include <future>
#include <atomic>
#include <functional>
#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

// 测试C++多线程
/*
class IntMutex : public std::mutex { // 继承std::mutex(Basic Mutex)
private:
    int val;
public:
    IntMutex() : val(0) {}

    explicit IntMutex(int val) : val(val) {}

    int &getVal() { return val; }
};

class A {
    IntMutex mutex;
public:
    A() = default;

    // 读写冲突加锁..
    void read() {
        std::lock_guard<std::mutex> mu(mutex);
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::cout << mutex.getVal() << "\n";
    }

    void write() {
        std::lock_guard<std::mutex> mu(mutex);
        mutex.getVal()++;
    }
};

void test_race_condition() {
    A a{};
    for (int i = 0; i < 300; i++) {
        if (i % 2 == 0) {
            std::thread thread(&A::read, &a);
            thread.join();
        } else {
            std::thread thread(&A::write, &a);
            thread.join();
        }
    }
}
*/

namespace DS {
    class ThreadPool {
    public:
        explicit ThreadPool(size_t);

        ~ThreadPool();

    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()> > tasks;
        std::mutex queue_mutex;
        std::condition_variable conditionVariable;
        bool stop;
    };

    ThreadPool::ThreadPool(size_t threads) : stop(false) {

    }

    inline ThreadPool::~ThreadPool() {

    }
}
#endif //DATASTRUCTURE_THREADPOOL_H
