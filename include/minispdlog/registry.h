#pragma once

#include "common.h"
#include "logger.h"
#include "details/threadpool.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>

namespace minispdlog
{

// 参考 spdlog 设计:全局管理所有 logger,提供便捷访问
class Registry
{
public:
    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;

    static Registry& instance() 
    {
        static Registry reg;
        return reg;
    }

    void registerLogger(std::shared_ptr<Logger> logger);
    std::shared_ptr<Logger> get(const std::string& loggerName);
    void drop(const std::string& loggerName);
    void dropAll();
    std::shared_ptr<Logger> defaultLogger();
    void setDefaultLogger(std::shared_ptr<Logger> logger);

    //设置所有logger级别，刷新所有logger
    void setLevel(level lvl);
    void flushAll();

    //初始化全局线程池
    // 注意:必须在创建异步 logger 之前调用
    void initThreadPool(size_t queueSize = 8192, size_t threadSize = 1);

    std::shared_ptr<details::ThreadPool> getThreadPool();

    void setThreadPool(std::shared_ptr<details::ThreadPool> threadPool);

private:
    Registry();
    ~Registry() = default;

    void ifExistsThrow(const std::string& loggerName);

    std::mutex m_mutex;
    std::unordered_map<std::string, std::shared_ptr<Logger>> m_loggers;
    std::shared_ptr<Logger> m_defaultLogger;
    std::shared_ptr<details::ThreadPool> m_threadPool;
};

}//minispdlog