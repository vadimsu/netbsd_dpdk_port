sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/netbsddpdk ./tcp_udp.bin -c 2 -n 1 --proc-type secondary -- --ip2connect 192.168.150.62 --ip2bind 192.168.150.63 --port2bind 7777 --port2connect 7777
