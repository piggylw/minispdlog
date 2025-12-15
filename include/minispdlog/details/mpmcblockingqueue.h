#pragma once

#include "circularqueue.h"
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace minispdlog {
namespace details {

template <typename T>
class MPMCBlockingQueue
{
public:
    explicit MPMCBlockingQueue(size_t capacity)
        : m_queue(capacity) {}

    MPMCBlockingQueue(const MPMCBlockingQueue&) = delete;
    MPMCBlockingQueue& operator=(const MPMCBlockingQueue&) = delete;

    //入队(阻塞模式):队列满时阻塞等待
    void enqueue(T&& item)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_producerCond.wait(lock, [this]() { return !m_queue.full(); });
            m_queue.pushBack(std::move(item));
        }
        m_consumerCond.notify_one(); //通知一个等待的消费者线程
    }

    void enqueueNoWait(T&& item)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_queue.pushBack(std::move(item));
        }
        m_consumerCond.notify_one(); //通知一个等待的消费者线程
    }

    bool dequeueFor(T& item, std::chrono::milliseconds waitDuration)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (!m_consumerCond.wait_for(lock, waitDuration, [this]() { return !m_queue.empty(); }))
            {
                return false; //等待超时
            }
            item = std::move(m_queue.front());
            m_queue.popFront();
        }
        m_producerCond.notify_one(); //通知一个等待的生产者线程
        return true;
    }

    size_t overrunCount()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.overrunCountValue();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_producerCond;
    std::condition_variable m_consumerCond;
    CircularQueue<T> m_queue;
};

}
}