#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <numeric>
#include "../simulator/concurrency/ThreadSafeQueue.hpp"
#include "../simulator/logging/LogEntry.hpp"

// Simulates realistic parsing, validation & structured telemetry processing per log
void processSingleLog(const LogEntry& log, std::atomic<uint64_t>& dummyCounter) {
    // Realistic telemetry processing: string formatting, field extraction, checksum
    std::string serialized = log.toJson();
    uint64_t hash = 5381;
    for (char c : serialized) {
        hash = ((hash << 5) + hash) + c;
    }
    dummyCounter += (hash & 1);
}

int main() {
    std::cout << "===================================================================\n";
    std::cout << "  BENCHMARK: Single-Threaded vs Multi-Threaded Log Processing\n";
    std::cout << "===================================================================\n";

    const size_t TOTAL_LOGS = 120000;
    std::atomic<uint64_t> dummyCounter{0};

    // Prepare test dataset in memory
    std::vector<LogEntry> dataset;
    dataset.reserve(TOTAL_LOGS);
    for (size_t i = 0; i < TOTAL_LOGS; ++i) {
        std::string sName = (i % 6 == 0) ? "Database Service" :
                            (i % 6 == 1) ? "Order Service" :
                            (i % 6 == 2) ? "Payment Service" :
                            (i % 6 == 3) ? "Authentication Service" :
                            (i % 6 == 4) ? "Notification Service" : "API Gateway";
        LogSeverity sev = (i % 25 == 0) ? LogSeverity::ERROR : LogSeverity::INFO;
        EventType ev = (sev == LogSeverity::ERROR) ? EventType::DATABASE_TIMEOUT : EventType::REQUEST_COMPLETED;
        dataset.emplace_back(sName, sev, ev,
                             "Processing synthetic telemetry transaction batch item " + std::to_string(i),
                             "REQ-" + std::to_string(i), "TRACE-" + std::to_string(i % 5000), 45);
    }

    // -------------------------------------------------------------
    // Test 1: Single-Threaded Execution
    // -------------------------------------------------------------
    std::cout << "\n[1/2] Running Single-Threaded Log Processing (" << TOTAL_LOGS << " logs)...\n";
    dummyCounter = 0;
    auto startSingle = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < TOTAL_LOGS; ++i) {
        processSingleLog(dataset[i], dummyCounter);
    }

    auto endSingle = std::chrono::high_resolution_clock::now();
    double singleDurationMs = std::chrono::duration<double, std::milli>(endSingle - startSingle).count();
    double singleThroughput = (static_cast<double>(TOTAL_LOGS) / (singleDurationMs / 1000.0));

    std::cout << "  Completed in: " << std::fixed << std::setprecision(2) << singleDurationMs << " ms\n";
    std::cout << "  Throughput:   " << std::fixed << std::setprecision(0) << singleThroughput << " logs/sec\n";

    // -------------------------------------------------------------
    // Test 2: Multi-Threaded Concurrent Producer-Consumer Execution
    // 6 Producer Threads (representing the 6 microservices)
    // 3 Worker Consumer Threads
    // -------------------------------------------------------------
    std::cout << "\n[2/2] Running Multi-Threaded (6 Producers + 3 Consumers, " << TOTAL_LOGS << " logs)...\n";
    dummyCounter = 0;

    auto queue = std::make_shared<ThreadSafeQueue<LogEntry>>(50000);
    const size_t numProducers = 6;
    const size_t numConsumers = 3;
    const size_t logsPerProducer = TOTAL_LOGS / numProducers;

    std::atomic<bool> startFlag{false};
    std::atomic<bool> producersDone{false};
    std::atomic<uint64_t> totalPopped{0};

    auto startMulti = std::chrono::high_resolution_clock::now();

    // Spawn Producers
    std::vector<std::thread> producers;
    for (size_t p = 0; p < numProducers; ++p) {
        producers.emplace_back([p, &queue, &startFlag, &dataset, logsPerProducer]() {
            while (!startFlag.load()) {
                std::this_thread::yield();
            }
            size_t startIdx = p * logsPerProducer;
            size_t endIdx = startIdx + logsPerProducer;
            for (size_t i = startIdx; i < endIdx; ++i) {
                queue->push(dataset[i]);
            }
        });
    }

    // Spawn Consumers
    std::vector<std::thread> consumers;
    for (size_t c = 0; c < numConsumers; ++c) {
        consumers.emplace_back([&queue, &producersDone, &totalPopped, &dummyCounter]() {
            while (!producersDone.load() || !queue->empty()) {
                std::vector<LogEntry> batch = queue->popBatch(200, std::chrono::milliseconds(10));
                for (const auto& log : batch) {
                    processSingleLog(log, dummyCounter);
                    totalPopped++;
                }
            }
        });
    }

    startFlag.store(true);

    for (auto& t : producers) {
        t.join();
    }
    producersDone.store(true);

    for (auto& t : consumers) {
        t.join();
    }

    auto endMulti = std::chrono::high_resolution_clock::now();
    double multiDurationMs = std::chrono::duration<double, std::milli>(endMulti - startMulti).count();
    double multiThroughput = (static_cast<double>(totalPopped.load()) / (multiDurationMs / 1000.0));
    double speedup = multiThroughput / singleThroughput;

    std::cout << "  Completed in: " << std::fixed << std::setprecision(2) << multiDurationMs << " ms\n";
    std::cout << "  Throughput:   " << std::fixed << std::setprecision(0) << multiThroughput << " logs/sec\n";

    // -------------------------------------------------------------
    // Display Benchmark Summary Table
    // -------------------------------------------------------------
    std::cout << "\n===================================================================\n";
    std::cout << "                      BENCHMARK RESULTS TABLE                      \n";
    std::cout << "===================================================================\n";
    std::cout << "| Metric                   | Single-Threaded  | Multi-Threaded (6P/3C) |\n";
    std::cout << "|--------------------------|------------------|------------------------|\n";
    std::cout << "| Total Logs Processed     | " << std::setw(16) << TOTAL_LOGS << " | " << std::setw(22) << totalPopped.load() << " |\n";
    std::cout << "| Execution Time (ms)      | " << std::setw(13) << std::fixed << std::setprecision(2) << singleDurationMs << " ms | " << std::setw(19) << multiDurationMs << " ms |\n";
    std::cout << "| Throughput (logs/sec)    | " << std::setw(13) << std::fixed << std::setprecision(0) << singleThroughput << " /s | " << std::setw(19) << multiThroughput << " /s |\n";
    std::cout << "| Concurrency Speedup      |             1.00x| " << std::setw(21) << std::fixed << std::setprecision(2) << speedup << "x |\n";
    std::cout << "===================================================================\n";

    return 0;
}
