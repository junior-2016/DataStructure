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
/* 测试C++多线程
class A {
    int resource = 0; // resource 必须声明在private段
    std::mutex mutex; // 信号量
public:
    A() = default;

    void read() {
        std::lock_guard<std::mutex> mu(mutex);
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::cout << resource << "\n";
    }

    void write() {
        std::lock_guard<std::mutex> mu(mutex);
        resource++;
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

}
#endif //DATASTRUCTURE_THREADPOOL_H
