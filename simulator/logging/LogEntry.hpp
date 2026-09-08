#ifndef LOG_ENTRY_HPP
#define LOG_ENTRY_HPP

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstdint>

enum class LogSeverity {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

enum class EventType {
    REQUEST_RECEIVED,
    REQUEST_COMPLETED,
    DATABASE_TIMEOUT,
    CONNECTION_ERROR,
    AUTH_FAILURE,
    HIGH_LATENCY,
    SERVICE_UNAVAILABLE,
    PAYMENT_FAILURE,
    HTTP_500,
    HTTP_503,
    HEARTBEAT,
    FAULT_INJECTED,
    SYSTEM_RECOVERED
};

struct LogEntry {
    std::string id;
    std::string timestamp;     // YYYY-MM-DD HH:MM:SS.mmm
    uint64_t epochMs;          // Milliseconds since Unix epoch
    std::string serviceName;
    LogSeverity severity;
    EventType eventType;
    std::string message;
    std::string requestId;
    std::string traceId;
    int64_t responseTimeMs;

    LogEntry();
    LogEntry(const std::string& service, LogSeverity sev, EventType ev,
             const std::string& msg, const std::string& reqId = "",
             const std::string& trId = "", int64_t respTime = 0);

    std::string severityToString() const;
    std::string eventTypeToString() const;
    static LogSeverity stringToSeverity(const std::string& str);
    static EventType stringToEventType(const std::string& str);

    std::string toString() const;
    std::string toJson() const;
    static std::string currentTimestamp(uint64_t* outEpochMs = nullptr);
};

#endif // LOG_ENTRY_HPP
