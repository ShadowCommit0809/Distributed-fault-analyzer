#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

template <typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex m_mutex;
    std::queue<T> m_queue;
    std::condition_variable m_cv;
    bool m_shutdownRequested{false};
    size_t m_maxCapacity{50000};

public:
    explicit ThreadSafeQueue(size_t maxCapacity = 50000)
        : m_maxCapacity(maxCapacity) {}

    ~ThreadSafeQueue() {
        shutdown();
    }

    // Push item into queue, notify one waiting consumer
    void push(T item) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_queue.size() >= m_maxCapacity) {
                // Drop oldest if at max capacity to avoid unbound memory growth
                m_queue.pop();
            }
            m_queue.push(std::move(item));
        }
        m_cv.notify_one();
    }

    // Try pop non-blocking
    bool tryPop(T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return false;
        }
        value = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    // Blocking pop with optional timeout
    bool waitAndPop(T& value, std::chrono::milliseconds timeout = std::chrono::milliseconds(200)) {
        std::unique_lock<std::mutex> lock(m_mutex);
        bool waitSuccess = m_cv.wait_for(lock, timeout, [this] {
            return !m_queue.empty() || m_shutdownRequested;
        });

        if (!waitSuccess || m_queue.empty()) {
            return false;
        }

        value = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    // Batch pop for high-throughput consumer processing
    std::vector<T> popBatch(size_t maxBatch = 100, std::chrono::milliseconds timeout = std::chrono::milliseconds(100)) {
        std::vector<T> batch;
        batch.reserve(maxBatch);

        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait_for(lock, timeout, [this] {
            return !m_queue.empty() || m_shutdownRequested;
        });

        while (!m_queue.empty() && batch.size() < maxBatch) {
            batch.push_back(std::move(m_queue.front()));
            m_queue.pop();
        }

        return batch;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (!m_queue.empty()) {
            m_queue.pop();
        }
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_shutdownRequested = true;
        }
        m_cv.notify_all();
    }

    bool isShutdown() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_shutdownRequested;
    }
};

#endif // THREAD_SAFE_QUEUE_HPP
