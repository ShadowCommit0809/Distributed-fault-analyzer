package com.faultanalyzer.controller;

import com.faultanalyzer.model.dto.SimulationRequest;
import com.faultanalyzer.service.SimulationControlService;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
@RequestMapping("/api/simulation")
public class SimulationController {

    private final SimulationControlService simulationControlService;

    public SimulationController(SimulationControlService simulationControlService) {
        this.simulationControlService = simulationControlService;
    }

    @PostMapping("/start")
    public ResponseEntity<String> startSimulation(@RequestBody(required = false) SimulationRequest request) {
        String result = simulationControlService.startSimulation(request);
        return ResponseEntity.ok(result != null ? result : "{\"status\":\"STARTED\"}");
    }

    @PostMapping("/stop")
    public ResponseEntity<String> stopSimulation() {
        String result = simulationControlService.stopSimulation();
        return ResponseEntity.ok(result != null ? result : "{\"status\":\"STOPPED\"}");
    }

    @PostMapping("/reset")
    public ResponseEntity<String> resetSimulation() {
        String result = simulationControlService.resetSimulation();
        return ResponseEntity.ok(result != null ? result : "{\"status\":\"RESET\"}");
    }

    @GetMapping("/status")
    public ResponseEntity<Map<String, String>> getSimulationStatus() {
        return ResponseEntity.ok(Map.of("currentScenario", simulationControlService.getCurrentScenario()));
    }
}
