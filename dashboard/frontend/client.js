// dashboard/frontend/client.js
(function () {
  console.log('client.js loaded');

  const socket = io();

  // DOM refs that remain
  const devicesUl   = document.getElementById('devices-ul');
  const refreshBtn  = document.getElementById('refresh-devices');
  const payloadInput= document.getElementById('payload');
  const sendBtn     = document.getElementById('send-btn');
  const recentEl    = document.getElementById('recent-messages');

  // Clear any leftover value the HTML might have injected
  if (payloadInput) payloadInput.value = "";

  // Chart (unchanged)
  const ctx = document.getElementById('telemetry-chart').getContext('2d');
  const chartData = {
    labels: [],
    datasets: [{
      label: 'value',
      data: [],
      tension: 0.3,
      pointRadius: 5,
      backgroundColor: '#4da6ff',
      borderColor: '#4da6ff'
    }]
  };
  const chart = new Chart(ctx, {
    type: 'line',
    data: chartData,
    options: {
      scales: { x: { type: 'category' }, y: { beginAtZero: true } },
      plugins: { legend: { display: false } }
    }
  });

  // We still show devices, but command sending no longer depends on device selection
  let selectedDevice = null;
  const MAX_POINTS = 40;

  function showToast(text, type = 'info') {
    const t = document.createElement('div');
    t.className = 'toast';
    t.style.background =
      type === 'success' ? '#2ecc71' :
      type === 'error'   ? '#e74c3c' : '#333';
    t.textContent = text;
    document.body.appendChild(t);
    setTimeout(() => t.remove(), 3000);
  }

  function addRecentMessage(rec) {
    const p = document.createElement('div');
    p.className = 'recent-item';
    const ts = rec.ts || new Date().toISOString();
    const dev = rec.deviceId || '-';
    const payloadText = (typeof rec.payload === 'string') ? rec.payload : JSON.stringify(rec.payload);
    p.textContent = `${ts} | ${dev} | ${rec.topic} ⇒ ${payloadText}`;
    recentEl.prepend(p);
    while (recentEl.children.length > 12) recentEl.removeChild(recentEl.lastChild);
  }

  function renderDevices(devices) {
    devicesUl.innerHTML = '';
    const ids = Object.keys(devices);
    ids.forEach(deviceId => {
      const li = document.createElement('li');
      li.textContent = deviceId;
      li.style.fontWeight = (deviceId === selectedDevice) ? '800' : '600';
      li.addEventListener('click', () => {
        selectedDevice = deviceId;
        Array.from(devicesUl.children).forEach(c => c.style.fontWeight = '600');
        li.style.fontWeight = '800';
      });

      const sub = document.createElement('div');
      sub.className = 'muted';
      const last = devices[deviceId].lastSeen || (devices[deviceId].status && devices[deviceId].status.ts) || '-';
      sub.textContent = `last: ${last}`;
      li.appendChild(sub);
      devicesUl.appendChild(li);
    });

    // No auto-select; we only highlight when the user clicks
  }

  function onTelemetry({ deviceId, ts, value }) {
    if (selectedDevice && deviceId !== selectedDevice) return;
    const label = new Date(ts).toLocaleTimeString();
    chartData.labels.push(label);
    chartData.datasets[0].data.push(value);
    if (chartData.labels.length > MAX_POINTS) {
      chartData.labels.shift();
      chartData.datasets[0].data.shift();
    }
    chart.update();
  }

  // --- Socket wiring ---
  socket.on('connect', () => {
    console.log('[SOCKET] connected', socket.id);
    socket.emit('list-devices');
  });

  socket.on('disconnect', (reason) => {
    console.log('[SOCKET] disconnected', reason);
  });

  socket.on('devices-list', (devices) => {
    console.log('[SOCKET] devices-list received', devices);
    renderDevices(devices);
  });

  socket.on('recent-messages', (msgs) => {
    console.log('[SOCKET] recent-messages', msgs && msgs.length);
    recentEl.innerHTML = '';
    for (const m of (msgs || []).slice(0, 12).reverse()) addRecentMessage(m);
  });

  socket.on('mqtt-message', (msg) => {
    console.log('[SOCKET] mqtt-message', msg.topic);
    addRecentMessage(msg);
  });

  socket.on('telemetry', (d) => onTelemetry(d));

  socket.on('publish-status', (info) => {
    console.log('[SOCKET] publish-status', info);
    sendBtn.disabled = false;
    if (info.success) showToast(`Publish acknowledged: ${info.topic}`, 'success');
    else showToast(`Publish failed: ${info.error}`, 'error');
  });

  // --- UI actions ---
  if (refreshBtn) {
    refreshBtn.addEventListener('click', () => {
      console.log('Refresh button clicked — requesting devices-list');
      refreshBtn.disabled = true;
      socket.emit('list-devices');
      setTimeout(() => { refreshBtn.disabled = false; }, 600);
    });
  }

  // Publish plain-string commands to fixed topic pico/cmd; QoS0
  sendBtn.addEventListener('click', () => {
    const payload = (payloadInput.value || '').trim();
    const topic   = 'pico/cmd';
    const qos     = 0; // checkbox removed; using QoS0 for commands

    if (!payload) {
      showToast('Enter a command (e.g., "led on")', 'error');
      return;
    }

    addRecentMessage({
      ts: new Date().toISOString(),
      deviceId: 'pico',
      topic,
      payload: `(sending) ${payload}`
    });

    sendBtn.disabled = true;
    console.log('[SOCKET] send-cmd', { topic, payload, qos });
    socket.emit('send-cmd', { topic, payload, qos });
  });
  
})();