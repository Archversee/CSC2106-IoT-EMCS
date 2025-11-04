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
const devices = {};           // devices[deviceId] = { status: { ts, payload }, telemetry: [...], lastSeen }
const recentMessages = [];    // array of { ts, deviceId, topic, payload }
const fileTransfers = new Map(); // key: deviceId, value: transfer state

function addRecent(msg) {
  recentMessages.unshift(msg);
  if (recentMessages.length > 50) recentMessages.pop();
}

// -------- MQTT SETUP --------
const mqttClient = mqtt.connect(MQTT_BROKER);

mqttClient.on('connect', () => {
  console.log('[MQTT] connected to', MQTT_BROKER);

  // original dashboard topics
  mqttClient.subscribe('devices/+/telemetry/#', { qos: 0 }, (err) => {
    if (err) console.error('[MQTT] subscribe error', err);
    else console.log('[MQTT] subscribed', [{ topic: 'devices/+/telemetry/#', qos: 0 }]);
  });

  mqttClient.subscribe('devices/+/status', { qos: 0 }, (err) => {
    if (err) console.error('[MQTT] subscribe error', err);
    else console.log('[MQTT] subscribed', [{ topic: 'devices/+/status', qos: 0 }]);
  });

  mqttClient.subscribe('devices/+/file-transfer/#', { qos: 0 }, (err) => {
    if (err) console.error('[MQTT] subscribe error', err);
    else console.log('[MQTT] subscribed to file-transfer topics');
  });

  // NEW: Pico / gateway topics
  mqttClient.subscribe('pico/#', { qos: 0 }, (err) => {
    if (err) console.error('[MQTT] subscribe pico/# error', err);
    else console.log('[MQTT] subscribed to pico/#');
  });

  mqttClient.subscribe('file/#', { qos: 0 }, (err) => {
    if (err) console.error('[MQTT] subscribe file/# error', err);
    else console.log('[MQTT] subscribed to file/#');
  });
});

mqttClient.on('error', (err) => {
  console.error('[MQTT] client error', err.message);
});

// unified message handler for devices/... and pico/... topics
mqttClient.on('message', (topic, payloadBuf) => {
  let payload = null;
  const str = payloadBuf.toString('utf8');
  try { payload = JSON.parse(str); } catch (e) { payload = str; }

  const parts = topic.split('/');

  let deviceId = 'unknown';
  let category = '';

  // 1) Dashboard-style topics: devices/<id>/status, devices/<id>/telemetry/...
  if (topic.startsWith('devices/')) {
    deviceId = parts[1] || 'unknown';
    category = parts[2] || '';

  // 2) Gateway/Pico topics: pico/status, pico/cmd, etc.
  } else if (topic.startsWith('pico/')) {
    const sub = parts[1] || '';
    deviceId = 'pico-001';   // single device in this project
    category = sub;          // 'status', 'cmd', etc.

  // 3) Raw file topics: file/meta, file/data, etc. (just log under 'file')
  } else if (topic.startsWith('file/')) {
    deviceId = 'pico-001';
    category = 'file';

  } else {
    deviceId = parts[1] || 'unknown';
    category = parts[2] || '';
  }

  // ---- File transfer messages via devices/.../file-transfer/... ----
  if (category === 'file-transfer') {
    const transferAction = parts[3];

    if (!fileTransfers.has(deviceId)) {
      fileTransfers.set(deviceId, {});
    }

    const transfer = fileTransfers.get(deviceId);

    if (transferAction === 'progress') {
      transfer.currentChunk = payload.chunk;
      transfer.totalChunks = payload.total;
      transfer.sequenceNum = payload.seq;
      transfer.checksum = payload.checksum;
      transfer.progress = Math.round((payload.chunk / payload.total) * 100);
      transfer.lastUpdate = new Date().toISOString();
    } else if (transferAction === 'status') {
      transfer.status = payload.status;
      transfer.fileName = payload.fileName;
      transfer.reason = payload.reason || '';
      transfer.lastUpdate = new Date().toISOString();

      if (payload.status === 'active') {
        transfer.currentChunk = 0;
        transfer.totalChunks = 0;
        transfer.progress = 0;
      }
    } else if (transferAction === 'validation') {
      transfer.validationResult = payload.result;
      transfer.expectedChecksum = payload.expected;
      transfer.actualChecksum = payload.actual;
      transfer.lastUpdate = new Date().toISOString();
    }

    io.emit('file-transfer-update', {
      deviceId,
      type: transferAction,
      data: payload,
      transfer: transfer
    });

    console.log(`[File Transfer] ${deviceId} - ${transferAction}:`, payload);
    return; // don't treat as normal telemetry/status
  }

  const ts = (payload && payload.ts) ? payload.ts : new Date().toISOString();

  // update devices map
  devices[deviceId] = devices[deviceId] || { telemetry: [], status: null, lastSeen: null };
  devices[deviceId].lastSeen = ts;

  if (category === 'status') {
    // works for both devices/<id>/status and pico/status
    devices[deviceId].status = { ts, payload };
  } else if (category === 'telemetry') {
    devices[deviceId].telemetry.push({ topic, ts, payload });
    if (devices[deviceId].telemetry.length > 100) devices[deviceId].telemetry.shift();
  }

  const msg = { ts, deviceId, topic, payload };
  addRecent(msg);

  io.emit('mqtt-message', msg);

  if (category === 'telemetry' && payload && (typeof payload.value === 'number')) {
    io.emit('telemetry', { deviceId, topic, ts, value: payload.value });
  }
});

// -------- SOCKET.IO SETUP --------
io.on('connection', (socket) => {
  console.log('[SOCKET] client connected', socket.id);

  // client wants list of devices
  socket.on('list-devices', () => {
    const deviceSummary = {};
    Object.keys(devices).forEach(id => {
      deviceSummary[id] = {
        lastSeen: devices[id].lastSeen,
        status: devices[id].status
      };
    });
    socket.emit('devices-list', deviceSummary);
    socket.emit('recent-messages', recentMessages.slice(0, 12));
  });

  // client requests to send a command / publish
  // payload: { topic, payload, qos }
    // client requests to send a command / publish
  // payload: { topic, payload, qos }
  socket.on('send-cmd', async (req) => {
    try {
      let topic = req.topic;           // <-- make this "let" so we can rewrite it
      const payload = req.payload || {};
      const qos = Number(req.qos) || 0;

      // --- NEW: translate dashboard-style topic to Pico-style topic ---
      // Dashboard builds:  devices/<deviceId>/<suffix>
      // Pico expects:      <deviceId>/<suffix>   (e.g. pico/cmd)
      if (topic && topic.startsWith('devices/')) {
        const parts = topic.split('/');
        if (parts.length >= 3) {
          // drop the leading "devices"
          topic = parts.slice(1).join('/');   // e.g. "devices/pico/cmd" -> "pico/cmd"
        }
      }

      console.log('[SOCKET] send-cmd', { original: req.topic, mappedTopic: topic, qos });

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
        mqttClient.publish(
          topic,
          (typeof payload === 'string' ? payload : JSON.stringify(payload)),
          { qos: 0 },
          (err) => {
            if (err) {
              socket.emit('publish-status', { topic, success: false, error: err.message });
            } else {
              socket.emit('publish-status', { topic, success: true });
            }
          }
        );
      }
    } catch (err) {
      console.error('[SOCKET] send-cmd handler error', err);
      socket.emit('publish-status', { topic: req && req.topic, success: false, error: err.message });
    }
  });

  socket.on('disconnect', (reason) => {
    console.log('[SOCKET] client disconnected', socket.id, reason);
  });

  // emit initial device list + recent messages
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

// -------- START SERVER --------
server.listen(PORT, () => {
  console.log(`HTTP+Socket server listening on http://localhost:${PORT}`);
  console.log('Serving static from', FRONTEND_DIR);
});