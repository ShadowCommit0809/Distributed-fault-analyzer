package com.faultanalyzer.repository;

import com.faultanalyzer.model.RootCauseEntity;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface RootCauseRepository extends JpaRepository<RootCauseEntity, Long> {
    List<RootCauseEntity> findByIncidentIdOrderByTotalScoreDesc(String incidentId);
}
