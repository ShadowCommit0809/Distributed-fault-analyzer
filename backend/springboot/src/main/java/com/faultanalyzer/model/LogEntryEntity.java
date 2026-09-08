package com.faultanalyzer.model;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.GeneratedValue;
import jakarta.persistence.GenerationType;
import jakarta.persistence.Id;
import jakarta.persistence.Index;
import jakarta.persistence.Table;

@Entity
@Table(name = "logs", indexes = {
    @Index(name = "idx_logs_timestamp", columnList = "epoch_ms"),
    @Index(name = "idx_logs_service", columnList = "service_name"),
    @Index(name = "idx_logs_severity", columnList = "severity"),
    @Index(name = "idx_logs_trace_id", columnList = "trace_id")
})
public class LogEntryEntity {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @Column(name = "log_id", nullable = false, length = 64)
    private String logId;

    @Column(name = "timestamp_str", nullable = false, length = 64)
    private String timestamp;

    @Column(name = "epoch_ms", nullable = false)
    private long epochMs;

    @Column(name = "service_name", nullable = false, length = 128)
    private String serviceName;

    @Column(name = "severity", nullable = false, length = 32)
    private String severity;

    @Column(name = "event_type", nullable = false, length = 64)
    private String eventType;

    @Column(name = "message", columnDefinition = "TEXT", nullable = false)
    private String message;

    @Column(name = "request_id", length = 64)
    private String requestId;

    @Column(name = "trace_id", length = 64)
    private String traceId;

    @Column(name = "response_time_ms", nullable = false)
    private int responseTimeMs;

    public LogEntryEntity() {}

    public LogEntryEntity(String logId, String timestamp, long epochMs, String serviceName,
                          String severity, String eventType, String message,
                          String requestId, String traceId, int responseTimeMs) {
        this.logId = logId;
        this.timestamp = timestamp;
        this.epochMs = epochMs;
        this.serviceName = serviceName;
        this.severity = severity;
        this.eventType = eventType;
        this.message = message;
        this.requestId = requestId;
        this.traceId = traceId;
        this.responseTimeMs = responseTimeMs;
    }

    public Long getId() { return id; }
    public void setId(Long id) { this.id = id; }

    public String getLogId() { return logId; }
    public void setLogId(String logId) { this.logId = logId; }

    public String getTimestamp() { return timestamp; }
    public void setTimestamp(String timestamp) { this.timestamp = timestamp; }

    public long getEpochMs() { return epochMs; }
    public void setEpochMs(long epochMs) { this.epochMs = epochMs; }

    public String getServiceName() { return serviceName; }
    public void setServiceName(String serviceName) { this.serviceName = serviceName; }

    public String getSeverity() { return severity; }
    public void setSeverity(String severity) { this.severity = severity; }

    public String getEventType() { return eventType; }
    public void setEventType(String eventType) { this.eventType = eventType; }

    public String getMessage() { return message; }
    public void setMessage(String message) { this.message = message; }

    public String getRequestId() { return requestId; }
    public void setRequestId(String requestId) { this.requestId = requestId; }

    public String getTraceId() { return traceId; }
    public void setTraceId(String traceId) { this.traceId = traceId; }

    public int getResponseTimeMs() { return responseTimeMs; }
    public void setResponseTimeMs(int responseTimeMs) { this.responseTimeMs = responseTimeMs; }
}
