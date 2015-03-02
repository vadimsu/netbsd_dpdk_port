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
#define mutex_destroy(a)
#define mutex_obj_hold(a)
#define mutex_obj_free(a)
#define mutex_owned(a) 1
#define kauth_authorize_network(a,b,c,d,e,f) 0
#define kmem_alloc(a,b) 0
#define PR_NOWAIT 0
#define KM_NOSLEEP 0
#define KM_SLEEP 0
#define IPL_NET 0
extern void exit(int);
#define KASSERT(a) if(!(a)) { printf("ASSERT FAILED HERE %s %d\n",__FILE__,__LINE__);exit(0); }
#define CTASSERT(a) KASSERT(a)
#define splnet() 0
#define splx(a)
#define splsoftnet() 0
#define splvm() 0
extern unsigned long hz;
#endif
