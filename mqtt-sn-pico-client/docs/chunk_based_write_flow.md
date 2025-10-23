# Chunk-Based Write Flow Diagram

## Out-of-Order Chunk Reception and Sequential Writing

This document illustrates how the chunk-based write system handles out-of-order MQTT data.

## Scenario

- **File**: 5 data chunks (247 bytes each) + 1 metadata chunk
- **Total chunks**: 6 (indexed 0-5)
- **Arrival order**: 3, 2, 1, 5, 4, 0 (random/out-of-order)
- **Result**: Sequential file on microSD with all chunks in correct positions

## Visual Flow

```
MQTT Messages Arriving (Out of Order)
────────────────────────────────────────────────

Time 1: Chunk 3 arrives ──┐
                          │
Time 2: Chunk 2 arrives ──┤
                          │
Time 3: Chunk 1 arrives ──┤─── All arrive in
                          │    random order
Time 4: Chunk 5 arrives ──┤
                          │
Time 5: Chunk 4 arrives ──┤
                          │
Time 6: Chunk 0 arrives ──┘
    (metadata)


Chunk Write Process
────────────────────────────────────────────────

Initial State (After Init)
┌──────────────────────────────────────────┐
│ MicroSD File (empty, clusters allocated) │
│                                          │
│  Chunk 1 space: [ empty ]               │
│  Chunk 2 space: [ empty ]               │
│  Chunk 3 space: [ empty ]               │
│  Chunk 4 space: [ empty ]               │
│  Chunk 5 space: [ empty ]               │
│                                          │
│ Metadata:                                │
│   Chunks received: 0/6                   │
│   Bitmap: 00000000                       │
└──────────────────────────────────────────┘


After Chunk 3 Arrives
┌──────────────────────────────────────────┐
│ MicroSD File                             │
│                                          │
│  Chunk 1 space: [ empty ]               │
│  Chunk 2 space: [ empty ]               │
│  Chunk 3 space: [████████] ◄── Written! │
│  Chunk 4 space: [ empty ]               │
│  Chunk 5 space: [ empty ]               │
│                                          │
│ Metadata:                                │
│   Chunks received: 1/6                   │
│   Bitmap: 00001000                       │
│                    ▲                     │
│                    └─ Chunk 3 bit set   │
└──────────────────────────────────────────┘


After Chunk 2 Arrives
┌──────────────────────────────────────────┐
│ MicroSD File                             │
│                                          │
│  Chunk 1 space: [ empty ]               │
│  Chunk 2 space: [████████] ◄── Written! │
│  Chunk 3 space: [████████]              │
│  Chunk 4 space: [ empty ]               │
│  Chunk 5 space: [ empty ]               │
│                                          │
│ Metadata:                                │
│   Chunks received: 2/6                   │
│   Bitmap: 00001100                       │
│                    ▲▲                    │
│                    ││                    │
│                    │└─ Chunk 3           │
│                    └── Chunk 2           │
└──────────────────────────────────────────┘


After Chunk 1 Arrives
┌──────────────────────────────────────────┐
│ MicroSD File                             │
│                                          │
│  Chunk 1 space: [████████] ◄── Written! │
│  Chunk 2 space: [████████]              │
│  Chunk 3 space: [████████]              │
│  Chunk 4 space: [ empty ]               │
│  Chunk 5 space: [ empty ]               │
│                                          │
│ Metadata:                                │
│   Chunks received: 3/6                   │
│   Bitmap: 00001110                       │
│                    ▲▲▲                   │
│                    │││                   │
│                    ││└─ Chunk 3          │
│                    │└── Chunk 2          │
│                    └─── Chunk 1          │
└──────────────────────────────────────────┘


After Chunk 5 Arrives
┌──────────────────────────────────────────┐
│ MicroSD File                             │
│                                          │
│  Chunk 1 space: [████████]              │
│  Chunk 2 space: [████████]              │
│  Chunk 3 space: [████████]              │
│  Chunk 4 space: [ empty ]               │
│  Chunk 5 space: [████████] ◄── Written! │
│                                          │
│ Metadata:                                │
│   Chunks received: 4/6                   │
│   Bitmap: 00101110                       │
│                    ▲ ▲▲▲                 │
│                    │ │││                 │
│                    │ ││└─ Chunk 3        │
│                    │ │└── Chunk 2        │
│                    │ └─── Chunk 1        │
│                    └───── Chunk 5        │
└──────────────────────────────────────────┘


After Chunk 4 Arrives
┌──────────────────────────────────────────┐
│ MicroSD File                             │
│                                          │
│  Chunk 1 space: [████████]              │
│  Chunk 2 space: [████████]              │
│  Chunk 3 space: [████████]              │
│  Chunk 4 space: [████████] ◄── Written! │
│  Chunk 5 space: [████████]              │
│                                          │
│ Metadata:                                │
│   Chunks received: 5/6                   │
│   Bitmap: 00111110                       │
│                    ▲▲▲▲▲                 │
│                    │││││                 │
│                    ││││└─ Chunk 3        │
│                    │││└── Chunk 2        │
│                    ││└─── Chunk 1        │
│                    │└──── Chunk 5        │
│                    └───── Chunk 4        │
└──────────────────────────────────────────┘


After Chunk 0 (Metadata) Arrives
┌──────────────────────────────────────────┐
│ MicroSD File (Data complete!)            │
│                                          │
│  Chunk 1 space: [████████]              │
│  Chunk 2 space: [████████]              │
│  Chunk 3 space: [████████]              │
│  Chunk 4 space: [████████]              │
│  Chunk 5 space: [████████]              │
│                                          │
│ Metadata:                                │
│   Chunks received: 6/6 ✓                │
│   Bitmap: 00111111                       │
│             ▲▲▲▲▲▲                       │
│             │││││└─ Chunk 3              │
│             ││││└── Chunk 2              │
│             │││└─── Chunk 1              │
│             ││└──── Chunk 5              │
│             │└───── Chunk 4              │
│             └────── Chunk 0 (metadata)   │
│                                          │
│ Status: Ready for finalization!          │
└──────────────────────────────────────────┘


After Finalization
┌──────────────────────────────────────────┐
│ MicroSD File System                      │
│                                          │
│ Directory Entry Created:                 │
│   Filename: mqtt_data.bin                │
│   Size: 1235 bytes                       │
│   Cluster: [allocated]                   │
│   Status: Complete ✓                     │
│                                          │
│ File Contents (Sequential):              │
│   [Chunk 1][Chunk 2][Chunk 3][Chunk 4]  │
│   [Chunk 5]                              │
│                                          │
│ File is now accessible via standard      │
│ microSD read operations!                 │
└──────────────────────────────────────────┘
```

## Bitmap Tracking Explanation

The bitmap uses individual bits to track which chunks have been received:

```
Bitmap: 00111111
        ││││││││
        │││││││└─ Bit 0: Chunk 0 (metadata)
        ││││││└── Bit 1: Chunk 1
        │││││└─── Bit 2: Chunk 2
        ││││└──── Bit 3: Chunk 3
        │││└───── Bit 4: Chunk 4
        ││└────── Bit 5: Chunk 5
        │└─────── Bit 6: (unused)
        └──────── Bit 7: (unused)

When all expected chunks arrive:
- All relevant bits are set to 1
- chunks_received == total_chunks
- Ready for finalization
```

## Memory Layout

```
Physical MicroSD Layout
───────────────────────────────────────────────

Cluster Chain:
┌─────────┐    ┌─────────┐    ┌─────────┐
│Cluster 1│───▶│Cluster 2│───▶│Cluster 3│
│         │    │         │    │         │
│ Chunk 1 │    │ Chunk 3 │    │ Chunk 5 │
│ Chunk 2 │    │ Chunk 4 │    │         │
└─────────┘    └─────────┘    └─────────┘

Each cluster contains multiple sectors (512 bytes each)
Chunks are written to their specific offsets within
the cluster chain, regardless of arrival order.


Chunk Position Calculation
───────────────────────────────────────────────

For chunk N (N >= 1):
  File offset = (N - 1) × chunk_size
  
Example for chunk 3 (247 bytes per chunk):
  File offset = (3 - 1) × 247 = 494 bytes
  
  494 ÷ 512 (sector size) = 0 remainder 494
  → Write to sector 0, offset 494
  
This ensures each chunk goes to the correct
position regardless of when it arrives.
```

## State Machine

```
State Machine for Chunk Reception
───────────────────────────────────────────────

        ┌──────────────┐
        │   INIT       │
        │ (Allocate    │
        │  clusters)   │
        └──────┬───────┘
               │
               ▼
        ┌──────────────┐
   ┌───▶│  RECEIVING   │◀───┐
   │    │  (Write      │    │
   │    │   chunks)    │    │
   │    └──────┬───────┘    │
   │           │             │
   │           │ Chunk       │
   │           │ arrives     │
   │           ▼             │
   │    ┌──────────────┐    │
   │    │ UPDATE       │    │
   │    │ BITMAP       │    │
   │    └──────┬───────┘    │
   │           │             │
   │           ▼             │
   │    ┌──────────────┐    │
   │    │  CHECK       │    │
   └────┤  COMPLETE?   │────┘
        └──────┬───────┘
               │ All chunks
               │ received
               ▼
        ┌──────────────┐
        │  FINALIZE    │
        │ (Create dir  │
        │  entry)      │
        └──────┬───────┘
               │
               ▼
        ┌──────────────┐
        │   COMPLETE   │
        └──────────────┘
```

## Error Handling Flow

```
Error Scenarios
───────────────────────────────────────────────

1. Duplicate Chunk
   Chunk N arrives
        ↓
   Check bitmap
        ↓
   Bit already set?
        ├─ Yes → Log warning, skip write
        └─ No  → Proceed with write

2. Invalid Chunk Index
   Chunk N arrives
        ↓
   N >= total_chunks?
        ├─ Yes → Return error
        └─ No  → Proceed with write

3. Incomplete Transfer
   Finalize called
        ↓
   All chunks received?
        ├─ No  → Log missing chunks, proceed anyway
        └─ Yes → Normal finalization

4. Write Failure
   Write chunk to SD
        ↓
   Write successful?
        ├─ No  → Return error, don't update bitmap
        └─ Yes → Update bitmap, increment counter
```

## Benefits of This Approach

1. **Robustness**: Handles network packet reordering
2. **Efficiency**: Each chunk written only once to correct position
3. **Trackability**: Always know which chunks are missing
4. **Flexibility**: Works with any chunk arrival order
5. **Recovery**: Can identify and request missing chunks
6. **Memory efficient**: No need to buffer entire file in RAM
