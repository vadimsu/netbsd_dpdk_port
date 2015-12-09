RTE_SDK=$(CURRENT_DIR)dpdk-2.0.0
RTE_TARGET ?= x86_64-native-linuxapp-gcc
include $(RTE_SDK)/mk/rte.vars.mk
SRC_ROOT=$(CURRENT_DIR)
SRCS-y := app_glue.c device.c
CFLAGS += -Ofast
CFLAGS += $(WERROR_FLAGS)
DPDK_HEADERS = $(SRC_ROOT)/dpdk-2.0.0/x86_64-native-linuxapp-gcc/include
NETBSD_HEADERS=-I$(SRC_ROOT)special_includes -I$(SRC_ROOT)netbsd/ -I$(SRC_ROOT)
ALL_HEADERS = $(NETBSD_HEADERS)
CFLAGS += $(ALL_HEADERS) -DRTE_MAX_LCORE=32 -D_KERNEL -D_NETBSD_SOURCE -D_KERNEL -DINET
LDFLAGS += -L$(CURRENT_DIR)netbsd/build -L$(CURRENT_DIR)netbsd/porting/rte_wrappers/build \
-L$(CURRENT_DIR)netbsd/porting/callout/build -L$(CURRENT_DIR)netbsd/porting/kmem/build \
-L$(CURRENT_DIR)netbsd/porting/misc/build -L$(CURRENT_DIR)netbsd/porting/pool/build \
-L$(CURRENT_DIR)service/build -L$(CURRENT_DIR)log/build \
-lbsdnetinet -lcalloutporting -lkmemporting -lmiscporting -lpoolporting -lrtewrappers -lnetbsddpdkservice -lnetbsddpdklog
#-DGSO
#
#-DMSIZE=256
#-D_NETBSD_SOURCE
#-D__NetBSD__
#
APP = uss
include $(RTE_SDK)/mk/rte.extapp.mk
