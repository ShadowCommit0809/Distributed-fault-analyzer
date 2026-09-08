#include "APIGateway.hpp"

APIGateway::APIGateway(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue,
                       std::shared_ptr<AuthService> auth,
                       std::shared_ptr<OrderService> order,
                       std::shared_ptr<NotificationService> notif)
    : Service("SRV-GATEWAY", "API Gateway", queue),
      m_auth(auth), m_order(order), m_notif(notif) {
    addDependency("SRV-AUTH");
    addDependency("SRV-ORDER");
}

GatewayResponse APIGateway::handleOrderRequest(const std::string& authToken,
                                              const std::string& orderId,
                                              double amount,
                                              const std::string& customerEmail,
                                              const std::string& reqId,
                                              const std::string& traceId) {
    GatewayResponse resp;
    resp.httpStatusCode = 200;
    resp.success = true;

    emitLog(LogSeverity::INFO, EventType::REQUEST_RECEIVED,
            "Inbound POST /api/v1/orders received for " + orderId,
            reqId, traceId, 0);

    FaultMode fault = m_currentFault.load();
    if (fault == FaultMode::SERVICE_CRASH) {
        resp.httpStatusCode = 503;
        resp.success = false;
        resp.responseBody = "{\"error\": \"API Gateway Service Unavailable\"}";
        resp.totalLatencyMs = 50;
        recordExecution(resp.totalLatencyMs, true);
        emitLog(LogSeverity::CRITICAL, EventType::HTTP_503,
                "HTTP 503 Service Unavailable: Gateway worker pool crashed",
                reqId, traceId, resp.totalLatencyMs);
        return resp;
    }

    // Step 1: Validate Auth
    int64_t authLatency = 0;
    if (m_auth && !m_auth->authenticate(authToken, reqId, traceId, authLatency)) {
        resp.httpStatusCode = 401;
        resp.success = false;
        resp.responseBody = "{\"error\": \"Unauthorized: Invalid or rejected auth token\"}";
        resp.totalLatencyMs = authLatency + 10;
        recordExecution(resp.totalLatencyMs, true);
        emitLog(LogSeverity::ERROR, EventType::HTTP_500,
                "Request rejected: Authentication failed, returning HTTP 401/500",
                reqId, traceId, resp.totalLatencyMs);
        return resp;
    }

    // Step 2: Forward to OrderService
    int64_t orderLatency = 0;
    if (m_order && !m_order->placeOrder(orderId, amount, reqId, traceId, orderLatency)) {
        resp.httpStatusCode = 500;
        resp.success = false;
        resp.responseBody = "{\"error\": \"Internal Server Error: Order placement downstream failure\"}";
        resp.totalLatencyMs = authLatency + orderLatency + 20;
        recordExecution(resp.totalLatencyMs, true);
        emitLog(LogSeverity::ERROR, EventType::HTTP_500,
                "HTTP 500 Internal Server Error: Downstream service failure encountered during order processing",
                reqId, traceId, resp.totalLatencyMs);
        return resp;
    }

    // Step 3: Trigger async notification
    int64_t notifLatency = 0;
    if (m_notif) {
        m_notif->sendNotification(customerEmail, "Order " + orderId + " confirmed!", reqId, traceId, notifLatency);
    }

    resp.totalLatencyMs = authLatency + orderLatency + notifLatency + 15;
    recordExecution(resp.totalLatencyMs, false);
    resp.responseBody = "{\"status\": \"CONFIRMED\", \"orderId\": \"" + orderId + "\"}";

    emitLog(LogSeverity::INFO, EventType::REQUEST_COMPLETED,
            "HTTP 200 OK: Order " + orderId + " processed successfully in " + std::to_string(resp.totalLatencyMs) + "ms",
            reqId, traceId, resp.totalLatencyMs);
    return resp;
}
