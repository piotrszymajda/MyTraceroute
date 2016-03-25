// Autor: Piotr Szymajda - 273 023
#include "packet_support.h"
#include <iostream>


int check_packet(u_int8_t * buff, u_int16_t pid, int ttl)
{

    struct ip* ip_packet = (struct ip*) buff;
    struct icmp* icmp_packet = (struct icmp*)(buff + ip_packet->ip_hl*4);

    if( !(icmp_packet->icmp_type == ICMP_TIME_EXCEEDED &&
		  icmp_packet->icmp_code == ICMP_EXC_TTL) &&
		  icmp_packet->icmp_type != ICMP_ECHOREPLY ) 
    {
		 return -1;
	}

    int rec_pid, rec_seq;

	if(icmp_packet->icmp_type == ICMP_TIME_EXCEEDED) 
	{
		struct ip* orig = (struct ip*)(icmp_packet->icmp_data);
		icmp_packet = (struct icmp*)(icmp_packet->icmp_data + orig->ip_hl*4);
		rec_pid = htons(icmp_packet->icmp_id);
		rec_seq = htons(icmp_packet->icmp_seq);
	}

	else if(icmp_packet->icmp_type == ICMP_ECHOREPLY)
	{
		rec_pid = htons(icmp_packet->icmp_id);
		rec_seq = htons(icmp_packet->icmp_seq);
	}   

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
