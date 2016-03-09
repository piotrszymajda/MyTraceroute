#ifndef PACKET_SUPP_H
#define PACKET_SUPP_H

#include <arpa/inet.h>
#include <netinet/ip_icmp.h>

int check_packet(u_int8_t * buff, ssize_t packet_len, u_int16_t pid, int ttl);
u_int16_t compute_icmp_checksum (const u_int16_t *buff, u_int32_t length);
void prepare_packet(icmphdr &icmp_header, u_int16_t id, u_int16_t sequence );

#endif //PACKET_SUPP_H
