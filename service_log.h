
#ifndef __SERVICE_LOG_H__
#define __SERVICE_LOG_H__

enum
{
	SERVICE_LOG_DEBUG,
	SERVICE_LOG_INFO,
	SERVICE_LOG_WARNING,
	SERVICE_LOG_ERR,
	SERVICE_LOG_CRIT,
	SERVICE_LOG_NONE
};

void service_log_init(int);
void service_log(int, const char *, ...);
void service_set_log_level(int);

#endif
