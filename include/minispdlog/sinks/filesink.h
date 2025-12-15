#pragma once

#include "basesink.h"
#include <fstream>
#include <string>
#include <mutex>

namespace minispdlog {
namespace sinks {

template<typename Mutex>
class FileSink : public BaseSink<Mutex>
{
public:
    explicit FileSink(const std::string& filename, bool truncate = false)
    {
        auto mode = truncate ? std::ios::trunc : std::ios::app;
        m_fileStream.open(filename, std::ios::out | mode);
        if (!m_fileStream.is_open())
        {
            throw std::runtime_error("Failed to open file: " + filename);   
        }
    }

    ~FileSink() override
    {
        if (m_fileStream.is_open()) 
        {
            m_fileStream.close();
        }
    }

protected:
    void sinkLog(const details::LogMsg& msg) override
    {
        fmt::memory_buffer formattedMsg;
        this->formatMessage(msg, formattedMsg);
        m_fileStream.write(formattedMsg.data(), formattedMsg.size());
    }

    void sinkFlush() override
    {
        m_fileStream.flush();
    }

private:
    std::ofstream m_fileStream;
};

using FileSinkMT = FileSink<std::mutex>;
using FileSinkST = FileSink<NullMutex>;

}
}