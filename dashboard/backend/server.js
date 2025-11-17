// dashboard/backend/server.js
const express = require('express');
const http = require('http');
const path = require('path');
const cors = require('cors');
const { Server } = require('socket.io');
const mqtt = require('mqtt');

const FRONTEND_DIR = path.join(__dirname, '..', 'frontend');

const PORT = process.env.PORT || 3000;
const MQTT_BROKER = process.env.MQTT_URL || 'mqtt://localhost:1883';

// This is the logical device id shown on the dashboard's left list
// (Topics don't carry an id; your firmware client id is "pico_me".)
const PICO_DEVICE_ID = process.env.PICO_DEVICE_ID || 'pico_me';

const app = express();
app.use(cors());
app.use(express.static(FRONTEND_DIR));

const server = http.createServer(app);
const io = new Server(server, { cors: { origin: '*' } });

// ─────────────────────────────────────────────────────────────────────────────
// State
// ─────────────────────────────────────────────────────────────────────────────
const devices = {};           // devices[deviceId] = { status: { ts, payload }, telemetry: [], lastSeen }
const recentMessages = [];    // [{ ts, deviceId, topic, payload }]
const MAX_RECENT = 50;

function addRecent(msg) {
  recentMessages.unshift(msg);
  if (recentMessages.length > MAX_RECENT) recentMessages.pop();
}

// ─────────────────────────────────────────────────────────────────────────────
// MQTT
// ─────────────────────────────────────────────────────────────────────────────
const mqttClient = mqtt.connect(MQTT_BROKER);

mqttClient.on('connect', () => {
  console.log('[MQTT] connected to', MQTT_BROKER);

  // Match your predefined topics from config.h / gateway:
  // 1 -> pico/cmd
  // 2 -> pico/status
  // 3 -> file/meta
  // 4 -> file/data
  mqttClient.subscribe('pico/#', { qos: 0 }, (err) => {
    if (err) console.error('[MQTT] subscribe pico/# error', err);
    else console.log('[MQTT] subscribed: pico/#');
  });
  mqttClient.subscribe('file/#', { qos: 0 }, (err) => {
    if (err) console.error('[MQTT] subscribe file/# error', err);
    else console.log('[MQTT] subscribed: file/#');
  });
});

mqttClient.on('error', (err) => {
  console.error('[MQTT] client error', err.message);
});

function extractNumeric(valueLike) {
  // Returns a Number or null
  if (valueLike == null) return null;

  // JSON object cases: { value: 12.3 } or { temp: 27.1 }
  if (typeof valueLike === 'object') {
    if (typeof valueLike.value === 'number') return valueLike.value;
    if (typeof valueLike.temp === 'number') return valueLike.temp;
  }

  // String cases: "value=12.3", "temp: 27.1", etc.
  if (typeof valueLike === 'string') {
    const m = valueLike.match(/(?:value|temp)\s*[:=]\s*(-?\d+(?:\.\d+)?)/i);
    if (m) return Number(m[1]);
    // try a bare number string
    if (!isNaN(Number(valueLike.trim()))) return Number(valueLike.trim());
  }

  return null;
}

mqttClient.on('message', (topic, payloadBuf) => {
  let payload = null;
  const str = payloadBuf.toString('utf8');
  try { payload = JSON.parse(str); } catch { payload = str; }

  const nowIso = new Date().toISOString();
  const deviceId = PICO_DEVICE_ID;

  // Initialize device slot
  devices[deviceId] = devices[deviceId] || { telemetry: [], status: null, lastSeen: null };
  devices[deviceId].lastSeen = nowIso;

  // Categorize topic
  // pico/cmd      -> commands to device (we may log if echoed back)
  // pico/status   -> device status/heartbeats/log lines (& we'll mine telemetry)
  // file/meta|data -> file transfer (we only log now)
  const parts = topic.split('/');
  const root = parts[0];      // 'pico' or 'file'
  const leaf = parts[1] || ''; // 'cmd' | 'status' | 'meta' | 'data'

  if (root === 'pico' && leaf === 'status') {
    devices[deviceId].status = { ts: nowIso, payload };
  }

  // Add recent log entry
  const msg = { ts: nowIso, deviceId, topic, payload };
  addRecent(msg);
  io.emit('mqtt-message', msg);

  // Telemetry for chart: try to extract a numeric datapoint from pico/status
  if (root === 'pico' && leaf === 'status') {
    const numeric = extractNumeric(payload);
    if (numeric !== null) {
      devices[deviceId].telemetry.push({ topic, ts: nowIso, payload: numeric });
      if (devices[deviceId].telemetry.length > 100) devices[deviceId].telemetry.shift();

      io.emit('telemetry', { deviceId, topic, ts: nowIso, value: numeric });
    }
  }

  // file/*: only log; UI progress bar was removed
});

io.on('connection', (socket) => {
  console.log('[SOCKET] client connected', socket.id);

  // Initial snapshot
  const summary = {};
  Object.keys(devices).forEach(id => {
    summary[id] = { lastSeen: devices[id].lastSeen, status: devices[id].status };
  });
  socket.emit('devices-list', summary);
  socket.emit('recent-messages', recentMessages.slice(0, 12));

  // Allow the client to refresh device list
  socket.on('list-devices', () => {
    const s = {};
    Object.keys(devices).forEach(id => {
      s[id] = { lastSeen: devices[id].lastSeen, status: devices[id].status };
    });
    socket.emit('devices-list', s);
    socket.emit('recent-messages', recentMessages.slice(0, 12));
  });

  // Publish command (dashboard → broker)
  // req: { topic: "pico/cmd", payload: "led on", qos?: 0|1 }
  socket.on('send-cmd', (req) => {
    try {
      const topic = String(req.topic || '').trim();
      const qos = Number(req.qos) || 0;

      // For your setup, we send *plain strings* to pico/cmd.
      // If payload is an object, we JSON-stringify; if it's already string,
      // we send it raw.
      let body = req.payload;
      if (typeof body === 'object') body = JSON.stringify(body);
      if (typeof body !== 'string') body = String(body ?? '');

      console.log('[PUBLISH]', { topic, qos, body });
      mqttClient.publish(topic, body, { qos }, (err) => {
        if (err) {
          socket.emit('publish-status', { topic, success: false, error: err.message });
        } else {
          socket.emit('publish-status', { topic, success: true });
        }
      });
    } catch (err) {
      console.error('[send-cmd] error', err);
      socket.emit('publish-status', { topic: req && req.topic, success: false, error: err.message });
    }
  });

  socket.on('disconnect', (reason) => {
    console.log('[SOCKET] client disconnected', socket.id, reason);
  });
});

server.listen(PORT, () => {
  console.log(`HTTP+Socket server listening on http://localhost:${PORT}`);
  console.log('Serving static from', FRONTEND_DIR);
});