#pragma once

#include "common.h"
#include "asynclogger.h"
#include "registry.h"
#include "sinks/filesink.h"
#include "sinks/rotatingfilesink.h"
#include "sinks/consolesink.h"
#include "sinks/colorconsolesink.h"
#include <memory>
#include <string>

namespace minispdlog
{
// async.h:异步日志工厂函数
// 参考 spdlog 设计:提供便捷的异步 logger 创建接口
//
// 使用方式:
//   #include "minispdlog/async.h"
//   
//   // 方式1:使用默认线程池
//   auto logger = minispdlog::async_stdout_color_mt("async_console");
//   
//   // 方式2:自定义线程池配置
//   minispdlog::init_thread_pool(16384, 2);  // 队列16384,2个线程
//   auto logger = minispdlog::async_file_mt("async_file", "log.txt");

inline void initThreadPool(size_t queueSize = 8192, size_t threadSize = 1)
{
    Registry::instance().initThreadPool(queueSize, threadSize);
}

inline std::shared_ptr<details::ThreadPool> getThreadPool()
{
    return Registry::instance().getThreadPool();
}

// ============================================================================
// 异步 Logger 工厂函数
// ============================================================================

// 创建异步彩色控制台 logger(多线程安全)
// overflowpolicy: 溢出策略(默认 block)

inline std::shared_ptr<AsyncLogger> asyncStdoutColorMTLogger(
    const std::string& name,
    AsyncOverflowPolicy overflowPolicy = AsyncOverflowPolicy::Block
)
{
    auto sink = std::make_shared<sinks::ColorConsoleSinkMT>();
    sink->setFormatter(std::make_unique<PatternFormatter>());   
    auto threadPool = Registry::instance().getThreadPool();
    auto logger = std::make_shared<AsyncLogger>(name, sink, threadPool, overflowPolicy);
    Registry::instance().registerLogger(logger);
    return logger;
}

inline std::shared_ptr<AsyncLogger> asyncStderrColorMTLogger(
    const std::string& name,
    AsyncOverflowPolicy overflowPolicy = AsyncOverflowPolicy::Block
)
{
    auto sink = std::make_shared<sinks::ColorStderrSinkMT>();
    sink->setFormatter(std::make_unique<PatternFormatter>());   
    auto threadPool = Registry::instance().getThreadPool();
    auto logger = std::make_shared<AsyncLogger>(name, sink, threadPool, overflowPolicy);
    Registry::instance().registerLogger(logger);
    return logger;
}

inline std::shared_ptr<AsyncLogger> asyncStdoutMTLogger(
    const std::string& name,
    AsyncOverflowPolicy overflowPolicy = AsyncOverflowPolicy::Block
)
{
    auto sink = std::make_shared<sinks::ConsoleSinkMT>();
    sink->setFormatter(std::make_unique<PatternFormatter>());   
    auto threadPool = Registry::instance().getThreadPool();
    auto logger = std::make_shared<AsyncLogger>(name, sink, threadPool, overflowPolicy);
    Registry::instance().registerLogger(logger);
    return logger;
}

inline std::shared_ptr<AsyncLogger> asyncStderrMTLogger(
    const std::string& name,
    AsyncOverflowPolicy overflowPolicy = AsyncOverflowPolicy::Block
)
{
    auto sink = std::make_shared<sinks::StderrSinkMT>();
    sink->setFormatter(std::make_unique<PatternFormatter>());   
    auto threadPool = Registry::instance().getThreadPool();
    auto logger = std::make_shared<AsyncLogger>(name, sink, threadPool, overflowPolicy);
    Registry::instance().registerLogger(logger);
    return logger;
}

inline std::shared_ptr<AsyncLogger> asyncFileMTLogger(
    const std::string& name,
    const std::string& filename,
    bool truncate = false,
    AsyncOverflowPolicy overflowPolicy = AsyncOverflowPolicy::Block
)
{
    auto sink = std::make_shared<sinks::FileSinkMT>(filename, truncate);
    sink->setFormatter(std::make_unique<PatternFormatter>());   
    auto threadPool = Registry::instance().getThreadPool();
    auto logger = std::make_shared<AsyncLogger>(name, sink, threadPool, overflowPolicy);
    Registry::instance().registerLogger(logger);
    return logger;
}

inline std::shared_ptr<AsyncLogger> asyncRotatingFileMTLogger(
    const std::string& name,
    const std::string& filename,
    size_t max_size,
    size_t max_files,
    AsyncOverflowPolicy overflowPolicy = AsyncOverflowPolicy::Block
)
{
    auto sink = std::make_shared<sinks::RotatingFileSinkMT>(filename, max_size, max_files);
    sink->setFormatter(std::make_unique<PatternFormatter>());   
    auto threadPool = Registry::instance().getThreadPool();
    auto logger = std::make_shared<AsyncLogger>(name, sink, threadPool, overflowPolicy);
    Registry::instance().registerLogger(logger);
    return logger;
}

// ============================================================================
// 高级用法:手动创建异步 logger(不自动注册)
// ============================================================================

// 手动创建异步 logger(需要自己注册)
// 适用于需要自定义 sink 组合的场景
template<typename Sink, typename... SinkArgs>
inline std::shared_ptr<AsyncLogger> createAsyncLogger(
    const std::string& name,
    AsyncOverflowPolicy overflowPolicy,
    SinkArgs&&... args
)
{
    auto sink = std::make_shared<Sink>(std::forward<SinkArgs>(args)...);
    sink->setFormatter(std::make_unique<PatternFormatter>());
    auto threadPool = Registry::instance().getThreadPool();
    auto logger = std::make_shared<AsyncLogger>(name, sink, threadPool, overflowPolicy);
    return logger; // 不自动注册,需要手动调用 Registry::instance().registerLogger(logger);
}

// 使用默认溢出策略(block)
template<typename Sink, typename... SinkArgs>
inline std::shared_ptr<AsyncLogger> createAsyncLogger(
    const std::string& name,
    SinkArgs&&... args
)
{
    return createAsyncLogger<Sink>(name, AsyncOverflowPolicy::Block, std::forward<SinkArgs>(args)...);
}


}//minispdlog