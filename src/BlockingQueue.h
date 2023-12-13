#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>
#include <optional>

template <typename T>
class BlockingQueue
{
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool isProducerDone{false};

public:
    void push(T item)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(item);
        m_cond.notify_all();
    };

    std::optional<T> pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty() && !isProducerDone)
        {
            m_cond.wait(lock);
        }

        if (!m_queue.empty())
        {
            T item = m_queue.front();
            m_queue.pop();
            return item;
        }
        return std::nullopt;
    };

    bool empty()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    };

    void setDone()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        isProducerDone = true;
        m_cond.notify_all();
    }
};
