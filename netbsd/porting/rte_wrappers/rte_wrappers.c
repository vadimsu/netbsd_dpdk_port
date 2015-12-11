

#include <rte_common.h>
#include <rte_config.h>
#include <rte_mbuf.h>
#include <rte_lcore.h>
#include <rte_malloc.h>
#include <rte_memcpy.h>
#include <rte_ethdev.h>
#include <rte_ether.h>
#include <rte_timer.h>
#include <rte_cycles.h>
#if 0
#include <special_includes/sys/param.h>
//#include <special_includes/sys/systm.h>
#include <special_includes/sys/malloc.h>
#include <lib/libkern/libkern.h>
#include <special_includes/sys/mbuf.h>
#endif
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

static struct rte_eth_conf port_conf = {
        .rxmode = {
                .split_hdr_size = 0,
                .header_split   = 0, /**< Header Split disabled */
                .hw_ip_checksum = 0, /**< IP checksum offload disabled */
                .hw_vlan_filter = 0, /**< VLAN filtering disabled */
                .jumbo_frame    = 0, /**< Jumbo Frame Support disabled */
                .hw_strip_crc   = 0, /**< CRC stripped by hardware */
                .mq_mode = ETH_MQ_RX_NONE/*ETH_MQ_RX_RSS*/,
        },
        .txmode = {
                .mq_mode = ETH_MQ_TX_NONE/*ETH_MQ_TX_VMDQ_ONLY*/,
        },
};
#define MAX_PKT_BURST 1
#define RX_PTHRESH 8
#define RX_HTHRESH 8
#define RX_WTHRESH 0
static const struct rte_eth_rxconf rx_conf = {
        .rx_thresh = {
                .pthresh = RX_PTHRESH,
                .hthresh = RX_HTHRESH,
                .wthresh = RX_WTHRESH,
        },
        .rx_free_thresh = MAX_PKT_BURST/*0*/,
        .rx_drop_en = 0,
};

#define TX_PTHRESH 32
#define TX_HTHRESH 0
#define TX_WTHRESH 0

static struct rte_eth_txconf tx_conf = {
        .tx_thresh = {
                .pthresh = TX_PTHRESH,
                .hthresh = TX_HTHRESH,
                .wthresh = TX_WTHRESH,
        },
        .tx_free_thresh = /*0*/MAX_PKT_BURST, /* Use PMD default values */
        .tx_rs_thresh = /*0*/MAX_PKT_BURST, /* Use PMD default values */
        .txq_flags = ((uint32_t)/*ETH_TXQ_FLAGS_NOMULTSEGS | \*/
                            ETH_TXQ_FLAGS_NOOFFLOADS),
};

#define MBUF_SIZE 2048
#define MBUFS_PER_RX_QUEUE 8192

static struct rte_mempool **init_rx_queue_mempools(int queue_count)
{
	uint16_t queue_id;
	char pool_name[1024];
	struct rte_mempool **pools;

	pools = rte_malloc("", sizeof(struct rte_mempool *)*queue_count, 0);

	for(queue_id = 0;queue_id < queue_count;queue_id++) {
                sprintf(pool_name,"rx_pool_%d",queue_id);
                pools[queue_id] =
                                rte_mempool_create(pool_name, MBUFS_PER_RX_QUEUE,
                                                   MBUF_SIZE, 0,
                                                   sizeof(struct rte_pktmbuf_pool_private),
                                                   rte_pktmbuf_pool_init, NULL,
                                                   rte_pktmbuf_init, NULL,
                                                   rte_socket_id(), 0);
                 if (pools[queue_id] == NULL)
                        rte_panic("Cannot init direct mbuf pool\n");
        }
	return pools;
}

static void setup_rx_queues(int portid, int queue_count, int nb_rxd)
{
	uint16_t queue_id;
	struct rte_mempool **pools = init_rx_queue_mempools(queue_count);

	for (queue_id = 0; queue_id < queue_count; queue_id++) {
		rte_eth_rx_queue_setup(portid, queue_id, nb_rxd, rte_eth_dev_socket_id(portid),
					&rx_conf, pools[queue_id]);
	}
}

static void setup_tx_queues(int portid, int queue_count, int nb_txd)
{
	uint16_t queue_id;
	int ret;

	for(queue_id = 0;queue_id < queue_count;queue_id++) {
		ret = rte_eth_tx_queue_setup(portid, queue_id, nb_txd,
                                        rte_eth_dev_socket_id(portid), &tx_conf);
		if (ret < 0)
			rte_exit(EXIT_FAILURE, "rte_eth_tx_queue_setup:err=%d, port=%u\n",
				ret, (unsigned) portid);
	}
}

int init_device(int portid, int queue_count)
{
	struct rte_eth_dev_info dev_info;
	rte_eth_dev_info_get(portid, &dev_info);
	int nb_txd, nb_rxd;
	int ret = rte_eth_dev_configure(portid, queue_count, queue_count, &port_conf);
	if (ret < 0)
		goto error_ret;
	if (!dev_info.tx_offload_capa) {
		nb_rxd = nb_txd = 256;
		tx_conf.txq_flags = ETH_TXQ_FLAGS_NOOFFLOADS;
	} else {
		nb_rxd = nb_txd = 4096;
		tx_conf.txq_flags = 0;
	}
	setup_rx_queues(portid, queue_count, nb_rxd);
	setup_tx_queues(portid, queue_count, nb_txd);
	ret = rte_eth_dev_start(portid);
error_ret:
	return ret;
}
void *m_devget(char *, int, int, void *, void *);
void poll_rx(void *ifp, int portid, int queue_id)
{
	struct rte_mbuf *mbufs[MAX_PKT_BURST];
	void *m;
	int i;
	int received = rte_eth_rx_burst(portid, queue_id, mbufs, MAX_PKT_BURST);
	if (received <= 0)
		return;

	for (i = 0;i < received; i++) {
		m = m_devget(rte_pktmbuf_mtod(mbufs[i], char *), rte_pktmbuf_data_len(mbufs[i]), 0, ifp, mbufs[i]);	
		ether_input(ifp,m);
	}
}

void transmit_mbuf(int portid, int queue_id, void *pdesc, int len, void *buf)
{
	struct rte_mbuf *mbuf = (struct rte_mbuf *)pdesc;

	if (len == 0) {
		printf("%s %d\n",__func__,__LINE__);
		goto free_mbuf;
	}

	if(rte_pktmbuf_data_len(mbuf) == 0) {
		rte_pktmbuf_data_len(mbuf) = len;
		rte_pktmbuf_pkt_len(mbuf) = len;
		mbuf->data_off += (char *)buf - rte_pktmbuf_mtod(mbuf, char *);
	}

	int  transmitted= rte_eth_tx_burst(portid, queue_id, &mbuf, 1);
	if (transmitted == 1)
		return;
	printf("%s %d\n",__func__,__LINE__);
free_mbuf:
	rte_pktmbuf_free(mbuf);
}

void get_port_mac_addr(int portid, char *mac_addr)
{
	struct ether_addr macaddr;

	rte_eth_macaddr_get(portid,&macaddr);
	memcpy(mac_addr, macaddr.addr_bytes, 6);
}

int get_max_drv_poll_interval_in_micros(int port_num)
{
	struct rte_eth_link rte_eth_link;
	float bytes_in_sec,bursts_in_sec,bytes_in_burst;

	rte_eth_link_get(port_num,&rte_eth_link);
	switch(rte_eth_link.link_speed) {
	case ETH_LINK_SPEED_10:
		bytes_in_sec = 10/8;
		break;
	case ETH_LINK_SPEED_100:
		bytes_in_sec = 100/8;
		break;
	case ETH_LINK_SPEED_1000:
		bytes_in_sec = 1000/8;
		break;
	case ETH_LINK_SPEED_10000:
		bytes_in_sec = 10000/8;
		break;
	default:
		bytes_in_sec = 10000/8;
	}
	if(rte_eth_link.link_duplex == ETH_LINK_HALF_DUPLEX)
		bytes_in_sec /= 2;
	bytes_in_sec *= 1024*1024;/* x1M*/
	/*MTU*BURST_SIZE*/
	bytes_in_burst = 1448*MAX_PKT_BURST;
	bursts_in_sec = bytes_in_sec / bytes_in_burst;
	/* micros in sec div burst in sec = max poll interval in micros */
	return (int)(1000000/bursts_in_sec)/2/*safe side*/; /* casted to int, is not so large */
}

static struct rte_mempool *mbufs_mempool = NULL;

int get_buffer_count()
{
#if 0
	return rte_mempool_count(mbufs_mempool);
#else
	return 0;
#endif
}

void create_app_mbufs_mempool()
{
	mbufs_mempool = rte_mempool_create("mbufs_mempool", APP_MBUFS_POOL_SIZE,
				MBUF_SIZE, 0,
				sizeof(struct rte_pktmbuf_pool_private),
				rte_pktmbuf_pool_init, NULL,
				rte_pktmbuf_init, NULL,
				rte_socket_id(), 0);
	if (mbufs_mempool == NULL) {
		printf("%s %d cannot initialize mbufs_pool\n",__func__,__LINE__);
		exit(0);
	}
}

extern volatile unsigned long tick;
extern unsigned long hz;

static struct rte_timer systick;

static void systick_expiry_cbk(struct rte_timer *tim,void *arg)
{
	tick++;
	rte_timer_reset(&systick,rte_get_timer_hz()/hz,SINGLE,rte_lcore_id(),systick_expiry_cbk,NULL);
}
void init_systick()
{
	rte_timer_init(&systick);
	rte_timer_reset(&systick,rte_get_timer_hz()/hz,SINGLE,rte_lcore_id(),systick_expiry_cbk,NULL);
}
