rm -rf ./netbsd/build/
make -C netbsd CURRENT_DIR=$(pwd)/ 
make -C netbsd/porting/pool CURRENT_DIR=$(pwd)/ 
make -C netbsd/porting/kmem CURRENT_DIR=$(pwd)/
make -C netbsd/porting/misc CURRENT_DIR=$(pwd)/ 
#make -C netbsd/porting/mbuf CURRENT_DIR=$(pwd)/
gcc -c test.c -o test.o
gcc test.o ./netbsd/build/libnetinet.a ./netbsd/porting/pool/build/poolporting.a ./netbsd/porting/kmem/build/kmemporting.a ./netbsd/porting/misc/build/miscporting.a ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_mempool.a ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_eal.a ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_malloc.a ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_ring.a -lpthread -lrt -ldl  -o test
