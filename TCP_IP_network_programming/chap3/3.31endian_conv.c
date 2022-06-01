/*
uint32_t htonl(uint32_t hostlong);

uint16_t htons(uint16_t hostshort);

uint32_t ntohl(uint32_t netlong);

uint16_t ntohs(uint16_t netshort);
*/

#include <stdio.h>
#include <arpa/inet.h>

int main(void)
{
    uint16_t host_port = 0x1234;
    uint16_t net_port;
    uint32_t host_addr = 0x12345678;
    uint32_t net_addr;

    net_port = htons(host_port);
    net_addr = htonl(host_addr);

    printf("Host ordered port: %#x \n", host_port);
    printf("Network ordered port: %#x \n", net_port);

    printf("Host ordered address: %#x \n", host_addr);
    printf("Network ordered address: %#x \n", net_addr);
    return 0;
}