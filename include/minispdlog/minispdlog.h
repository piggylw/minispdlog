
#include "common.h"
#include "level.h"
#include "logger.h"
#include "registry.h"
#include "sinks/consolesink.h"
#include "sinks/colorconsolesink.h"
#include "sinks/filesink.h"
#include "sinks/rotatingfilesink.h"
#include <fmt/format.h>
#include <memory>
#include <string>

namespace minispdlog
{

// Registry 便捷访问
inline std::shared_ptr<Logger> get(const std::string& name) 
{
    return Registry::instance().get(name);
}

inline void registerLogger(std::shared_ptr<Logger> logger) 
{
    Registry::instance().registerLogger(std::move(logger));
}

inline void drop(const std::string& name) 
{
    Registry::instance().drop(name);
}

inline void dropAll() 
{
    Registry::instance().dropAll();
}

inline std::shared_ptr<Logger> defaultLogger() 
{
    return Registry::instance().defaultLogger();   
}

inline void setDefaultLogger(std::shared_ptr<Logger> logger) 
{
    Registry::instance().setDefaultLogger(logger);
}

inline void setLevel(level lvl) 
{
    Registry::instance().setLevel(lvl);
}

inline void flushAll() 
{
    Registry::instance().flushAll();
}

//工厂函数，快速创建logger
inline std::shared_ptr<Logger> colorStdoutMTLogger(const std::string& name) 
{
    auto sink = std::make_shared<sinks::ColorConsoleSinkMT>();
    sink->setFormatter(std::make_unique<PatternFormatter>());
    auto logger = std::make_shared<Logger>(name, sink);
    registerLogger(logger);
    return logger;
}

inline std::shared_ptr<Logger> colorStderrMTLogger(const std::string& name) 
{
    auto sink = std::make_shared<sinks::ColorStderrSinkMT>();
    sink->setFormatter(std::make_unique<PatternFormatter>());
    auto logger = std::make_shared<Logger>(name, sink);
    registerLogger(logger);
    return logger;
}

inline std::shared_ptr<Logger> stdoutMTLogger(const std::string& name) 
{
    auto sink = std::make_shared<sinks::ConsoleSinkMT>();
    sink->setFormatter(std::make_unique<PatternFormatter>());
    auto logger = std::make_shared<Logger>(name, sink);
    registerLogger(logger);
    return logger;
}

inline std::shared_ptr<Logger> fileLoggerMTLogger(const std::string& name, const std::string& filename, bool truncate) 
{
    auto sink = std::make_shared<sinks::FileSinkMT>(filename, truncate);
    sink->setFormatter(std::make_unique<PatternFormatter>());
    auto logger = std::make_shared<Logger>(name, sink);
    registerLogger(logger);
    return logger;
}

inline std::shared_ptr<Logger> rotatingFileLoggerMT(const std::string& name, const std::string& baseFilename, size_t maxSize, size_t maxFiles) 
{
    auto sink = std::make_shared<sinks::RotatingFileSinkMT>(baseFilename, maxSize, maxFiles);
    sink->setFormatter(std::make_unique<PatternFormatter>());
    auto logger = std::make_shared<Logger>(name, sink);
    registerLogger(logger);
    return logger;
}

inline std::shared_ptr<Logger> colorStdoutSTLogger(const std::string& name) 
{
    auto sink = std::make_shared<sinks::ColorConsoleSinkST>();
    sink->setFormatter(std::make_unique<PatternFormatter>());
    auto logger = std::make_shared<Logger>(name, sink);
    registerLogger(logger);
    return logger;
}

inline std::shared_ptr<Logger> colorStderrSTLogger(const std::string& name) 
{
    auto sink = std::make_shared<sinks::ColorStderrSinkST>();
    sink->setFormatter(std::make_unique<PatternFormatter>());
    auto logger = std::make_shared<Logger>(name, sink);
    registerLogger(logger);
    return logger;
}

inline std::shared_ptr<Logger> stdoutSTLogger(const std::string& name) 
{
    auto sink = std::make_shared<sinks::ConsoleSinkST>();
    sink->setFormatter(std::make_unique<PatternFormatter>());
    auto logger = std::make_shared<Logger>(name, sink);
    registerLogger(logger);
    return logger;
}

inline std::shared_ptr<Logger> fileLoggerSTLogger(const std::string& name, const std::string& filename, bool truncate) 
{
    auto sink = std::make_shared<sinks::FileSinkST>(filename, truncate);
    sink->setFormatter(std::make_unique<PatternFormatter>());
    auto logger = std::make_shared<Logger>(name, sink);
    registerLogger(logger);
    return logger;
}

inline std::shared_ptr<Logger> rotatingFileLoggerST(const std::string& name, const std::string& baseFilename, size_t maxSize, size_t maxFiles) 
{
    auto sink = std::make_shared<sinks::RotatingFileSinkST>(baseFilename, maxSize, maxFiles);
    sink->setFormatter(std::make_unique<PatternFormatter>());
    auto logger = std::make_shared<Logger>(name, sink);
    registerLogger(logger);
    return logger;
}

//全局日志接口，使用默认logger
template<typename... Args>
inline void trace(fmt::format_string<Args...> fmt, Args&&... args) 
{
    defaultLogger()->trace(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void debug(fmt::format_string<Args...> fmt, Args&&... args) 
{
    defaultLogger()->debug(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void info(fmt::format_string<Args...> fmt, Args&&... args) 
{
    defaultLogger()->info(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void warn(fmt::format_string<Args...> fmt, Args&&... args) 
{
    defaultLogger()->warn(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void error(fmt::format_string<Args...> fmt, Args&&... args) 
{
    defaultLogger()->error(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void critical(fmt::format_string<Args...> fmt, Args&&... args)
{
    defaultLogger()->critical(fmt, std::forward<Args>(args)...);
}


}//minispdlog