RTE_SDK=$(CURRENT_DIR)dpdk-2.0.0
RTE_TARGET ?= x86_64-native-linuxapp-gcc
include $(RTE_SDK)/mk/rte.vars.mk
SRC_ROOT=$(CURRENT_DIR)
SRCS-y := service_log.c
#opencrypto/cryptosoft_xform.c
#porting/kern/uipc_mbuf.c
#CFLAGS += -Ofast -fno-builtin
CFLAGS += -g
CFLAGS += $(WERROR_FLAGS) -I$(SRC_ROOT)
#-DGSO
#-DMSIZE=256
SHARED = libnetbsddpdklog.so
include $(RTE_SDK)/mk/rte.extshared.mk
