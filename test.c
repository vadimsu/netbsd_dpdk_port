#include<stdio.h>
#include <rte_config.h>
#include <rte_eal.h>
#define COHERENCY_UNIT 64
unsigned long hz=0;
unsigned long tick=0;
size_t  coherency_unit = COHERENCY_UNIT;
int main(int argc,char **argv)
{
    int ret;

    ret = rte_eal_init(argc, argv);
    if (ret < 0) {
        //rte_exit(EXIT_FAILURE, "Invalid EAL arguments\n");
        printf("cannot initialize EAL\n");
        exit(0);
    }
   
    /* init RTE timer library */
    rte_timer_subsystem_init();
    printf("%s %d\n",__FILE__,__LINE__);
    domaininit(1);
    printf("%s %d\n",__FILE__,__LINE__);
//    arp_init();
//    printf("%s %d\n",__FILE__,__LINE__);
//    ip_init();
//    printf("%s %d\n",__FILE__,__LINE__);
    printf("HELLO\n");
    return 0;
}
