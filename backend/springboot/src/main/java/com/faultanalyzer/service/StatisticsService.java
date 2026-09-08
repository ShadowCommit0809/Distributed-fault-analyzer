package com.faultanalyzer.service;

import com.faultanalyzer.model.ServiceEntity;
import com.faultanalyzer.model.dto.StatisticsResponse;
import com.faultanalyzer.repository.IncidentRepository;
import com.faultanalyzer.repository.LogRepository;
import com.faultanalyzer.repository.ServiceRepository;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class StatisticsService {

    private final ServiceRepository serviceRepository;
    private final IncidentRepository incidentRepository;
    private final LogRepository logRepository;
    private final SimulationControlService simulationControlService;
    private final CppEngineClient cppEngineClient;

    public StatisticsService(ServiceRepository serviceRepository,
                             IncidentRepository incidentRepository,
                             LogRepository logRepository,
                             SimulationControlService simulationControlService,
                             CppEngineClient cppEngineClient) {
        this.serviceRepository = serviceRepository;
        this.incidentRepository = incidentRepository;
        this.logRepository = logRepository;
        this.simulationControlService = simulationControlService;
        this.cppEngineClient = cppEngineClient;
    }

    public StatisticsResponse getStatistics() {
        StatisticsResponse stats = new StatisticsResponse();

        List<ServiceEntity> services = serviceRepository.findAll();
        stats.setTotalServices(services.size());

        int healthy = 0;
        int warning = 0;
        int failed = 0;
        long totalReq = 0;
        long totalErr = 0;
        double latencySum = 0;

        for (ServiceEntity s : services) {
            switch (s.getStatus()) {
                case "HEALTHY" -> healthy++;
                case "WARNING" -> warning++;
                case "CRITICAL", "DOWN", "FAILED" -> failed++;
                default -> healthy++;
            }
            totalReq += s.getTotalRequests();
            totalErr += s.getTotalErrors();
            latencySum += s.getAvgResponseTimeMs();
        }

        stats.setHealthyServices(healthy);
        stats.setWarningServices(warning);
        stats.setFailedServices(failed);

        stats.setActiveIncidents((int) incidentRepository.countByStatus("ACTIVE"));
        stats.setResolvedIncidents((int) incidentRepository.countByStatus("RESOLVED"));

        stats.setTotalLogsRecorded(logRepository.count());
        stats.setTotalErrorsRecorded(totalErr);
        stats.setOverallAvgResponseTimeMs(services.isEmpty() ? 0.0 : Math.round((latencySum / services.size()) * 10.0) / 10.0);

        stats.setActiveScenario(simulationControlService.getCurrentScenario());
        stats.setEngineConnected(cppEngineClient.isHealthy());

        return stats;
    }
}
