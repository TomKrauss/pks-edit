/***************************************************************************/
/* edctype.h                                                               */
/* fast character type checking via bit matching            TOM 30.03.88   */
/***************************************************************************/

#ifndef _EDCTYPE_INC
#define _EDCTYPE_INC

#define	_U      0x4      /* upper alpha           */
#define	_L      0x2      /* lower alpha           */
#define	_N      0x1      /* digit                 */
#define	_C      0x8      /* is a word			*/
#define	_S      0x10     /* white space Chars     */
#define	_T      0x20     /* TOS - Filenamecharacters (excl. alpha and digit ) */
#define	_UU     0x40     /* upper German Umlauts  */
#define	_LU     0x80     /* lower German Umlauts  */

extern unsigned char _asciitab[];
extern unsigned char _l2uset[256],_u2lset[256];

#define	pks_isalpha(c)		(_asciitab[(c)]&(_U|_L))
#define	pks_isupper(c)		(_asciitab[(c)]&_U)
#define	pks_islower(c)		(_asciitab[(c)]&_L)
#define	pks_isdigit(c)		(_asciitab[(c)]&_N)
#define	pks_isalnum(c)		(_asciitab[(c)]&(_U|_L|_N))
#define	pks_isspace(c)		(_asciitab[(c)]&_S)
#define	pks_isumlaut(c)     (_asciitab[(c)]&(_UU))
#define	isident(c)   		(_asciitab[(c)]&_C)
#define	istosfname(c)		(_asciitab[(c)]&_T)

#endif
