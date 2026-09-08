#ifndef FAULT_DETECTOR_HPP
#define FAULT_DETECTOR_HPP

#include <vector>
#include <deque>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "Incident.hpp"
#include "../graph/DependencyGraph.hpp"
#include "../rootcause/RootCauseAnalyzer.hpp"
#include "../../simulator/logging/LogEntry.hpp"

class FaultDetector {
private:
    std::shared_ptr<DependencyGraph> m_graph;
    std::shared_ptr<RootCauseAnalyzer> m_rca;

    std::mutex m_detectorMutex;
    std::deque<LogEntry> m_slidingWindow;
    uint64_t m_windowDurationMs{8000}; // 8-second sliding window

    // Detection thresholds
    size_t m_errorSpikeThreshold{4};
    int64_t m_highLatencyThresholdMs{1500};

    std::vector<Incident> m_incidents;
    std::shared_ptr<Incident> m_activeIncident{nullptr};
    uint64_t m_lastIncidentTriggerEpochMs{0};
    uint64_t m_incidentCounter{100};

public:
    FaultDetector(std::shared_ptr<DependencyGraph> graph,
                  std::shared_ptr<RootCauseAnalyzer> rca);

    // Feed logs into the detector
    void processLog(const LogEntry& log);
    void processLogs(const std::vector<LogEntry>& logs);

    // Check rules and evaluate incidents
    void evaluate();

    std::vector<Incident> getAllIncidents() const;
    std::shared_ptr<Incident> getActiveIncident() const;
    std::shared_ptr<Incident> getIncidentById(const std::string& id) const;

    void reset();

private:
    void purgeOldLogs(uint64_t currentEpochMs);
    bool detectErrorSpike(std::unordered_map<std::string, size_t>& outServiceErrors) const;
    bool detectHighLatency(std::unordered_map<std::string, int64_t>& outServiceLatency) const;
    bool detectServiceUnavailable(std::vector<std::string>& outUnavailableServices) const;
    bool detectCascadingFailure(const std::vector<std::string>& affectedServices) const;

    void triggerIncident(const std::string& detectionReason,
                         const std::vector<std::string>& affectedServices,
                         const std::string& severity);

    std::vector<IncidentTimelineEvent> buildTimeline(const std::vector<LogEntry>& logs,
                                                     const std::string& rootCause) const;
};

#endif // FAULT_DETECTOR_HPP
