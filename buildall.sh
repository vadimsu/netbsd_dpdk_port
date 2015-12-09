rm -rf ./netbsd/build/
make -C ./dpdk-2.0.0 config T=x86_64-native-linuxapp-gcc
make -C ./dpdk-2.0.0 install T=x86_64-native-linuxapp-gcc
make -C log CURRENT_DIR=$(pwd)/ 
make -C service CURRENT_DIR=$(pwd)/ RTE_SDK=$(pwd)/dpdk-2.0.0
make -C netbsd CURRENT_DIR=$(pwd)/ RTE_SDK=$(pwd)/dpdk-2.0.0
make -C netbsd/porting/pool CURRENT_DIR=$(pwd)/ 
make -C netbsd/porting/callout CURRENT_DIR=$(pwd)/
make -C netbsd/porting/kmem CURRENT_DIR=$(pwd)/
make -C netbsd/porting/rte_wrappers CURRENT_DIR=$(pwd)/
make -C netbsd/porting/misc CURRENT_DIR=$(pwd)/ 
make -C glue CURRENT_DIR=$(pwd)/ RTE_SDK=$(pwd)/dpdk-2.0.0
echo $(pwd)
#gcc -g -c test.c -Idpdk-2.0.0/x86_64-native-linuxapp-gcc/include -o test.o
#gcc ./netbsd/porting/pool/build/poolporting.a ./netbsd/porting/callout/build/calloutporting.a ./netbsd/porting/kmem/build/kmemporting.a ./netbsd/porting/rte_wrappers/build/rtewrappers.a ./netbsd/porting/misc/build/miscporting.a -L/usr/lib/ipaugenblick -lrte_mbuf -lrte_mempool -lrte_eal -lrte_malloc -lrte_ring  -lrte_timer -lethdev -lpthread -lrt -ldl -lglue test.o -lnetinet -o test
