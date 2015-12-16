
#include <special_includes/sys/cdefs.h>
#include <special_includes/sys/param.h>
#include <special_includes/sys/malloc.h>
#include <special_includes/sys/queue.h>
#include <special_includes/sys/protosw.h>
#include <netbsd/net/if.h>
#include <netbsd/net/if_dl.h>
#include <netbsd/net/route.h>
#include <netbsd/net/pfil.h>
#include <netbsd/netinet/in.h>
#include <netbsd/netinet/in_systm.h>
#include <netbsd/netinet/ip.h>
#include <netbsd/netinet/in_pcb.h>
#include <netbsd/netinet/in_proto.h>
#include <netbsd/netinet/in_var.h>
#include <netbsd/netinet/ip_var.h>
#include <netbsd/netinet/if_inarp.h>
#include <netbsd/net/netisr.h>

typedef struct iface_data
{
    TAILQ_ENTRY(iface_data) node;
    void (*cbk)(void *);
    void *arg;
}iface_data_t;

TAILQ_HEAD(ifaces_list,iface_data);

typedef struct
{
   struct ifaces_list iflist; 
}softcpu_t;

typedef struct netisrs
{
	TAILQ_ENTRY(netisrs) node;
	void (*cbk)(void);
	int scheduled;
}netisrs_t;
static netisrs_t netisr_cb[NETISR_MAX];
static TAILQ_HEAD(netisrs_list_t,netisrs) netisrs_list;

softcpu_t softcpu[1];

void *softint_establish(u_int flags, void (*func)(void *), void *arg)
{
	iface_data_t *p_iface_data = malloc(sizeof(iface_data_t),0,0);

	if(!p_iface_data) {
		return NULL;
	}
	memset((void*)p_iface_data,0,sizeof(iface_data_t));
	p_iface_data->cbk = func;
	p_iface_data->arg = arg;
	TAILQ_INSERT_TAIL(&softcpu[0].iflist,p_iface_data,node);
        return p_iface_data;
}

void softint_init()
{
	int idx;

	TAILQ_INIT(&softcpu[0].iflist);
	TAILQ_INIT(&netisrs_list);
	memset(netisr_cb, 0, sizeof(netisr_cb));
	netisr_cb[NETISR_ARP].cbk = &arpintr;
	netisr_cb[NETISR_IP].cbk = &ipintr;
}

void softint_schedule(void *arg)
{
        iface_data_t *p_iface_data = (iface_data_t *)arg;

	p_iface_data->cbk(p_iface_data->arg);
}

void schednetisr(int isr)
{
	if (netisr_cb[isr].scheduled)
		return;
	TAILQ_INSERT_TAIL(&netisrs_list,&netisr_cb[isr],node);
	netisr_cb[isr].scheduled = 1;
}

void softint_run()
{
	netisrs_t *p_netisr = TAILQ_FIRST(&netisrs_list);
	while(p_netisr) {
		TAILQ_REMOVE(&netisrs_list,
			     p_netisr,
			     node);
		p_netisr->scheduled = 0;
		p_netisr->cbk();
		p_netisr = TAILQ_FIRST(&netisrs_list);
	}
}
