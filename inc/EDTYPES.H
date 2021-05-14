
#ifndef EDTYPES_H

#define EDNEAR		/**/

#if defined(WIN32)
#define EDFAR
#define _alloc(x)			malloc(x)
#define _free(x)			free(x)
#define MoveTo(hdc,x,y)		MoveToEx(hdc, x, y, 0)
#else
#define CALLBACK			FAR PASCAL _export
#define EDFAR				far
extern void *				_alloc(unsigned size);
extern void 				_free(void *ap);
#endif

typedef unsigned long 		MEMHANDLE;
typedef MEMHANDLE EDFAR *	PMEMHANDLE;
typedef void   EDFAR  	*	FARP;
typedef void   EDNEAR	*	NEARP;
typedef	int					CTXID;

#define EDTYPES_H
#endif

