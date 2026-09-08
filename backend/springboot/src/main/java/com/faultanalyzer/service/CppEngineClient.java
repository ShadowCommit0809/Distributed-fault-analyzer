package com.faultanalyzer.service;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.time.Duration;

@Component
public class CppEngineClient {

    private static final Logger logger = LoggerFactory.getLogger(CppEngineClient.class);

    private final String baseUrl;
    private final HttpClient httpClient;

    public CppEngineClient(@Value("${app.cpp-engine.base-url:http://localhost:8081}") String baseUrl) {
        this.baseUrl = baseUrl.endsWith("/") ? baseUrl.substring(0, baseUrl.length() - 1) : baseUrl;
        this.httpClient = HttpClient.newBuilder()
                .version(HttpClient.Version.HTTP_1_1)
                .connectTimeout(Duration.ofMillis(1500))
                .build();
    }

    public boolean isHealthy() {
        try {
            HttpRequest request = HttpRequest.newBuilder()
                    .uri(URI.create(baseUrl + "/health"))
                    .timeout(Duration.ofMillis(1500))
                    .GET()
                    .build();
            HttpResponse<String> response = httpClient.send(request, HttpResponse.BodyHandlers.ofString());
            return response.statusCode() == 200;
        } catch (Exception e) {
            return false;
        }
    }

    public String getServices() {
        return get(baseUrl + "/api/services");
    }

    public String getActiveIncident() {
        return get(baseUrl + "/api/incidents/active");
    }

    public String getAllIncidents() {
        return get(baseUrl + "/api/incidents");
    }

    public String getLogs(int limit, String service, String severity) {
        StringBuilder url = new StringBuilder(baseUrl + "/api/logs?limit=" + limit);
        if (service != null && !service.isEmpty()) {
            url.append("&service=").append(service);
        }
        if (severity != null && !severity.isEmpty()) {
            url.append("&severity=").append(severity);
        }
        return get(url.toString());
    }

    public String startSimulation(String scenario, int rps) {
        String json = "{\"scenario\":\"" + scenario + "\",\"rps\":" + rps + "}";
        return post(baseUrl + "/api/simulation/start", json);
    }

    public String stopSimulation() {
        return post(baseUrl + "/api/simulation/stop", "{}");
    }

    public String resetSimulation() {
        return post(baseUrl + "/api/simulation/reset", "{}");
    }

    private String get(String url) {
        try {
            HttpRequest request = HttpRequest.newBuilder()
                    .uri(URI.create(url))
                    .timeout(Duration.ofMillis(2000))
                    .GET()
                    .build();
            HttpResponse<String> response = httpClient.send(request, HttpResponse.BodyHandlers.ofString());
            if (response.statusCode() == 200) {
                return response.body();
            }
        } catch (Exception e) {
            logger.debug("Error connecting to C++ engine endpoint {}: {}", url, e.getMessage());
        }
        return null;
    }

    private String post(String url, String body) {
        try {
            HttpRequest request = HttpRequest.newBuilder()
                    .uri(URI.create(url))
                    .timeout(Duration.ofMillis(2000))
                    .header("Content-Type", "application/json")
                    .POST(HttpRequest.BodyPublishers.ofString(body))
                    .build();
            HttpResponse<String> response = httpClient.send(request, HttpResponse.BodyHandlers.ofString());
            if (response.statusCode() == 200) {
                return response.body();
            }
        } catch (Exception e) {
            logger.error("Error executing POST to C++ engine {}: {}", url, e.getMessage());
        }
        return null;
    }
}
