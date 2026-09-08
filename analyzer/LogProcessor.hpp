#ifndef LOG_PROCESSOR_HPP
#define LOG_PROCESSOR_HPP

#include <memory>
#include <thread>
#include <atomic>
#include <vector>
#include <deque>
#include <mutex>
#include "../simulator/concurrency/ThreadSafeQueue.hpp"
#include "../simulator/logging/LogEntry.hpp"
#include "detector/FaultDetector.hpp"

class LogProcessor {
private:
    std::shared_ptr<ThreadSafeQueue<LogEntry>> m_queue;
    std::shared_ptr<FaultDetector> m_detector;

    std::atomic<bool> m_running{false};
    std::thread m_workerThread;

    mutable std::mutex m_logStoreMutex;
    std::deque<LogEntry> m_recentLogs;
    size_t m_maxStoredLogs{2000};

    std::atomic<uint64_t> m_totalProcessed{0};

public:
    LogProcessor(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue,
                 std::shared_ptr<FaultDetector> detector,
                 size_t maxStoredLogs = 2000);
    ~LogProcessor();

    void start();
    void stop();

    uint64_t getTotalProcessed() const { return m_totalProcessed.load(); }

    std::vector<LogEntry> getRecentLogs(size_t limit = 100,
                                       const std::string& serviceFilter = "",
                                       const std::string& severityFilter = "") const;

    void clearLogs();

private:
    void processLoop();
};

#endif // LOG_PROCESSOR_HPP
