#pragma once

#include "minispdlog/details/logmsg.h"
#include <memory>
#include <string>

namespace minispdlog {

class AsyncLogger;

namespace details {

enum class AsyncMsgType 
{
    Log,        //日志消息
    Flush,        //刷新日志
    Shutdown     //关闭日志
};

struct LogMsgBuffer : LogMsg
{
    std::string m_buffer;
    LogMsgBuffer() = default;
    ~LogMsgBuffer() = default;

    explicit LogMsgBuffer(const LogMsg& msg)
        : LogMsg(msg), m_buffer(msg.m_payload.data(), msg.m_payload.size())
    {
        m_payload = StringView(m_buffer.data(), m_buffer.size());
    }

    LogMsgBuffer(LogMsgBuffer&& other) noexcept
        : LogMsg(std::move(other)),
          m_buffer(std::move(other.m_buffer))  
    {
        m_payload = StringView(m_buffer.data(), m_buffer.size());
    }  
    
    LogMsgBuffer& operator=(LogMsgBuffer&& other) noexcept
    {
        if(this != &other) 
        {
            LogMsg::operator=(std::move(other));
            m_buffer = std::move(other.m_buffer);
            m_payload = StringView(m_buffer.data(), m_buffer.size());
        }
        return *this;
    }
};

// AsyncMsg: 异步日志消息
// 参考 spdlog 设计:继承 LogMsgBuffer + AsyncLogger 的 shared_ptr
//
// 关键修正:
//   - worker_ptr 的类型改为 std::shared_ptr<async_logger> (而不是 logger)
//   - 这样 thread_pool 可以调用 AsyncLogger 的 backend_sink_it_()
using AsyncLoggerPtr = std::shared_ptr<minispdlog::AsyncLogger>;

struct AsyncMsg : LogMsgBuffer
{
    AsyncMsgType m_type{AsyncMsgType::Log};
    AsyncLoggerPtr m_workerPtr;

    AsyncMsg() = default;
    ~AsyncMsg() = default;

    AsyncMsg(const AsyncMsg&) = delete;
    AsyncMsg& operator=(const AsyncMsg&) = delete;

    //移动构造函数
    AsyncMsg(AsyncMsg&& other) noexcept
        : LogMsgBuffer(std::move(other)),
          m_type(other.m_type),
          m_workerPtr(std::move(other.m_workerPtr))
    {}

    //移动赋值运算符
    AsyncMsg& operator=(AsyncMsg&& other) noexcept
    {
        if (this != &other)
        {
            LogMsgBuffer::operator=(std::move(other));
            m_type = other.m_type;
            m_workerPtr = std::move(other.m_workerPtr); 
        }
        return *this;
    }

    AsyncMsg(
        AsyncMsgType type,
        AsyncLoggerPtr&& workerPtr,
        const LogMsg& msg
    )
        : LogMsgBuffer(msg),
          m_type(type),
          m_workerPtr(std::move(workerPtr))
    {}

    AsyncMsg(
        AsyncMsgType type,
        AsyncLoggerPtr&& workerPtr
    )
        : LogMsgBuffer{},
          m_type(type),
          m_workerPtr(std::move(workerPtr))
    {}

    explicit AsyncMsg(AsyncMsgType type)
        : AsyncMsg{type, nullptr}
    {}
};


} // namespace details
} // namespace minispdlog