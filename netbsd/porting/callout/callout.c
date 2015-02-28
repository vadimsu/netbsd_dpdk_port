#include <sys/cdefs.h>

#include <sys/param.h>
#include<sys/callout.h>


void	callout_startup(void)
{
}
void	callout_init_cpu(struct cpu_info *ci)
{
}
void	callout_hardclock(void)
{
}

void	callout_init(callout_t *co, u_int duration)
{
}
void	callout_destroy(callout_t *co)
{
}
void	callout_setfunc(callout_t *co, void (*cbk)(void *), void *arg)
{
}
void	callout_reset(callout_t *co, int duration, void (*cbk)(void *), void *arg)
{
}
void	callout_schedule(callout_t *co, int duration)
{
}
bool	callout_stop(callout_t *co)
{
    return FALSE;
}
bool	callout_halt(callout_t *co)
{
    return FALSE;
}
bool	callout_pending(callout_t *co)
{
    return FALSE;
}
bool	callout_expired(callout_t *co)
{
    return FALSE;
}
bool	callout_active(callout_t *co)
{
    return FALSE;
}
bool	callout_invoking(callout_t *co)
{
    return FALSE;
}
void	callout_ack(callout_t *co)
{
}
void	callout_bind(callout_t *co, struct cpu_info *ci)
{
}

