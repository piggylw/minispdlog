
#include <vector>
#include <cassert>

namespace minispdlog {
namespace details {

template <typename T>
class CircularQueue
{
public:
    explicit CircularQueue(size_t capacity)
        : m_data(capacity + 1), m_head(0), m_tail(0), m_capacity(capacity + 1), overrunCount(0) {}

    CircularQueue(const CircularQueue&) = delete;
    CircularQueue& operator=(const CircularQueue&) = delete;

    void pushBack(T&& item)
    {
        m_data[m_tail] = std::move(item);
        m_tail = (m_tail + 1) % m_capacity;
        if (m_tail == m_head)
        {
            m_head = (m_head + 1) % m_capacity; //溢出，覆盖头部
            ++overrunCount; //记录溢出次数
        }
    }

    const T& front() const
    {
        assert(!empty());
        return m_data[m_head];
    }

    T& front()
    {
        assert(!empty());
        return m_data[m_head];
    }

    void popFront()
    {
        assert(!empty());
        m_head = (m_head + 1) % m_capacity;
    }

    bool empty() const
    {
        return m_head == m_tail;
    }

    bool full() const
    {
        return (m_tail + 1) % m_capacity == m_head;
    }

    size_t size() const
    {
        return (m_tail + m_capacity - m_head) % m_capacity;
    }

    size_t capacity() const
    {
        return m_capacity - 1; //实际容量比m_capacity小1
    }

    size_t overrunCountValue() const
    {
        return overrunCount;
    }

private:
    std::vector<T> m_data;
    size_t m_head;
    size_t m_tail;
    size_t m_capacity;
    size_t overrunCount;    //溢出计数
};

}
}
