#include "DatabaseService.hpp"
#include <thread>
#include <chrono>

DatabaseService::DatabaseService(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue)
    : Service("SRV-DB", "Database Service", queue) {}

bool DatabaseService::executeQuery(const std::string& query, const std::string& reqId,
                                  const std::string& traceId, int64_t& outLatencyMs) {
    FaultMode fault = m_currentFault.load();

    if (fault == FaultMode::DATABASE_DOWN || fault == FaultMode::SERVICE_CRASH) {
        outLatencyMs = 3000;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::CRITICAL, EventType::DATABASE_TIMEOUT,
                "Database connection timeout after 3000ms: pool exhausted for query: " + query,
                reqId, traceId, outLatencyMs);
        return false;
    }

    if (fault == FaultMode::HIGH_LATENCY) {
        outLatencyMs = 2800;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::WARNING, EventType::HIGH_LATENCY,
                "Database lock wait timeout approaching threshold: 2800ms for query: " + query,
                reqId, traceId, outLatencyMs);
        return false;
    }

    outLatencyMs = 25; // normal fast execution
    recordExecution(outLatencyMs, false);
    emitLog(LogSeverity::INFO, EventType::REQUEST_COMPLETED,
            "Database query executed successfully in 25ms: " + query,
            reqId, traceId, outLatencyMs);
    return true;
}
