RTE_SDK=$(CURRENT_DIR)dpdk-1.8.0
RTE_TARGET ?= x86_64-native-linuxapp-gcc
include $(RTE_SDK)/mk/rte.extvars.mk
SRC_ROOT=$(CURRENT_DIR)
SRCS-y := net/if.c net/if_ethersubr.c net/if_etherip.c net/radix.c net/route.c net/rtbl.c \
net/raw_cb.c net/raw_usrreq.c netinet/if_arp.c netinet/in.c netinet/in_proto.c netinet/raw_ip.c \
netinet/ip_input.c netinet/ip_output.c netinet/in4_cksum.c netinet/tcp_input.c netinet/tcp_output.c \
netinet/tcp_sack.c netinet/tcp_timer.c netinet/tcp_subr.c netinet/tcp_vtw.c netinet/tcp_usrreq.c \
netinet/udp_usrreq.c netinet/ip_icmp.c netinet/ip_reass.c netinet/cpu_in_cksum.c \
netinet/in_pcb.c netinet/in_cksum.c netinet/ip_id.c netinet/tcp_congctl.c netinet/igmp.c \
porting/callout/callout.c netinet/rfc6056.c net/if_loop.c net/bpf_stub.c netinet/in_offload.c \
porting/kern/subr_percpu.c porting/kern/subr_hash.c porting/mbuf/uipc_mbuf.c porting/kern/uipc_mbuf2.c \
porting/kern/uipc_socket2.c porting/kern/uipc_socket.c porting/kern/uipc_domain.c netinet/ip_encap.c \
net/rtsock.c net/link_proto.c lib/libkern/intoa.c porting/kern/kern_tc.c porting/kern/subr_once.c
#opencrypto/cryptosoft_xform.c
#porting/kern/uipc_mbuf.c
CFLAGS += -Ofast -fno-builtin
CFLAGS += $(WERROR_FLAGS)
NETBSD_HEADERS=-I$(SRC_ROOT)/special_includes -I$(SRC_ROOT)/netbsd -I$(SRC_ROOT)
DPDK_HEADERS=$(SRC_ROOT)/dpdk-1.8.0/x86_64-native-linuxapp-gcc/include
ALL_HEADERS = $(NETBSD_HEADERS) -I$(DPDK_HEADERS)
CFLAGS += $(ALL_HEADERS) -D_KERNEL -D__NetBSD__ -DINET -D_NETBSD_SOURCE -DSTACK_MBUFS_COUNT=16384
#-DGSO
#-DMSIZE=256
LIB = libnetinet.a
include $(RTE_SDK)/mk/rte.extlib.mk
