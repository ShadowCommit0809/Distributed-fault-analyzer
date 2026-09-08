import React from 'react';

export default function Navbar({ engineConnected, activeScenario, onReset, isResetting }) {
  return (
    <header style={{
      display: 'flex',
      alignItems: 'center',
      justifyContent: 'space-between',
      padding: '16px 28px',
      borderBottom: '1px solid var(--border-color)',
      background: 'rgba(9, 13, 22, 0.85)',
      backdropFilter: 'blur(12px)',
      position: 'sticky',
      top: 0,
      zIndex: 100
    }}>
      <div style={{ display: 'flex', alignItems: 'center', gap: '14px' }}>
        <div style={{
          width: '38px',
          height: '38px',
          borderRadius: '10px',
          background: 'linear-gradient(135deg, #06b6d4, #4f46e5)',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          boxShadow: '0 0 20px rgba(6, 182, 212, 0.4)'
        }}>
          <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#fff" strokeWidth="2.2" strokeLinecap="round" strokeLinejoin="round">
            <polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"></polygon>
          </svg>
        </div>
        <div>
          <div style={{ display: 'flex', alignItems: 'center', gap: '10px' }}>
            <h1 style={{ fontSize: '18px', fontWeight: '700', letterSpacing: '-0.02em' }}>
              Fault<span style={{ color: 'var(--accent-cyan)' }}>Scope</span>
            </h1>
            <span style={{
              fontSize: '11px',
              fontWeight: '600',
              padding: '2px 8px',
              borderRadius: '6px',
              background: 'rgba(99, 102, 241, 0.15)',
              color: '#a5b4fc',
              border: '1px solid rgba(99, 102, 241, 0.3)'
            }}>
              C++ Core + Spring Boot
            </span>
          </div>
          <p style={{ fontSize: '12px', color: 'var(--text-secondary)' }}>
            Intelligent Distributed System Fault Detection & Root-Cause Observability
          </p>
        </div>
      </div>

      <div style={{ display: 'flex', alignItems: 'center', gap: '16px' }}>
        {/* Scenario badge */}
        <div style={{
          display: 'flex',
          alignItems: 'center',
          gap: '8px',
          background: 'var(--bg-secondary)',
          padding: '6px 12px',
          borderRadius: '8px',
          border: '1px solid var(--border-color)',
          fontSize: '12px'
        }}>
          <span style={{ color: 'var(--text-muted)' }}>Scenario:</span>
          <span style={{
            fontWeight: '600',
            color: activeScenario === 'NORMAL' ? 'var(--status-healthy)' : 'var(--status-warning)'
          }}>
            {activeScenario || 'NORMAL'}
          </span>
        </div>

        {/* C++ Engine Connection Badge */}
        <div style={{
          display: 'flex',
          alignItems: 'center',
          gap: '8px',
          background: 'var(--bg-secondary)',
          padding: '6px 12px',
          borderRadius: '8px',
          border: '1px solid var(--border-color)',
          fontSize: '12px'
        }}>
          <span style={{
            width: '8px',
            height: '8px',
            borderRadius: '50%',
            background: engineConnected ? 'var(--status-healthy)' : '#ef4444',
            boxShadow: engineConnected ? '0 0 10px #10b981' : '0 0 10px #ef4444'
          }} />
          <span style={{ color: 'var(--text-secondary)' }}>
            {engineConnected ? 'C++ Engine Connected' : 'C++ Engine Reconnecting'}
          </span>
        </div>

        {/* Reset Button */}
        <button
          className="btn btn-secondary"
          onClick={onReset}
          disabled={isResetting}
          style={{ padding: '6px 14px', fontSize: '12px' }}
        >
          <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2">
            <polyline points="1 4 1 10 7 10"></polyline>
            <path d="M3.51 15a9 9 0 1 0 2.13-9.36L1 10"></path>
          </svg>
          {isResetting ? 'Resetting...' : 'Reset System'}
        </button>
      </div>
    </header>
  );
}
