//
// TRACE.H
//
#ifndef TRACE_H

#ifdef DEBUG
#define EdASSERT(expr)		edidebug("Assertion failed |%s|file %s, line %d",\
                 				#expr, __FILE__, __LINE__ );
#define EdTRACE(x)			x
#else
#define EdTRACE(x)			/* x */
#define EdASSERT(expr)		/* ... */
#endif

#define DEBUG_FUNCS			0x1
#define DEBUG_ASSERT		0x2
#define DEBUG_ERR			0x4
#define DEBUG_TRACE			0x8
#define DEBUG_WINMESS		0x10
#define DEBUG_MALLOC		0x20
#define DEBUG_ALL			0xFF

void Debug(int dbg_mask, char *fmt, ...);
void edidebug(char *fmt, ...);

#define TRACE_H
#endif
