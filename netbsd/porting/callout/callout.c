//#include <special_includes/sys/cdefs.h>

//#include <special_includes/sys/param.h>
#include <rte_common.h>
#include <rte_timer.h>
#include <rte_lcore.h>
#include <rte_cycles.h>
#include <../../../special_includes/sys/callout.h>

typedef unsigned char bool;
extern int hz;

typedef struct callout_impl {
	struct rte_timer timer;
	void	(*c_func)(void *);		/* function to call */
	void	*c_arg;				/* function argument */
	int c_cpu;	/* associated CPU */
	int	c_time;				/* when callout fires */
	u_int	c_flags;			/* state of this entry */
	u_int	c_magic;			/* magic number */
} callout_impl_t;

static void wrapper_func(struct rte_timer *tim, void *arg)
{
	callout_impl_t *impl = (callout_impl_t *)arg;

	if ((!impl) || (!impl->c_func))
		return;
	impl->c_flags = (impl->c_flags & ~CALLOUT_PENDING) | (CALLOUT_FIRED | CALLOUT_INVOKING);
	impl->c_func(impl->c_arg);
}

void	callout_startup(void)
{
	/* init RTE timer library */
	rte_timer_subsystem_init();
}

void	callout_hardclock(void)
{
	rte_timer_manage();
}

void	callout_init(callout_t *co, u_int duration)
{
	callout_impl_t *impl = (callout_impl_t *)co;

	impl->c_arg = NULL;
	impl->c_time = 0;
	impl->c_func = NULL;
	impl->c_flags = 0;
	rte_timer_init(&impl->timer);
}
void	callout_destroy(callout_t *co)
{
}
void	callout_setfunc(callout_t *co, void (*cbk)(void *), void *arg)
{
	callout_impl_t *impl = (callout_impl_t *)co;

	impl->c_arg = arg;
	impl->c_func = cbk;
}
void	callout_reset(callout_t *co, int duration, void (*cbk)(void *), void *arg)
{
	callout_impl_t *impl = (callout_impl_t *)co;

	impl->c_arg = arg;
	impl->c_func = cbk;
	impl->c_flags &= ~(CALLOUT_FIRED | CALLOUT_INVOKING);
	impl->c_flags |= CALLOUT_PENDING;
	rte_timer_reset(&impl->timer, (rte_get_timer_hz()/hz) * duration, SINGLE, rte_lcore_id(), wrapper_func, impl);
}
void	callout_schedule(callout_t *co, int duration)
{
	callout_impl_t *impl = (callout_impl_t *)co;

	impl->c_flags &= ~(CALLOUT_FIRED | CALLOUT_INVOKING);
	impl->c_flags |= CALLOUT_PENDING;
	rte_timer_reset(&impl->timer, (rte_get_timer_hz()/hz) * duration, SINGLE, rte_lcore_id(), wrapper_func, impl);
}
bool	callout_stop(callout_t *co)
{
	callout_impl_t *impl = (callout_impl_t *)co;

	impl->c_flags &= ~(CALLOUT_PENDING|CALLOUT_FIRED);
    return (rte_timer_stop(&impl->timer) == 0);
}
bool	callout_halt(callout_t *co)
{
	callout_impl_t *impl = (callout_impl_t *)co;

	impl->c_flags &= ~(CALLOUT_PENDING|CALLOUT_FIRED);
    return (rte_timer_stop(&impl->timer) == 0);
}
bool	callout_pending(callout_t *co)
{
	callout_impl_t *impl = (callout_impl_t *)co;

    return ((impl->c_flags & CALLOUT_PENDING) != 0);
}
bool	callout_expired(callout_t *co)
{
	callout_impl_t *impl = (callout_impl_t *)co;

    return ((impl->c_flags & CALLOUT_FIRED) != 0);
}
bool	callout_active(callout_t *co)
{
	callout_impl_t *impl = (callout_impl_t *)co;

    return ((impl->c_flags & (CALLOUT_PENDING|CALLOUT_FIRED)) != 0);
}
bool	callout_invoking(callout_t *co)
{
	callout_impl_t *impl = (callout_impl_t *)co;

    return ((impl->c_flags & CALLOUT_INVOKING) != 0);
}
void	callout_ack(callout_t *co)
{
	callout_impl_t *impl = (callout_impl_t *)co;

	impl->c_flags &= ~CALLOUT_INVOKING;
}
void	callout_bind(callout_t *co, void *dummy)
{
}

