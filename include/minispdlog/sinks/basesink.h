#pragma once

#include "../common.h"
#include "../details/logmsg.h"
#include "../formatter.h"
#include "../patternformatter.h"
#include <mutex>
#include <memory>

namespace minispdlog {
namespace sinks {

class Sink
{
public:
    virtual ~Sink() = default;
    //输出日志
    virtual void log(const details::LogMsg& msg) = 0;
    //刷新缓冲区
    virtual void flush() = 0;

    virtual void setLevel(level lvl) = 0;

    virtual level getLevel() const = 0;

    virtual bool shouldLog(level msgLevel) const = 0;

    virtual void setFormatter(std::unique_ptr<Formatter> formatter) = 0;
};

template<typename Mutex>
class BaseSink : public Sink
{
public:
    BaseSink()
        : m_level(level::trace)
    {}

    BaseSink(const BaseSink&) = delete;
    BaseSink& operator=(const BaseSink&) = delete;

    void log(const details::LogMsg& msg) override
    {
        std::lock_guard<Mutex> lock(m_mutex);
        sinkLog(msg);
    }

    void flush() override
    {
        std::lock_guard<Mutex> lock(m_mutex);
        sinkFlush();
    }

    void setLevel(level lvl) override
    {
        std::lock_guard<Mutex> lock(m_mutex);
        m_level = lvl;
    }

    level getLevel() const override
    {
        std::lock_guard<Mutex> lock(m_mutex);
        return m_level;
    }

    bool shouldLog(level msgLevel) const override
    {
        std::lock_guard<Mutex> lock(m_mutex);
        return logLevelEnabled(m_level, msgLevel);
    }

    void setFormatter(std::unique_ptr<Formatter> formatter) override
    {
        std::lock_guard<Mutex> lock(m_mutex);
        m_formatter = std::move(formatter);
    }

protected:
    virtual void sinkLog(const details::LogMsg& msg) = 0;
    virtual void sinkFlush() = 0;

    void formatMessage(const details::LogMsg& msg, fmt::memory_buffer& dest)
    {
        m_formatter->format(msg, dest);
    }

    mutable Mutex m_mutex;
    level m_level;
    std::unique_ptr<Formatter> m_formatter;
};

struct NullMutex
{
    void lock() {}
    void unlock() {}
};

using SinkPtr = std::shared_ptr<Sink>;

}//sink
}//minispdlog