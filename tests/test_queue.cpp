#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <unordered_set>
#include <cassert>
#include "../simulator/concurrency/ThreadSafeQueue.hpp"
#include "../simulator/logging/LogEntry.hpp"

int main() {
    std::cout << "[Test: Concurrency & ThreadSafeQueue] Starting multi-producer multi-consumer test...\n";

    const size_t numProducers = 6;
    const size_t logsPerProducer = 5000;
    const size_t totalExpectedLogs = numProducers * logsPerProducer;

    auto queue = std::make_shared<ThreadSafeQueue<LogEntry>>(100000);

    std::atomic<bool> startFlag{false};
    std::atomic<size_t> totalPushed{0};
    std::atomic<size_t> totalPopped{0};

    // 1. Launch Producers
    std::vector<std::thread> producers;
    for (size_t p = 0; p < numProducers; ++p) {
        producers.emplace_back([p, &queue, &startFlag, &totalPushed, logsPerProducer]() {
            while (!startFlag.load()) {
                std::this_thread::yield();
            }
            std::string sName = "Service-" + std::to_string(p);
            for (size_t i = 0; i < logsPerProducer; ++i) {
                LogEntry entry(sName, LogSeverity::INFO, EventType::REQUEST_COMPLETED,
                               "Log item " + std::to_string(i),
                               "REQ-" + std::to_string(p * 100000 + i),
                               "TRACE-" + std::to_string(p * 100000 + i), 20);
                queue->push(std::move(entry));
                totalPushed++;
            }
        });
    }

    // 2. Launch Consumers
    std::atomic<bool> producersDone{false};
    std::vector<LogEntry> collectedLogs;
    std::mutex collectMutex;

    std::vector<std::thread> consumers;
    const size_t numConsumers = 3;
    for (size_t c = 0; c < numConsumers; ++c) {
        consumers.emplace_back([&queue, &producersDone, &collectedLogs, &collectMutex, &totalPopped]() {
            while (!producersDone.load() || !queue->empty()) {
                std::vector<LogEntry> batch = queue->popBatch(100, std::chrono::milliseconds(20));
                if (!batch.empty()) {
                    totalPopped += batch.size();
                    std::lock_guard<std::mutex> lock(collectMutex);
                    collectedLogs.insert(collectedLogs.end(), batch.begin(), batch.end());
                }
            }
        });
    }

    // Trigger start
    auto startTime = std::chrono::high_resolution_clock::now();
    startFlag.store(true);

    for (auto& t : producers) {
        t.join();
    }
    producersDone.store(true);

    for (auto& t : consumers) {
        t.join();
    }
    auto endTime = std::chrono::high_resolution_clock::now();

    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    std::cout << "  Producers: " << numProducers << ", Consumers: " << numConsumers << "\n";
    std::cout << "  Pushed: " << totalPushed.load() << " logs\n";
    std::cout << "  Popped: " << totalPopped.load() << " logs\n";
    std::cout << "  Duration: " << durationMs << " ms\n";
    double throughput = (static_cast<double>(totalPopped.load()) / (durationMs > 0 ? durationMs : 1)) * 1000.0;
    std::cout << "  Throughput: " << static_cast<uint64_t>(throughput) << " logs/sec\n";

    assert(totalPushed.load() == totalExpectedLogs);
    assert(totalPopped.load() == totalExpectedLogs);
    assert(collectedLogs.size() == totalExpectedLogs);

    // Verify uniqueness of IDs
    std::unordered_set<std::string> uniqueIds;
    for (const auto& log : collectedLogs) {
        uniqueIds.insert(log.id);
    }
    assert(uniqueIds.size() == totalExpectedLogs);
    std::cout << "  Zero log loss verified! Zero duplicate IDs verified!\n";
    std::cout << "[Test: Concurrency & ThreadSafeQueue] PASSED!\n";

    return 0;
}
