#include "minispdlog/level.h"
#include <array>
#include <algorithm>

namespace minispdlog
{
    static constexpr std::array<const char*, 7> levelNames = {
        "trace",
        "debug",
        "info",
        "warn",
        "err",
        "critical",
        "off"
    };

    static constexpr std::array<const char*, 7> levelShortNames = {
        "T",
        "D",
        "I",
        "W",
        "E",
        "C",
        "O"
    };


const char *level2String(level lv)
{
    auto index = static_cast<size_t>(lv);
    if (index < levelNames.size())
    {
        return levelNames[index];
    }
    return "unknown";
}

const char *level2ShortString(level lv)
{
    auto index = static_cast<size_t>(lv);
    if (index < levelShortNames.size())
    {
        return levelShortNames[index];
    }
    return "unknown";
}

level string2Level(const std::string &str)
{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), 
                   [](unsigned char c) { return std::tolower(c); });

    for(size_t i = 0; i < levelNames.size(); ++i)
    {
        if (lowerStr == levelNames[i])
        {
            return static_cast<level>(i);
        }
    }
    return level::info;
}

bool logLevelEnabled(level loggerLevel, level msgLevel)
{
    return msgLevel >= loggerLevel;
}

}