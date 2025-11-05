# Control Message Implementation - Fix Summary

## Overview

Fixed the control message sending mechanism for the Go-Back-N protocol by passing UDP parameters directly through function calls instead of using placeholder TODO comments.

## Changes Made

### 1. Updated `mqtt_sn_context_t` Structure

**File**: `mqtt/mqtt-sn-udp.h`

Added UDP connection parameters to the context:

```c
typedef struct {
    // ... existing fields ...
    
    /* UDP connection parameters (for sending control messages) */
    struct udp_pcb* pcb;      // UDP PCB pointer
    ip_addr_t gw_addr;        // Gateway address
    u16_t gw_port;            // Gateway port
} mqtt_sn_context_t;
```

**Benefit**: Allows context to store gateway information for sending responses, though direct parameter passing is preferred for clarity.

### 2. Updated `handle_file_payload()` Function Signature

**File**: `mqtt/mqtt-sn-udp.h` and `mqtt/mqtt-sn-udp.c`

**Before**:
```c
void handle_file_payload(mqtt_sn_context_t* ctx, const uint8_t* payload, size_t len);
```

**After**:
```c
void handle_file_payload(mqtt_sn_context_t* ctx, const uint8_t* payload, size_t len,
                         struct udp_pcb* pcb, const ip_addr_t* addr, u16_t port);
```

**Reason**: Enables the function to send control messages directly without needing to store parameters in context.

### 3. Replaced TODO Placeholders with Actual Control Message Sending

**File**: `mqtt/mqtt-sn-udp.c`

#### COMPLETE Message (Transfer Complete)

**Before**:
```c
// Send COMPLETE control message
control_message_t ctrl_msg = {0};
ctrl_msg.type = CTRL_COMPLETE;
ctrl_msg.seq_num = total;
strncpy(ctrl_msg.session_id, ctx->rx_session_id, sizeof(ctrl_msg.session_id) - 1);

printf("  [RECEIVER] TODO: Send COMPLETE to file/control\n");
```

**After**:
```c
// Send COMPLETE control message
control_message_t ctrl_msg = {0};
ctrl_msg.type = CTRL_COMPLETE;
ctrl_msg.seq_num = total;
strncpy(ctrl_msg.session_id, ctx->rx_session_id, sizeof(ctrl_msg.session_id) - 1);

send_control_message(pcb, addr, port, &ctrl_msg);
```

#### REQUEST_NEXT Message (Next Window Request)

**Before**:
```c
control_message_t ctrl_msg = {0};
ctrl_msg.type = CTRL_REQUEST_NEXT;
ctrl_msg.seq_num = ctx->last_acked_seq;
ctrl_msg.window_start = next_window_start;
ctrl_msg.window_end = next_window_end;
strncpy(ctrl_msg.session_id, ctx->rx_session_id, sizeof(ctrl_msg.session_id) - 1);

printf("  [RECEIVER] TODO: Send REQUEST_NEXT for window [%u-%u] to file/control\n",
       next_window_start, next_window_end);
```

**After**:
```c
control_message_t ctrl_msg = {0};
ctrl_msg.type = CTRL_REQUEST_NEXT;
ctrl_msg.seq_num = ctx->last_acked_seq;
ctrl_msg.window_start = next_window_start;
ctrl_msg.window_end = next_window_end;
strncpy(ctrl_msg.session_id, ctx->rx_session_id, sizeof(ctrl_msg.session_id) - 1);

send_control_message(pcb, addr, port, &ctrl_msg);
```

#### NACK Message (Missing Chunk Detection)

**Before**:
```c
control_message_t ctrl_msg = {0};
ctrl_msg.type = CTRL_NACK;
ctrl_msg.seq_num = first_missing - 1;
ctrl_msg.window_start = window_start;
ctrl_msg.window_end = current_window_end;
strncpy(ctrl_msg.session_id, ctx->rx_session_id, sizeof(ctrl_msg.session_id) - 1);

printf("  [RECEIVER] TODO: Send NACK (retransmit from chunk %u) to file/control\n",
       first_missing);
```

**After**:
```c
control_message_t ctrl_msg = {0};
ctrl_msg.type = CTRL_NACK;
ctrl_msg.seq_num = first_missing - 1;
ctrl_msg.window_start = window_start;
ctrl_msg.window_end = current_window_end;
strncpy(ctrl_msg.session_id, ctx->rx_session_id, sizeof(ctrl_msg.session_id) - 1);

send_control_message(pcb, addr, port, &ctrl_msg);
```

### 4. Updated UDP Receive Callback

**File**: `mqtt/mqtt-sn-udp.c`

Updated the call to `handle_file_payload()` to pass UDP parameters:

**Before**:
```c
} else if (topic_id == FILE_TRANSFER_TOPIC_DATA) {
    // file/data topic (expects QoS 1)
    handle_file_payload(ctx, payload, payload_len);
    // Send PUBACK for QoS 1
    if (qos == QOS_LEVEL_1) {
        mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id, MQTTSN_RETURN_ACCEPTED);
    }
}
```

**After**:
```c
} else if (topic_id == FILE_TRANSFER_TOPIC_DATA) {
    // file/data topic (expects QoS 1)
    handle_file_payload(ctx, payload, payload_len, pcb, addr, port);
    // Send PUBACK for QoS 1
    if (qos == QOS_LEVEL_1) {
        mqtt_sn_send_puback(pcb, addr, port, topic_id, msg_id, MQTTSN_RETURN_ACCEPTED);
    }
}
```

### 5. Updated Documentation

**File**: `docs/GO_BACK_N_MQTT_IMPLEMENTATION.md`

- Updated function signatures in API documentation
- Added note about automatic control message sending in receiver
- Removed outdated warning about TODO placeholders
- Updated context structure documentation with UDP parameters

## Message Flow (Now Complete)

### Successful Window Transfer

```
Receiver receives chunk → writes to buffer → checks window status
   ↓
Window complete detected
   ↓
chunk_transfer_sync_window() writes to SD card
   ↓
send_control_message(REQUEST_NEXT) → publishes to file/control
   ↓
Sender's handle_control_message() receives REQUEST_NEXT
   ↓
Sender transmits next window
```

### Missing Chunk Detection

```
Receiver receives chunk → writes to buffer → checks window status
   ↓
Missing chunks detected (bitmap check)
   ↓
send_control_message(NACK) → publishes to file/control
   ↓
Sender's handle_control_message() receives NACK
   ↓
Sender retransmits from failed chunk (Go-Back-N)
```

### Transfer Complete

```
Receiver receives final chunk → writes to buffer → all chunks received
   ↓
chunk_transfer_finalize() renames temp file to final file
   ↓
send_control_message(COMPLETE) → publishes to file/control
   ↓
Sender's handle_control_message() receives COMPLETE
   ↓
Sender marks transfer complete, cleans up window
```

## Benefits of This Implementation

1. **Clean Architecture**: UDP parameters flow naturally from callback → handler → sender
2. **No Global State**: No need to store pcb/addr/port in context (though available if needed)
3. **Clear Data Flow**: Easy to trace how control messages are sent
4. **Testable**: Can pass mock UDP parameters for testing
5. **Complete Protocol**: All control messages now properly sent

## Testing Checklist

- [x] Code compiles without errors (IntelliSense warnings about SDK headers are expected)
- [ ] Sender receives COMPLETE message after successful transfer
- [ ] Sender receives REQUEST_NEXT message after each window
- [ ] Sender receives NACK on simulated packet loss
- [ ] Sender retransmits correctly after NACK
- [ ] Receiver writes windows to SD card correctly
- [ ] File integrity verified after complete transfer

## Files Modified

1. `mqtt/mqtt-sn-udp.h` - Updated context structure and function signatures
2. `mqtt/mqtt-sn-udp.c` - Implemented control message sending
3. `docs/GO_BACK_N_MQTT_IMPLEMENTATION.md` - Updated documentation

## Backward Compatibility

**Breaking Change**: The function signature for `handle_file_payload()` has changed. Any code calling this function directly must be updated to pass the additional UDP parameters.

**Mitigation**: This is an internal function called only from the UDP receive callback, so impact is minimal. The public API (`send_file_via_mqtt_gbn()`) remains unchanged.
