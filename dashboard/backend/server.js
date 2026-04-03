"use strict";

/**
 * server.js — CSC2106 IoT Dashboard Backend
 *
 * Real MQTT message format (confirmed from lora_singlehop CSV):
 *   topic:   sensors/arduino/data   payload: "arduino-02 <N>"  (QoS0, 3x per cycle)
 *   topic:   sensors/data           payload: "arduino-02 <N>"  (QoS1, 1x per cycle)
 *   topic:   sensors/pico/data      payload: "pico_w_iot <N>"
 *   topic:   sensors/cmd            (commands to nodes)
 *
 * What we derive from MQTT alone (no gateway changes needed):
 *   - forwarding mode label  (set by operator before each run)
 *   - seq gaps  → lost packets → delivery rate per node
 *   - seq dupes → duplicate forwards (flooding artifact)
 *   - per-node traffic volume
 *   - message rate / interval
 *   - inferred hop count from topology (2-node mesh: a02 → a03 → gateway)
 */

const express = require("express");
const http = require("http");
const socketIo = require("socket.io");
const mqtt = require("mqtt");
const cors = require("cors");

const app = express();
const server = http.createServer(app);

app.use(cors());
app.use(express.json());
app.use(express.static(require("path").join(__dirname, "../frontend")));

const io = socketIo(server, { cors: { origin: "*" } });

// ── MQTT ──────────────────────────────────────────────────────────────────
const MQTT_BROKER = "mqtt://172.20.10.13:1883"; // "mqtt://localhost:1883" if running locally
const mqttClient = mqtt.connect(MQTT_BROKER, {
    clientId: "csc2106_dashboard_" + Math.random().toString(16).slice(2, 8),
    clean: true,
    reconnectPeriod: 3000
});

mqttClient.on("connect", () => {
    console.log("[MQTT] Connected");
    mqttClient.subscribe("sensors/#", (err) => {
        if (!err) console.log("[MQTT] Subscribed to sensors/#");
    });
});
mqttClient.on("error", (e) => console.error("[MQTT] Error:", e.message));
mqttClient.on("reconnect", () => console.log("[MQTT] Reconnecting..."));

// ── Payload parser ────────────────────────────────────────────────────────
// "arduino-02 5"  →  { node: "arduino-02", seq: 5 }
function parsePayload(raw) {
    const parts = raw.trim().split(/\s+/);
    const node = parts[0] || "";
    const seq = parts.length > 1 ? parseInt(parts[1], 10) : NaN;
    return { node, seq: isNaN(seq) ? null : seq };
}

// Map node string → short bucket key
function bucket(node) {
    if (node === "arduino-03") return "a03";
    if (node === "arduino-02") return "a02";
    if (node === "arduino-04") return "a04";
    if (node === "arduino-05") return "a05";
    if (node.startsWith("pico")) return "pico";
    return null;
}

// ── Per-run state ─────────────────────────────────────────────────────────
function freshRun(mode) {
    return {
        mode,
        active: false,
        startTime: null,
        endTime: null,
        pkts: 0,
        seqTrack: {},          // topic → { last, gaps, dupes }
        dupes: 0,
        gaps: 0,
        perNode: { a02: 0, a03: 0, a04: 0, a05: 0, pico: 0 },
        timestamps: [],
        events: []
    };
}

let currentMode = "flooding";
let activeRun = null;

const runs = {
    flooding: freshRun("flooding"),
    routing: freshRun("routing")
};

const allTime = { total: 0, perNode: { a02: 0, a03: 0, a04: 0, a05: 0, pico: 0 } };

// ── Sequence gap/dupe detection ───────────────────────────────────────────
// Key is "topic::node" so each node's counter is tracked independently.
// arduino-02 and arduino-03 both publish to sensors/arduino/data with their
// own counters starting from 1 — without the node key they look like dupes.
function trackSeq(run, topic, node, seq) {
    if (seq === null) return { isDupe: false, gap: 0 };

    const key = topic + '::' + (node || 'unknown');

    if (!run.seqTrack[key]) {
        run.seqTrack[key] = { last: seq, gaps: 0, dupes: 0 };
        return { isDupe: false, gap: 0 };
    }

    const t = run.seqTrack[key];

    if (seq <= t.last) {
        t.dupes++;
        run.dupes++;
        return { isDupe: true, gap: 0 };
    }

    const gap = seq - t.last - 1;
    if (gap > 0) { t.gaps += gap; run.gaps += gap; }
    t.last = seq;
    return { isDupe: false, gap };
}

// ── Summary builder ───────────────────────────────────────────────────────
function buildSummary(mode) {
    const r = runs[mode];

    let avgInterval = null;
    if (r.timestamps.length > 1) {
        let sum = 0;
        for (let i = 1; i < r.timestamps.length; i++)
            sum += r.timestamps[i] - r.timestamps[i - 1];
        avgInterval = (sum / (r.timestamps.length - 1) / 1000).toFixed(2);
    }

    const expected = r.pkts + r.gaps;
    const deliveryPct = expected > 0
        ? ((r.pkts / expected) * 100).toFixed(1)
        : null;

    // Inferred from topology: flooding always goes through relay (2 hops),
    // routing uses static table which can be 1 or 2 hops.
    const inferredHops = mode === "flooding" ? 2 : 1;

    return {
        mode,
        active: r.active,
        pkts: r.pkts,
        dupes: r.dupes,
        gaps: r.gaps,
        deliveryPct,
        avgInterval,
        inferredHops,
        perNode: { ...r.perNode },
        duration: r.startTime ? (r.endTime || Date.now()) - r.startTime : 0,
        startTime: r.startTime,
        endTime: r.endTime,
        events: r.events.slice(-60)
    };
}

function broadcastUpdate() {
    io.emit("run_update", {
        flooding: buildSummary("flooding"),
        routing: buildSummary("routing"),
        allTime,
        activeRun,
        currentMode
    });
}

// ── MQTT incoming ─────────────────────────────────────────────────────────
mqttClient.on("message", (topic, message) => {
    const raw = message.toString().trim();
    const time = new Date().toLocaleTimeString("en-GB");
    const now = Date.now();

    console.log(`[MQTT] ${topic} : ${raw}`);

    if (topic.includes("cmd")) {
        io.emit("mqtt_message", { topic, message: raw, time, isCmd: true, mode: currentMode });
        return;
    }

    const { node, seq } = parsePayload(raw);
    const bkt = bucket(node);

    allTime.total++;
    if (bkt) allTime.perNode[bkt]++;

    let isDupe = false, gap = 0;

    if (activeRun) {
        const r = runs[activeRun];
        r.pkts++;
        r.timestamps.push(now);
        if (bkt) r.perNode[bkt]++;

        const result = trackSeq(r, topic, node, seq);
        isDupe = result.isDupe;
        gap = result.gap;

        r.events.push({ time, node, topic, seq, isDupe, gap, mode: activeRun });
        if (r.events.length > 200) r.events.shift();

        broadcastUpdate();
    }

    io.emit("mqtt_message", {
        topic, message: raw, time, node, seq,
        isDupe, gap, isCmd: false, mode: currentMode, ts: now
    });
});

// ── WebSocket ─────────────────────────────────────────────────────────────
io.on("connection", (socket) => {
    console.log("[WS] Client connected");
    socket.emit("mode_change", { mode: currentMode });
    socket.emit("run_update", {
        flooding: buildSummary("flooding"),
        routing: buildSummary("routing"),
        allTime, activeRun, currentMode
    });
    socket.on("disconnect", () => console.log("[WS] Client disconnected"));
});

// ── REST API ──────────────────────────────────────────────────────────────

app.get("/mode/:mode", (req, res) => {
    const mode = req.params.mode;
    if (!["flooding", "routing"].includes(mode))
        return res.status(400).json({ error: "Use flooding or routing" });
    currentMode = mode;
    io.emit("mode_change", { mode });
    console.log(`[MODE] ${mode}`);
    res.json({ ok: true, mode });
});

app.get("/run/start/:mode", (req, res) => {
    const mode = req.params.mode;
    if (!["flooding", "routing"].includes(mode))
        return res.status(400).json({ error: "Invalid mode" });

    if (activeRun) _stopRun(activeRun);

    runs[mode] = freshRun(mode);
    runs[mode].active = true;
    runs[mode].startTime = Date.now();
    activeRun = mode;
    currentMode = mode;

    io.emit("mode_change", { mode });
    io.emit("run_started", { mode, startTime: runs[mode].startTime });
    mqttClient.publish("sensors/cmd", `run start ${mode}`);
    console.log(`[RUN] Started: ${mode}`);
    broadcastUpdate();
    res.json({ ok: true, mode, startTime: runs[mode].startTime });
});

app.get("/run/stop", (_req, res) => {
    if (!activeRun) return res.json({ ok: true, message: "No active run" });
    const stopped = activeRun;
    _stopRun(stopped);
    res.json({ ok: true, mode: stopped, summary: buildSummary(stopped) });
});

app.get("/run/stop/:mode", (req, res) => {
    const mode = req.params.mode;
    if (!runs[mode]) return res.status(400).json({ error: "Invalid mode" });
    _stopRun(mode);
    res.json({ ok: true, mode, summary: buildSummary(mode) });
});

function _stopRun(mode) {
    runs[mode].active = false;
    runs[mode].endTime = Date.now();
    if (activeRun === mode) activeRun = null;
    const s = buildSummary(mode);
    console.log(`[RUN] Stopped: ${mode} | pkts=${s.pkts} dupes=${s.dupes} gaps=${s.gaps} delivery=${s.deliveryPct}%`);
    io.emit("run_stopped", { mode, summary: s });
    mqttClient.publish("sensors/cmd", `run stop ${mode}`);
    broadcastUpdate();
}

app.get("/run/stats", (_req, res) => res.json({
    currentMode, activeRun,
    flooding: buildSummary("flooding"),
    routing: buildSummary("routing"),
    allTime
}));

app.get("/reset", (_req, res) => {
    if (activeRun) _stopRun(activeRun);
    runs.flooding = freshRun("flooding");
    runs.routing = freshRun("routing");
    allTime.total = 0; allTime.perNode = { a02: 0, a03: 0, a04: 0, a05: 0, pico: 0 };
    broadcastUpdate();
    console.log("[RESET] Cleared");
    res.json({ ok: true });
});

// GET /cmd/qos0?topic=sensors/cmd&message=led+on  (QoS 0, topic-aware)
app.get("/cmd/qos0", (req, res) => {
    const topic = req.query.topic || "sensors/cmd";
    const message = req.query.message || "";
    if (!["sensors/cmd", "sensors/arduino/cmd", "sensors/pico/cmd"].includes(topic))
        return res.status(400).json({ error: "Invalid topic" });
    mqttClient.publish(topic, message, { qos: 0 });
    console.log(`[CMD QoS0] ${topic} : ${message}`);
    io.emit("mqtt_message", {
        topic, message,
        time: new Date().toLocaleTimeString("en-GB"),
        isCmd: true, mode: currentMode
    });
    res.json({ ok: true, topic, message, qos: 0 });
});

app.get("/cmd/:msg", (req, res) => {
    const msg = decodeURIComponent(req.params.msg);
    mqttClient.publish("sensors/cmd", msg);
    console.log(`[CMD] ${msg}`);
    res.json({ ok: true, cmd: msg });
});

// POST /cmd/qos — publish with explicit QoS level (1 or 2)
// Body: { topic: "sensors/cmd", message: "led on", qos: 1 }
app.post("/cmd/qos", (req, res) => {
    const { topic = "sensors/cmd", message, qos } = req.body;
    if (!message) return res.status(400).json({ error: "message required" });
    const q = [0, 1, 2].includes(Number(qos)) ? Number(qos) : 0;
    mqttClient.publish(topic, message, { qos: q }, (err) => {
        if (err) return res.status(500).json({ error: err.message });
        console.log(`[CMD QoS${q}] ${topic} : ${message}`);
        // Echo to all websocket clients so the feed updates
        io.emit("mqtt_message", {
            topic,
            message,
            time: new Date().toLocaleTimeString("en-GB"),
            isCmd: true,
            mode: currentMode
        });
        res.json({ ok: true, topic, message, qos: q });
    });
});

app.get("/export", (_req, res) => {
    res.setHeader("Content-Disposition",
        `attachment; filename="lora-mesh-${Date.now()}.json"`);
    res.json({
        exportedAt: new Date().toISOString(),
        currentMode, activeRun,
        flooding: buildSummary("flooding"),
        routing: buildSummary("routing"),
        allTime
    });
});

app.get("/health", (_req, res) => res.json({
    ok: true, mqttConnected: mqttClient.connected,
    currentMode, activeRun, uptime: process.uptime()
}));

// ── Start ─────────────────────────────────────────────────────────────────
server.listen(3000, () => {
    console.log("\n[SERVER] http://localhost:3000");
    console.log("  GET /mode/flooding|routing      set mode label");
    console.log("  GET /run/start/flooding|routing start run");
    console.log("  GET /run/stop                   stop active run");
    console.log("  GET /run/stats                  JSON stats");
    console.log("  GET /reset                      clear all stats");
    console.log("  GET /cmd/:msg                   publish to sensors/cmd");
    console.log("  GET /export                     download JSON\n");
});