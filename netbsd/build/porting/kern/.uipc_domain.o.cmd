cmd_porting/kern/uipc_domain.o = cc -Wp,-MD,porting/kern/.uipc_domain.o.d.tmp -Ofast -fno-builtin  -I/host/dpdknetbsd//special_includes -I/host/dpdknetbsd//netbsd -I/host/dpdknetbsd/ -I/host/dpdknetbsd//dpdk-1.8.0/x86_64-native-linuxapp-gcc/include -D_KERNEL -D__NetBSD__ -DINET -D_NETBSD_SOURCE -DSTACK_MBUFS_COUNT=16384   -o porting/kern/uipc_domain.o -c /host/dpdknetbsd/netbsd/porting/kern/uipc_domain.c 
