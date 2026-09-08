package com.faultanalyzer.model.dto;

public class SimulationRequest {
    private String scenario;
    private Integer requestsPerSecond;

    public SimulationRequest() {}

    public SimulationRequest(String scenario, Integer requestsPerSecond) {
        this.scenario = scenario;
        this.requestsPerSecond = requestsPerSecond;
    }

    public String getScenario() { return scenario; }
    public void setScenario(String scenario) { this.scenario = scenario; }

    public Integer getRequestsPerSecond() { return requestsPerSecond; }
    public void setRequestsPerSecond(Integer requestsPerSecond) { this.requestsPerSecond = requestsPerSecond; }
}
