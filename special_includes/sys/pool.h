/*	$NetBSD: pool.h,v 1.73 2012/01/27 19:48:41 para Exp $	*/

/*-
 * Copyright (c) 1997, 1998, 1999, 2000, 2007 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Paul Kranenburg; by Jason R. Thorpe of the Numerical Aerospace
 * Simulation Facility, NASA Ames Research Center.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SYS_POOL_H_
#define _SYS_POOL_H_

struct rte_mempool;

struct pool {
    struct rte_mempool *mempool;
};

#define	PR_WAITOK	0x01	/* Note: matches KM_SLEEP */
#define PR_NOWAIT	0x02	/* Note: matches KM_NOSLEEP */
#define PR_WANTED	0x04
#define PR_PHINPAGE	0x40
#define PR_LOGGING	0x80
#define PR_LIMITFAIL	0x100	/* even if waiting, fail if we hit limit */
#define PR_RECURSIVE	0x200	/* pool contains pools, for vmstat(8) */
#define PR_NOTOUCH	0x400	/* don't use free items to keep internal state*/
#define PR_NOALIGN	0x800	/* don't assume backend alignment */
#define	PR_LARGECACHE	0x1000	/* use large cache groups */

//#define rte_mempool pool_cache 

typedef unsigned int u_int;
typedef unsigned long size_t;
typedef unsigned long uint64_t;

typedef struct rte_mempool *pool_cache_t;

void		pool_init(struct pool *, size_t, u_int, u_int,
		    int, const char *, void *, int);
void		pool_destroy(struct pool *);

void		pool_set_drain_hook(struct pool *,
		    void (*)(void *, int), void *);

void		*pool_get(struct pool *, int);
void		pool_put(struct pool *, void *);
int		pool_reclaim(struct pool *);

int		pool_prime(struct pool *, int);
void		pool_setlowat(struct pool *, int);
void		pool_sethiwat(struct pool *, int);
void		pool_sethardlimit(struct pool *, int, const char *, int);
void		pool_drain_start(struct pool **, uint64_t *);
bool	pool_drain_end(struct pool *, uint64_t);

/*
 * Debugging and diagnostic aides.
 */
void		pool_print(struct pool *, const char *);
int		pool_chk(struct pool *, const char *);

/*
 * Pool cache routines.
 */
pool_cache_t	pool_cache_init(size_t, u_int, u_int, u_int, const char *,
		    void *, int, int (*)(void *, void *, int),
		    void (*)(void *, void *), void *);
void		pool_cache_destroy(pool_cache_t);
void		*pool_cache_get(pool_cache_t, int);
void 		*pool_data_mbuf_clone(void *mempool, void *to_clone);
void		pool_cache_put(pool_cache_t, void *);
void		pool_cache_destruct_object(pool_cache_t, void *);
void		pool_cache_invalidate(pool_cache_t);
bool	pool_cache_reclaim(pool_cache_t);
void		pool_cache_set_drain_hook(pool_cache_t,
		    void (*)(void *, int), void *);
void		pool_cache_setlowat(pool_cache_t, int);
void		pool_cache_sethiwat(pool_cache_t, int);
void		pool_cache_sethardlimit(pool_cache_t, int, const char *, int);

#endif /* _SYS_POOL_H_ */
