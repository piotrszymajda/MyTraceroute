#include "traceroute.h"
#include "socket_op.h"

#include <assert.h>
#include <sys/time.h>
#include <iostream>


u_int16_t compute_icmp_checksum (const u_int16_t *buff, u_int32_t length)
{
    u_int32_t sum = 0;

    while (length > 1)  
    {
        sum += *buff++;
        length -= 2;
    }

    if (length == 1) // if length is odd 
        sum += htons(*(u_char *)buff << 8);

    sum = (sum >> 16) + (sum & 0xffff); 
    return ~(sum + (sum >> 16));
}

void prepare_packet(icmphdr &icmp_header, u_int16_t id, u_int16_t sequence )
{
    icmp_header.type = ICMP_ECHO;
    icmp_header.code = 0;
    icmp_header.un.echo.id = htons(id); // process id
    icmp_header.un.echo.sequence = htons(sequence); // packet number
    icmp_header.checksum = 0;
    icmp_header.checksum = compute_icmp_checksum( 
        (u_int16_t*) &icmp_header, sizeof(icmp_header) );
        
    //std::cout << "ICMP - CS = " << icmp_header.checksum << "\n";
}

int trace(sockaddr_in &recipient, u_int16_t pid)
{
    int sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    
    char ip[20];
    inet_ntop(AF_INET, &(recipient.sin_addr), ip, sizeof(ip));
    std::cout << "Traceroute to IP: " << ip << ", Max TTL: "<< MAX_TTL << "\n";
    
    struct icmphdr icmp_header;
    struct timeval start[3], end;
    for(int ttl = 1; ttl <= MAX_TTL; ++ttl)
    {
        for(int i = 0; i < 3; ++i)
        {
            prepare_packet (icmp_header, pid, i + 3*(ttl-1));
            Setsockopt (sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
            Sendto ( sockfd, icmp_header, 0, &recipient );
            gettimeofday(&start[i], NULL); 
        }
        
        // TO DO reciv packet
        
        int p = 0; //packet seq % 3
        gettimeofday(&end, NULL); 
        long long secs = end.tv_sec - start[p].tv_sec;
        long long micros = secs*1000000LL + end.tv_usec - start[p].tv_usec;
        if(secs) 
            std::cout << secs << "." << micros << " s\n";
        else 
            std::cout << micros << " ms\n";
    }   
    return 0;
}
