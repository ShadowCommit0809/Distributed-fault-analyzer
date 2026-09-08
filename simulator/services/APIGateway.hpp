#ifndef API_GATEWAY_HPP
#define API_GATEWAY_HPP

#include "Service.hpp"
#include "AuthService.hpp"
#include "OrderService.hpp"
#include "NotificationService.hpp"

struct GatewayResponse {
    int httpStatusCode;
    std::string responseBody;
    int64_t totalLatencyMs;
    bool success;
};

class APIGateway : public Service {
private:
    std::shared_ptr<AuthService> m_auth;
    std::shared_ptr<OrderService> m_order;
    std::shared_ptr<NotificationService> m_notif;

public:
    APIGateway(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue,
               std::shared_ptr<AuthService> auth,
               std::shared_ptr<OrderService> order,
               std::shared_ptr<NotificationService> notif);

    GatewayResponse handleOrderRequest(const std::string& authToken,
                                      const std::string& orderId,
                                      double amount,
                                      const std::string& customerEmail,
                                      const std::string& reqId,
                                      const std::string& traceId);
};

#endif // API_GATEWAY_HPP
