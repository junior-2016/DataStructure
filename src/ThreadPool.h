//
// Created by junior on 19-4-19.
//

#ifndef DATASTRUCTURE_THREADPOOL_H
#define DATASTRUCTURE_THREADPOOL_H
/**
 * C++11 线程池实现.
 * reference: https://github.com/progschj/ThreadPool
 * reference: https://github.com/jhasse/ThreadPool(C++17版本)
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
        using namespace std::chrono_literals;
        std::lock_guard<std::mutex> mu(mutex);
        //std::this_thread::sleep_for(1ms);
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
        explicit ThreadPool(size_t= 16); // 默认线程池大小16

        template<typename F, typename ... Args>
        // auto enqueue(F&&f,Args&&... args);
        decltype(auto) enqueue(F &&f, Args &&... args);

        ~ThreadPool();

    private:
        std::vector<std::thread> thread_pool;
        std::queue<std::function<void()> > tasks_queue;
        std::mutex queue_mutex;
        std::condition_variable exec_task_cv;
        bool stop;
    };

    inline ThreadPool::ThreadPool(size_t threads) : stop(false) {
        using namespace std::chrono_literals;
        for (size_t i = 0; i < threads; i++) {
            // 注意 std::thread 的拷贝构造函数 thread(const thread&)是被删除的,只有移动构造函数,
            // 所以这里最好用 std::vector::emplace_back(Function)往vector 添加线程.
            // 另外需要注意的是,当往thread_pool里 emplace_back 一个线程的时候,线程就已经启动了.
            thread_pool.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        // 这里和我参考的代码不同在于,我将wait改成wait_for,每隔1s后如果没有其他线程notify(
                        // 其实只有主线程的enqueue()函数或者线程池的析构函数会执行notify_*),那么线程池线程会考虑
                        // 自动唤醒,尝试获取锁,如果获取成功返回Pred函数[]{ return !tasks_queue.empty() || stop; }的结果,
                        // 然后如果这个结果是false,我就通过while循环,重新调用wait_for让线程重新进入等待状态并释放获得的锁,
                        // 如果结果是true,那就可以执行相应的task了.
                        // 如果只使用 wait,那线程唤醒的唯一途径只能依靠其他线程的notify,假设线程池只有四线程可以工作,
                        // 然后之前分配的task使得这四个线程满负载工作,此时又来了七八个新task,而在enqueue()里执行
                        // notify_one()是没用的,因为所有的线程都在工作,并不是等待状态,所以这七八个task只能存在任务队列里.
                        // 如果此时线程池又恰好析构了,此时析构函数调用的notify_all()也不起任何作用(假设当前线程池中所有线程
                        // 还在工作,没有一个等待的),接下来的情况是:
                        // 四个线程中有一个工作结束了,按照外层死循环它会重新获取互斥量的锁,然后调用wait(lock,pred()),
                        // 我一开始想错了,以为调用了wait就会立刻进入等待状态,然后需要别的线程notify唤醒它并且pred()判断为true才能执行task,
                        // 那这样岂不是所有的线程都只能wait而无法执行task了(注意此时已经不存在其他线程可以执行notify操作),
                        // 但实际不是这样,wait(lock,pred)的具体实现是:
                        // while(!pred){wait(lock);}
                        // 也就是它会先判断pred,只有pred判断false才会等待,如果一开始pred判断就是true,它是不会等待的,
                        // 因此并不存在最后任务队列里一大堆任务,却没有一个线程执行的情况.
                        // 因此,不管是wait(lock,pred)还是wait_for(lock,timeout,pred),其实都是可以的.
                        while (!exec_task_cv.wait_for(lock, 1s,
                                                      [this] { return !tasks_queue.empty() || stop; })) {}
                        //exec_task_cv.wait(lock, [this] { return !tasks_queue.empty() || stop; });

                        if (stop && tasks_queue.empty()) {
                            return;// 如果已经下达stop命令,并且任务队列也已经变空,就可以终结当前线程了.
                        }
                        //if (!tasks_queue.empty()) {
                        // 这里不需要判断 tasks_queue是否非空,因为按照condition_variable的条件,被唤醒并且满足 !queue.empty() || stop
                        // 才能脱离wait状态. 对于 stop && queue.empty()的情况,前面已经讨论过,剩下的只能是!queue.empty()的情况.
                        task = std::move(tasks_queue.front());
                        tasks_queue.pop();
                        //}
                    } // 离开{..}作用域,unique_lock自动解锁,释放mutex.
                    // 注意这里获取task后就可以解锁,没必要调用task()执行完再解锁,一个原因是我们线程池获取的核心是task对象本身,
                    // 另外一个原因是,等执行完再释放锁会导致性能严重下降,因为task的执行可能非常耗时(甚至死循环执行..),
                    // 严重影响到其他需要获取mutex的任务线程和enqueue()入队函数.
                    task();
                }
            });
        }
    }

    template<typename F, typename... Args>
    // auto ThreadPool::enqueue(F &&f, Args &&... args)
    decltype(auto) ThreadPool::enqueue(F &&f, Args &&... args) {
        using return_type = decltype(f(args...));
        // 这里也可以std::result_of<F(Args...)>::type 或者 std::invoke_result_t<F,Args...>;

        std::function<return_type()> task_func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(task_func);
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop) { // 不允许在已经stop的pool上添加task
                throw std::runtime_error("Enqueue task on stop thread pool.\n");
            }
            std::function<void()> wrap_task = [task_ptr]() { (*task_ptr)(); };
            tasks_queue.push(wrap_task);
        } // 离开{...}作用域, unique_lock自动解锁
        exec_task_cv.notify_one(); // 注意先解锁,再调用 notify_*
        return task_ptr->get_future();
    }

    inline ThreadPool::~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        } // 加锁然后设置stop为true.
        exec_task_cv.notify_all();//尝试唤醒所有线程处理剩下的task.
        // 需要注意的是,线程池对象 thread_pool 在主线程. 当主线程代码即将结束时,线程池对象会被析构,
        // 但此时可能还有很多线程还处于等待接收任务的状态,所以这里第一步是唤醒所有线程,一个接一个把任务完成;
        // 另外一步是将所有线程池的线程join到主线程,这样主线程会一直堵塞,直到线程池中所有线程都结束了主线程才会结束.
        for (auto &th:thread_pool) {
            if (th.joinable()) {
                th.join();
            }
        }
    }
}
#endif //DATASTRUCTURE_THREADPOOL_H
