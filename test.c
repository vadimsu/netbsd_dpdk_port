#include<stdio.h>
#include <rte_config.h>
#include <rte_eal.h>

#define COHERENCY_UNIT 64
unsigned long hz=0;
unsigned long tick=0;
size_t  coherency_unit = COHERENCY_UNIT;
void *createInterface(int instance);
void *create_udp_socket(const char *ip_addr,unsigned short port);
void *create_client_socket(const char *my_ip_addr,unsigned short my_port,
		                   const char *peer_ip_addr,unsigned short port);
void *create_server_socket(const char *my_ip_addr,unsigned short port);
extern void *sender_so;
extern void *receiver_so;
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
    softint_init();
    callout_startup(); 
    printf("%s %d\n",__FILE__,__LINE__);
    domaininit(1);
    printf("%s %d\n",__FILE__,__LINE__);
    bpf_setops();
    rt_init();
    soinit();
    mbinit();
    app_glue_init();
    ifp = createInterface(0);
    printf("%s %d %p\n",__FILE__,__LINE__,ifp);
    configure_if_addr(ifp,inet_addr("192.168.1.1"),inet_addr("255.255.255.0"));
    printf("%s %d\n",__FILE__,__LINE__);
    void *socket1,*socket2;

    createLoopbackInterface();
    unsigned i = 0,iterations_count = 100;

    sender_so = create_udp_socket("127.0.0.1",7777);
    printf("%s %d\n",__FILE__,__LINE__);
    receiver_so = create_udp_socket("127.0.0.1",7778);
    user_on_transmission_opportunity(sender_so); 
    while(i < iterations_count) {
	    user_on_transmission_opportunity(sender_so);
	    softint_run();
	    app_glue_periodic(1,NULL,0);
	    i++;
    }
printf("%s %d\n",__FILE__,__LINE__);
    if(sender_so) {
        app_glue_close_socket(sender_so);
    }
    if(receiver_so) {
        app_glue_close_socket(receiver_so);
    }

printf("%s %d\n",__FILE__,__LINE__);
    receiver_so = create_server_socket("127.0.0.1",7777);
    if(!receiver_so) {
        printf("cannot open server socket\n");
        return -1;
    }
    sender_so = create_client_socket("127.0.0.1",11111,"127.0.0.1",7777);
    if(!sender_so) {
        printf("cannot open client socket\n");
        return -1;
    }
    softint_run();
    softint_run();
    softint_run();
    i = 0;
    while(i < iterations_count) {
	    user_on_transmission_opportunity(sender_so);
	    softint_run();
	    softint_run();
            softint_run();
	    app_glue_periodic(1,NULL,0);
	    i++;
    }
    //app_glue_close_socket(socket1);
    //app_glue_close_socket(socket2);
    printf("The END\n");
    return 0;
}
