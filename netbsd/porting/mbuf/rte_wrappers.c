

#include <rte_common.h>
#include <rte_config.h>
#include <rte_mbuf.h>

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
