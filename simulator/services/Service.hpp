#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <string>
#include <memory>
#include <atomic>
#include <vector>
#include <functional>
#include "../logging/LogEntry.hpp"
#include "../concurrency/ThreadSafeQueue.hpp"

enum class ServiceStatus {
    HEALTHY,
    WARNING,
    CRITICAL,
    DOWN
};

enum class FaultMode {
    NONE,
    DATABASE_DOWN,
    AUTH_REJECT,
    HIGH_LATENCY,
    SERVICE_CRASH,
    CASCADING_FAIL
};

struct ServiceMetrics {
    std::string id;
    std::string name;
    std::string statusStr;
    int64_t avgResponseTimeMs;
    uint64_t totalRequests;
    uint64_t totalErrors;
    double errorRatePercent;
    std::string currentFault;
};

class Service {
protected:
    std::string m_id;
    std::string m_name;
    std::atomic<ServiceStatus> m_status{ServiceStatus::HEALTHY};
    std::atomic<FaultMode> m_currentFault{FaultMode::NONE};
    std::shared_ptr<ThreadSafeQueue<LogEntry>> m_logQueue;

    std::atomic<uint64_t> m_totalRequests{0};
    std::atomic<uint64_t> m_totalErrors{0};
    std::atomic<int64_t> m_lastResponseTimeMs{50};
    std::atomic<int64_t> m_movingAvgResponseTimeMs{50};

    std::vector<std::string> m_dependencies;

public:
    Service(const std::string& id, const std::string& name,
            std::shared_ptr<ThreadSafeQueue<LogEntry>> queue);
    virtual ~Service() = default;

    const std::string& getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    ServiceStatus getStatus() const { return m_status.load(); }
    std::string getStatusString() const;

    void setStatus(ServiceStatus status) { m_status = status; }
    void addDependency(const std::string& depServiceId) { m_dependencies.push_back(depServiceId); }
    const std::vector<std::string>& getDependencies() const { return m_dependencies; }

    virtual void injectFault(FaultMode fault);
    virtual void clearFault();
    FaultMode getCurrentFault() const { return m_currentFault.load(); }

    void emitLog(LogSeverity sev, EventType type, const std::string& msg,
                 const std::string& reqId = "", const std::string& traceId = "",
                 int64_t latency = 0);

    void recordExecution(int64_t latencyMs, bool isError);

    virtual ServiceMetrics getMetrics() const;
    virtual void reset();
};

#endif // SERVICE_HPP
