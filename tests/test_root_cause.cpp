#include <iostream>
#include <cassert>
#include <memory>
#include "../simulator/logging/LogEntry.hpp"
#include "../analyzer/graph/DependencyGraph.hpp"
#include "../analyzer/rootcause/RootCauseAnalyzer.hpp"

int main() {
    std::cout << "[Test: RootCauseAnalyzer] Testing 5-factor explainable root-cause algorithm...\n";

    auto graph = std::make_shared<DependencyGraph>();
    auto rca = std::make_shared<RootCauseAnalyzer>(graph);

    uint64_t t0 = 1725790000000ULL;
    std::vector<LogEntry> logs;

    // Database Service logs ONE critical error first:
    LogEntry dbLog("Database Service", LogSeverity::CRITICAL, EventType::DATABASE_TIMEOUT,
                   "Connection timeout: pool exhausted", "REQ-1", "TR-1", 3000);
    dbLog.epochMs = t0;
    logs.push_back(dbLog);

    // Order Service logs 2 errors next:
    for (int i = 0; i < 2; ++i) {
        LogEntry ordLog("Order Service", LogSeverity::ERROR, EventType::CONNECTION_ERROR,
                        "Database connection failed", "REQ-" + std::to_string(i + 2), "TR-1", 3100);
        ordLog.epochMs = t0 + 100 + (i * 50);
        logs.push_back(ordLog);
    }

    // Payment Service logs 2 errors:
    for (int i = 0; i < 2; ++i) {
        LogEntry payLog("Payment Service", LogSeverity::ERROR, EventType::PAYMENT_FAILURE,
                        "Payment timeout", "REQ-" + std::to_string(i + 4), "TR-1", 3200);
        payLog.epochMs = t0 + 200 + (i * 50);
        logs.push_back(payLog);
    }

    // API Gateway logs TEN errors (symptom storm at the edge):
    for (int i = 0; i < 10; ++i) {
        LogEntry gwLog("API Gateway", LogSeverity::ERROR, EventType::HTTP_500,
                       "HTTP 500 downstream failure", "REQ-" + std::to_string(i + 6), "TR-1", 3300);
        gwLog.epochMs = t0 + 400 + (i * 50);
        logs.push_back(gwLog);
    }

    std::vector<std::string> affected = {"API Gateway", "Order Service", "Payment Service", "Database Service"};

    RootCauseResult res = rca->analyze(logs, affected);

    std::cout << "\n  Probable Root Cause: " << res.probableRootCause << "\n";
    std::cout << "  Confidence: " << res.confidence << "%\n";
    std::cout << "  Ranked Candidates:\n";
    for (size_t i = 0; i < res.rankedCandidates.size(); ++i) {
        const auto& c = res.rankedCandidates[i];
        std::cout << "    " << (i + 1) << ". " << c.serviceName
                  << " -> Total Score: " << c.totalScore
                  << " (Temporal=" << c.temporalPriorityScore
                  << ", Dependency=" << c.dependencyImpactScore
                  << ", Frequency=" << c.errorFrequencyScore
                  << ", Severity=" << c.severityScore
                  << ", Propagation=" << c.propagationScore << ")\n";
    }

    std::cout << "\n  Evidence points:\n";
    for (const auto& ev : res.evidencePoints) {
        std::cout << "    " << ev << "\n";
    }
    std::cout << "\n  Conclusion:\n    " << res.conclusion << "\n";

    // CRITICAL ASSERTION: Even though API Gateway had 10 errors and Database had 1 error,
    // Database Service MUST win because of Temporal Priority and Dependency Impact!
    assert(res.probableRootCause == "Database Service");
    assert(res.confidence > 80.0);
    assert(!res.evidencePoints.empty());

    std::cout << "\n[Test: RootCauseAnalyzer] PASSED! Correctly avoided naive error counting.\n";
    return 0;
}
