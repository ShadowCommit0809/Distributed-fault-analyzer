# Build script for C++ Engine and Test Suite
param(
    [string]$Target = "all"
)

$ErrorActionPreference = "Stop"

$binDir = "bin"
if (!(Test-Path $binDir)) {
    New-Item -ItemType Directory -Path $binDir | Out-Null
}

$CXX = "g++"
$FLAGS = "-std=c++17", "-O2", "-Wall", "-I.", "-Isimulator/logging", "-Isimulator/services", "-Isimulator/concurrency", "-Isimulator/scenarios", "-Ianalyzer/parser", "-Ianalyzer/graph", "-Ianalyzer/detector", "-Ianalyzer/rootcause", "-Ianalyzer", "-Iengine"
$LIBS = "-lws2_32"

$simSources = @(
    "simulator/logging/LogEntry.cpp",
    "simulator/services/Service.cpp",
    "simulator/services/DatabaseService.cpp",
    "simulator/services/PaymentService.cpp",
    "simulator/services/OrderService.cpp",
    "simulator/services/AuthService.cpp",
    "simulator/services/NotificationService.cpp",
    "simulator/services/APIGateway.cpp",
    "simulator/scenarios/SimulationManager.cpp"
)

$analyzerSources = @(
    "analyzer/parser/LogParser.cpp",
    "analyzer/graph/DependencyGraph.cpp",
    "analyzer/rootcause/RootCauseAnalyzer.cpp",
    "analyzer/detector/FaultDetector.cpp",
    "analyzer/LogProcessor.cpp"
)

Write-Host "Building C++ components..." -ForegroundColor Cyan

# 1. Unit Tests
Write-Host "Compiling test_queue..." -ForegroundColor Yellow
& $CXX $FLAGS tests/test_queue.cpp simulator/logging/LogEntry.cpp -o bin/test_queue.exe $LIBS

Write-Host "Compiling test_dependency_graph..." -ForegroundColor Yellow
& $CXX $FLAGS tests/test_dependency_graph.cpp analyzer/graph/DependencyGraph.cpp -o bin/test_dependency_graph.exe $LIBS

Write-Host "Compiling test_root_cause..." -ForegroundColor Yellow
& $CXX $FLAGS tests/test_root_cause.cpp simulator/logging/LogEntry.cpp analyzer/graph/DependencyGraph.cpp analyzer/rootcause/RootCauseAnalyzer.cpp -o bin/test_root_cause.exe $LIBS

Write-Host "Compiling test_fault_detector..." -ForegroundColor Yellow
& $CXX $FLAGS tests/test_fault_detector.cpp simulator/logging/LogEntry.cpp analyzer/graph/DependencyGraph.cpp analyzer/rootcause/RootCauseAnalyzer.cpp analyzer/detector/FaultDetector.cpp -o bin/test_fault_detector.exe $LIBS

# 2. Concurrency Benchmark
Write-Host "Compiling benchmark_concurrency..." -ForegroundColor Yellow
& $CXX $FLAGS tests/benchmark_concurrency.cpp simulator/logging/LogEntry.cpp -o bin/benchmark_concurrency.exe $LIBS

# 3. Main Engine App
Write-Host "Compiling engine_app..." -ForegroundColor Yellow
$engineSources = @("engine/EngineApp.cpp", "engine/HttpServer.cpp") + $simSources + $analyzerSources
& $CXX $FLAGS $engineSources -o bin/engine_app.exe $LIBS

Write-Host "All C++ targets successfully built in ./bin/" -ForegroundColor Green
