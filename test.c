#include<stdio.h>
#define COHERENCY_UNIT 64
unsigned long hz=0;
unsigned long tick=0;
size_t  coherency_unit = COHERENCY_UNIT;
int main(int argc,char **argv)
{
    arp_init();
    ip_init();
    tcp_init();
    udp_init();
    printf("HELLO\n");
    return 0;
}
