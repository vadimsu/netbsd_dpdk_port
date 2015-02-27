#ifndef __MISSING_TYPES_H_
#define __MISSING_TYPES_H_

/*typedef unsigned char bool;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;*/
#define M_NOWAIT 0
#define M_WAITOK 0
#define M_ZERO 0
#define M_DEVBUF 0
#define malloc(a,b,c) NULL
#define free(a,b)
#define mutex_init(a,b,c)
#define cv_signal(a)
#define mutex_enter(a)
#define mutex_exit(a)
#define kauth_authorize_network(a,b,c,d,e,f) 0
#define kmem_alloc(a,b) 0
#define PR_NOWAIT 0
extern unsigned long hz;
#endif
