import React, { useState } from 'react';

// Graph Node Layout Coordinates
const NODE_POSITIONS = {
  'API Gateway': { x: 80, y: 160, role: 'Edge Ingress' },
  'Authentication Service': { x: 300, y: 60, role: 'Security & Auth' },
  'Order Service': { x: 300, y: 260, role: 'Core Orchestrator' },
  'Payment Service': { x: 540, y: 180, role: 'Billing & Gateway' },
  'DatabaseService': { x: 760, y: 260, role: 'Data Persistence' },
  'Database Service': { x: 760, y: 260, role: 'Data Persistence' },
  'Notification Service': { x: 540, y: 350, role: 'Alerts & Messages' }
};

const EDGES = [
  { from: 'API Gateway', to: 'Authentication Service' },
  { from: 'API Gateway', to: 'Order Service' },
  { from: 'Order Service', to: 'Payment Service' },
  { from: 'Order Service', to: 'Database Service' },
  { from: 'Order Service', to: 'Notification Service' },
  { from: 'Payment Service', to: 'Database Service' }
];

export default function DependencyGraphView({ services, activeIncident }) {
  const [selectedService, setSelectedService] = useState(null);

  // Map services by name
  const serviceMap = {};
  (services || []).forEach(s => {
    serviceMap[s.name] = s;
    if (s.name === 'Database Service') serviceMap['DatabaseService'] = s;
  });

  const rootCauseName = activeIncident?.probableRootCause;

  const getNodeStatus = (name) => {
    const s = serviceMap[name];
    if (!s) return 'HEALTHY';
    return s.status || 'HEALTHY';
  };

  const getStatusColor = (status, isRootCause) => {
    if (isRootCause) return '#f43f5e';
    switch (status) {
      case 'CRITICAL':
      case 'DOWN':
      case 'FAILED': return '#ef4444';
      case 'WARNING': return '#f59e0b';
      default: return '#10b981';
    }
  };

  const selectedData = selectedService ? (serviceMap[selectedService] || null) : null;

  return (
    <div className="glass-panel" style={{ padding: '20px', position: 'relative' }}>
      <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '14px' }}>
        <div>
          <h3 style={{ fontSize: '15px', fontWeight: '700', letterSpacing: '-0.01em' }}>
            Distributed Service Dependency Topology (DAG)
          </h3>
          <p style={{ fontSize: '12px', color: 'var(--text-secondary)' }}>
            Real-time Directed Acyclic Graph displaying health states & failure propagation paths
          </p>
        </div>

        {/* Legend */}
        <div style={{ display: 'flex', alignItems: 'center', gap: '14px', fontSize: '11px', color: 'var(--text-secondary)' }}>
          <span style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
            <span style={{ width: '8px', height: '8px', borderRadius: '50%', background: 'var(--status-healthy)' }}></span>
            Healthy
          </span>
          <span style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
            <span style={{ width: '8px', height: '8px', borderRadius: '50%', background: 'var(--status-warning)' }}></span>
            Warning
          </span>
          <span style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
            <span style={{ width: '8px', height: '8px', borderRadius: '50%', background: 'var(--status-critical)' }}></span>
            Fault / Down
          </span>
          <span style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
            <span style={{ width: '8px', height: '8px', borderRadius: '50%', background: '#f43f5e', boxShadow: '0 0 8px #f43f5e' }}></span>
            Root Cause Origin
          </span>
        </div>
      </div>

      {/* SVG Canvas */}
      <div style={{ width: '100%', overflowX: 'auto', background: 'rgba(9, 13, 22, 0.6)', borderRadius: '10px', border: '1px solid var(--border-color)' }}>
        <svg viewBox="0 0 900 420" style={{ width: '100%', minWidth: '760px', height: 'auto', display: 'block' }}>
          <defs>
            {/* Arrowhead marker normal */}
            <marker id="arrow" viewBox="0 0 10 10" refX="22" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
              <path d="M 0 1 L 10 5 L 0 9 z" fill="#475569" />
            </marker>
            {/* Arrowhead marker propagating failure */}
            <marker id="arrow-critical" viewBox="0 0 10 10" refX="22" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
              <path d="M 0 1 L 10 5 L 0 9 z" fill="#ef4444" />
            </marker>
            {/* Pulsing filter for root cause */}
            <filter id="glow-rootcause" x="-20%" y="-20%" width="140%" height="140%">
              <feDropShadow dx="0" dy="0" stdDeviation="6" floodColor="#f43f5e" floodOpacity="0.8" />
            </filter>
          </defs>

          {/* Edges */}
          {EDGES.map((edge, i) => {
            const p1 = NODE_POSITIONS[edge.from];
            const p2 = NODE_POSITIONS[edge.to];
            if (!p1 || !p2) return null;

            const fromStatus = getNodeStatus(edge.from);
            const toStatus = getNodeStatus(edge.to);
            const isFailingPath = (fromStatus === 'CRITICAL' || toStatus === 'CRITICAL' || edge.from === rootCauseName || edge.to === rootCauseName);

            // Bezier curve path
            const dx = p2.x - p1.x;
            const dy = p2.y - p1.y;
            const cx1 = p1.x + dx * 0.4;
            const cy1 = p1.y;
            const cx2 = p1.x + dx * 0.6;
            const cy2 = p2.y;
            const d = `M ${p1.x} ${p1.y} C ${cx1} ${cy1}, ${cx2} ${cy2}, ${p2.x} ${p2.y}`;

            return (
              <g key={i}>
                <path
                  d={d}
                  fill="none"
                  stroke={isFailingPath ? '#ef4444' : '#334155'}
                  strokeWidth={isFailingPath ? '2.5' : '1.8'}
                  className={isFailingPath ? 'animated-propagation-line' : ''}
                  markerEnd={isFailingPath ? 'url(#arrow-critical)' : 'url(#arrow)'}
                />
              </g>
            );
          })}

          {/* Nodes */}
          {Object.entries(NODE_POSITIONS).filter(([k]) => k !== 'DatabaseService').map(([name, pos]) => {
            const sData = serviceMap[name] || {};
            const status = getNodeStatus(name);
            const isRootCause = (name === rootCauseName);
            const color = getStatusColor(status, isRootCause);
            const isSelected = selectedService === name;

            return (
              <g
                key={name}
                transform={`translate(${pos.x}, ${pos.y})`}
                onClick={() => setSelectedService(isSelected ? null : name)}
                style={{ cursor: 'pointer' }}
              >
                {/* Halo for Root Cause */}
                {isRootCause && (
                  <circle
                    r="44"
                    fill="none"
                    stroke="#f43f5e"
                    strokeWidth="2"
                    strokeDasharray="4 4"
                    style={{ animation: 'spin 12s linear infinite' }}
                  />
                )}

                {/* Node Outer Circle */}
                <circle
                  r="34"
                  fill="#0f172a"
                  stroke={color}
                  strokeWidth={isRootCause ? '3' : (isSelected ? '3' : '2')}
                  filter={isRootCause ? 'url(#glow-rootcause)' : 'none'}
                />

                {/* Node Inner Ring */}
                <circle
                  r="28"
                  fill="rgba(30, 41, 59, 0.7)"
                />

                {/* Service Icon or Indicator */}
                <circle
                  r="6"
                  fill={color}
                  cy="-10"
                />

                {/* Service Label */}
                <text
                  textAnchor="middle"
                  y="6"
                  fill="#f8fafc"
                  fontSize="11"
                  fontWeight="600"
                  fontFamily="Inter, sans-serif"
                >
                  {name.replace(' Service', '')}
                </text>

                {/* Role / Latency */}
                <text
                  textAnchor="middle"
                  y="18"
                  fill="var(--text-muted)"
                  fontSize="9"
                  fontFamily="JetBrains Mono, monospace"
                >
                  {sData.avgResponseTimeMs ? `${sData.avgResponseTimeMs}ms` : pos.role}
                </text>

                {/* Root Cause Banner */}
                {isRootCause && (
                  <g transform="translate(0, -42)">
                    <rect
                      x="-42"
                      y="-10"
                      width="84"
                      height="18"
                      rx="9"
                      fill="#f43f5e"
                      filter="drop-shadow(0 2px 4px rgba(0,0,0,0.4))"
                    />
                    <text
                      textAnchor="middle"
                      y="2"
                      fill="#ffffff"
                      fontSize="9"
                      fontWeight="800"
                      letterSpacing="0.05em"
                    >
                      ROOT CAUSE
                    </text>
                  </g>
                )}
              </g>
            );
          })}
        </svg>
      </div>

      {/* Selected Node Drawer / Info Card */}
      {selectedData && (
        <div style={{
          marginTop: '14px',
          background: 'var(--bg-secondary)',
          border: '1px solid var(--border-color)',
          borderRadius: '10px',
          padding: '14px 18px',
          display: 'flex',
          justifyContent: 'space-between',
          alignItems: 'center',
          flexWrap: 'wrap',
          gap: '12px'
        }}>
          <div>
            <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
              <h4 style={{ fontSize: '14px', fontWeight: '700' }}>{selectedData.name}</h4>
              <span className={`badge badge-${selectedData.status?.toLowerCase() || 'healthy'}`}>
                {selectedData.status || 'HEALTHY'}
              </span>
              {selectedData.name === rootCauseName && (
                <span className="badge badge-rootcause">Root Cause</span>
              )}
            </div>
            <div style={{ fontSize: '12px', color: 'var(--text-secondary)', marginTop: '4px' }}>
              Fault State: <span className="font-mono" style={{ color: selectedData.currentFault !== 'NONE' ? '#f43f5e' : 'var(--text-muted)' }}>{selectedData.currentFault || 'NONE'}</span>
            </div>
          </div>

          <div style={{ display: 'flex', gap: '18px', fontSize: '12px' }}>
            <div>
              <div style={{ color: 'var(--text-muted)' }}>Avg Latency</div>
              <div className="font-mono" style={{ fontSize: '15px', fontWeight: '700', color: 'var(--accent-cyan)' }}>
                {selectedData.avgResponseTimeMs} ms
              </div>
            </div>
            <div>
              <div style={{ color: 'var(--text-muted)' }}>Total Requests</div>
              <div className="font-mono" style={{ fontSize: '15px', fontWeight: '700' }}>
                {selectedData.totalRequests}
              </div>
            </div>
            <div>
              <div style={{ color: 'var(--text-muted)' }}>Error Rate</div>
              <div className="font-mono" style={{ fontSize: '15px', fontWeight: '700', color: selectedData.errorRatePercent > 0 ? '#ef4444' : 'var(--status-healthy)' }}>
                {selectedData.errorRatePercent?.toFixed(1)}%
              </div>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
