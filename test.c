#include<stdio.h>
#include <rte_config.h>
#include <rte_eal.h>

#define COHERENCY_UNIT 64
unsigned long hz=0;
unsigned long tick=0;
size_t  coherency_unit = COHERENCY_UNIT;
void *createInterface(int instance);
void *create_udp_socket(const char *ip_addr,unsigned short port);
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
    rt_init();
    soinit2();
    mbinit();
    ifp = createInterface(0);
    printf("%s %d %p\n",__FILE__,__LINE__,ifp);
    configure_if_addr(ifp,inet_addr("192.168.1.1"),inet_addr("255.255.255.0"));
    void *socket1 = create_udp_socket("192.168.1.1",7777);
    void *socket2 = create_udp_socket("192.168.1.1",7778);
    int rc = app_glue_sendto(socket1, "SOME DATA", 10 ,inet_addr("192.168.1.1"),7778);
    printf("rc=%d\n",rc);
    if(socket1) {
        app_glue_close_socket(socket1);
    }
    printf("HELLO\n");
    return 0;
}
