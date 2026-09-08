#include <iostream>
#include <cassert>
#include <memory>
#include "../simulator/logging/LogEntry.hpp"
#include "../analyzer/graph/DependencyGraph.hpp"
#include "../analyzer/rootcause/RootCauseAnalyzer.hpp"
#include "../analyzer/detector/FaultDetector.hpp"

int main() {
    std::cout << "[Test: FaultDetector] Starting rule-based anomaly and incident tests...\n";

    auto graph = std::make_shared<DependencyGraph>();
    auto rca = std::make_shared<RootCauseAnalyzer>(graph);
    auto detector = std::make_shared<FaultDetector>(graph, rca);

    // 1. Send normal logs -> No incident
    for (int i = 0; i < 5; ++i) {
        LogEntry log("API Gateway", LogSeverity::INFO, EventType::REQUEST_COMPLETED,
                     "Handled request 200 OK", "REQ-1", "TR-1", 50);
        detector->processLog(log);
    }
    assert(detector->getActiveIncident() == nullptr);
    std::cout << "  Normal traffic: No false positives verified.\n";

    // 2. Simulate cascading database failure chain:
    // First: Database timeout
    uint64_t t0 = 1725790000000ULL;
    LogEntry dbLog("Database Service", LogSeverity::CRITICAL, EventType::DATABASE_TIMEOUT,
                   "Connection pool exhausted timeout", "REQ-100", "TR-100", 3000);
    dbLog.epochMs = t0;
    detector->processLog(dbLog);

    // Second: Order Service failure
    LogEntry orderLog("Order Service", LogSeverity::ERROR, EventType::CONNECTION_ERROR,
                      "Database request failed", "REQ-100", "TR-100", 3050);
    orderLog.epochMs = t0 + 150;
    detector->processLog(orderLog);

    // Third: Payment Service timeout
    LogEntry payLog("Payment Service", LogSeverity::ERROR, EventType::PAYMENT_FAILURE,
                    "Payment timeout on database ledger", "REQ-100", "TR-100", 3200);
    payLog.epochMs = t0 + 200;
    detector->processLog(payLog);

    // Fourth: API Gateway 500 error
    LogEntry gwLog("API Gateway", LogSeverity::ERROR, EventType::HTTP_500,
                   "HTTP 500 Internal Server Error", "REQ-100", "TR-100", 3300);
    gwLog.epochMs = t0 + 300;
    detector->processLog(gwLog);

    // More errors to breach spike threshold
    for (int i = 1; i <= 3; ++i) {
        LogEntry extraLog("API Gateway", LogSeverity::ERROR, EventType::HTTP_500,
                          "Repeated HTTP 500 downstream failure", "REQ-10" + std::to_string(i), "TR-10" + std::to_string(i), 3200);
        extraLog.epochMs = t0 + 350 + (i * 50);
        detector->processLog(extraLog);
    }

    // Verify an incident was triggered
    auto incident = detector->getActiveIncident();
    assert(incident != nullptr);
    std::cout << "  Incident Detected: " << incident->id << " - " << incident->summary << "\n";
    std::cout << "  Severity: " << incident->severity << "\n";
    std::cout << "  Probable Root Cause: " << incident->probableRootCause << "\n";
    std::cout << "  Confidence: " << incident->confidence << "%\n";

    assert(incident->probableRootCause == "Database Service");
    assert(incident->confidence >= 80.0);
    assert(!incident->evidence.empty());
    assert(!incident->timeline.empty());

    std::cout << "  Evidence points count: " << incident->evidence.size() << "\n";
    for (const auto& ev : incident->evidence) {
        std::cout << "    " << ev << "\n";
    }

    std::cout << "[Test: FaultDetector] PASSED!\n";
    return 0;
}
