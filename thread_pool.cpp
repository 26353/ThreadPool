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
    // 构造函数:创建指定数量的工作线程
    // 参数numTreads:线程池中工作线程的数量
    ThreadPool(int numThreads) : stop(false)
    {
        for (int i = 0; i < numThreads; i++)
        {
            // 使用emplace_back直接构造线程对象（避免拷贝）
            threads.emplace_back(
                // 每个线程执行一个无线循环的任务处理逻辑
                [this]
                {
                                     while (1)
                                     {
                                         std::unique_lock<std::mutex> lock(mtx);//访问共享资源前加锁
                                         //等待条件变量唤醒，唤醒条件：任务队列非空或线程池停止标志为真
                                         condition.wait(lock, [this]()
                                                        { return !tasks.empty() || stop; });
                                        //如果线程池以停止工作，结束线程
                                         if (stop&&tasks.empty())
                                         {
                                             return;
                                         }
                                         std::function<void()> task(std::move(tasks.front()));
                                         tasks.pop();
                                         lock.unlock();//提前释放锁，允许其他线程继续操作队列
                                         task();//执行任务（在无锁状态下执行，减少锁竞争）
                                     } }

            );
        } // 比push_back节省资源，emplace_back进行有参构造，push_back是拷贝构造，同时线程不支持拷贝构造
    }
    // 析构函数：安全停止所有线程并回收资源
    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(mtx);
            stop = true; // 设置停止标志
        }
        condition.notify_all(); // 唤醒所有等待线程
        // 等待所有工作线程结束
        for (auto &t : threads)
        {
            t.join();
        }
    }
    template <class F, class... Args> // 可变参数列表
    void enqueue(F &&f, Args &&...args)
    {
        std::function<void()> task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        // 函数参数和函数绑定在一起后，不论函数有多少个参数都不用在传参

        {
            std::unique_lock<std::mutex> lock(mtx);
            tasks.emplace(std::move(task)); // 将任务加入队列（移动语义提升性能）
        }
        condition.notify_one(); // 通知一个等待中的线程

    } // 两个取值在函数里是右值引用，一个是左值引用，模板中两个&是万能引用
private:
    std::vector<std::thread> threads;         // 工作线程集合
    std ::queue<std::function<void()>> tasks; // 任务队列（存储无参void函数）

    std::mutex mtx;                    // 保护任务队列和停止标志的互斥锁
    std::condition_variable condition; // 任务通知条件变量

    bool stop; // 线程池停止标志（true时终止所有线程）
};
// #define TEST
#ifdef TEST
int main()
{
    ThreadPool pool(4);

    for (int i = 0; i < 100; i++)
    {
        pool.enqueue([i]()
                     { std::cout << "task:" << i+1 << "is running" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));//暂停下，否则线程太快不好观察
            std::cout << "task:" << i+1 << "is done" << std::endl; });
    }
    return 0;
}
#else
std::mutex Mtx;
void _sort(std::vector<int> &al, int left, int right, ThreadPool &pool)
{
    if (left >= right)
        return;
    pool.enqueue([&]
                 {
                    int i = left;
                    int j = right;
                    int _base = al[left];
        while (i < j)
        {

            while (i < j && _base <= al[j])
            {
                j--;
            }
            while (i < j && al[i] <= _base)
            {
                i++;
            }
            if (i != j)
            {
                int temp = al[i];
                al[i] = al[j];
                al[j] = temp;
            }
            al[left] = al[i];
            al[i] = _base;
            _sort(al, left, i - 1, pool);
            _sort(al, i + 1, right, pool);
        } });
} // 快速排序
int main()
{
    ThreadPool pool(5);
    std::vector<int> al{3, 6, 3, 7, 5, 4, 2, 6, 8, 9};
    _sort(al, 0, al.size() - 1, pool);
    for (int i = 0; i < al.size(); i++)
    {
        std::cout << al[i] << " ";
    }
}
#endif
