#ifndef SOCKET_OP_H
#define SOCKET_OP_H

#include <arpa/inet.h>
#include <netinet/ip_icmp.h>

int Socket (int family, int type, int protocol);

void Sendto (int fd, struct icmphdr &icmp_hdr, int flags, const struct sockaddr_in *saddr);
             
void Setsockopt (int sockfd, int level, int name, const void *val, socklen_t len);
                 
ssize_t Recvfrom (int fd, void *ptr, int flags, struct sockaddr_in *saddr);

#endif // SOCKET_OP_H

