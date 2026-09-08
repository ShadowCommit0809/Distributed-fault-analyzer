#ifndef ROOT_CAUSE_ANALYZER_HPP
#define ROOT_CAUSE_ANALYZER_HPP

#include <vector>
#include <memory>
#include "RootCauseResult.hpp"
#include "../../simulator/logging/LogEntry.hpp"
#include "../graph/DependencyGraph.hpp"

class RootCauseAnalyzer {
private:
    std::shared_ptr<DependencyGraph> m_graph;

    // Configurable weights (summing to 1.0)
    double m_wTemporal{0.30};
    double m_wDependency{0.25};
    double m_wFrequency{0.20};
    double m_wSeverity{0.15};
    double m_wPropagation{0.10};

public:
    RootCauseAnalyzer(std::shared_ptr<DependencyGraph> graph);

    void setWeights(double wTemp, double wDep, double wFreq, double wSev, double wProp);

    RootCauseResult analyze(const std::vector<LogEntry>& incidentLogs,
                            const std::vector<std::string>& affectedServices);

private:
    std::vector<std::string> generateEvidence(const std::string& rootCause,
                                              const std::vector<ServiceScoreBreakdown>& rankings,
                                              const std::vector<LogEntry>& logs,
                                              const std::vector<std::string>& affectedServices);

    std::string generateConclusion(const std::string& rootCause, double confidence,
                                   const std::vector<std::string>& affectedServices);
};

#endif // ROOT_CAUSE_ANALYZER_HPP
