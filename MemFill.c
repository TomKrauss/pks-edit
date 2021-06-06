/*
 *
 * MemFill.c
 *
 * tiny memset version
 *
 * PROJECT: PKSEDIT/Command Editor
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 29.12.1990
 *									
 */

#include <stdio.h>
#include <string.h>
#include "lineoperations.h"

/*--------------------------------------------
* blfill(char *buf,int count,int fillbyte)
* Similar to memset, but return pointer to the end of 
* the filled area.
*--------------------------------------------*/
EXPORT unsigned char *blfill(void *buf, int count, unsigned char fillbyte) {	
	memset(buf, (unsigned char)fillbyte, count);
	return (unsigned char*)buf + count;
}


