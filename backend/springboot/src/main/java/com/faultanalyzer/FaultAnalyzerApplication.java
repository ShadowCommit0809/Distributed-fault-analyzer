package com.faultanalyzer;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.scheduling.annotation.EnableScheduling;

/**
 * Intelligent Distributed System Fault Detection & Root-Cause Analyzer
 * Spring Boot Main Application Entrypoint
 */
@SpringBootApplication
@EnableScheduling
public class FaultAnalyzerApplication {

    public static void main(String[] args) {
        SpringApplication.run(FaultAnalyzerApplication.class, args);
    }
}
