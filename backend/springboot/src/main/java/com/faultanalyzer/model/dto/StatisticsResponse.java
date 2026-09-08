package com.faultanalyzer.model.dto;

public class StatisticsResponse {
    private int totalServices;
    private int healthyServices;
    private int warningServices;
    private int failedServices;
    private int activeIncidents;
    private int resolvedIncidents;
    private long totalLogsRecorded;
    private long totalErrorsRecorded;
    private double overallAvgResponseTimeMs;
    private String activeScenario;
    private boolean engineConnected;

    public StatisticsResponse() {}

    public int getTotalServices() { return totalServices; }
    public void setTotalServices(int totalServices) { this.totalServices = totalServices; }

    public int getHealthyServices() { return healthyServices; }
    public void setHealthyServices(int healthyServices) { this.healthyServices = healthyServices; }

    public int getWarningServices() { return warningServices; }
    public void setWarningServices(int warningServices) { this.warningServices = warningServices; }

    public int getFailedServices() { return failedServices; }
    public void setFailedServices(int failedServices) { this.failedServices = failedServices; }

    public int getActiveIncidents() { return activeIncidents; }
    public void setActiveIncidents(int activeIncidents) { this.activeIncidents = activeIncidents; }

    public int getResolvedIncidents() { return resolvedIncidents; }
    public void setResolvedIncidents(int resolvedIncidents) { this.resolvedIncidents = resolvedIncidents; }

    public long getTotalLogsRecorded() { return totalLogsRecorded; }
    public void setTotalLogsRecorded(long totalLogsRecorded) { this.totalLogsRecorded = totalLogsRecorded; }

    public long getTotalErrorsRecorded() { return totalErrorsRecorded; }
    public void setTotalErrorsRecorded(long totalErrorsRecorded) { this.totalErrorsRecorded = totalErrorsRecorded; }

    public double getOverallAvgResponseTimeMs() { return overallAvgResponseTimeMs; }
    public void setOverallAvgResponseTimeMs(double overallAvgResponseTimeMs) { this.overallAvgResponseTimeMs = overallAvgResponseTimeMs; }

    public String getActiveScenario() { return activeScenario; }
    public void setActiveScenario(String activeScenario) { this.activeScenario = activeScenario; }

    public boolean isEngineConnected() { return engineConnected; }
    public void setEngineConnected(boolean engineConnected) { this.engineConnected = engineConnected; }
}
