cmake_minimum_required(VERSION 3.15)
project(IntelligentDistributedFaultAnalyzer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common Simulator Sources
set(SIMULATOR_SOURCES
    simulator/logging/LogEntry.cpp
    simulator/services/Service.cpp
    simulator/services/DatabaseService.cpp
    simulator/services/PaymentService.cpp
    simulator/services/OrderService.cpp
    simulator/services/AuthService.cpp
    simulator/services/NotificationService.cpp
    simulator/services/APIGateway.cpp
    simulator/scenarios/SimulationManager.cpp
)

# Common Analyzer Sources
set(ANALYZER_SOURCES
    analyzer/parser/LogParser.cpp
    analyzer/graph/DependencyGraph.cpp
    analyzer/rootcause/RootCauseAnalyzer.cpp
    analyzer/detector/FaultDetector.cpp
    analyzer/LogProcessor.cpp
)

include_directories(
    simulator/logging
    simulator/services
    simulator/concurrency
    simulator/scenarios
    analyzer/parser
    analyzer/graph
    analyzer/detector
    analyzer/rootcause
    analyzer
    engine
)

# Link libraries (Winsock for Windows)
set(EXTRA_LIBS)
if(WIN32)
    set(EXTRA_LIBS ws2_32)
endif()

# Main Integrated Engine
add_executable(engine_app
    engine/EngineApp.cpp
    engine/HttpServer.cpp
    ${SIMULATOR_SOURCES}
    ${ANALYZER_SOURCES}
)
target_link_libraries(engine_app ${EXTRA_LIBS})

# Unit Tests
add_executable(test_queue
    tests/test_queue.cpp
    simulator/logging/LogEntry.cpp
)
target_link_libraries(test_queue ${EXTRA_LIBS})

add_executable(test_dependency_graph
    tests/test_dependency_graph.cpp
    analyzer/graph/DependencyGraph.cpp
)
target_link_libraries(test_dependency_graph ${EXTRA_LIBS})

add_executable(test_fault_detector
    tests/test_fault_detector.cpp
    simulator/logging/LogEntry.cpp
    analyzer/graph/DependencyGraph.cpp
    analyzer/rootcause/RootCauseAnalyzer.cpp
    analyzer/detector/FaultDetector.cpp
)
target_link_libraries(test_fault_detector ${EXTRA_LIBS})

add_executable(test_root_cause
    tests/test_root_cause.cpp
    simulator/logging/LogEntry.cpp
    analyzer/graph/DependencyGraph.cpp
    analyzer/rootcause/RootCauseAnalyzer.cpp
)
target_link_libraries(test_root_cause ${EXTRA_LIBS})

add_executable(benchmark_concurrency
    tests/benchmark_concurrency.cpp
    simulator/logging/LogEntry.cpp
)
target_link_libraries(benchmark_concurrency ${EXTRA_LIBS})
