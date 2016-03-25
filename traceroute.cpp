// Autor: Piotr Szymajda - 273 023
#include "traceroute.h"
#include "socket_op.h"
#include "packet_support.h"

#include <assert.h>
#include <sys/time.h>
#include <iostream>

#define microsec_to_sec 1000000LL
#define microsec_to_milisec 1000LL

#define N_PACKETS 3

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

void print_host_name(sockaddr_in &host)
{
    char host_ip[20]; 
    inet_ntop(AF_INET, &(host.sin_addr), host_ip, sizeof(host_ip));
    std::cout << host_ip << " \t";
    if(strlen(host_ip) < 10 )
    {
        std::cout << "\t";
    }
}

int trace(sockaddr_in &recipient, u_int16_t pid)
{
    int sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    
    char ip[20]; //u_int8_t
    inet_ntop(AF_INET, &(recipient.sin_addr), ip, sizeof(ip));
    std::cout << "Traceroute to IP: " << ip << ", Max TTL: "<< MAX_TTL << "\n";
    
    struct icmphdr icmp_header;
    struct timeval start[N_PACKETS], end, timeout;
    struct sockaddr_in sender;
    u_int8_t buff[IP_MAXPACKET+1];
    
    
    for(int ttl = 1; ttl <= MAX_TTL; ++ttl)
    {
        std::cout << " " << ttl << "  ";
        if(ttl < 10) std::cout << " ";
  
        for(int i = 0; i < N_PACKETS; ++i)
        {
            prepare_packet (icmp_header, pid, i + N_PACKETS*(ttl-1));
            Setsockopt (sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
            Sendto ( sockfd, icmp_header, 0, &recipient );
            gettimeofday(&start[i], NULL); 
        }
        
        // Recived packets
        short     recived_pac = 0;
        bool      host_name = false;
        long long time_average = 0;
        
        unsigned long prev_sender_ip = 0;
        
        timeout.tv_sec = TIMEOUT / microsec_to_sec;
        timeout.tv_usec = TIMEOUT % microsec_to_sec;
        
        while(recived_pac < N_PACKETS)
        {
            fd_set read_fd;
            FD_ZERO(&read_fd);
            FD_SET(sockfd, &read_fd);
            int rc = select(sockfd+1, &read_fd, NULL, NULL, &timeout);
   
            ssize_t rec_bytes = Recvfrom(sockfd, buff, MSG_DONTWAIT, sender);
    
            gettimeofday(&end, NULL);
   
            if(rc == 0)
                break;
            else if(rc < 0)
            {
                print_error("select", -1);
            }
            
            if(rec_bytes < 0) 
            {// if time between send last packet and now is >=> TIMEOUT stop waiting 
                if(time_interval(start[N_PACKETS-1], end) >= TIMEOUT) 
                    break;
                continue;
            }
            
            int p = check_packet(buff, pid, ttl);
            
            if(p == -1)
                continue;
            
            if(!host_name)
            {
                print_host_name(sender);
                host_name = true;
            }
            else if(sender.sin_addr.s_addr - prev_sender_ip != 0)
            {
                print_host_name(sender);
            }
            prev_sender_ip = sender.sin_addr.s_addr;
                
            p %= N_PACKETS;
            long long time_us = time_interval(start[p], end);
            assert( time_us <= TIMEOUT );
            time_average += time_us;
            //print_time(time_us);
 
            ++recived_pac;
        }
        
        if(recived_pac < N_PACKETS)
        {   
            if(recived_pac == 0)
                std::cout << "*";
            else
                std::cout << "???";
        }
        else
            print_time(time_average / N_PACKETS);
        
        
        std::cout << "\n";
        
        if( sender.sin_addr.s_addr - recipient.sin_addr.s_addr  == 0)
            return  0;
    }   
    return 0;
}
