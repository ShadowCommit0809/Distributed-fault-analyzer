#ifndef LOG_PARSER_HPP
#define LOG_PARSER_HPP

#include <string>
#include <vector>
#include "../../simulator/logging/LogEntry.hpp"

class LogParser {
public:
    static bool parseLine(const std::string& line, LogEntry& outEntry);
    static bool parseJson(const std::string& jsonStr, LogEntry& outEntry);
    static std::vector<LogEntry> parseBatch(const std::vector<std::string>& lines);
};

#endif // LOG_PARSER_HPP
