

#include <rte_common.h>
#include <rte_config.h>
#include <rte_mbuf.h>
#include <rte_lcore.h>
#include <rte_malloc.h>

void *allocate_mbuf(void *pool)
{
	struct rte_mempool *rte_pool = (struct rte_mempool*)pool;

	struct rte_mbuf *data_buf;

        data_buf = rte_pktmbuf_alloc(rte_pool);

	return data_buf;
}

char *get_mbuf_data(void *buf)
{
	struct rte_mbuf *mbuf = (struct rte_mbuf *)buf;

	return rte_pktmbuf_mtod(mbuf,char *);
}

unsigned get_cpu_count()
{
    return rte_lcore_count();
}

void  *kern_malloc(unsigned long size, struct malloc_type *type, int flags)
{
	return rte_malloc(NULL,size,0);
}
void kern_free(void *addr, struct malloc_type *type)
{
	rte_free(addr);
}
