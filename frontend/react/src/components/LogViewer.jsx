import React, { useState } from 'react';

export default function LogViewer({ logs, services, onSelectTrace, activeTraceFilter }) {
  const [selectedService, setSelectedService] = useState('');
  const [selectedSeverity, setSelectedSeverity] = useState('');
  const [searchTerm, setSearchTerm] = useState('');

  const getSeverityStyle = (sev) => {
    switch (sev) {
      case 'CRITICAL': return { color: '#f87171', bg: 'rgba(239, 68, 68, 0.15)', border: 'rgba(239, 68, 68, 0.3)' };
      case 'ERROR': return { color: '#fb7185', bg: 'rgba(244, 63, 94, 0.15)', border: 'rgba(244, 63, 94, 0.3)' };
      case 'WARNING': return { color: '#fbbf24', bg: 'rgba(245, 158, 11, 0.15)', border: 'rgba(245, 158, 11, 0.3)' };
      default: return { color: '#38bdf8', bg: 'rgba(56, 189, 248, 0.1)', border: 'rgba(56, 189, 248, 0.25)' };
    }
  };

  const filteredLogs = (logs || []).filter(log => {
    if (selectedService && log.serviceName !== selectedService) return false;
    if (selectedSeverity && log.severity !== selectedSeverity) return false;
    if (activeTraceFilter && log.traceId !== activeTraceFilter) return false;
    if (searchTerm) {
      const q = searchTerm.toLowerCase();
      return (
        (log.message && log.message.toLowerCase().includes(q)) ||
        (log.eventType && log.eventType.toLowerCase().includes(q)) ||
        (log.serviceName && log.serviceName.toLowerCase().includes(q)) ||
        (log.traceId && log.traceId.toLowerCase().includes(q))
      );
    }
    return true;
  });

  return (
    <div className="glass-panel" style={{ padding: '20px' }}>
      <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '16px', flexWrap: 'wrap', gap: '10px' }}>
        <div>
          <h3 style={{ fontSize: '15px', fontWeight: '700' }}>
            Distributed Log Ingestion & Stream Inspection
          </h3>
          <p style={{ fontSize: '12px', color: 'var(--text-secondary)' }}>
            Real-time multi-threaded logs consumed by C++ FaultDetector sliding window
          </p>
        </div>

        {activeTraceFilter && (
          <div style={{ display: 'flex', alignItems: 'center', gap: '8px', background: 'rgba(99, 102, 241, 0.15)', border: '1px solid rgba(99, 102, 241, 0.4)', borderRadius: '6px', padding: '4px 10px', fontSize: '12px' }}>
            <span style={{ color: 'var(--text-secondary)' }}>Filtered Trace:</span>
            <span className="font-mono" style={{ color: '#a5b4fc', fontWeight: '600' }}>{activeTraceFilter}</span>
            <button
              onClick={() => onSelectTrace(null)}
              style={{ background: 'none', border: 'none', color: '#fff', cursor: 'pointer', marginLeft: '4px', fontWeight: 'bold' }}
            >
              ×
            </button>
          </div>
        )}
      </div>

      {/* Filters Toolbar */}
      <div style={{ display: 'flex', gap: '10px', marginBottom: '14px', flexWrap: 'wrap' }}>
        {/* Service filter */}
        <select
          value={selectedService}
          onChange={(e) => setSelectedService(e.target.value)}
          style={{
            background: 'var(--bg-secondary)',
            color: 'var(--text-primary)',
            border: '1px solid var(--border-color)',
            borderRadius: '6px',
            padding: '6px 12px',
            fontSize: '12px'
          }}
        >
          <option value="">All Services</option>
          {(services || []).map(s => (
            <option key={s.id} value={s.name}>{s.name}</option>
          ))}
        </select>

        {/* Severity filter */}
        <select
          value={selectedSeverity}
          onChange={(e) => setSelectedSeverity(e.target.value)}
          style={{
            background: 'var(--bg-secondary)',
            color: 'var(--text-primary)',
            border: '1px solid var(--border-color)',
            borderRadius: '6px',
            padding: '6px 12px',
            fontSize: '12px'
          }}
        >
          <option value="">All Severities</option>
          <option value="INFO">INFO</option>
          <option value="WARNING">WARNING</option>
          <option value="ERROR">ERROR</option>
          <option value="CRITICAL">CRITICAL</option>
        </select>

        {/* Search input */}
        <input
          type="text"
          placeholder="Search message, event type, or trace ID..."
          value={searchTerm}
          onChange={(e) => setSearchTerm(e.target.value)}
          style={{
            flex: 1,
            minWidth: '220px',
            background: 'var(--bg-secondary)',
            color: 'var(--text-primary)',
            border: '1px solid var(--border-color)',
            borderRadius: '6px',
            padding: '6px 12px',
            fontSize: '12px'
          }}
        />

        {(selectedService || selectedSeverity || searchTerm || activeTraceFilter) && (
          <button
            className="btn btn-secondary"
            onClick={() => {
              setSelectedService('');
              setSelectedSeverity('');
              setSearchTerm('');
              onSelectTrace(null);
            }}
            style={{ padding: '6px 12px', fontSize: '11px' }}
          >
            Clear Filters
          </button>
        )}
      </div>

      {/* Logs Table */}
      <div style={{ maxHeight: '420px', overflowY: 'auto', border: '1px solid var(--border-color)', borderRadius: '8px', background: 'rgba(9, 13, 22, 0.4)' }}>
        <table style={{ width: '100%', borderCollapse: 'collapse', textAlign: 'left', fontSize: '12px' }}>
          <thead>
            <tr style={{ position: 'sticky', top: 0, background: 'var(--bg-secondary)', borderBottom: '1px solid var(--border-color)', color: 'var(--text-muted)', fontSize: '11px', textTransform: 'uppercase', letterSpacing: '0.05em' }}>
              <th style={{ padding: '8px 10px' }}>Timestamp</th>
              <th style={{ padding: '8px 10px' }}>Service</th>
              <th style={{ padding: '8px 10px' }}>Severity</th>
              <th style={{ padding: '8px 10px' }}>Event Type</th>
              <th style={{ padding: '8px 10px' }}>Message</th>
              <th style={{ padding: '8px 10px' }}>Trace ID</th>
              <th style={{ padding: '8px 10px' }}>Latency</th>
            </tr>
          </thead>
          <tbody>
            {filteredLogs.slice(0, 100).map((log, idx) => {
              const sev = getSeverityStyle(log.severity);
              return (
                <tr
                  key={log.id || log.logId || idx}
                  style={{
                    borderBottom: '1px solid rgba(255,255,255,0.03)',
                    background: log.severity === 'CRITICAL' ? 'rgba(239, 68, 68, 0.05)' : 'transparent'
                  }}
                >
                  <td style={{ padding: '8px 10px', color: 'var(--text-muted)', whiteSpace: 'nowrap' }} className="font-mono">
                    {log.timestamp ? log.timestamp.split('T')[1]?.slice(0, 12) || log.timestamp : '—'}
                  </td>
                  <td style={{ padding: '8px 10px', fontWeight: '600', whiteSpace: 'nowrap' }}>
                    {log.serviceName}
                  </td>
                  <td style={{ padding: '8px 10px' }}>
                    <span style={{
                      display: 'inline-block',
                      padding: '2px 8px',
                      borderRadius: '4px',
                      fontSize: '10px',
                      fontWeight: '700',
                      background: sev.bg,
                      color: sev.color,
                      border: `1px solid ${sev.border}`
                    }}>
                      {log.severity}
                    </span>
                  </td>
                  <td style={{ padding: '8px 10px', color: 'var(--text-secondary)' }} className="font-mono">
                    {log.eventType}
                  </td>
                  <td style={{ padding: '8px 10px', color: '#cbd5e1', maxWidth: '340px', overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}>
                    {log.message}
                  </td>
                  <td style={{ padding: '8px 10px' }}>
                    {log.traceId ? (
                      <button
                        onClick={() => onSelectTrace(log.traceId)}
                        className="font-mono"
                        style={{
                          background: 'none',
                          border: 'none',
                          color: 'var(--accent-cyan)',
                          cursor: 'pointer',
                          textDecoration: 'underline',
                          fontSize: '11px',
                          padding: 0
                        }}
                        title="Click to filter by this distributed trace ID"
                      >
                        {log.traceId.slice(0, 8)}...
                      </button>
                    ) : '—'}
                  </td>
                  <td style={{ padding: '8px 10px', color: 'var(--text-muted)' }} className="font-mono">
                    {log.responseTimeMs > 0 ? `${log.responseTimeMs}ms` : '—'}
                  </td>
                </tr>
              );
            })}
            {filteredLogs.length === 0 && (
              <tr>
                <td colSpan="7" style={{ padding: '30px', textAlign: 'center', color: 'var(--text-muted)' }}>
                  No logs matching the current filter criteria.
                </td>
              </tr>
            )}
          </tbody>
        </table>
      </div>
    </div>
  );
}
