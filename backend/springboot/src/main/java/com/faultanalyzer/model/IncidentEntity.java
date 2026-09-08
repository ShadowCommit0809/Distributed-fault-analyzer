package com.faultanalyzer.model;

import jakarta.persistence.CascadeType;
import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.FetchType;
import jakarta.persistence.Id;
import jakarta.persistence.OneToMany;
import jakarta.persistence.Table;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;

@Entity
@Table(name = "incidents")
public class IncidentEntity {

    @Id
    @Column(name = "id", length = 64)
    private String id;

    @Column(name = "title", nullable = false)
    private String title;

    @Column(name = "severity", nullable = false, length = 32)
    private String severity;

    @Column(name = "status", nullable = false, length = 32)
    private String status;

    @Column(name = "start_time", nullable = false, length = 64)
    private String startTime;

    @Column(name = "resolved_time", length = 64)
    private String resolvedTime;

    @Column(name = "probable_root_cause", nullable = false, length = 128)
    private String probableRootCause;

    @Column(name = "confidence", nullable = false)
    private double confidence;

    @Column(name = "explanation", columnDefinition = "TEXT")
    private String explanation;

    @Column(name = "evidence_json", columnDefinition = "TEXT")
    private String evidenceJson;

    @Column(name = "ranked_candidates_json", columnDefinition = "TEXT")
    private String rankedCandidatesJson;

    @Column(name = "created_at")
    private LocalDateTime createdAt;

    @OneToMany(mappedBy = "incident", cascade = CascadeType.ALL, orphanRemoval = true, fetch = FetchType.EAGER)
    private List<IncidentServiceEntity> affectedServices = new ArrayList<>();

    public IncidentEntity() {
        this.status = "ACTIVE";
        this.createdAt = LocalDateTime.now();
    }

    public IncidentEntity(String id, String title, String severity, String status,
                          String startTime, String probableRootCause, double confidence,
                          String explanation, String evidenceJson, String rankedCandidatesJson) {
        this.id = id;
        this.title = title;
        this.severity = severity;
        this.status = status;
        this.startTime = startTime;
        this.probableRootCause = probableRootCause;
        this.confidence = confidence;
        this.explanation = explanation;
        this.evidenceJson = evidenceJson;
        this.rankedCandidatesJson = rankedCandidatesJson;
        this.createdAt = LocalDateTime.now();
    }

    public String getId() { return id; }
    public void setId(String id) { this.id = id; }

    public String getTitle() { return title; }
    public void setTitle(String title) { this.title = title; }

    public String getSeverity() { return severity; }
    public void setSeverity(String severity) { this.severity = severity; }

    public String getStatus() { return status; }
    public void setStatus(String status) { this.status = status; }

    public String getStartTime() { return startTime; }
    public void setStartTime(String startTime) { this.startTime = startTime; }

    public String getResolvedTime() { return resolvedTime; }
    public void setResolvedTime(String resolvedTime) { this.resolvedTime = resolvedTime; }

    public String getProbableRootCause() { return probableRootCause; }
    public void setProbableRootCause(String probableRootCause) { this.probableRootCause = probableRootCause; }

    public double getConfidence() { return confidence; }
    public void setConfidence(double confidence) { this.confidence = confidence; }

    public String getExplanation() { return explanation; }
    public void setExplanation(String explanation) { this.explanation = explanation; }

    public String getEvidenceJson() { return evidenceJson; }
    public void setEvidenceJson(String evidenceJson) { this.evidenceJson = evidenceJson; }

    public String getRankedCandidatesJson() { return rankedCandidatesJson; }
    public void setRankedCandidatesJson(String rankedCandidatesJson) { this.rankedCandidatesJson = rankedCandidatesJson; }

    public LocalDateTime getCreatedAt() { return createdAt; }
    public void setCreatedAt(LocalDateTime createdAt) { this.createdAt = createdAt; }

    public List<IncidentServiceEntity> getAffectedServices() { return affectedServices; }
    public void setAffectedServices(List<IncidentServiceEntity> affectedServices) { this.affectedServices = affectedServices; }

    public void addAffectedService(String serviceName) {
        IncidentServiceEntity ise = new IncidentServiceEntity(this, serviceName);
        this.affectedServices.add(ise);
    }
}
