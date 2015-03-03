RTE_SDK=$(CURRENT_DIR)dpdk-1.8.0
RTE_TARGET ?= x86_64-native-linuxapp-gcc
include $(RTE_SDK)/mk/rte.extvars.mk
SRC_ROOT=$(CURRENT_DIR)
SRCS-y := netbsd/net/if.c netbsd/net/if_ethersubr.c netbsd/net/if_etherip.c netbsd/net/radix.c netbsd/net/route.c netbsd/net/rtbl.c \
netbsd/net/raw_cb.c netbsd/net/raw_usrreq.c netbsd/netinet/if_arp.c netbsd/netinet/in.c netbsd/netinet/in_proto.c netbsd/netinet/raw_ip.c \
netbsd/netinet/ip_input.c netbsd/netinet/ip_output.c netbsd/netinet/in4_cksum.c netbsd/netinet/tcp_input.c netbsd/netinet/tcp_output.c \
netbsd/netinet/tcp_sack.c netbsd/netinet/tcp_timer.c netbsd/netinet/tcp_subr.c netbsd/netinet/tcp_vtw.c netbsd/netinet/tcp_usrreq.c \
netbsd/netinet/udp_usrreq.c netbsd/netinet/ip_icmp.c netbsd/netinet/ip_reass.c netbsd/netinet/cpu_in_cksum.c \
netbsd/netinet/in_pcb.c netbsd/netinet/in_cksum.c netbsd/netinet/ip_id.c netbsd/netinet/tcp_congctl.c netbsd/netinet/igmp.c \
netbsd/porting/callout/callout.c netbsd/netinet/rfc6056.c \
netbsd/porting/kern/subr_percpu.c netbsd/porting/kern/subr_hash.c netbsd/porting/mbuf/uipc_mbuf.c netbsd/porting/kern/uipc_mbuf2.c \
netbsd/porting/kern/uipc_socket2.c netbsd/porting/kern/uipc_socket.c netbsd/porting/kern/uipc_domain.c \
netbsd/net/rtsock.c netbsd/net/link_proto.c netbsd/lib/libkern/intoa.c netbsd/porting/kern/kern_tc.c netbsd/porting/kern/subr_once.c
#netbsd/opencrypto/cryptosoft_xform.c
#netbsd/porting/kern/uipc_mbuf.c
CFLAGS += -Ofast
CFLAGS += $(WERROR_FLAGS)
NETBSD_HEADERS=-I$(SRC_ROOT)/special_includes -I$(SRC_ROOT)/netbsd -I$(SRC_ROOT)
DPDK_HEADERS=$(SRC_ROOT)/dpdk-1.8.0/x86_64-native-linuxapp-gcc/include
ALL_HEADERS = $(NETBSD_HEADERS) -I$(DPDK_HEADERS)
CFLAGS += $(ALL_HEADERS) -D_KERNEL -D__NetBSD__ -DINET -D_NETBSD_SOURCE -DSTACK_MBUFS_COUNT=16384
#-DGSO
#-DMSIZE=256
LIB = libnetinet.a
include $(RTE_SDK)/mk/rte.extlib.mk
