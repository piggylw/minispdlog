#pragma once

#include "basesink.h"
#include <iostream>
#include <mutex>
#include <array>

namespace minispdlog {
namespace sinks {

namespace color {
    constexpr const char* reset   = "\033[0m";
    constexpr const char* trace    = "\033[90m";      // 灰色 - 低调
    constexpr const char* debug    = "\033[36m";      // 青色 - 清晰
    constexpr const char* info     = "\033[32m";      // 绿色 - 舒适
    constexpr const char* warn     = "\033[93m\033[1m";  // 亮黄色+加粗 - 醒目
    constexpr const char* error    = "\033[91m\033[1m";  // 亮红色+加粗 - 显眼
    constexpr const char* critical = "\033[41m\033[97m\033[1m";  // 红底白字+加粗 
}

template<typename Mutex>
class ColorConsoleSink : public BaseSink<Mutex>
{
public:
    ColorConsoleSink()
    {
        // 初始化颜色映射
        m_colors[static_cast<int>(level::trace)] = color::trace;
        m_colors[static_cast<int>(level::debug)] = color::debug;
        m_colors[static_cast<int>(level::info)] = color::info;
        m_colors[static_cast<int>(level::warn)] = color::warn;
        m_colors[static_cast<int>(level::error)] = color::error;
        m_colors[static_cast<int>(level::critical)] = color::critical;
    }

    ~ColorConsoleSink() override = default;

protected:
    void sinkLog(const details::LogMsg& msg) override
    {
        fmt::memory_buffer formattedMsg;
        this->formatMessage(msg, formattedMsg);

        // 添加颜色前缀
        const std::string& prefix = m_colors[static_cast<int>(msg.m_level)];
        
        // 输出: 颜色前缀 + 消息 + 颜色重置
        std::cout << prefix;
        std::cout.write(formattedMsg.data(), formattedMsg.size());
        std::cout << color::reset;
    }

    void sinkFlush() override
    {
        std::cout << std::flush;
    }

private:
    std::array<std::string, 7> m_colors;
};

using ColorConsoleSinkMT = ColorConsoleSink<std::mutex>;
using ColorConsoleSinkST = ColorConsoleSink<NullMutex>;


template<typename Mutex>
class ColorStderrSink : public BaseSink<Mutex>
{
public:
    ColorStderrSink()
    {
        m_colors[static_cast<int>(level::trace)] = color::trace;
        m_colors[static_cast<int>(level::debug)] = color::debug;
        m_colors[static_cast<int>(level::info)] = color::info;
        m_colors[static_cast<int>(level::warn)] = color::warn;
        m_colors[static_cast<int>(level::error)] = color::error;
        m_colors[static_cast<int>(level::critical)] = color::critical;
    }

    ~ColorStderrSink() override = default;

protected:
    void sinkLog(const details::LogMsg& msg) override
    {
        fmt::memory_buffer formattedMsg;
        this->formatMessage(msg, formattedMsg);

        // 添加颜色前缀
        const std::string& prefix = m_colors[static_cast<int>(msg.m_level)];
        
        // 输出: 颜色前缀 + 消息 + 颜色重置
        std::cerr << prefix;
        std::cerr.write(formattedMsg.data(), formattedMsg.size());
        std::cerr << color::reset;
    }

    void sinkFlush() override
    {
        std::cout << std::flush;
    }

private:
    std::array<std::string, 7> m_colors;
};

using ColorStderrSinkMT = ColorStderrSink<std::mutex>;
using ColorStderrSinkST = ColorStderrSink<NullMutex>;

}
}