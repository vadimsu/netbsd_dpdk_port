rm -rf ./netbsd/build/
make -C netbsd CURRENT_DIR=$(pwd)/ 
make -C netbsd/porting/pool CURRENT_DIR=$(pwd)/ 
make -C netbsd/porting/callout CURRENT_DIR=$(pwd)/
make -C netbsd/porting/kmem CURRENT_DIR=$(pwd)/
make -C netbsd/porting/rte_wrappers CURRENT_DIR=$(pwd)/
make -C netbsd/porting/misc CURRENT_DIR=$(pwd)/ 
make -C glue CURRENT_DIR=$(pwd)/
echo $(pwd)
gcc -g -c test.c -Idpdk-1.8.0/x86_64-native-linuxapp-gcc/include -o test.o
gcc test.o ./glue/build/glue.a ./netbsd/build/libnetinet.a ./netbsd/porting/pool/build/poolporting.a ./netbsd/porting/callout/build/calloutporting.a ./netbsd/porting/kmem/build/kmemporting.a ./netbsd/porting/rte_wrappers/build/rtewrappers.a ./netbsd/porting/misc/build/miscporting.a ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_mbuf.a ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_mempool.a ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_eal.a ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_malloc.a ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_ring.a  ./dpdk-1.8.0/x86_64-native-linuxapp-gcc/lib/librte_timer.a -lpthread -lrt -ldl  -o test
