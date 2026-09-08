#include "NotificationService.hpp"

NotificationService::NotificationService(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue)
    : Service("SRV-NOTIF", "Notification Service", queue) {
    addDependency("SRV-ORDER");
}

bool NotificationService::sendNotification(const std::string& recipient, const std::string& messageText,
                                          const std::string& reqId, const std::string& traceId,
                                          int64_t& outLatencyMs) {
    FaultMode fault = m_currentFault.load();

    if (fault == FaultMode::SERVICE_CRASH) {
        outLatencyMs = 1200;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::CRITICAL, EventType::SERVICE_UNAVAILABLE,
                "Notification Service crashed: SMTP connection refused for " + recipient,
                reqId, traceId, outLatencyMs);
        return false;
    }

    if (fault == FaultMode::HIGH_LATENCY) {
        outLatencyMs = 2600;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::WARNING, EventType::HIGH_LATENCY,
                "Notification gateway slow: SMS queue backlogged 2600ms",
                reqId, traceId, outLatencyMs);
        return false;
    }

    outLatencyMs = 40;
    recordExecution(outLatencyMs, false);
    emitLog(LogSeverity::INFO, EventType::REQUEST_COMPLETED,
            "Notification successfully dispatched to " + recipient + ": " + messageText,
            reqId, traceId, outLatencyMs);
    return true;
}
