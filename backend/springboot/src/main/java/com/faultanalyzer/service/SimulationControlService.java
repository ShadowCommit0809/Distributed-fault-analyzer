package com.faultanalyzer.service;

import com.faultanalyzer.model.SimulationRunEntity;
import com.faultanalyzer.model.dto.SimulationRequest;
import com.faultanalyzer.repository.SimulationRunRepository;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.Optional;
import java.util.UUID;

@Service
public class SimulationControlService {

    private static final Logger logger = LoggerFactory.getLogger(SimulationControlService.class);

    private final CppEngineClient cppEngineClient;
    private final SimulationRunRepository simulationRunRepository;

    private String currentScenario = "NORMAL";

    public SimulationControlService(CppEngineClient cppEngineClient,
                                    SimulationRunRepository simulationRunRepository) {
        this.cppEngineClient = cppEngineClient;
        this.simulationRunRepository = simulationRunRepository;
    }

    public String startSimulation(SimulationRequest request) {
        String scenario = (request != null && request.getScenario() != null) ? request.getScenario() : "NORMAL";
        int rps = (request != null && request.getRequestsPerSecond() != null) ? request.getRequestsPerSecond() : 4;
        this.currentScenario = scenario;

        String runId = "RUN-" + UUID.randomUUID().toString().substring(0, 8).toUpperCase();
        SimulationRunEntity run = new SimulationRunEntity(runId, scenario, "RUNNING", rps);
        simulationRunRepository.save(run);

        if (cppEngineClient.isHealthy()) {
            return cppEngineClient.startSimulation(scenario, rps);
        } else {
            return "{\"status\":\"STARTED_LOCAL_FALLBACK\",\"scenario\":\"" + scenario + "\"}";
        }
    }

    public String stopSimulation() {
        this.currentScenario = "STOPPED";
        Optional<SimulationRunEntity> lastRun = simulationRunRepository.findFirstByOrderByStartTimeDesc();
        lastRun.ifPresent(run -> {
            run.setStatus("STOPPED");
            run.setEndTime(LocalDateTime.now());
            simulationRunRepository.save(run);
        });

        if (cppEngineClient.isHealthy()) {
            return cppEngineClient.stopSimulation();
        } else {
            return "{\"status\":\"STOPPED\"}";
        }
    }

    public String resetSimulation() {
        this.currentScenario = "NORMAL";
        Optional<SimulationRunEntity> lastRun = simulationRunRepository.findFirstByOrderByStartTimeDesc();
        lastRun.ifPresent(run -> {
            run.setStatus("RESET");
            run.setEndTime(LocalDateTime.now());
            simulationRunRepository.save(run);
        });

        if (cppEngineClient.isHealthy()) {
            return cppEngineClient.resetSimulation();
        } else {
            return "{\"status\":\"RESET\",\"scenario\":\"NORMAL\"}";
        }
    }

    public String getCurrentScenario() {
        return currentScenario;
    }
}
