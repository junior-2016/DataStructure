//
// Created by junior on 19-4-17.
//
#ifndef DATASTRUCTURE_TESTIMPL_H
#define DATASTRUCTURE_TESTIMPL_H

#include "Test.h"

namespace DS {
    /**
     * 测试要点:
     * 1. 测试时为了防止名称空间发生污染(比如std::to_string和DS::to_string),不使用using指令;
     */

    void TEST_MULTI_ARRAY() {
        std::cout << "--------------TEST MULTI ARRAY----------------\n";
        DS::MultiArray<int, 4, 5, 6, 7, 8, 10, 11> array1;
        std::cout << array1[2][3][4][5][6][4][8] << std::endl;
        std::cout << "----------------------------------------------\n\n";
    }

    void TEST_LIST() {
        std::cout << "-------------TEST LIST--------------------\n";

        DS::List list({{{1, 2, 3}, {1, 2, 3.1415926535}}, 5, {6, 7, {1, 4, 5}, {33, {24, 5}}},
                       {{4}, {1, 2}}, 88, {1, 5}, {1, 3, std::string("Hello World"), "My favorite"}, 7});
        DS::List list1({1, {"25", 1.23}, std::vector<int>({1, 2})});
        std::cout << list1 << list1.size() << "\n";

        std::cout << DS::List::flat(list1);

        std::cout << list << list.size() << "\n";

        DS::List flat_list = DS::List::flat(list);
        std::cout << flat_list << flat_list.size() << "\n";

        for (const auto &i : list) { // for-ranged loop
            std::cout << i;
        }

        list.append(1);
        std::cout << list << list.size() << "\n";

        list.append(List(6));
        std::cout << list << list.size() << "\n";

        list.append({5, 5, {7, 8}});
        std::cout << list << list.size() << "\n";

        list.append(flat_list);
        std::cout << list << list.size() << "\n";

        list.insert(list.cbegin() + 2, List({"insert list in here"}));
        std::cout << list << list.size() << "\n";

        list.insert(list.cbegin(), List(std::string("insert string in here")));
        std::cout << list << list.size() << "\n";

        std::cout << DS::List::flat(list).size() << "\n";

        std::cout << "---------------------------------------------\n\n";
    }

    void TEST_ANY() {
        std::cout << "---------------TEST ANY -----------------------\n";
        DS::any any(std::vector<int>{1, 2, 3});
        DS::any any1(2.36f);
        const double &d = 6.66;
        DS::any any2(d);
        std::cout << any.type() << "\n";
        std::cout << any1.type() << "\n";
        std::cout << any2.type() << "\n";
        try {
            std::cout << DS::any_cast<std::vector<int, std::allocator<int>>>(any).size() << "\n";
            std::cout << DS::any_cast<float>(any1) << "\n";
            std::cout << DS::any_cast<double>(any2) << "\n";
        } catch (const DS::any_cast_error &e) {
            std::cerr << e.what() << "\n";
        }
        std::cout << "-------------------------------------------------\n\n";
    }

    void TEST_VARIANT() {
        std::cout << "--------------TEST VARIANT------------------------\n";
        DS::variant<int, double, std::string, std::vector<int>, std::vector<std::string>> variant;
        try {
            variant.set<int>(5);
            std::cout << DS::get<int>(variant) << "\n";

            variant.set<double>(5.23);
            std::cout << DS::get<double>(variant) << "\n";

            variant.set<std::string>("Hello world");
            std::cout << DS::get<std::string>(variant) << "\n";

            variant.set<std::vector<int>>(std::vector<int>{5, 4, 3, 7, 10});
            std::cout << DS::to_string(DS::get<std::vector<int>>(variant)) << "\n";

            variant.set<std::vector<std::string>>(std::vector<std::string>{"Hello world", "Wo hhh"});
            std::cout << DS::to_string(DS::get<std::vector<std::string>>(variant)) << "\n";

            variant.set<double>(3.14);
            std::cout << DS::to_string(DS::get<std::vector<int>>(variant)) << "\n"; // 这里会抛出异常....
        } catch (DS::variant_access_error &e) {
            std::cout << e.what() << "\n";
        }
        std::cout << "---------------------------------------------------\n";
    }

    void TEST_CONTAINER_TO_STRING() {
        std::cout << "--------------TEST CONTAINER TO STRING--------------\n";
        std::vector<int> vector = {1, 2};
        std::vector<int> empty_vector;
        std::vector<int> single_vector = {1};
        std::cout << DS::to_string(vector) << "\n";
        std::cout << DS::to_string(vector, ";") << "\n";
        std::cout << DS::to_string(empty_vector) << "\n";
        std::cout << DS::to_string(single_vector) << "\n";
        std::cout << "---------------------------------------------------\n\n";
    }

    // C++ thread detach 分离线程到后台例子:
    struct background_task { // 后台任务
    private:
        int value;
    public:
        void add_one() {
            using namespace std::chrono_literals;
            // 每隔1s计算加1.后台计算
            while (true) {
                value++;
                std::this_thread::sleep_for(1s);
            }
        }

        const int getValue() const { return value; }

        background_task() { value = 0; }
    };

    void MainThread() {
        using namespace std::chrono_literals;
        background_task task;
        std::thread background_thread(&background_task::add_one, &task);
        background_thread.detach(); // 后台线程脱离当前 MainThread.
        while (true) {
            // 当前 MainThread先自己做一些操作(这里用sleep代替),做完后拿取后台计算的数据并输出.
            std::this_thread::sleep_for(5s);
            std::cout << "Now background value is:" << task.getValue() << "\n";
        }
    }

    void TEST_DETACH_THREAD() {
        std::cout << "------------TEST DETACH THREAD--------------\n";
        std::thread mainThread(MainThread);
        mainThread.join();
    }

    std::condition_variable cv;
    std::mutex mu;

    void wait_until_thread() {
        using namespace std::chrono_literals;
        auto now = std::chrono::system_clock::now();
        std::unique_lock<std::mutex> lock(mu);
        if (cv.wait_until(lock, now + 10s) == std::cv_status::no_timeout) {
            std::cout << "中途唤醒\n";
        } else {
            std::cout << "时间点到,结束\n";
        }
    }

    void main_thread() {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        // 第一步休眠很重要,因为我们要模拟的是wait_until_thread先取得锁,然后因为wait_until进入阻塞状态,同时把锁释放,
        // 接着才是我们的main_thread获得锁,休眠2s后释放锁并且立刻唤醒wait线程,中途打断wait线程,那么它返回的状态自然是
        // cv_status::no_timeout. 所以这里main_thread先休眠1s就是为了保证wait线程先执行,如果main_thread先执行的话,
        // 执行结束后cv.notify_one()也没用,因为wait还没有进入阻塞状态呢.
        {
            std::lock_guard<std::mutex> lock(mu); // lock_guard 离开{..}scope后自动解锁
            //std::this_thread::sleep_for(2s); // 睡眠2s后解锁并中途唤醒线程
            std::this_thread::sleep_for(15s);
            // 如果我们睡眠的时间超过wait_until等待的时间范围,实际上wait_until已经自动唤醒了(不需要notify_*)
            // 但是wait_until线程唤醒后,会调用它所拥有的lock对象的lock()函数,尝试锁住互斥量,这是不成功的,
            // 因为互斥量还被main_thread()锁着呢,这就导致wait_until线程因为lock()函数得不得互斥量而被堵塞.
            // 当main_thread()线程睡眠结束并释放锁后,wait_until随即锁住互斥量并结束线程,最后main_thread的
            // notify_*没有什么用,因为wait_until线程已经结束了.
        }
        cv.notify_one();
    }

    void TEST_WAIT_UNTIL() {
        std::thread thread1(wait_until_thread);
        std::thread thread2(main_thread);
        thread1.join();
        thread2.join();
    }

    void TEST_THREAD_POOL() {
        std::cout << "--------------TEST THREAD POOL--------------\n";
        DS::ThreadPool pool(4);
        std::vector<std::future<int>> result_collect;
        for (auto i = 0; i < 8; i++) {
            result_collect.emplace_back(
                    pool.enqueue(
                            [=](int times) { return times * (i + 1); }, // F
                            4 // args...
                    ));
            // 注意这里的顺序是: 先依次调用enqueue启动线程执行任务,同时把future保存起来,等全部任务enqueue后,再同时执行future->get().
            // 如果每enqueue一个task就执行一次future->get(),会堵塞for循环,直到任务结束获得值才能启动下一个任务,那就跟串行一样了,没有意义.
        }
        for (auto &result:result_collect) {
            std::cout << result.get() << " ";
            // 这里每执行一次future.get(),如果没有得到值会堵塞住,直到获得值才会执行下一次result.get(),
            // 所以得到的输出会是顺序的 4 8 12 16 20 24 28 32 (但执行计算任务的线程是并发同时进行的)
        }
        std::cout << "\n";
        std::cout << "--------------------------------------------\n";
    }

    void test() {
        TEST_CONTAINER_TO_STRING();
        TEST_MULTI_ARRAY();
        TEST_LIST();
        TEST_ANY();
        TEST_VARIANT();
        TEST_THREAD_POOL();
    }
}
#endif //DATASTRUCTURE_TESTIMPL_H
