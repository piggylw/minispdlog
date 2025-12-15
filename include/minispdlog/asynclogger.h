#pragma once

#include "logger.h"
#include "details/threadpool.h"
#include <memory>

namespace minispdlog {

namespace details {
class ThreadPool;
}

enum class AsyncOverflowPolicy
{
    Overwrite, // 覆盖旧数据
    Block // 阻塞等待
};

class AsyncLogger final: public Logger, public std::enable_shared_from_this<AsyncLogger>
{
    friend class details::ThreadPool;

public:
    AsyncLogger(
        const std::string& name,
        sinks::SinkPtr singSink,
        std::weak_ptr<details::ThreadPool> threadPool,
        AsyncOverflowPolicy overflowPolicy = AsyncOverflowPolicy::Block
    );

    AsyncLogger(
        const std::string& name,
        std::vector<sinks::SinkPtr> sinks,
        std::weak_ptr<details::ThreadPool> threadPool,
        AsyncOverflowPolicy overflowPolicy = AsyncOverflowPolicy::Block
    );

    ~AsyncLogger() override = default;

    AsyncLogger(const AsyncLogger&) = delete;
    AsyncLogger& operator=(const AsyncLogger&) = delete;

protected:
    void sinkLog(const details::LogMsg& msg) override;
    void sinkFlush() override;

    // 后台线程调用:真正执行日志输出
    // 注意:这个方法在工作线程中执行,不是用户线程
    void backendSinkLog(const details::LogMsg& msg);
    void backendSinkFlush();

private:
    std::weak_ptr<details::ThreadPool> m_threadPool; // 弱引用线程池
    AsyncOverflowPolicy m_overflowPolicy;
};


}//minispdlog