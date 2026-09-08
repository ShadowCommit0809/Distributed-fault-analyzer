package com.faultanalyzer.service;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.faultanalyzer.model.*;
import com.faultanalyzer.repository.*;
import jakarta.annotation.PostConstruct;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDateTime;
import java.util.*;

@Service
public class FaultAnalyzerService {

    private static final Logger logger = LoggerFactory.getLogger(FaultAnalyzerService.class);

    private final ServiceRepository serviceRepository;
    private final LogRepository logRepository;
    private final IncidentRepository incidentRepository;
    private final RootCauseRepository rootCauseRepository;
    private final CppEngineClient cppEngineClient;
    private final ObjectMapper objectMapper;

    private final Set<String> processedLogIds = Collections.synchronizedSet(new HashSet<>());

    public FaultAnalyzerService(ServiceRepository serviceRepository,
                                LogRepository logRepository,
                                IncidentRepository incidentRepository,
                                RootCauseRepository rootCauseRepository,
                                CppEngineClient cppEngineClient) {
        this.serviceRepository = serviceRepository;
        this.logRepository = logRepository;
        this.incidentRepository = incidentRepository;
        this.rootCauseRepository = rootCauseRepository;
        this.cppEngineClient = cppEngineClient;
        this.objectMapper = new ObjectMapper();
    }

    @PostConstruct
    public void initSeedData() {
        if (serviceRepository.count() == 0) {
            logger.info("Initializing baseline microservices in database...");
            List<ServiceEntity> baseline = List.of(
                new ServiceEntity("SRV-GATEWAY", "API Gateway", "HEALTHY", 120, 0, 0, 0.0, "NONE", "API Gateway for client requests"),
                new ServiceEntity("SRV-AUTH", "Authentication Service", "HEALTHY", 25, 0, 0, 0.0, "NONE", "JWT Authentication and session tokens"),
                new ServiceEntity("SRV-ORDER", "Order Service", "HEALTHY", 180, 0, 0, 0.0, "NONE", "Order lifecycle orchestration"),
                new ServiceEntity("SRV-PAYMENT", "Payment Service", "HEALTHY", 90, 0, 0, 0.0, "NONE", "Payment processing and card authorization"),
                new ServiceEntity("SRV-DB", "Database Service", "HEALTHY", 35, 0, 0, 0.0, "NONE", "Relational persistence and transaction pool"),
                new ServiceEntity("SRV-NOTIF", "Notification Service", "HEALTHY", 45, 0, 0, 0.0, "NONE", "Customer email and alert notifications")
            );
            serviceRepository.saveAll(baseline);
        }
    }

    @Scheduled(fixedRate = 1000)
    @Transactional
    public void pollAndSyncFromCppEngine() {
        if (!cppEngineClient.isHealthy()) {
            return;
        }

        try {
            // 1. Sync Services
            String servicesJson = cppEngineClient.getServices();
            if (servicesJson != null) {
                JsonNode array = objectMapper.readTree(servicesJson);
                if (array.isArray()) {
                    for (JsonNode sNode : array) {
                        String id = sNode.has("id") ? sNode.get("id").asText() : "";
                        String name = sNode.has("name") ? sNode.get("name").asText() : "";
                        String status = sNode.has("status") ? sNode.get("status").asText() : "HEALTHY";
                        int avgLatency = sNode.has("avgResponseTimeMs") ? sNode.get("avgResponseTimeMs").asInt() : 50;
                        long totalReq = sNode.has("totalRequests") ? sNode.get("totalRequests").asLong() : 0;
                        long totalErr = sNode.has("totalErrors") ? sNode.get("totalErrors").asLong() : 0;
                        double errRate = sNode.has("errorRatePercent") ? sNode.get("errorRatePercent").asDouble() : 0.0;
                        String fault = sNode.has("currentFault") ? sNode.get("currentFault").asText() : "NONE";

                        ServiceEntity entity = serviceRepository.findById(id).orElse(new ServiceEntity());
                        entity.setId(id);
                        entity.setName(name);
                        entity.setStatus(status);
                        entity.setAvgResponseTimeMs(avgLatency);
                        entity.setTotalRequests(totalReq);
                        entity.setTotalErrors(totalErr);
                        entity.setErrorRatePercent(errRate);
                        entity.setCurrentFault(fault);
                        entity.setUpdatedAt(LocalDateTime.now());
                        serviceRepository.save(entity);
                    }
                }
            }

            // 2. Sync Active Incident
            String incidentJson = cppEngineClient.getActiveIncident();
            if (incidentJson != null && !incidentJson.equals("null") && !incidentJson.trim().isEmpty()) {
                JsonNode incNode = objectMapper.readTree(incidentJson);
                if (incNode.has("id")) {
                    String incId = incNode.get("id").asText();
                    String title = incNode.has("title") ? incNode.get("title").asText() : "Distributed Incident";
                    String severity = incNode.has("severity") ? incNode.get("severity").asText() : "CRITICAL";
                    String status = incNode.has("status") ? incNode.get("status").asText() : "ACTIVE";
                    String startTime = incNode.has("startTime") ? incNode.get("startTime").asText() : "";
                    String rootCause = incNode.has("probableRootCause") ? incNode.get("probableRootCause").asText() : "Unknown";
                    double confidence = incNode.has("confidence") ? incNode.get("confidence").asDouble() : 0.0;
                    String explanation = incNode.has("explanation") ? incNode.get("explanation").asText() : "";

                    String evidenceJson = incNode.has("evidence") ? incNode.get("evidence").toString() : "[]";
                    String rankedJson = incNode.has("rankedCandidates") ? incNode.get("rankedCandidates").toString() : "[]";

                    IncidentEntity incident = incidentRepository.findById(incId).orElse(new IncidentEntity());
                    incident.setId(incId);
                    incident.setTitle(title);
                    incident.setSeverity(severity);
                    incident.setStatus(status);
                    incident.setStartTime(startTime);
                    incident.setProbableRootCause(rootCause);
                    incident.setConfidence(confidence);
                    incident.setExplanation(explanation);
                    incident.setEvidenceJson(evidenceJson);
                    incident.setRankedCandidatesJson(rankedJson);

                    // Sync affected services
                    if (incNode.has("affectedServices") && incNode.get("affectedServices").isArray()) {
                        incident.getAffectedServices().clear();
                        for (JsonNode sNameNode : incNode.get("affectedServices")) {
                            incident.addAffectedService(sNameNode.asText());
                        }
                    }

                    incidentRepository.save(incident);

                    // Sync Ranked Candidates RCA table
                    if (incNode.has("rankedCandidates") && incNode.get("rankedCandidates").isArray()) {
                        for (JsonNode cand : incNode.get("rankedCandidates")) {
                            String sName = cand.has("serviceName") ? cand.get("serviceName").asText() : "";
                            double totalScore = cand.has("totalScore") ? cand.get("totalScore").asDouble() : 0;
                            double tScore = cand.has("temporalScore") ? cand.get("temporalScore").asDouble() : 0;
                            double dScore = cand.has("dependencyScore") ? cand.get("dependencyScore").asDouble() : 0;
                            double fScore = cand.has("frequencyScore") ? cand.get("frequencyScore").asDouble() : 0;
                            double sScore = cand.has("severityScore") ? cand.get("severityScore").asDouble() : 0;
                            double pScore = cand.has("propagationScore") ? cand.get("propagationScore").asDouble() : 0;
                            boolean isRc = sName.equals(rootCause);

                            RootCauseEntity rca = new RootCauseEntity(incident, sName, totalScore,
                                    tScore, dScore, fScore, sScore, pScore, isRc);
                            rootCauseRepository.save(rca);
                        }
                    }
                }
            }

            // 3. Sync Recent Logs
            String logsJson = cppEngineClient.getLogs(50, null, null);
            if (logsJson != null) {
                JsonNode logsArray = objectMapper.readTree(logsJson);
                if (logsArray.isArray()) {
                    List<LogEntryEntity> batch = new ArrayList<>();
                    for (JsonNode lNode : logsArray) {
                        String logId = lNode.has("id") ? lNode.get("id").asText() : "";
                        if (!processedLogIds.contains(logId)) {
                            processedLogIds.add(logId);
                            String ts = lNode.has("timestamp") ? lNode.get("timestamp").asText() : "";
                            long epoch = lNode.has("epochMs") ? lNode.get("epochMs").asLong() : System.currentTimeMillis();
                            String sName = lNode.has("serviceName") ? lNode.get("serviceName").asText() : "";
                            String sev = lNode.has("severity") ? lNode.get("severity").asText() : "INFO";
                            String evt = lNode.has("eventType") ? lNode.get("eventType").asText() : "";
                            String msg = lNode.has("message") ? lNode.get("message").asText() : "";
                            String reqId = lNode.has("requestId") ? lNode.get("requestId").asText() : "";
                            String trId = lNode.has("traceId") ? lNode.get("traceId").asText() : "";
                            int resp = lNode.has("responseTimeMs") ? lNode.get("responseTimeMs").asInt() : 0;

                            batch.add(new LogEntryEntity(logId, ts, epoch, sName, sev, evt, msg, reqId, trId, resp));
                        }
                    }
                    if (!batch.isEmpty()) {
                        logRepository.saveAll(batch);
                    }
                }
            }

            // Keep in-memory cache pruned
            if (processedLogIds.size() > 5000) {
                processedLogIds.clear();
            }

        } catch (Exception e) {
            logger.debug("Error during sync with C++ engine: {}", e.getMessage());
        }
    }

    public List<ServiceEntity> getAllServices() {
        return serviceRepository.findAll();
    }

    public Optional<ServiceEntity> getServiceById(String id) {
        return serviceRepository.findById(id);
    }

    public List<IncidentEntity> getAllIncidents() {
        return incidentRepository.findAll();
    }

    public Optional<IncidentEntity> getActiveIncident() {
        return incidentRepository.findFirstByStatusOrderByCreatedAtDesc("ACTIVE");
    }

    public Optional<IncidentEntity> getIncidentById(String id) {
        return incidentRepository.findById(id);
    }

    public List<RootCauseEntity> getRootCausesForIncident(String incidentId) {
        return rootCauseRepository.findByIncidentIdOrderByTotalScoreDesc(incidentId);
    }
}
