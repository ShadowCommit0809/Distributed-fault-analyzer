import React from 'react';

export default function ServiceHealthTable({ services, rootCauseService }) {
  const getBadgeClass = (status) => {
    switch (status) {
      case 'CRITICAL':
      case 'DOWN':
      case 'FAILED': return 'badge-critical';
      case 'WARNING': return 'badge-warning';
      default: return 'badge-healthy';
    }
  };

  return (
    <div className="glass-panel" style={{ padding: '20px' }}>
      <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '16px' }}>
        <div>
          <h3 style={{ fontSize: '15px', fontWeight: '700' }}>
            Microservices Telemetry & Performance
          </h3>
          <p style={{ fontSize: '12px', color: 'var(--text-secondary)' }}>
            Live metrics aggregated from multi-threaded log stream
          </p>
        </div>
      </div>

      <div style={{ overflowX: 'auto' }}>
        <table style={{ width: '100%', borderCollapse: 'collapse', textAlign: 'left', fontSize: '13px' }}>
          <thead>
            <tr style={{ borderBottom: '1px solid var(--border-color)', color: 'var(--text-muted)', fontSize: '11px', textTransform: 'uppercase', letterSpacing: '0.05em' }}>
              <th style={{ padding: '10px 12px' }}>Service</th>
              <th style={{ padding: '10px 12px' }}>Status</th>
              <th style={{ padding: '10px 12px' }}>Active Fault</th>
              <th style={{ padding: '10px 12px' }}>Avg Latency</th>
              <th style={{ padding: '10px 12px' }}>Total Reqs</th>
              <th style={{ padding: '10px 12px' }}>Errors</th>
              <th style={{ padding: '10px 12px' }}>Error Rate</th>
            </tr>
          </thead>
          <tbody>
            {(services || []).map((s) => {
              const isRootCause = (s.name === rootCauseService);
              return (
                <tr
                  key={s.id}
                  style={{
                    borderBottom: '1px solid rgba(255,255,255,0.04)',
                    background: isRootCause ? 'rgba(244, 63, 94, 0.06)' : 'transparent',
                    transition: 'background 0.15s ease'
                  }}
                >
                  <td style={{ padding: '12px 12px', fontWeight: '600' }}>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                      {isRootCause && (
                        <span style={{
                          width: '8px',
                          height: '8px',
                          borderRadius: '50%',
                          background: '#f43f5e',
                          boxShadow: '0 0 8px #f43f5e'
                        }} />
                      )}
                      <span>{s.name}</span>
                      {isRootCause && (
                        <span className="badge badge-rootcause" style={{ fontSize: '9px', padding: '1px 6px' }}>
                          ROOT CAUSE
                        </span>
                      )}
                    </div>
                    <div style={{ fontSize: '11px', color: 'var(--text-muted)', fontFamily: 'monospace' }}>
                      {s.id}
                    </div>
                  </td>
                  <td style={{ padding: '12px 12px' }}>
                    <span className={`badge ${getBadgeClass(s.status)}`}>
                      {s.status}
                    </span>
                  </td>
                  <td style={{ padding: '12px 12px' }}>
                    <span className="font-mono" style={{
                      fontSize: '11px',
                      color: s.currentFault !== 'NONE' ? '#f43f5e' : 'var(--text-muted)',
                      fontWeight: s.currentFault !== 'NONE' ? '600' : '400'
                    }}>
                      {s.currentFault}
                    </span>
                  </td>
                  <td style={{ padding: '12px 12px' }}>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                      <span className="font-mono" style={{ minWidth: '46px', fontWeight: '600' }}>
                        {s.avgResponseTimeMs} ms
                      </span>
                      <div style={{ width: '60px', height: '4px', background: 'rgba(255,255,255,0.08)', borderRadius: '2px', overflow: 'hidden' }}>
                        <div
                          style={{
                            width: `${Math.min(100, Math.round((s.avgResponseTimeMs / 600) * 100))}%`,
                            height: '100%',
                            background: s.avgResponseTimeMs > 400 ? '#ef4444' : (s.avgResponseTimeMs > 200 ? '#f59e0b' : '#06b6d4')
                          }}
                        />
                      </div>
                    </div>
                  </td>
                  <td style={{ padding: '12px 12px' }} className="font-mono">
                    {s.totalRequests?.toLocaleString()}
                  </td>
                  <td style={{ padding: '12px 12px', color: s.totalErrors > 0 ? '#f87171' : 'var(--text-secondary)' }} className="font-mono">
                    {s.totalErrors?.toLocaleString()}
                  </td>
                  <td style={{ padding: '12px 12px' }}>
                    <span className="font-mono" style={{
                      fontWeight: '700',
                      color: s.errorRatePercent > 10 ? '#ef4444' : (s.errorRatePercent > 0 ? '#f59e0b' : 'var(--status-healthy)')
                    }}>
                      {s.errorRatePercent?.toFixed(1)}%
                    </span>
                  </td>
                </tr>
              );
            })}
          </tbody>
        </table>
      </div>
    </div>
  );
}
