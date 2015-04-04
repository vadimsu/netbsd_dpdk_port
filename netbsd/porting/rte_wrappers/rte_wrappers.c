

#include <rte_common.h>
#include <rte_config.h>
#include <rte_mbuf.h>
#include <rte_lcore.h>
#include <rte_malloc.h>
#include <rte_memcpy.h>

typedef int malloc_type;
void *allocate_mbuf(void *pool)
{
	struct rte_mempool *rte_pool = (struct rte_mempool*)pool;

	struct rte_mbuf *data_buf;

        data_buf = rte_pktmbuf_alloc(rte_pool);

	return data_buf;
}

void free_mbuf(void *mbuf)
{
	struct rte_mbuf *rte_mbuf = (struct rte_mbuf*)mbuf;

        rte_pktmbuf_free(rte_mbuf);
}

void increment_refcnt(void *mbuf)
{
	struct rte_mbuf *rte_mbuf = (struct rte_mbuf*)mbuf;

        rte_mbuf_refcnt_update(rte_mbuf,1);
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

void  *kern_malloc(unsigned long size, malloc_type type, int flags)
{
	return rte_malloc(NULL,size,0);
}
void kern_free(void *addr, malloc_type type)
{
	rte_free(addr);
}

void internal_memcpy(void *d,const void *s, size_t sz)
{
    rte_memcpy(d, s, sz);
}

void internal_memset(void *d, int v, size_t sz)
{
    memset(d, v, sz);
}
