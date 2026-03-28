import csv
import glob
import statistics

TOPIC_QOS = {
    "sensors/arduino/data": "QoS0",
    "sensors/data":         "QoS1",
}

files = sorted(glob.glob("data/lora_multihop*.csv"))

print("\n===== LORA MULTI-HOP MQTT-SN PERFORMANCE ANALYSIS =====\n")
print(f"{'File':<14} {'QoS':<6} {'Topic':<28} {'Nodes':>6} {'Msgs':>6} {'Duration(s)':>12} {'Throughput':>12} {'AvgInterval':>12} {'Jitter':>10} {'PDR':>8} {'Loss':>8}")
print("-" * 130)

for f in files:
    data = {}  # topic -> list of (timestamp, node, seq)

    with open(f, "r") as file:
        reader = csv.DictReader(file)
        for row in reader:
            topic = row["topic"]
            ts    = float(row["timestamp"])
            node  = row["node"]
            seq   = int(row["seq"])
            if topic not in data:
                data[topic] = []
            data[topic].append((ts, node, seq))

    label = f.replace(".csv", "")[:12]

    for topic, rows in sorted(data.items()):
        qos_label = TOPIC_QOS.get(topic, "unknown")

        if len(rows) < 2:
            print(f"{label:<14} {qos_label:<6} {topic:<28} not enough data")
            continue

        rows.sort(key=lambda x: x[0])
        timestamps = [r[0] for r in rows]
        nodes      = set(r[1] for r in rows)

        messages     = len(timestamps)
        duration     = timestamps[-1] - timestamps[0]
        throughput   = messages / duration

        intervals    = [timestamps[i] - timestamps[i-1] for i in range(1, len(timestamps))]
        avg_interval = sum(intervals) / len(intervals)
        jitter       = statistics.stdev(intervals) if len(intervals) > 1 else 0

        # PDR per node then average
        node_pdrs = []
        for node in nodes:
            node_seqs = sorted([r[2] for r in rows if r[1] == node])
            if len(node_seqs) < 2:
                continue
            gaps = sum(max(0, node_seqs[i] - node_seqs[i-1] - 1) for i in range(1, len(node_seqs)))
            expected = node_seqs[-1] - node_seqs[0] + 1
            node_loss = max(0, gaps / expected)
            node_pdrs.append(1 - node_loss)

        avg_pdr  = sum(node_pdrs) / len(node_pdrs) if node_pdrs else 0
        avg_loss = 1 - avg_pdr

        print(f"{label:<14} {qos_label:<6} {topic:<28} {len(nodes):>6} {messages:>6} {round(duration,2):>12} {round(throughput,4):>12} {round(avg_interval,4):>12} {round(jitter,4):>10} {round(avg_pdr*100,1):>7}% {round(avg_loss*100,1):>7}%")

    print()