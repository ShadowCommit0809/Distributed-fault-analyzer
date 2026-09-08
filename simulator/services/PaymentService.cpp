#include "PaymentService.hpp"

PaymentService::PaymentService(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue,
                               std::shared_ptr<DatabaseService> db)
    : Service("SRV-PAYMENT", "Payment Service", queue), m_db(db) {
    addDependency("SRV-DB");
}

bool PaymentService::processPayment(double amount, const std::string& reqId,
                                    const std::string& traceId, int64_t& outLatencyMs) {
    FaultMode fault = m_currentFault.load();

    if (fault == FaultMode::SERVICE_CRASH) {
        outLatencyMs = 3000;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::CRITICAL, EventType::SERVICE_UNAVAILABLE,
                "Payment gateway crashed: Connection refused by internal payment worker",
                reqId, traceId, outLatencyMs);
        return false;
    }

    if (fault == FaultMode::HIGH_LATENCY) {
        outLatencyMs = 3200;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::WARNING, EventType::HIGH_LATENCY,
                "Payment gateway response delayed: 3200ms processing card auth",
                reqId, traceId, outLatencyMs);
        return false;
    }

    // Call downstream DatabaseService to record ledger transaction
    int64_t dbLatency = 0;
    if (m_db && !m_db->executeQuery("INSERT INTO transactions(amount) VALUES(" + std::to_string(amount) + ")",
                                     reqId, traceId, dbLatency)) {
        outLatencyMs = dbLatency + 150;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::ERROR, EventType::PAYMENT_FAILURE,
                "Payment failed: Database ledger transaction timed out after " + std::to_string(outLatencyMs) + "ms",
                reqId, traceId, outLatencyMs);
        return false;
    }

    outLatencyMs = 80 + dbLatency;
    recordExecution(outLatencyMs, false);
    emitLog(LogSeverity::INFO, EventType::REQUEST_COMPLETED,
            "Payment processed successfully for $" + std::to_string(amount),
            reqId, traceId, outLatencyMs);
    return true;
}
