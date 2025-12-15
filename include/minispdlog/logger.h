#pragma once

#include "common.h"
#include "level.h"
#include "sinks/basesink.h"
#include "details/logmsg.h"
#include "details/threadpool.h"
#include <fmt/format.h>
#include <vector>
#include <memory>
#include <string>

namespace minispdlog
{

class Logger
{
public:
    explicit Logger(const std::string& name);
    explicit Logger(const std::string& name, sinks::SinkPtr singleSink);
    explicit Logger(const std::string& name, std::vector<sinks::SinkPtr> sinks);

    virtual ~Logger() = default;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // 变参模板接口:支持 fmt 格式化
    // 示例: logger->info("Hello, {}!", "World")
    template<typename... Args>
    void trace(fmt::format_string<Args...> fmt, Args&&... args) {
        log(level::trace, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void debug(fmt::format_string<Args...> fmt, Args&&... args) {
        log(level::debug, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(fmt::format_string<Args...> fmt, Args&&... args) {
        log(level::info, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(fmt::format_string<Args...> fmt, Args&&... args) {
        log(level::warn, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(fmt::format_string<Args...> fmt, Args&&... args) {
        log(level::error, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void critical(fmt::format_string<Args...> fmt, Args&&... args) {
        log(level::critical, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void log(level lvl, fmt::format_string<Args...> fmt, Args&&... args)
    {
        if(!shouldLog(lvl)) 
        {
            return;
        }

        fmt::memory_buffer buf;
        fmt::format_to(std::back_inserter(buf), fmt, std::forward<Args>(args)...);
        details::LogMsg msg(m_name, lvl, StringView(buf.data(), buf.size()));
        sinkLog(msg);
    }
    
    // ========== Sink 管理 ==========
    void addSink(sinks::SinkPtr sink);
    void removeSink(sinks::SinkPtr sink);
    std::vector<sinks::SinkPtr>& sinks();
    const std::vector<sinks::SinkPtr>& sinks() const;

    // ========== 日志级别管理 ==========
    void setLevel(level lvl);
    level getLevel() const;
    bool shouldLog(level msgLevel) const;

    // ========== 刷新操作 ==========
    void flush();
    void flushOn(level lvl);

    // ========== 日志名称 ==========
    const std::string& name() const;

protected:
    virtual void sinkLog(const details::LogMsg& msg);
    virtual void sinkFlush();

    friend class details::ThreadPool;

    std::string m_name;
    std::vector<sinks::SinkPtr> m_sinks;
    level m_level{level::trace};
    level m_flushLevel{level::off};

};

}//minispdlog