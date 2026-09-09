#include "../../include/net.h"
#include "../../include/string.h"
#include "../../include/stdio.h"
#include "../../include/serial.h"
#include "../../include/heap.h"

#define MAX_ARP_ENTRIES 32

typedef struct {
    uint32_t ip;
    uint8_t  mac[6];
    bool     valid;
} arp_entry_t;

static uint32_t local_ip   = 0x0A00020F; // 10.0.2.15
static uint32_t subnet_mask = 0xFFFFFF00; // 255.255.255.0
static uint32_t gateway_ip = 0x0A000202; // 10.0.2.2
static uint8_t  broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static arp_entry_t arp_cache[MAX_ARP_ENTRIES];
static uint16_t ip_packet_id = 0x1000;

static uint16_t net_calculate_checksum(const void* data, int len) {
    const uint16_t* ptr = (const uint16_t*)data;
    uint32_t sum = 0;
    while (len > 1) {
        sum += *ptr++;
        len -= 2;
    }
    if (len == 1) {
        sum += *(const uint8_t*)ptr;
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return (uint16_t)(~sum);
}

void net_init(void) {
    memset(arp_cache, 0, sizeof(arp_cache));
    e1000_init();

    if (e1000_is_present()) {
        serial_printf("NET: Network stack initialized (IP: 10.0.2.15, Gateway: 10.0.2.2)\n");
    } else {
        serial_printf("NET: Network stack initialized in loopback / offline mode\n");
    }
}

void net_set_ip(uint32_t ip, uint32_t subnet, uint32_t gateway) {
    local_ip = ip;
    subnet_mask = subnet;
    gateway_ip = gateway;
}

uint32_t net_get_ip(void) {
    return local_ip;
}

const uint8_t* net_get_mac(void) {
    return e1000_get_mac();
}

static void arp_cache_insert(uint32_t ip, const uint8_t* mac) {
    for (int i = 0; i < MAX_ARP_ENTRIES; i++) {
        if (arp_cache[i].valid && arp_cache[i].ip == ip) {
            memcpy(arp_cache[i].mac, mac, 6);
            return;
        }
    }
    for (int i = 0; i < MAX_ARP_ENTRIES; i++) {
        if (!arp_cache[i].valid) {
            arp_cache[i].ip = ip;
            memcpy(arp_cache[i].mac, mac, 6);
            arp_cache[i].valid = true;
            return;
        }
    }
}

static const uint8_t* arp_cache_lookup(uint32_t ip) {
    for (int i = 0; i < MAX_ARP_ENTRIES; i++) {
        if (arp_cache[i].valid && arp_cache[i].ip == ip) {
            return arp_cache[i].mac;
        }
    }
    return NULL;
}

void net_send_ethernet(const uint8_t* dst_mac, uint16_t ethertype, const uint8_t* payload, uint32_t len) {
    uint8_t packet[1514];
    if (len + sizeof(ethernet_header_t) > sizeof(packet)) return;

    ethernet_header_t* eth = (ethernet_header_t*)packet;
    memcpy(eth->dst_mac, dst_mac, 6);
    memcpy(eth->src_mac, e1000_get_mac(), 6);
    eth->ethertype = htons(ethertype);

    memcpy(packet + sizeof(ethernet_header_t), payload, len);
    uint32_t total = len + sizeof(ethernet_header_t);
    if (total < 60) {
        memset(packet + total, 0, 60 - total);
        total = 60;
    }

    e1000_send_packet(packet, total);
}

void net_send_arp(uint16_t opcode, const uint8_t* dst_mac, uint32_t dst_ip) {
    arp_packet_t arp;
    arp.hw_type = htons(1);          // Ethernet
    arp.proto_type = htons(0x0800);   // IPv4
    arp.hw_addr_len = 6;
    arp.proto_addr_len = 4;
    arp.opcode = htons(opcode);

    memcpy(arp.src_mac, e1000_get_mac(), 6);
    arp.src_ip = htonl(local_ip);

    if (opcode == 1) { // ARP Request
        memset(arp.dst_mac, 0, 6);
    } else {
        memcpy(arp.dst_mac, dst_mac, 6);
    }
    arp.dst_ip = htonl(dst_ip);

    net_send_ethernet(dst_mac, ETHERTYPE_ARP, (const uint8_t*)&arp, sizeof(arp));
}

static void net_handle_arp(const arp_packet_t* arp, uint32_t len) {
    if (len < sizeof(arp_packet_t)) return;

    uint16_t opcode = ntohs(arp->opcode);
    uint32_t sender_ip = ntohl(arp->src_ip);
    uint32_t target_ip = ntohl(arp->dst_ip);

    arp_cache_insert(sender_ip, arp->src_mac);

    if (opcode == 1 && target_ip == local_ip) {
        // ARP Request for us -> Send ARP Reply
        net_send_arp(2, arp->src_mac, sender_ip);
        serial_printf("NET: ARP Reply sent to %08x (%02x:%02x:%02x:%02x:%02x:%02x)\n",
                      sender_ip, arp->src_mac[0], arp->src_mac[1], arp->src_mac[2],
                      arp->src_mac[3], arp->src_mac[4], arp->src_mac[5]);
    }
}

void net_send_ipv4(uint32_t dst_ip, uint8_t protocol, const uint8_t* payload, uint32_t len) {
    uint8_t buffer[1500];
    if (len + sizeof(ipv4_header_t) > sizeof(buffer)) return;

    ipv4_header_t* ip = (ipv4_header_t*)buffer;
    ip->ihl_version = (4 << 4) | (sizeof(ipv4_header_t) / 4); // IPv4, 20 bytes
    ip->tos = 0;
    ip->total_length = htons(sizeof(ipv4_header_t) + len);
    ip->identification = htons(ip_packet_id++);
    ip->flags_fragment = htons(0x4000); // Don't Fragment
    ip->ttl = 64;
    ip->protocol = protocol;
    ip->checksum = 0;
    ip->src_ip = htonl(local_ip);
    ip->dst_ip = htonl(dst_ip);
    ip->checksum = net_calculate_checksum(ip, sizeof(ipv4_header_t));

    memcpy(buffer + sizeof(ipv4_header_t), payload, len);

    // Determine destination MAC address
    uint32_t target_ip = dst_ip;
    if ((dst_ip & subnet_mask) != (local_ip & subnet_mask)) {
        target_ip = gateway_ip; // Route through gateway
    }

    const uint8_t* dst_mac = arp_cache_lookup(target_ip);
    if (!dst_mac) {
        // Broadcast ARP request and send packet to broadcast MAC as fallback
        net_send_arp(1, broadcast_mac, target_ip);
        dst_mac = broadcast_mac;
    }

    net_send_ethernet(dst_mac, ETHERTYPE_IPV4, buffer, sizeof(ipv4_header_t) + len);
}

void net_send_icmp_echo_request(uint32_t dst_ip, uint16_t id, uint16_t seq) {
    uint8_t payload[64];
    icmp_header_t* icmp = (icmp_header_t*)payload;
    icmp->type = ICMP_TYPE_ECHO_REQUEST;
    icmp->code = 0;
    icmp->checksum = 0;
    icmp->id = htons(id);
    icmp->sequence = htons(seq);

    // Fill with pattern
    for (int i = 0; i < 32; i++) {
        payload[sizeof(icmp_header_t) + i] = (uint8_t)('a' + (i % 26));
    }

    uint32_t len = sizeof(icmp_header_t) + 32;
    icmp->checksum = net_calculate_checksum(icmp, len);

    net_send_ipv4(dst_ip, IP_PROTO_ICMP, payload, len);
    serial_printf("NET: ICMP Echo Request (Ping) sent to %u.%u.%u.%u (seq=%u)\n",
                  (dst_ip >> 24) & 0xFF, (dst_ip >> 16) & 0xFF, (dst_ip >> 8) & 0xFF, dst_ip & 0xFF, seq);
}

static void net_handle_icmp(uint32_t src_ip, const icmp_header_t* icmp, uint32_t len) {
    if (len < sizeof(icmp_header_t)) return;

    if (icmp->type == ICMP_TYPE_ECHO_REQUEST) {
        // Reply with Echo Reply
        uint8_t reply_buf[1500];
        memcpy(reply_buf, icmp, len);
        icmp_header_t* reply_icmp = (icmp_header_t*)reply_buf;
        reply_icmp->type = ICMP_TYPE_ECHO_REPLY;
        reply_icmp->checksum = 0;
        reply_icmp->checksum = net_calculate_checksum(reply_icmp, len);

        net_send_ipv4(src_ip, IP_PROTO_ICMP, reply_buf, len);
        serial_printf("NET: ICMP Echo Reply sent to %08x\n", src_ip);
    } else if (icmp->type == ICMP_TYPE_ECHO_REPLY) {
        serial_printf("NET: ICMP Echo Reply received from %08x (id=%u, seq=%u)\n",
                      src_ip, ntohs(icmp->id), ntohs(icmp->sequence));
    }
}

static void net_handle_ipv4(const ipv4_header_t* ip, uint32_t len) {
    if (len < sizeof(ipv4_header_t)) return;

    uint8_t ihl = (ip->ihl_version & 0x0F) * 4;
    uint16_t total_len = ntohs(ip->total_length);
    if (total_len > len) total_len = len;

    uint32_t src_ip = ntohl(ip->src_ip);
    uint32_t dst_ip = ntohl(ip->dst_ip);

    if (dst_ip != local_ip && dst_ip != 0xFFFFFFFF) {
        return; // Not for us
    }

    const uint8_t* payload = (const uint8_t*)ip + ihl;
    uint32_t payload_len = total_len - ihl;

    if (ip->protocol == IP_PROTO_ICMP) {
        net_handle_icmp(src_ip, (const icmp_header_t*)payload, payload_len);
    } else if (ip->protocol == IP_PROTO_UDP) {
        serial_printf("NET: UDP packet received from %08x (len %u)\n", src_ip, payload_len);
    } else if (ip->protocol == IP_PROTO_TCP) {
        serial_printf("NET: TCP packet received from %08x (len %u)\n", src_ip, payload_len);
    }
}

void net_handle_rx_packet(const uint8_t* packet, uint32_t len) {
    if (!packet || len < sizeof(ethernet_header_t)) return;

    const ethernet_header_t* eth = (const ethernet_header_t*)packet;
    uint16_t ethertype = ntohs(eth->ethertype);

    const uint8_t* payload = packet + sizeof(ethernet_header_t);
    uint32_t payload_len = len - sizeof(ethernet_header_t);

    if (ethertype == ETHERTYPE_ARP) {
        net_handle_arp((const arp_packet_t*)payload, payload_len);
    } else if (ethertype == ETHERTYPE_IPV4) {
        net_handle_ipv4((const ipv4_header_t*)payload, payload_len);
    }
}
