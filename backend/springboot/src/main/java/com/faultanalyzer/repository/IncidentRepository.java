package com.faultanalyzer.repository;

import com.faultanalyzer.model.IncidentEntity;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;

@Repository
public interface IncidentRepository extends JpaRepository<IncidentEntity, String> {

    List<IncidentEntity> findByStatusOrderByCreatedAtDesc(String status);

    Optional<IncidentEntity> findFirstByStatusOrderByCreatedAtDesc(String status);

    long countByStatus(String status);
}
