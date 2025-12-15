#pragma once

#include "../common.h"
#include "basesink.h"
#include "filesink.h"
#include <string>
#include <cstdio>
#include <sys/stat.h>
#include <fstream>
#include <stdexcept>

namespace minispdlog {
namespace sinks {

// rotating_file_sink: 按大小滚动的文件 Sink
// 参考 spdlog 设计:当文件大小超过限制时自动轮转
//
// 轮转策略:
//   - mylog.txt 达到 max_size 后轮转
//   - mylog.txt → mylog.1.txt (重命名)
//   - mylog.1.txt → mylog.2.txt (如果存在)
//   - 创建新的 mylog.txt
//   - 当达到 max_files 限制时,删除最旧的文件
template<typename Mutex>
class RotatingFileSink : public BaseSink<Mutex>
{
public:
    RotatingFileSink(const std::string& baseFilename, size_t maxSize, size_t maxFiles)
        : m_baseFilename(baseFilename), m_maxSize(maxSize), m_maxFiles(maxFiles), m_currentSize(0)
    {
        if(maxSize == 0 || maxFiles == 0) 
        {
            throw std::invalid_argument("maxSize and maxFiles must be greater than 0");
        }

        auto filename = calcFilename(m_baseFilename, 0);
        m_fileStream.open(filename, std::ios::app);
        if (!m_fileStream.is_open())
        {
            throw std::runtime_error("Failed to open log file: " + filename);
        }

        if(fileExists(filename))
        {
            m_currentSize = getFileSize(filename);
        }

    }

    ~RotatingFileSink() override = default;

    std::string filename() const
    {
        return calcFilename(m_baseFilename, 0);
    }

    static std::string calcFilename(const std::string& baseFilename, size_t index)
    {
        if(index == 0)
        {
            return baseFilename;
        }
        else
        {
            return baseFilename + std::to_string(index) + ".txt";
        }
    }

protected:
    void sinkLog(const details::LogMsg& msg) override
    {
        fmt::memory_buffer formattedMsg;
        this->formatMessage(msg, formattedMsg);
        size_t msgSize = formattedMsg.size();

        if(m_currentSize + msgSize > m_maxSize)
        {
            rotateFiles();
            m_currentSize = 0; // 重置当前大小
        }

        if(m_fileStream.is_open())
        {
            m_fileStream.write(formattedMsg.data(), msgSize);
            m_currentSize += msgSize;
        }
        else
        {
            throw std::runtime_error("Log file stream is not open");
        }
    }

    void sinkFlush() override
    {
        m_fileStream.flush();
    }

private:
    void rotateFiles()
    {
        if(m_fileStream.is_open())
        {
            m_fileStream.close();
        }

        // 重命名当前文件
        for(size_t i = m_maxFiles; i > 0; --i)
        {
            std::string oldName = calcFilename(m_baseFilename, i - 1);
            std::string newName = calcFilename(m_baseFilename, i);

            if(!fileExists(oldName))
            {
                continue; // 如果文件不存在,跳过
            }

            renameFile(oldName, newName);
        }

        std::string newFilename = calcFilename(m_baseFilename, 0);
        m_fileStream.open(newFilename, std::ios::app);
        if (!m_fileStream.is_open())
        {
            throw std::runtime_error("Failed to open new log file: " + newFilename);
        }
    }

    bool renameFile(const std::string& oldName, const std::string& newName)
    {
        removeFile(newName); // 确保新文件不存在
        return std::rename(oldName.c_str(), newName.c_str()) == 0;
    }

    bool removeFile(const std::string& filename)
    {
        return std::remove(filename.c_str()) == 0;
    }

    bool fileExists(const std::string& filename) const
    {
        struct stat buffer;
        return (stat(filename.c_str(), &buffer) == 0);
    }

    size_t getFileSize(const std::string& filename) const
    {
        struct stat buffer;
        if(stat(filename.c_str(), &buffer) == 0)
        {
            return buffer.st_size;
        }
        return 0; // 如果文件不存在,返回0
    }

    std::string m_baseFilename; // 基础文件名
    size_t m_maxSize;            // 最大文件大小 (字节)
    size_t m_maxFiles;           // 最大保留文件数
    size_t m_currentSize;       // 当前文件大小
    std::ofstream m_fileStream; // 当前文件流
};

using RotatingFileSinkMT = RotatingFileSink<std::mutex>;
using RotatingFileSinkST = RotatingFileSink<NullMutex>;

}
}