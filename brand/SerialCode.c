
/* #include <ctype.h> */

char *_cryptend;

void c2asc(char *start, char *source, unsigned char hi, unsigned char lo)
{
	while(!isalnum(hi))
		hi += 51;
	*_cryptend++ = hi;
	if ((lo & 1) || source < start + 2) {
		while(!isalnum(lo))
			lo += 51;
		*_cryptend++ = lo;
	}
	_cryptend[1] = 0;
}

static int magics[] = {	
	23,47,191,17,39,97,88,31,5,201,29,
	23,47,191,17,39,97,88,31,5,201,29,
	23,47,191,17,39,97,88,31,5,201,29,
	23,47,191,17,39,97,88,31,5,201,29,
};

/*---------------------------------*/
/* cryptXXXX()					*/
/*---------------------------------*/
char *cryptXXXX(char *dest, char *source,
	 void (*f)(char *start, char *s, unsigned char hi, unsigned char lo))
{	long x;
	int  *p;
	unsigned char hi,lo;
	char *s,*start = source;
	char *end      = source;

	while(end[1])
		end++;
	x 		 = 0;
	_cryptend = dest;

	while(*source) {
		p = magics;
		s = source + 1;
		while(1) {
			if (s > end)
				s = start;
			x += ((long)*p * (long)*s);
			x += *++p;
			if (s == source)
				break;
			s++;
		}
		source++;
		hi = (unsigned char)(short)(x >> 8);
		lo = (unsigned char)(short)x;
		if (f) 
			(*f)(start,source,hi,lo);
		else {
			*dest++ = hi;
			*dest++ = lo;
		}
		x >>= 8;
	}
	if (!f)
		_cryptend = dest;
	*_cryptend = 0;
	return _cryptend;
}


