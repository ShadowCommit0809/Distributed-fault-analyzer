#ifndef SIMULATION_MANAGER_HPP
#define SIMULATION_MANAGER_HPP

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include "Scenario.hpp"
#include "../concurrency/ThreadSafeQueue.hpp"
#include "../logging/LogEntry.hpp"
#include "../services/Service.hpp"
#include "../services/APIGateway.hpp"
#include "../services/AuthService.hpp"
#include "../services/OrderService.hpp"
#include "../services/PaymentService.hpp"
#include "../services/DatabaseService.hpp"
#include "../services/NotificationService.hpp"

class SimulationManager {
private:
    std::shared_ptr<ThreadSafeQueue<LogEntry>> m_logQueue;

    std::shared_ptr<DatabaseService> m_databaseService;
    std::shared_ptr<PaymentService> m_paymentService;
    std::shared_ptr<OrderService> m_orderService;
    std::shared_ptr<AuthService> m_authService;
    std::shared_ptr<NotificationService> m_notificationService;
    std::shared_ptr<APIGateway> m_apiGateway;

    std::vector<std::shared_ptr<Service>> m_allServices;

    std::atomic<bool> m_isRunning{false};
    std::atomic<SimulationScenario> m_currentScenario{SimulationScenario::NORMAL};
    std::atomic<int> m_requestsPerSecond{4};

    std::thread m_simulationWorker;
    std::mutex m_managerMutex;

    void simulationLoop();

public:
    SimulationManager(std::shared_ptr<ThreadSafeQueue<LogEntry>> queue);
    ~SimulationManager();

    void startSimulation(SimulationScenario scenario = SimulationScenario::NORMAL, int rps = 4);
    void stopSimulation();
    void resetSimulation();

    SimulationScenario getCurrentScenario() const { return m_currentScenario.load(); }
    bool isRunning() const { return m_isRunning.load(); }

    std::vector<ServiceMetrics> getAllServiceMetrics() const;
    std::shared_ptr<Service> getServiceById(const std::string& id) const;

    std::shared_ptr<ThreadSafeQueue<LogEntry>> getLogQueue() const { return m_logQueue; }
};

#endif // SIMULATION_MANAGER_HPP
