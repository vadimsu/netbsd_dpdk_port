RTE_SDK=$(CURRENT_DIR)dpdk-1.8.0
RTE_TARGET ?= x86_64-native-linuxapp-gcc
include $(RTE_SDK)/mk/rte.extvars.mk
SRC_ROOT=$(CURRENT_DIR)
SRCS-y := rte_wrappers.c
CFLAGS += -Ofast
CFLAGS += $(WERROR_FLAGS)
DPDK_HEADERS = $(SRC_ROOT)/dpdk-1.8.0/x86_64-native-linuxapp-gcc/include
ALL_HEADERS = -I$(DPDK_HEADERS)
CFLAGS += $(ALL_HEADERS) -DRTE_MAX_LCORE=32
#-DGSO
#-D_KERNEL
#-DMSIZE=256
#-D_NETBSD_SOURCE
#-D__NetBSD__
#-DINET
LIB = mbufporting.a
include $(RTE_SDK)/mk/rte.extlib.mk
