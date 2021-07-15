
#ifndef EDTYPES_H

#define EDNEAR		/**/

#define EDFAR
#define MoveTo(hdc,x,y)		MoveToEx(hdc, x, y, 0)

#define EXPORT		/**/

typedef unsigned long 		MEMHANDLE;
typedef MEMHANDLE EDFAR *	PMEMHANDLE;
typedef void   EDFAR  	*	FARP;
typedef void   EDNEAR	*	NEARP;
typedef	int					CTXID;
typedef int					BOOL;

#define EDTYPES_H
#endif

