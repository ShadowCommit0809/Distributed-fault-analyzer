#include "LogParser.hpp"
#include <sstream>
#include <regex>

static std::string extractJsonField(const std::string& json, const std::string& field) {
    std::string key = "\"" + field + "\":\"";
    size_t pos = json.find(key);
    if (pos != std::string::npos) {
        size_t start = pos + key.length();
        size_t end = json.find("\"", start);
        if (end != std::string::npos) {
            return json.substr(start, end - start);
        }
    }

    // Number field without quotes
    key = "\"" + field + "\":";
    pos = json.find(key);
    if (pos != std::string::npos) {
        size_t start = pos + key.length();
        while (start < json.length() && (json[start] == ' ' || json[start] == '\t')) start++;
        size_t end = json.find_first_of(",}", start);
        if (end != std::string::npos) {
            return json.substr(start, end - start);
        }
    }
    return "";
}

bool LogParser::parseJson(const std::string& jsonStr, LogEntry& outEntry) {
    if (jsonStr.empty() || jsonStr.front() != '{' || jsonStr.back() != '}') {
        return false;
    }

    outEntry.id = extractJsonField(jsonStr, "id");
    outEntry.timestamp = extractJsonField(jsonStr, "timestamp");
    std::string epochStr = extractJsonField(jsonStr, "epochMs");
    if (!epochStr.empty()) {
        try { outEntry.epochMs = std::stoull(epochStr); } catch (...) {}
    }
    outEntry.serviceName = extractJsonField(jsonStr, "serviceName");
    outEntry.severity = LogEntry::stringToSeverity(extractJsonField(jsonStr, "severity"));
    outEntry.eventType = LogEntry::stringToEventType(extractJsonField(jsonStr, "eventType"));
    outEntry.message = extractJsonField(jsonStr, "message");
    outEntry.requestId = extractJsonField(jsonStr, "requestId");
    outEntry.traceId = extractJsonField(jsonStr, "traceId");
    std::string respStr = extractJsonField(jsonStr, "responseTimeMs");
    if (!respStr.empty()) {
        try { outEntry.responseTimeMs = std::stoll(respStr); } catch (...) {}
    }
    return !outEntry.serviceName.empty();
}

bool LogParser::parseLine(const std::string& line, LogEntry& outEntry) {
    if (line.empty()) return false;
    if (line.front() == '{' && line.back() == '}') {
        return parseJson(line, outEntry);
    }

    // Standard string format: [timestamp] [SEVERITY] [serviceName] [EVENT] reqId=X traceId=Y latency=Zms - message
    outEntry.message = line;
    return true;
}

std::vector<LogEntry> LogParser::parseBatch(const std::vector<std::string>& lines) {
    std::vector<LogEntry> result;
    result.reserve(lines.size());
    for (const auto& line : lines) {
        LogEntry entry;
        if (parseLine(line, entry)) {
            result.push_back(std::move(entry));
        }
    }
    return result;
}
