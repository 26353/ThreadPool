#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue> //队列头文件
std::queue<int> q_queue;
std::condition_variable q_cv;
std::mutex mtx;
void producer()
{
    for (int i = 0; i < 10; i++)
    {
        {
            std::unique_lock<std::mutex> lock(mtx);
            q_queue.push(i);
            q_cv.notify_one();
            std::cout << "task:" << i << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
} // 生产者生成任务
void consumer()
{
    while (1)
    {
        std::unique_lock<std::mutex> lock(mtx);
        q_cv.wait(lock, []()
                  { return !q_queue.empty(); });
        int value = q_queue.front();
        q_queue.pop();

        std::cout << value << std::endl;
    }
} // 消费者处理任务
int main()
{
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
    return 0;
}