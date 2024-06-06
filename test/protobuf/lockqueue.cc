// #include "lockqueue.h"

// //多个工作线程写日志
// template <typename T>
// void LockQueue<T>::Push(const T& message)
// {
//     std::lock_guard<std::mutex> lock(m_mutex);
//     m_queue.push(message);
//     m_condvar.notify_one();
// }


// //日志线程写日志文件(磁盘I/O)
// template <typename T>
// T LockQueue<T>::Pop()
// {
//     std::unique_lock<std::mutex> lock(m_mutex);
//     while(m_queue.empty())
//     {
//         m_condvar.wait(lock);
//     }
//     T message = m_queue.front();
//     m_queue.pop();
//     return message;
// }