cmd_netinet/udp_usrreq.o = cc -Wp,-MD,netinet/.udp_usrreq.o.d.tmp -Ofast -fno-builtin  -I/host/dpdknetbsd//special_includes -I/host/dpdknetbsd//netbsd -I/host/dpdknetbsd/ -I/host/dpdknetbsd//dpdk-1.8.0/x86_64-native-linuxapp-gcc/include -D_KERNEL -D__NetBSD__ -DINET -D_NETBSD_SOURCE -DSTACK_MBUFS_COUNT=16384   -o netinet/udp_usrreq.o -c /host/dpdknetbsd/netbsd/netinet/udp_usrreq.c 
