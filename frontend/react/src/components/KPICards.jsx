import React from 'react';

export default function KPICards({ stats }) {
  const s = stats || {
    totalServices: 6,
    healthyServices: 6,
    warningServices: 0,
    failedServices: 0,
    activeIncidents: 0,
    totalLogsRecorded: 0,
    totalErrorsRecorded: 0,
    overallAvgResponseTimeMs: 0
  };

  const cards = [
    {
      title: 'Total Services',
      value: s.totalServices || 6,
      sub: `${s.healthyServices || 0} Healthy`,
      color: 'var(--accent-cyan)',
      bg: 'rgba(6, 182, 212, 0.08)',
      border: 'rgba(6, 182, 212, 0.25)',
      icon: (
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
          <rect x="2" y="2" width="20" height="8" rx="2" ry="2"></rect>
          <rect x="2" y="14" width="20" height="8" rx="2" ry="2"></rect>
          <line x1="6" y1="6" x2="6.01" y2="6"></line>
          <line x1="6" y1="18" x2="6.01" y2="18"></line>
        </svg>
      )
    },
    {
      title: 'System Health',
      value: s.failedServices > 0 ? `${s.failedServices} Failed` : (s.warningServices > 0 ? `${s.warningServices} Warning` : 'Normal'),
      sub: s.failedServices > 0 ? 'Action Required' : 'All systems green',
      color: s.failedServices > 0 ? 'var(--status-critical)' : (s.warningServices > 0 ? 'var(--status-warning)' : 'var(--status-healthy)'),
      bg: s.failedServices > 0 ? 'rgba(239, 68, 68, 0.08)' : 'rgba(16, 185, 129, 0.08)',
      border: s.failedServices > 0 ? 'rgba(239, 68, 68, 0.25)' : 'rgba(16, 185, 129, 0.25)',
      icon: (
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
          <path d="M22 12h-4l-3 9L9 3l-3 9H2"></path>
        </svg>
      )
    },
    {
      title: 'Active Incidents',
      value: s.activeIncidents || 0,
      sub: s.activeIncidents > 0 ? 'Root Cause Identified' : 'No active incident',
      color: s.activeIncidents > 0 ? 'var(--accent-rose)' : 'var(--text-secondary)',
      bg: s.activeIncidents > 0 ? 'rgba(244, 63, 94, 0.12)' : 'rgba(255, 255, 255, 0.03)',
      border: s.activeIncidents > 0 ? 'rgba(244, 63, 94, 0.35)' : 'var(--border-color)',
      icon: (
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
          <circle cx="12" cy="12" r="10"></circle>
          <line x1="12" y1="8" x2="12" y2="12"></line>
          <line x1="12" y1="16" x2="12.01" y2="16"></line>
        </svg>
      )
    },
    {
      title: 'Avg Latency',
      value: `${Math.round(s.overallAvgResponseTimeMs || 0)} ms`,
      sub: 'Mean across graph',
      color: 'var(--accent-indigo)',
      bg: 'rgba(99, 102, 241, 0.08)',
      border: 'rgba(99, 102, 241, 0.25)',
      icon: (
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
          <circle cx="12" cy="12" r="10"></circle>
          <polyline points="12 6 12 12 16 14"></polyline>
        </svg>
      )
    },
    {
      title: 'Observed Logs',
      value: (s.totalLogsRecorded || 0).toLocaleString(),
      sub: 'Ingested via C++ Queue',
      color: 'var(--accent-blue)',
      bg: 'rgba(59, 130, 246, 0.08)',
      border: 'rgba(59, 130, 246, 0.25)',
      icon: (
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
          <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path>
          <polyline points="14 2 14 8 20 8"></polyline>
          <line x1="16" y1="13" x2="8" y2="13"></line>
          <line x1="16" y1="17" x2="8" y2="17"></line>
        </svg>
      )
    },
    {
      title: 'Total Faults / Errors',
      value: (s.totalErrorsRecorded || 0).toLocaleString(),
      sub: s.totalErrorsRecorded > 0 ? 'Fault rules triggered' : 'Zero errors recorded',
      color: s.totalErrorsRecorded > 0 ? 'var(--status-warning)' : 'var(--text-secondary)',
      bg: s.totalErrorsRecorded > 0 ? 'rgba(245, 158, 11, 0.08)' : 'rgba(255, 255, 255, 0.03)',
      border: s.totalErrorsRecorded > 0 ? 'rgba(245, 158, 11, 0.25)' : 'var(--border-color)',
      icon: (
        <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
          <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"></path>
          <line x1="12" y1="9" x2="12" y2="13"></line>
          <line x1="12" y1="17" x2="12.01" y2="17"></line>
        </svg>
      )
    }
  ];

  return (
    <div style={{
      display: 'grid',
      gridTemplateColumns: 'repeat(auto-fit, minmax(170px, 1fr))',
      gap: '14px',
      marginBottom: '20px'
    }}>
      {cards.map((c, i) => (
        <div
          key={i}
          className="glass-panel"
          style={{
            padding: '16px 18px',
            background: c.bg,
            borderColor: c.border,
            display: 'flex',
            flexDirection: 'column',
            justifyContent: 'space-between',
            position: 'relative',
            overflow: 'hidden'
          }}
        >
          <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginBottom: '8px' }}>
            <span style={{ fontSize: '12px', fontWeight: '500', color: 'var(--text-secondary)' }}>
              {c.title}
            </span>
            <span style={{ color: c.color }}>{c.icon}</span>
          </div>
          <div>
            <div style={{ fontSize: '24px', fontWeight: '800', color: c.color, letterSpacing: '-0.02em' }}>
              {c.value}
            </div>
            <div style={{ fontSize: '11px', color: 'var(--text-muted)', marginTop: '2px' }}>
              {c.sub}
            </div>
          </div>
        </div>
      ))}
    </div>
  );
}
