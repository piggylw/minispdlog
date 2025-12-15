#include "minispdlog/logger.h"
#include <algorithm>

namespace minispdlog
{
    
Logger::Logger(const std::string& name)
    : m_name(std::move(name))
{}

Logger::Logger(const std::string& name, sinks::SinkPtr singleSink)
    : m_name(std::move(name))
{
    m_sinks.push_back(std::move(singleSink));
}

Logger::Logger(const std::string& name, std::vector<sinks::SinkPtr> sinks)
    : m_name(std::move(name))
    , m_sinks(std::move(sinks))
{}

void Logger::addSink(sinks::SinkPtr sink) 
{
    m_sinks.push_back(std::move(sink));
}

void Logger::removeSink(sinks::SinkPtr sink) 
{
    m_sinks.erase(std::remove(m_sinks.begin(), m_sinks.end(), sink), m_sinks.end());
}

std::vector<sinks::SinkPtr>& Logger::sinks() 
{
    return m_sinks;
}

const std::vector<sinks::SinkPtr>& Logger::sinks() const 
{
    return m_sinks;
}

void Logger::setLevel(level log_level) 
{
    m_level = log_level;
}

level Logger::getLevel() const 
{
    return m_level;
}

bool Logger::shouldLog(level msg_level) const 
{
    return msg_level >= m_level;
}

void Logger::flush() 
{
    sinkFlush();
}

void Logger::flushOn(level log_level) 
{
    m_flushLevel = log_level;
}

const std::string& Logger::name() const 
{
    return m_name;
}

void Logger::sinkLog(const details::LogMsg& msg) 
{
    for (auto& sink : m_sinks)
    {
        if (sink->shouldLog(msg.m_level)) 
        {
            sink->log(msg);
        }
    }
    
    // 如果消息级别 >= m_flushLevel,自动刷新
    if (msg.m_level >= m_flushLevel) 
    {
        flush();
    }
}

void Logger::sinkFlush() 
{
    for (auto& sink : m_sinks) 
    {
        sink->flush();
    }
}

}//minispdlog