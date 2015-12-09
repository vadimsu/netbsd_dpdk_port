
//#include <syslog.h>
#include <stdio.h>
#include <stdarg.h>
#include <service_log.h>

static int g_service_log_dest = 0; /* 0 - stdio, 1 - syslog */
static int g_service_log_level = SERVICE_LOG_NONE;

void service_log_init(int dest)
{
	g_service_log_dest = dest;
	switch(g_service_log_dest) {
		case 0:
		break;
		case 1:
/*			openlog(NULL, 0, LOG_DAEMON)*/;
		break;
	}
}

void service_set_log_level(int log_level)
{
	g_service_log_level = log_level;
}

void service_log(int level, const char* format, ...)
{
	va_list argptr;

	if (level < g_service_log_level)
		return;

	switch(g_service_log_dest) {
		case 0:			
    			va_start(argptr, format);
			vfprintf(stdout, format, argptr);
    			va_end(argptr);
		break;
#if 0
		case 1:
			switch(level) {
				case SERVICE_LOG_DEBUG:
					syslog(LOG_DEBUG, format, argptr);
				break;
				case SERVICE_LOG_INFO:
					syslog(LOG_INFO, format, argptr);
				break;
				case SERVICE_LOG_WARNING:
					syslog(LOG_WARNING, format, argptr);
				case SERVICE_LOG_ERR:
					syslog(LOG_ERR,format, argptr);
				break;
				case SERVICE_LOG_CRIT:
					syslog(LOG_CRIT,format, argptr);
				break;
			}
		break;
#endif
	}
}
