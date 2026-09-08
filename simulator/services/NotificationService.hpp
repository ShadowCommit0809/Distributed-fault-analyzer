#ifndef NOTIFICATION_SERVICE_HPP
#define NOTIFICATION_SERVICE_HPP

#include "Service.hpp"

class NotificationService : public Service {
public:
    NotificationService(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue);
    bool sendNotification(const std::string& recipient, const std::string& messageText,
                          const std::string& reqId, const std::string& traceId,
                          int64_t& outLatencyMs);
};

#endif // NOTIFICATION_SERVICE_HPP
