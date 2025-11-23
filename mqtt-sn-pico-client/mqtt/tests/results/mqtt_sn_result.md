================================================================================
              AUTOMATED MQTT-SN PROTOCOL UNIT TESTS
================================================================================
Test Suite: MQTT-SN Client Implementation
Based on: MQTT-SN Protocol Specification v1.2
Date: Nov 23 2025
================================================================================

Initializing Wi-Fi...
Connecting to Wi-Fi (SSID: Neo)...
✓ Wi-Fi connected: 192.168.28.38
✓ UDP client ready
Gateway: 192.168.28.20:10000


=== TEST UT-CONN: Connection Establishment ===
Target executions: 1
Sent CONNECT as 'pico_w_rx'
CONNACK: return_code=0 (Accepted)
✓ PASS: UT-CONN completed successfully

=== TEST UT-MSGID: Message ID Generation ===
Target executions: 50
[UT-MSGID] Iteration 10/50
[UT-MSGID] Iteration 20/50
[UT-MSGID] Iteration 30/50
[UT-MSGID] Iteration 40/50
[UT-MSGID] Iteration 50/50
✓ PASS: UT-MSGID completed successfully

=== TEST UT-REG: Topic Registration ===
Target executions: 1
Sent REGISTER for topic 'test/topic' (msg_id=101)
✓ REGACK: Topic registered successfully with ID 1 (msg_id=101)
✓ PASS: UT-REG completed successfully

=== TEST UT-SUB: Subscribe Operations ===
Target executions: 1
Sent SUBSCRIBE for topic 'sensor/temperature' (QoS 1, msg_id=102)
SUBACK: topic_id=2, msg_id=102, return_code=0
✓ PASS: UT-SUB completed successfully

=== TEST UT-QOS0: QoS 0 Publishing ===
Target executions: 20
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 103, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 104, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 105, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 106, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 107, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
[UT-QOS0] Iteration 5/20
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 108, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 109, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 110, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 111, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 112, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
[UT-QOS0] Iteration 10/20
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 113, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 114, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 115, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 116, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 117, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
[UT-QOS0] Iteration 15/20
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 118, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 119, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 120, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 121, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
Sent PUBLISH to Topic ID 2 (QoS 0, Msg ID 122, Len 17)
PUBLISH received (QoS 0, Msg ID 0), Payload (10 bytes):
[UT-QOS0] Iteration 20/20
✓ PASS: UT-QOS0 completed successfully

=== TEST UT-QOS1: QoS 1 Publishing ===
Target executions: 25
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 123, Len 17)
PUBLISH received (QoS 1, Msg ID 1), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 1, rc: 0)
PUBACK received for Msg ID: 123
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 124, Len 17)
PUBLISH received (QoS 1, Msg ID 2), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 2, rc: 0)
PUBACK received for Msg ID: 124
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 125, Len 17)
PUBLISH received (QoS 1, Msg ID 3), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 3, rc: 0)
PUBACK received for Msg ID: 125
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 126, Len 17)
PUBLISH received (QoS 1, Msg ID 4), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 4, rc: 0)
PUBACK received for Msg ID: 126
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 127, Len 17)
PUBLISH received (QoS 1, Msg ID 5), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 5, rc: 0)
PUBACK received for Msg ID: 127
[UT-QOS1] Iteration 5/25
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 128, Len 17)
PUBLISH received (QoS 1, Msg ID 6), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 6, rc: 0)
PUBACK received for Msg ID: 128
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 129, Len 17)
PUBLISH received (QoS 1, Msg ID 7), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 7, rc: 0)
PUBACK received for Msg ID: 129
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 130, Len 17)
PUBLISH received (QoS 1, Msg ID 8), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 8, rc: 0)
PUBACK received for Msg ID: 130
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 131, Len 17)
PUBLISH received (QoS 1, Msg ID 9), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 9, rc: 0)
PUBACK received for Msg ID: 131
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 132, Len 17)
PUBLISH received (QoS 1, Msg ID 10), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 10, rc: 0)
PUBACK received for Msg ID: 132
[UT-QOS1] Iteration 10/25
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 133, Len 17)
PUBLISH received (QoS 1, Msg ID 11), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 11, rc: 0)
PUBACK received for Msg ID: 133
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 134, Len 17)
PUBLISH received (QoS 1, Msg ID 12), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 12, rc: 0)
PUBACK received for Msg ID: 134
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 135, Len 17)
PUBLISH received (QoS 1, Msg ID 13), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 13, rc: 0)
PUBACK received for Msg ID: 135
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 136, Len 17)
PUBLISH received (QoS 1, Msg ID 14), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 14, rc: 0)
PUBACK received for Msg ID: 136
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 137, Len 17)
PUBLISH received (QoS 1, Msg ID 15), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 15, rc: 0)
PUBACK received for Msg ID: 137
[UT-QOS1] Iteration 15/25
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 138, Len 17)
PUBLISH received (QoS 1, Msg ID 16), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 16, rc: 0)
PUBACK received for Msg ID: 138
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 139, Len 17)
PUBLISH received (QoS 1, Msg ID 17), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 17, rc: 0)
PUBACK received for Msg ID: 139
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 140, Len 17)
PUBLISH received (QoS 1, Msg ID 18), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 18, rc: 0)
PUBACK received for Msg ID: 140
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 141, Len 17)
PUBLISH received (QoS 1, Msg ID 19), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 19, rc: 0)
PUBACK received for Msg ID: 141
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 142, Len 17)
PUBLISH received (QoS 1, Msg ID 20), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 20, rc: 0)
PUBACK received for Msg ID: 142
[UT-QOS1] Iteration 20/25
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 143, Len 17)
PUBLISH received (QoS 1, Msg ID 21), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 21, rc: 0)
PUBACK received for Msg ID: 143
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 144, Len 17)
PUBLISH received (QoS 1, Msg ID 22), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 22, rc: 0)
PUBACK received for Msg ID: 144
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 145, Len 17)
PUBLISH received (QoS 1, Msg ID 23), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 23, rc: 0)
PUBACK received for Msg ID: 145
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 146, Len 17)
PUBLISH received (QoS 1, Msg ID 24), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 24, rc: 0)
PUBACK received for Msg ID: 146
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 147, Len 17)
PUBLISH received (QoS 1, Msg ID 25), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 25, rc: 0)
PUBACK received for Msg ID: 147
[UT-QOS1] Iteration 25/25
✓ PASS: UT-QOS1 completed successfully

=== TEST UT-QOS2: QoS 2 Publishing ===
Target executions: 15
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 148, Len 17)
PUBREC received for Msg ID: 148. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 26), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 26, rc: 0)
PUBCOMP received for Msg ID: 148
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 149, Len 17)
PUBREC received for Msg ID: 149. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 27), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 27, rc: 0)
PUBCOMP received for Msg ID: 149
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 150, Len 17)
PUBREC received for Msg ID: 150. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 28), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 28, rc: 0)
PUBCOMP received for Msg ID: 150
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 151, Len 17)
PUBREC received for Msg ID: 151. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 29), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 29, rc: 0)
PUBCOMP received for Msg ID: 151
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 152, Len 17)
PUBREC received for Msg ID: 152. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 30), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 30, rc: 0)
PUBCOMP received for Msg ID: 152
[UT-QOS2] Iteration 5/15
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 153, Len 17)
PUBREC received for Msg ID: 153. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 31), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 31, rc: 0)
PUBCOMP received for Msg ID: 153
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 154, Len 17)
PUBREC received for Msg ID: 154. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 32), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 32, rc: 0)
PUBCOMP received for Msg ID: 154
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 155, Len 17)
PUBREC received for Msg ID: 155. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 33), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 33, rc: 0)
PUBCOMP received for Msg ID: 155
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 156, Len 17)
PUBREC received for Msg ID: 156. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 34), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 34, rc: 0)
PUBCOMP received for Msg ID: 156
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 157, Len 17)
PUBREC received for Msg ID: 157. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 35), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 35, rc: 0)
PUBCOMP received for Msg ID: 157
[UT-QOS2] Iteration 10/15
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 158, Len 17)
PUBREC received for Msg ID: 158. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 36), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 36, rc: 0)
PUBCOMP received for Msg ID: 158
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 159, Len 17)
PUBREC received for Msg ID: 159. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 37), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 37, rc: 0)
PUBCOMP received for Msg ID: 159
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 160, Len 17)
PUBREC received for Msg ID: 160. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 38), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 38, rc: 0)
PUBCOMP received for Msg ID: 160
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 161, Len 17)
PUBREC received for Msg ID: 161. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 39), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 39, rc: 0)
PUBCOMP received for Msg ID: 161
Sent PUBLISH to Topic ID 2 (QoS 2, Msg ID 162, Len 17)
PUBREC received for Msg ID: 162. Sending PUBREL...
PUBLISH received (QoS 1, Msg ID 40), Payload (10 bytes):
Sent PUBACK (topic_id: 2, msg_id: 40, rc: 0)
PUBCOMP received for Msg ID: 162
[UT-QOS2] Iteration 15/15
✓ PASS: UT-QOS2 completed successfully

=== TEST UT-PING: Keepalive Mechanism ===
Target executions: 10
Sent PINGREQ
Received PINGRESP
Sent PINGREQ
Received PINGRESP
[UT-PING] Iteration 2/10
Sent PINGREQ
Received PINGRESP
Sent PINGREQ
Received PINGRESP
[UT-PING] Iteration 4/10
Sent PINGREQ
Received PINGRESP
Sent PINGREQ
Received PINGRESP
[UT-PING] Iteration 6/10
Sent PINGREQ
Received PINGRESP
Sent PINGREQ
Received PINGRESP
[UT-PING] Iteration 8/10
Sent PINGREQ
Received PINGRESP
Sent PINGREQ
Received PINGRESP
[UT-PING] Iteration 10/10
✓ PASS: UT-PING completed successfully

=== TEST UT-BINARY: Large Binary Payload ===
Target executions: 10
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 163, Len 254)
PUBLISH received (QoS 1, Msg ID 41), Payload (247 bytes):
Sent PUBACK (topic_id: 2, msg_id: 41, rc: 0)
PUBACK received for Msg ID: 163
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 164, Len 254)
PUBLISH received (QoS 1, Msg ID 42), Payload (247 bytes):
Sent PUBACK (topic_id: 2, msg_id: 42, rc: 0)
PUBACK received for Msg ID: 164
[UT-BINARY] Iteration 2/10 (247 bytes)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 165, Len 254)
PUBLISH received (QoS 1, Msg ID 43), Payload (247 bytes):
Sent PUBACK (topic_id: 2, msg_id: 43, rc: 0)
PUBACK received for Msg ID: 165
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 166, Len 254)
PUBLISH received (QoS 1, Msg ID 44), Payload (247 bytes):
Sent PUBACK (topic_id: 2, msg_id: 44, rc: 0)
PUBACK received for Msg ID: 166
[UT-BINARY] Iteration 4/10 (247 bytes)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 167, Len 254)
PUBLISH received (QoS 1, Msg ID 45), Payload (247 bytes):
Sent PUBACK (topic_id: 2, msg_id: 45, rc: 0)
PUBACK received for Msg ID: 167
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 168, Len 254)
PUBLISH received (QoS 1, Msg ID 46), Payload (247 bytes):
Sent PUBACK (topic_id: 2, msg_id: 46, rc: 0)
PUBACK received for Msg ID: 168
[UT-BINARY] Iteration 6/10 (247 bytes)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 169, Len 254)
PUBLISH received (QoS 1, Msg ID 47), Payload (247 bytes):
Sent PUBACK (topic_id: 2, msg_id: 47, rc: 0)
PUBACK received for Msg ID: 169
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 170, Len 254)
PUBLISH received (QoS 1, Msg ID 48), Payload (247 bytes):
Sent PUBACK (topic_id: 2, msg_id: 48, rc: 0)
PUBACK received for Msg ID: 170
[UT-BINARY] Iteration 8/10 (247 bytes)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 171, Len 254)
PUBLISH received (QoS 1, Msg ID 49), Payload (247 bytes):
Sent PUBACK (topic_id: 2, msg_id: 49, rc: 0)
PUBACK received for Msg ID: 171
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 172, Len 254)
PUBLISH received (QoS 1, Msg ID 50), Payload (247 bytes):
Sent PUBACK (topic_id: 2, msg_id: 50, rc: 0)
PUBACK received for Msg ID: 172
[UT-BINARY] Iteration 10/10 (247 bytes)
✓ PASS: UT-BINARY completed successfully

=== TEST UT-RETRY: Retransmission Logic ===
Target executions: 5
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 173, Len 18)
PUBLISH received (QoS 1, Msg ID 51), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 51, rc: 0)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 173
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 173, Len 18)
PUBLISH received (QoS 1, Msg ID 52), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 52, rc: 0)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 173
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 173, Len 18)
PUBLISH received (QoS 1, Msg ID 53), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 53, rc: 0)
PUBACK received for Msg ID: 173
[UT-RETRY] Iteration 1/5
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 174, Len 18)
PUBLISH received (QoS 1, Msg ID 54), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 54, rc: 0)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 174
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 174, Len 18)
PUBLISH received (QoS 1, Msg ID 55), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 55, rc: 0)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 174
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 174, Len 18)
PUBLISH received (QoS 1, Msg ID 56), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 56, rc: 0)
PUBACK received for Msg ID: 174
[UT-RETRY] Iteration 2/5
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 175, Len 18)
PUBLISH received (QoS 1, Msg ID 57), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 57, rc: 0)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 175
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 175, Len 18)
PUBLISH received (QoS 1, Msg ID 58), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 58, rc: 0)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 175
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 175, Len 18)
PUBLISH received (QoS 1, Msg ID 59), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 59, rc: 0)
PUBACK received for Msg ID: 175
[UT-RETRY] Iteration 3/5
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 176, Len 18)
PUBLISH received (QoS 1, Msg ID 60), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 60, rc: 0)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 176
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 176, Len 18)
PUBLISH received (QoS 1, Msg ID 61), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 61, rc: 0)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 176
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 176, Len 18)
PUBLISH received (QoS 1, Msg ID 62), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 62, rc: 0)
PUBACK received for Msg ID: 176
[UT-RETRY] Iteration 4/5
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 177, Len 18)
PUBLISH received (QoS 1, Msg ID 63), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 63, rc: 0)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 177
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 177, Len 18)
PUBLISH received (QoS 1, Msg ID 64), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 64, rc: 0)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 177
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 177, Len 18)
PUBLISH received (QoS 1, Msg ID 65), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 65, rc: 0)
PUBACK received for Msg ID: 177
[UT-RETRY] Iteration 5/5
✓ PASS: UT-RETRY completed successfully

=== TEST UT-MAXRT: Maximum Retry Exhaustion ===
Target executions: 1
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 178, Len 22)
PUBLISH received (QoS 1, Msg ID 66), Payload (15 bytes):
Sent PUBACK (topic_id: 2, msg_id: 66, rc: 0)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 178
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 178, Len 22)
PUBLISH received (QoS 1, Msg ID 67), Payload (15 bytes):
Sent PUBACK (topic_id: 2, msg_id: 67, rc: 0)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 178
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 178, Len 22)
PUBLISH received (QoS 1, Msg ID 68), Payload (15 bytes):
Sent PUBACK (topic_id: 2, msg_id: 68, rc: 0)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 178
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 178, Len 22)
PUBLISH received (QoS 1, Msg ID 69), Payload (15 bytes):
Sent PUBACK (topic_id: 2, msg_id: 69, rc: 0)
Simulated drop of ACK type 0x0D
QoS 1 Msg ID 178 failed after 3 retries
✓ PASS: UT-MAXRT completed successfully

=== TEST UT-BURST: Rapid Message Burst ===
Target executions: 10
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 179, Len 18)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 180, Len 18)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 181, Len 18)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 182, Len 18)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 183, Len 18)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 184, Len 18)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 185, Len 18)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 186, Len 18)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 187, Len 18)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 188, Len 18)
PUBLISH received (QoS 1, Msg ID 70), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 70, rc: 0)
PUBACK received for Msg ID: 179
PUBLISH received (QoS 1, Msg ID 71), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 71, rc: 0)
PUBACK received for Msg ID: 180
PUBLISH received (QoS 1, Msg ID 72), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 72, rc: 0)
PUBACK received for Msg ID: 181
PUBLISH received (QoS 1, Msg ID 73), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 73, rc: 0)
PUBACK received for Msg ID: 182
PUBLISH received (QoS 1, Msg ID 74), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 74, rc: 0)
PUBACK received for Msg ID: 183
PUBLISH received (QoS 1, Msg ID 75), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 75, rc: 0)
PUBACK received for Msg ID: 184
PUBLISH received (QoS 1, Msg ID 76), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 76, rc: 0)
PUBACK received for Msg ID: 185
PUBLISH received (QoS 1, Msg ID 77), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 77, rc: 0)
PUBACK received for Msg ID: 186
PUBLISH received (QoS 1, Msg ID 78), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 78, rc: 0)
PUBACK received for Msg ID: 187
PUBLISH received (QoS 1, Msg ID 79), Payload (11 bytes):
Sent PUBACK (topic_id: 2, msg_id: 79, rc: 0)
PUBACK received for Msg ID: 188
[UT-BURST] Sent and acknowledged 10 messages
✓ PASS: UT-BURST completed successfully

=== TEST UT-QUEUE: Queue Overflow Handling ===
Target executions: 1
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 189, Len 21)
PUBLISH received (QoS 1, Msg ID 80), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 80, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 190, Len 21)
PUBLISH received (QoS 1, Msg ID 81), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 81, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 191, Len 21)
PUBLISH received (QoS 1, Msg ID 82), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 82, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 192, Len 21)
PUBLISH received (QoS 1, Msg ID 83), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 83, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 193, Len 21)
PUBLISH received (QoS 1, Msg ID 84), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 84, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 194, Len 21)
PUBLISH received (QoS 1, Msg ID 85), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 85, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 195, Len 21)
PUBLISH received (QoS 1, Msg ID 86), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 86, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 196, Len 21)
PUBLISH received (QoS 1, Msg ID 87), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 87, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 197, Len 21)
PUBLISH received (QoS 1, Msg ID 88), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 88, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 198, Len 21)
PUBLISH received (QoS 1, Msg ID 89), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 89, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 199, Len 21)
PUBLISH received (QoS 1, Msg ID 90), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 90, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 200, Len 21)
PUBLISH received (QoS 1, Msg ID 91), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 91, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 201, Len 21)
PUBLISH received (QoS 1, Msg ID 92), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 92, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 202, Len 21)
PUBLISH received (QoS 1, Msg ID 93), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 93, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 203, Len 21)
PUBLISH received (QoS 1, Msg ID 94), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 94, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 204, Len 21)
PUBLISH received (QoS 1, Msg ID 95), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 95, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 205, Len 21)
PUBLISH received (QoS 1, Msg ID 96), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 96, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 206, Len 21)
PUBLISH received (QoS 1, Msg ID 97), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 97, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 207, Len 21)
PUBLISH received (QoS 1, Msg ID 98), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 98, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 208, Len 21)
PUBLISH received (QoS 1, Msg ID 99), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 99, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 209, Len 21)
PUBLISH received (QoS 1, Msg ID 100), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 100, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 210, Len 21)
PUBLISH received (QoS 1, Msg ID 101), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 101, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 211, Len 21)
PUBLISH received (QoS 1, Msg ID 102), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 102, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 212, Len 21)
PUBLISH received (QoS 1, Msg ID 103), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 103, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 213, Len 21)
PUBLISH received (QoS 1, Msg ID 104), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 104, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 214, Len 21)
PUBLISH received (QoS 1, Msg ID 105), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 105, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 215, Len 21)
PUBLISH received (QoS 1, Msg ID 106), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 106, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 216, Len 21)
PUBLISH received (QoS 1, Msg ID 107), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 107, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 217, Len 21)
PUBLISH received (QoS 1, Msg ID 108), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 108, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 218, Len 21)
PUBLISH received (QoS 1, Msg ID 109), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 109, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 219, Len 21)
PUBLISH received (QoS 1, Msg ID 110), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 110, rc: 0)
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 220, Len 21)
Simulated drop of ACK type 0x0D
PUBLISH received (QoS 1, Msg ID 111), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 111, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 221, Len 21)
ERROR: All QoS slots full (32), message 221 will not be tracked for retransmission
PUBLISH received (QoS 1, Msg ID 112), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 112, rc: 0)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 2 (QoS 1, Msg ID 222, Len 21)
ERROR: All QoS slots full (32), message 222 will not be tracked for retransmission
PUBLISH received (QoS 1, Msg ID 113), Payload (14 bytes):
Sent PUBACK (topic_id: 2, msg_id: 113, rc: 0)
Simulated drop of ACK type 0x0D
[UT-QUEUE] Queue limit verified: 32 messages
✓ PASS: UT-QUEUE completed successfully

=== TEST UT-WRAP: Message ID Wraparound ===
Target executions: 100
[UT-WRAP] Iteration 20/100 (ID: 0x0106)
[UT-WRAP] Iteration 40/100 (ID: 0x012E)
[UT-WRAP] Iteration 60/100 (ID: 0x0156)
[UT-WRAP] Iteration 80/100 (ID: 0x017E)
[UT-WRAP] Iteration 100/100 (ID: 0x01A6)
✓ PASS: UT-WRAP completed successfully

================================================================================
                 AUTOMATED MQTT-SN TEST SUMMARY
================================================================================

Total test executions: 241
Passed:  14 (100.0%)
Failed:  0 (0.0%)
Errors:  0

Test Breakdown:
  UT-CONN (Connection):         1 execution
  UT-MSGID (Message ID):        50 executions
  UT-REG (Registration):        1 execution
  UT-SUB (Subscribe):           1 executions
  UT-QOS0 (QoS 0 publish):      20 executions
  UT-QOS1 (QoS 1 publish):      25 executions
  UT-QOS2 (QoS 2 publish):      15 executions
  UT-PING (Keepalive):          10 executions
  UT-BINARY (Large payload):    10 executions
  UT-RETRY (Retransmission):    5 executions
  UT-MAXRT (Max retries):       1 execution
  UT-BURST (Message burst):     1 execution (10 msgs)
  UT-QUEUE (Queue overflow):    1 execution
  UT-WRAP (ID wraparound):      100 executions
  ─────────────────────────────────────────
  Total:                        241 executions

Assertions checked: 579

✓ ALL TESTS PASSED!

================================================================================
