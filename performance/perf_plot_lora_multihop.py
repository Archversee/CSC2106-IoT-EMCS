import pandas as pd
import matplotlib.pyplot as plt
import glob

files = sorted(glob.glob("data/lora_multihop*.csv"))
file = files[-1]
data = pd.read_csv(file)

# split QoS by topic
qos0 = data[data["topic"] == "sensors/arduino/data"].sort_values("timestamp")
qos1 = data[data["topic"] == "sensors/data"].sort_values("timestamp")

results = {}

def compute_metrics(df):
    if len(df) < 2:
        return None

    timestamps = df["timestamp"]
    duration   = max(timestamps.iloc[-1] - timestamps.iloc[0], 1e-6)
    throughput = len(timestamps) / duration
    intervals  = timestamps.diff().dropna()
    avg_interval = intervals.mean()
    jitter     = intervals.std()

    # PDR per node then average
    node_pdrs = []
    for node, grp in df.groupby("node"):
        seqs = sorted(grp["seq"].tolist())
        if len(seqs) < 2:
            continue
        gaps = sum(max(0, seqs[i] - seqs[i-1] - 1) for i in range(1, len(seqs)))
        expected = seqs[-1] - seqs[0] + 1
        node_pdrs.append(max(0, 1 - gaps / expected))

    avg_pdr  = sum(node_pdrs) / len(node_pdrs) if node_pdrs else 1.0
    avg_loss = 1 - avg_pdr

    return {
        "throughput": throughput,
        "latency":    avg_interval,
        "jitter":     jitter,
        "loss":       avg_loss
    }

results["QoS0"] = compute_metrics(qos0)
results["QoS1"] = compute_metrics(qos1)

print("\n===== LORA MULTI-HOP ANALYSIS =====\n")
for k, v in results.items():
    if v:
        print(f"{k}:")
        print(f"  Throughput : {v['throughput']:.4f}")
        print(f"  Interval   : {v['latency']:.4f}")
        print(f"  Jitter     : {v['jitter']:.4f}")
        print(f"  Loss       : {v['loss']:.4f}")
        print()

labels, throughputs, latencies, jitters, losses = [], [], [], [], []
for k, v in results.items():
    if v:
        labels.append(k)
        throughputs.append(v["throughput"])
        latencies.append(v["latency"])
        jitters.append(v["jitter"])
        losses.append(v["loss"])

plt.figure()
plt.bar(labels, throughputs, color="#9C27B0")
plt.title("LoRa Throughput (Multi-Hop)")
plt.ylabel("Messages/sec")
plt.savefig("charts/lora_multihop_throughput.png")

plt.figure()
plt.bar(labels, latencies, color="#9C27B0")
plt.title("LoRa Average Interval (Multi-Hop)")
plt.ylabel("Seconds")
plt.savefig("charts/lora_multihop_latency.png")

plt.figure()
plt.bar(labels, jitters, color="#9C27B0")
plt.title("LoRa Jitter (Multi-Hop)")
plt.ylabel("Std Deviation")
plt.savefig("charts/lora_multihop_jitter.png")

plt.figure()
plt.bar(labels, losses, color="#9C27B0")
plt.title("LoRa Packet Loss (Multi-Hop)")
plt.ylabel("Loss Ratio")
plt.savefig("charts/lora_multihop_packet_loss.png")

plt.figure()
colors = plt.cm.tab10.colors
for i, (node, grp) in enumerate(data.groupby("node")):
    for label, topic in [("QoS0", "sensors/arduino/data"), ("QoS1", "sensors/data")]:
        sub = grp[grp["topic"] == topic].sort_values("timestamp")
        if len(sub) < 1:
            continue
        ts = sub["timestamp"] - data["timestamp"].min()
        plt.scatter(sub["seq"], ts, label=f"{node} {label}", color=colors[i % len(colors)],
                    marker="o" if label == "QoS0" else "^", s=30)

plt.title("LoRa Timeline (Multi-Hop, Seq vs Time)")
plt.xlabel("Sequence Number")
plt.ylabel("Time (s)")
plt.legend(fontsize=7, ncol=2)
plt.savefig("charts/lora_multihop_timeline.png")
