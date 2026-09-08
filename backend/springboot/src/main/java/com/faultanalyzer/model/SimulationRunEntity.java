package com.faultanalyzer.model;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.Id;
import jakarta.persistence.Table;
import java.time.LocalDateTime;

@Entity
@Table(name = "simulation_runs")
public class SimulationRunEntity {

    @Id
    @Column(name = "id", length = 64)
    private String id;

    @Column(name = "scenario_name", nullable = false, length = 64)
    private String scenarioName;

    @Column(name = "status", nullable = false, length = 32)
    private String status;

    @Column(name = "start_time", nullable = false)
    private LocalDateTime startTime;

    @Column(name = "end_time")
    private LocalDateTime endTime;

    @Column(name = "requests_per_second", nullable = false)
    private int requestsPerSecond;

    @Column(name = "total_events_generated", nullable = false)
    private long totalEventsGenerated;

    @Column(name = "incident_id", length = 64)
    private String incidentId;

    public SimulationRunEntity() {
        this.startTime = LocalDateTime.now();
        this.status = "RUNNING";
        this.requestsPerSecond = 4;
    }

    public SimulationRunEntity(String id, String scenarioName, String status, int requestsPerSecond) {
        this.id = id;
        this.scenarioName = scenarioName;
        this.status = status;
        this.requestsPerSecond = requestsPerSecond;
        this.startTime = LocalDateTime.now();
        this.totalEventsGenerated = 0;
    }

    public String getId() { return id; }
    public void setId(String id) { this.id = id; }

    public String getScenarioName() { return scenarioName; }
    public void setScenarioName(String scenarioName) { this.scenarioName = scenarioName; }

    public String getStatus() { return status; }
    public void setStatus(String status) { this.status = status; }

    public LocalDateTime getStartTime() { return startTime; }
    public void setStartTime(LocalDateTime startTime) { this.startTime = startTime; }

    public LocalDateTime getEndTime() { return endTime; }
    public void setEndTime(LocalDateTime endTime) { this.endTime = endTime; }

    public int getRequestsPerSecond() { return requestsPerSecond; }
    public void setRequestsPerSecond(int requestsPerSecond) { this.requestsPerSecond = requestsPerSecond; }

    public long getTotalEventsGenerated() { return totalEventsGenerated; }
    public void setTotalEventsGenerated(long totalEventsGenerated) { this.totalEventsGenerated = totalEventsGenerated; }

    public String getIncidentId() { return incidentId; }
    public void setIncidentId(String incidentId) { this.incidentId = incidentId; }
}
