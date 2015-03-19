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
    printf("%s %d\n",__FILE__,__LINE__);
    void *socket1,*socket2;
    int i;
    for(i = 0; i < 1000000;i++) {
        socket1 = create_udp_socket("192.168.1.1",7777);
   //     printf("%s %d\n",__FILE__,__LINE__);
        socket2 = create_udp_socket("192.168.1.1",7778);
     //   printf("%s %d\n",__FILE__,__LINE__);
        if(socket1) {
            app_glue_close_socket(socket1);
        }
        if(socket2) {
            app_glue_close_socket(socket2);
        }
    }
    socket1 = create_udp_socket("192.168.1.1",7777);
    printf("%s %d\n",__FILE__,__LINE__);
    socket2 = create_udp_socket("192.168.1.1",7778);
    int rc = app_glue_sendto(socket1, "SOME DATA", 10 ,inet_addr("192.168.1.1"),7778);
    printf("rc=%d\n",rc);
    int buflen = 1024;
    void *buf = NULL;
    unsigned short port;
    unsigned int ip_addr;
    rc = app_glue_receivefrom(socket2,&ip_addr, &port,&buf,buflen);
    printf("rc=%d\n",rc);
    if(socket1) {
        app_glue_close_socket(socket1);
    }
    if(socket2) {
        app_glue_close_socket(socket2);
    }
    printf("HELLO\n");
    return 0;
}
