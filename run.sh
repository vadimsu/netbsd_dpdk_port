sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./netbsd/build:./netbsd/porting/rte_wrappers/build:./netbsd/porting/callout/build:./netbsd/porting/kmem/build:./netbsd/porting/misc/build:./netbsd/porting/pool/build:/usr/lib/ipaugenblick ./glue/build/uss -c 0x3 -n 1 -d librte_pmd_ixgbe.so -- -p 0x3
