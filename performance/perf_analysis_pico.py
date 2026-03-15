import csv
import glob

files = sorted(glob.glob("pico_qos*.csv"))

print("\n===== PICO MQTT-SN PERFORMANCE ANALYSIS =====\n")

print("Test\tMessages\tDuration(s)\tThroughput(msg/s)\tAvg Interval(s)")

for f in files:

    timestamps = []

    with open(f,"r") as file:

        reader = csv.DictReader(file)

        for row in reader:
            timestamps.append(float(row["timestamp"]))

    if len(timestamps) < 2:
        print(f,"Not enough data")
        continue

    messages = len(timestamps)

    duration = timestamps[-1] - timestamps[0]

    throughput = messages / duration

    intervals = []

    for i in range(1,len(timestamps)):
        intervals.append(timestamps[i]-timestamps[i-1])

    avg_interval = sum(intervals)/len(intervals)

    label = f.split("_")[1]

    print(
        label,
        "\t",
        messages,
        "\t\t",
        round(duration,2),
        "\t\t",
        round(throughput,2),
        "\t\t",
        round(avg_interval,3)
    )
