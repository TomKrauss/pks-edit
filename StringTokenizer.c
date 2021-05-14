/*
 * StringTokenizer.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: standard strtok substitute
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

char *strtok(char *s,char *set)
{	static char *laststart;
	char c,*cmp,*start;

	if (s == 0L) s = laststart;
	start = s;
	if (s != 0L) {
		while((c = *s++) != 0) {
			cmp = (char *) set;
			while(*cmp) {
				if (*cmp == c) {
					s[-1] 	= 0;
					laststart = s;
					goto out;
				}
				cmp++;
			}
		}
		laststart = 0L;
	}
out:
	return start;
}

