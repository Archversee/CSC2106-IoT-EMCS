import pandas as pd
import glob
import matplotlib.pyplot as plt
import numpy as np

files = sorted(glob.glob("pico_qos*.csv"))

labels=[]
throughputs=[]
latencies=[]
jitters=[]
loss_rates=[]

# ---------- metrics ----------
for f in files:

    data=pd.read_csv(f)

    timestamps=data["timestamp"]

    duration=timestamps.iloc[-1]-timestamps.iloc[0]

    throughput=len(timestamps)/duration

    intervals=timestamps.diff().dropna()

    avg_interval=intervals.mean()

    jitter=intervals.std()

    seq_numbers=data["payload"].str.split().str[-1].astype(int)

    expected=max(seq_numbers)-min(seq_numbers)+1

    received=len(seq_numbers)

    loss=(expected-received)/expected

    label=f.split("_")[1]

    labels.append(label)
    throughputs.append(throughput)
    latencies.append(avg_interval)
    jitters.append(jitter)
    loss_rates.append(loss)


# ---------- throughput ----------
plt.figure()
plt.bar(labels,throughputs)
plt.title("Pico MQTT-SN Throughput")
plt.ylabel("Messages/sec")
plt.savefig("pico_throughput.png")


# ---------- latency ----------
plt.figure()
plt.bar(labels,latencies)
plt.title("Average Message Interval")
plt.ylabel("Seconds")
plt.savefig("pico_latency.png")


# ---------- jitter ----------
plt.figure()
plt.bar(labels,jitters)
plt.title("Message Jitter")
plt.ylabel("Std Deviation")
plt.savefig("pico_jitter.png")


# ---------- packet loss ----------
plt.figure()
plt.bar(labels,loss_rates)
plt.title("Packet Loss Rate")
plt.ylabel("Loss Ratio")
plt.savefig("pico_packet_loss.png")


# ---------- timeline plot (ADVANCED GRAPH) ----------
plt.figure()

for f in files:

    data=pd.read_csv(f)

    seq=data["payload"].str.split().str[-1].astype(int)

    timestamps=data["timestamp"]

    timestamps=timestamps-timestamps.iloc[0]

    label=f.split("_")[1]

    plt.scatter(seq,timestamps,label=label)

plt.title("Message Timeline (Sequence vs Time)")
plt.xlabel("Message Sequence Number")
plt.ylabel("Time Since First Message (s)")
plt.legend()

plt.savefig("pico_message_timeline.png")

plt.show()