FLAGS=-Ofast

gcc $FLAGS -c route.c -o route.o

gcc route.o -L/usr/lib/netbsddpdk -lnetbsddpdkapi -lrte_ring -lrte_timer -lrte_eal -lrte_mempool -lrte_malloc  -lpthread -lrt -ldl -o route.bin
