#!/bin/bash
# ─────────────────────────────────────────────────────────────
#  start_emcs.sh — CSC2106 IoT EMCS Full Stack Launcher
#  Opens each service in its own terminal window
#
#  Usage:
#    chmod +x start_emcs.sh
#    ./start_emcs.sh
# ─────────────────────────────────────────────────────────────

BASE="$HOME/Documents/CSC2106-IoT-EMCS"

# Detect which terminal emulator is available
if command -v lxterminal &>/dev/null; then
    TERM_CMD="lxterminal --title"
    TERM_EXEC="-e"
elif command -v xterm &>/dev/null; then
    TERM_CMD="xterm -title"
    TERM_EXEC="-e"
elif command -v gnome-terminal &>/dev/null; then
    TERM_CMD="gnome-terminal --title"
    TERM_EXEC="--"
else
    echo "[!] No supported terminal emulator found (lxterminal, xterm, gnome-terminal)"
    exit 1
fi

echo "[*] Starting CSC2106 EMCS services..."

# ── Terminal 1: Mosquitto MQTT Broker ────────────────────────
echo "[1] Starting Mosquitto..."
$TERM_CMD "1 - Mosquitto MQTT Broker" $TERM_EXEC bash -c "
    sudo systemctl stop mosquitto 2>/dev/null
    sleep 1
    echo '=== Mosquitto MQTT Broker ==='
    mosquitto -v
    bash
" &
sleep 3

# ── Terminal 2: MQTT-SN Gateway ──────────────────────────────
echo "[2] Starting MQTT-SN Gateway..."
$TERM_CMD "2 - MQTT-SN Gateway" $TERM_EXEC bash -c "
    echo '=== MQTT-SN Gateway ==='
    cd $BASE/paho.mqtt-sn.embedded-c/MQTTSNGateway/bin
    ./MQTT-SNGateway
    bash
" &
sleep 2

# ── Terminal 3: LoRa UDP Gateway ─────────────────────────────
echo "[3] Starting LoRa Gateway..."
$TERM_CMD "3 - LoRa Gateway" $TERM_EXEC bash -c "
    echo '=== LoRa UDP Gateway ==='
    cd $BASE/Waveshare1121-Gateway
    make run
    bash
" &
sleep 2

# ── Terminal 4: Dashboard Backend ────────────────────────────
echo "[4] Starting Dashboard Backend..."
$TERM_CMD "4 - Dashboard Backend" $TERM_EXEC bash -c "
    echo '=== Dashboard Backend ==='
    cd $BASE/dashboard/backend
    node server.js
    bash
" &
sleep 1

# ── Terminal 5: Performance Tools (manual) ───────────────────
echo "[5] Opening Performance terminal..."
$TERM_CMD "5 - Performance Collector" $TERM_EXEC bash -c "
    cd $BASE/performance
    echo ''
    echo '=== Performance Tools Ready ==='
    echo ''
    echo 'Flooding : python3 perf_collector_lora.py --mode flooding --duration 300'
    echo 'Routing  : python3 perf_collector_lora.py --mode routing  --duration 300'
    echo 'Analysis : python3 perf_analysis_lora.py'
    echo ''
    bash
" &

echo ""
echo "  All terminals launched!"
echo "  Dashboard: http://localhost:3000"
echo ""
