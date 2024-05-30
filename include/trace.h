//
// TRACE.H
//
#ifndef TRACE_H

#define EdASSERT(expr)		log_vsprintf("Assertion failed |%s|file %s, line %d",\
                 				#expr, __FILE__, __LINE__ );
#define EdTRACE(x)			x
#define log_debug1(expr, arg)  log_errorArgs(DEBUG_ALL, expr, arg)

#define DEBUG_FUNCS			0x1
#define DEBUG_ASSERT		0x2
#define DEBUG_ERR			0x4
#define DEBUG_TRACE			0x8
#define DEBUG_WARN			0x40
#define DEBUG_INFO			0x80
#define DEBUG_WINMESS		0x10
#define DEBUG_MALLOC		0x20
#define DEBUG_ALL			0xFF

extern void log_errorArgs(int dbg_mask, char *fmt, ...);
extern void log_vsprintf(char *fmt, ...);

/*
 * Can be used to configure the PKS-Edit logging level (see levels defined above).
 */
extern void log_setLogLevel(int logLevel);

/*
 * In case a system function failed, use GetLastError to log the failure.
 */
extern void log_lastWindowsError(const char* lpszFunction);

#define TRACE_H
#endif
