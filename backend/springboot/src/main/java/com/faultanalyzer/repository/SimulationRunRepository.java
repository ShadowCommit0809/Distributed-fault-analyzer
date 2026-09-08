package com.faultanalyzer.repository;

import com.faultanalyzer.model.SimulationRunEntity;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface SimulationRunRepository extends JpaRepository<SimulationRunEntity, String> {
    Optional<SimulationRunEntity> findFirstByOrderByStartTimeDesc();
}
