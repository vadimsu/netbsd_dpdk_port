
#ifndef __SERVICE__COMMON_H__
#define __SERVICE__COMMON_H__

enum
{
    SERVICE_DUMMY_COMMAND = 0,
    SERVICE_OPEN_SOCKET_COMMAND,
    SERVICE_SOCKET_CONNECT_BIND_COMMAND,
    SERVICE_LISTEN_SOCKET_COMMAND,
    SERVICE_SETSOCKOPT_COMMAND,
    SERVICE_OPEN_SOCKET_FEEDBACK,
    SERVICE_SOCKET_TX_KICK_COMMAND,
    SERVICE_SOCKET_RX_KICK_COMMAND,
    SERVICE_SOCKET_ACCEPTED_COMMAND,
    SERVICE_SET_SOCKET_RING_COMMAND,
    SERVICE_SET_SOCKET_SELECT_COMMAND,
    SERVICE_SOCKET_READY_FEEDBACK,
    SERVICE_SOCKET_CLOSE_COMMAND,
    SERVICE_SOCKET_TX_POOL_EMPTY_COMMAND,
    SERVICE_ROUTE_ADD_COMMAND,
    SERVICE_ROUTE_DEL_COMMAND,
    SERVICE_CONNECT_CLIENT,
    SERVICE_DISCONNECT_CLIENT,
    SERVICE_NEW_IFACES,
    SERVICE_NEW_ADDRESSES,
    SERVICE_END_OF_RECORD,
    SERVICE_SOCKET_SHUTDOWN_COMMAND,
    SERVICE_SOCKET_DECLINE_COMMAND
};

typedef struct
{
    int family;
    int type;
    unsigned long pid;
}__attribute__((packed))service_open_sock_cmd_t;

typedef struct
{
    unsigned long socket_descr;
    unsigned int ipaddr;
    unsigned short port;
}__attribute__((packed))service_open_accepted_socket_t;

typedef struct
{
    unsigned long socket_descr;
}__attribute__((packed))service_decline_accepted_socket_t;

typedef struct
{
    unsigned long socket_descr;
}__attribute__((packed))service_socket_kick_cmd_t;

typedef struct
{
    unsigned long socket_descr;
    unsigned long pid;
}__attribute__((packed))service_set_socket_ring_cmd_t;

typedef struct
{
    int socket_select; 
    unsigned long pid;
}__attribute__((packed))service_set_socket_select_cmd_t;

typedef struct
{
    unsigned int ipaddr;
    unsigned short port;
    unsigned short is_connect;
}__attribute__((packed))service_socket_connect_bind_cmd_t;

typedef struct
{
    unsigned int dest_ipaddr;
    unsigned int dest_mask;
    unsigned int next_hop;
    short        metric;
}__attribute__((packed))service_route_cmd_t;

typedef struct
{
    int level;
    int optname;
    int optlen;
    char optval[128];
}__attribute__((packed))service_setsockopt_cmd_t;

#define SOCKET_READABLE_BIT 1
#define SOCKET_WRITABLE_BIT 2
#define SOCKET_CLOSED_BIT   4
#define SOCKET_READY_SHIFT 16
#define SOCKET_READY_MASK 0xFFFF

typedef struct
{
    unsigned int bitmask;
}__attribute__((packed))service_socket_ready_feedback_t;

typedef struct
{
    int how;
}__attribute__((packed))service_socket_shutdown_t;

typedef struct
{
    int cmd;
    unsigned int ringset_idx;
    int          parent_idx;
    union {
        service_open_sock_cmd_t open_sock;
        service_socket_kick_cmd_t socket_kick_cmd;
        service_open_accepted_socket_t accepted_socket;
        service_set_socket_ring_cmd_t set_socket_ring;
        service_set_socket_select_cmd_t set_socket_select;
        service_socket_ready_feedback_t socket_ready_feedback;
        service_socket_connect_bind_cmd_t socket_connect_bind;
	service_route_cmd_t route;
	service_setsockopt_cmd_t setsockopt;
	service_socket_shutdown_t socket_shutdown;
	service_decline_accepted_socket_t socket_decline;
    }u;
}__attribute__((packed))service_cmd_t;

typedef struct
{
    unsigned long connection_idx; /* to be aligned */
    rte_atomic16_t  read_ready_to_app;
    rte_atomic16_t  write_ready_to_app;
    rte_atomic16_t  write_done_from_app;
    unsigned int local_ipaddr;
    unsigned short local_port;
    rte_atomic32_t tx_space;
}__attribute__((packed))service_socket_t;

typedef struct
{
    struct rte_ring  *ready_connections;
}__attribute__((packed))service_selector_t;

#define COMMAND_POOL_SIZE 4096*2
#define DATA_RINGS_SIZE 1024
#define FREE_CONNECTIONS_POOL_NAME "free_connections_pool"
#define FREE_CONNECTIONS_RING "free_connections_ring"
#define FREE_CLIENTS_RING "free_clients_ring"
#define COMMAND_RING_NAME "command_ring"
#define FREE_COMMAND_POOL_NAME "free_command_pool"
#define RX_RING_NAME_BASE "rx_ring"
#define TX_RING_NAME_BASE "tx_ring"
#define ACCEPTED_RING_NAME "accepted_ring"
#define FREE_ACCEPTED_POOL_NAME "free_accepted_pool"
#define SELECTOR_POOL_NAME "selector_pool"
#define SELECTOR_RING_NAME "selector_ring"
#define SERVICE_CONNECTION_POOL_SIZE 16
#define SERVICE_CLIENTS_POOL_SIZE 64
#define SERVICE_SELECTOR_POOL_SIZE 64
#define COMMON_NOTIFICATIONS_POOL_NAME "common_notifications_pool_name"
#define COMMON_NOTIFICATIONS_RING_NAME "common_notifications_ring_name"

extern struct rte_mempool *free_command_pool;

static inline service_cmd_t *service_get_free_command_buf()
{
    service_cmd_t *cmd;
    if(rte_mempool_get(free_command_pool,(void **)&cmd))
        return NULL;
    return cmd;
}

#endif /* __SERVICE_MEMORY_COMMON_H__ */
