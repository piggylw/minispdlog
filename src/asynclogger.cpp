#include "minispdlog/asynclogger.h"
#include <stdexcept>

namespace minispdlog
{

AsyncLogger::AsyncLogger(
    const std::string& name,
    sinks::SinkPtr singSink,
    std::weak_ptr<details::ThreadPool> threadPool,
    AsyncOverflowPolicy overflowPolicy
)
    : Logger(name, singSink), 
        m_threadPool(threadPool), 
        m_overflowPolicy(overflowPolicy)
{}


AsyncLogger::AsyncLogger(
    const std::string& name,
    std::vector<sinks::SinkPtr> sinks,
    std::weak_ptr<details::ThreadPool> threadPool,
    AsyncOverflowPolicy overflowPolicy
)
    : Logger(name, std::move(sinks)), 
        m_threadPool(threadPool), 
        m_overflowPolicy(overflowPolicy)
{}

void AsyncLogger::sinkLog(const details::LogMsg& msg)
{
    if(auto pool = m_threadPool.lock())
    {
        // 异步投递日志消息
        if(m_overflowPolicy == AsyncOverflowPolicy::Block)
        {
            pool->post(shared_from_this(), msg);
        }
        else
        {
            pool->postNoWait(shared_from_this(), msg);
        }
    }
    else
    {
        throw std::runtime_error("ThreadPool is no longer available");
    }
}

void AsyncLogger::sinkFlush()
{
    if(auto pool = m_threadPool.lock())
    {
        pool->postFlush(shared_from_this());
    }
    else
    {
        throw std::runtime_error("ThreadPool is no longer available");
    }
}

void AsyncLogger::backendSinkLog(const details::LogMsg& msg)
{
    for(auto& sink : m_sinks)
    {
        if(sink->shouldLog(msg.m_level))
        {
            sink->log(msg);
        }
    }

    if(msg.m_level >= m_flushLevel)
    {
        backendSinkFlush();
    }
}

void AsyncLogger::backendSinkFlush()
{
    for(auto& sink : m_sinks)
    {
        sink->flush();
    }
}


} //minispdlog 
