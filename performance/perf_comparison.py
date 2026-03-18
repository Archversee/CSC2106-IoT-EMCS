import pandas as pd
import matplotlib.pyplot as plt
import glob

# ---------- load pico data ----------
pico_files = sorted(glob.glob("pico_qos*.csv"))
pico_results = {}

for f in pico_files:
    label = f.split("_")[1].upper()
    data = pd.read_csv(f)
    timestamps = data["timestamp"]
    duration = timestamps.iloc[-1] - timestamps.iloc[0]
    throughput = len(timestamps) / duration
    intervals = timestamps.diff().dropna()
    avg_interval = intervals.mean()
    jitter = intervals.std()

    # FIXED: count actual gaps
    seq = data["payload"].str.split().str[-1].astype(int).reset_index(drop=True).sort_values()
    gaps = 0
    for i in range(1, len(seq)):
        gap = seq.iloc[i] - seq.iloc[i-1] - 1
        if gap > 0:
            gaps += gap
    expected = seq.iloc[-1] - seq.iloc[0] + 1
    loss = max(0, gaps / expected)

    pico_results[label] = {
        "throughput": throughput,
        "latency": avg_interval,
        "jitter": jitter,
        "loss": loss
    }

# ---------- load lora data ----------
lora_files = sorted(glob.glob("lora_test*.csv"))
lora_data = pd.read_csv(lora_files[-1])

lora_qos0 = lora_data[lora_data["topic"] == "sensors/arduino/data"].sort_values("timestamp")
lora_qos1 = lora_data[lora_data["topic"] == "sensors/data"].sort_values("timestamp")

lora_results = {}

for label, df in [("QOS0", lora_qos0), ("QOS1", lora_qos1)]:
    if len(df) < 2:
        continue
    timestamps = df["timestamp"]
    duration = timestamps.iloc[-1] - timestamps.iloc[0]
    throughput = len(timestamps) / duration
    intervals = timestamps.diff().dropna()
    avg_interval = intervals.mean()
    jitter = intervals.std()

    # FIXED: count actual gaps
    seq = df["payload"].str.split().str[-1].astype(int).reset_index(drop=True).sort_values()
    gaps = 0
    for i in range(1, len(seq)):
        gap = seq.iloc[i] - seq.iloc[i-1] - 1
        if gap > 0:
            gaps += gap
    expected = seq.iloc[-1] - seq.iloc[0] + 1
    loss = max(0, gaps / expected)

    lora_results[label] = {
        "throughput": throughput,
        "latency": avg_interval,
        "jitter": jitter,
        "loss": loss
    }

# ---------- print summary ----------
print("\n===== WIFI vs LORA SINGLE HOP COMPARISON =====\n")
print(f"{'Metric':<20} {'WiFi QoS0':>12} {'WiFi QoS1':>12} {'WiFi QoS2':>12} {'LoRa QoS0':>12} {'LoRa QoS1':>12}")
print("-" * 72)

metrics = [
    ("Throughput(msg/s)", "throughput", "{:.4f}"),
    ("Avg Interval(s)",   "latency",    "{:.4f}"),
    ("Jitter(s)",         "jitter",     "{:.4f}"),
    ("Packet Loss",       "loss",       "{:.4f}"),
]

for name, key, fmt in metrics:
    wifi0 = fmt.format(pico_results.get("QOS0", {}).get(key, 0))
    wifi1 = fmt.format(pico_results.get("QOS1", {}).get(key, 0))
    wifi2 = fmt.format(pico_results.get("QOS2", {}).get(key, 0))
    lora0 = fmt.format(lora_results.get("QOS0", {}).get(key, 0))
    lora1 = fmt.format(lora_results.get("QOS1", {}).get(key, 0))
    print(f"{name:<20} {wifi0:>12} {wifi1:>12} {wifi2:>12} {lora0:>12} {lora1:>12}")

# ---------- comparison charts ----------
fig, axes = plt.subplots(2, 2, figsize=(12, 8))
fig.suptitle("WiFi (Pico) vs LoRa Single-Hop (Arduino) — Transport Comparison", fontsize=13)

wifi_labels = list(pico_results.keys())
lora_labels = list(lora_results.keys())
wifi_color = "#2196F3"
lora_color = "#FF9800"

def plot_comparison(ax, title, ylabel, key):
    x = range(len(wifi_labels))
    lora_x = range(len(lora_labels))
    ax.bar([i - 0.2 for i in x],
           [pico_results[k][key] for k in wifi_labels],
           width=0.4, label="WiFi", color=wifi_color, alpha=0.8)
    ax.bar([i + 0.2 for i in lora_x],
           [lora_results[k][key] for k in lora_labels],
           width=0.4, label="LoRa", color=lora_color, alpha=0.8)
    ax.set_title(title)
    ax.set_ylabel(ylabel)
    ax.set_xticks(list(x))
    ax.set_xticklabels(wifi_labels)
    ax.legend()
    ax.grid(axis="y", linestyle="--", alpha=0.4)

plot_comparison(axes[0][0], "Throughput (msg/s)",      "Messages/sec", "throughput")
plot_comparison(axes[0][1], "Avg Message Interval (s)", "Seconds",      "latency")
plot_comparison(axes[1][0], "Jitter (Std Deviation)",   "Seconds",      "jitter")
plot_comparison(axes[1][1], "Packet Loss Ratio",        "Loss Ratio",   "loss")

plt.tight_layout()
plt.savefig("comparison_wifi_vs_lora.png", dpi=150)
print("\nSaved → comparison_wifi_vs_lora.png")
plt.show()