RTE_SDK=$(CURRENT_DIR)dpdk-2.0.0
RTE_TARGET ?= x86_64-native-linuxapp-gcc
include $(RTE_SDK)/mk/rte.vars.mk
SRC_ROOT=$(CURRENT_DIR)
SRCS-y := callout.c 
CFLAGS += $(OPTIMIZATION)
#CFLAGS += -g
CFLAGS += $(WERROR_FLAGS)
DPDK_HEADERS = $(SRC_ROOT)/dpdk-2.0.0/x86_64-native-linuxapp-gcc/include
ALL_HEADERS = -I$(DPDK_HEADERS) 
CFLAGS += $(ALL_HEADERS) -DRTE_MAX_LCORE=32
#-DGSO
#-D_KERNEL
#-DMSIZE=256
#-D_NETBSD_SOURCE
#-D__NetBSD__
#-DINET
SHARED = libcalloutporting.so
include $(RTE_SDK)/mk/rte.extshared.mk
