//
//
// FIO.H
//
// reading + writing files
//
//
#ifndef FIO_H

#define FIO_INVAL	0xFFFF

typedef unsigned short EDFHANDLE;

EDFHANDLE 	EdFopen(char *fname);
unsigned char 	*EdGetS(EDFHANDLE hFile, unsigned char term, long *len);
int 		EdFsetBufSize(EDFHANDLE efd, unsigned size);
void 		EdFclose(EDFHANDLE hFile);

#define	FIO_H

#endif
