#include "socket_op.h"

#include <errno.h>
#include <iostream>

int Socket(int family, int type, int protocol)
{
	int	fd = socket(family, type, protocol);
	if(fd == -1)
    {
        std::cout << "socket(...) failed. Error: " << errno << "\n";
        exit (-1); 
    }
    
	return fd;
}

void Sendto(int fd, struct icmphdr &icmp_hdr, int flags, const struct sockaddr_in *saddr)
{
	int sent_bytes = sendto(fd, &icmp_hdr, sizeof(icmp_hdr), flags, (struct sockaddr*)saddr, sizeof(*saddr));
	
	if( sent_bytes == -1 || sent_bytes != sizeof(icmp_hdr) )
    {
        std::cout << "sendto(...) failed. Error: " << errno << "\n";
        exit (-2); 
    }
}

void Setsockopt(int sockfd, int level, int name, const void *val, socklen_t len)
{
	if ( setsockopt(sockfd, level, name, val, len) == -1 )
    {
        std::cout << "setsockopt(...) failed. Error: " << errno << "\n";
        exit (-3); 
    }
}

ssize_t Recvfrom(int fd, void *ptr, int flags, struct sockaddr_in &saddr)
{   
	socklen_t saddr_len = sizeof(saddr);
	ssize_t	rec_bytes = recvfrom(fd, ptr, IP_MAXPACKET, flags, (struct sockaddr*)&saddr, &saddr_len);
	if ( rec_bytes == -1)
		if ( errno != EAGAIN )
		{
            std::cout << "recvfrom(...) failed. Error: " << errno << "\n";
            exit (-4); 
        }
			
	return rec_bytes;
}
