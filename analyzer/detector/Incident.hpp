#ifndef INCIDENT_HPP
#define INCIDENT_HPP

#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include "../../simulator/logging/LogEntry.hpp"

struct IncidentTimelineEvent {
    std::string timestamp;
    std::string serviceName;
    std::string description;
    std::string eventType;
    std::string severity;
    int64_t relativeOffsetMs; // offset from first failure event
    std::string traceId;

    std::string toJson() const {
        std::ostringstream oss;
        oss << "{"
            << "\"timestamp\":\"" << timestamp << "\","
            << "\"serviceName\":\"" << serviceName << "\","
            << "\"description\":\"" << description << "\","
            << "\"eventType\":\"" << eventType << "\","
            << "\"severity\":\"" << severity << "\","
            << "\"relativeOffsetMs\":" << relativeOffsetMs << ","
            << "\"traceId\":\"" << traceId << "\""
            << "}";
        return oss.str();
    }
};

struct Incident {
    std::string id;
    std::string timestamp;
    uint64_t epochMs;
    std::string severity;       // WARNING, ERROR, CRITICAL
    std::string status;         // ACTIVE, RESOLVED
    std::string summary;
    std::string probableRootCause;
    double confidence;          // 0.0 to 100.0
    std::vector<std::string> affectedServices;
    std::vector<IncidentTimelineEvent> timeline;
    std::vector<std::string> evidence;
    std::vector<LogEntry> correlatedLogs;
    std::string conclusion;

    Incident() : epochMs(0), confidence(0.0) {}

    std::string toJson() const {
        std::ostringstream oss;
        oss << "{"
            << "\"id\":\"" << id << "\","
            << "\"timestamp\":\"" << timestamp << "\","
            << "\"epochMs\":" << epochMs << ","
            << "\"severity\":\"" << severity << "\","
            << "\"status\":\"" << status << "\","
            << "\"summary\":\"" << summary << "\","
            << "\"probableRootCause\":\"" << probableRootCause << "\","
            << "\"confidence\":" << confidence << ","
            << "\"conclusion\":\"" << conclusion << "\",";

        // affectedServices
        oss << "\"affectedServices\":[";
        for (size_t i = 0; i < affectedServices.size(); ++i) {
            oss << "\"" << affectedServices[i] << "\"";
            if (i + 1 < affectedServices.size()) oss << ",";
        }
        oss << "],";

        // evidence
        oss << "\"evidence\":[";
        for (size_t i = 0; i < evidence.size(); ++i) {
            oss << "\"" << evidence[i] << "\"";
            if (i + 1 < evidence.size()) oss << ",";
        }
        oss << "],";

        // timeline
        oss << "\"timeline\":[";
        for (size_t i = 0; i < timeline.size(); ++i) {
            oss << timeline[i].toJson();
            if (i + 1 < timeline.size()) oss << ",";
        }
        oss << "],";

        // correlatedLogs
        oss << "\"correlatedLogs\":[";
        for (size_t i = 0; i < correlatedLogs.size() && i < 50; ++i) {
            oss << correlatedLogs[i].toJson();
            if (i + 1 < correlatedLogs.size() && i + 1 < 50) oss << ",";
        }
        oss << "]";

        oss << "}";
        return oss.str();
    }
};

#endif // INCIDENT_HPP
