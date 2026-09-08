#include "LogEntry.hpp"
#include <atomic>
#include <ctime>

static std::atomic<uint64_t> g_logCounter{1000};

std::string LogEntry::currentTimestamp(uint64_t* outEpochMs) {
    auto now = std::chrono::system_clock::now();
    auto epochDuration = now.time_since_epoch();
    uint64_t millis = std::chrono::duration_cast<std::chrono::milliseconds>(epochDuration).count();
    if (outEpochMs) {
        *outEpochMs = millis;
    }

    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tmVal;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tmVal, &tt);
#else
    localtime_r(&tt, &tmVal);
#endif

    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tmVal);
    int msPart = static_cast<int>(millis % 1000);
    char finalBuf[80];
    snprintf(finalBuf, sizeof(finalBuf), "%s.%03d", buffer, msPart);
    return std::string(finalBuf);
}

LogEntry::LogEntry() 
    : id("LOG" + std::to_string(++g_logCounter)),
      epochMs(0),
      severity(LogSeverity::INFO),
      eventType(EventType::REQUEST_RECEIVED),
      responseTimeMs(0) {
    timestamp = currentTimestamp(&epochMs);
}

LogEntry::LogEntry(const std::string& service, LogSeverity sev, EventType ev,
                   const std::string& msg, const std::string& reqId,
                   const std::string& trId, int64_t respTime)
    : id("LOG" + std::to_string(++g_logCounter)),
      serviceName(service),
      severity(sev),
      eventType(ev),
      message(msg),
      requestId(reqId),
      traceId(trId),
      responseTimeMs(respTime) {
    timestamp = currentTimestamp(&epochMs);
}

std::string LogEntry::severityToString() const {
    switch (severity) {
        case LogSeverity::INFO: return "INFO";
        case LogSeverity::WARNING: return "WARNING";
        case LogSeverity::ERROR: return "ERROR";
        case LogSeverity::CRITICAL: return "CRITICAL";
        default: return "INFO";
    }
}

std::string LogEntry::eventTypeToString() const {
    switch (eventType) {
        case EventType::REQUEST_RECEIVED: return "REQUEST_RECEIVED";
        case EventType::REQUEST_COMPLETED: return "REQUEST_COMPLETED";
        case EventType::DATABASE_TIMEOUT: return "DATABASE_TIMEOUT";
        case EventType::CONNECTION_ERROR: return "CONNECTION_ERROR";
        case EventType::AUTH_FAILURE: return "AUTH_FAILURE";
        case EventType::HIGH_LATENCY: return "HIGH_LATENCY";
        case EventType::SERVICE_UNAVAILABLE: return "SERVICE_UNAVAILABLE";
        case EventType::PAYMENT_FAILURE: return "PAYMENT_FAILURE";
        case EventType::HTTP_500: return "HTTP_500";
        case EventType::HTTP_503: return "HTTP_503";
        case EventType::HEARTBEAT: return "HEARTBEAT";
        case EventType::FAULT_INJECTED: return "FAULT_INJECTED";
        case EventType::SYSTEM_RECOVERED: return "SYSTEM_RECOVERED";
        default: return "UNKNOWN";
    }
}

LogSeverity LogEntry::stringToSeverity(const std::string& str) {
    if (str == "WARNING") return LogSeverity::WARNING;
    if (str == "ERROR") return LogSeverity::ERROR;
    if (str == "CRITICAL") return LogSeverity::CRITICAL;
    return LogSeverity::INFO;
}

EventType LogEntry::stringToEventType(const std::string& str) {
    if (str == "REQUEST_COMPLETED") return EventType::REQUEST_COMPLETED;
    if (str == "DATABASE_TIMEOUT") return EventType::DATABASE_TIMEOUT;
    if (str == "CONNECTION_ERROR") return EventType::CONNECTION_ERROR;
    if (str == "AUTH_FAILURE") return EventType::AUTH_FAILURE;
    if (str == "HIGH_LATENCY") return EventType::HIGH_LATENCY;
    if (str == "SERVICE_UNAVAILABLE") return EventType::SERVICE_UNAVAILABLE;
    if (str == "PAYMENT_FAILURE") return EventType::PAYMENT_FAILURE;
    if (str == "HTTP_500") return EventType::HTTP_500;
    if (str == "HTTP_503") return EventType::HTTP_503;
    if (str == "HEARTBEAT") return EventType::HEARTBEAT;
    if (str == "FAULT_INJECTED") return EventType::FAULT_INJECTED;
    if (str == "SYSTEM_RECOVERED") return EventType::SYSTEM_RECOVERED;
    return EventType::REQUEST_RECEIVED;
}

std::string LogEntry::toString() const {
    std::ostringstream oss;
    oss << "[" << timestamp << "] "
        << "[" << severityToString() << "] "
        << "[" << serviceName << "] "
        << "[" << eventTypeToString() << "] "
        << "reqId=" << (requestId.empty() ? "-" : requestId) << " "
        << "traceId=" << (traceId.empty() ? "-" : traceId) << " "
        << "latency=" << responseTimeMs << "ms - "
        << message;
    return oss.str();
}

static std::string escapeJson(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 10);
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\b') out += "\\b";
        else if (c == '\f') out += "\\f";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

std::string LogEntry::toJson() const {
    std::ostringstream oss;
    oss << "{"
        << "\"id\":\"" << id << "\","
        << "\"timestamp\":\"" << escapeJson(timestamp) << "\","
        << "\"epochMs\":" << epochMs << ","
        << "\"serviceName\":\"" << escapeJson(serviceName) << "\","
        << "\"severity\":\"" << severityToString() << "\","
        << "\"eventType\":\"" << eventTypeToString() << "\","
        << "\"message\":\"" << escapeJson(message) << "\","
        << "\"requestId\":\"" << escapeJson(requestId) << "\","
        << "\"traceId\":\"" << escapeJson(traceId) << "\","
        << "\"responseTimeMs\":" << responseTimeMs
        << "}";
    return oss.str();
}
