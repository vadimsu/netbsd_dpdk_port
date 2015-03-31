
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
	TAILQ_INIT(&softcpu[0].iflist);
}

void softint_schedule(void *arg)
{
        iface_data_t *p_iface_data = (iface_data_t *)arg;

	p_iface_data->cbk(p_iface_data->arg);
}

void schednetisr(int isr)
{
	ipintr();
}
