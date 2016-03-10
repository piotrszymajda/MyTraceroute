// Autor: Piotr Szymajda - 273 023
#include "packet_support.h"
#include <iostream>


int check_packet(u_int8_t * buff, ssize_t packet_len, u_int16_t pid, int ttl)
{

    if(packet_len > 56)
        packet_len = 56;
        
    //256 * x_i + x_{i+1}
    int rec_pid = ((int)buff[packet_len-4])*256 + ((int)buff[packet_len-3]);
    int rec_seq = ((int)buff[packet_len-2])*256 + ((int)buff[packet_len-1]); 

    //std::cout << "PID " << pid << " " << rec_pid << "  ";
    //std::cout << "SEQ " << seq << "\n";
    
    if ( rec_pid != pid )
        return -1;
        
    if ( rec_seq/3 != ttl-1 )
        return -1;

    return rec_seq;
}

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
