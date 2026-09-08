#include "LogProcessor.hpp"

LogProcessor::LogProcessor(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue,
                           std::shared_ptr<FaultDetector> detector,
                           size_t maxStoredLogs)
    : m_queue(queue), m_detector(detector), m_maxStoredLogs(maxStoredLogs) {}

LogProcessor::~LogProcessor() {
    stop();
}

void LogProcessor::start() {
    if (m_running.load()) return;
    m_running = true;
    m_workerThread = std::thread(&LogProcessor::processLoop, this);
}

void LogProcessor::stop() {
    if (!m_running.load()) return;
    m_running = false;
    if (m_queue) {
        m_queue->shutdown();
    }
    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
}

void LogProcessor::processLoop() {
    while (m_running.load()) {
        if (!m_queue) break;

        std::vector<LogEntry> batch = m_queue->popBatch(50, std::chrono::milliseconds(100));
        if (batch.empty()) {
            continue;
        }

        m_totalProcessed += batch.size();

        // 1. Store into bounded ring buffer
        {
            std::lock_guard<std::mutex> lock(m_logStoreMutex);
            for (const auto& log : batch) {
                if (m_recentLogs.size() >= m_maxStoredLogs) {
                    m_recentLogs.pop_front();
                }
                m_recentLogs.push_back(log);
            }
        }

        // 2. Feed batch into FaultDetector
        if (m_detector) {
            m_detector->processLogs(batch);
        }
    }
}

std::vector<LogEntry> LogProcessor::getRecentLogs(size_t limit,
                                                 const std::string& serviceFilter,
                                                 const std::string& severityFilter) const {
    std::lock_guard<std::mutex> lock(m_logStoreMutex);
    std::vector<LogEntry> result;
    result.reserve(limit);

    // Read from most recent backwards
    for (auto it = m_recentLogs.rbegin(); it != m_recentLogs.rend() && result.size() < limit; ++it) {
        if (!serviceFilter.empty() && it->serviceName != serviceFilter) {
            continue;
        }
        if (!severityFilter.empty() && it->severityToString() != severityFilter) {
            continue;
        }
        result.push_back(*it);
    }

    return result;
}

void LogProcessor::clearLogs() {
    std::lock_guard<std::mutex> lock(m_logStoreMutex);
    m_recentLogs.clear();
}
