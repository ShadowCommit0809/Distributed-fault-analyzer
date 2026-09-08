package com.faultanalyzer.controller;

import com.faultanalyzer.model.ServiceEntity;
import com.faultanalyzer.service.FaultAnalyzerService;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/services")
public class ServiceController {

    private final FaultAnalyzerService faultAnalyzerService;

    public ServiceController(FaultAnalyzerService faultAnalyzerService) {
        this.faultAnalyzerService = faultAnalyzerService;
    }

    @GetMapping
    public ResponseEntity<List<ServiceEntity>> getAllServices() {
        return ResponseEntity.ok(faultAnalyzerService.getAllServices());
    }

    @GetMapping("/{id}")
    public ResponseEntity<ServiceEntity> getServiceById(@PathVariable String id) {
        return faultAnalyzerService.getServiceById(id)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }
}
