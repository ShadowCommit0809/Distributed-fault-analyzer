#include "FaultDetector.hpp"
#include <algorithm>
#include <unordered_set>

FaultDetector::FaultDetector(std::shared_ptr<DependencyGraph> graph,
                             std::shared_ptr<RootCauseAnalyzer> rca)
    : m_graph(graph), m_rca(rca) {}

void FaultDetector::processLog(const LogEntry& log) {
    std::lock_guard<std::mutex> lock(m_detectorMutex);
    m_slidingWindow.push_back(log);
    purgeOldLogs(log.epochMs);
    evaluate();
}

void FaultDetector::processLogs(const std::vector<LogEntry>& logs) {
    if (logs.empty()) return;
    std::lock_guard<std::mutex> lock(m_detectorMutex);
    for (const auto& log : logs) {
        m_slidingWindow.push_back(log);
    }
    purgeOldLogs(logs.back().epochMs);
    evaluate();
}

void FaultDetector::purgeOldLogs(uint64_t currentEpochMs) {
    if (currentEpochMs < m_windowDurationMs) return;
    uint64_t cutoff = currentEpochMs - m_windowDurationMs;
    while (!m_slidingWindow.empty() && m_slidingWindow.front().epochMs < cutoff) {
        m_slidingWindow.pop_front();
    }
}

bool FaultDetector::detectErrorSpike(std::unordered_map<std::string, size_t>& outServiceErrors) const {
    size_t totalErrors = 0;
    for (const auto& log : m_slidingWindow) {
        if (log.severity == LogSeverity::ERROR || log.severity == LogSeverity::CRITICAL) {
            outServiceErrors[log.serviceName]++;
            totalErrors++;
        }
    }
    return totalErrors >= m_errorSpikeThreshold;
}

bool FaultDetector::detectHighLatency(std::unordered_map<std::string, int64_t>& outServiceLatency) const {
    std::unordered_map<std::string, std::pair<int64_t, size_t>> latMap;
    for (const auto& log : m_slidingWindow) {
        if (log.responseTimeMs > 0) {
            latMap[log.serviceName].first += log.responseTimeMs;
            latMap[log.serviceName].second++;
        }
    }

    bool hasHighLatency = false;
    for (const auto& pair : latMap) {
        if (pair.second.second > 0) {
            int64_t avg = pair.second.first / pair.second.second;
            outServiceLatency[pair.first] = avg;
            if (avg >= m_highLatencyThresholdMs) {
                hasHighLatency = true;
            }
        }
    }
    return hasHighLatency;
}

bool FaultDetector::detectServiceUnavailable(std::vector<std::string>& outUnavailableServices) const {
    std::unordered_set<std::string> unavail;
    for (const auto& log : m_slidingWindow) {
        if (log.eventType == EventType::SERVICE_UNAVAILABLE ||
            log.eventType == EventType::DATABASE_TIMEOUT ||
            log.eventType == EventType::CONNECTION_ERROR) {
            unavail.insert(log.serviceName);
        }
    }
    outUnavailableServices.assign(unavail.begin(), unavail.end());
    return !outUnavailableServices.empty();
}

bool FaultDetector::detectCascadingFailure(const std::vector<std::string>& affectedServices) const {
    if (affectedServices.size() < 2 || !m_graph) return false;
    for (const auto& s1 : affectedServices) {
        for (const auto& s2 : affectedServices) {
            if (s1 != s2 && m_graph->isReachable(s1, s2)) {
                return true;
            }
        }
    }
    return false;
}

void FaultDetector::evaluate() {
    if (m_slidingWindow.empty()) return;

    uint64_t nowEpoch = m_slidingWindow.back().epochMs;
    // Debounce incident creation (at most once every 4 seconds)
    if (nowEpoch < m_lastIncidentTriggerEpochMs + 4000) {
        return;
    }

    std::unordered_map<std::string, size_t> serviceErrors;
    bool hasSpike = detectErrorSpike(serviceErrors);

    std::unordered_map<std::string, int64_t> serviceLatency;
    bool hasHighLatency = detectHighLatency(serviceLatency);

    std::vector<std::string> unavailableServices;
    bool hasUnavailable = detectServiceUnavailable(unavailableServices);

    if (!hasSpike && !hasHighLatency && !hasUnavailable) {
        return;
    }

    // Collect all affected services
    std::unordered_set<std::string> affectedSet;
    for (const auto& p : serviceErrors) {
        if (p.second > 0) affectedSet.insert(p.first);
    }
    for (const auto& p : serviceLatency) {
        if (p.second >= m_highLatencyThresholdMs) affectedSet.insert(p.first);
    }
    for (const auto& s : unavailableServices) {
        affectedSet.insert(s);
    }

    std::vector<std::string> affectedServices(affectedSet.begin(), affectedSet.end());
    bool isCascade = detectCascadingFailure(affectedServices);

    std::string detectionReason;
    std::string severity = "ERROR";

    if (isCascade) {
        detectionReason = "Cascading failure detected propagating across " + std::to_string(affectedServices.size()) + " distributed services";
        severity = "CRITICAL";
    } else if (hasUnavailable) {
        detectionReason = "Service unavailable or connection dropped in: " + (unavailableServices.empty() ? "" : unavailableServices.front());
        severity = "CRITICAL";
    } else if (hasSpike) {
        detectionReason = "Error spike threshold exceeded (>=" + std::to_string(m_errorSpikeThreshold) + " errors in sliding window)";
        severity = "ERROR";
    } else if (hasHighLatency) {
        detectionReason = "Service response latency exceeded anomaly threshold (" + std::to_string(m_highLatencyThresholdMs) + "ms)";
        severity = "WARNING";
    }

    triggerIncident(detectionReason, affectedServices, severity);
}

std::vector<IncidentTimelineEvent> FaultDetector::buildTimeline(const std::vector<LogEntry>& logs,
                                                               const std::string& rootCause) const {
    std::vector<IncidentTimelineEvent> timeline;
    uint64_t firstEpoch = 0;

    for (const auto& log : logs) {
        if (log.severity == LogSeverity::ERROR || log.severity == LogSeverity::CRITICAL ||
            log.eventType == EventType::HIGH_LATENCY || log.eventType == EventType::DATABASE_TIMEOUT) {
            if (firstEpoch == 0) {
                firstEpoch = log.epochMs;
            }

            IncidentTimelineEvent ev;
            ev.timestamp = log.timestamp;
            ev.serviceName = log.serviceName;
            ev.description = log.message;
            ev.eventType = log.eventTypeToString();
            ev.severity = log.severityToString();
            ev.relativeOffsetMs = static_cast<int64_t>(log.epochMs - firstEpoch);
            ev.traceId = log.traceId;

            timeline.push_back(std::move(ev));
            if (timeline.size() >= 15) break; // Keep timeline concise and clean
        }
    }

    return timeline;
}

void FaultDetector::triggerIncident(const std::string& detectionReason,
                                   const std::vector<std::string>& affectedServices,
                                   const std::string& severity) {
    m_incidentCounter++;
    Incident incident;
    incident.id = "INC-" + std::to_string(m_incidentCounter);
    incident.timestamp = LogEntry::currentTimestamp(&incident.epochMs);
    incident.severity = severity;
    incident.status = "ACTIVE";
    incident.summary = detectionReason;
    incident.affectedServices = affectedServices;

    // Collect correlated logs
    std::vector<LogEntry> incidentLogs;
    for (const auto& log : m_slidingWindow) {
        incidentLogs.push_back(log);
    }
    incident.correlatedLogs = incidentLogs;

    // Run Root Cause Analysis
    if (m_rca) {
        RootCauseResult rcaRes = m_rca->analyze(incidentLogs, affectedServices);
        incident.probableRootCause = rcaRes.probableRootCause;
        incident.confidence = rcaRes.confidence;
        incident.evidence = rcaRes.evidencePoints;
        incident.conclusion = rcaRes.conclusion;
    } else {
        incident.probableRootCause = affectedServices.empty() ? "Unknown" : affectedServices.front();
        incident.confidence = 80.0;
        incident.conclusion = "Incident detected based on anomaly thresholds.";
    }

    // Build timeline
    incident.timeline = buildTimeline(incidentLogs, incident.probableRootCause);

    m_lastIncidentTriggerEpochMs = incident.epochMs;
    m_incidents.push_back(incident);
    m_activeIncident = std::make_shared<Incident>(incident);
}

std::vector<Incident> FaultDetector::getAllIncidents() const {
    return m_incidents;
}

std::shared_ptr<Incident> FaultDetector::getActiveIncident() const {
    return m_activeIncident;
}

std::shared_ptr<Incident> FaultDetector::getIncidentById(const std::string& id) const {
    for (const auto& inc : m_incidents) {
        if (inc.id == id) {
            return std::make_shared<Incident>(inc);
        }
    }
    return nullptr;
}

void FaultDetector::reset() {
    std::lock_guard<std::mutex> lock(m_detectorMutex);
    m_slidingWindow.clear();
    m_activeIncident = nullptr;
    for (auto& inc : m_incidents) {
        inc.status = "RESOLVED";
    }
}
