#include "SimulationManager.hpp"
#include <random>
#include <chrono>

SimulationManager::SimulationManager(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue)
    : m_logQueue(queue) {
    // Instantiate all 6 microservices
    m_databaseService = std::make_shared<DatabaseService>(m_logQueue);
    m_paymentService = std::make_shared<PaymentService>(m_logQueue, m_databaseService);
    m_orderService = std::make_shared<OrderService>(m_logQueue, m_paymentService, m_databaseService);
    m_authService = std::make_shared<AuthService>(m_logQueue);
    m_notificationService = std::make_shared<NotificationService>(m_logQueue);
    m_apiGateway = std::make_shared<APIGateway>(m_logQueue, m_authService, m_orderService, m_notificationService);

    m_allServices = {
        m_apiGateway,
        m_authService,
        m_orderService,
        m_paymentService,
        m_databaseService,
        m_notificationService
    };
}

SimulationManager::~SimulationManager() {
    stopSimulation();
}

void SimulationManager::startSimulation(SimulationScenario scenario, int rps) {
    std::lock_guard<std::mutex> lock(m_managerMutex);

    stopSimulation(); // Stop any existing run first

    m_currentScenario = scenario;
    m_requestsPerSecond = (rps > 0 && rps <= 50) ? rps : 4;
    m_isRunning = true;

    // Reset faults first
    for (auto& s : m_allServices) {
        s->clearFault();
    }

    // Apply scenario fault injection
    switch (scenario) {
        case SimulationScenario::NORMAL:
            // All services remain healthy
            break;
        case SimulationScenario::DATABASE_FAILURE:
            m_databaseService->injectFault(FaultMode::DATABASE_DOWN);
            break;
        case SimulationScenario::AUTH_FAILURE:
            m_authService->injectFault(FaultMode::AUTH_REJECT);
            break;
        case SimulationScenario::HIGH_LATENCY:
            m_orderService->injectFault(FaultMode::HIGH_LATENCY);
            break;
        case SimulationScenario::SERVICE_UNAVAILABLE:
            m_paymentService->injectFault(FaultMode::SERVICE_CRASH);
            break;
        case SimulationScenario::CASCADING_FAILURE:
            m_databaseService->injectFault(FaultMode::DATABASE_DOWN);
            break;
    }

    m_simulationWorker = std::thread(&SimulationManager::simulationLoop, this);
}

void SimulationManager::stopSimulation() {
    m_isRunning = false;
    if (m_simulationWorker.joinable()) {
        m_simulationWorker.join();
    }
}

void SimulationManager::resetSimulation() {
    stopSimulation();
    std::lock_guard<std::mutex> lock(m_managerMutex);
    m_currentScenario = SimulationScenario::NORMAL;
    for (auto& s : m_allServices) {
        s->reset();
    }
}

void SimulationManager::simulationLoop() {
    uint64_t reqIndex = 1000;
    std::mt19937 rng(1337);
    std::uniform_real_distribution<double> amountDist(19.99, 499.99);

    int intervalMs = 1000 / (m_requestsPerSecond.load() > 0 ? m_requestsPerSecond.load() : 4);

    while (m_isRunning.load()) {
        reqIndex++;
        std::string reqId = "REQ-" + std::to_string(reqIndex);
        std::string traceId = "TRACE-" + std::to_string(5000 + (reqIndex % 9000));
        std::string orderId = "ORD-" + std::to_string(reqIndex);
        double amount = amountDist(rng);
        std::string authToken = (m_currentScenario == SimulationScenario::AUTH_FAILURE && (reqIndex % 3 == 0))
                                ? "INVALID_EXPIRED_TOKEN"
                                : "JWT_BEARER_VALID_TOKEN_" + std::to_string(reqIndex);

        // Execute request through API Gateway
        m_apiGateway->handleOrderRequest(authToken, orderId, amount,
                                         "customer" + std::to_string(reqIndex % 100) + "@example.com",
                                         reqId, traceId);

        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
    }
}

std::vector<ServiceMetrics> SimulationManager::getAllServiceMetrics() const {
    std::vector<ServiceMetrics> metrics;
    metrics.reserve(m_allServices.size());
    for (const auto& s : m_allServices) {
        metrics.push_back(s->getMetrics());
    }
    return metrics;
}

std::shared_ptr<Service> SimulationManager::getServiceById(const std::string& id) const {
    for (const auto& s : m_allServices) {
        if (s->getId() == id || s->getName() == id) {
            return s;
        }
    }
    return nullptr;
}
