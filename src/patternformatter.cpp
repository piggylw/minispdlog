#include "minispdlog/patternformatter.h"
#include "minispdlog/details/utils.h"
#include <iomanip>
#include <sstream>
#include <cctype>

namespace minispdlog
{

    // 快速整数到字符串转换
inline void fast_uint_to_str(uint64_t n, char* buffer) {
    // 使用查表法优化小数字
    static constexpr char digits_table[] = 
        "0001020304050607080910111213141516171819"
        "2021222324252627282930313233343536373839"
        "4041424344454647484950515253545556575859"
        "6061626364656667686970717273747576777879"
        "8081828384858687888990919293949596979899";
    
    if (n < 100) {
        if (n < 10) {
            buffer[0] = '0' + n;
            buffer[1] = '\0';
        } else {
            const char* d = digits_table + n * 2;
            buffer[0] = d[0];
            buffer[1] = d[1];
            buffer[2] = '\0';
        }
        return;
    }
    
    // 对于大数字，回退到标准方法
    fmt::format_to(buffer, "{}", n);
}

// 快速两位数转换(用于时间格式化)
inline void fast_two_digits(uint32_t n, char* buffer) {
    if (n < 100) {
        const char* d = "0001020304050607080910111213141516171819"
                       "2021222324252627282930313233343536373839"
                       "4041424344454647484950515253545556575859"
                       "6061626364656667686970717273747576777879"
                       "8081828384858687888990919293949596979899" + n * 2;
        buffer[0] = d[0];
        buffer[1] = d[1];
    } else {
        buffer[0] = '0';
        buffer[1] = '0';
    }
}

//普通文本
class RawStringFormatter : public PatternFormatter::FlagFormatter
{
public:
    explicit RawStringFormatter(const std::string& str)
        : m_str(str)
    {}

    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        dest.append(m_str.data(), m_str.data() + m_str.size());
    }

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<RawStringFormatter>(m_str);
    }

private:
    std::string m_str;
};

class AggregateFormatter : public PatternFormatter::FlagFormatter
{
public:
    explicit AggregateFormatter(std::string str)
        : m_str(std::move(str))
    {}

    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        dest.append(m_str.data(), m_str.data() + m_str.size());
    }
    
    void addCh(char ch){ m_str += ch; }
    void addStr(const std::string& str){ m_str += str; }

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<AggregateFormatter>(m_str);
    }

private:
    std::string m_str;
};

//%Y : 年份
class YearFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        char buffer[5];
        int year = time.tm_year + 1900;
        // 手动展开避免 sprintf 开销
        buffer[0] = '0' + (year / 1000);
        buffer[1] = '0' + ((year / 100) % 10);
        buffer[2] = '0' + ((year / 10) % 10);
        buffer[3] = '0' + (year % 10);
        buffer[4] = '\0';
        dest.append(buffer, buffer + 4);
    }

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<YearFormatter>();
    }
};

//%m : 月份
class MonthFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        char buffer[3];
        fast_two_digits(time.tm_mon + 1, buffer);
        dest.append(buffer, buffer + 2);
    }

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<MonthFormatter>();
    }
};

//%d : 日期
class DayFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        char buffer[3];
        fast_two_digits(time.tm_mday, buffer);
        dest.append(buffer, buffer + 2);
    }   

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<DayFormatter>();
    }

};

//H : 小时
class HourFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        char buffer[3];
        fast_two_digits(time.tm_hour, buffer);
        dest.append(buffer, buffer + 2);
    }   

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<HourFormatter>();
    }
};

//%M : 分钟
class MinuteFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        char buffer[3];
        fast_two_digits(time.tm_min, buffer);
        dest.append(buffer, buffer + 2);
    }   

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<MinuteFormatter>();
    }
};

//%S : 秒
class SecondFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        char buffer[3];
        fast_two_digits(time.tm_sec, buffer);
        dest.append(buffer, buffer + 2);
    }   

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<SecondFormatter>();
    }
};

//%t : 线程ID
class ThreadIdFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        char buffer[64];
        fast_uint_to_str(static_cast<uint64_t>(msg.m_threadId), buffer);
        size_t len = std::strlen(buffer);
        dest.append(buffer, buffer + len);
    }   

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<ThreadIdFormatter>();
    }
};

//%l : 日志级别(短格式 I W E C T D)
class LevelShortFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        static constexpr std::string_view levelStrings[] = {
            "T", "D", "I", "W", "E", "C"  // 编译时预计算
        };
        
        auto levelStr = levelStrings[static_cast<size_t>(msg.m_level)];
        dest.append(levelStr.data(), levelStr.data() + levelStr.size());
    }   

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<LevelShortFormatter>();
    }
};

//%L : 日志级别(完整格式 INFO WARN ERROR CRITICAL TRACE DEBUG)
class LevelFullFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        static constexpr std::string_view level_strings[] = {
            "trace", "debug", "info", "warning", "error", "critical"
        };

        auto level_str = level_strings[static_cast<size_t>(msg.m_level)];
        dest.append(level_str.data(), level_str.data() + level_str.size());

    }   

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<LevelFullFormatter>();
    }
};

//%n : logger名称
class LoggerNameFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        dest.append(msg.m_loggerName.data(), msg.m_loggerName.data() + msg.m_loggerName.size());
    }   

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<LoggerNameFormatter>();
    }
};

//%v : 日志消息内容
class PayloadFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        dest.append(msg.m_payload.data(), msg.m_payload.data() + msg.m_payload.size());
    }   

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<PayloadFormatter>();
    }
};

//%F : 源码文件名
class SourceFileFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        dest.append(msg.m_sourceLocation.m_fileName,
                    msg.m_sourceLocation.m_fileName + std::strlen(msg.m_sourceLocation.m_fileName));
    }   

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<SourceFileFormatter>();
    }
};

//%f : 源码所在函数
class SourceFunctionFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        dest.append(msg.m_sourceLocation.m_functionName,
                    msg.m_sourceLocation.m_functionName + std::strlen(msg.m_sourceLocation.m_functionName));
    }   

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<SourceFunctionFormatter>();
    }
};

//%P : 源码行号
class SourceLineFormatter : public PatternFormatter::FlagFormatter
{
public:
    void format(const details::LogMsg& msg, const std::tm& time, fmt::memory_buffer& dest) override
    {
        fmt::format_to(std::back_inserter(dest), "{}", msg.m_sourceLocation.m_line);
    }   

    std::unique_ptr<PatternFormatter::FlagFormatter> clone() const override
    {
        return std::make_unique<SourceLineFormatter>();
    }
};


//PatternFormatter 方法实现
PatternFormatter::PatternFormatter(std::string pattern)
    : m_pattern(std::move(pattern))
{
    compilePattern();
}

void PatternFormatter::format(const details::LogMsg& msg, fmt::memory_buffer& dest)
{
    dest.reserve(dest.size() + 256); // 预留空间，避免多次内存分配

    auto secs = std::chrono::duration_cast<std::chrono::seconds>(msg.m_timePoint.time_since_epoch());
    if (secs != m_lastTimeSec)
    {
        m_cachedTm = getTime(msg);
        m_lastTimeSec = secs;
    }

    for(auto& formatter : m_formatters)
    {
        formatter->format(msg, m_cachedTm, dest);
    }

    dest.push_back('\n');
}

std::unique_ptr<Formatter> PatternFormatter::clone() const
{
    return std::make_unique<PatternFormatter>(m_pattern);
}

void PatternFormatter::setPattern(const std::string& pattern)
{
    m_pattern = pattern;
    m_formatters.clear();
    compilePattern();
}

void PatternFormatter::compilePattern()
{
    auto it = m_pattern.begin();
    auto end = m_pattern.end();
    std::unique_ptr<AggregateFormatter> userChars;
    
    while (it != end) {
        if (*it == '%') {
            // 保存聚合的普通文本
            if (userChars) {
                m_formatters.push_back(std::move(userChars));
            }
            
            // 解析占位符
            ++it;
            if (it != end) {
                char flag = *it;
                ++it;
                
                // 根据 flag 创建对应的 formatter
                switch (flag) {
                    case 'Y': m_formatters.push_back(std::make_unique<YearFormatter>()); break;
                    case 'm': m_formatters.push_back(std::make_unique<MonthFormatter>()); break;
                    case 'd': m_formatters.push_back(std::make_unique<DayFormatter>()); break;
                    case 'H': m_formatters.push_back(std::make_unique<HourFormatter>()); break;
                    case 'M': m_formatters.push_back(std::make_unique<MinuteFormatter>()); break;
                    case 'S': m_formatters.push_back(std::make_unique<SecondFormatter>()); break;
                    case 'l': m_formatters.push_back(std::make_unique<LevelShortFormatter>()); break;
                    case 'L': m_formatters.push_back(std::make_unique<LevelFullFormatter>()); break;
                    case 'n': m_formatters.push_back(std::make_unique<LoggerNameFormatter>()); break;
                    case 'v': m_formatters.push_back(std::make_unique<PayloadFormatter>()); break;
                    case 't': m_formatters.push_back(std::make_unique<ThreadIdFormatter>()); break;
                    case '%': 
                        if (!userChars) userChars = std::make_unique<AggregateFormatter>("");
                        userChars->addCh('%'); 
                        break;
                    default:
                        // 未知占位符,创建聚合formatter
                        if (!userChars) userChars = std::make_unique<AggregateFormatter>("");
                        userChars->addCh('%');
                        userChars->addCh(flag);
                        break;
                }
            }
        } else {
            // 普通字符,累积到聚合formatter
            if (!userChars) {
                userChars = std::make_unique<AggregateFormatter>("");
            }
            userChars->addCh(*it);
            ++it;
        }
    }
    
    // 处理末尾的聚合文本
    if (userChars) {
        m_formatters.push_back(std::move(userChars));
    }
}

std::tm PatternFormatter::getTime(const details::LogMsg& msg)
{
    auto timeT = LogClock::to_time_t(msg.m_timePoint);
    std::tm tmVal;
    localtime_r(&timeT, &tmVal);
    return tmVal;
}

}//minispdlog