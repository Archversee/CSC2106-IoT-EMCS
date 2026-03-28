"""
perf_collector_lora.py — LoRa Mesh Performance Collector

Usage:
    python3 perf_collector_lora.py --mode flooding --duration 300
    python3 perf_collector_lora.py --mode routing  --duration 300

Saves a CSV with columns:
    timestamp, elapsed, mode, topic, node, seq, payload

Run A (Flooding):  flash nodes with MESH_MODE_FLOODING, then run this
Run B (Routing):   flash nodes with MESH_MODE_ROUTING,  then run this

Recommended duration: 300s (5 min) per mode for ~7-8 full publish cycles.
Minimum:             180s (3 min) for ~4-5 cycles.
"""

import time
import csv
import argparse
import paho.mqtt.client as mqtt
from datetime import datetime

# ── Args ──────────────────────────────────────────────────────────────────
parser = argparse.ArgumentParser()
parser.add_argument("--mode",     default="flooding", choices=["flooding","routing"],
                    help="Label for this run (flooding or routing)")
parser.add_argument("--duration", default=300, type=int,
                    help="How many seconds to collect (default: 300)")
parser.add_argument("--broker",   default="localhost",
                    help="MQTT broker host (default: localhost)")
args = parser.parse_args()

# ── Setup ─────────────────────────────────────────────────────────────────
filename  = f"lora_{args.mode}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
start_ts  = None
msg_count = 0

print(f"\n=== LoRa Performance Collector ===")
print(f"  Mode     : {args.mode.upper()}")
print(f"  Duration : {args.duration}s ({args.duration//60}m {args.duration%60}s)")
print(f"  Output   : {filename}")
print(f"\nWaiting for first message...\n")

file   = open(filename, "w", newline="")
writer = csv.writer(file)
writer.writerow(["timestamp", "elapsed_s", "mode", "topic", "node", "seq", "payload"])

# ── Parse "arduino-02 5" → ("arduino-02", 5) ─────────────────────────────
def parse_payload(raw):
    parts = raw.strip().split()
    node  = parts[0] if parts else ""
    try:
        seq = int(parts[1]) if len(parts) > 1 else None
    except ValueError:
        seq = None
    return node, seq

# ── MQTT callbacks ────────────────────────────────────────────────────────
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"[MQTT] Connected to {args.broker}:1883")
        client.subscribe("sensors/#")
        print(f"[MQTT] Subscribed to sensors/#")
    else:
        print(f"[MQTT] Connection failed: rc={rc}")

def on_message(client, userdata, msg):
    global start_ts, msg_count

    now = time.time()
    raw = msg.payload.decode().strip()

    # Skip command messages
    if "cmd" in msg.topic:
        return

    # Start timing on first real message
    if start_ts is None:
        start_ts = now
        print(f"[START] First message received — collecting for {args.duration}s")

    elapsed = now - start_ts
    node, seq = parse_payload(raw)
    msg_count += 1

    # Print progress every 10 messages
    remaining = args.duration - elapsed
    if msg_count % 10 == 0 or msg_count <= 5:
        print(f"  [{elapsed:6.1f}s / {args.duration}s remaining {remaining:.0f}s] "
              f"#{msg_count:4d}  {msg.topic:<28} {raw}")

    writer.writerow([
        f"{now:.3f}",
        f"{elapsed:.3f}",
        args.mode,
        msg.topic,
        node,
        seq if seq is not None else "",
        raw
    ])
    file.flush()

    # Auto-stop after duration
    if elapsed >= args.duration:
        print(f"\n[DONE] {args.duration}s elapsed — {msg_count} messages collected")
        print(f"[DONE] Saved to: {filename}")
        print_summary()
        client.disconnect()

def print_summary():
    """Re-read the written CSV and print a quick summary."""
    file.flush()
    import collections
    nodes    = collections.Counter()
    topics   = collections.Counter()
    seq_data = {}   # (topic, node) → list of seq nums

    file.seek(0)
    import csv as _csv
    reader = _csv.DictReader(file)
    rows   = list(reader)

    for row in rows:
        nodes[row["node"]]   += 1
        topics[row["topic"]] += 1
        key = (row["topic"], row["node"])
        try:
            s = int(row["seq"])
            seq_data.setdefault(key, []).append(s)
        except (ValueError, KeyError):
            pass

    print(f"\n{'─'*50}")
    print(f"  SUMMARY — {args.mode.upper()} mode ({len(rows)} packets)")
    print(f"{'─'*50}")
    print(f"  {'Node':<20} {'Pkts':>6}")
    for node, count in sorted(nodes.items()):
        print(f"  {node:<20} {count:>6}")

    print(f"\n  {'Topic':<35} {'Pkts':>6}")
    for topic, count in sorted(topics.items()):
        print(f"  {topic:<35} {count:>6}")

    # Gap / dupe analysis per (topic, node)
    total_gaps  = 0
    total_dupes = 0
    print(f"\n  {'Track (topic::node)':<45} {'Gaps':>5} {'Dupes':>6}")
    for (topic, node), seqs in sorted(seq_data.items()):
        seqs_sorted = seqs  # arrival order, not sorted
        gaps  = 0
        dupes = 0
        seen  = set()
        last  = None
        for s in seqs_sorted:
            if s in seen:
                dupes += 1
            else:
                seen.add(s)
                if last is not None and s > last + 1:
                    gaps += s - last - 1
                if last is None or s > last:
                    last = s
        total_gaps  += gaps
        total_dupes += dupes
        label = f"{topic}::{node}"
        print(f"  {label:<45} {gaps:>5} {dupes:>6}")

    expected = len(rows) + total_gaps
    delivery = (len(rows) / expected * 100) if expected > 0 else 0
    print(f"\n  Total gaps (lost pkts) : {total_gaps}")
    print(f"  Total dupes            : {total_dupes}")
    print(f"  Delivery rate          : {delivery:.1f}%  ({len(rows)}/{expected})")
    print(f"{'─'*50}\n")

# ── Run ───────────────────────────────────────────────────────────────────
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

try:
    client.connect(args.broker, 1883)
    client.loop_forever()
except KeyboardInterrupt:
    print(f"\n[STOPPED] Ctrl+C — {msg_count} messages collected")
    print(f"[SAVED]   {filename}")
    if msg_count > 0:
        print_summary()
finally:
    file.close()