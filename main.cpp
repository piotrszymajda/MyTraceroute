/*
Program typu "Traceroute" - pierwsze zad. programistyczne na SK
Autor: Piotr Szymajda - 273 023
*/
#include <iostream>
#include <cstring>

#include "traceroute.h"

using namespace std;

void man();

int main(int argc, char *argv[])
{
    if (argc == 2)
    {   
        struct sockaddr_in recp;
        bzero (&recp, sizeof(recp));
        recp.sin_family = AF_INET;
        
        if(inet_pton(AF_INET, argv[1], &(recp.sin_addr)) == 1) // check "is argv[1] ip adress ?"
        {
            cout << "IP: " << argv[1] <<"\n";
            // traceroute to specific IP
            if(trace(recp) != 0)
            {
                return 3;
            }
        }
        else 
        {
            if ( strcmp (argv [1], "help") != 0 )
                cout << "(!)Error: Wrong argument type.\n";
            man();
            return 1;
        }
    }
    else
    {
        cout << "(!)Error: Wrong numbers of arguments.\n";
        man();
        return 2;
    }

    return 0;
}

/*
Function displays manual
*/
void man()
{
    cout << "MyTraceroute - print the route of packets from local to network host\n";
    cout << "Usage:\n";
    cout << "\t./MyTraceroute ip_adress\n";
    cout << "Example:\n";
    cout << "\t./MyTraceroute 8.8.8.8\n";
}
