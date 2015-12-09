FLAGS=-Ofast

gcc $FLAGS -c tcp_udp.c -o tcp_udp.o

gcc tcp_udp.o -L/usr/lib/netbsddpdk -lnetbsddpdkapi -lrte_ring -lrte_timer -lrte_eal -lrte_mempool -lrte_malloc  -lpthread -lrt -ldl -o tcp_udp.bin
