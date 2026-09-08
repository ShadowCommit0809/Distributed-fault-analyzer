#ifndef AUTH_SERVICE_HPP
#define AUTH_SERVICE_HPP

#include "Service.hpp"

class AuthService : public Service {
public:
    AuthService(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue);
    bool authenticate(const std::string& token, const std::string& reqId,
                      const std::string& traceId, int64_t& outLatencyMs);
};

#endif // AUTH_SERVICE_HPP
