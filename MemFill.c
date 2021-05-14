/*
 *
 * MemFill.c
 *
 * tiny memset version
 *
 * PROJECT: PKSEDIT/Command Editor
 *
 * (c) Pahlen & Krauß					Author : TOM
 * All Rights Reserved.					created: 29.12.1990
 *									
 */

#include "lineoperations.h"

#ifdef MC68000

	.globl	_fill
	.globl	blfill

*--------------------------------------------
* blfill(char *buf,int count,int fillbyte)
*--------------------------------------------
blfill:
	SUBQ 	#1,D0
	BLT		eblfill
bloop:
	MOVE.B	D1,(A0)+
	DBF		D0,bloop
eblfill:
	RTS


*--------------------------------------------
* _fill(char *buf,int count) (with ' ')
*--------------------------------------------
_fill:
	LSR		#4,D0
	MOVE.L	#$20202020,D1
loop:
	MOVE.L	D1,(A0)+
	MOVE.L	D1,(A0)+
	MOVE.L	D1,(A0)+
	MOVE.L	D1,(A0)+
	DBF		D0,loop
	RTS

#else
#if 0
EXPORT void *memmove(void *D, const void *S, size_t Len)
{
#if 0
	char *d = D,	
	 	*s = (char *)S;
	register int len = Len;
	 	
	if (P_GT(d,s)) {
		void *ret;

		d += len;
		s += len;
		ret = (void*)d;
		while(--len >= 0)
			*--d = *--s;

		return ret;
	} else {
		while(len-- > 0)
			*d++ = *s++;
		return (void*)d;
	}
#else
	memmove(D,S,Len);
	return ((char*)S+Len);
#endif
}
#endif

/*--------------------------------------------
* blfill(char *buf,int count,int fillbyte)
* Similar to memset, but return pointer to the end of 
* the filled area.
*--------------------------------------------*/
EXPORT unsigned char *blfill(void *buf, int count, unsigned char fillbyte) {	
	memset(buf, (unsigned char)fillbyte, count);
	return (unsigned char*)buf + count;
}
#endif


