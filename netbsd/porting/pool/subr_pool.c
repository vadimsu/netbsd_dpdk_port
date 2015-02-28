/*	$NetBSD: subr_pool.c,v 1.194.2.2 2014/05/21 20:34:38 bouyer Exp $	*/

/*-
 * Copyright (c) 1997, 1999, 2000, 2002, 2007, 2008, 2010
 *     The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Paul Kranenburg; by Jason R. Thorpe of the Numerical Aerospace
 * Simulation Facility, NASA Ames Research Center, and by Andrew Doran.
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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <string.h>
#include <sys/queue.h>
#include <stdarg.h>

#include <rte_common.h>
#include <rte_byteorder.h>
#include <rte_log.h>
#include <rte_tailq.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ring.h>

#include<rte_mempool.h>
typedef unsigned char bool;
#include <../../../special_includes/sys/pool.h>
/*
 * Initialize the given pool resource structure.
 *
 * We export this routine to allow other kernel parts to declare
 * static pools that must be initialized before malloc() is available.
 */
void
pool_init(struct pool *pp, size_t size, u_int count, u_int ioff, int flags,
    const char *wchan, void *dummy, int ipl)
{
    struct rte_mempool *mempool = rte_mempool_create(wchan, count, size /*elt_size*/,
                   0 /*cache_size*/, 0 /*private_data_size*/,
                   NULL/*rte_mempool_ctor_t * */, NULL/* *mp_init_arg*/,
                   NULL/*rte_mempool_obj_ctor_t **/, NULL/* *obj_init_arg*/,
                   rte_socket_id(), ipl);
    pp->mempool = mempool;
}

/*
 * De-commision a pool resource.
 */
void
pool_destroy(struct pool *pp)
{
    printf("not implemented %s %d\n",__FILE__,__LINE__);
}

/*
 * Grab an item from the pool.
 */
void *
#ifdef POOL_DIAGNOSTIC
_pool_get(struct pool *pp, int flags, const char *file, long line)
#else
pool_get(struct pool *pp, int flags)
#endif
{
    void *obj;
    if(rte_mempool_get(pp->mempool,&obj)) {
        return NULL;
    }
    return obj;
}
void *pool_cache_get(pool_cache_t pool_cache, int dummy)
{
    void *obj;
    if(rte_mempool_get(pool_cache,&obj)) {
        return NULL;
    }
    return obj;
}
void pool_cache_put(pool_cache_t pool_cache, void *obj)
{
    rte_mempool_put(pool_cache,obj);
}
void
pool_put(struct pool *pp, void *v)
{
    rte_mempool_put(pp->mempool,v);
}

void
pool_setlowat(struct pool *pp, int n)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
}

void
pool_sethiwat(struct pool *pp, int n)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
}

void
pool_sethardlimit(struct pool *pp, int n, const char *warnmess, int ratecap)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
}

/*
 * Release all complete pages that have not been used recently.
 *
 * Might be called from interrupt context.
 */
int
#ifdef POOL_DIAGNOSTIC
_pool_reclaim(struct pool *pp, const char *file, long line)
#else
pool_reclaim(struct pool *pp)
#endif
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
    return 0;
}

/*
 * Drain pools, one at a time.  This is a two stage process;
 * drain_start kicks off a cross call to drain CPU-level caches
 * if the pool has an associated pool_cache.  drain_end waits
 * for those cross calls to finish, and then drains the cache
 * (if any) and pool.
 *
 * Note, must never be called from interrupt context.
 */
void
pool_drain_start(struct pool **ppp, uint64_t *wp)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
}

bool
pool_drain_end(struct pool *pp, uint64_t where)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
    return 0;
}

/*
 * Diagnostic helpers.
 */
void
pool_print(struct pool *pp, const char *modif)
{
   rte_mempool_dump(stdout,pp->mempool);
}

void
pool_printall(const char *modif, void (*pr)(const char *, ...))
{
    rte_mempool_list_dump(stdout);
}

void
pool_printit(struct pool *pp, const char *modif, void (*pr)(const char *, ...))
{
    rte_mempool_dump(stdout,pp->mempool);
}

int
pool_chk(struct pool *pp, const char *label)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
    return 0;
}

/*
 * pool_cache_init:
 *
 *	Initialize a pool cache.
 */
pool_cache_t
pool_cache_init(size_t size, u_int count, u_int align_offset, u_int flags,
    const char *wchan, void *dummy, int ipl,
    int (*ctor)(void *, void *, int), void (*dtor)(void *, void *), void *arg)
{
    struct rte_mempool *mempool = rte_mempool_create(wchan, count, size /*elt_size*/,
                   0 /*cache_size*/, 0 /*private_data_size*/,
                   NULL/*rte_mempool_ctor_t **/, NULL/*void **/,
                   ctor, arg,
                   rte_socket_id(), ipl);
    return mempool;
}

/*
 * pool_cache_destroy:
 *
 *	Destroy a pool cache.
 */
void
pool_cache_destroy(pool_cache_t pc)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
}

/*
 * pool_cache_bootstrap_destroy:
 *
 *	Destroy a pool cache.
 */
void
pool_cache_bootstrap_destroy(pool_cache_t pc)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
}

/*
 * pool_cache_reclaim:
 *
 *	Reclaim memory from a pool cache.
 */
bool
pool_cache_reclaim(pool_cache_t pc)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
    return 0;
}

/*
 *	Force destruction of an object and its release back into
 *	the pool.
 */
void
pool_cache_destruct_object(pool_cache_t pc, void *object)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
}

/*
 * pool_cache_invalidate:
 *
 *	Invalidate a pool cache (destruct and release all of the
 *	cached objects).  Does not reclaim objects from the pool.
 *
 *	Note: For pool caches that provide constructed objects, there
 *	is an assumption that another level of synchronization is occurring
 *	between the input to the constructor and the cache invalidation.
 */
void
pool_cache_invalidate(pool_cache_t pc)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
}

void
pool_cache_set_drain_hook(pool_cache_t pc, void (*fn)(void *, int), void *arg)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
}

void
pool_cache_setlowat(pool_cache_t pc, int n)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
}

void
pool_cache_sethiwat(pool_cache_t pc, int n)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
}

void
pool_cache_sethardlimit(pool_cache_t pc, int n, const char *warnmess, int ratecap)
{
    printf("NOT IMPLEMENTED %s %d\n",__FILE__,__LINE__);
}
