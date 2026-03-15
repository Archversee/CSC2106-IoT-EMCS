import time
import csv
import paho.mqtt.client as mqtt
from datetime import datetime

print("=== Pico MQTT-SN Performance Collector ===")
print("Enter QoS level to test:")
print("0 → QoS0")
print("1 → QoS1")
print("2 → QoS2")

qos_input = input("QoS Level: ").strip()

if qos_input not in ["0","1","2"]:
    print("Invalid QoS level")
    exit()

QOS_LEVEL = f"qos{qos_input}"

filename = f"pico_{QOS_LEVEL}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"

file = open(filename,"w",newline="")
writer = csv.writer(file)

writer.writerow(["timestamp","topic","payload"])

print("\nSaving results to:", filename)
print("Press GP20 on Pico to send messages")
print("Press CTRL+C to stop\n")

def on_message(client,userdata,msg):

    ts = time.time()
    payload = msg.payload.decode()

    print(ts,msg.topic,payload)

    writer.writerow([ts,msg.topic,payload])
    file.flush()

client = mqtt.Client()

client.connect("localhost",1883)

client.subscribe("sensors/data")
client.subscribe("sensors/pico/data")

client.on_message = on_message

client.loop_forever()
