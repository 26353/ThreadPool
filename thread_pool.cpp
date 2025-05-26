#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>

class ThreadPool
{
public:
    ThreadPool(int numThreads) : stop(false)
    {
        for (int i = 0; i < numThreads; i++)
        {
            threads.emplace_back([this]
                                 {
                                     while (1)
                                     {
                                         std::unique_lock<std::mutex> lock(mtx);
                                         condition.wait(lock, [this]()
                                                        { return !tasks.empty() || stop; });
                                         if (stop)
                                         {
                                             return;
                                         }
                                         std::function<void()> task(std::move(tasks.front()));
                                         tasks.pop();
                                         lock.unlock();
                                         task();
                                     } }

            );
        } // 比push_back节省资源，emplace_back进行有参构造，push_back是拷贝构造，同时线程不支持拷贝构造
    }
    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(mtx);
            stop = true;
        }
        condition.notify_all();
        for (auto &t : threads)
        {
            t.join();
        }
    }
    template <class F, class... Args> // 可变列表
    void enqueue(F &&f, Args &&...args)
    {
        std::function<void()> task = std::bind(std::forward<F>(f), std::forward<F>((args)...));
        // 函数参数和函数绑定在一起后，不论函数有多少个参数都不用在传参

        {
            std::unique_lock<std::mutex> lock(mtx);
            tasks.emplace(std::move(task));
        }
        condition.notify_one();

    } // 两个取值在函数里是右值引用，一个是左值引用，模板中两个&是万能引用
private:
    std::vector<std::thread> threads;
    std ::queue<std::function<void()>> tasks;

    std::mutex mtx;
    std::condition_variable condition;

    bool stop;
};
int main()
{
    ThreadPool pool(4);

    for (int i = 0; i < 10; i++)
    {
    }
}