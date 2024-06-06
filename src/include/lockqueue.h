#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>


//线程之间通信:互斥量+条件变量
template <typename T>
class LockQueue
{
    public:
        //写入日志到缓冲队列
        void Push(const T& message)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(message);
            m_condvar.notify_one();
        }


        //日志线程写日志文件(磁盘I/O)
        T Pop()
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            while(m_queue.empty())
            {
                m_condvar.wait(lock);
            }
            T message = m_queue.front();
            m_queue.pop();
            return message;
        }
       

    private:
        std::queue<T> m_queue;
        std::mutex m_mutex;
        std::condition_variable m_condvar;
};