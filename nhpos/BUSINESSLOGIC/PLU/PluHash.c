/****************************************************************************\
||																			||
||		  *=*=*=*=*=*=*=*=*													||
||        *  NCR 2172     *             NCR Corporation, E&M OISO			||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1999				||
||    <|\/~               ~\/|>												||
||   _/^\_                 _/^\_											||
||																			||
\****************************************************************************/

/*==========================================================================*\
*	Title:			ComputeHash 64
*	Category:		PLU module
*	Program Name:	PluHash.c
* ---------------------------------------------------------------------------
*	Compiler:		VC++ Ver. 6.00 A by Microsoft Corp.              
*	Memory Model:	
*	Options:		
* ---------------------------------------------------------------------------
*	Abstract:
*
* ---------------------------------------------------------------------------
*	Update Histories:
* ---------------------------------------------------------------------------
*	Date     | Version  | Descriptions							| By
* ---------------------------------------------------------------------------
*  Nov/26/99 |			| 2172 initial							| 
\*==========================================================================*/

/*==========================================================================*\
:	PVCS ENTRY
:-------------------------------------------------------------------------
:	$Revision$
:	$Date$
:	$Author$
:	$Log$
\*==========================================================================*/

/*==========================================================================*\
;+																			+
;+					I N C L U D E    F I L E s								+
;+																			+
\*==========================================================================*/

#include	<tchar.h>
#include	<memory.h>							/* memxxx() header			*/
#include	<ecr.h>								/* 2172 system header		*/
#include    <rfl.h>                             /* reentrant library header */
#include	<PLU.H>								/* PLU common header		*/
#include	"PLUDEFIN.H"						/* Mass Memory Module header*/


/*==========================================================================*\
;+																			+
;+			P R O T O T Y P E     D E C L A R A T I O N s					+
;+																			+
\*==========================================================================*/

static __int64	_AtoI64(const char *);
static __int64	_TtoI64(const TCHAR *);

/*==========================================================================*\
;+																			+
;+				P R O G R A M     D E C L A R A T I O N s					+
;+																			+
\*==========================================================================*/

// This hash computation function, ComputeHash_64(), is used by the
// hash computation function ComputeHash() under the condition of
// the variable usLen being greater than 7.
// The representation of the PLU Number has changed over the years
// from the early days in which it was an actual numeric variable
// to when it was a BCD type of digit string.
//
// The number of digits in a PLU number has increased over the years with
// changes in bar codes used. The last change was to 14 digits to support
// RSS bar codes with 14 digits.
//
// Prior to Rel 1.4.0, which is when NCR donated the software to Georgia Southern,
// there were several changes to how the PLU Number was stored. When the number of
// digits was small enough, the PLU Number was stored as a numeric value. When
// the number of digits became sufficently large, probably when a 10 digit PLU
// Number was implemented, the storage was changed from a numeric variable to
// a BCD type character array with a string of character digits.
//
// See also the writeup on Mix and Match where struct RECMIX is defined. Mix and
// Match used the FILE_MIX_MATCH file handle to access the file containing the
// identifier/key and associated data.
//
// In Rel 1.4.0, the original NCR 7442 keyyboard terminal with Windows CE,
// this function worked with a UCHAR array of character digits. The PLU
// Number was composed of 13 digits. Since a unsigned long long of 64 bits
// could handle 13 digits, the hash function used 64 bit variables.
//
// In Rel 2.0.0, the touchscreen, Windows XP, this function was changed
// to work with a TCHAR array of character digits. The PLU Number length
// was changed to 14 digits to accommodate RSS-14 bar codes. Why did this
// require a change in the hash algorithm?
//
//     Richard Chambers, Sep-25-2018
//

// The hashing algorithm being used here as well as the previous version are
// actually pretty bad at spreading the keys fairly uniformly across a space.
// The following hash function from stackoverflow at URL:
//     https://stackoverflow.com/questions/7666509/hash-function-for-string/45641002#45641002
// seems to be much better in my testing of the current, RSS-14 Rel 2.0.0, version
// of the hash functon. This function is based on the djb2 algoritm from the
// work of Dan Bernstein. See URL:
//     http://www.cse.yorku.ca/~oz/hash.html
//
// See also the interesting article about hash algorithms at http://burtleburtle.net/bob/hash/doobs.html
// as well as http://burtleburtle.net/bob/hash/evahash.html
// and http://www.partow.net/programming/hashfunctions/
// and https://softwareengineering.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed
//
//    Richard Chambers, Sep-28-2018
#if 0
// A version of the djb2 hasing algorithm that has been tested in a stand alone
// test application generating PLU Numbers with a 500 thousand max PLU size
// against the legacy, current algorithm introduced with the RSS-14 PLU
// Number change in Rel 2.0.0 which provides a very significant clustering
// of hash values.
//
//    Richard Chambers, Sep-28-2018

ULONG ComputeHash_64_2 (TCHAR *pucKey, USHORT usLen, ULONG ulBase)
{
    ULONG hash = 5381;
    int c;

	for (int i = 0; i < 14; i++) {
		c = pucKey[i];
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	hash %= ulBase;
    return hash;
}
#endif

ULONG	ComputeHash_64(TCHAR *pucKey, USHORT usLen, ULONG ulBase)
{
	TCHAR*		pStop;
	TCHAR		tchUpr[10],tchLwr[10];
	ULONG		ulUpr, ulLwr, ulHash = 0;
//	TCHAR		aucBuf[21];
//	D13DIGITS	d13Hash, d13Key, d13Base;

//	_PLU_ASSERT("ComputeHash",NULL,(usLen < sizeof(aucBuf)));

	/* ASCII to D13DIGIT(__int64) */
	_tcsncpy(tchUpr, pucKey, 9); tchUpr[9] = '\0';
	_tcsncpy(tchLwr, &pucKey[9], 5); tchLwr[5] = '\0';
	ulUpr = _tcstoul(tchUpr, &pStop, 10);
	ulLwr = _tcstoul(tchLwr, &pStop, 10);

	ulLwr += ((ulUpr % ulBase)*10000);

	return (ulLwr % ulBase);
//	memcpy(aucBuf,pucKey,usLen);
//	aucBuf[usLen/2] = _T('\0');
//	d13Key = _TtoI64(aucBuf);
//	d13Base = (D13DIGITS)ulBase;

//	d13Hash = d13Key % d13Base;

//	return	(ULONG)d13Hash;
}

#if defined(POSSIBLE_DEAD_CODE)

// The following source code does not appear to be used for anything
// currently. Looking over the history since 2002 and Rel 1.4.0 when the source
// was donated by NCR to Georgia Southern, it looks like the function _AtoI64()
// was used when the PLU Number was stored as a UCHAR array of character
// digits. It looks like the function _TtoI64() was introduced with Rel 2.0.0
// and used in function ComputeHash_64() at some point but that function was since
// changed and the usage commented out.
//     Richard Chambers, Sep-25-2018

#define _ISSPACE(j) (j==' ' || j=='\t' || j=='\n')
#define _ISDIGIT(c)		( ((c) >= '0') && ((c) <= '9') )

__int64	_AtoI64(const char *nptr)
{
	int		c;			/* current char */
	__int64	total;		/* current total */
	int		sign;		/* if '-', then negative, otherwise positive */

	/* skip whitespace */
	while ( _ISSPACE((int)(unsigned char)*nptr) )
		++nptr;

	c = (int)(unsigned char)*nptr++;
	sign = c;			/* save sign indication */
	if (c == '-' || c == '+')
		c = (int)(unsigned char)*nptr++;	/* skip sign */
		total = 0;
		while (_ISDIGIT(c)) {
		total = 10 * total + (c - '0');	 /* accumulate digit */
		c = (int)(unsigned char)*nptr++;	/* get next char */
	}

	if (sign == '-')
		return -total;
	else
		return total;	/* return result, negated if necessary */
}

#define _ISSPACET(j) (j==_T(' ') || j==_T('\t') || j==_T('\n'))
#define _ISDIGITT(c)		( ((c) >= _T('0')) && ((c) <= _T('9')) )

__int64	_TtoI64(const TCHAR *nptr)
{
	int		c;			/* current char */
	__int64	total;		/* current total */
	int		sign;		/* if '-', then negative, otherwise positive */

	/* skip whitespace */
	while ( _ISSPACET((int)(TCHAR)*nptr) )
		++nptr;

	c = (int)(TCHAR)*nptr++;
	sign = c;			/* save sign indication */
	if (c == _T('-') || c == _T('+'))
		c = (int)(TCHAR)*nptr++;	/* skip sign */
		total = 0;
		while (_ISDIGITT(c)) {
		total = 10 * total + (c - 0x30);	 /* accumulate digit */
		c = (int)(TCHAR)*nptr++;	/* get next char */
	}

	if (sign == _T('-'))
		return -total;
	else
		return total;	/* return result, negated if necessary */
}

#endif

/*==========================================================================*/

