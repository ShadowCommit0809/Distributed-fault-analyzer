import React, { useState, useEffect } from 'react';
import Navbar from './components/Navbar';
import KPICards from './components/KPICards';
import DependencyGraphView from './components/DependencyGraphView';
import IncidentDetailsCard from './components/IncidentDetailsCard';
import SimulationControl from './components/SimulationControl';
import ServiceHealthTable from './components/ServiceHealthTable';
import LogViewer from './components/LogViewer';
import {
  fetchStatistics,
  fetchServices,
  fetchActiveIncident,
  fetchLogs,
  startSimulation,
  stopSimulation,
  resetSimulation
} from './services/api';

export default function App() {
  const [stats, setStats] = useState(null);
  const [services, setServices] = useState([]);
  const [activeIncident, setActiveIncident] = useState(null);
  const [logs, setLogs] = useState([]);
  const [activeTraceFilter, setActiveTraceFilter] = useState(null);
  const [isProcessing, setIsProcessing] = useState(false);
  const [activeTab, setActiveTab] = useState('overview'); // 'overview', 'services', 'logs'

  // Polling loop
  useEffect(() => {
    let isMounted = true;

    async function loadData() {
      try {
        const [statsData, servicesData, incidentData, logsData] = await Promise.all([
          fetchStatistics(),
          fetchServices(),
          fetchActiveIncident(),
          fetchLogs({ limit: 100, traceId: activeTraceFilter })
        ]);

        if (isMounted) {
          if (statsData) setStats(statsData);
          if (servicesData) setServices(servicesData);
          setActiveIncident(incidentData);
          if (logsData) setLogs(logsData);
        }
      } catch (err) {
        console.error('Error polling data:', err);
      }
    }

    loadData();
    const interval = setInterval(loadData, 1000);
    return () => {
      isMounted = false;
      clearInterval(interval);
    };
  }, [activeTraceFilter]);

  const handleStartScenario = async (scenario, rps) => {
    setIsProcessing(true);
    await startSimulation(scenario, rps);
    setIsProcessing(false);
  };

  const handleStopScenario = async () => {
    setIsProcessing(true);
    await stopSimulation();
    setIsProcessing(false);
  };

  const handleReset = async () => {
    setIsProcessing(true);
    await resetSimulation();
    setActiveTraceFilter(null);
    setIsProcessing(false);
  };

  const currentScenario = stats?.activeScenario || 'NORMAL';
  const rootCauseService = activeIncident?.probableRootCause;

  return (
    <div style={{ minHeight: '100vh', display: 'flex', flexDirection: 'column' }}>
      <Navbar
        engineConnected={stats?.engineConnected ?? false}
        activeScenario={currentScenario}
        onReset={handleReset}
        isResetting={isProcessing}
      />

      <main style={{ flex: 1, padding: '24px 28px', maxWidth: '1440px', margin: '0 auto', width: '100%' }}>
        {/* KPI Metrics */}
        <KPICards stats={stats} />

        {/* Navigation Tabs */}
        <div style={{
          display: 'flex',
          gap: '8px',
          marginBottom: '20px',
          borderBottom: '1px solid var(--border-color)',
          paddingBottom: '12px'
        }}>
          {[
            { id: 'overview', label: 'Topology & Root Cause' },
            { id: 'services', label: 'Microservices Telemetry' },
            { id: 'logs', label: 'Real-Time Ingestion Logs' }
          ].map((tab) => (
            <button
              key={tab.id}
              onClick={() => setActiveTab(tab.id)}
              style={{
                background: activeTab === tab.id ? 'rgba(99, 102, 241, 0.18)' : 'transparent',
                color: activeTab === tab.id ? 'var(--text-primary)' : 'var(--text-secondary)',
                border: activeTab === tab.id ? '1px solid var(--border-highlight)' : '1px solid transparent',
                borderRadius: '8px',
                padding: '8px 16px',
                fontSize: '13px',
                fontWeight: activeTab === tab.id ? '600' : '400',
                cursor: 'pointer',
                transition: 'all 0.15s ease'
              }}
            >
              {tab.label}
            </button>
          ))}
        </div>

        {/* Tab 1: Overview (DAG + Incident Deep Dive + Scenario Studio) */}
        {activeTab === 'overview' && (
          <div style={{ display: 'flex', flexDirection: 'column', gap: '20px' }}>
            <div style={{ display: 'grid', gridTemplateColumns: activeIncident ? '1.2fr 1fr' : '1fr', gap: '20px' }}>
              <DependencyGraphView
                services={services}
                activeIncident={activeIncident}
              />
              {activeIncident && (
                <IncidentDetailsCard
                  incident={activeIncident}
                  services={services}
                />
              )}
            </div>

            <SimulationControl
              currentScenario={currentScenario}
              onStartScenario={handleStartScenario}
              onStopScenario={handleStopScenario}
              isProcessing={isProcessing}
            />

            <ServiceHealthTable
              services={services}
              rootCauseService={rootCauseService}
            />
          </div>
        )}

        {/* Tab 2: Services Telemetry */}
        {activeTab === 'services' && (
          <div style={{ display: 'flex', flexDirection: 'column', gap: '20px' }}>
            <ServiceHealthTable
              services={services}
              rootCauseService={rootCauseService}
            />
            <DependencyGraphView
              services={services}
              activeIncident={activeIncident}
            />
          </div>
        )}

        {/* Tab 3: Real-Time Ingestion Logs */}
        {activeTab === 'logs' && (
          <LogViewer
            logs={logs}
            services={services}
            activeTraceFilter={activeTraceFilter}
            onSelectTrace={setActiveTraceFilter}
          />
        )}
      </main>

      {/* Footer */}
      <footer style={{
        padding: '16px 28px',
        borderTop: '1px solid var(--border-color)',
        textAlign: 'center',
        fontSize: '12px',
        color: 'var(--text-muted)',
        background: 'rgba(9, 13, 22, 0.9)'
      }}>
        <span>Intelligent Distributed System Fault Detection & Root-Cause Analyzer</span>
        <span style={{ margin: '0 8px' }}>•</span>
        <span>C++17 Multi-Threaded Engine & IPC Bridge</span>
        <span style={{ margin: '0 8px' }}>•</span>
        <span>Spring Boot 3.3 Data JPA</span>
        <span style={{ margin: '0 8px' }}>•</span>
        <span>React 18 Observability Studio</span>
      </footer>
    </div>
  );
}
