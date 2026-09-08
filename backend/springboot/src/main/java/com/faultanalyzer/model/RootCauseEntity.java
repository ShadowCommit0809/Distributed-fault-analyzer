package com.faultanalyzer.model;

import com.fasterxml.jackson.annotation.JsonIgnore;
import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.GeneratedValue;
import jakarta.persistence.GenerationType;
import jakarta.persistence.Id;
import jakarta.persistence.JoinColumn;
import jakarta.persistence.ManyToOne;
import jakarta.persistence.Table;
import java.time.LocalDateTime;

@Entity
@Table(name = "root_cause_analysis")
public class RootCauseEntity {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @ManyToOne
    @JoinColumn(name = "incident_id", nullable = false)
    @JsonIgnore
    private IncidentEntity incident;

    @Column(name = "service_name", nullable = false, length = 128)
    private String serviceName;

    @Column(name = "total_score", nullable = false)
    private double totalScore;

    @Column(name = "temporal_score", nullable = false)
    private double temporalScore;

    @Column(name = "dependency_score", nullable = false)
    private double dependencyScore;

    @Column(name = "frequency_score", nullable = false)
    private double frequencyScore;

    @Column(name = "severity_score", nullable = false)
    private double severityScore;

    @Column(name = "propagation_score", nullable = false)
    private double propagationScore;

    @Column(name = "is_root_cause", nullable = false)
    private boolean isRootCause;

    @Column(name = "analysis_timestamp")
    private LocalDateTime analysisTimestamp;

    public RootCauseEntity() {
        this.analysisTimestamp = LocalDateTime.now();
    }

    public RootCauseEntity(IncidentEntity incident, String serviceName, double totalScore,
                           double temporalScore, double dependencyScore, double frequencyScore,
                           double severityScore, double propagationScore, boolean isRootCause) {
        this.incident = incident;
        this.serviceName = serviceName;
        this.totalScore = totalScore;
        this.temporalScore = temporalScore;
        this.dependencyScore = dependencyScore;
        this.frequencyScore = frequencyScore;
        this.severityScore = severityScore;
        this.propagationScore = propagationScore;
        this.isRootCause = isRootCause;
        this.analysisTimestamp = LocalDateTime.now();
    }

    public Long getId() { return id; }
    public void setId(Long id) { this.id = id; }

    public IncidentEntity getIncident() { return incident; }
    public void setIncident(IncidentEntity incident) { this.incident = incident; }

    public String getServiceName() { return serviceName; }
    public void setServiceName(String serviceName) { this.serviceName = serviceName; }

    public double getTotalScore() { return totalScore; }
    public void setTotalScore(double totalScore) { this.totalScore = totalScore; }

    public double getTemporalScore() { return temporalScore; }
    public void setTemporalScore(double temporalScore) { this.temporalScore = temporalScore; }

    public double getDependencyScore() { return dependencyScore; }
    public void setDependencyScore(double dependencyScore) { this.dependencyScore = dependencyScore; }

    public double getFrequencyScore() { return frequencyScore; }
    public void setFrequencyScore(double frequencyScore) { this.frequencyScore = frequencyScore; }

    public double getSeverityScore() { return severityScore; }
    public void setSeverityScore(double severityScore) { this.severityScore = severityScore; }

    public double getPropagationScore() { return propagationScore; }
    public void setPropagationScore(double propagationScore) { this.propagationScore = propagationScore; }

    public boolean isRootCause() { return isRootCause; }
    public void setRootCause(boolean rootCause) { isRootCause = rootCause; }

    public LocalDateTime getAnalysisTimestamp() { return analysisTimestamp; }
    public void setAnalysisTimestamp(LocalDateTime analysisTimestamp) { this.analysisTimestamp = analysisTimestamp; }
}
