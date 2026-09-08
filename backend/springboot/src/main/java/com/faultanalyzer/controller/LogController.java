package com.faultanalyzer.controller;

import com.faultanalyzer.model.LogEntryEntity;
import com.faultanalyzer.repository.LogRepository;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/logs")
public class LogController {

    private final LogRepository logRepository;

    public LogController(LogRepository logRepository) {
        this.logRepository = logRepository;
    }

    @GetMapping
    public ResponseEntity<List<LogEntryEntity>> getLogs(
            @RequestParam(required = false) String service,
            @RequestParam(required = false) String severity,
            @RequestParam(required = false) String traceId,
            @RequestParam(defaultValue = "100") int limit) {

        int safeLimit = Math.min(Math.max(limit, 1), 500);
        Pageable pageable = PageRequest.of(0, safeLimit);

        if ((service == null || service.isBlank()) &&
            (severity == null || severity.isBlank()) &&
            (traceId == null || traceId.isBlank())) {
            return ResponseEntity.ok(logRepository.findTop100ByOrderByEpochMsDesc());
        }

        List<LogEntryEntity> results = logRepository.searchLogs(service, severity, traceId, pageable);
        return ResponseEntity.ok(results);
    }
}
