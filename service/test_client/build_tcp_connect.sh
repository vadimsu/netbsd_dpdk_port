FLAGS=-Ofast

gcc $FLAGS -c tcp_connect.c -o tcp_connect.o

gcc tcp_connect.o -L/usr/lib/netbsddpdk -lipnetbsddpdkapi -lrte_ring -lrte_timer -lrte_eal -lrte_mempool -lrte_mbuf -lrte_malloc -lpthread -lrt -ldl -o tcp_connect.bin
