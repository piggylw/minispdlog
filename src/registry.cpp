#include "minispdlog/registry.h"
#include "minispdlog/sinks/colorconsolesink.h"
#include <stdexcept>

namespace minispdlog
{
Registry::Registry()
{
    //默认使用color console
    auto consoleSink = std::make_shared<sinks::ColorConsoleSinkMT>();
    consoleSink->setFormatter(std::make_unique<PatternFormatter>());

    m_defaultLogger = std::make_shared<Logger>("DefaultLogger", consoleSink);
    m_defaultLogger->setLevel(level::info);
}

void Registry::registerLogger(std::shared_ptr<Logger> logger)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    ifExistsThrow(logger->name());
    m_loggers[logger->name()] = std::move(logger);
}

std::shared_ptr<Logger> Registry::get(const std::string& loggerName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_loggers.find(loggerName);
    if(it != m_loggers.end())
    {
        return it->second;
    }
    return nullptr;
}

void Registry::drop(const std::string& loggerName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_loggers.erase(loggerName);

    // if(m_defaultLogger && m_defaultLogger->name() == loggerName)
    // {
    //     m_defaultLogger.reset();
    // }
}

void Registry::dropAll()
{

    std::lock_guard<std::mutex> lock(m_mutex);
    m_loggers.clear();
    // m_defaultLogger.reset();
}

std::shared_ptr<Logger> Registry::defaultLogger()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_defaultLogger;
}

void Registry::setDefaultLogger(std::shared_ptr<Logger> logger)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_defaultLogger = std::move(logger);
}

//设置所有logger级别，刷新所有logger
void Registry::setLevel(level lvl)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for(auto& pair : m_loggers)
    {
        pair.second->setLevel(lvl);
    }
    if(m_defaultLogger)
    {
        m_defaultLogger->setLevel(lvl);     
    }
}

void Registry::flushAll()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for(auto& pair : m_loggers)
    {
        pair.second->flush();
    }
    if(m_defaultLogger)
    {
        m_defaultLogger->flush();
    }
}

void Registry::ifExistsThrow(const std::string& loggerName)
{
    if(m_loggers.find(loggerName) != m_loggers.end())
    {
        throw std::runtime_error("Logger with name '" + loggerName + "' already exists.");
    }
}

void Registry::initThreadPool(size_t queueSize, size_t threadSize)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_threadPool = std::make_shared<details::ThreadPool>(queueSize, threadSize);
}

std::shared_ptr<details::ThreadPool> Registry::getThreadPool()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(!m_threadPool)
    {
        m_threadPool = std::make_shared<details::ThreadPool>(8192, 1);
    }
    return m_threadPool;
}


void Registry::setThreadPool(std::shared_ptr<details::ThreadPool> threadPool)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_threadPool = std::move(threadPool);
}


} // minispdlog
