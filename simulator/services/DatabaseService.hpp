#ifndef DATABASE_SERVICE_HPP
#define DATABASE_SERVICE_HPP

#include "Service.hpp"

class DatabaseService : public Service {
private:
    std::atomic<bool> m_isAvailable{true};

public:
    DatabaseService(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue);
    bool executeQuery(const std::string& query, const std::string& reqId,
                      const std::string& traceId, int64_t& outLatencyMs);
};

#endif // DATABASE_SERVICE_HPP
