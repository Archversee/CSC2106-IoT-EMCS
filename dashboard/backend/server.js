// dashboard/backend/server.js
const express = require('express');
const http = require('http');
const path = require('path');
const cors = require('cors');
const { Server } = require('socket.io');
const mqtt = require('mqtt');
const { publishWithRetry } = require('./qos');

const FRONTEND_DIR = path.join(__dirname, '..', 'frontend');

const PORT = process.env.PORT || 3000;
const MQTT_BROKER = process.env.MQTT_URL || 'mqtt://localhost:1883';

const app = express();
app.use(cors());

// serve static frontend
app.use(express.static(FRONTEND_DIR));

const server = http.createServer(app);
const io = new Server(server, {
  cors: { origin: '*' }
});

// in-memory state
const devices = {}; // devices[deviceId] = { status: { ts, payload }, telemetry: [...], lastSeen }
const recentMessages = []; // array of { ts, deviceId, topic, payload }

function addRecent(msg) {
  recentMessages.unshift(msg);
  if (recentMessages.length > 50) recentMessages.pop();
}

// MQTT client
const mqttClient = mqtt.connect(MQTT_BROKER);

mqttClient.on('connect', () => {
  console.log('[MQTT] connected to', MQTT_BROKER);
  // subscribe to device topics (telemetry + status)
  mqttClient.subscribe('devices/+/telemetry/#', { qos: 0 }, (err) => {
    if (err) console.error('[MQTT] subscribe error', err);
    else console.log('[MQTT] subscribed', [{ topic: 'devices/+/telemetry/#', qos: 0 }]);
  });
  mqttClient.subscribe('devices/+/status', { qos: 0 }, (err) => {
    if (err) console.error('[MQTT] subscribe error', err);
    else console.log('[MQTT] subscribed', [{ topic: 'devices/+/status', qos: 0 }]);
  });
});

mqttClient.on('error', (err) => {
  console.error('[MQTT] client error', err.message);
});

mqttClient.on('message', (topic, payloadBuf) => {
  let payload = null;
  const str = payloadBuf.toString('utf8');
  try { payload = JSON.parse(str); } catch (e) { payload = str; }

  // topic e.g. devices/pico-001/telemetry/temp
  const pieces = topic.split('/');
  const deviceId = pieces[1] || 'unknown';
  const category = pieces[2] || '';

  const ts = (payload && payload.ts) ? payload.ts : new Date().toISOString();

  // update devices map
  devices[deviceId] = devices[deviceId] || { telemetry: [], status: null, lastSeen: null };
  devices[deviceId].lastSeen = ts;
  if (category === 'status') {
    devices[deviceId].status = { ts, payload };
  } else if (category === 'telemetry') {
    // collect last telemetry (trim)
    devices[deviceId].telemetry.push({ topic, ts, payload });
    if (devices[deviceId].telemetry.length > 100) devices[deviceId].telemetry.shift();
  }

  const msg = { ts, deviceId, topic, payload };
  addRecent(msg);

  // emit to connected sockets: mqtt-message and telemetry event (if numeric)
  io.emit('mqtt-message', msg);

  // if telemetry value present and numeric, emit telemetry plot event
  if (category === 'telemetry' && payload && (typeof payload.value === 'number')) {
    io.emit('telemetry', { deviceId, topic, ts, value: payload.value });
  }
});

io.on('connection', (socket) => {
  console.log('[SOCKET] client connected', socket.id);

  // client wants list of devices
  socket.on('list-devices', () => {
    // send a shallow copy to avoid accidental mutation on client
    const deviceSummary = {};
    Object.keys(devices).forEach(id => {
      deviceSummary[id] = {
        lastSeen: devices[id].lastSeen,
        status: devices[id].status
      };
    });
    socket.emit('devices-list', deviceSummary);
    // also send recent messages
    socket.emit('recent-messages', recentMessages.slice(0, 12));
  });

  // client requests to send a command / publish
  // payload: { topic, payload, qos }
  socket.on('send-cmd', async (req) => {
    try {
      const topic = req.topic;
      const payload = req.payload || {};
      const qos = Number(req.qos) || 0;

      console.log('[SOCKET] send-cmd', { topic, qos });

      if (qos === 1) {
        // use helper that retries until PUBACK or fail
        try {
          await publishWithRetry(mqttClient, topic, payload, 1, { maxRetries: 4, ackTimeoutMs: 2000 });
          socket.emit('publish-status', { topic, success: true });
        } catch (err) {
          console.error('[PUBLISH] qos1 failed', err);
          socket.emit('publish-status', { topic, success: false, error: err.message });
        }
      } else {
        // qos 0: normal fire-and-forget
        mqttClient.publish(topic, (typeof payload === 'string' ? payload : JSON.stringify(payload)), { qos: 0 }, (err) => {
          if (err) {
            socket.emit('publish-status', { topic, success: false, error: err.message });
          } else {
            socket.emit('publish-status', { topic, success: true });
          }
        });
      }
    } catch (err) {
      console.error('[SOCKET] send-cmd handler error', err);
      socket.emit('publish-status', { topic: req && req.topic, success: false, error: err.message });
    }
  });

  socket.on('disconnect', (reason) => {
    console.log('[SOCKET] client disconnected', socket.id, reason);
  });

  // emit initial device list to the newly connected socket
  const initialDeviceSummary = {};
  Object.keys(devices).forEach(id => {
    initialDeviceSummary[id] = {
      lastSeen: devices[id].lastSeen,
      status: devices[id].status
    };
  });
  socket.emit('devices-list', initialDeviceSummary);
  socket.emit('recent-messages', recentMessages.slice(0, 12));
});

// fallback for root: serve index.html (static middleware already does it)
// keep server listening
server.listen(PORT, () => {
  console.log(`HTTP+Socket server listening on http://localhost:${PORT}`);
  console.log('Serving static from', FRONTEND_DIR);
});