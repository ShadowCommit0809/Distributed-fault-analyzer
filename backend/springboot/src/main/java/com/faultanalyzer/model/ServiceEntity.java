package com.faultanalyzer.model;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.Id;
import jakarta.persistence.Table;
import java.time.LocalDateTime;

@Entity
@Table(name = "services")
public class ServiceEntity {

    @Id
    @Column(name = "id", length = 64)
    private String id;

    @Column(name = "name", nullable = false, length = 128)
    private String name;

    @Column(name = "status", nullable = false, length = 32)
    private String status;

    @Column(name = "avg_response_time_ms", nullable = false)
    private int avgResponseTimeMs;

    @Column(name = "total_requests", nullable = false)
    private long totalRequests;

    @Column(name = "total_errors", nullable = false)
    private long totalErrors;

    @Column(name = "error_rate_percent", nullable = false)
    private double errorRatePercent;

    @Column(name = "current_fault", nullable = false, length = 64)
    private String currentFault;

    @Column(name = "description", length = 255)
    private String description;

    @Column(name = "updated_at")
    private LocalDateTime updatedAt;

    public ServiceEntity() {
        this.status = "HEALTHY";
        this.currentFault = "NONE";
        this.updatedAt = LocalDateTime.now();
    }

    public ServiceEntity(String id, String name, String status, int avgResponseTimeMs,
                         long totalRequests, long totalErrors, double errorRatePercent,
                         String currentFault, String description) {
        this.id = id;
        this.name = name;
        this.status = status;
        this.avgResponseTimeMs = avgResponseTimeMs;
        this.totalRequests = totalRequests;
        this.totalErrors = totalErrors;
        this.errorRatePercent = errorRatePercent;
        this.currentFault = currentFault;
        this.description = description;
        this.updatedAt = LocalDateTime.now();
    }

    public String getId() { return id; }
    public void setId(String id) { this.id = id; }

    public String getName() { return name; }
    public void setName(String name) { this.name = name; }

    public String getStatus() { return status; }
    public void setStatus(String status) { this.status = status; }

    public int getAvgResponseTimeMs() { return avgResponseTimeMs; }
    public void setAvgResponseTimeMs(int avgResponseTimeMs) { this.avgResponseTimeMs = avgResponseTimeMs; }

    public long getTotalRequests() { return totalRequests; }
    public void setTotalRequests(long totalRequests) { this.totalRequests = totalRequests; }

    public long getTotalErrors() { return totalErrors; }
    public void setTotalErrors(long totalErrors) { this.totalErrors = totalErrors; }

    public double getErrorRatePercent() { return errorRatePercent; }
    public void setErrorRatePercent(double errorRatePercent) { this.errorRatePercent = errorRatePercent; }

    public String getCurrentFault() { return currentFault; }
    public void setCurrentFault(String currentFault) { this.currentFault = currentFault; }

    public String getDescription() { return description; }
    public void setDescription(String description) { this.description = description; }

    public LocalDateTime getUpdatedAt() { return updatedAt; }
    public void setUpdatedAt(LocalDateTime updatedAt) { this.updatedAt = updatedAt; }
}
