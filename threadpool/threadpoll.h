#ifndef _THREADPOLL_H
#define _THREADPOLL_H

#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>
#include <future>

// 线程池
class Threadpoll
{
private:
    bool isClosed;
    std::vector<std::thread> thread_arr;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cond;
public:
    explicit Threadpoll(size_t threadCount = 8)
        : isClosed(false)
    {
        for(size_t i = 0; i < threadCount; ++i)
        {
            // lamda表达式传入的是创建的新线程要执行的函数
            thread_arr.emplace_back(
                [&]()
                {
                    std::unique_lock<std::mutex> locker(mtx);
                    while(true)
                    {
                        // 执行队列中的任务
                        if(!tasks.empty())
                        {
                            auto task = std::move(tasks.front());       // 取任务加锁
                            tasks.pop();
                            locker.unlock();
                            task();         // 执行任务解锁，多线程并发执行
                            locker.lock();
                        }
                        else if(isClosed) break;
                        else cond.wait(locker);
                    }
                }
            );
            // thread_arr.emplace_back(
            //     [&]()
            //     {
            //         while(true)
            //         {
            //             // 执行队列中的任务
            //             std::function<void()> task;
            //             {
            //                 std::unique_lock<std::mutex> locker(mtx);
            //                 cond.wait(locker, [&]() {return isClosed || !tasks.empty();});
            //                 if(isClosed && tasks.empty()) break;
            //                 task = std::move(tasks.front());
            //                 tasks.pop();
            //             }
            //             task();
            //         }
            //     }
            // );
        }
    }
    Threadpoll() = default;

    Threadpoll(const Threadpoll&) = delete;
    Threadpoll(Threadpoll&&) = delete;

    Threadpoll& operator=(const Threadpoll&) = delete;
    Threadpoll& operator=(Threadpoll&&) = delete;



    ~Threadpoll()
    {
        {
            std::unique_lock<std::mutex> locker(mtx);
            isClosed = true;
        }
        cond.notify_all();
        for(auto& threads : thread_arr)
        {
            threads.join();         // 阻塞线程，确保调用线程对象完成执行
        }
    }

    template<typename F, typename... Args>
    auto submit(F&& f, Args&& ... args)->std::future<decltype(f(args...))>
    {
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        {
            std::unique_lock<std::mutex> locker(mtx);
            if(isClosed)
            {
                throw std::runtime_error("submit on closed Threadpool");
            }
            tasks.emplace([&]() { (*task_ptr)(); });
        }
        cond.notify_one();
        return task_ptr->get_future();
    }

    template<typename F>
    void addTask(F&& task)
    {
        {
            std::unique_lock<std::mutex> locker(mtx);
            tasks.emplace(std::forward<F>(task));       // std::forward是将右值引用以原始类型传递，完美转发，确保参数值类别保留，避免发生不必要的临时对象创建和拷贝
        }
        cond.notify_one();
    }
};

#endif