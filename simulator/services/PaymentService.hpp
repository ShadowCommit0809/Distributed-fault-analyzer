#ifndef PAYMENT_SERVICE_HPP
#define PAYMENT_SERVICE_HPP

#include "Service.hpp"
#include "DatabaseService.hpp"

class PaymentService : public Service {
private:
    std::shared_ptr<DatabaseService> m_db;

public:
    PaymentService(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue,
                   std::shared_ptr<DatabaseService> db);
    bool processPayment(double amount, const std::string& reqId,
                        const std::string& traceId, int64_t& outLatencyMs);
};

#endif // PAYMENT_SERVICE_HPP
