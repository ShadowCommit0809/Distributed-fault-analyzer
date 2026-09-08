#include "Service.hpp"

Service::Service(const std::string& id, const std::string& name,
                 std::shared_ptr<ThreadSafeQueue<LogEntry>> queue)
    : m_id(id), m_name(name), m_logQueue(queue) {}

std::string Service::getStatusString() const {
    switch (m_status.load()) {
        case ServiceStatus::HEALTHY: return "HEALTHY";
        case ServiceStatus::WARNING: return "WARNING";
        case ServiceStatus::CRITICAL: return "CRITICAL";
        case ServiceStatus::DOWN: return "DOWN";
        default: return "UNKNOWN";
    }
}

void Service::injectFault(FaultMode fault) {
    m_currentFault = fault;
    if (fault == FaultMode::NONE) {
        m_status = ServiceStatus::HEALTHY;
    } else if (fault == FaultMode::SERVICE_CRASH || fault == FaultMode::DATABASE_DOWN) {
        m_status = ServiceStatus::DOWN;
    } else if (fault == FaultMode::HIGH_LATENCY) {
        m_status = ServiceStatus::WARNING;
    } else {
        m_status = ServiceStatus::CRITICAL;
    }

    emitLog(LogSeverity::WARNING, EventType::FAULT_INJECTED,
            "Injected fault mode: " + std::to_string(static_cast<int>(fault)));
}

void Service::clearFault() {
    m_currentFault = FaultMode::NONE;
    m_status = ServiceStatus::HEALTHY;
    emitLog(LogSeverity::INFO, EventType::SYSTEM_RECOVERED,
            "Service recovered to normal operation");
}

void Service::emitLog(LogSeverity sev, EventType type, const std::string& msg,
                      const std::string& reqId, const std::string& traceId,
                      int64_t latency) {
    if (m_logQueue) {
        LogEntry entry(m_name, sev, type, msg, reqId, traceId, latency);
        m_logQueue->push(std::move(entry));
    }
}

void Service::recordExecution(int64_t latencyMs, bool isError) {
    m_totalRequests++;
    if (isError) {
        m_totalErrors++;
    }
    m_lastResponseTimeMs = latencyMs;

    // Moving average: newAvg = (prevAvg * 7 + latency) / 8
    int64_t prev = m_movingAvgResponseTimeMs.load();
    int64_t next = (prev * 7 + latencyMs) / 8;
    m_movingAvgResponseTimeMs.store(next);
}

ServiceMetrics Service::getMetrics() const {
    ServiceMetrics m;
    m.id = m_id;
    m.name = m_name;
    m.statusStr = getStatusString();
    m.avgResponseTimeMs = m_movingAvgResponseTimeMs.load();
    m.totalRequests = m_totalRequests.load();
    m.totalErrors = m_totalErrors.load();
    m.errorRatePercent = (m.totalRequests > 0)
        ? (static_cast<double>(m.totalErrors) / static_cast<double>(m.totalRequests)) * 100.0
        : 0.0;
    
    switch (m_currentFault.load()) {
        case FaultMode::DATABASE_DOWN: m.currentFault = "DATABASE_DOWN"; break;
        case FaultMode::AUTH_REJECT: m.currentFault = "AUTH_REJECT"; break;
        case FaultMode::HIGH_LATENCY: m.currentFault = "HIGH_LATENCY"; break;
        case FaultMode::SERVICE_CRASH: m.currentFault = "SERVICE_CRASH"; break;
        case FaultMode::CASCADING_FAIL: m.currentFault = "CASCADING_FAIL"; break;
        default: m.currentFault = "NONE"; break;
    }
    return m;
}

void Service::reset() {
    m_currentFault = FaultMode::NONE;
    m_status = ServiceStatus::HEALTHY;
    m_totalRequests = 0;
    m_totalErrors = 0;
    m_lastResponseTimeMs = 45;
    m_movingAvgResponseTimeMs = 45;
}
