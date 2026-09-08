-- =====================================================================
-- Distributed System Fault Detection & Root-Cause Analyzer
-- Production MySQL DDL & Initial Schema Seed
-- Compatible with MySQL 8.0+ and embedded H2 mode (PostgreSQL/MySQL modes)
-- =====================================================================

CREATE DATABASE IF NOT EXISTS `fault_analyzer_db` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE `fault_analyzer_db`;

-- ---------------------------------------------------------------------
-- 1. Services Table
-- ---------------------------------------------------------------------
DROP TABLE IF EXISTS `incident_services`;
DROP TABLE IF EXISTS `root_cause_analysis`;
DROP TABLE IF EXISTS `logs`;
DROP TABLE IF EXISTS `incidents`;
DROP TABLE IF EXISTS `simulation_runs`;
DROP TABLE IF EXISTS `services`;

CREATE TABLE `services` (
    `id` VARCHAR(64) NOT NULL PRIMARY KEY,
    `name` VARCHAR(128) NOT NULL,
    `status` VARCHAR(32) NOT NULL DEFAULT 'HEALTHY',
    `avg_response_time_ms` INT NOT NULL DEFAULT 50,
    `total_requests` BIGINT NOT NULL DEFAULT 0,
    `total_errors` BIGINT NOT NULL DEFAULT 0,
    `error_rate_percent` DOUBLE NOT NULL DEFAULT 0.0,
    `current_fault` VARCHAR(64) NOT NULL DEFAULT 'NONE',
    `description` VARCHAR(255) NULL,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------
-- 2. Logs Table
-- ---------------------------------------------------------------------
CREATE TABLE `logs` (
    `id` BIGINT AUTO_INCREMENT PRIMARY KEY,
    `log_id` VARCHAR(64) NOT NULL,
    `timestamp_str` VARCHAR(64) NOT NULL,
    `epoch_ms` BIGINT NOT NULL,
    `service_name` VARCHAR(128) NOT NULL,
    `severity` VARCHAR(32) NOT NULL,
    `event_type` VARCHAR(64) NOT NULL,
    `message` TEXT NOT NULL,
    `request_id` VARCHAR(64) NULL,
    `trace_id` VARCHAR(64) NULL,
    `response_time_ms` INT NOT NULL DEFAULT 0,
    INDEX `idx_logs_timestamp` (`epoch_ms`),
    INDEX `idx_logs_service` (`service_name`),
    INDEX `idx_logs_severity` (`severity`),
    INDEX `idx_logs_trace_id` (`trace_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------
-- 3. Incidents Table
-- ---------------------------------------------------------------------
CREATE TABLE `incidents` (
    `id` VARCHAR(64) NOT NULL PRIMARY KEY,
    `title` VARCHAR(255) NOT NULL,
    `severity` VARCHAR(32) NOT NULL,
    `status` VARCHAR(32) NOT NULL DEFAULT 'ACTIVE',
    `start_time` VARCHAR(64) NOT NULL,
    `resolved_time` VARCHAR(64) NULL,
    `probable_root_cause` VARCHAR(128) NOT NULL,
    `confidence` DOUBLE NOT NULL,
    `explanation` TEXT NULL,
    `evidence_json` TEXT NULL,
    `ranked_candidates_json` TEXT NULL,
    `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    INDEX `idx_incidents_status` (`status`),
    INDEX `idx_incidents_severity` (`severity`),
    INDEX `idx_incidents_root_cause` (`probable_root_cause`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------
-- 4. Incident Services Junction Table
-- ---------------------------------------------------------------------
CREATE TABLE `incident_services` (
    `id` BIGINT AUTO_INCREMENT PRIMARY KEY,
    `incident_id` VARCHAR(64) NOT NULL,
    `service_name` VARCHAR(128) NOT NULL,
    CONSTRAINT `fk_incident_services_incident` FOREIGN KEY (`incident_id`) REFERENCES `incidents` (`id`) ON DELETE CASCADE,
    INDEX `idx_inc_srv_name` (`service_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------
-- 5. Root Cause Analysis Details Table
-- ---------------------------------------------------------------------
CREATE TABLE `root_cause_analysis` (
    `id` BIGINT AUTO_INCREMENT PRIMARY KEY,
    `incident_id` VARCHAR(64) NOT NULL,
    `service_name` VARCHAR(128) NOT NULL,
    `total_score` DOUBLE NOT NULL,
    `temporal_score` DOUBLE NOT NULL,
    `dependency_score` DOUBLE NOT NULL,
    `frequency_score` DOUBLE NOT NULL,
    `severity_score` DOUBLE NOT NULL,
    `propagation_score` DOUBLE NOT NULL,
    `is_root_cause` BOOLEAN NOT NULL DEFAULT FALSE,
    `analysis_timestamp` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT `fk_rca_incident` FOREIGN KEY (`incident_id`) REFERENCES `incidents` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------
-- 6. Simulation Runs Table
-- ---------------------------------------------------------------------
CREATE TABLE `simulation_runs` (
    `id` VARCHAR(64) NOT NULL PRIMARY KEY,
    `scenario_name` VARCHAR(64) NOT NULL,
    `status` VARCHAR(32) NOT NULL,
    `start_time` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `end_time` TIMESTAMP NULL,
    `requests_per_second` INT NOT NULL DEFAULT 4,
    `total_events_generated` BIGINT NOT NULL DEFAULT 0,
    `incident_id` VARCHAR(64) NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ---------------------------------------------------------------------
-- Initial Seed: 6 Microservices in the Distributed Architecture
-- ---------------------------------------------------------------------
INSERT INTO `services` (`id`, `name`, `status`, `avg_response_time_ms`, `total_requests`, `total_errors`, `error_rate_percent`, `current_fault`, `description`) VALUES
('SRV-GATEWAY', 'API Gateway', 'HEALTHY', 120, 0, 0, 0.0, 'NONE', 'Entry point for external client HTTP REST requests, SSL termination & rate limiting'),
('SRV-AUTH', 'Authentication Service', 'HEALTHY', 25, 0, 0, 0.0, 'NONE', 'JWT validation, token issuing, and permission evaluation for incoming requests'),
('SRV-ORDER', 'Order Service', 'HEALTHY', 180, 0, 0, 0.0, 'NONE', 'Order lifecycle orchestration, order state transitions, and coordination with payment/DB'),
('SRV-PAYMENT', 'Payment Service', 'HEALTHY', 90, 0, 0, 0.0, 'NONE', 'Third-party payment gateway integration, transaction authorization and settlement'),
('SRV-DB', 'Database Service', 'HEALTHY', 35, 0, 0, 0.0, 'NONE', 'Relational database connection pool manager, persistence engine, and transaction logs'),
('SRV-NOTIF', 'Notification Service', 'HEALTHY', 45, 0, 0, 0.0, 'NONE', 'Asynchronous customer order confirmation emails, webhook delivery, and SMS alerts');
