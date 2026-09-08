#ifndef ROOT_CAUSE_RESULT_HPP
#define ROOT_CAUSE_RESULT_HPP

#include <string>
#include <vector>
#include <sstream>
#include <cstdint>

struct ServiceScoreBreakdown {
    std::string serviceName;
    double totalScore;           // 0.0 to 100.0
    double temporalPriorityScore;// 0.0 to 100.0 (30% weight)
    double dependencyImpactScore;// 0.0 to 100.0 (25% weight)
    double errorFrequencyScore;  // 0.0 to 100.0 (20% weight)
    double severityScore;        // 0.0 to 100.0 (15% weight)
    double propagationScore;     // 0.0 to 100.0 (10% weight)
    uint64_t firstErrorTimeMs;
    size_t errorCount;

    std::string toJson() const {
        std::ostringstream oss;
        oss << "{"
            << "\"serviceName\":\"" << serviceName << "\","
            << "\"totalScore\":" << totalScore << ","
            << "\"temporalPriorityScore\":" << temporalPriorityScore << ","
            << "\"dependencyImpactScore\":" << dependencyImpactScore << ","
            << "\"errorFrequencyScore\":" << errorFrequencyScore << ","
            << "\"severityScore\":" << severityScore << ","
            << "\"propagationScore\":" << propagationScore << ","
            << "\"firstErrorTimeMs\":" << firstErrorTimeMs << ","
            << "\"errorCount\":" << errorCount
            << "}";
        return oss.str();
    }
};

struct RootCauseResult {
    std::string probableRootCause;
    double confidence; // 0.0 to 100.0
    std::vector<ServiceScoreBreakdown> rankedCandidates;
    std::vector<std::string> evidencePoints;
    std::string conclusion;

    std::string toJson() const {
        std::ostringstream oss;
        oss << "{"
            << "\"probableRootCause\":\"" << probableRootCause << "\","
            << "\"confidence\":" << confidence << ","
            << "\"conclusion\":\"" << conclusion << "\",";

        oss << "\"rankedCandidates\":[";
        for (size_t i = 0; i < rankedCandidates.size(); ++i) {
            oss << rankedCandidates[i].toJson();
            if (i + 1 < rankedCandidates.size()) oss << ",";
        }
        oss << "],";

        oss << "\"evidencePoints\":[";
        for (size_t i = 0; i < evidencePoints.size(); ++i) {
            oss << "\"" << evidencePoints[i] << "\"";
            if (i + 1 < evidencePoints.size()) oss << ",";
        }
        oss << "]";

        oss << "}";
        return oss.str();
    }
};

#endif // ROOT_CAUSE_RESULT_HPP
