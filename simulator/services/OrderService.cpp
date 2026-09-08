#include "OrderService.hpp"

OrderService::OrderService(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue,
                           std::shared_ptr<PaymentService> payment,
                           std::shared_ptr<DatabaseService> db)
    : Service("SRV-ORDER", "Order Service", queue), m_payment(payment), m_db(db) {
    addDependency("SRV-DB");
    addDependency("SRV-PAYMENT");
}

bool OrderService::placeOrder(const std::string& orderId, double amount,
                              const std::string& reqId, const std::string& traceId,
                              int64_t& outLatencyMs) {
    emitLog(LogSeverity::INFO, EventType::REQUEST_RECEIVED,
            "Received place order request for " + orderId + " ($" + std::to_string(amount) + ")",
            reqId, traceId, 0);

    FaultMode fault = m_currentFault.load();
    if (fault == FaultMode::HIGH_LATENCY) {
        outLatencyMs = 3500;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::WARNING, EventType::HIGH_LATENCY,
                "Order processing thread backlog: 3500ms latency on order " + orderId,
                reqId, traceId, outLatencyMs);
        return false;
    }

    if (fault == FaultMode::SERVICE_CRASH) {
        outLatencyMs = 1500;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::CRITICAL, EventType::SERVICE_UNAVAILABLE,
                "OrderService internal thread crashed: Null pointer in OrderDispatcher",
                reqId, traceId, outLatencyMs);
        return false;
    }

    // Step 1: Check database for inventory/order state
    int64_t dbLatency = 0;
    if (m_db && !m_db->executeQuery("SELECT * FROM inventory WHERE order_id='" + orderId + "'",
                                     reqId, traceId, dbLatency)) {
        outLatencyMs = dbLatency + 50;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::ERROR, EventType::CONNECTION_ERROR,
                "Order Service failed: Database request failed during inventory validation",
                reqId, traceId, outLatencyMs);
        return false;
    }

    // Step 2: Process payment
    int64_t paymentLatency = 0;
    if (m_payment && !m_payment->processPayment(amount, reqId, traceId, paymentLatency)) {
        outLatencyMs = dbLatency + paymentLatency + 60;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::ERROR, EventType::PAYMENT_FAILURE,
                "Order Service failed: Downstream PaymentService failed to fulfill transaction",
                reqId, traceId, outLatencyMs);
        return false;
    }

    // Step 3: Finalize order in database
    int64_t finalDbLatency = 0;
    if (m_db && !m_db->executeQuery("UPDATE orders SET status='CONFIRMED' WHERE id='" + orderId + "'",
                                     reqId, traceId, finalDbLatency)) {
        outLatencyMs = dbLatency + paymentLatency + finalDbLatency + 50;
        recordExecution(outLatencyMs, true);
        emitLog(LogSeverity::ERROR, EventType::DATABASE_TIMEOUT,
                "Order Service failed: Final state update timed out in DatabaseService",
                reqId, traceId, outLatencyMs);
        return false;
    }

    outLatencyMs = dbLatency + paymentLatency + finalDbLatency + 40;
    recordExecution(outLatencyMs, false);
    emitLog(LogSeverity::INFO, EventType::REQUEST_COMPLETED,
            "Order " + orderId + " placed successfully in " + std::to_string(outLatencyMs) + "ms",
            reqId, traceId, outLatencyMs);
    return true;
}
