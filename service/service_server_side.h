
#ifndef __SERVICE_SERVER_SIDE_H__
#define __SERVICE_SERVER_SIDE_H__
//#include <sys/types.h>
//#include <signal.h>
#define PKTMBUF_HEADROOM 128
#define SERVICE_BUFSIZE (PKTMBUF_HEADROOM+1448)
#include <service_log.h>

struct service_clients
{
        TAILQ_ENTRY(service_clients) queue_entry;
        struct rte_ring *client_ring;
	int is_busy;
};

typedef struct
{
    struct socket *socket;
    int ringset_idx;
    int parent_idx;
    struct rte_ring *tx_ring;
    struct rte_ring *rx_ring;
    pid_t apppid;
} socket_satelite_data_t;

extern struct rte_ring *command_ring;
extern struct rte_ring *selectors_ring;
extern struct rte_ring *free_connections_ring;
extern struct rte_mempool *free_connections_pool;
extern struct rte_ring *free_clients_ring;
extern struct service_clients service_clients[SERVICE_CLIENTS_POOL_SIZE];
extern struct rte_mempool *selectors_pool;
extern struct rte_ring *rx_mbufs_ring;
extern struct rte_mempool *free_command_pool;
extern socket_satelite_data_t socket_satelite_data[SERVICE_CONNECTION_POOL_SIZE];
extern service_socket_t *g_service_sockets;
extern service_selector_t *g_service_selectors;
extern uint64_t user_kick_select_tx;
extern uint64_t user_kick_select_rx;
extern uint64_t user_pending_accept;

#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

static inline struct service_memory *service_api_init(int command_bufs_count,
                                                          int rx_bufs_count,
                                                          int tx_bufs_count)
{   
    char ringname[1024];
    int ringset_idx,i; 
    service_socket_t *service_socket;
    service_selector_t *service_selector;

    memset(service_clients,0,sizeof(service_clients));

    free_clients_ring = rte_ring_create(FREE_CLIENTS_RING,SERVICE_CLIENTS_POOL_SIZE,rte_socket_id(), 0);
    if(!free_clients_ring) {
        service_log(SERVICE_LOG_ERR,"cannot create ring %s %d\n",__FILE__,__LINE__);
        exit(0);
    }

    service_log(SERVICE_LOG_INFO,"FREE CLIENTS RING CREATED\n");

    for(ringset_idx = 0;ringset_idx < SERVICE_CLIENTS_POOL_SIZE;ringset_idx++) {
        rte_ring_enqueue(free_clients_ring,(void*)ringset_idx);
	sprintf(ringname,"%s%d",FREE_CLIENTS_RING,ringset_idx);
	service_clients[ringset_idx].client_ring = rte_ring_create(ringname, 64,rte_socket_id(), RING_F_SP_ENQ | RING_F_SC_DEQ);
	if(!service_clients[ringset_idx].client_ring) {
		service_log(SERVICE_LOG_ERR,"cannot create ring %s %d\n",__FILE__,__LINE__);
        	exit(0);
	}
    }

    memset(socket_satelite_data,0,sizeof(void *)*SERVICE_CONNECTION_POOL_SIZE);

    sprintf(ringname,COMMAND_RING_NAME);

    command_ring = rte_ring_create(ringname, command_bufs_count,rte_socket_id(), RING_F_SC_DEQ);
    if(!command_ring) {
        service_log(SERVICE_LOG_ERR,"cannot create ring %s %d\n",__FILE__,__LINE__);
        exit(0);
    }
    service_log(SERVICE_LOG_INFO,"COMMAND RING CREATED\n");
    sprintf(ringname,"rx_mbufs_ring");

    rx_mbufs_ring = rte_ring_create(ringname, rx_bufs_count*SERVICE_CONNECTION_POOL_SIZE,rte_socket_id(), 0);
    if(!rx_mbufs_ring) {
        service_log(SERVICE_LOG_ERR,"cannot create ring %s %d\n",__FILE__,__LINE__);
        exit(0);
    }
    service_log(SERVICE_LOG_INFO,"RX RING CREATED\n");
    sprintf(ringname,FREE_COMMAND_POOL_NAME);

    free_command_pool = rte_mempool_create(ringname, command_bufs_count,
	    			           sizeof(service_cmd_t), 0,
				           0,
				           NULL, NULL,
				           NULL, NULL,
				           rte_socket_id(), 0);
    if(!free_command_pool) {
        service_log(SERVICE_LOG_ERR,"cannot create mempool %s %d\n",__FILE__,__LINE__);
        exit(0);
    }
    service_log(SERVICE_LOG_INFO,"COMMAND POOL CREATED\n");
    sprintf(ringname,FREE_CONNECTIONS_POOL_NAME);

    free_connections_pool = rte_mempool_create(ringname, 1,
	    	  		               sizeof(service_socket_t)*SERVICE_CONNECTION_POOL_SIZE, 0,
				               0,
				               NULL, NULL,
				               NULL, NULL,
				               rte_socket_id(), 0);
    if(!free_connections_pool) {
        service_log(SERVICE_LOG_ERR,"cannot create mempool %s %d\n",__FILE__,__LINE__);
        exit(0);
    }

    service_log(SERVICE_LOG_INFO,"FREE CONNECTIONS POOL CREATED\n");

    if(rte_mempool_get(free_connections_pool,(void **)&g_service_sockets)) {
        service_log(SERVICE_LOG_ERR,"cannot get from mempool %s %d \n",__FILE__,__LINE__);
        exit(0);
    }

    service_socket = g_service_sockets;

    free_connections_ring = rte_ring_create(FREE_CONNECTIONS_RING,SERVICE_CONNECTION_POOL_SIZE,rte_socket_id(), 0);
    if(!free_connections_ring) {
        service_log(SERVICE_LOG_ERR,"cannot create ring %s %d\n",__FILE__,__LINE__);
        exit(0);
    }

    service_log(SERVICE_LOG_INFO,"FREE CONNECTIONS RING CREATED\n");

    for(ringset_idx = 0;ringset_idx < SERVICE_CONNECTION_POOL_SIZE;ringset_idx++,service_socket++) {
          
        memset(service_socket,0,sizeof(service_socket_t));
        service_socket->connection_idx = ringset_idx;
        rte_atomic16_init(&service_socket->read_ready_to_app);
        rte_atomic16_init(&service_socket->write_ready_to_app);
	rte_atomic32_init(&service_socket->tx_space);
        rte_ring_enqueue(free_connections_ring,(void*)service_socket);
        sprintf(ringname,TX_RING_NAME_BASE"%d",ringset_idx);
        socket_satelite_data[ringset_idx].tx_ring = rte_ring_create(ringname, tx_bufs_count,rte_socket_id(), RING_F_SP_ENQ | RING_F_SC_DEQ);
        if(!socket_satelite_data[ringset_idx].tx_ring) {
            service_log(SERVICE_LOG_ERR,"cannot create ring %s %d\n",__FILE__,__LINE__);
            exit(0);
        }
        rte_ring_set_water_mark(socket_satelite_data[ringset_idx].tx_ring,/*tx_bufs_count/10*/1);
        sprintf(ringname,RX_RING_NAME_BASE"%d",ringset_idx);
        socket_satelite_data[ringset_idx].rx_ring = rte_ring_create(ringname, rx_bufs_count,rte_socket_id(), RING_F_SP_ENQ | RING_F_SC_DEQ);
        if(!socket_satelite_data[ringset_idx].rx_ring) {
            service_log(SERVICE_LOG_ERR,"cannot create ring %s %d\n",__FILE__,__LINE__);
            exit(0);
        }
        rte_ring_set_water_mark(socket_satelite_data[ringset_idx].rx_ring,/*rx_bufs_count/10*/1);
        socket_satelite_data[ringset_idx].ringset_idx = -1;
        socket_satelite_data[ringset_idx].parent_idx = -1;
        socket_satelite_data[ringset_idx].socket = NULL;
	socket_satelite_data[ringset_idx].apppid = 0;
    }
    service_log(SERVICE_LOG_INFO,"CONNECTIONS Tx/Rx RINGS CREATED\n");
    
    sprintf(ringname,SELECTOR_POOL_NAME);

    selectors_pool = rte_mempool_create(ringname, 1,
	     		                sizeof(service_selector_t)*SERVICE_SELECTOR_POOL_SIZE, 0,
				               0,
				               NULL, NULL,
				               NULL, NULL,
				               rte_socket_id(), 0);
    if(!selectors_pool) {
        service_log(SERVICE_LOG_ERR,"cannot create mempool %s %d\n",__FILE__,__LINE__);
        exit(0);
    }
    service_log(SERVICE_LOG_INFO,"SELECTOR POOL CREATED\n");
    if(rte_mempool_get(selectors_pool,(void **)&g_service_selectors)) {
        service_log(SERVICE_LOG_ERR,"cannot get from mempool %s %d \n",__FILE__,__LINE__);
        exit(0);
    }
    selectors_ring = rte_ring_create(SELECTOR_RING_NAME,SERVICE_SELECTOR_POOL_SIZE,rte_socket_id(), 0);
    if(!selectors_ring) {
        service_log(SERVICE_LOG_ERR,"cannot create ring %s %d \n",__FILE__,__LINE__);
        exit(0);
    }
    service_log(SERVICE_LOG_INFO,"SELECTOR RING CREATED\n");
    service_selector = g_service_selectors;
    for(ringset_idx = 0;ringset_idx < SERVICE_SELECTOR_POOL_SIZE;ringset_idx++) {
        sprintf(ringname,"SELECTOR_RING_NAME%d",ringset_idx);
        service_selector[ringset_idx].ready_connections = rte_ring_create(ringname, tx_bufs_count,rte_socket_id(), RING_F_SP_ENQ | RING_F_SC_DEQ);
        if(!service_selector[ringset_idx].ready_connections) {
            service_log(SERVICE_LOG_ERR,"cannot create ring %s %d\n",__FILE__,__LINE__);
            exit(0);
        }
        service_log(SERVICE_LOG_INFO,"SELECTOR READY RING#%d CREATED\n",ringset_idx);
        rte_ring_enqueue(selectors_ring,(void*)ringset_idx);
    } 
    service_log(SERVICE_LOG_INFO,"DONE\n");
    return 0;
}
static inline service_cmd_t *service_dequeue_command_buf()
{
    service_cmd_t *cmd;
    if(!rte_ring_count(command_ring))
        return NULL;
    if(rte_ring_sc_dequeue_bulk(command_ring,(void **)&cmd,1)) {
        return NULL;
    }
    return cmd;
}

static inline void service_free_command_buf(service_cmd_t *cmd)
{
    rte_mempool_put(free_command_pool,(void *)cmd);
}

extern unsigned long app_pid;

static inline void service_mark_readable(void *descriptor)
{
    uint32_t ringidx_ready_mask; 
    socket_satelite_data_t *socket_satelite_data = (socket_satelite_data_t *)descriptor;
    if(socket_satelite_data->parent_idx == -1) {
        //service_log(SERVICE_LOG_INFO,"%s %d\n",__FILE__,__LINE__);
        return;
    }
#if 1
//    service_log(SERVICE_LOG_INFO,"%s %d %d\n",__FILE__,__LINE__,socket_satelite_data->apppid);
    if(!rte_atomic16_test_and_set(&g_service_sockets[socket_satelite_data->ringset_idx].read_ready_to_app)) {
        if(socket_satelite_data->apppid)
           kill(socket_satelite_data->apppid,/*SIGUSR1*/10);
        return;
    }
#endif
    ringidx_ready_mask = socket_satelite_data->ringset_idx|(SOCKET_READABLE_BIT << SOCKET_READY_SHIFT);
    rte_ring_enqueue(g_service_selectors[socket_satelite_data->parent_idx].ready_connections,(void *)ringidx_ready_mask);
    if(socket_satelite_data->apppid)
           kill(socket_satelite_data->apppid,/*SIGUSR1*/10);
    user_kick_select_rx++; 
}

static inline void service_post_accepted(service_cmd_t *cmd,void *parent_descriptor)
{
    socket_satelite_data_t *socket_satelite_data = (socket_satelite_data_t *)parent_descriptor;
//    service_log(SERVICE_LOG_INFO,"%s %d %d\n",__FILE__,__LINE__,socket_satelite_data->ringset_idx);
//    cmd->ringset_idx = socket_satelite_data->ringset_idx;
    if(rte_ring_enqueue(socket_satelite_data->rx_ring,(void *)cmd) == -ENOBUFS) { 
        service_free_command_buf(cmd);
    }
    else {
        service_mark_readable(parent_descriptor);
    }
    user_pending_accept++;
}

static inline struct rte_mbuf *service_dequeue_tx_buf(void *descriptor)
{
    struct rte_mbuf *mbuf;
    socket_satelite_data_t *socket_satelite_data = (socket_satelite_data_t *)descriptor;
    if(rte_ring_sc_dequeue_bulk(socket_satelite_data->tx_ring,(void **)&mbuf,1)) { 
        return NULL;
    }
    return mbuf;
}

static inline int service_dequeue_tx_buf_burst(void *descriptor,struct rte_mbuf **mbufs,int max_count)
{
    socket_satelite_data_t *socket_satelite_data = (socket_satelite_data_t *)descriptor; 
    return rte_ring_sc_dequeue_burst(socket_satelite_data->tx_ring,(void **)mbufs,max_count);
}

static inline int service_tx_buf_count(void *descriptor)
{
    socket_satelite_data_t *socket_satelite_data = (socket_satelite_data_t *)descriptor;
    rte_atomic16_set(&g_service_sockets[socket_satelite_data->ringset_idx].write_done_from_app,0);
    return rte_ring_count(socket_satelite_data->tx_ring);
}

static inline int service_rx_buf_free_count(void *descriptor)
{ 
    socket_satelite_data_t *socket_satelite_data = (socket_satelite_data_t *)descriptor;
    return rte_ring_free_count(socket_satelite_data->rx_ring);
}

static inline int service_submit_rx_buf(struct rte_mbuf *mbuf,void *descriptor)
{
    uint32_t ringidx_ready_mask; 
    int rc;
    socket_satelite_data_t *socket_satelite_data = (socket_satelite_data_t *)descriptor;
    rc = rte_ring_sp_enqueue_bulk(socket_satelite_data->rx_ring,(void *)&mbuf,1);
     
    if(rc != 0)
        service_mark_readable(descriptor);
    
    return (rc == -ENOBUFS);
}

static inline int service_mark_writable(void *descriptor)
{
    uint32_t ringidx_ready_mask;
    int rc;
    socket_satelite_data_t *socket_satelite_data = (socket_satelite_data_t *)descriptor;
    if(socket_satelite_data->parent_idx == -1) {
//	service_log(SERVICE_LOG_INFO,"%s %d\n",__FILE__,__LINE__);
        return 1;
    }
    if(!rte_atomic16_test_and_set(&g_service_sockets[socket_satelite_data->ringset_idx].write_ready_to_app)) {
	if(socket_satelite_data->apppid)
        	kill(socket_satelite_data->apppid,/*SIGUSR1*/10);
        return;
    }
    ringidx_ready_mask = socket_satelite_data->ringset_idx|(SOCKET_WRITABLE_BIT << SOCKET_READY_SHIFT);
    rc = rte_ring_enqueue(g_service_selectors[socket_satelite_data->parent_idx].ready_connections,(void *)ringidx_ready_mask);
    user_kick_select_tx++;
//    service_log(SERVICE_LOG_INFO,"%s %d %d\n",__FILE__,__LINE__,socket_satelite_data->apppid);
    if(socket_satelite_data->apppid)
        kill(socket_satelite_data->apppid,/*SIGUSR1*/10);
    return (rc == -ENOBUFS);
}

static inline int service_mark_closed(void *descriptor)
{
    uint32_t ringidx_ready_mask;
    int rc;
    socket_satelite_data_t *socket_satelite_data = (socket_satelite_data_t *)descriptor;
    if(socket_satelite_data->parent_idx == -1) {
//	service_log(SERVICE_LOG_INFO,"%s %d\n",__FILE__,__LINE__);
        return 1;
    }
    ringidx_ready_mask = socket_satelite_data->ringset_idx|(SOCKET_CLOSED_BIT << SOCKET_READY_SHIFT);
    rc = rte_ring_enqueue(g_service_selectors[socket_satelite_data->parent_idx].ready_connections,(void *)ringidx_ready_mask);
//    service_log(SERVICE_LOG_INFO,"%s %d %d\n",__FILE__,__LINE__,socket_satelite_data->apppid);
    if(socket_satelite_data->apppid)
        kill(socket_satelite_data->apppid,/*SIGUSR1*/10);
    return (rc == -ENOBUFS);
}

static inline void service_free_socket(int connidx)
{
   rte_ring_enqueue(free_connections_ring,(void *)&g_service_sockets[connidx]);
}

#endif
