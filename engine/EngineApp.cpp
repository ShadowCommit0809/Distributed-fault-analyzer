#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <csignal>

#include "../simulator/logging/LogEntry.hpp"
#include "../simulator/concurrency/ThreadSafeQueue.hpp"
#include "../simulator/scenarios/SimulationManager.hpp"
#include "../analyzer/graph/DependencyGraph.hpp"
#include "../analyzer/rootcause/RootCauseAnalyzer.hpp"
#include "../analyzer/detector/FaultDetector.hpp"
#include "../analyzer/LogProcessor.hpp"
#include "HttpServer.hpp"

static std::atomic<bool> g_appRunning{true};

void signalHandler(int signum) {
    std::cout << "\n[Engine] Interrupted (signal " << signum << "). Shutting down gracefully...\n";
    g_appRunning = false;
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    int port = 8081;
    if (argc > 1) {
        try { port = std::stoi(argv[1]); } catch (...) {}
    }

    std::cout << "===================================================================\n";
    std::cout << "  Intelligent Distributed System Fault Detection & Root-Cause Engine\n";
    std::cout << "  C++17 Multi-threaded Simulator & Telemetry Analyzer\n";
    std::cout << "===================================================================\n";

    // 1. Initialize Core Concurrency & Data Structures
    auto logQueue = std::make_shared<ThreadSafeQueue<LogEntry>>(20000);
    auto simManager = std::make_shared<SimulationManager>(logQueue);
    auto depGraph = std::make_shared<DependencyGraph>();
    auto rca = std::make_shared<RootCauseAnalyzer>(depGraph);
    auto detector = std::make_shared<FaultDetector>(depGraph, rca);
    auto logProcessor = std::make_shared<LogProcessor>(logQueue, detector);

    logProcessor->start();
    std::cout << "[Engine] Background log processor worker started.\n";

    // Start in Normal Operation mode by default
    simManager->startSimulation(SimulationScenario::NORMAL, 4);
    std::cout << "[Engine] Initialized simulation in NORMAL operation mode.\n";

    // 2. Setup REST API Endpoints
    HttpServer server(port);

    // Health check
    server.get("/health", [](const HttpRequest& req) {
        HttpResponse res;
        res.body = "{\"status\":\"UP\",\"engine\":\"Intelligent Distributed System Fault Detection Engine (C++17)\"}";
        return res;
    });
    server.get("/api/health", [](const HttpRequest& req) {
        HttpResponse res;
        res.body = "{\"status\":\"UP\",\"engine\":\"Intelligent Distributed System Fault Detection Engine (C++17)\"}";
        return res;
    });

    // Services status & metrics
    server.get("/api/services", [simManager](const HttpRequest& req) {
        HttpResponse res;
        auto metrics = simManager->getAllServiceMetrics();
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < metrics.size(); ++i) {
            const auto& m = metrics[i];
            oss << "{"
                << "\"id\":\"" << m.id << "\","
                << "\"name\":\"" << m.name << "\","
                << "\"status\":\"" << m.statusStr << "\","
                << "\"avgResponseTimeMs\":" << m.avgResponseTimeMs << ","
                << "\"totalRequests\":" << m.totalRequests << ","
                << "\"totalErrors\":" << m.totalErrors << ","
                << "\"errorRatePercent\":" << m.errorRatePercent << ","
                << "\"currentFault\":\"" << m.currentFault << "\""
                << "}";
            if (i + 1 < metrics.size()) oss << ",";
        }
        oss << "]";
        res.body = oss.str();
        return res;
    });

    // Dependency graph topology
    server.get("/api/graph", [depGraph](const HttpRequest& req) {
        HttpResponse res;
        res.body = depGraph->toJson();
        return res;
    });

    // Incidents
    server.get("/api/incidents", [detector](const HttpRequest& req) {
        HttpResponse res;
        auto incidents = detector->getAllIncidents();
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < incidents.size(); ++i) {
            oss << incidents[i].toJson();
            if (i + 1 < incidents.size()) oss << ",";
        }
        oss << "]";
        res.body = oss.str();
        return res;
    });

    // Active incident
    server.get("/api/incidents/active", [detector](const HttpRequest& req) {
        HttpResponse res;
        auto active = detector->getActiveIncident();
        if (active) {
            res.body = active->toJson();
        } else {
            res.body = "null";
        }
        return res;
    });

    // Logs query
    server.get("/api/logs", [logProcessor](const HttpRequest& req) {
        HttpResponse res;
        size_t limit = 100;
        std::string sFilter = "";
        std::string sevFilter = "";

        // Query param parsing
        if (!req.query.empty()) {
            size_t sPos = req.query.find("service=");
            if (sPos != std::string::npos) {
                size_t sEnd = req.query.find("&", sPos);
                sFilter = req.query.substr(sPos + 8, (sEnd == std::string::npos ? req.query.length() : sEnd) - (sPos + 8));
            }
            size_t sevPos = req.query.find("severity=");
            if (sevPos != std::string::npos) {
                size_t sevEnd = req.query.find("&", sevPos);
                sevFilter = req.query.substr(sevPos + 9, (sevEnd == std::string::npos ? req.query.length() : sevEnd) - (sevPos + 9));
            }
            size_t limPos = req.query.find("limit=");
            if (limPos != std::string::npos) {
                size_t limEnd = req.query.find("&", limPos);
                std::string lStr = req.query.substr(limPos + 6, (limEnd == std::string::npos ? req.query.length() : limEnd) - (limPos + 6));
                try { limit = std::stoul(lStr); } catch (...) {}
            }
        }

        auto logs = logProcessor->getRecentLogs(limit, sFilter, sevFilter);
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < logs.size(); ++i) {
            oss << logs[i].toJson();
            if (i + 1 < logs.size()) oss << ",";
        }
        oss << "]";
        res.body = oss.str();
        return res;
    });

    // Simulation controls: Start
    server.post("/api/simulation/start", [simManager](const HttpRequest& req) {
        HttpResponse res;
        SimulationScenario sc = SimulationScenario::NORMAL;
        if (req.body.find("DATABASE_FAILURE") != std::string::npos || req.body.find("DATABASE_DOWN") != std::string::npos) {
            sc = SimulationScenario::DATABASE_FAILURE;
        } else if (req.body.find("AUTH_FAILURE") != std::string::npos || req.body.find("AUTH_REJECT") != std::string::npos) {
            sc = SimulationScenario::AUTH_FAILURE;
        } else if (req.body.find("HIGH_LATENCY") != std::string::npos) {
            sc = SimulationScenario::HIGH_LATENCY;
        } else if (req.body.find("SERVICE_UNAVAILABLE") != std::string::npos || req.body.find("SERVICE_CRASH") != std::string::npos) {
            sc = SimulationScenario::SERVICE_UNAVAILABLE;
        } else if (req.body.find("CASCADING_FAILURE") != std::string::npos) {
            sc = SimulationScenario::CASCADING_FAILURE;
        } else {
            sc = SimulationScenario::NORMAL;
        }

        int rps = 4;
        simManager->startSimulation(sc, rps);

        res.body = "{\"status\":\"STARTED\",\"scenario\":\"" + scenarioToString(sc) + "\"}";
        std::cout << "[Engine API] Scenario started: " << scenarioToString(sc) << "\n";
        return res;
    });

    // Simulation controls: Stop
    server.post("/api/simulation/stop", [simManager](const HttpRequest& req) {
        HttpResponse res;
        simManager->stopSimulation();
        res.body = "{\"status\":\"STOPPED\"}";
        std::cout << "[Engine API] Simulation stopped.\n";
        return res;
    });

    // Simulation controls: Reset
    server.post("/api/simulation/reset", [simManager, detector, logProcessor](const HttpRequest& req) {
        HttpResponse res;
        simManager->resetSimulation();
        detector->reset();
        logProcessor->clearLogs();
        res.body = "{\"status\":\"RESET\",\"scenario\":\"NORMAL\"}";
        std::cout << "[Engine API] Simulation and detector reset to clean baseline.\n";
        return res;
    });

    // Statistics
    server.get("/api/statistics", [simManager, detector, logProcessor](const HttpRequest& req) {
        HttpResponse res;
        auto metrics = simManager->getAllServiceMetrics();
        int healthy = 0, warning = 0, failed = 0;
        uint64_t totalErrors = 0;
        int64_t totalLatency = 0;

        for (const auto& m : metrics) {
            if (m.statusStr == "HEALTHY") healthy++;
            else if (m.statusStr == "WARNING") warning++;
            else failed++;
            totalErrors += m.totalErrors;
            totalLatency += m.avgResponseTimeMs;
        }

        int64_t avgLatency = metrics.empty() ? 0 : (totalLatency / metrics.size());
        auto incidents = detector->getAllIncidents();
        int activeIncidents = detector->getActiveIncident() ? 1 : 0;

        std::ostringstream oss;
        oss << "{"
            << "\"totalServices\":" << metrics.size() << ","
            << "\"healthyServices\":" << healthy << ","
            << "\"warningServices\":" << warning << ","
            << "\"failedServices\":" << failed << ","
            << "\"activeIncidents\":" << activeIncidents << ","
            << "\"totalIncidents\":" << incidents.size() << ","
            << "\"totalLogsProcessed\":" << logProcessor->getTotalProcessed() << ","
            << "\"totalErrors\":" << totalErrors << ","
            << "\"avgResponseTimeMs\":" << avgLatency << ","
            << "\"currentScenario\":\"" << scenarioToString(simManager->getCurrentScenario()) << "\","
            << "\"isSimulationRunning\":" << (simManager->isRunning() ? "true" : "false")
            << "}";
        res.body = oss.str();
        return res;
    });

    if (!server.start()) {
        std::cerr << "[Engine Error] Failed to bind HTTP server to port " << port << "!\n";
        return 1;
    }

    std::cout << "[Engine] REST API Server listening on http://localhost:" << port << "\n";
    std::cout << "Available interactive scenarios via CLI:\n";
    std::cout << "  [1] Normal Operation\n";
    std::cout << "  [2] Database Failure (Cascading: DB -> Order -> Payment -> Gateway)\n";
    std::cout << "  [3] Authentication Failure (Auth rejects tokens)\n";
    std::cout << "  [4] High Latency (Order Service latency spike)\n";
    std::cout << "  [5] Service Unavailable (Payment Service crashes)\n";
    std::cout << "  [6] Cascading Failure (Database timeout + retry surge)\n";
    std::cout << "  [s] Stop simulation | [r] Reset all | [q] Quit\n";
    std::cout << "-------------------------------------------------------------------\n";

    // Main loop: non-blocking sleep so signal handler or keyboard can control it
    while (g_appRunning.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "[Engine] Stopping HTTP server and workers...\n";
    server.stop();
    simManager->stopSimulation();
    logProcessor->stop();
    std::cout << "[Engine] Gracefully shutdown complete.\n";

    return 0;
}
