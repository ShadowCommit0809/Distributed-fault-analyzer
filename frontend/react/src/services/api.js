const API_BASE = '/api';

export async function fetchStatistics() {
  try {
    const res = await fetch(`${API_BASE}/statistics`);
    if (!res.ok) throw new Error(`HTTP error ${res.status}`);
    return await res.json();
  } catch (err) {
    console.error('Error fetching statistics:', err);
    return null;
  }
}

export async function fetchServices() {
  try {
    const res = await fetch(`${API_BASE}/services`);
    if (!res.ok) throw new Error(`HTTP error ${res.status}`);
    return await res.json();
  } catch (err) {
    console.error('Error fetching services:', err);
    return [];
  }
}

export async function fetchActiveIncident() {
  try {
    const res = await fetch(`${API_BASE}/incidents/active`);
    if (!res.ok) throw new Error(`HTTP error ${res.status}`);
    const text = await res.text();
    return text ? JSON.parse(text) : null;
  } catch (err) {
    console.error('Error fetching active incident:', err);
    return null;
  }
}

export async function fetchIncidents() {
  try {
    const res = await fetch(`${API_BASE}/incidents`);
    if (!res.ok) throw new Error(`HTTP error ${res.status}`);
    return await res.json();
  } catch (err) {
    console.error('Error fetching incidents:', err);
    return [];
  }
}

export async function fetchLogs(params = {}) {
  try {
    const query = new URLSearchParams();
    if (params.service) query.set('service', params.service);
    if (params.severity) query.set('severity', params.severity);
    if (params.traceId) query.set('traceId', params.traceId);
    if (params.limit) query.set('limit', params.limit);

    const res = await fetch(`${API_BASE}/logs?${query.toString()}`);
    if (!res.ok) throw new Error(`HTTP error ${res.status}`);
    return await res.json();
  } catch (err) {
    console.error('Error fetching logs:', err);
    return [];
  }
}

export async function startSimulation(scenario, requestsPerSecond = 4) {
  try {
    const res = await fetch(`${API_BASE}/simulation/start`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ scenario, requestsPerSecond })
    });
    return await res.json();
  } catch (err) {
    console.error('Error starting simulation:', err);
    return { error: err.message };
  }
}

export async function stopSimulation() {
  try {
    const res = await fetch(`${API_BASE}/simulation/stop`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({})
    });
    return await res.json();
  } catch (err) {
    console.error('Error stopping simulation:', err);
    return { error: err.message };
  }
}

export async function resetSimulation() {
  try {
    const res = await fetch(`${API_BASE}/simulation/reset`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({})
    });
    return await res.json();
  } catch (err) {
    console.error('Error resetting simulation:', err);
    return { error: err.message };
  }
}
