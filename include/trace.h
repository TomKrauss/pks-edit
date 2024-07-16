//
// TRACE.H
//
#ifndef TRACE_H

#define EdASSERT(expr)		log_vsprintf("Assertion failed |%s|file %s, line %d",\
                 				#expr, __FILE__, __LINE__ );
#define EdTRACE(x)			x

typedef enum enumLOG_LEVEL {
    // Use this as a log level to ensure, everything is logged.
    DEBUG_ALL   = 0x0,
    DEBUG_TRACE = 0x1,
    DEBUG_INFO  = 0x2,
    DEBUG_WARN	= 0x4,
    DEBUG_ERR   = 0x8
} LOG_LEVEL;

extern void log_message(LOG_LEVEL logLevel, char *fmt, ...);
extern void log_vsprintf(char *fmt, ...);

/*
 * Can be used to configure the PKS-Edit logging level (see levels defined above).
 */
extern void log_setLogLevel(LOG_LEVEL logLevel);

/*
 * In case a system function failed, use GetLastError to log the failure.
 */
extern void log_lastWindowsError(const char* lpszFunction);

#define TRACE_H
#endif
