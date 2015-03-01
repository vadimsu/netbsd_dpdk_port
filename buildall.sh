rm -rf build/
make CURRENT_DIR=$(pwd)/ 
make -C netbsd/porting/pool CURRENT_DIR=$(pwd)/ 
#make -C netbsd/porting/mbuf CURRENT_DIR=$(pwd)/
gcc -c test.c -o test.o
gcc test.o ./build/libnetinet.a ./netbsd/porting/pool/build/poolporting.a ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_mempool.a ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_eal.a ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_malloc.a ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_ring.a -lpthread -lrt -ldl  -o test
