#include<stdio.h>
#include <rte_config.h>
#include <rte_eal.h>

#define COHERENCY_UNIT 64
unsigned long hz=0;
unsigned long tick=0;
size_t  coherency_unit = COHERENCY_UNIT;
void *createInterface(int instance);
int main(int argc,char **argv)
{
    int ret;
    void *ifp;

    ret = rte_eal_init(argc, argv);
    if (ret < 0) {
        //rte_exit(EXIT_FAILURE, "Invalid EAL arguments\n");
        printf("cannot initialize EAL\n");
        exit(0);
    }
    callout_startup(); 
    printf("%s %d\n",__FILE__,__LINE__);
    domaininit(1);
    printf("%s %d\n",__FILE__,__LINE__);
    bpf_setops();
    ifp = createInterface(0);
    printf("%s %d %p\n",__FILE__,__LINE__,ifp);
    configure_if_addr(ifp,inet_addr("192.168.1.1"),inet_addr("255.255.255.0"));
//    arp_init();
//    printf("%s %d\n",__FILE__,__LINE__);
//    ip_init();
//    printf("%s %d\n",__FILE__,__LINE__);
    printf("HELLO\n");
    return 0;
}
