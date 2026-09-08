#include <iostream>
#include <cassert>
#include <algorithm>
#include "../analyzer/graph/DependencyGraph.hpp"

int main() {
    std::cout << "[Test: DependencyGraph] Starting graph traversal and impact tests...\n";

    DependencyGraph graph;

    // 1. Verify downstream from Database Service
    auto dbDownstream = graph.getDownstreamServices("Database Service");
    std::cout << "  Database Service downstream count: " << dbDownstream.size() << "\n";
    for (const auto& s : dbDownstream) {
        std::cout << "    -> " << s << "\n";
    }
    // Should reach Order Service, Payment Service, API Gateway, Notification Service
    assert(std::find(dbDownstream.begin(), dbDownstream.end(), "Order Service") != dbDownstream.end());
    assert(std::find(dbDownstream.begin(), dbDownstream.end(), "Payment Service") != dbDownstream.end());
    assert(std::find(dbDownstream.begin(), dbDownstream.end(), "API Gateway") != dbDownstream.end());

    // 2. Verify reachability
    assert(graph.isReachable("Database Service", "API Gateway"));
    assert(graph.isReachable("Authentication Service", "API Gateway"));
    assert(!graph.isReachable("API Gateway", "Database Service")); // Directed: downstream only

    // 3. Verify path from Database Service to API Gateway
    auto path = graph.getPath("Database Service", "API Gateway");
    std::cout << "  Shortest failure path Database -> Gateway: ";
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i] << (i + 1 < path.size() ? " -> " : "\n");
    }
    assert(!path.empty());
    assert(path.front() == "Database Service");
    assert(path.back() == "API Gateway");

    // 4. Verify dependency impact calculation
    std::vector<std::string> affected = {"Database Service", "Order Service", "Payment Service", "API Gateway"};
    double dbImpact = graph.calculateDependencyImpact("Database Service", affected);
    double gatewayImpact = graph.calculateDependencyImpact("API Gateway", affected);
    std::cout << "  Database Service Impact Score: " << (dbImpact * 100.0) << "%\n";
    std::cout << "  API Gateway Impact Score: " << (gatewayImpact * 100.0) << "%\n";
    assert(dbImpact > gatewayImpact); // Upstream root explains downstream failures

    std::cout << "[Test: DependencyGraph] PASSED!\n";
    return 0;
}
