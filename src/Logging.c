/*
 * Project: PKS Edit for Windows
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
#include <stdio.h>
#include <time.h>
#include "winterf.h"
#include "fileutil.h"
#include "stringutil.h"
#include "trace.h"
#include "documentmodel.h"

static LOG_LEVEL _logLevel =
#ifdef _DEBUG
DEBUG_TRACE;
#else
DEBUG_WARN;
#endif

static FILE* _pksEditLogfile;

/*-----------------------------------------------------------
 * log_levelMessageArgs()
 */
static void log_levelMessageArgs(LOG_LEVEL logLevel, LPSTR fmt, va_list ap) {
	char buf[1024];
	char bufTotal[1024];
	char szTime[64];
	char* pszLevel = "TRACE";
	if (logLevel == DEBUG_WARN) {
		pszLevel = "WARN";
	} else if (logLevel == DEBUG_INFO) {
		pszLevel = "INFO";
	} else if (logLevel == DEBUG_ERR) {
		pszLevel = "ERROR";
	}
    vsprintf_s((LPSTR)buf, sizeof buf, (LPSTR)fmt,(LPSTR)ap);
	time_t rawtime;
	rawtime = time(NULL);
	struct tm* tm_info;
	tm_info = localtime(&rawtime);
	strftime(szTime, 26, "%Y-%m-%d %H:%M:%S", tm_info);
	sprintf_s(bufTotal, sizeof bufTotal, "%s:%s:%s:%s\n", szAppName, pszLevel, szTime, buf);
	if (_pksEditLogfile == NULL) {
		char szLogFilename[MAX_PATH];
		szLogFilename[0] = 0;
		string_concatPathAndFilename(szLogFilename, _pksSysFolder, "pksedit.log");
		_pksEditLogfile = fopen(szLogFilename, "w");
	}
	if (_pksEditLogfile) {
		fwrite(bufTotal, sizeof(bufTotal[0]), strlen(bufTotal), _pksEditLogfile);
		fflush(_pksEditLogfile);
	}
	OutputDebugString(bufTotal);
}

/*
 * Can be used to put PKS-Edit in verbose mode, having it printing all debug messages to stderr.
 */
EXPORT void log_setLogLevel(LOG_LEVEL logLevel) {
	_logLevel = logLevel;
}

/*-----------------------------------------------------------
 * log_message()
 */
EXPORT void log_message(LOG_LEVEL logLevel, char *fmt, ...)
{
	va_list ap;
	if (logLevel < _logLevel) {
		return;
	}
	va_start(ap,fmt);
	log_levelMessageArgs(logLevel,fmt,ap);
	va_end(ap);
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
	log_message(DEBUG_ERR, "%s failed with error %d: %s", lpszFunction, dw, lpMsgBuf);
	LocalFree(lpMsgBuf);
}

/*-----------------------------------------------------------
 * log_vsprintf()
 */
EXPORT void log_vsprintf(char *fmt, ...)
{
	va_list ap;
	va_start(ap,fmt);
	log_levelMessageArgs(0,fmt,ap);	
	va_end(ap);
}


