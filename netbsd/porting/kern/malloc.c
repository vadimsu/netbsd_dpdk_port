#include <sys/cdefs.h>

#include <sys/malloc.h>

MALLOC_DEFINE(M_DEVBUF,"","");
MALLOC_DEFINE(M_DMAMAP,"","");
MALLOC_DEFINE(M_FREE,"","");
MALLOC_DEFINE(M_PCB,"","");
MALLOC_DEFINE(M_TEMP,"","");

/* XXX These should all be declared elsewhere. */
MALLOC_DEFINE(M_RTABLE,"","");
MALLOC_DEFINE(M_FTABLE,"","");
MALLOC_DEFINE(M_UFSMNT,"","");
MALLOC_DEFINE(M_NETADDR,"","");
MALLOC_DEFINE(M_IPMOPTS,"","");
MALLOC_DEFINE(M_IPMADDR,"","");
MALLOC_DEFINE(M_MRTABLE,"","");
MALLOC_DEFINE(M_BWMETER,"","");
