/**
 * @file threadpool.cpp
 * @author your name (you@domain.com)
 * @brief my test of study thread pool
 * @version 0.1
 * @date 2025-05-26
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <iostream>
#include <thread> //线程头文件
#include <memory> //智能指针头文件
#include <mutex>  //互斥锁头文件
#include <string>

int a = 0;
std::mutex mux;
std::once_flag once;
class log
{
public:
    log(const log &log) = delete;
    log &operator=(const log &log) = delete;
    static log &getlog()
    {
        std::call_once(once, init);
        return *_log;
    }
    void Error(std::string msg)
    {
        std::cout << msg;
    }

private:
    static log *_log;
    static void init()
    {
        if (_log == nullptr)
        {
            _log = new log;
        }
    }
    log() {};
};
log *log::_log = nullptr;
void fun()
{
    log::getlog().Error("hello world");
}
int main()
{
    std::thread t1(fun);
    std::thread t2(fun);
    t1.join();
    t2.join();
    return 0;
}

/**
 * @brief
 * join 等待线程结束
 * detach 分离线程
 * joinable 判断线程是否可以调用join和detach
 * void(A::*ptr) ()= &A::print;将A类的print函数地址存放在指针ptr中
 * std::shared_ptr<> =std::make_shared<>();创建智能指针指向对象
 * std::lock_guard是C++标准库中的一种互斥量封装类，用于保护共享数据，防止多个线程同时访问同一资源二导致的数据竞争问题
 * 当构造函数被调用时，该互斥量会被自动锁定。
 * 当析构函数被调用时，该互斥量会被自动解锁。
 * std::lock_guard对象不能复制或移动，因此它只能在局部作用域中使用。
 * std::unique_lock是c++标准库中提供的一个互斥量封装类，用于在多线程程序中对互斥量进行加锁和解锁操作，它的主要特点是可以对互斥量进行更加灵活的管理，包括延迟加锁、条件变量、超时等。
 * std::unique_lock提供了以下成员函数。
 * lock():尝试对互斥量进行加锁操作，如果当前互斥量已经被其他线程持有，则当前线程会被阻塞，直到互斥量被成功加锁
 * try_lock():尝试对互斥量进行加锁操作，如果当前互斥量已经被其他线程持有，则函数立即返回false,否则返回true
 * try_lock_for(const std::chrono::duration<Rep,Period>& rel_time):尝试对互斥量进行加锁操作，如果当前互斥量已经被其他线程持有，则当前线程会被阻塞，直到互斥量被成功加锁，或者超过了指定的时间。
 * try_lock_until(const std::chrono::time_point<Clock,Duration>&abs_time):尝试对互斥量进行加锁操作，如果当前互斥量已经被其他线程持有，则当前线程会被阻塞，直到互斥量被成功加锁，或者超过了指定时间
 * unlock():对互斥量进行解锁操作
 *
 *
 *
 *
class log
{
public:
    log(const log &log) = delete;禁用拷贝构造函数
    log &operator=(const log &log) = delete;禁用赋值构造函数
    static log &getlog()
    {
        static log _log;
        return _log;
    }
    void Error(std::string msg)
    {
        std::cout << msg;
    }

private:
    log() {};
};单一类的写法
 *
 *
 *
 *
 *
 *
 *
 */