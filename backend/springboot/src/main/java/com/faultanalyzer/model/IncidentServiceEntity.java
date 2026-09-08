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

@Entity
@Table(name = "incident_services")
public class IncidentServiceEntity {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @ManyToOne
    @JoinColumn(name = "incident_id", nullable = false)
    @JsonIgnore
    private IncidentEntity incident;

    @Column(name = "service_name", nullable = false, length = 128)
    private String serviceName;

    public IncidentServiceEntity() {}

    public IncidentServiceEntity(IncidentEntity incident, String serviceName) {
        this.incident = incident;
        this.serviceName = serviceName;
    }

    public Long getId() { return id; }
    public void setId(Long id) { this.id = id; }

    public IncidentEntity getIncident() { return incident; }
    public void setIncident(IncidentEntity incident) { this.incident = incident; }

    public String getServiceName() { return serviceName; }
    public void setServiceName(String serviceName) { this.serviceName = serviceName; }
}
