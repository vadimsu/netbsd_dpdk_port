cmd_net/radix.o = cc -Wp,-MD,net/.radix.o.d.tmp -Ofast -fno-builtin  -I/host/dpdknetbsd//special_includes -I/host/dpdknetbsd//netbsd -I/host/dpdknetbsd/ -I/host/dpdknetbsd//dpdk-1.8.0/x86_64-native-linuxapp-gcc/include -D_KERNEL -D__NetBSD__ -DINET -D_NETBSD_SOURCE -DSTACK_MBUFS_COUNT=16384   -o net/radix.o -c /host/dpdknetbsd/netbsd/net/radix.c 
