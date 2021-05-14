/*
 * RegularExpression.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: regular expression string scanning automata
 * 		  character class definitions
 *
 * 										created      : 04.02.97
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include	<stdio.h>
#include	<tos.h>
#include	<dos.h>
#include	"regexp.h"
#include	"pksedit.h"
#include	"edctype.h"

#if defined(WIN32)
#define near
#endif

/*
	-	all Flags with Bit 0 set arent to be combined with range
	-	only on Flags with (flags & 0x3 == 0) garant a length of the
		resulting match
	-	CCEOF and CDOL must be last (2nd last) Flags in expression
*/

#define	RANGEINVALID		1
#define	LENGTHGARANT(c)	(! (c & 3))

#define	RNGE				(0 | RANGEINVALID)
#define	CBRA				(2 | RANGEINVALID)
#define	CCHR				4
#define	CDOT				8
#define	CKET				10
#define	CIDS				(12 | RANGEINVALID)
#define	CCL				16
#define	CIDE				(24 | RANGEINVALID)
#define	CDOL				26
#define	CCEOF			30
#define	CCASE			64
#define	CBACK			66
#define	CPBAR			128
#define	CPIPE			(CPBAR | RANGEINVALID)

#define	NBRA				9		/* maximum number of brackets	*/
#define	ISTHERE(c)		(ep[(c & 0xff) >> 3] & bittab[c & 07])
#define	MAXCTAB			32		/* maximum byte length of chartestset */
#define	BIG				512		/* big Range Size used by: +,*,{x,} */

#define	upcase(c)			_l2uset[(c)]
#define	lowcase(c)		_u2lset[(c)]

#define P_LE(p1,p2)			((char near *)p1 <= (char near *)p2)
#define P_GE(p1,p2)			((char near *)p1 >= (char near *)p2)
#define P_LT(p1,p2)			((char near *)p1 <  (char near *)p2)
#define P_GT(p1,p2)			((char near *)p1 >  (char near *)p2)

/***************************************************************************/
/* ERRORS:													*/
/*															*/
/* 1 	= 0-expression encountered								*/
/* 2 	= expressionbuffer overflow								*/
/* 3 	= illegal makro/range combination							*/
/* 4 	= wrong characterclass-definition							*/
/* 5 	= too many Subexpressions								*/
/* 6 	= subexpression-brackets not in balance 					*/
/* 7 	= wrong {..,..} - Syntax 								*/
/* 8 	= { - Number too big									*/
/* 9 	= {i,j}: i must be less than j							*/
/* 10	= \ at end of the line encountered 						*/
/* 11	= Subexpression # unknown								*/
/* 12	= Pipe outside () - Pairs								*/
/*															*/
/***************************************************************************/

/*--------------------------------------------------------------------------
 * USER-DEFINES
 */

#define	INIT		register unsigned char *sp = instring; int err;
#define	GETC() 	(*sp++)
#define	PEEKC()	(*sp)
#define	UNGETC(c)	(--sp)
#define	RETURN(c)	return(c)
#define	REGEX_ERROR(c)	{ err = c; goto endcompile; }

static int 	_chsetinited;
unsigned char 	bittab[] = { 1,2,4,8,16,32,64,128 };
int			__size;
int 			_staronly; 	/* expression is only stared expression ? */

static int near advance(unsigned char *lp, unsigned char *ep, unsigned char *end);

/*----------------- EXPORT ---------------------*/

int		    _circf,_cnostart,_regerror,_regepos,_regspos;
char 		*__loc1,*__loc2,*__locs;
int		    _nbrackets;

unsigned char _asciitab[256] = 
	"\0\0\0\0\0\0\0\0\0\020\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\020\0\0\0\0\0\0\0\0\0\040\0\0\0\040\0\051\051"
	"\051\051\051\051\051\051\051\051\040\0\0\0\0\0\0\054\054"
	"\054\054\054\054\054\054\054\054\054\054\054\054\054\054"
	"\054\054\054\054\054\054\054\054\054\054\0\040\0\0\050\0"
	"\052\052\052\052\052\052\052\052\052\052\052\052\052\052"
	"\052\052\052\052\052\052\052\052\052\052\052\052\0\0\0\0"
	"\0\0Ä\0\0Ä\0\0\0\0\0\0\0\0\0@\0\0\0\0\0Ä\0\0\0\0@@\0\0\0"
	"Ä\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\040\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0";

/* don't change order */
char 		*_braslist[NBRA];
char 		*_braelist[NBRA];

unsigned char 	_l2uset[256],
			_u2lset[256];

/*--------------------------------------------------------------------------
 * fixsets()
 */
void fixsets(unsigned char *l2upat)
{	int i,j;

	_chsetinited = 1;

	if (!l2upat)
		l2upat = "a-z=A-Z";

	for (i = 0; i < 256; i++)
		_asciitab[i] &= (~_C);

	tlcompile(_l2uset,l2upat,_asciitab);

	/* calculate the inverse tab */

	blfill(_u2lset,sizeof(_u2lset),0);

	for (i = 0; i < sizeof(_l2uset); i++)
		if ((j = _l2uset[i]) != i)
			_u2lset[j] = i;
	for (i = 0; i < sizeof(_u2lset); i++)
		if (_u2lset[i] == 0)
			_u2lset[i] = i;
}

/*--------------------------------------------------------------------------
 * getrnge()
 * returns minimum character range
 * count in D0
 * maximum less minimum count
 * -> var size
 */
unsigned getrnge(char *padd, unsigned char *s)
{	unsigned i;

	if ((i = s[1]) == 255) {
		__size = BIG;
	} else {
		__size = i;
	}
#if defined(WIN32)
	return *s;
#else
	{
	unsigned max;
	max = ((unsigned)-1)-FP_OFF(padd)-1;
	if (__size > max)
		__size = max;
	return (*s > max) ? max : *s;
	}
#endif
}

/*--------------------------------------------------------------------------
 * place()
 */
static unsigned char *_placeloc;
static int		  _compileflags;
static void place(register unsigned char c)
{	register unsigned char *l;
	int c2;

	l = _placeloc;
	if (_compileflags & RE_IGNCASE) {
		c2 = upcase(c);
		l[c2 >> 3] |= bittab[c2 & 07];
		c  = lowcase(c);
	}
	l[c >> 3] |= bittab[c & 07];
}

/*--------------------------------------------------------------------------
 * octalinput()
 */
unsigned char *_octalloc;
int octalinput(register unsigned char *s)
{	register unsigned char c,i;

	switch (c = *s++) {
	  case 'n':	i = '\n';	break;
	  case 't':	i = '\t';	break;
	  case 'r':	i = '\r'; break;
	  case '0':  	i = 0;
	  	/* maximum 3 digits \0ddd */
	  	if ((c = *s++) <= '7' && c >= '0') {
			i <<= 3; i += (c-'0');
		  	if ((c = *s++) <= '7' && c >= '0') {
				i <<= 3; i += (c-'0');
			  	if ((c = *s++) <= '7' && c >= '0') {
					s++;	i <<= 3; i += (c-'0');
				}
			}
		}
		s--;
		break;
	  default:	i = c;
	}
	_octalloc = s;
	return i;
}

/*--------------------------------------------------------------------------
 * setpipe()
 */
static unsigned char *pipes[NBRA];
static void setpipe(register int i,unsigned char *ep)
{	register unsigned char *pip;

	pip 		= pipes[i];
	pip[0]   |= CPIPE;
	pip[1]	= i;				/* () - level #				*/
	pip[2]	= (char )(ep-pip);	/* current ep - last cpipe (br) ep */
	pipes[i]	= ep;
}

/*--------------------------------------------------------------------------
 * precompile()
 * precompile shell meta format to a regular expression
 * wildcards are: *, ?
 */
static void precompile(char *dest, char *src)
{
	while(*src) {
		switch(*src) {
			case '?': *dest++ = '.'; break;
			case '*': *dest++ = '.', *dest++ = '*'; break;
			case '.': case '[': case ']': case '$': case '^':
			case '{': case '}': case '(': case ')':
			case '+': case '<': case '>': case '|':
				*dest++ = '\\', *dest++ = *src; break;
			case '\\':
				if (src[1] > '0' && src[1] <='9' || src[1] == 'T') {
					src++;
				} else {
					*dest++ = *src++;
					if (!*src)
						goto out;
				}
				/* drop through */
			default:
				*dest++ = *src;
		}
		src++;
	}
out: *dest = 0;
}

/*--------------------------------------------------------------------------
 * compile()
 * compile an expression for fast
 * scan - Algorithm
 */
unsigned char *compile(unsigned char *instring, 
				   unsigned char *ep, 
				   unsigned char *endbuf,
				   unsigned char eof,
				   int flags)
{	INIT /* Dependent declarations and initializations */
	register unsigned char c;
	int	    cflg;
	register unsigned char *lastep;
	unsigned char bracket[NBRA], *bracketp, closed[NBRA];
	char     shellbuf[512];
	int	    f1,neg;
	unsigned char *firstep,*f2,*Cep;
	register int i;

	if (!_chsetinited)
		fixsets((unsigned char *) 0);
		
	if (flags & RE_SHELLWILD) {
		precompile(shellbuf,sp);
		sp = shellbuf;
	}

	Cep = sp;
	if ((c = GETC()) == eof) {
		if (ep[1] == 0) REGEX_ERROR(1);
		RETURN(ep);
	}

	bracketp = bracket;
	lastep = 0;
	blfill(closed,(int)sizeof(closed),0);
	_nbrackets   = 0;
	firstep = ep;
	
	/* Space for: _circf, fastchar, .* - marker */
	ep  += 3;

	f1   = 1, f2 = 0;
	_staronly = 1;
	_compileflags = flags;
	if (flags & (RE_DOREX|RE_SHELLWILD)) {
		if (c == '^') {
			*firstep = 1;			/* _circf */
			goto nounget;
		}
	}
	*firstep = 0;					/* ! _circf */
	UNGETC(c);
nounget:
	_circf = *firstep++;
	for(;;) {
		Cep = sp;
		if(ep >= endbuf) REGEX_ERROR(2);
		if((c = GETC()) == eof) {
			if (bracketp != bracket)
				REGEX_ERROR(6); 				/* brackets open	  */
			if (f2 && (*f2 == CCHR || f2[2]))
				*firstep = f2[1];
			else *firstep = 0;
		/* optimizing expressions wich force starvation 
		   in the form ".* ...",	
		 */
			if (firstep[2] == (CDOT|RNGE) && firstep[3] && firstep[4] == 255) {
				firstep[1] = 255;
			}
			else firstep[1] = 0;
			_regepos	= sp -instring;
			*ep++ = CCEOF;
			RETURN(ep);
		}
		if (!(flags & (RE_DOREX|RE_SHELLWILD))) goto defchar;
		if (c != '*' && c != '{' && c != '+' && c != '?') {
			if (lastep && LENGTHGARANT(*lastep))
				_staronly = 0;
			if (c != '|') lastep = ep;
		} else {
			if (lastep == 0L || (*lastep & RANGEINVALID) != 0)
				REGEX_ERROR(3);
			*lastep |= RNGE;
		}
		switch(c) {

		case '.':
			f1 = 0;
			*ep++ = CDOT;
			break;

		case '$':
			if(PEEKC() != eof) goto defchar;
			*ep++ = CDOL;
			break;

		case '<':
			*ep++ = CIDS;
			break;

		case '>':
			*ep++ = CIDE;
			break;

		case '*':
			*ep++ = 0;
			*ep++ = 255;
			break;

		case '+':
			*ep++ = 1;
			*ep++ = 255;
			break;

		case '?':
			*ep++ = 0;
			*ep++ = 1;
			break;

		case '{':
			cflg = 0;
			c = GETC();
getcnt:
			i = 0;
			do {
				if(c >= '0' && c <= '9')
					i = 10 * i + c - '0';
				else REGEX_ERROR(7);
			} while((c = GETC()) != ',' && c != '}');
			if(i > 255) REGEX_ERROR(8);
			*ep++ = i;
			if(c == ',') {
				if (cflg++) REGEX_ERROR(7);
				if((c = GETC()) == '}') {
					*ep++ = 255;		/* open end -> size = 255 */
					break;
				} else goto getcnt;
			}
			if(!cflg) 				/* one number -> size = 0 */
				*ep++ = 0;
			else	{
				if ((i = ep[-1] - ep[-2]) < 0)
					REGEX_ERROR(9);
				ep[-1] = i;
			}
			break;

		case '(':
			if(_nbrackets >= NBRA) REGEX_ERROR(5);
			*bracketp++ = _nbrackets;
			pipes[_nbrackets] = ep;
			*ep++ = CBRA;
			*ep++ = _nbrackets++;
			ep++;				/* space for optional | -> pointer */
			break;

		case ')':
			/* all brackets already closed ?? 	*/
			if (bracketp <= bracket) REGEX_ERROR(6);

			/* avoid 0-Expressions			*/
			if ((i = ep[-3]) == CBRA || i == CPIPE)
				REGEX_ERROR(1);
			i = *(--bracketp);
			if (closed[i]) setpipe(i,ep);
			closed[i] |= 1;
			*ep++ = CKET;
			*ep++ = i;
			break;

		case '|':
		/* - 	On bracket open reserve one more char in ep
		   - 	Insert at last opened bracket (resp on last opened pipe)
				delta to current ep
		   - 	Reserve on current ep place for delta to next opened pipe
				resp to next closed bracket and # of current sub expr
		*/
			f1 = 0;	f2 = 0;		/* disable fastchecks			*/
			if (bracket == bracketp)
				REGEX_ERROR(12);
#if 0
			if ((i = *lastep) == CBRA || i == CPIPE)
				REGEX_ERROR(1);
#endif
			i = _nbrackets-1;
			closed[i] |= 2;		
			setpipe(i,ep);
			lastep = ep;
			*ep 	  = CPIPE;
			ep 	 += 3;
			break;

		case '[':
			if(&ep[MAXCTAB+1] >= endbuf)
				REGEX_ERROR(2);
			*ep++ 	= CCL;
			blfill(ep,(int)MAXCTAB,0);
			cflg  	= -1;
			f1 	 	= 0;
			neg 		= 0;
			_placeloc = ep;
			if((c = GETC()) == '^') {
				neg = 1;
				c = GETC();
			}
			do {
				if (c == '\0') REGEX_ERROR(4);
				if (c == '\\') {
					c  = octalinput(sp);
					sp = _octalloc;
				}
				else if (c == '-' && cflg >= 0) {
					if ((c = GETC()) == ']') {
						place('-');
						break;
					}
					if (c == '\\') {
						c = octalinput(sp);
						sp= _octalloc;
					}
					while(cflg < c ) {
						place(cflg);
						cflg++;
					}
				}
				cflg = c;
				place(c);
			} while((c = GETC()) != ']');
			if (neg) {
				for (i = 0; i < MAXCTAB; i++) ep[i] ^= -1;
				*ep &= 0xFE;
			}
			ep += MAXCTAB;
			break;

		case '\\':
			if ((c = (GETC() & 0xFF)) == eof && !eof) REGEX_ERROR(10);
			if (c >= '1' && c <= '9') {
				c -= '1';
				if(!(closed[c] & 1)) REGEX_ERROR(11);
				*ep++ = CBACK;
				*ep++ = c;
				continue;
			}
			if (c == 'T') {
				c = ' ';
				cflg = '\t';
				goto docase;
			}
			sp--;
			c 	= octalinput(sp);
			sp 	= _octalloc;

	/* Drop through to default to use \ to turn off special chars */

		default:
		defchar:
			/* lastep = ep; */
			if (flags & RE_IGNCASE) {
				c    = _l2uset[c];
				cflg = _u2lset[c];
				if (c == cflg)	/* no alpha char, don't distinguish cases */
					goto nocase;
docase:
				f1 = 0;
				*ep++ = CCASE;
				*ep++ = c;
				*ep++ = cflg;
			} else {
nocase:
				if (f1 && !f2) f2 = ep;
				*ep++ = CCHR;
				*ep++ = c;
			}
		}
	}
endcompile:
	_regerror = err;
	_regspos	= Cep-instring;
	_regepos	= sp -instring-1;
	return 0;
}

/*--------------------------------------------------------------------------
 * advpipe()
 */
static int advpipe(unsigned char *lp, unsigned char *ep, unsigned char *end)
{	char *sbra[2*NBRA];
	unsigned char *l2sav = 0;

	memmove(sbra,_braslist,sizeof sbra);
	while (ep[-3] & CPBAR) {
		if (advance(lp,ep,end)) {
			if (__loc2 > l2sav) {
				l2sav = __loc2;
				memmove(sbra,_braslist,sizeof sbra);
			}
		}
		ep += ep[-1];
	}
	memmove(_braslist,sbra,sizeof sbra);
	if (l2sav) {
		__loc2 = l2sav;
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * ecmp()
 */
int ecmp(unsigned char *s,unsigned char *p,unsigned char *send)
{
	while(s < (unsigned char *)send)
		if (*s++ != *p++)
			return 0;
	return 1;
}

/*--------------------------------------------------------------------------
 * advance()
 */
static int near advance(unsigned char *lp, unsigned char *ep, unsigned char *end)
{
	unsigned char 	c;
	unsigned char 	c2;
	int	    		cnt1;
	int	    		cnt2;
	unsigned char *	e2;
	unsigned char *	curlp;
	unsigned char *	bbeg;

	while(lp <= end) {
	  switch(*ep++) {
		case CCHR:
			if(*ep++ == *lp++) continue;
			return(0);

		case CCASE:
			c = *lp++;
			if (*ep == c || ep[1] == c) {
				ep += 2;
				continue;
			}
			return 0;

		case CDOT:
			if (lp++ < end) continue;
			return(0);

		case CIDS:
			if (lp == __locs)
				continue;
			if (!isident(lp[-1]))
				continue;
			return 0;

		case CIDE:
			if (lp == end || !isident(*lp)) 
			    break;
			return 0;

		case CCL:
			c = *lp++;
			if (ISTHERE(c)) {
				ep += MAXCTAB;
				continue;
			}
			return(0);

		case CDOL:
			if (lp == end) continue;
			return(0);

		case CCEOF:
			__loc2 = lp;
			return(1);

		case CBRA:
			_braslist[*ep++] = lp;
			ep++;
			continue;

		case CKET:
			_braelist[*ep++] = lp;
			continue;

		case CBACK:
			bbeg = _braslist[*ep];
			e2   = &lp[_braelist[*ep++]-bbeg];
			if (e2 <= end && ecmp(lp, bbeg, e2)) {
				lp = e2;
				continue;
			}
			return(0);

		case CBRA | CPIPE:
			_braslist[*ep] = lp;
			ep += 2;
			if (advpipe(lp,ep,end))
				return 1;
			return 0;
		
		case CPIPE:
			ep--;
			ep += ep[2];
			break;

		case CCASE | RNGE:
			c2  = *ep++;
			goto _1;

		case CCHR | RNGE:
			c2  = *ep;
_1:			c   = *ep++;
			e2  = lp + getrnge(lp,ep);
			ep += 2;
			if (e2 > end)
				return 0;
			while(lp < e2) {
				if(*lp != c && *lp != c2)
					return(0);
				lp++;
			}
			curlp = lp;
			e2   += __size;
			if (e2 > end)
				e2 = end;
			while(lp < (unsigned char *) e2) {
				if(*lp != c && *lp != c2)
					goto star;
				lp++;
			}
			goto star;

		case CDOT | RNGE:
			lp = lp + getrnge(lp,ep);
			ep += 2;
			if (lp > end)
				return 0;
			curlp = lp;
			lp  += __size;
			if (lp > end)
				lp = (unsigned char *) end;
			goto star;

		case CCL | RNGE:
			e2 = lp + getrnge(lp,&ep[MAXCTAB]);
			if (e2 > end)
				return 0;
			while(lp < e2) {
				c = *lp++;
				if(!ISTHERE(c))
					return(0);
			}
			curlp = lp;
			e2   += __size;
			if (e2 > end)
				e2 = end;
			while(lp < e2) {
				c = *lp++;
				if(!ISTHERE(c)) {
					lp--;
					break;
				}
			}
			ep += (MAXCTAB + 2) ;
			goto star;

		case CBACK | RNGE:
docback:		bbeg  = _braslist[*ep];
			c     = _braelist[*ep++] - bbeg;
			e2    = &lp[c];
			cnt1  = getrnge(0,ep);
			cnt2  = cnt1 + __size;
			ep   += 2;
			curlp = lp;
			while(P_LE(e2,end) && --cnt2 >= 0 && ecmp(lp, bbeg, e2)) {
				cnt1--;
				lp += c;
				e2 += c;
			}
			if (cnt1 <= 0)
				while(P_GE(lp,curlp)) {
					if(advance(lp, ep, end)) 
						return(1);
					lp -= c;
				}
			return(0);

		
		case CKET | RNGE:
			_braelist[*ep] = lp;
			lp             = _braslist[*ep];
			goto docback;	

		default:
			/* INTERNAL REGEX_ERROR !!!! */
			return 0;
star:
		lp++;
		do {
			lp--;
			if (P_LT(lp,__locs))
				break;
			if (advance(lp, ep, end))
				return(1);
		} while (P_GT(lp,curlp));
		return(0);
	  }
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * step()
 */
int step(unsigned char *lp, unsigned char *ep, unsigned char *end)
{	unsigned char firstc,firstc2;

	__locs = lp;
	if (*ep++) {
		__loc1 = lp;
		return advance(lp,ep+2,end);
	}

	ep = &ep[2];

	if ((firstc = ep[-2]) != 0) {
		/* fast check: case sensitive */
		do {
			if (*lp++ == firstc || *lp++ == firstc || 
			    *lp++ == firstc || *lp++ == firstc) {
				if (P_LE(lp,end) && 
				    advance(&lp[-1],ep,end)) {
					__loc1 = &lp[-1];
					return 1;
				}
			}
		} while (P_LT(lp,end));
		return 0;
	}

	if (*ep == CCASE) {
		/* fast check: ignore case */
		firstc  = ep[1];
		firstc2 = ep[2];

		while (P_LT(lp,end)) {
			if (*lp == firstc || *lp == firstc2) {
				if (advance(lp,ep,end)) {
					__loc1 = lp;
					return 1;
				}
			}
			lp++;
		}
	}

	else

	while(P_LE(lp,end)) {
		if (advance(lp,ep,end)) {
			__loc1 = lp;
			return 1;
		}
		lp++;
	}
	return 0;
}

int _reerrmsg[] = {
     IDS_MSGRENOTNULL,     
     IDS_MSGRECOMPLEXEXPR,
     IDS_MSGREMACRORANGESYNTAX,
     IDS_MSGRERANGE,     
     IDS_MSGREMANYBRACKETS,
     IDS_MSGRENOBRACKETMATCH,
     IDS_MSGREFROMTO,    
     IDS_MSGREMINMAXOVL, 
     IDS_MSGREMINMAXERR, 
     IDS_MSGREBSLERR,    
     IDS_MSGREBRUNDEF,   
     IDS_MSGREPIPEERR    
};                       

REGCMP _regcmp = {
	compile, step, 
	&__loc1, &__loc2, &__locs,
	&_nbrackets,&_staronly,
	&_braslist,&_braelist,
	&_circf,
	&_regerror,&_regepos,
	_reerrmsg
};

#ifdef	TEST
/*--------------------------------------------------------------------------
 * MAIN
 */
int main(int argc,char **argv)
{	char buf[128],*p;
	char ebuf[400];
	int  i,flags = (RE_DOREX|RE_SHELLWILD);
	
	if (argc > 1) {
		if (argc == 3 && strcmp(argv[1],"-y") == 0) {
			flags |= RE_IGNCASE;
			argv++;
		}
		if (!compile(argv[1],ebuf,&ebuf[sizeof ebuf],0,flags)) {
			printf("re REGEX_ERROR %d at \n",_regerror);
			printf("%s\n",argv[1]);
			for (i = 0; i < _regepos; i++)
				printf(">");
			printf("\n");
			return 1;
		}
		while(gets(buf)) {
			if (step(buf,ebuf,(long )&buf[strlen(buf)])) {
				printf("|");
				for (p = buf; *p; p++)
					printf("%c", *p == '\t' ? ' ': *p);
				printf("\n>");
				if (__loc2)
					*__loc2 = 0;
				else
					printf("oops: no __loc2\n");
				for (p = buf; p < __loc1; p++)
					printf(">");
				printf("%s<<<\n",__loc1);
			}
		}
	}
	return 0;
}

#endif

