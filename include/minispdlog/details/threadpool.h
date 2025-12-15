#pragma once

#include "minispdlog/common.h"
#include "minispdlog/details/mpmcblockingqueue.h"
#include "minispdlog/details/asyncmsg.h"
#include <thread>
#include <vector>
#include <functional>

namespace minispdlog {

class AsyncLogger;

namespace details {

// thread_pool: 异步日志的线程池
// 参考 spdlog 设计:管理工作线程 + MPMC 队列
//
// 特性:
//   - 创建指定数量的工作线程
//   - 持有 MPMC 队列用于消息传递
//   - 支持阻塞/非阻塞两种 post 模式
//   - 支持优雅关闭
class ThreadPool
{
public:
    explicit ThreadPool(size_t queueSize, size_t threadSize);

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool();

    // 向线程池中添加异步消息(阻塞)
    void post(std::shared_ptr<AsyncLogger>&& logger, const LogMsg& msg);

    // 向线程池中添加异步消息(非阻塞)
    void postNoWait(std::shared_ptr<AsyncLogger>&& logger, const LogMsg& msg);

    //投递刷新
    void postFlush(std::shared_ptr<AsyncLogger>&& logger);

    size_t overrunCount()
    {
        return m_queue.overrunCount();
    }

private:
    void loop();
    // 处理下一条消息(返回 false 表示应该退出)
    bool processNextMsg();

private:
    std::vector<std::thread> m_workers; // 工作线程
    MPMCBlockingQueue<details::AsyncMsg> m_queue; // MPMC 队列
};

}
}