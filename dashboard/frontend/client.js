// dashboard/frontend/client.js
(function () {
  console.log('client.js loaded');

  const socket = io();

  // DOM refs
  const devicesUl = document.getElementById('devices-ul');
  const refreshBtn = document.getElementById('refresh-devices');
  const deviceIdInput = document.getElementById('device-id');
  const topicSuffixInput = document.getElementById('topic-suffix');
  const payloadInput = document.getElementById('payload');
  const sendBtn = document.getElementById('send-btn');
  const qos1Checkbox = document.getElementById('qos1');
  const recentEl = document.getElementById('recent-messages');

  // Chart
  const ctx = document.getElementById('telemetry-chart').getContext('2d');
  const chartData = { labels: [], datasets: [{ label: 'value', data: [], tension: 0.3, pointRadius: 5, backgroundColor: '#4da6ff', borderColor: '#4da6ff' }] };
  const chart = new Chart(ctx, {
    type: 'line',
    data: chartData,
    options: {
      scales: { x: { type: 'category' }, y: { beginAtZero: true } },
      plugins: { legend: { display: false } }
    }
  });

  let selectedDevice = null;
  const MAX_POINTS = 40;

  function showToast(text, type = 'info') {
    const t = document.createElement('div');
    t.className = 'toast';
    t.style.background = (type === 'success') ? '#2ecc71' : (type === 'error') ? '#e74c3c' : '#333';
    t.textContent = text;
    document.body.appendChild(t);
    setTimeout(() => t.remove(), 3000);
  }

  function addRecentMessage(rec) {
    const p = document.createElement('div');
    p.className = 'recent-item';
    const ts = rec.ts || new Date().toISOString();
    const dev = rec.deviceId || '-';
    p.textContent = `${ts} | ${dev} | ${rec.topic} ⇒ ${typeof rec.payload === 'string' ? rec.payload : JSON.stringify(rec.payload)}`;
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
        deviceIdInput.value = deviceId;
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

    if (!selectedDevice && ids.length > 0) {
      selectedDevice = ids[0];
      deviceIdInput.value = selectedDevice;
      const firstLi = devicesUl.querySelector('li');
      if (firstLi) firstLi.style.fontWeight = '800';
    }
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

  socket.on('telemetry', (d) => {
    onTelemetry(d);
  });

  socket.on('publish-status', (info) => {
    console.log('[SOCKET] publish-status', info);
    sendBtn.disabled = false;
    if (info.success) showToast(`Publish acknowledged: ${info.topic}`, 'success');
    else showToast(`Publish failed: ${info.error}`, 'error');
  });

  socket.on('file-transfer-update', (data) => {
    console.log('[File Transfer Update] RECEIVED:', data);
    console.log('Device ID:', data.deviceId);
    console.log('Type:', data.type);
    console.log('Transfer data:', data.transfer);
    
    updateFileTransferUI(data);
    

    if (data.type === 'status' && data.data.status === 'failed') {
      showToast(`Transfer failed on ${data.deviceId}: ${data.data.reason || 'Unknown error'}`, 'error');
    }
    
    if (data.type === 'validation' && data.data.result === 'failed') {
      showToast(`Checksum validation failed on ${data.deviceId}`, 'error');
    }
  });
  
    

  function updateFileTransferUI(data) {
    console.log('[updateFileTransferUI] Called with:', data);
    const { deviceId, type, transfer } = data;
    let card = document.getElementById(`transfer-${deviceId}`);
    console.log('[updateFileTransferUI] Existing card:', card);
  
    if (!card) {
      console.log('[updateFileTransferUI] Creating new card for', deviceId);
      card = createTransferCard(deviceId);
    }

    // Update progress bar
    if (type === 'progress') {
      const percent = transfer.progress || 0;
      card.querySelector('.transfer-progress-bar').style.width = `${percent}%`;
      card.querySelector('.chunk-info').textContent = 
        `Chunk ${transfer.currentChunk}/${transfer.totalChunks} (Seq: ${transfer.sequenceNum})`;
      card.querySelector('.checksum-info').textContent = `Checksum: ${transfer.checksum || '-'}`;
    }
    
    // Update status
    if (type === 'status') {
      card.querySelector('.status').textContent = transfer.status || '-';
      card.querySelector('.status').className = `status status-${transfer.status}`;
      card.querySelector('.file-name').textContent = transfer.fileName || 'Unknown';
    }
    
    // Update validation
    if (type === 'validation') {
      const validationDiv = card.querySelector('.validation-info');
      validationDiv.innerHTML = `
        <strong>Validation:</strong> ${transfer.validationResult}<br>
        Expected: ${transfer.expectedChecksum}<br>
        Actual: ${transfer.actualChecksum}
      `;
      validationDiv.className = `validation-info ${transfer.validationResult === 'success' ? 'status-completed' : 'status-failed'}`;
    }
  }

  function createTransferCard(deviceId) {
    console.log('[createTransferCard] Creating card for:', deviceId);
  
    // Always show the section first
    const section = document.getElementById('file-transfers-section');
    if (section) {
      console.log('[createTransferCard] Making section visible');
      section.style.display = 'block';
    }
  
    let container = document.getElementById('file-transfers-container');
    console.log('[createTransferCard] Container found:', container);
  
    if (!container) {
      console.log('[createTransferCard] ERROR: Container not found even after showing section!');
      return null;
    }
    
    const card = document.createElement('div');
    card.id = `transfer-${deviceId}`;
    card.className = 'transfer-card';
    card.innerHTML = `
      <h3>Device: ${deviceId}</h3>
      <div><strong>File:</strong> <span class="file-name">-</span></div>
      <div><strong>Status:</strong> <span class="status">-</span></div>
      <div class="transfer-progress">
        <div class="transfer-progress-bar" style="width: 0%"></div>
      </div>
      <div class="chunk-info">Waiting...</div>
      <div class="checksum-info"></div>
      <div class="validation-info"></div>
    `;
    
    console.log('[createTransferCard] Appending card to container');
    container.appendChild(card);
    console.log('[createTransferCard] Card created and appended successfully');
    
    return card;
  }

  refreshBtn.addEventListener('click', () => {
    console.log('Refresh button clicked — requesting devices-list');
    refreshBtn.disabled = true;
    socket.emit('list-devices');
    setTimeout(() => { refreshBtn.disabled = false; }, 600);
  });

  sendBtn.addEventListener('click', () => {
    const device = deviceIdInput.value.trim();
    const suffix = topicSuffixInput.value.trim();
    const body = payloadInput.value.trim();

    if (!device || !suffix) { showToast('Device and topic suffix required', 'error'); return; }

    let payload;
    try { payload = body ? JSON.parse(body) : {}; }
    catch (e) { showToast('Payload must be valid JSON', 'error'); return; }

    const topic = `devices/${device}/${suffix}`;
    const qos = qos1Checkbox && qos1Checkbox.checked ? 1 : 0;

    addRecentMessage({ ts: new Date().toISOString(), deviceId: device, topic, payload: '(sending) ' + JSON.stringify(payload) });

    sendBtn.disabled = true;
    console.log('[SOCKET] send-cmd', { topic, payload, qos });
    socket.emit('send-cmd', { topic, payload, qos });
  });

})();