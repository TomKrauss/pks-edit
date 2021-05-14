
#ifndef EDSTD_H

#ifndef DIM
#define	DIM(x)	(sizeof(x)/sizeof(x[0]))
#endif

#undef offsetof
#define offsetof(typ,offs)	(size_t)&(((typ *)0)->offs)

#define EDSTD_H
#endif
