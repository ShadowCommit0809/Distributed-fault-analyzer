#include "AuthService.hpp"

AuthService::AuthService(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue)
    : Service("SRV-AUTH", "Authentication Service", queue) {}

bool AuthService::authenticate(const std::string& token, const std::string& reqId,
                               const std::string& traceId, int64_t& outLatencyMs) {
    FaultMode fault = m_currentFault.load();

    if (fault == FaultMode::AUTH_REJECT) {
        outLatencyMs = 60;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::ERROR, EventType::AUTH_FAILURE,
                "Authentication rejected: Signature verification failed for token " + token,
                reqId, traceId, outLatencyMs);
        return false;
    }

    if (fault == FaultMode::SERVICE_CRASH) {
        outLatencyMs = 2500;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::CRITICAL, EventType::SERVICE_UNAVAILABLE,
                "Authentication Service unavailable: Keystore daemon unresponsive",
                reqId, traceId, outLatencyMs);
        return false;
    }

    if (fault == FaultMode::HIGH_LATENCY) {
        outLatencyMs = 2000;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::WARNING, EventType::HIGH_LATENCY,
                "Authentication Service high latency: RSA key verification delayed 2000ms",
                reqId, traceId, outLatencyMs);
        return false;
    }

    outLatencyMs = 15;
    recordExecution(outLatencyMs, false);
    emitLog(LogSeverity::INFO, EventType::REQUEST_COMPLETED,
            "Authentication successful for token " + token,
            reqId, traceId, outLatencyMs);
    return true;
}
