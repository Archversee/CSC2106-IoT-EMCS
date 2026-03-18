import time
import csv
import paho.mqtt.client as mqtt
from datetime import datetime

print("=== LoRa MQTT-SN Performance Collector ===")

filename = f"lora_test_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"

file = open(filename, "w", newline="")
writer = csv.writer(file)

writer.writerow(["timestamp", "topic", "payload"])

print("\nSaving results to:", filename)
print("Waiting for LoRa messages... (CTRL+C to stop)\n")

def on_message(client, userdata, msg):
    ts = time.time()
    payload = msg.payload.decode()

    print(ts, msg.topic, payload)

    writer.writerow([ts, msg.topic, payload])
    file.flush()

client = mqtt.Client()

# connect to local broker
client.connect("localhost", 1883)

client.subscribe("sensors/#")

client.on_message = on_message

client.loop_forever()
