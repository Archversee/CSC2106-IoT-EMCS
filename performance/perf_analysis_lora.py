import csv
import glob
import statistics

# Map topic to QoS label
TOPIC_QOS = {
    "sensors/arduino/data": "QoS0",
    "sensors/data":         "QoS1",
}

files = sorted(glob.glob("lora_*.csv"))

print("\n===== LORA MQTT-SN PERFORMANCE ANALYSIS =====\n")
print(f"{'File':<12} {'QoS':<6} {'Topic':<28} {'Msgs':>6} {'Duration(s)':>12} {'Throughput':>12} {'AvgInterval':>12} {'Jitter':>10} {'PDR':>8} {'Loss':>8}")
print("-" * 115)

for f in files:
    data = {}

    with open(f, "r") as file:
        reader = csv.DictReader(file)
        for row in reader:
            topic = row["topic"]
            ts    = float(row["timestamp"])
            seq   = int(row["payload"].strip().split()[-1])
            if topic not in data:
                data[topic] = []
            data[topic].append((ts, seq))

    label = f.replace(".csv", "").split("_", 1)[1][:10]

    for topic, rows in sorted(data.items()):
        qos_label = TOPIC_QOS.get(topic, "unknown")

        if len(rows) < 2:
            print(f"{label:<12} {qos_label:<6} {topic:<28} not enough data")
            continue

        rows.sort(key=lambda x: x[0])
        timestamps = [r[0] for r in rows]
        sequences  = sorted([r[1] for r in rows])

        messages     = len(timestamps)
        duration     = timestamps[-1] - timestamps[0]
        throughput   = messages / duration

        intervals    = [timestamps[i] - timestamps[i-1] for i in range(1, len(timestamps))]
        avg_interval = sum(intervals) / len(intervals)
        jitter       = statistics.stdev(intervals) if len(intervals) > 1 else 0

        gaps     = sum(max(0, sequences[i] - sequences[i-1] - 1) for i in range(1, len(sequences)))
        expected = sequences[-1] - sequences[0] + 1
        loss     = max(0, gaps / expected)
        pdr      = 1 - loss

        print(f"{label:<12} {qos_label:<6} {topic:<28} {messages:>6} {round(duration,2):>12} {round(throughput,4):>12} {round(avg_interval,4):>12} {round(jitter,4):>10} {round(pdr*100,1):>7}% {round(loss*100,1):>7}%")

    print()