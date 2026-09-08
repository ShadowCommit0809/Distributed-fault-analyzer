package com.faultanalyzer.controller;

import com.faultanalyzer.model.RootCauseEntity;
import com.faultanalyzer.service.FaultAnalyzerService;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/root-causes")
public class RootCauseController {

    private final FaultAnalyzerService faultAnalyzerService;

    public RootCauseController(FaultAnalyzerService faultAnalyzerService) {
        this.faultAnalyzerService = faultAnalyzerService;
    }

    @GetMapping("/{incidentId}")
    public ResponseEntity<List<RootCauseEntity>> getRootCausesForIncident(@PathVariable String incidentId) {
        return ResponseEntity.ok(faultAnalyzerService.getRootCausesForIncident(incidentId));
    }
}
