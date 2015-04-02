/*
 * app_glue.c
 *
 *  Created on: Jul 6, 2014
 *      Author: Vadim Suraev vadim.suraev@gmail.com
 *  Contains API functions for building applications
 *  on the top of Linux TCP/IP ported to userland and integrated with DPDK 1.6
 */
#include <stdint.h>
#include <special_includes/sys/types.h>
#include <special_includes/sys/param.h>
#include <special_includes/sys/malloc.h>
#include <special_includes/sys/mbuf.h>
#include <special_includes/sys/queue.h>
#include <special_includes/sys/socket.h>
#include <special_includes/sys/socketvar.h>
#include <special_includes/sys/time.h>
#include <netbsd/netinet/in.h>

TAILQ_HEAD(read_ready_socket_list_head, socket) read_ready_socket_list_head;
uint64_t read_sockets_queue_len = 0;
TAILQ_HEAD(closed_socket_list_head, socket) closed_socket_list_head;
TAILQ_HEAD(write_ready_socket_list_head, socket) write_ready_socket_list_head;
uint64_t write_sockets_queue_len = 0;
TAILQ_HEAD(accept_ready_socket_list_head, socket) accept_ready_socket_list_head;
uint64_t working_cycles_stat = 0;
uint64_t total_cycles_stat = 0;
uint64_t work_prev = 0;
uint64_t total_prev = 0;
/*
 * This callback function is invoked when data arrives to socket.
 * It inserts the socket into a list of readable sockets
 * which is processed in periodic function app_glue_periodic
 * Paramters: a pointer to struct sock, len (dummy)
 * Returns: void
 *
 */
#if 0
static void app_glue_sock_readable(struct sock *sk, int len)
{
	const struct tcp_sock *tp = tcp_sk(sk);
	int target = sock_rcvlowat(sk, 0, INT_MAX);

	if((sk->sk_state != TCP_ESTABLISHED)&&(sk->sk_socket->type == SOCK_STREAM)) {
		return;
	}
	if(!sk->sk_socket) {
		return;
	}
	if(sk->sk_socket->read_queue_present) {
		return;
	}
	sk->sk_socket->read_queue_present = 1;
	TAILQ_INSERT_TAIL(&read_ready_socket_list_head,sk->sk_socket,read_queue_entry);
        read_sockets_queue_len++;
}
/*
 * This callback function is invoked when data canbe transmitted on socket.
 * It inserts the socket into a list of writable sockets
 * which is processed in periodic function app_glue_periodic
 * Paramters: a pointer to struct sock
 * Returns: void
 *
 */
static void app_glue_sock_write_space(struct sock *sk)
{
	if((sk->sk_state != TCP_ESTABLISHED)&&(sk->sk_socket->type == SOCK_STREAM)) {
		return;
	}
	if (sk_stream_is_writeable(sk) && sk->sk_socket) {
		clear_bit(SOCK_NOSPACE, &sk->sk_socket->flags);
		if(sk->sk_socket->write_queue_present) {
			return;
		}
		sk->sk_socket->write_queue_present = 1;
		TAILQ_INSERT_TAIL(&write_ready_socket_list_head,sk->sk_socket,write_queue_entry);
                write_sockets_queue_len++;
	}
}
/*
 * This callback function is invoked when an error occurs on socket.
 * It inserts the socket into a list of closable sockets
 * which is processed in periodic function app_glue_periodic
 * Paramters: a pointer to struct sock
 * Returns: void
 *
 */
static void app_glue_sock_error_report(struct sock *sk)
{
	if(sk->sk_socket) {
		if(sk->sk_socket->closed_queue_present) {
			return;
		}
		sk->sk_socket->closed_queue_present = 1;
		TAILQ_INSERT_TAIL(&closed_socket_list_head,sk->sk_socket,closed_queue_entry);
	}
}
/*
 * This callback function is invoked when a new connection can be accepted on socket.
 * It looks up the parent (listening) socket for the newly established connection
 * and inserts it into the accept queue
 * which is processed in periodic function app_glue_periodic
 * Paramters: a pointer to struct sock
 * Returns: void
 *
 */
static void app_glue_sock_wakeup(struct sock *sk)
{
	struct sock *sock;
        struct tcp_sock *tp;
        tp = tcp_sk(sk);

	sock = __inet_lookup_listener(&init_net/*sk->sk_net*/,
			&tcp_hashinfo,
			sk->sk_daddr,
			sk->sk_dport/*__be16 sport*/,
			sk->sk_rcv_saddr,
			ntohs(tp->inet_conn.icsk_inet.inet_sport),//sk->sk_num/*const unsigned short hnum*/,
			sk->sk_bound_dev_if);
	if(sock) {
		if(sock->sk_socket->accept_queue_present) {
			return;
		}
		sock->sk_socket->accept_queue_present = 1;
		TAILQ_INSERT_TAIL(&accept_ready_socket_list_head,sock->sk_socket,accept_queue_entry);
	}
        else {
              struct tcp_sock *tp;
              tp = tcp_sk(sk);
              printf("%s %d %x %d %x %d %d \n",__FILE__,__LINE__,sk->sk_daddr,sk->sk_dport,sk->sk_rcv_saddr,sk->sk_num,tp->inet_conn.icsk_inet.inet_sport);
              return;
        }
	sock_reset_flag(sk,SOCK_USE_WRITE_QUEUE);
	sk->sk_data_ready = app_glue_sock_readable;
	sk->sk_write_space = app_glue_sock_write_space;
	sk->sk_error_report = app_glue_sock_error_report; 
}
#endif
/*
 * This is a wrapper function for RAW socket creation.
 * Paramters: IP address & port (protocol number) to bind
 * Returns: a pointer to socket structure (handle)
 * or NULL if failed
 *
 */
void *create_raw_socket2(unsigned int ip_addr,unsigned short port)
{
	struct sockaddr_in *sin;
	struct timeval tv;
	struct socket *raw_sock = NULL;
	struct mbuf *m;

	if(socreate(AF_INET,&raw_sock,SOCK_RAW,port,NULL)) {
		printf("cannot create socket %s %d\n",__FILE__,__LINE__);
		return NULL;
	}

	m = m_get(M_WAIT, MT_SONAME);
	if (!m) {
		printf("cannot create socket %s %d\n",__FILE__,__LINE__);
		return NULL;
	}
	m->m_len = sizeof(struct sockaddr);
	sin = mtod(m, struct sockaddr_in *);
	sin->sin_len = sizeof(struct sockaddr_in);

	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = ip_addr;
	sin->sin_port = htons(port);

	if(sobind(raw_sock,m)) {
		printf("cannot bind %s %d\n",__FILE__,__LINE__);
		m_freem(m);
		return NULL;
	}
	m_freem(m);
	return raw_sock;
}
void *create_raw_socket(const char *ip_addr,unsigned short port)
{
    return create_raw_socket(inet_addr(ip_addr),port);
}
/*
 * This is a wrapper function for UDP socket creation.
 * Paramters: IP address & port to bind
 * Returns: a pointer to socket structure (handle)
 * or NULL if failed
 *
 */
void *create_udp_socket2(unsigned int ip_addr,unsigned short port)
{
	struct sockaddr_in *sin;
	struct timeval tv;
	struct socket *udp_sock = NULL;
	struct mbuf *m;
	int rc;

	if(socreate(AF_INET,&udp_sock,SOCK_DGRAM,0,NULL)) {
		printf("cannot create socket %s %d\n",__FILE__,__LINE__);
		return NULL;
	}

	m = m_get(M_WAIT, MT_SONAME);
	if (!m) {
		printf("cannot create socket %s %d\n",__FILE__,__LINE__);
		return NULL;
	}
	m->m_len = sizeof(struct sockaddr);
	sin = mtod(m, struct sockaddr_in *);
	sin->sin_len = sizeof(struct sockaddr_in);
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = ip_addr;
	sin->sin_port = htons(port);

	rc = sobind(udp_sock,m);
	if(rc) {
		printf("cannot bind %s %d %d\n",__FILE__,__LINE__,rc);	
		m_freem(m);
		return NULL;
	}

#if 0
	if(udp_sock->sk) {
            sock_reset_flag(udp_sock->sk,SOCK_USE_WRITE_QUEUE);
            udp_sock->sk->sk_data_ready = app_glue_sock_readable;
            udp_sock->sk->sk_write_space = app_glue_sock_write_space;
            app_glue_sock_write_space(udp_sock->sk);
	}
#endif
	m_freem(m);
	return udp_sock;
}

void *create_udp_socket(const char *ip_addr,unsigned short port)
{
    return create_udp_socket2(inet_addr(ip_addr),port);
}
/*
 * This is a wrapper function for TCP connecting socket creation.
 * Paramters: IP address & port to bind, IP address & port to connect
 * Returns: a pointer to socket structure (handle)
 * or NULL if failed
 *
 */
void *create_client_socket2(unsigned int my_ip_addr,unsigned short my_port,
		            unsigned int peer_ip_addr,unsigned short port)
{
	struct sockaddr_in *sin;
	struct timeval tv;
	struct socket *client_sock = NULL;
	struct mbuf *m;
	struct sockopt soopt;
	
	if(socreate(AF_INET,&client_sock,SOCK_STREAM,0,NULL)) {
		printf("cannot create socket %s %d\n",__FILE__,__LINE__);
		return NULL;
	}
	m = m_get(M_WAIT, MT_SONAME);
	if (!m) {
		printf("cannot create socket %s %d\n",__FILE__,__LINE__);
		return NULL;
	}
	m->m_len = sizeof(struct sockaddr);
	sin = mtod(m, struct sockaddr_in *);
	sin->sin_len = sizeof(struct sockaddr_in);
	tv.tv_sec = -1;
	tv.tv_usec = 0;

	soopt.sopt_level = SOL_SOCKET;
	soopt.sopt_name = SO_RCVTIMEO;
	soopt.sopt_data = &tv;
	if(sosetopt(client_sock,&soopt)) {
		printf("%s %d cannot set notimeout option\n",__FILE__,__LINE__);
	}
	tv.tv_sec = -1;
	tv.tv_usec = 0;
	soopt.sopt_name = SO_SNDTIMEO;
	if(sosetopt(client_sock,&soopt)) {
		printf("%s %d cannot set notimeout option\n",__FILE__,__LINE__);
	}
	while(1) {
		sin->sin_family = AF_INET;
		sin->sin_addr.s_addr = my_ip_addr;
		if(my_port) {
			sin->sin_port = htons(my_port);
		}
		else {
			sin->sin_port = htons(rand() & 0xffff);
		}
		if(sobind(client_sock,m)) {
			printf("cannot bind %s %d\n",__FILE__,__LINE__);
			if(my_port) {
				break;
			}
			continue;
		}
		break;
	}
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = peer_ip_addr;
	sin->sin_port = htons(port);
#if 0
	if(client_sock->sk) {
		client_sock->sk->sk_state_change = app_glue_sock_wakeup;
	}
#endif
	soconnect(client_sock, m);

	m_freem(m);
	return client_sock;
}
/*
 * This is a wrapper function for TCP connecting socket creation.
 * Paramters: IP address & port to bind, IP address & port to connect
 * Returns: a pointer to socket structure (handle)
 * or NULL if failed
 *
 */
void *create_client_socket(const char *my_ip_addr,unsigned short my_port,
		                   const char *peer_ip_addr,unsigned short port)
{
    return create_client_socket2(inet_addr(my_ip_addr),my_port,inet_addr(peer_ip_addr),port);
}
void *create_server_socket2(unsigned int my_ip_addr,unsigned short port)
{
	struct sockaddr_in *sin;
	struct timeval tv;
	struct socket *server_sock = NULL;
	uint32_t bufsize;
	struct mbuf *m;
	struct sockopt soopt;

	if(socreate(AF_INET,&server_sock,SOCK_STREAM,0,NULL)) {
		printf("cannot create socket %s %d\n",__FILE__,__LINE__);
		return NULL;
	}
	m = m_get(M_WAIT, MT_SONAME);
	if (!m) {
		printf("cannot create socket %s %d\n",__FILE__,__LINE__);
		return NULL;
	}
	m->m_len = sizeof(struct sockaddr);
	sin = mtod(m, struct sockaddr_in *);
	sin->sin_len = sizeof(struct sockaddr_in);

	tv.tv_sec = -1;
	tv.tv_usec = 0;
	soopt.sopt_level = SOL_SOCKET;
	soopt.sopt_name = SO_RCVTIMEO;
	soopt.sopt_data = &tv;
	if(sosetopt(server_sock,&soopt)) {
		printf("%s %d cannot set notimeout option\n",__FILE__,__LINE__);
	}
	tv.tv_sec = -1;
	tv.tv_usec = 0;
	soopt.sopt_name = SO_SNDTIMEO;
	if(sosetopt(server_sock,&soopt)) {
		printf("%s %d cannot set notimeout option\n",__FILE__,__LINE__);
	}
#if 0
	bufsize = 0x1000000;
	soopt.sopt_name = SO_SNDBUF;
	soopt.sopt_data = &bufsize;
	if(sosetopt(server_sock,&soopt)) {
		printf("%s %d cannot set bufsize\n",__FILE__,__LINE__);
	}
	soopt.sopt_name = SO_SNDBUF;
	if(sosetopt(server_sock,&soopt)) {
		printf("%s %d cannot set bufsize\n",__FILE__,__LINE__);
	}
#endif
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = my_ip_addr;
	sin->sin_port = htons(port);

	if(sobind(server_sock,m)) {
		printf("cannot bind %s %d\n",__FILE__,__LINE__);
		return NULL;
	}
#if 0
	if(server_sock->sk) {
		server_sock->sk->sk_state_change = app_glue_sock_wakeup;
	}
	else {
		printf("FATAL %s %d\n",__FILE__,__LINE__);exit(0);
	}
#endif
	if(solisten(server_sock,32000)) {
		printf("cannot listen %s %d\n",__FILE__,__LINE__);
		return NULL;
	}

	m_freem(m);
	return server_sock;
}
/*
 * This is a wrapper function for TCP listening socket creation.
 * Paramters: IP address & port to bind
 * Returns: a pointer to socket structure (handle)
 * or NULL if failed
 *
 */
void *create_server_socket(const char *my_ip_addr,unsigned short port)
{
    return create_server_socket2(inet_addr(my_ip_addr),port);
}
/*
 * This function polls the driver for the received packets.Called from app_glue_periodic
 * Paramters: ethernet port number.
 * Returns: None
 *
 */
static inline void app_glue_poll(int port_num)
{
#if 0
	struct net_device *netdev = (struct net_device *)get_dpdk_dev_by_port_num(port_num);

	if(!netdev) {
		printf("Cannot get netdev %s %d\n",__FILE__,__LINE__);
		return;
	}
	netdev->netdev_ops->ndo_poll_controller(netdev);
#endif
}

/*
 * This function must be called before app_glue_periodic is called for the first time.
 * It initializes the readable, writable and acceptable (listeners) lists
 * Paramters: None
 * Returns: None
 *
 */
void app_glue_init()
{
	TAILQ_INIT(&read_ready_socket_list_head);
	TAILQ_INIT(&write_ready_socket_list_head);
	TAILQ_INIT(&accept_ready_socket_list_head);
	TAILQ_INIT(&closed_socket_list_head);
}
/*
 * This function walks on closable, acceptable and readable lists and calls.
 * the application's (user's) function. Called from app_glue_periodic
 * Paramters: None
 * Returns: None
 *
 */
static inline void process_rx_ready_sockets()
{
	struct socket *sock;
        uint64_t idx,limit;

	while(!TAILQ_EMPTY(&closed_socket_list_head)) {
		sock = TAILQ_FIRST(&closed_socket_list_head);
//		user_on_socket_fatal(sock);
		sock->closed_queue_present = 0;
		TAILQ_REMOVE(&closed_socket_list_head,sock,closed_queue_entry);
		soclose(sock);
	}
	while(!TAILQ_EMPTY(&accept_ready_socket_list_head)) {

		sock = TAILQ_FIRST(&accept_ready_socket_list_head);
//		user_on_accept(sock);
		sock->accept_queue_present = 0;
		TAILQ_REMOVE(&accept_ready_socket_list_head,sock,accept_queue_entry);
	}
        idx = 0;
        limit = read_sockets_queue_len;
	while((idx < limit)&&(!TAILQ_EMPTY(&read_ready_socket_list_head))) {
		sock = TAILQ_FIRST(&read_ready_socket_list_head);
                sock->read_queue_present = 0;
		TAILQ_REMOVE(&read_ready_socket_list_head,sock,read_queue_entry);
//                user_data_available_cbk(sock);
                read_sockets_queue_len--;
                idx++;	
	}
}
/*
 * This function walks on writable lists and calls.
 * the application's (user's) function. Called from app_glue_periodic
 * Paramters: None
 * Returns: None
 *
 */
static inline void process_tx_ready_sockets()
{
	struct socket *sock;
        uint64_t idx,limit;
 
        idx = 0;
        limit = write_sockets_queue_len;
	while((idx < limit)&&(!TAILQ_EMPTY(&write_ready_socket_list_head))) {
		sock = TAILQ_FIRST(&write_ready_socket_list_head);
		TAILQ_REMOVE(&write_ready_socket_list_head,sock,write_queue_entry);
                sock->write_queue_present = 0;
//		user_on_transmission_opportunity(sock);
//                set_bit(SOCK_NOSPACE, &sock->flags);
                write_sockets_queue_len--;
	        idx++;
	}
}
/* These are in translation of micros to cycles */
static uint64_t app_glue_drv_poll_interval = 0;
static uint64_t app_glue_timer_poll_interval = 0;
static uint64_t app_glue_tx_ready_sockets_poll_interval = 0;
static uint64_t app_glue_rx_ready_sockets_poll_interval = 0;

static uint64_t app_glue_drv_last_poll_ts = 0;
static uint64_t app_glue_timer_last_poll_ts = 0;
static uint64_t app_glue_tx_ready_sockets_last_poll_ts = 0;
static uint64_t app_glue_rx_ready_sockets_last_poll_ts = 0;

/*
 * This function must be called by application to initialize.
 * the rate of polling for driver, timer, readable & writable socket lists
 * Paramters: drv_poll_interval,timer_poll_interval,tx_ready_sockets_poll_interval,
 * rx_ready_sockets_poll_interval - all in micros
 * Returns: None
 *
 */
void app_glue_init_poll_intervals(int drv_poll_interval,
		                          int timer_poll_interval,
		                          int tx_ready_sockets_poll_interval,
		                          int rx_ready_sockets_poll_interval)
{
#if 0
	printf("%s %d %d %d %d %d\n",__func__,__LINE__,
			drv_poll_interval,timer_poll_interval,tx_ready_sockets_poll_interval,
			rx_ready_sockets_poll_interval);
	float cycles_in_micro = rte_get_tsc_hz()/1000000;
	app_glue_drv_poll_interval = cycles_in_micro*(float)drv_poll_interval;
	app_glue_timer_poll_interval = cycles_in_micro*(float)timer_poll_interval;
	app_glue_tx_ready_sockets_poll_interval = cycles_in_micro*(float)tx_ready_sockets_poll_interval;
	app_glue_rx_ready_sockets_poll_interval = cycles_in_micro*(float)rx_ready_sockets_poll_interval;
	printf("%s %d %"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64"\n",__func__,__LINE__,
			app_glue_drv_poll_interval,app_glue_timer_poll_interval,
			app_glue_tx_ready_sockets_poll_interval,app_glue_rx_ready_sockets_poll_interval);
#endif
}
uint64_t app_glue_periodic_called = 0;
uint64_t app_glue_tx_queues_process = 0;
uint64_t app_glue_rx_queues_process = 0;
/*
 * This function must be called by application periodically.
 * This is the heart of the system, it performs all the driver/IP stack work
 * and timers
 * Paramters: call_flush_queues - if non-zero, the readable, closable and writable queues
 * are processed and user's functions are called.
 * Alternatively, call_flush_queues can be 0 and the application may call
 * app_glue_get_next* functions to get readable, acceptable, closable and writable sockets
 * Returns: None
 *
 */
inline void app_glue_periodic(int call_flush_queues,uint8_t *ports_to_poll,int ports_to_poll_count)
{
	uint64_t ts,ts2,ts3,ts4;
    uint8_t port_idx;

	app_glue_periodic_called++;
//	ts = rte_rdtsc();
	if((ts - app_glue_drv_last_poll_ts) >= app_glue_drv_poll_interval) {
//		ts4 = rte_rdtsc();
		for(port_idx = 0;port_idx < ports_to_poll_count;port_idx++)
		    app_glue_poll(ports_to_poll[port_idx]);
		app_glue_drv_last_poll_ts = ts;
//		working_cycles_stat += rte_rdtsc() - ts4;
	}

	if((ts - app_glue_timer_last_poll_ts) >= app_glue_timer_poll_interval) {
//		ts3 = rte_rdtsc();
//		rte_timer_manage();
		app_glue_timer_last_poll_ts = ts;
//		working_cycles_stat += rte_rdtsc() - ts3;
	}
	if(call_flush_queues) {
		if((ts - app_glue_tx_ready_sockets_last_poll_ts) >= app_glue_tx_ready_sockets_poll_interval) {
//			ts2 = rte_rdtsc();
			app_glue_tx_queues_process++;
			process_tx_ready_sockets();
//			working_cycles_stat += rte_rdtsc() - ts2;
			app_glue_tx_ready_sockets_last_poll_ts = ts;
		}
		if((ts - app_glue_rx_ready_sockets_last_poll_ts) >= app_glue_rx_ready_sockets_poll_interval) {
//			ts2 = rte_rdtsc();
			app_glue_rx_queues_process++;
			process_rx_ready_sockets();
//			working_cycles_stat += rte_rdtsc() - ts2;
//			app_glue_rx_ready_sockets_last_poll_ts = ts;
		}
	}
	else {
		app_glue_tx_ready_sockets_last_poll_ts = ts;
		app_glue_rx_ready_sockets_last_poll_ts = ts;
	}
//	total_cycles_stat += rte_rdtsc() - ts;
}
/*
 * This function may be called to attach user's data to the socket.
 * Paramters: a pointer  to socket (returned, for example, by create_*_socket)
 * a pointer to data to be attached to the socket
 * Returns: None
 *
 */
void app_glue_set_user_data(void *socket,void *data)
{
#if 0
	struct socket *sock = (struct socket *)socket;

	if(!sock) {
		printf("PANIC: socket NULL %s %d \n",__FILE__,__LINE__);while(1);
	}
//	if(sock->sk)
		sock->sk->sk_user_data = data;
//	else
//		printf("PANIC: socket->sk is NULL\n");while(1);
#endif
}
/*
 * This function may be called to get attached to the socket user's data .
 * Paramters: a pointer  to socket (returned, for example, by create_*_socket,)
 * Returns: pointer to data to be attached to the socket
 *
 */
inline void *app_glue_get_user_data(void *socket)
{
#if 0
	struct socket *sock = (struct socket *)socket;
	if(!sock) {
		printf("PANIC: socket NULL %s %d\n",__FILE__,__LINE__);while(1);
	}
	if(!sock->sk) {
		printf("PANIC: socket->sk NULL\n");while(1);
	}
printf("%s %d\n",__FILE__,__LINE__);
	return sock->sk->sk_user_data;
#else
	return NULL;
#endif
}
/*
 * This function may be called to get next closable socket .
 * Paramters: None
 * Returns: pointer to socket to be closed
 *
 */
void *app_glue_get_next_closed()
{
	struct socket *sock;
	void *user_data;
	if(!TAILQ_EMPTY(&closed_socket_list_head)) {
		sock = TAILQ_FIRST(&closed_socket_list_head);
		sock->closed_queue_present = 0;
		TAILQ_REMOVE(&closed_socket_list_head,sock,closed_queue_entry);
#if 0
		if(sock->sk)
			user_data = sock->sk->sk_user_data;
			//kernel_close(sock);
		return user_data;
#endif
	}
	return NULL;
}
/*
 * This function may be called to get next writable socket .
 * Paramters: None
 * Returns: pointer to socket to be written
 *
 */
void *app_glue_get_next_writer()
{
	struct socket *sock;

	if(!TAILQ_EMPTY(&write_ready_socket_list_head)) {
		sock = TAILQ_FIRST(&write_ready_socket_list_head);
		sock->write_queue_present = 0;
		TAILQ_REMOVE(&write_ready_socket_list_head,sock,write_queue_entry);
#if 0
		if(sock->sk)
		    return sock->sk->sk_user_data;
#endif
  	    printf("PANIC: socket->sk is NULL\n");
	}
	return NULL;
}
/*
 * This function may be called to get next readable socket .
 * Paramters: None
 * Returns: pointer to socket to be read
 *
 */
void *app_glue_get_next_reader()
{
	struct socket *sock;
	if(!TAILQ_EMPTY(&read_ready_socket_list_head)) {
		sock = TAILQ_FIRST(&read_ready_socket_list_head);
		sock->read_queue_present = 0;
		TAILQ_REMOVE(&read_ready_socket_list_head,sock,read_queue_entry);
#if 0
		if(sock->sk)
		    return sock->sk->sk_user_data;
#endif
	    printf("PANIC: socket->sk is NULL\n");
	}
	return NULL;
}
/*
 * This function may be called to get next acceptable socket .
 * Paramters: None
 * Returns: pointer to socket on which to accept a new connection
 *
 */
void *app_glue_get_next_listener()
{
	struct socket *sock;
	if(!TAILQ_EMPTY(&accept_ready_socket_list_head))
	{
		sock = TAILQ_FIRST(&accept_ready_socket_list_head);
		sock->accept_queue_present = 0;
		TAILQ_REMOVE(&accept_ready_socket_list_head,sock,accept_queue_entry);
#if 0
		if(sock->sk)
	        return sock->sk->sk_user_data;
#endif
	    printf("PANIC: socket->sk is NULL\n");
		return NULL;
	}
	return NULL;
}
/*
 * This function may be called to close socket .
 * Paramters: a pointer to socket structure
 * Returns: None
 *
 */
void app_glue_close_socket(void *sk)
{
	struct socket *sock = (struct socket *)sk;
	
	if(sock->read_queue_present) {
		TAILQ_REMOVE(&read_ready_socket_list_head,sock,read_queue_entry);
		sock->read_queue_present = 0;
	}
	if(sock->write_queue_present) {
		TAILQ_REMOVE(&write_ready_socket_list_head,sock,write_queue_entry);
		sock->write_queue_present = 0;
	}
	if(sock->accept_queue_present) {
                struct socket *newsock = NULL;
#if 0
	        while(kernel_accept(sock, &newsock, 0) == 0) {
                    soclose(newsock);
                }
#endif
		TAILQ_REMOVE(&accept_ready_socket_list_head,sock,accept_queue_entry);
		sock->accept_queue_present = 0;
	}
	if(sock->closed_queue_present) {
		TAILQ_REMOVE(&closed_socket_list_head,sock,closed_queue_entry);
		sock->closed_queue_present = 0;
	}
#if 0
	if(sock->sk)
		sock->sk->sk_user_data = NULL;
#endif
	soclose(sock);
}
/*
 * This function may be called to estimate amount of data can be sent .
 * Paramters: a pointer to socket structure
 * Returns: number of bytes the application can send
 *
 */
int app_glue_calc_size_of_data_to_send(void *sock)
{
#if 0
	int bufs_count1,bufs_count2,bufs_count3,stream_space,bufs_min;
	struct sock *sk = ((struct socket *)sock)->sk;
	if(!sk_stream_is_writeable(sk)) {
		return 0;
	}
	bufs_count1 = kmem_cache_get_free(get_fclone_cache());
	bufs_count2 = kmem_cache_get_free(get_header_cache());
	bufs_count3 = get_buffer_count();
	if(bufs_count1 > 2) {
		bufs_count1 -= 2;
	}
	if(bufs_count2 > 2) {
		bufs_count2 -= 2;
	}
	bufs_min = min(bufs_count1,bufs_count2);
	bufs_min = min(bufs_min,bufs_count3);
	if(bufs_min <= 0) {
		return 0;
	}
	stream_space = sk_stream_wspace(((struct socket *)sock)->sk);
	return min(bufs_min << 10,stream_space);
#else
	return 0;
#endif
}
/*
 * This function may be called to allocate rte_mbuf from existing pool.
 * Paramters: None
 * Returns: a pointer to rte_mbuf, if succeeded, NULL if failed
 *
 */
#if 0
struct rte_mbuf *app_glue_get_buffer()
{
	return get_buffer();
}
#endif
void app_glue_print_stats()
{
#if 0
	float ratio;

	ratio = (float)(total_cycles_stat - total_prev)/(float)(working_cycles_stat - work_prev);
	total_prev = total_cycles_stat;
	work_prev = working_cycles_stat;
	printf("total %"PRIu64" work %"PRIu64" ratio %f\n",total_cycles_stat,working_cycles_stat,ratio);
	printf("app_glue_periodic_called %"PRIu64"\n",app_glue_periodic_called);
	printf("app_glue_tx_queues_process %"PRIu64"\n",app_glue_tx_queues_process);
	printf("app_glue_rx_queues_process %"PRIu64"\n",app_glue_rx_queues_process);
#endif
}

int app_glue_sendto(struct socket *so, void *data,int len,unsigned int ip_addr,unsigned short port)
{
    struct mbuf *addr,*top;
    struct sockaddr_in *sin;
    int rc;
printf("%s %d\n",__FILE__,__LINE__);
    addr = m_get(M_WAIT, MT_SONAME);
    if (!addr) {
	printf("cannot create socket %s %d\n",__FILE__,__LINE__);
	return NULL;
    }
    addr->m_len = sizeof(struct sockaddr_in);
    sin = mtod(addr, struct sockaddr_in *);

    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = ip_addr;
    sin->sin_port = htons(port);
    top = m_get(M_WAIT, MT_DATA);
    if(!top) {
        m_freem(addr);
        return -1;
    }
    printf("%s %d %p\n",__FILE__,__LINE__,top);
    memcpy(mtod(top, void *),data,len);
    top->m_len = len;
    rc = sosend(so, addr, top,NULL, 0);
    m_freem(addr);
printf("%s %d\n",__FILE__,__LINE__);
    return rc;
}

int app_glue_receivefrom(struct socket *so,unsigned int *ip_addr, unsigned short *port,void *buf,int buflen)
{
    struct mbuf *paddr = NULL,*mp0 = NULL,*controlp = NULL;
    int flags = 0,rc;
printf("%s %d\n",__FILE__,__LINE__);
    rc = soreceive( so, &paddr,&mp0, &controlp, &flags);
    if(!rc) {
	struct mbuf *tmp = mp0;
	unsigned copied = 0;
	char *p = (char *)buf;
	while(tmp) {
		printf("%s %d %p %d\n",__FILE__,__LINE__,tmp,tmp->m_len);
		if(tmp->m_len > 0) {
			if((copied + tmp->m_len) > buflen) {
				printf("%s %d\n",__FILE__,__LINE__);
				break;
			}
			memcpy(&p[copied],tmp->m_data,tmp->m_len);
			copied += tmp->m_len;
		}
		tmp = tmp->m_next;
	}
	m_freem(mp0);
	if(paddr) {
		m_freem(paddr);
	}
    }
printf("%s %d\n",__FILE__,__LINE__);
    return rc;
}
