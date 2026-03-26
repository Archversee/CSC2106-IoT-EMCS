import csv
import glob
import statistics

files = sorted(glob.glob("pico_qos*.csv"))

print("\n===== PICO MQTT-SN PERFORMANCE ANALYSIS =====\n")
print(f"{'Test':<8} {'Msgs':>6} {'Duration(s)':>12} {'Throughput':>12} {'AvgInterval':>12} {'Jitter':>8} {'PDR':>8} {'Loss':>8}")
print("-" * 80)

for f in files:
    timestamps = []
    sequences = []

    with open(f, "r") as file:
        reader = csv.DictReader(file)
        for row in reader:
            timestamps.append(float(row["timestamp"]))
            seq = int(row["payload"].strip().split()[-1])
            sequences.append(seq)

    if len(timestamps) < 2:
        print(f, "Not enough data")
        continue

    messages  = len(timestamps)
    duration  = timestamps[-1] - timestamps[0]
    throughput = messages / duration

    intervals = [timestamps[i] - timestamps[i-1] for i in range(1, len(timestamps))]
    avg_interval = sum(intervals) / len(intervals)
    jitter = statistics.stdev(intervals)

    sequences.sort()
    gaps = sum(max(0, sequences[i] - sequences[i-1] - 1) for i in range(1, len(sequences)))
    expected = sequences[-1] - sequences[0] + 1
    loss = gaps / expected
    pdr = 1 - loss

    label = f.split("_")[1]

    print(f"{label:<8} {messages:>6} {round(duration,2):>12} {round(throughput,4):>12} {round(avg_interval,4):>12} {round(jitter,4):>8} {round(pdr*100,1):>7}% {round(loss*100,1):>7}%")