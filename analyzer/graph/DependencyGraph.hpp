#ifndef DEPENDENCY_GRAPH_HPP
#define DEPENDENCY_GRAPH_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class DependencyGraph {
private:
    // Directed graph: service -> list of services that directly depend on it (downstream)
    std::unordered_map<std::string, std::vector<std::string>> m_downstream;
    // Reverse graph: service -> list of services it directly calls/depends on (upstream)
    std::unordered_map<std::string, std::vector<std::string>> m_upstream;
    std::vector<std::string> m_allServices;

public:
    DependencyGraph();

    void addDependency(const std::string& upstreamService, const std::string& downstreamService);
    
    // Returns all services directly or indirectly affected if 'service' fails (downstream impact)
    std::vector<std::string> getDownstreamServices(const std::string& service) const;

    // Returns all services that 'service' depends on directly or indirectly (upstream causes)
    std::vector<std::string> getUpstreamDependencies(const std::string& service) const;

    // Checks if target is reachable downstream from source
    bool isReachable(const std::string& source, const std::string& target) const;

    // Finds the shortest dependency path from source to target
    std::vector<std::string> getPath(const std::string& source, const std::string& target) const;

    // Calculates what fraction of the affected services are downstream of candidateService
    double calculateDependencyImpact(const std::string& candidateService,
                                    const std::vector<std::string>& affectedServices) const;

    const std::vector<std::string>& getAllServices() const { return m_allServices; }
    
    // Serializes the graph edges to JSON for the React UI
    std::string toJson() const;
};

#endif // DEPENDENCY_GRAPH_HPP
