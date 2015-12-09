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
sudo mkdir -p /usr/lib/netbsddpdk
sudo cp netbsd/build/libbsdnetinet.so /usr/lib/netbsddpdk/.
sudo cp netbsd/porting/callout/build/libcalloutporting.so /usr/lib/netbsddpdk/.
sudo cp netbsd/porting/kmem/build/libkmemporting.so /usr/lib/netbsddpdk/.
sudo cp netbsd/porting/misc/build/libmiscporting.so /usr/lib/netbsddpdk/.
sudo cp netbsd/porting/pool/build/libpoolporting.so /usr/lib/netbsddpdk/.
sudo cp netbsd/porting/rte_wrappers/build/librtewrappers.so /usr/lib/netbsddpdk/.
sudo cp service/build/libnetbsddpdkservice.so /usr/lib/netbsddpdk/.
sudo cp log/build/libnetbsddpdklog.so /usr/lib/netbsddpdk/.
make -C glue CURRENT_DIR=$(pwd)/ RTE_SDK=$(pwd)/dpdk-2.0.0
make -C service/app_api CURRENT_DIR=$(pwd)/ RTE_SDK=$(pwd)/dpdk-2.0.0
sudo mkdir -p /usr/include/netbsddpdk
sudo cp service/app_api/api.h /usr/include/netbsddpdk/.
echo $(pwd)
#gcc -g -c test.c -Idpdk-2.0.0/x86_64-native-linuxapp-gcc/include -o test.o
#gcc ./netbsd/porting/pool/build/poolporting.a ./netbsd/porting/callout/build/calloutporting.a ./netbsd/porting/kmem/build/kmemporting.a ./netbsd/porting/rte_wrappers/build/rtewrappers.a ./netbsd/porting/misc/build/miscporting.a -L/usr/lib/ipaugenblick -lrte_mbuf -lrte_mempool -lrte_eal -lrte_malloc -lrte_ring  -lrte_timer -lethdev -lpthread -lrt -ldl -lglue test.o -lnetinet -o test
