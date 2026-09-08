#ifndef SCENARIO_HPP
#define SCENARIO_HPP

#include <string>

enum class SimulationScenario {
    NORMAL,
    DATABASE_FAILURE,
    AUTH_FAILURE,
    HIGH_LATENCY,
    SERVICE_UNAVAILABLE,
    CASCADING_FAILURE
};

inline std::string scenarioToString(SimulationScenario s) {
    switch (s) {
        case SimulationScenario::NORMAL: return "NORMAL";
        case SimulationScenario::DATABASE_FAILURE: return "DATABASE_FAILURE";
        case SimulationScenario::AUTH_FAILURE: return "AUTH_FAILURE";
        case SimulationScenario::HIGH_LATENCY: return "HIGH_LATENCY";
        case SimulationScenario::SERVICE_UNAVAILABLE: return "SERVICE_UNAVAILABLE";
        case SimulationScenario::CASCADING_FAILURE: return "CASCADING_FAILURE";
        default: return "UNKNOWN";
    }
}

inline SimulationScenario stringToScenario(const std::string& str) {
    if (str == "DATABASE_FAILURE" || str == "DATABASE_DOWN") return SimulationScenario::DATABASE_FAILURE;
    if (str == "AUTH_FAILURE" || str == "AUTH_REJECT") return SimulationScenario::AUTH_FAILURE;
    if (str == "HIGH_LATENCY") return SimulationScenario::HIGH_LATENCY;
    if (str == "SERVICE_UNAVAILABLE" || str == "SERVICE_CRASH") return SimulationScenario::SERVICE_UNAVAILABLE;
    if (str == "CASCADING_FAILURE") return SimulationScenario::CASCADING_FAILURE;
    return SimulationScenario::NORMAL;
}

#endif // SCENARIO_HPP
