FLAGS=-Ofast

gcc $FLAGS -c udp.c -o udp.o

gcc udp.o -L/usr/lib/netbsddpdk -lnetbsddpdkapi -lrte_ring -lrte_timer -lrte_eal -lrte_mempool -lrte_malloc -lpthread -lrt -ldl -o udp.bin
