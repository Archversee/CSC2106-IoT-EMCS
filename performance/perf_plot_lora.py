import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import glob

files = sorted(glob.glob("lora_test*.csv"))

# ---------- load file ----------
file = files[-1]   # take latest file
data = pd.read_csv(file)

# ---------- split QoS ----------
qos0 = data[data["topic"] == "sensors/arduino/data"]
qos1 = data[data["topic"] == "sensors/data"]

results = {}

def compute_metrics(df, name):

    if len(df) < 2:
        return None

    df = df.sort_values(by="timestamp")

    timestamps = df["timestamp"]

    duration = max(timestamps.iloc[-1] - timestamps.iloc[0], 1e-6)
    throughput = len(timestamps) / duration

    intervals = timestamps.diff().dropna()
    avg_interval = intervals.mean()
    jitter = intervals.std()

    seq_numbers = df["payload"].str.split().str[-1].astype(int)

    expected = seq_numbers.iloc[-1] - seq_numbers.iloc[0] + 1
    received = len(seq_numbers)
    loss = (expected - received) / expected

    return {
        "throughput": throughput,
        "latency": avg_interval,
        "jitter": jitter,
        "loss": loss
    }

# ---------- compute ----------
results["QoS0"] = compute_metrics(qos0, "QoS0")
results["QoS1"] = compute_metrics(qos1, "QoS1")

# ---------- print ----------
print("\n===== LORA SINGLE-HOP ANALYSIS =====\n")

for k, v in results.items():
    if v:
        print(f"{k}:")
        print(f"  Throughput : {v['throughput']:.4f}")
        print(f"  Interval   : {v['latency']:.4f}")
        print(f"  Jitter     : {v['jitter']:.4f}")
        print(f"  Loss       : {v['loss']:.4f}")
        print()

# ---------- prepare for plotting ----------
labels = []
throughputs = []
latencies = []
jitters = []
losses = []

for k, v in results.items():
    if v:
        labels.append(k)
        throughputs.append(v["throughput"])
        latencies.append(v["latency"])
        jitters.append(v["jitter"])
        losses.append(v["loss"])

# ---------- throughput ----------
plt.figure()
plt.bar(labels, throughputs)
plt.title("LoRa Throughput (Single-Hop)")
plt.ylabel("Messages/sec")
plt.savefig("lora_throughput.png")

# ---------- latency ----------
plt.figure()
plt.bar(labels, latencies)
plt.title("LoRa Average Interval")
plt.ylabel("Seconds")
plt.savefig("lora_latency.png")

# ---------- jitter ----------
plt.figure()
plt.bar(labels, jitters)
plt.title("LoRa Jitter")
plt.ylabel("Std Deviation")
plt.savefig("lora_jitter.png")

# ---------- packet loss ----------
plt.figure()
plt.bar(labels, losses)
plt.title("LoRa Packet Loss")
plt.ylabel("Loss Ratio")
plt.savefig("lora_packet_loss.png")

# ---------- timeline ----------
plt.figure()

for label, df in [("QoS0", qos0), ("QoS1", qos1)]:

    if len(df) < 2:
        continue

    seq = df["payload"].str.split().str[-1].astype(int)
    timestamps = df["timestamp"]
    timestamps = timestamps - timestamps.iloc[0]

    plt.scatter(seq, timestamps, label=label)

plt.title("LoRa Timeline (Seq vs Time)")
plt.xlabel("Sequence Number")
plt.ylabel("Time (s)")
plt.legend()

plt.savefig("lora_timeline.png")

plt.show()