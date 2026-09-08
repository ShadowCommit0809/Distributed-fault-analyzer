#include "DependencyGraph.hpp"
#include <queue>
#include <sstream>
#include <algorithm>

DependencyGraph::DependencyGraph() {
    // Service catalog
    m_allServices = {
        "Database Service",
        "Payment Service",
        "Order Service",
        "Authentication Service",
        "API Gateway",
        "Notification Service"
    };

    // Failure propagation edges: Failure in A directly propagates to B
    // Database failure propagates to Order Service and Payment Service
    addDependency("Database Service", "Order Service");
    addDependency("Database Service", "Payment Service");

    // Payment failure propagates to Order Service
    addDependency("Payment Service", "Order Service");

    // Order failure propagates to API Gateway and Notification Service
    addDependency("Order Service", "API Gateway");
    addDependency("Order Service", "Notification Service");

    // Auth failure propagates to API Gateway
    addDependency("Authentication Service", "API Gateway");
}

void DependencyGraph::addDependency(const std::string& upstreamService, const std::string& downstreamService) {
    m_downstream[upstreamService].push_back(downstreamService);
    m_upstream[downstreamService].push_back(upstreamService);
}

std::vector<std::string> DependencyGraph::getDownstreamServices(const std::string& service) const {
    std::vector<std::string> result;
    std::unordered_set<std::string> visited;
    std::queue<std::string> q;

    q.push(service);
    visited.insert(service);

    while (!q.empty()) {
        std::string curr = q.front();
        q.pop();

        auto it = m_downstream.find(curr);
        if (it != m_downstream.end()) {
            for (const auto& neighbor : it->second) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    result.push_back(neighbor);
                    q.push(neighbor);
                }
            }
        }
    }

    return result;
}

std::vector<std::string> DependencyGraph::getUpstreamDependencies(const std::string& service) const {
    std::vector<std::string> result;
    std::unordered_set<std::string> visited;
    std::queue<std::string> q;

    q.push(service);
    visited.insert(service);

    while (!q.empty()) {
        std::string curr = q.front();
        q.pop();

        auto it = m_upstream.find(curr);
        if (it != m_upstream.end()) {
            for (const auto& neighbor : it->second) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    result.push_back(neighbor);
                    q.push(neighbor);
                }
            }
        }
    }

    return result;
}

bool DependencyGraph::isReachable(const std::string& source, const std::string& target) const {
    if (source == target) return true;
    auto downstream = getDownstreamServices(source);
    return std::find(downstream.begin(), downstream.end(), target) != downstream.end();
}

std::vector<std::string> DependencyGraph::getPath(const std::string& source, const std::string& target) const {
    if (source == target) return {source};

    std::unordered_map<std::string, std::string> parent;
    std::unordered_set<std::string> visited;
    std::queue<std::string> q;

    q.push(source);
    visited.insert(source);

    bool found = false;
    while (!q.empty()) {
        std::string curr = q.front();
        q.pop();

        if (curr == target) {
            found = true;
            break;
        }

        auto it = m_downstream.find(curr);
        if (it != m_downstream.end()) {
            for (const auto& neighbor : it->second) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    parent[neighbor] = curr;
                    q.push(neighbor);
                }
            }
        }
    }

    if (!found) return {};

    std::vector<std::string> path;
    std::string step = target;
    while (step != source) {
        path.push_back(step);
        step = parent[step];
    }
    path.push_back(source);
    std::reverse(path.begin(), path.end());
    return path;
}

double DependencyGraph::calculateDependencyImpact(const std::string& candidateService,
                                                 const std::vector<std::string>& affectedServices) const {
    if (affectedServices.empty()) return 0.0;

    auto downstream = getDownstreamServices(candidateService);
    std::unordered_set<std::string> downstreamSet(downstream.begin(), downstream.end());

    size_t explainedCount = 0;
    for (const auto& aff : affectedServices) {
        if (aff == candidateService) {
            explainedCount++;
        } else if (downstreamSet.find(aff) != downstreamSet.end()) {
            explainedCount++;
        }
    }

    return static_cast<double>(explainedCount) / static_cast<double>(affectedServices.size());
}

std::string DependencyGraph::toJson() const {
    std::ostringstream oss;
    oss << "{\"nodes\":[";
    for (size_t i = 0; i < m_allServices.size(); ++i) {
        oss << "\"" << m_allServices[i] << "\"";
        if (i + 1 < m_allServices.size()) oss << ",";
    }
    oss << "],\"edges\":[";
    bool first = true;
    for (const auto& pair : m_downstream) {
        for (const auto& target : pair.second) {
            if (!first) oss << ",";
            first = false;
            oss << "{\"from\":\"" << pair.first << "\",\"to\":\"" << target << "\"}";
        }
    }
    oss << "]}";
    return oss.str();
}
