#include "minispdlog/details/threadpool.h"
#include "minispdlog/asynclogger.h"

namespace minispdlog {
namespace details {

ThreadPool::ThreadPool(size_t queueSize, size_t threadSize)
    : m_queue(queueSize)
{
    if(threadSize == 0 || threadSize > 1000)
    {
        throw std::invalid_argument("ThreadPool thread size must be greater than 0 and less than or equal to 1000");
    }

    if(queueSize == 0 || queueSize > 1000000)
    {
        throw std::invalid_argument("ThreadPool queue size must be greater than 0 and less than or equal to 1000000");
    }

    for(size_t i = 0; i < threadSize; ++i)
    {
        m_workers.emplace_back([this]() { loop(); });
    }
}

ThreadPool::~ThreadPool()
{
    for(size_t i = 0; i < m_workers.size(); ++i)
    {
        AsyncMsg terminateMsg(AsyncMsgType::Shutdown);
        m_queue.enqueue(std::move(terminateMsg));
    }

    for(auto& worker : m_workers)
    {
        if(worker.joinable())
        {
            worker.join();
        }
    }
}

void ThreadPool::post(std::shared_ptr<AsyncLogger>&& logger, const LogMsg& msg)
{
    AsyncMsg asyncMsg(AsyncMsgType::Log, std::move(logger), msg);
    m_queue.enqueue(std::move(asyncMsg));
}

void ThreadPool::postNoWait(std::shared_ptr<AsyncLogger>&& logger, const LogMsg& msg)
{
    AsyncMsg asyncMsg(AsyncMsgType::Log, std::move(logger), msg);
    m_queue.enqueueNoWait(std::move(asyncMsg));
}

void ThreadPool::postFlush(std::shared_ptr<AsyncLogger>&& logger)
{
    AsyncMsg asyncMsg(AsyncMsgType::Flush, std::move(logger));
    m_queue.enqueue(std::move(asyncMsg));
}

void ThreadPool::loop()
{
    while(processNextMsg()){}
}

bool ThreadPool::processNextMsg()
{
    AsyncMsg msg;
    if(!m_queue.dequeueFor(msg, std::chrono::milliseconds(10)))
        return true; // 没有消息，继续等待
    
    switch(msg.m_type)
    {
        case AsyncMsgType::Log:
        {
            if(msg.m_workerPtr)
            {
                // 调用 AsyncLogger 的 backendSinkLog
                msg.m_workerPtr->backendSinkLog(msg);
            }
            return true;
        }
        case AsyncMsgType::Flush:
        {
            if(msg.m_workerPtr)
            {

                msg.m_workerPtr->backendSinkFlush();
            }
            return true;
        }
        case AsyncMsgType::Shutdown:
        {
            // 处理关闭消息，退出循环
            return false;
        }
    }
    return true; // 默认继续处理
}


}// namespace details
}// namespace minispdlog