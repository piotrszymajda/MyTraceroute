#include "traceroute.h"
#include "socket_op.h"

#include <assert.h>
#include <sys/time.h>
#include <iostream>

#define microsec_to_sec 1000000LL
#define microsec_to_milisec 1000LL

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

long long time_interval(struct timeval &start, struct timeval &end)
{
    long long secs = end.tv_sec - start.tv_sec;
    long long micros = secs*microsec_to_sec + end.tv_usec - start.tv_usec;
        
    return micros;
}

void print_time(long long time_us)
{
    assert(time_us < microsec_to_sec);
    std::cout << (double)time_us/microsec_to_milisec << " ms ";//\n";
}

int trace(sockaddr_in &recipient, u_int16_t pid)
{
    int sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    
    char ip[20]; //u_int8_t
    inet_ntop(AF_INET, &(recipient.sin_addr), ip, sizeof(ip));
    std::cout << "Traceroute to IP: " << ip << ", Max TTL: "<< MAX_TTL << "\n";
    
    struct icmphdr icmp_header;
    struct timeval start[3], end;
    struct sockaddr_in sender;
    char buff[IP_MAXPACKET+1];
    
    for(int ttl = 1; ttl <= MAX_TTL; ++ttl)
    {
        std::cout << " " << ttl << "  ";
        if(ttl < 10) std::cout << " ";
  
        for(int i = 0; i < 3; ++i)
        {
            prepare_packet (icmp_header, pid, i + 3*(ttl-1));
            Setsockopt (sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
            Sendto ( sockfd, icmp_header, 0, &recipient );
            gettimeofday(&start[i], NULL); 
        }
        
        // TO DO reciv packet
        short recived_pac = 0;
        bool host_name = false;
        
        while(recived_pac < 3)
        {
            ssize_t	rec_bytes = Recvfrom(sockfd, buff, MSG_DONTWAIT, sender);
	
            gettimeofday(&end, NULL);
            
            if(rec_bytes < 0) 
            {// if time between send last packet and now is > TIMEOUT stop waiting 
				if(time_interval(start[2], end) > TIMEOUT) 
				    break;
				continue;
			}
            
            if(!host_name)
            {
                char sender_ip[20]; 
		        inet_ntop(AF_INET, &(sender.sin_addr), sender_ip, sizeof(sender_ip));
		        std::cout << sender_ip << " \t";
		        host_name = true;
		    }    
                
            //struct iphdr* 		ip_header = (struct iphdr*) buff;
		    //ssize_t				ip_header_len = 4 * ip_header->ihl;
		
            int p = 0; //packet seq % 3
            long long time_us = time_interval(start[p], end);
            if( time_us >= TIMEOUT )
            {
                std::cout << "TIMEOUT\n";
            }
            else
            {
                print_time(time_us);
            }
        }
        
        std::cout << "\n";
        if( sender.sin_addr.s_addr - recipient.sin_addr.s_addr  == 0)
            return  0;
    }   
    return 0;
}
