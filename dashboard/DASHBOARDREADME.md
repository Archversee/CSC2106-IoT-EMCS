**MQTT Subscriber Dashboard (MVP)**

Small realtime web dashboard to visualise and control Pico W devices via MQTT.
This dashboard is the subscriber/dashboard component of the INF2004 project: it receives telemetry & status messages from devices (via an MQTT broker), renders them in the browser, and lets a user send commands back by publishing MQTT messages. The backend acts as the bridge between the MQTT broker and browser clients using Socket.IO.

⸻

**What’s included**
	•	backend/ — Node.js server (Express + Socket.IO + mqtt) that:
	•	connects to a local MQTT broker (default mqtt://localhost:1883),
	•	subscribes to devices/+/telemetry/# and devices/+/status,
	•	forwards incoming messages to browsers over Socket.IO,
	•	accepts send-cmd socket requests from browsers and publishes to MQTT (supports a QoS1 checkbox and returns publish-status events).
	•	frontend/ — static web UI (HTML + JS) that:
	•	displays devices and last-seen timestamps,
	•	shows recent messages,
	•	renders a simple live telemetry chart,
	•	provides a command form (device id, topic suffix, payload, QoS1).

⸻

**Quick architecture / message flow**
Browser receives messages as socket events (mqtt-message, devices-list, etc.).
	•	Browser emits send-cmd to request that the backend publish to a device topic.
	•	Backend publishes to MQTT and emits publish-status back to the browser with success/failure.

**Setup (local dev)**

Prerequisites
	•	Node.js (v16+ recommended; tested with v24)
	•	npm
	•	Mosquitto (local MQTT broker)
	•	macOS (Homebrew): brew install mosquitto
	•	Start interactively to see logs: mosquitto -v
or run as a service: brew services start mosquitto

**Install backend dependencies**
```cd dashboard/backend
npm install```

**Start the backend**
```cd dashboard/backend
node server.js```

**Open the frontend**
Open: http://localhost:3000 in your browser. The page is served from frontend/.

**Manual tests**
Open a new terminal with mosquitto running and use mosquitto_pub / mosquitto_sub:

**Publish a fake device status**
```mosquitto_pub -t "devices/pico-001/status" \
  -m '{"ts":"2025-10-23T06:00:00Z","status":"online"}'```

**Publish telemetry**
```mosquitto_pub -t "devices/pico-001/telemetry/temp" \
  -m '{"ts":"2025-10-23T06:00:00Z","value":27.9}'```

**Watch broker traffic**
```mosquitto_sub -t "devices/#" -v```

**From the dashboard UI**
	•	Click Refresh — the UI will request the devices list from the backend.
	•	In device id put pico-001, topic suffix put cmd/restart, payload:
```{"action":"restart"}```

**What was implemented**
	•	Backend: Express static server, Socket.IO server, MQTT client that:
	•	subscribes to devices/+/telemetry/# and devices/+/status,
	•	relays messages to connected browser clients,
	•	accepts send-cmd socket events and publishes to MQTT,
	•	emits publish-status to confirm QoS1 or normal publish outcome.
	•	Frontend:
	•	Device list (last-seen timestamps),
	•	Recent messages feed and message formatting,
	•	Simple live telemetry chart (Chart.js or similar) with timestamps as x-axis,
	•	Command UI (device id, topic suffix, JSON payload, QoS1 checkbox),
	•	Optimistic UI when sending commands + feedback when publish-status arrives.
