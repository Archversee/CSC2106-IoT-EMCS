
================================================================================
              AUTOMATED MQTT-SN PROTOCOL UNIT TESTS
================================================================================
Test Suite: MQTT-SN Client Implementation
Based on: MQTT-SN Protocol Specification v1.2
Date: Nov 24 2025
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
Target executions: 5
✓ Added topic 'test/topic_0' for registration (sender)
Sent REGISTER for topic 'test/topic_0' (msg_id=101)
✓ REGACK: Topic registered successfully with ID 1 (msg_id=101)
✓ Topic 'test/topic_0' registered with ID 1 (ready to publish)
[UT-REG] Iteration 1/5 (Topic: test/topic_0, ID: 1)
✓ Added topic 'test/topic_1' for registration (sender)
Sent REGISTER for topic 'test/topic_1' (msg_id=102)
✓ REGACK: Topic registered successfully with ID 2 (msg_id=102)
✓ Topic 'test/topic_1' registered with ID 2 (ready to publish)
[UT-REG] Iteration 2/5 (Topic: test/topic_1, ID: 2)
✓ Added topic 'test/topic_2' for registration (sender)
Sent REGISTER for topic 'test/topic_2' (msg_id=103)
✓ REGACK: Topic registered successfully with ID 3 (msg_id=103)
✓ Topic 'test/topic_2' registered with ID 3 (ready to publish)
[UT-REG] Iteration 3/5 (Topic: test/topic_2, ID: 3)
✓ Added topic 'test/topic_3' for registration (sender)
Sent REGISTER for topic 'test/topic_3' (msg_id=104)
✓ REGACK: Topic registered successfully with ID 4 (msg_id=104)
✓ Topic 'test/topic_3' registered with ID 4 (ready to publish)
[UT-REG] Iteration 4/5 (Topic: test/topic_3, ID: 4)
✓ Added topic 'test/topic_4' for registration (sender)
Sent REGISTER for topic 'test/topic_4' (msg_id=105)
✓ REGACK: Topic registered successfully with ID 5 (msg_id=105)
✓ Topic 'test/topic_4' registered with ID 5 (ready to publish)
[UT-REG] Iteration 5/5 (Topic: test/topic_4, ID: 5)
✓ PASS: UT-REG completed successfully

=== TEST UT-SUB: Subscribe Operations ===
Target executions: 5
✓ Added topic 'test/subscribe_0' for subscription (receiver, QoS 1)
Sent SUBSCRIBE for topic 'test/subscribe_0' (QoS 1, msg_id=106)
SUBACK: topic_id=6, msg_id=106, return_code=0
✓ Topic 'test/subscribe_0' subscribed successfully (ID 6)
[UT-SUB] Iteration 1/5 (Topic: test/subscribe_0, ID: 6)
✓ Added topic 'test/subscribe_1' for subscription (receiver, QoS 1)
Sent SUBSCRIBE for topic 'test/subscribe_1' (QoS 1, msg_id=107)
SUBACK: topic_id=7, msg_id=107, return_code=0
✓ Topic 'test/subscribe_1' subscribed successfully (ID 7)
[UT-SUB] Iteration 2/5 (Topic: test/subscribe_1, ID: 7)
✓ Added topic 'test/subscribe_2' for subscription (receiver, QoS 1)
Sent SUBSCRIBE for topic 'test/subscribe_2' (QoS 1, msg_id=108)
SUBACK: topic_id=8, msg_id=108, return_code=0
✓ Topic 'test/subscribe_2' subscribed successfully (ID 8)
[UT-SUB] Iteration 3/5 (Topic: test/subscribe_2, ID: 8)
✓ Added topic 'test/subscribe_3' for subscription (receiver, QoS 1)
Sent SUBSCRIBE for topic 'test/subscribe_3' (QoS 1, msg_id=109)
SUBACK: topic_id=9, msg_id=109, return_code=0
✓ Topic 'test/subscribe_3' subscribed successfully (ID 9)
[UT-SUB] Iteration 4/5 (Topic: test/subscribe_3, ID: 9)
✓ Added topic 'test/subscribe_4' for subscription (receiver, QoS 1)
Sent SUBSCRIBE for topic 'test/subscribe_4' (QoS 1, msg_id=110)
SUBACK: topic_id=10, msg_id=110, return_code=0
✓ Topic 'test/subscribe_4' subscribed successfully (ID 10)
[UT-SUB] Iteration 5/5 (Topic: test/subscribe_4, ID: 10)
✓ PASS: UT-SUB completed successfully

=== TEST UT-QOS0: QoS 0 Publishing ===
Target executions: 20
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 111, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 112, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 113, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 114, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 115, Len 17)
[UT-QOS0] Iteration 5/20
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 116, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 117, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 118, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 119, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 120, Len 17)
[UT-QOS0] Iteration 10/20
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 121, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 122, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 123, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 124, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 125, Len 17)
[UT-QOS0] Iteration 15/20
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 126, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 127, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 128, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 129, Len 17)
Sent PUBLISH to Topic ID 1 (QoS 0, Msg ID 130, Len 17)
[UT-QOS0] Iteration 20/20
✓ PASS: UT-QOS0 completed successfully

=== TEST UT-QOS1: QoS 1 Publishing ===
Target executions: 25
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 131, Len 17)
PUBACK received for Msg ID: 131
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 132, Len 17)
PUBACK received for Msg ID: 132
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 133, Len 17)
PUBACK received for Msg ID: 133
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 134, Len 17)
PUBACK received for Msg ID: 134
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 135, Len 17)
PUBACK received for Msg ID: 135
[UT-QOS1] Iteration 5/25
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 136, Len 17)
PUBACK received for Msg ID: 136
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 137, Len 17)
PUBACK received for Msg ID: 137
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 138, Len 17)
PUBACK received for Msg ID: 138
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 139, Len 17)
PUBACK received for Msg ID: 139
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 140, Len 17)
PUBACK received for Msg ID: 140
[UT-QOS1] Iteration 10/25
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 141, Len 17)
PUBACK received for Msg ID: 141
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 142, Len 17)
PUBACK received for Msg ID: 142
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 143, Len 17)
PUBACK received for Msg ID: 143
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 144, Len 17)
PUBACK received for Msg ID: 144
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 145, Len 17)
PUBACK received for Msg ID: 145
[UT-QOS1] Iteration 15/25
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 146, Len 17)
PUBACK received for Msg ID: 146
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 147, Len 17)
PUBACK received for Msg ID: 147
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 148, Len 17)
PUBACK received for Msg ID: 148
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 149, Len 17)
PUBACK received for Msg ID: 149
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 150, Len 17)
PUBACK received for Msg ID: 150
[UT-QOS1] Iteration 20/25
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 151, Len 17)
PUBACK received for Msg ID: 151
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 152, Len 17)
PUBACK received for Msg ID: 152
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 153, Len 17)
PUBACK received for Msg ID: 153
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 154, Len 17)
PUBACK received for Msg ID: 154
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 155, Len 17)
PUBACK received for Msg ID: 155
[UT-QOS1] Iteration 25/25
✓ PASS: UT-QOS1 completed successfully

=== TEST UT-QOS2: QoS 2 Publishing ===
Target executions: 15
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 156, Len 17)
PUBREC received for Msg ID: 156. Sending PUBREL...
PUBCOMP received for Msg ID: 156
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 157, Len 17)
PUBREC received for Msg ID: 157. Sending PUBREL...
PUBCOMP received for Msg ID: 157
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 158, Len 17)
PUBREC received for Msg ID: 158. Sending PUBREL...
PUBCOMP received for Msg ID: 158
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 159, Len 17)
PUBREC received for Msg ID: 159. Sending PUBREL...
PUBCOMP received for Msg ID: 159
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 160, Len 17)
PUBREC received for Msg ID: 160. Sending PUBREL...
PUBCOMP received for Msg ID: 160
[UT-QOS2] Iteration 5/15
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 161, Len 17)
PUBREC received for Msg ID: 161. Sending PUBREL...
PUBCOMP received for Msg ID: 161
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 162, Len 17)
PUBREC received for Msg ID: 162. Sending PUBREL...
PUBCOMP received for Msg ID: 162
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 163, Len 17)
PUBREC received for Msg ID: 163. Sending PUBREL...
PUBCOMP received for Msg ID: 163
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 164, Len 17)
PUBREC received for Msg ID: 164. Sending PUBREL...
PUBCOMP received for Msg ID: 164
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 165, Len 17)
PUBREC received for Msg ID: 165. Sending PUBREL...
PUBCOMP received for Msg ID: 165
[UT-QOS2] Iteration 10/15
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 166, Len 17)
PUBREC received for Msg ID: 166. Sending PUBREL...
PUBCOMP received for Msg ID: 166
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 167, Len 17)
PUBREC received for Msg ID: 167. Sending PUBREL...
PUBCOMP received for Msg ID: 167
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 168, Len 17)
PUBREC received for Msg ID: 168. Sending PUBREL...
PUBCOMP received for Msg ID: 168
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 169, Len 17)
PUBREC received for Msg ID: 169. Sending PUBREL...
PUBCOMP received for Msg ID: 169
Sent PUBLISH to Topic ID 1 (QoS 2, Msg ID 170, Len 17)
PUBREC received for Msg ID: 170. Sending PUBREL...
PUBCOMP received for Msg ID: 170
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
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 171, Len 254)
PUBACK received for Msg ID: 171
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 172, Len 254)
PUBACK received for Msg ID: 172
[UT-BINARY] Iteration 2/10 (247 bytes)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 173, Len 254)
PUBACK received for Msg ID: 173
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 174, Len 254)
PUBACK received for Msg ID: 174
[UT-BINARY] Iteration 4/10 (247 bytes)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 175, Len 254)
PUBACK received for Msg ID: 175
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 176, Len 254)
PUBACK received for Msg ID: 176
[UT-BINARY] Iteration 6/10 (247 bytes)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 177, Len 254)
PUBACK received for Msg ID: 177
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 178, Len 254)
PUBACK received for Msg ID: 178
[UT-BINARY] Iteration 8/10 (247 bytes)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 179, Len 254)
PUBACK received for Msg ID: 179
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 180, Len 254)
PUBACK received for Msg ID: 180
[UT-BINARY] Iteration 10/10 (247 bytes)
✓ PASS: UT-BINARY completed successfully

=== TEST UT-RETRY: Retransmission Logic ===
Target executions: 5
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 181, Len 18)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 181
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 181, Len 18)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 181
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 181, Len 18)
PUBACK received for Msg ID: 181
[UT-RETRY] Iteration 1/5
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 182, Len 18)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 182
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 182, Len 18)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 182
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 182, Len 18)
PUBACK received for Msg ID: 182
[UT-RETRY] Iteration 2/5
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 183, Len 18)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 183
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 183, Len 18)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 183
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 183, Len 18)
PUBACK received for Msg ID: 183
[UT-RETRY] Iteration 3/5
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 184, Len 18)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 184
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 184, Len 18)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 184
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 184, Len 18)
PUBACK received for Msg ID: 184
[UT-RETRY] Iteration 4/5
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 185, Len 18)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 185
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 185, Len 18)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 185
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 185, Len 18)
PUBACK received for Msg ID: 185
[UT-RETRY] Iteration 5/5
✓ PASS: UT-RETRY completed successfully

=== TEST UT-MAXRT: Maximum Retry Exhaustion ===
Target executions: 1
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 186, Len 22)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 186
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 186, Len 22)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 186
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 186, Len 22)
Simulated drop of ACK type 0x0D
Retransmitting QoS1 PUBLISH for Msg ID 186
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 186, Len 22)
Simulated drop of ACK type 0x0D
QoS 1 Msg ID 186 failed after 3 retries
✓ PASS: UT-MAXRT completed successfully

=== TEST UT-BURST: Rapid Message Burst ===
Target executions: 10
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 187, Len 18)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 188, Len 18)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 189, Len 18)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 190, Len 18)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 191, Len 18)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 192, Len 18)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 193, Len 18)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 194, Len 18)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 195, Len 18)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 196, Len 18)
PUBACK received for Msg ID: 187
PUBACK received for Msg ID: 188
PUBACK received for Msg ID: 189
PUBACK received for Msg ID: 190
PUBACK received for Msg ID: 191
PUBACK received for Msg ID: 192
PUBACK received for Msg ID: 193
PUBACK received for Msg ID: 194
PUBACK received for Msg ID: 195
PUBACK received for Msg ID: 196
[UT-BURST] Sent and acknowledged 10 messages
✓ PASS: UT-BURST completed successfully

=== TEST UT-QUEUE: Queue Overflow Handling ===
Target executions: 1
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 197, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 198, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 199, Len 21)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 200, Len 21)
Simulated drop of ACK type 0x0D
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 201, Len 21)
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 202, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 203, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 204, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 205, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 206, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 207, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 208, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 209, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 210, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 211, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 212, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 213, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 214, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 215, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 216, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 217, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 218, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 219, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 220, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 221, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 222, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 223, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 224, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 225, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 226, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 227, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 228, Len 21)
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 229, Len 21)
ERROR: All QoS slots full (32), message 229 will not be tracked for retransmission
Simulated drop of ACK type 0x0D
Sent PUBLISH to Topic ID 1 (QoS 1, Msg ID 230, Len 21)
ERROR: All QoS slots full (32), message 230 will not be tracked for retransmission
Simulated drop of ACK type 0x0D
[UT-QUEUE] Queue limit verified: 32 messages
✓ PASS: UT-QUEUE completed successfully

=== TEST UT-WRAP: Message ID Wraparound ===
Target executions: 100
[UT-WRAP] Iteration 20/100 (ID: 0x010E)
[UT-WRAP] Iteration 40/100 (ID: 0x0136)
[UT-WRAP] Iteration 60/100 (ID: 0x015E)
[UT-WRAP] Iteration 80/100 (ID: 0x0186)
[UT-WRAP] Iteration 100/100 (ID: 0x01AE)
✓ PASS: UT-WRAP completed successfully

================================================================================
                 AUTOMATED MQTT-SN TEST SUMMARY
================================================================================

Total test executions: 249
Passed:  14 (100.0%)
Failed:  0 (0.0%)
Errors:  0

Test Breakdown:
  UT-CONN (Connection):         1 execution
  UT-MSGID (Message ID):        50 executions
  UT-REG (Registration):        5 executions
  UT-SUB (Subscribe):           5 executions
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
  Total:                        249 executions

Assertions checked: 599

✓ ALL TESTS PASSED!

================================================================================
