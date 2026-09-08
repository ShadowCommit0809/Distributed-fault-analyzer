import React from 'react';

export default function IncidentDetailsCard({ incident, services }) {
  if (!incident) {
    return (
      <div className="glass-panel" style={{ padding: '24px', textAlign: 'center' }}>
        <div style={{
          width: '54px',
          height: '54px',
          borderRadius: '50%',
          background: 'rgba(16, 185, 129, 0.1)',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          margin: '0 auto 12px auto',
          color: 'var(--status-healthy)',
          border: '1px solid rgba(16, 185, 129, 0.25)'
        }}>
          <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.2">
            <path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"></path>
            <polyline points="22 4 12 14.01 9 11.01"></polyline>
          </svg>
        </div>
        <h3 style={{ fontSize: '16px', fontWeight: '700', marginBottom: '6px' }}>
          All Distributed Microservices Healthy
        </h3>
        <p style={{ fontSize: '13px', color: 'var(--text-secondary)', maxWidth: '460px', margin: '0 auto' }}>
          No anomalies or cascading faults detected. Sliding-window fault detector is continuously monitoring all service logs.
        </p>
      </div>
    );
  }

  // Parse evidence and ranked candidates
  let evidenceList = [];
  try {
    evidenceList = typeof incident.evidenceJson === 'string'
      ? JSON.parse(incident.evidenceJson)
      : (incident.evidence || []);
  } catch (e) {
    evidenceList = [];
  }

  let rankedCandidates = [];
  try {
    rankedCandidates = typeof incident.rankedCandidatesJson === 'string'
      ? JSON.parse(incident.rankedCandidatesJson)
      : (incident.rankedCandidates || []);
  } catch (e) {
    rankedCandidates = [];
  }

  const confidencePct = Math.round((incident.confidence || 0.85) * 100);
  const rootCauseService = rankedCandidates.find(c => c.serviceName === incident.probableRootCause) || rankedCandidates[0];

  return (
    <div className="glass-panel pulse-critical" style={{ padding: '24px', borderColor: 'rgba(239, 68, 68, 0.4)' }}>
      {/* Incident Header */}
      <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start', flexWrap: 'wrap', gap: '12px', marginBottom: '18px' }}>
        <div>
          <div style={{ display: 'flex', alignItems: 'center', gap: '10px', marginBottom: '6px' }}>
            <span className="badge badge-critical">ACTIVE INCIDENT</span>
            <span style={{ fontSize: '12px', color: 'var(--text-muted)', fontFamily: 'monospace' }}>
              {incident.id}
            </span>
          </div>
          <h2 style={{ fontSize: '19px', fontWeight: '800', color: '#fff', letterSpacing: '-0.01em' }}>
            {incident.title}
          </h2>
          <div style={{ fontSize: '12px', color: 'var(--text-secondary)', marginTop: '4px' }}>
            Triggered at: <span className="font-mono">{incident.startTime || 'Recently'}</span>
          </div>
        </div>

        {/* Confidence Gauge */}
        <div style={{
          background: 'rgba(239, 68, 68, 0.1)',
          border: '1px solid rgba(239, 68, 68, 0.3)',
          borderRadius: '12px',
          padding: '10px 18px',
          textAlign: 'right'
        }}>
          <div style={{ fontSize: '11px', color: 'var(--text-secondary)', textTransform: 'uppercase', letterSpacing: '0.05em' }}>
            Algorithm Confidence
          </div>
          <div style={{ fontSize: '26px', fontWeight: '800', color: '#f87171' }}>
            {confidencePct}%
          </div>
        </div>
      </div>

      {/* Probable Root Cause Spotlight */}
      <div style={{
        background: 'linear-gradient(135deg, rgba(30, 41, 59, 0.9), rgba(15, 23, 42, 0.9))',
        border: '1px solid rgba(244, 63, 94, 0.4)',
        borderRadius: '12px',
        padding: '18px 20px',
        marginBottom: '20px',
        boxShadow: '0 0 25px rgba(244, 63, 94, 0.15)'
      }}>
        <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginBottom: '10px' }}>
          <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
            <span style={{
              display: 'inline-block',
              width: '10px',
              height: '10px',
              borderRadius: '50%',
              background: '#f43f5e',
              boxShadow: '0 0 10px #f43f5e'
            }} />
            <span style={{ fontSize: '13px', fontWeight: '700', textTransform: 'uppercase', letterSpacing: '0.05em', color: '#fda4af' }}>
              Identified Root Cause
            </span>
          </div>
          <span className="badge badge-rootcause">Rank #1 Primary Origin</span>
        </div>

        <div style={{ fontSize: '22px', fontWeight: '800', color: '#fff', marginBottom: '8px' }}>
          {incident.probableRootCause}
        </div>

        <p style={{ fontSize: '13px', color: '#cbd5e1', lineHeight: '1.6', marginBottom: '14px' }}>
          {incident.explanation || 'Primary failure originated here and propagated downstream across the dependency topology.'}
        </p>

        {/* 5-Factor Score Decomposition */}
        {rootCauseService && (
          <div>
            <div style={{ fontSize: '12px', fontWeight: '600', color: 'var(--text-secondary)', marginBottom: '8px' }}>
              Multi-Factor Root Cause Scoring Breakdown (Score: {rootCauseService.totalScore?.toFixed(3)}):
            </div>
            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(140px, 1fr))', gap: '10px' }}>
              {[
                { label: 'Temporal Priority (30%)', val: rootCauseService.temporalScore, color: 'var(--accent-cyan)' },
                { label: 'Dependency Rel (25%)', val: rootCauseService.dependencyScore, color: 'var(--accent-indigo)' },
                { label: 'Error Frequency (20%)', val: rootCauseService.frequencyScore, color: 'var(--accent-amber)' },
                { label: 'Severity Weight (15%)', val: rootCauseService.severityScore, color: 'var(--accent-rose)' },
                { label: 'Propagation Proof (10%)', val: rootCauseService.propagationScore, color: 'var(--accent-purple)' }
              ].map((f, idx) => (
                <div key={idx} style={{ background: 'rgba(0,0,0,0.25)', padding: '8px 10px', borderRadius: '8px', border: '1px solid var(--border-color)' }}>
                  <div style={{ fontSize: '10px', color: 'var(--text-muted)' }}>{f.label}</div>
                  <div style={{ fontSize: '15px', fontWeight: '700', color: f.color }}>
                    {(f.val || 0).toFixed(2)}
                  </div>
                  <div style={{ width: '100%', height: '4px', background: 'rgba(255,255,255,0.08)', borderRadius: '2px', marginTop: '4px', overflow: 'hidden' }}>
                    <div style={{ width: `${Math.min(100, Math.round((f.val || 0) * 100))}%`, height: '100%', background: f.color }} />
                  </div>
                </div>
              ))}
            </div>
          </div>
        )}
      </div>

      {/* Two Column Section: Ranked Suspects & Evidence Checklist */}
      <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(320px, 1fr))', gap: '18px' }}>
        {/* Ranked Candidates Table */}
        <div style={{ background: 'var(--bg-secondary)', padding: '16px', borderRadius: '10px', border: '1px solid var(--border-color)' }}>
          <h4 style={{ fontSize: '13px', fontWeight: '700', marginBottom: '12px', color: 'var(--text-primary)' }}>
            Ranked Service Suspects (Automated Scoring)
          </h4>
          <div style={{ display: 'flex', flexDirection: 'column', gap: '8px' }}>
            {rankedCandidates.map((cand, idx) => (
              <div
                key={idx}
                style={{
                  display: 'flex',
                  alignItems: 'center',
                  justifyContent: 'space-between',
                  padding: '8px 12px',
                  borderRadius: '6px',
                  background: cand.serviceName === incident.probableRootCause ? 'rgba(244, 63, 94, 0.12)' : 'rgba(255,255,255,0.02)',
                  border: cand.serviceName === incident.probableRootCause ? '1px solid rgba(244, 63, 94, 0.4)' : '1px solid transparent'
                }}
              >
                <div style={{ display: 'flex', alignItems: 'center', gap: '10px' }}>
                  <span style={{
                    width: '20px',
                    height: '20px',
                    borderRadius: '50%',
                    background: idx === 0 ? '#ef4444' : '#334155',
                    color: '#fff',
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'center',
                    fontSize: '11px',
                    fontWeight: '700'
                  }}>
                    {idx + 1}
                  </span>
                  <span style={{ fontSize: '13px', fontWeight: cand.serviceName === incident.probableRootCause ? '700' : '500' }}>
                    {cand.serviceName}
                  </span>
                </div>
                <div style={{ textAlign: 'right' }}>
                  <span style={{
                    fontSize: '13px',
                    fontWeight: '700',
                    color: cand.serviceName === incident.probableRootCause ? '#f43f5e' : 'var(--text-secondary)'
                  }}>
                    {(cand.totalScore || 0).toFixed(3)}
                  </span>
                  <span style={{ fontSize: '10px', color: 'var(--text-muted)', marginLeft: '6px' }}>score</span>
                </div>
              </div>
            ))}
          </div>
        </div>

        {/* Evidence Checklist */}
        <div style={{ background: 'var(--bg-secondary)', padding: '16px', borderRadius: '10px', border: '1px solid var(--border-color)' }}>
          <h4 style={{ fontSize: '13px', fontWeight: '700', marginBottom: '12px', color: 'var(--text-primary)' }}>
            Corroborating Telemetry Evidence
          </h4>
          <div style={{ display: 'flex', flexDirection: 'column', gap: '8px' }}>
            {evidenceList.map((ev, idx) => (
              <div
                key={idx}
                style={{
                  display: 'flex',
                  alignItems: 'flex-start',
                  gap: '10px',
                  padding: '8px 10px',
                  borderRadius: '6px',
                  background: 'rgba(0,0,0,0.2)',
                  border: '1px solid var(--border-color)'
                }}
              >
                <span style={{ color: 'var(--accent-cyan)', fontSize: '14px', marginTop: '1px' }}>✓</span>
                <div style={{ fontSize: '12px', color: 'var(--text-secondary)', lineHeight: '1.4' }}>
                  <span style={{ fontWeight: '600', color: 'var(--text-primary)' }}>
                    [{ev.serviceName || 'Cluster'}]:
                  </span>{' '}
                  {ev.description || ev.message || JSON.stringify(ev)}
                </div>
              </div>
            ))}
            {evidenceList.length === 0 && (
              <div style={{ fontSize: '12px', color: 'var(--text-muted)', fontStyle: 'italic' }}>
                Analyzing incoming stream evidence...
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}
