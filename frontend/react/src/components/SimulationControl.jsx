import React, { useState } from 'react';

const SCENARIOS = [
  {
    id: 'NORMAL',
    name: 'Normal Traffic',
    desc: 'Healthy baseline requests with standard response times across all services.',
    badge: 'Baseline',
    color: '#10b981'
  },
  {
    id: 'DATABASE_FAILURE',
    name: 'Database Exhaustion',
    desc: 'Injects Connection Pool Exhaustion into Database Service. Cascades to Order & Payment.',
    badge: 'Cascading Fault',
    color: '#ef4444'
  },
  {
    id: 'AUTH_FAILURE',
    name: 'Auth Token Timeout',
    desc: 'Injects JWT Verification Failures into Authentication Service. Cascades to API Gateway.',
    badge: 'Security Fault',
    color: '#f59e0b'
  },
  {
    id: 'HIGH_LATENCY',
    name: 'Order Latency Spike',
    desc: 'Injects 800ms thread delay into Order Service. Triggers downstream timeouts.',
    badge: 'Degradation',
    color: '#a855f7'
  },
  {
    id: 'SERVICE_UNAVAILABLE',
    name: 'Payment Service Crash',
    desc: 'Total outage of Payment Service. Order checkout failures propagate to client.',
    badge: 'Crash Outage',
    color: '#f43f5e'
  },
  {
    id: 'CASCADING_FAILURE',
    name: 'Full Cascading Cascade',
    desc: 'Database connection drop triggers cascading queue saturation across all microservices.',
    badge: 'Full Stress Test',
    color: '#dc2626'
  }
];

export default function SimulationControl({ currentScenario, onStartScenario, onStopScenario, isProcessing }) {
  const [rps, setRps] = useState(4);

  return (
    <div className="glass-panel" style={{ padding: '20px' }}>
      <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '16px', flexWrap: 'wrap', gap: '10px' }}>
        <div>
          <h3 style={{ fontSize: '15px', fontWeight: '700' }}>
            Failure Simulation & Fault Injection Studio
          </h3>
          <p style={{ fontSize: '12px', color: 'var(--text-secondary)' }}>
            Trigger distributed failure scenarios and observe automated root cause scoring in real-time
          </p>
        </div>

        <div style={{ display: 'flex', alignItems: 'center', gap: '12px' }}>
          <div style={{ display: 'flex', alignItems: 'center', gap: '8px', fontSize: '12px' }}>
            <span style={{ color: 'var(--text-muted)' }}>Traffic Rate:</span>
            <select
              value={rps}
              onChange={(e) => setRps(Number(e.target.value))}
              style={{
                background: 'var(--bg-secondary)',
                color: 'var(--text-primary)',
                border: '1px solid var(--border-color)',
                borderRadius: '6px',
                padding: '4px 8px',
                fontSize: '12px'
              }}
            >
              <option value={2}>2 req/sec</option>
              <option value={4}>4 req/sec (Standard)</option>
              <option value={8}>8 req/sec (High Load)</option>
              <option value={15}>15 req/sec (Stress Test)</option>
            </select>
          </div>

          <button
            className="btn btn-danger"
            onClick={onStopScenario}
            disabled={isProcessing || currentScenario === 'STOPPED'}
            style={{ padding: '6px 14px', fontSize: '12px' }}
          >
            <svg width="12" height="12" viewBox="0 0 24 24" fill="currentColor">
              <rect x="3" y="3" width="18" height="18" rx="2" ry="2"></rect>
            </svg>
            Halt Simulation
          </button>
        </div>
      </div>

      {/* Scenario Selection Grid */}
      <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(260px, 1fr))', gap: '12px' }}>
        {SCENARIOS.map((sc) => {
          const isActive = (currentScenario === sc.id);
          return (
            <div
              key={sc.id}
              style={{
                background: isActive ? 'rgba(99, 102, 241, 0.12)' : 'var(--bg-secondary)',
                border: isActive ? '1px solid var(--accent-cyan)' : '1px solid var(--border-color)',
                borderRadius: '10px',
                padding: '14px 16px',
                display: 'flex',
                flexDirection: 'column',
                justifyContent: 'space-between',
                boxShadow: isActive ? '0 0 15px rgba(6, 182, 212, 0.2)' : 'none',
                transition: 'all 0.2s ease'
              }}
            >
              <div>
                <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '8px' }}>
                  <span style={{ fontSize: '13px', fontWeight: '700', color: '#fff' }}>
                    {sc.name}
                  </span>
                  <span style={{
                    fontSize: '10px',
                    fontWeight: '600',
                    padding: '2px 8px',
                    borderRadius: '9999px',
                    background: 'rgba(255,255,255,0.06)',
                    color: sc.color,
                    border: `1px solid ${sc.color}40`
                  }}>
                    {sc.badge}
                  </span>
                </div>
                <p style={{ fontSize: '12px', color: 'var(--text-secondary)', lineHeight: '1.4', marginBottom: '14px' }}>
                  {sc.desc}
                </p>
              </div>

              <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                <span style={{ fontSize: '11px', color: isActive ? 'var(--accent-cyan)' : 'var(--text-muted)', fontWeight: '600' }}>
                  {isActive ? '● ACTIVE RUN' : 'Ready'}
                </span>
                <button
                  className={`btn ${isActive ? 'btn-danger' : 'btn-primary'}`}
                  onClick={() => onStartScenario(sc.id, rps)}
                  disabled={isProcessing}
                  style={{ padding: '6px 12px', fontSize: '11px' }}
                >
                  {isActive ? 'Re-Inject' : 'Inject Fault'}
                </button>
              </div>
            </div>
          );
        })}
      </div>
    </div>
  );
}
