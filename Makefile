RTE_SDK=$(CURRENT_DIR)dpdk-1.8.0
RTE_TARGET ?= x86_64-native-linuxapp-gcc
include $(RTE_SDK)/mk/rte.extvars.mk
SRC_ROOT=$(CURRENT_DIR)
SRCS-y := netbsd/net/if.c netbsd/net/if_ethersubr.c netbsd/net/if_etherip.c netbsd/net/radix.c netbsd/net/route.c \
netbsd/net/raw_cb.c netbsd/net/raw_usrreq.c netbsd/netinet/if_arp.c netbsd/netinet/in.c netbsd/netinet/in_proto.c \
netbsd/netinet/ip_input.c

CFLAGS += -Ofast
CFLAGS += $(WERROR_FLAGS)
NETBSD_HEADERS=-I$(SRC_ROOT)/special_includes -I$(SRC_ROOT)
DPDK_HEADERS=$(SRC_ROOT)/dpdk-1.8.0/x86_64-native-linuxapp-gcc/include
ALL_HEADERS = $(NETBSD_HEADERS) -I$(DPDK_HEADERS)
CFLAGS += $(ALL_HEADERS) -D_KERNEL -DMSIZE=256 -D_NETBSD_SOURCE -D__NetBSD__
#-DGSO
LIB = libnetinet.a
include $(RTE_SDK)/mk/rte.extlib.mk
