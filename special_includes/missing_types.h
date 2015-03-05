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
#define M_CRYPTO_DATA 0
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
#define IPL_NET 0
extern void exit_inernal(int);
#ifndef KASSERT
#define KASSERT(a) if(!(a)) { printf("ASSERT FAILED HERE %s %d\n",__FILE__,__LINE__);exit(0); }
#endif
#ifndef CTASSERT
#define CTASSERT(a) KASSERT(a)
#endif
#define splnet() 0
#define splx(a)
#define splsoftnet() 0
#define splvm() 0
//#define rt_newaddrmsg(a,b,c,d)
extern unsigned long hz;
extern unsigned long tick;
#define ppsratecheck(a,b,c) 1
#define atomic_inc_uint(c) (*(c))++
//#define COMPATNAME(name) name
#endif
