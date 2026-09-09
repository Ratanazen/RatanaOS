# 🌐 RatanaOS Network Stack & Driver Architecture

RatanaOS includes a native networking subsystem and hardware driver for Intel Gigabit Ethernet (e1000) PCI network adapters.

---

## Network Architecture Layering

```text
                 Userspace Sockets / Network Utilities
                                   │
                           POSIX Network API
                                   │
                 ┌─────────────────┴─────────────────┐
                 │                                   │
              IPv4 Layer                         ARP Engine
                 │                                   │
        ┌────────┼────────┐                ┌─────────┴─────────┐
        │        │        │                │                   │
      ICMP      UDP      TCP          ARP Requests        ARP Replies
        │        │        │                │                   │
        └────────┼────────┘                │                   │
                 │                         │                   │
                 └────────────┬────────────┘                   │
                              │                                │
                     Ethernet Framing (802.3) ─────────────────┘
                              │
                    e1000 PCI Network Driver
                              │
                Intel 82540EM / 82545EM Hardware
```

---

## Hardware Driver: Intel e1000 (`src/drivers/e1000.c`)

- **PCI Device Discovery**: Scans PCI buses for Intel Vendor `0x8086` and Device IDs `0x100E` (82540EM), `0x100F` (82545EM), `0x1004` (82543GC).
- **MMIO Mapping**: Maps BAR0 memory-mapped I/O registers for low-latency descriptor ring access.
- **EEPROM Access**: Reads hardware MAC address via the EERD register with fallback to RAL/RAH registers.
- **Ring Buffers**:
  - Receive Ring (RX): 32 descriptors of 2048-byte buffers configured with `RCTL_EN | RCTL_BAM | RCTL_BSIZE_2048`.
  - Transmit Ring (TX): 32 descriptors configured with `TCTL_EN | TCTL_PSP`.

---

## Protocol Implementations (`src/kernel/net/net.c`)

### 1. Ethernet (802.3)
- 14-byte standard Ethernet header (`dst_mac`, `src_mac`, `ethertype`).
- Minimum packet padding to 60 bytes.
- Automatic routing based on subnet mask and gateway IP.

### 2. Address Resolution Protocol (ARP)
- Formats and sends standard ARP Requests (`opcode = 1`) and ARP Replies (`opcode = 2`).
- Maintains an in-memory 32-entry dynamic ARP cache (`IP <-> MAC`).

### 3. IPv4
- 20-byte standard IPv4 header with Internet Checksum computation.
- Fragment flags and TTL management.
- Dispatches payloads by protocol number (1 = ICMP, 6 = TCP, 17 = UDP).

### 4. ICMP (Ping)
- Responds automatically to incoming `ECHO_REQUEST` packets with valid `ECHO_REPLY` checksums.
- Supports kernel and userspace generation of ping requests.
