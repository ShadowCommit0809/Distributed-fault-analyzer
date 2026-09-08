package com.faultanalyzer.repository;

import com.faultanalyzer.model.LogEntryEntity;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface LogRepository extends JpaRepository<LogEntryEntity, Long> {

    List<LogEntryEntity> findTop100ByOrderByEpochMsDesc();

    Page<LogEntryEntity> findByOrderByEpochMsDesc(Pageable pageable);

    @Query("SELECT l FROM LogEntryEntity l WHERE " +
           "(:service IS NULL OR :service = '' OR l.serviceName = :service) AND " +
           "(:severity IS NULL OR :severity = '' OR l.severity = :severity) AND " +
           "(:traceId IS NULL OR :traceId = '' OR l.traceId = :traceId) " +
           "ORDER BY l.epochMs DESC")
    List<LogEntryEntity> searchLogs(@Param("service") String service,
                                   @Param("severity") String severity,
                                   @Param("traceId") String traceId,
                                   Pageable pageable);

    long countBySeverity(String severity);
}
