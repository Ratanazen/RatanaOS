#ifndef RATANAOS_NET_H
#define RATANAOS_NET_H

#include "types.h"
#include <stdbool.h>

#define ETHERTYPE_ARP  0x0806
#define ETHERTYPE_IPV4 0x0800

#define IP_PROTO_ICMP  1
#define IP_PROTO_TCP   6
#define IP_PROTO_UDP   17

#define ICMP_TYPE_ECHO_REPLY   0
#define ICMP_TYPE_ECHO_REQUEST 8

// 16-bit / 32-bit Big Endian / Little Endian conversion
static inline uint16_t htons(uint16_t v) {
    return (uint16_t)((v >> 8) | (v << 8));
}

static inline uint16_t ntohs(uint16_t v) {
    return htons(v);
}

static inline uint32_t htonl(uint32_t v) {
    return ((v >> 24) & 0xFF) | ((v >> 8) & 0xFF00) | ((v << 8) & 0xFF0000) | ((v << 24) & 0xFF000000);
}

static inline uint32_t ntohl(uint32_t v) {
    return htonl(v);
}

// Ethernet Frame (14 bytes)
typedef struct {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
    uint8_t payload[];
} __attribute__((packed)) ethernet_header_t;

// ARP Packet (28 bytes)
typedef struct {
    uint16_t hw_type;       // 1 for Ethernet
    uint16_t proto_type;    // 0x0800 for IPv4
    uint8_t  hw_addr_len;   // 6
    uint8_t  proto_addr_len;// 4
    uint16_t opcode;        // 1 = Request, 2 = Reply
    uint8_t  src_mac[6];
    uint32_t src_ip;
    uint8_t  dst_mac[6];
    uint32_t dst_ip;
} __attribute__((packed)) arp_packet_t;

// IPv4 Header (20 bytes min)
typedef struct {
    uint8_t  ihl_version;   // Version (4 bits) + IHL (4 bits)
    uint8_t  tos;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
} __attribute__((packed)) ipv4_header_t;

// ICMP Header (8 bytes min)
typedef struct {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint16_t id;
    uint16_t sequence;
    uint8_t  payload[];
} __attribute__((packed)) icmp_header_t;

// UDP Header (8 bytes)
typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
    uint8_t  payload[];
} __attribute__((packed)) udp_header_t;

// TCP Header (20 bytes min)
typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint8_t  data_offset_reserved; // Data offset (4 bits) + Reserved (4 bits)
    uint8_t  flags;               // FIN, SYN, RST, PSH, ACK, URG
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_pointer;
} __attribute__((packed)) tcp_header_t;

// Network Interface API
void net_init(void);
void net_set_ip(uint32_t ip, uint32_t subnet, uint32_t gateway);
uint32_t net_get_ip(void);
const uint8_t* net_get_mac(void);
void net_handle_rx_packet(const uint8_t* packet, uint32_t len);

void net_send_ethernet(const uint8_t* dst_mac, uint16_t ethertype, const uint8_t* payload, uint32_t len);
void net_send_arp(uint16_t opcode, const uint8_t* dst_mac, uint32_t dst_ip);
void net_send_ipv4(uint32_t dst_ip, uint8_t protocol, const uint8_t* payload, uint32_t len);
void net_send_icmp_echo_request(uint32_t dst_ip, uint16_t id, uint16_t seq);
void net_send_udp(uint32_t dst_ip, uint16_t src_port, uint16_t dst_port, const uint8_t* data, uint32_t len);

// e1000 Driver Hooks
void e1000_init(void);
bool e1000_is_present(void);
const uint8_t* e1000_get_mac(void);
void e1000_send_packet(const uint8_t* data, uint16_t len);
void e1000_poll_rx(void);

#endif // RATANAOS_NET_H
