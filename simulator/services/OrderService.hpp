#ifndef ORDER_SERVICE_HPP
#define ORDER_SERVICE_HPP

#include "Service.hpp"
#include "PaymentService.hpp"
#include "DatabaseService.hpp"

class OrderService : public Service {
private:
    std::shared_ptr<PaymentService> m_payment;
    std::shared_ptr<DatabaseService> m_db;

public:
    OrderService(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue,
                 std::shared_ptr<PaymentService> payment,
                 std::shared_ptr<DatabaseService> db);

    bool placeOrder(const std::string& orderId, double amount,
                    const std::string& reqId, const std::string& traceId,
                    int64_t& outLatencyMs);
};

#endif // ORDER_SERVICE_HPP
