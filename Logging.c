/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * Logging.c
 *
 * log trace messages to a file
 *
 * 						created: 14.07.91 
 *						Author : TOM
 */

#include <windows.h>
# pragma hdrstop
#include <stdarg.h>
#include "winterf.h"
#include "trace.h"
#include "documentmodel.h"

static int _debugmask = DEBUG_ALL;
static int _debugfd = -1;

/*-----------------------------------------------------------
 * vdebug()
 */
static void vdebug(int err, LPSTR fmt, va_list ap)
{
	char buf[1024];
    wvsprintf((LPSTR)buf,(LPSTR)fmt,(LPSTR)ap);
	strcat(buf, "\r\n");
	OutputDebugString(buf);
}

/*-----------------------------------------------------------
 * log_errorArgs()
 */
EXPORT void log_errorArgs(int dbgmask, char *fmt, ...)
{
#ifdef _DEBUG
	va_list ap;
	if ((_debugmask & dbgmask) == 0)
		return;

	va_start(ap,fmt);
	vdebug(dbgmask&DEBUG_ERR,fmt,ap);	
	va_end(ap);
#endif
}

/*
 * In case a system function failed, use GetLastError to log the failure.
 */
void log_lastWindowsError(const char* lpszFunction) {
	LPVOID lpMsgBuf = 0;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);
	log_errorArgs(DEBUG_ERR, "%s failed with error %d: %s", lpszFunction, dw, lpMsgBuf);
	LocalFree(lpMsgBuf);
}

/*-----------------------------------------------------------
 * log_vsprintf()
 */
EXPORT void log_vsprintf(char *fmt, ...)
{
	va_list ap;
	va_start(ap,fmt);
	vdebug(0,fmt,ap);	
	va_end(ap);
}


