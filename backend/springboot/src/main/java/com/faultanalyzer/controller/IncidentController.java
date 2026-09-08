package com.faultanalyzer.controller;

import com.faultanalyzer.model.IncidentEntity;
import com.faultanalyzer.service.FaultAnalyzerService;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/incidents")
public class IncidentController {

    private final FaultAnalyzerService faultAnalyzerService;

    public IncidentController(FaultAnalyzerService faultAnalyzerService) {
        this.faultAnalyzerService = faultAnalyzerService;
    }

    @GetMapping
    public ResponseEntity<List<IncidentEntity>> getAllIncidents() {
        return ResponseEntity.ok(faultAnalyzerService.getAllIncidents());
    }

    @GetMapping("/active")
    public ResponseEntity<?> getActiveIncident() {
        return faultAnalyzerService.getActiveIncident()
                .<ResponseEntity<?>>map(ResponseEntity::ok)
                .orElse(ResponseEntity.ok().body(null));
    }

    @GetMapping("/{id}")
    public ResponseEntity<IncidentEntity> getIncidentById(@PathVariable String id) {
        return faultAnalyzerService.getIncidentById(id)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }
}
