/*

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation

*===========================================================================
* Title       : ASCII string format routine                 
* Category    : Reentrant Functions Library, NCR 2170  Application
* Program Name: RflStr.C                                                      
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-21-92 : 00.00.01 : K.Nakajima :                                    
* Sep-24-92 : 00.01.04 : O.Nakada   : Modify buffer size in RflStrAdjust()
* Dec-26-95 : 02.05.05 : M.Suzuki   : Modify RflStrAdjust()
* Sep-24-03 :          :R.Herrington: Add tcharset()
*           :          :            :                                    
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/

/**
;========================================================================
;+                  I N C L U D E     F I L E s 
;========================================================================
**/
#include <tchar.h>
#include <string.h>
#include <stdlib.h>
#include <ecr.h>
#include "pif.h"
#include <rfl.h>
#include "rflin.h"
#include "log.h"

// Following value is used to provide a reasonable upper bound on character buffer
// sizes for ASSERTs.  We should not have any strings longer than this value.
// This value is basically used to help detect when a subtraction has resulted
// in a signed value less than zero which has then been converted into a huge
// unsigned value.  For instance -1 signed is something like 0xFFFE unsigned.
#define MAX_COUNT_ASSERT   2000

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/

/*
 *=============================================================
 *
 *   input    : char   *aszFilePath     - file path name to shorten
 *
 *   return   : char *					- pointer within path to shorten the path name
 *
 *   output   : TCHAR  *pszDest          -Formated String
 *
 *   synopis  : return a pointer that ensures the file path is only the
 *              last part of the path. The purpose of this function is
 *              to be used with the debug logging functions scattered
 *              about to reduce the length of a file path text string
 *              to a known max length to prevent buffer overrun.
 *
 *=============================================================
*/
const char * RflPathChop(const char *aszFilePath)
{
	int iLen = strlen(aszFilePath);

	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	return aszFilePath + iLen;
}

/**fh
;=============================================================
;
;   function : format string  ( simulate sprintf() )
;
;   Format   : USHORT RflSPrintf(UCHAR *, UCHAR, const UCHAR FAR *,...);       
;   input    : TCHAR   *pszDest,        -Destination buffer pointer
;              USHORT  uchDestLen       -Destination buffer Length
;              TCHAR   *pszFormat,      -Source string pointer Length
;              argumtent,               -print option
;
;   return   : USHORT					-Written string's length (not include '\0')
;
;   output   : TCHAR  *pszDest          -Formated String
;
;   synopis  : This function formats string. 
;
;==============================================================
fh**/
#if defined(RflSPrintf)
static  char    *RflSPrintf_Debug_aszFilePath = 0;
static  int      RflSPrintf_Debug_nLineNo = 0;
static  USHORT   RflSPrintf_Debug_usRetStatus = 0;

USHORT   RflSPrintf_Debug_line(char *aszFilePath, int nLineNo)
{
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	RflSPrintf_Debug_aszFilePath = aszFilePath + iLen;
	RflSPrintf_Debug_nLineNo = nLineNo;

	return RflSPrintf_Debug_usRetStatus;
}

USHORT   RflSPrintf_Debug(TCHAR *pszDest, USHORT usDestLen, const TCHAR FAR *pszFormat, ...)
{
    RflSPrintf_Debug_usRetStatus = _RflFormStr(pszFormat, (SHORT *)( (UCHAR *)&pszFormat + sizeof(pszFormat) ), pszDest, usDestLen);

	if (RflSPrintf_Debug_usRetStatus >= 0x7fff) {
		char xBuffer[128];
		sprintf (xBuffer, "RflSPrintf_Debug(): File %s, lineno = %d", RflSPrintf_Debug_aszFilePath, RflSPrintf_Debug_nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
		PifAbort(MODULE_RFL_FORMSTR, FAULT_BUFFER_OVERFLOW);
	}
	return RflSPrintf_Debug_usRetStatus;
}

#else
USHORT  RflSPrintf(TCHAR *pszDest, USHORT usDestLen, const TCHAR FAR *pszFormat, ...)
{
	NHPOS_ASSERT(usDestLen < MAX_COUNT_ASSERT);

    return( _RflFormStr(pszFormat, (SHORT *)( (UCHAR *)&pszFormat + sizeof(pszFormat) ), pszDest, usDestLen) );
}
#endif

/**fh
;=============================================================
;
;   function : Adjust string with MaxColumn
;
;   Format   : UCHAR  RflStrAdjust(UCHAR *, USHORT, UCHAR *,  UCHAR, BOOL);       
;   input    : UCHAR  *pszDest,         -Destination buffer pointer
;              USHORT usDestLen,        -Destination buffer length
;              UCHAR  *pszSource,       -Source string pointer Length
;              UCHAR  uchMaxColumn,     -Max column
;              BOOL   fAutoFeed,        -Auto line feed flag
;
;   return   : UCHAR  uchWrtLen         -Written string's length (not include '\0')
;
;   output   : UCHAR  *pszDest           -Formated String
;
;   synopis  : This function formats string with MaxColumn 
;
;
;==============================================================
fh**/
USHORT  RflStrAdjust(TCHAR *pszDest,  USHORT usDestLen, TCHAR *pszSource, USHORT uchMaxColumn, BOOL fAutoFeed)
{
    TCHAR  *puchRead;         /* Source buffer read pointer */
    TCHAR  *puchWrite;        /* Destination buffer write pointer */
    USHORT uchColPos;         /* Current Column position */
    TCHAR  *puchWork;         /* Work Area */
    TCHAR  auchTabString[80]; /* Tab stirng buffer  */
    TCHAR  *puchTabWrt;       /* Tab stirng buffer write pointer */
    TCHAR  uchTabChar;        /* Tab string character counter */
    USHORT uchWrtCount;       /* Tab string write counter */
	USHORT usChineseFlag = 0;

	NHPOS_ASSERT(usDestLen < MAX_COUNT_ASSERT);

    /* --- initialize ---*/
    tcharnset(pszDest, RFL_SPACE, usDestLen);
    puchRead   = pszSource;
    puchWrite  = pszDest;
    uchColPos  = 0;

    while ( (*puchRead != _T('\0'))                         /* until string end */
          && (puchWrite < (pszDest+usDestLen - 1)) ) {  /* until destination buffer end */

/*---------------------------------*\
       --- Tab management ---
\*---------------------------------*/
        if (*puchRead == _T('\t')) {
        
            /* --- initialize --- */
            tcharnset(auchTabString, RFL_SPACE, 80);
            //memset(auchTabString, RFL_SPACE, sizeof(auchTabString));
            uchTabChar = 0;                          /* Num. of written character */
            puchRead++;                              /* for character '\t' */
            puchTabWrt  = auchTabString;

            while ( (*puchRead != _T('\t')) && (*puchRead != _T('\n')) && (*puchRead != _T('\0')) ) {
                *puchTabWrt++ = *puchRead++;
                uchTabChar++;
            }

            if (uchTabChar + uchColPos > uchMaxColumn) {
                if (fAutoFeed == RFL_FEED_ON) {
                    *puchWrite++ = _T('\n');
                    puchWrite += uchMaxColumn - 1;

                } else {
                    puchWrite += uchMaxColumn - uchColPos - 1;
                }
            } else {
                puchWrite += uchMaxColumn - uchColPos - 1;
            }

            /* --- Check write point --- */
            if ( puchWrite >=  (pszDest+usDestLen) )  {
                pszDest[usDestLen - 1] = _T('\0');   /* Change Dec-26-95 */
                return((USHORT)(usDestLen - 1)); /* return written length Chg Dec-26-95 */
            }

            uchWrtCount = 0;
            puchTabWrt--;                       /* point the last char */ 

			/*===If puchTabWrt is in this range of values, then the characters that we are working with
			are double wide and we need to back the pointer up one extra space, to accomodate the extra 
			character, we also turn a flag on so that it can be used down in the function*/
			if(*puchTabWrt >= 0x4E00 && *puchTabWrt <= 0x9FA5 ){
				puchWrite--;
				usChineseFlag = 1;
			}

            /* --Write TabStringbufferdata to destination buffer -- */
            while ((auchTabString <= puchTabWrt) && (uchWrtCount < uchMaxColumn)) {
                *puchWrite-- = *puchTabWrt--;
                uchWrtCount++;
            }

			/*If the flag is on, we are working with chinese characters and we have subtracted an extra character for
			them, and therefore need to push the pointer 2 spaces to acccomdaete for this.*/
			if( usChineseFlag)
			{
				puchWrite += uchWrtCount + (UCHAR)2;
				usChineseFlag = 0;
			}
			else
			{
				puchWrite += uchWrtCount + (UCHAR)1;
			}

/*---------------------------------*\
  --- Carrige return management ---
\*---------------------------------*/
        } else if (*puchRead == _T('\n')) {
                                 
            *puchWrite++ = *puchRead++;        /* write data, increment read & write point */
            uchColPos = 0;                     /* column position initialize */
/*---------------------------------*\
  --- other character management ---
\*---------------------------------*/
        } else {

            /* when cureent column is max - 1column and going to write double wide character */                  
            if ( uchColPos == (uchMaxColumn - (UCHAR)1) && (UCHAR)*puchRead == (UCHAR)(RFL_DOUBLE) ) {

                if (fAutoFeed == RFL_FEED_ON) {

                    *puchWrite++ = _T('\n');
                    
                    /* --- Check write point --- */
                    if ( puchWrite >=  (pszDest+usDestLen) )  {
                        *(pszDest + (usDestLen - 1)) = _T('\0');
                        return( (USHORT)(usDestLen - 1) );
                    }

                    /* -- write data, increment read & write point -- */
                    *puchWrite++ = *puchRead++;

                    /* --- Check write point --- */
                    if ( puchWrite >=  (pszDest+usDestLen) )  {
                        *(pszDest + (usDestLen - 1)) = _T('\0');
                        return( (USHORT)(usDestLen - 1) );
                    }
                    uchColPos = 1;
                } else {
                    puchRead++;                 /* write single wide character */
                }
            }


            *puchWrite++ = *puchRead++;         /* write data, increment read & write point */
            uchColPos++;                        /* increment column posit */

            /* -- Check current column position -- */
            if (uchColPos >= uchMaxColumn) {
                if (fAutoFeed == RFL_FEED_ON) {
                    *puchWrite++ = _T('\n');
                    uchColPos = 0;              /* initialize column position */
                } else {
                    /* -- move Readpoint(puchRead) to next '\t','\n' or '\0' -- */
                    /* - get '\t'  point - */
                    puchWork  = _tcschr(puchRead, _T('\t'));       

                    /* - move readpoint to next '\n' - */
                    puchRead  = _tcschr(puchRead, _T('\n'));

                    if ( (puchRead==NULL) && (puchWork!=NULL) ) {
                        puchRead = puchWork;
                    } else if ( puchRead == puchWork ) {
                        /* - move readpoint to string end - */
                        puchRead = _tcschr(pszDest, _T('\0'));     
                    } else if ( (puchRead > puchWork) && (puchWork != NULL) ) {
                        /* - move readpoint to next '\t' - */
                        puchRead = puchWork;
                    }
                }
            }
        }
    }

    if (*puchWrite == _T('\n')) {
        puchWrite--;                            /* delete last return */
    }

    *puchWrite = _T('\0');                          /* write null */

    return((USHORT)(puchWrite - pszDest));       /* return written length */

}

/*******************************************************
input:
 TCHAR* string	- area/string to modify
 TCHAR  val		_ value to set the string
 size_t count	_ Number of Characters to set
output:
 TCHAR* - the start of the string modified

corrects the initialization for WIDE Characters
_tcsnset checks the length of string and will use
the shorter of the two values (count or length) to set
this function(tcharnset) uses only the count

  This function DOES NOT check for a NULL Character and 
  will go beyond the end of the buffer if the size specified
  is too large.
********************************************************/

TCHAR * tcharnset (TCHAR * string, TCHAR val, size_t count)
{
	TCHAR *start = string;

	NHPOS_ASSERT(count < MAX_COUNT_ASSERT);

	for( ; count > 0; count--)
		*string++ = val;

	return(start);
}

TCHAR * tcharncpy (TCHAR *tcDest, CONST TCHAR *tcSource, size_t count)
{
	TCHAR *start = tcDest;

	NHPOS_ASSERT(count < MAX_COUNT_ASSERT);

	for( ; count > 0; count--)
		*tcDest++ = *tcSource++;

	return(start);
}

USHORT tcharlen(CONST TCHAR *tcSource)
{
	CONST TCHAR *start = tcSource;

	if (tcSource) {
		while (*tcSource) tcSource++;
	}

	NHPOS_ASSERT((tcSource - start) < MAX_COUNT_ASSERT);

	return (tcSource - start);
}

TCHAR *tcharTrimLeading (TCHAR  *tcString)
{
	TCHAR *tcStringSave = tcString;

	if (tcString) {
		TCHAR *tcFirst = tcString;

		for ( ; *tcString == _T(' '); tcString++) ;  // find first non-blank or end of string

		for ( ; (*tcFirst++ = *tcString++); ) ;      // copy rest of string up to and including end of string

	}
	return tcStringSave;
}

TCHAR *tcharTrimRight (TCHAR  *tcString)
{
	TCHAR *tcLastBlank = 0;
	TCHAR *tcStringSave = tcString;

	if (tcString) {
		for ( ; *tcString; tcString++) {
			if (*tcString == _T(' ')) {
				if (tcLastBlank == 0)
					tcLastBlank = tcString;
			} else {
				tcLastBlank = 0;
			}
		}
		if (tcLastBlank) {
			*tcLastBlank = 0;
		}
	}
	return tcStringSave;
}

/*
	_RflStrcpyUchar2Tchar - copy UCHAR string to a TCHAR string buffer
 */
void _RflStrcpyUchar2Tchar (TCHAR *aszDest, UCHAR *aszSource)
{
#if defined(DEBUG) || defined(_DEBUG)
	int count = 0;

	while ((*aszDest++ = (TCHAR) *aszSource++)) {
		count++;
		NHPOS_ASSERT(count < MAX_COUNT_ASSERT);
	}
#else
	while ((*aszDest++ = (TCHAR) *aszSource++)) ;
#endif
}

// This function takes a currency value that is in a string and converts it to
// a long.
// This function does not consider issues such as the decimal setting of the
// GenPOS provisioning for number of decimal places.
// Primary intent is the conversion of purchase and gratuity fields from DSI Client.
LONG RflConvertCharFieldToLongCurrency (CHAR *auchCurrency, int iMaxChars)
{
	LONG     lCurrencyTtl = 0;
	int      i, x;
	CHAR    temp[28];

	if (iMaxChars >= sizeof(temp)) {
		iMaxChars = sizeof(temp) - 1;
	}

	i=0;
	for( x = 0; x < iMaxChars; x++){
		if(auchCurrency[x] != '.')
		{
			temp[i] = auchCurrency[x];
			i++;
		}
	}

	temp[i] = 0;
	lCurrencyTtl = atol (temp);

	return lCurrencyTtl;
}

CHAR *RflConvertLongCurrencyToCharField (LONG lCurrency, CHAR *auchCurrency, int iMaxChars)
{
	int       i;
//	PARAMDC   myParaMDC;

	for (i = 0; i < iMaxChars; i++) auchCurrency[i] = '0';
	if (lCurrency < 0) {
		auchCurrency[0] = '-';   // put the negative sign into the first digit position
		lCurrency *= -1;         // change the sign of the currency to be positive
	}

#if 1
	i = iMaxChars - 3;
	auchCurrency[i] = '.';
#else
	// lets find out where the decimal belongs and go ahead and put it there.
	myParaMDC.uchMajorClass = CLASS_PARAMDC;
	myParaMDC.usAddress = MDC_SYSTEM3_ADR;
	ParaMDCRead(&myParaMDC);

	if((myParaMDC.uchMDCData & MDC_ODD_MASK) == 0x00)//2 decimal
	{
		i = iMaxChars - 3;
		auchCurrency[i] = '.';
	}
	else if((myParaMDC.uchMDCData & MDC_ODD_MASK) == 0x01)//3 decimal
	{
		i = iMaxChars - 4;
		auchCurrency[i] = '.';
	}
	else//no decimal
	{
		;  // nothing to do
	}
#endif
	i = iMaxChars - 1;
	while (lCurrency > 0 && i >= 0) {
		if (auchCurrency[i] != '.' && auchCurrency[i] != '-') {
			auchCurrency[i] = (UCHAR)(lCurrency % 10) + '0';
			lCurrency /= 10;
		}
		i--;
	}

	return auchCurrency;
}

/*
	_RflStrncpyUchar2Tchar - copy UCHAR string to a TCHAR string buffer with max number specified
 */
void _RflStrncpyUchar2Tchar (TCHAR *aszDest, UCHAR *aszSource, USHORT usMaxChars)
{
	NHPOS_ASSERT(usMaxChars < MAX_COUNT_ASSERT);

	while (usMaxChars > 0 && (*aszDest++ = (TCHAR) *aszSource++)) usMaxChars--;
}

/*
	_RflStrcpyTchar2Uchar - copy TCHAR string to a UCHAR string buffer
 */
void _RflStrcpyTchar2Uchar (UCHAR *aszDest, TCHAR *aszSource)
{
#if defined(DEBUG) || defined(_DEBUG)
	int count = 0;

	while ((*aszDest++ = (UCHAR) (*aszSource++ & 0x00ff))) {
		count++;
		NHPOS_ASSERT(count < MAX_COUNT_ASSERT);
	}
#else
	while ((*aszDest++ = (UCHAR) (*aszSource++ & 0x00ff))) ;
#endif
}

/*
	_RflStrncpyTchar2Uchar - copy TCHAR string to a UCHAR string buffer with max number specified
 */
void _RflStrncpyTchar2Uchar (UCHAR *aszDest, TCHAR *aszSource, USHORT usMaxChars)
{
	NHPOS_ASSERT(usMaxChars < MAX_COUNT_ASSERT);

	while (usMaxChars > 0 && (*aszDest++ = (UCHAR) (*aszSource++ & 0x00ff))) usMaxChars--;
}

/*
	_RflMemcpyTchar2Uchar - copy TCHAR string to a UCHAR string buffer with mandatory number specified
		This replaces memcpy when source string is a TCHAR and the destination string is a UCHAR.
 */
void _RflMemcpyTchar2Uchar (UCHAR *aszDest, TCHAR *aszSource, USHORT usNoChars)
{
	NHPOS_ASSERT(usNoChars < MAX_COUNT_ASSERT);

	while (usNoChars > 0) {
		*aszDest++ = (UCHAR) (*aszSource++ & 0x00ff);
		usNoChars--;
	}
}

/*
	_RflMemcpyUchar2Tchar - copy UCHAR string to a TCHAR string buffer with mandatory number specified
		This replaces memcpy when source string is a UCHAR and the destination string is a TCHAR.
 */
void _RflMemcpyUchar2Tchar (TCHAR *aszDest, UCHAR *aszSource, USHORT usNoChars)
{
	NHPOS_ASSERT(usNoChars < MAX_COUNT_ASSERT);

	while (usNoChars > 0) {
		*aszDest++ = (TCHAR) (*aszSource++ & 0x00ff);
		usNoChars--;
	}
}


#if 0
See https://stackoverflow.com/questions/4040835/how-do-i-write-a-simple-regular-expression-pattern-matching-function-in-c-or-c/34824044#34824044
and http://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

From Brian Kernighan's article

"Rob disappeared into his office, and at least as I remember it now, appeared
again in no more than an hour or two with the 30 lines of C code that
subsequently appeared in Chapter 9 of TPOP. That code implements a
regular expression matcher that handles these constructs:

  c    matches any literal character c
  .    matches any single character
  ^    matches the beginning of the input string
  $    matches the end of the input string
  *    matches zero or more occurrences of the previous character
"
#endif

static int matchstar(int c, const wchar_t *regexp, const wchar_t *text);
static int matchhere(const wchar_t *regexp, const wchar_t *text);


/* matchhere: search for regexp at beginning of text */
static int matchhere(const wchar_t *regexp, const wchar_t *text)
{
    if (regexp[0] == L'\0')
        return 1;
    if (regexp[1] == L'*')
        return matchstar(regexp[0], regexp + 2, text);
    if (regexp[0] == L'$' && regexp[1] == L'\0')
        return *text == L'\0';
    if (*text != L'\0' && (regexp[0] == L'.' || regexp[0] == *text))
        return matchhere(regexp + 1, text + 1);
    return 0;
}

/* matchstar: search for c*regexp at beginning of text */
static int matchstar(int c, const wchar_t *regexp, const wchar_t *text)
{
    do {    /* a * matches zero or more instances */
        if (matchhere(regexp, text))
            return 1;
    } while (*text != L'\0' && (*text++ == c || c == L'.'));
    return 0;
}

/* match: search for regexp anywhere in text */
int RflMatch(const wchar_t *regexp, const wchar_t *text)
{
    if (regexp[0] == L'^')
        return matchhere(regexp + 1, text);
    do {    /* must look even if string is empty */
        if (matchhere(regexp, text))
            return 1;
    } while (*text++ != L'\0');
    return 0;
}

#if 0
// the encryption works as follows.
// each byte is transformed so that the various bits are moved around.
// Which bits go where are index dependent which provides for more
// random looking bit strings as a result though if the same character
// is used through out the same string and the string is long enough,
// then a pattern of transforms will be evident.

// WARNING:  Becareful when modifying this algorithm because there are
//           places in the code where the encrypted value is checked against
//           binary zero.  Make sure than any algorithm used will replicate
//           a binary zero as binary zero.

typedef enum {ShiftLeft = 1, ShiftRight, ShiftNone, ShiftEnd} ShiftType;

typedef struct  {
	short  nShift;
	ShiftType  xType;
	unsigned short  usMask;
}  ShiftDescription;

#define SHIFT_TABLE_SIZE  8

static ShiftDescription myShifts [SHIFT_TABLE_SIZE][6] = {
	{
		// byte 0
		//    [8 7 6 5 4 3 2 1]
		//    [4 3 2 1 6 5 8 7]
		{6, ShiftRight, 0x03},
		{4, ShiftLeft, 0xf0},
		{2, ShiftRight, 0x0c},
		{0, ShiftEnd, 0x00}
	},
	{
		// byte 1
		//    [8 7 6 5 4 3 2 1]
		//    [2 1 4 3 5 8 7 6]
		{5, ShiftRight, 0x07},
		{6, ShiftLeft, 0xc0},
		{1, ShiftRight, 0x08},
		{2, ShiftLeft, 0x30},
		{0, ShiftEnd, 0x00}
	},
	{
		// byte 2
		//    [8 7 6 5 4 3 2 1]
		//    [3 8 7 6 2 1 5 4]
		{1, ShiftRight, 0x70},
		{3, ShiftRight, 0x03},
		{5, ShiftLeft, 0x80},
		{2, ShiftLeft, 0x0c},
		{0, ShiftEnd, 0x00}
	},
	{
		// byte 3
		//    [8 7 6 5 4 3 2 1]
		//    [6 5 4 3 2 8 7 1]
		{5, ShiftRight, 0x06},
		{2, ShiftLeft, 0xe0},
		{2, ShiftLeft, 0x18},
		{0, ShiftNone, 0x01},
		{0, ShiftEnd, 0x00}
	},
	{
		// byte 4
		//    [8 7 6 5 4 3 2 1]
		//    [6 5 8 7 2 1 4 3]
		{2, ShiftRight, 0x33},
		{2, ShiftLeft, 0xcc},
		{0, ShiftEnd, 0x00}
	},
	{
		// byte 5
		//    [8 7 6 5 4 3 2 1]
		//    [7 3 2 1 8 4 6 5]
		{1, ShiftLeft, 0x80},
		{4, ShiftLeft, 0x70},
		{4, ShiftRight, 0x08},
		{1, ShiftRight, 0x04},
		{4, ShiftRight, 0x03},
		{0, ShiftEnd, 0x00}
	},
	{
		// byte 6
		//    [8 7 6 5 4 3 2 1]
		//    [3 8 7 6 2 1 5 4]
		{1, ShiftRight, 0x70},
		{3, ShiftRight, 0x03},
		{5, ShiftLeft, 0x80},
		{2, ShiftLeft, 0x0c},
		{0, ShiftEnd, 0x00}
	},
	{
		// byte 7
		//    [8 7 6 5 4 3 2 1]
		//    [6 5 8 7 2 1 4 3]
		{2, ShiftRight, 0x33},
		{2, ShiftLeft, 0xcc},
		{0, ShiftEnd, 0x00}
	}
};

// Encrypt a string.
//  Beware of the WARNING above!!!!!
UCHAR * RflEncryptByteString (UCHAR *bsString, int nByteCount)
{
	UCHAR  bsByte;
	int   i, j, k;

	for (i = 0; i < nByteCount; i++) {
		bsByte = bsString[i];
		bsString[i] = 0;
		k = (i % SHIFT_TABLE_SIZE);
		for (j = 0; myShifts[k][j].xType != ShiftEnd; j++) {
			if (myShifts[k][j].xType == ShiftLeft) {
				bsString[i] |= ((bsByte << myShifts[k][j].nShift) & myShifts[k][j].usMask);
			}
			else if (myShifts[k][j].xType == ShiftRight) {
				bsString[i] |= ((bsByte >> myShifts[k][j].nShift) & myShifts[k][j].usMask);
			}
			else if (myShifts[k][j].xType == ShiftNone) {
				bsString[i] |= (bsByte & myShifts[k][j].usMask);
			}
		}
	}
	return bsString;
}

// Decrypt a string.
//  Beware of the WARNING above!!!!!
UCHAR * RflDecryptByteString (UCHAR *bsString, int nByteCount)
{
	UCHAR  bsByte;
	int   i, j, k;

	for (i = 0; i < nByteCount; i++) {
		bsByte = bsString[i];
		bsString[i] = 0;
		k = (i % SHIFT_TABLE_SIZE);
		for (j = 0; myShifts[k][j].xType != ShiftEnd; j++) {
			if (myShifts[k][j].xType == ShiftLeft) {
				bsString[i] |= ((bsByte & myShifts[k][j].usMask) >> myShifts[k][j].nShift);
			}
			else if (myShifts[k][j].xType == ShiftRight) {
				bsString[i] |= ((bsByte & myShifts[k][j].usMask) << myShifts[k][j].nShift);
			}
			else if (myShifts[k][j].xType == ShiftNone) {
				bsString[i] |= (bsByte & myShifts[k][j].usMask);
			}
		}
	}
	return bsString;
}

#if 0
int main(int argc, char* argv[])
{
	char *pstrings[] = {
		"00000000000000000",
		"12345678901234565",
		"45678000012222202",
		"ABCDEFGHIJKLMNOPQ",
		"RSTUVWXYZ01234567",
		"89012345678901234",
		"BCDEFGHIJKLMNOPQR",
		"STUVWXYZAZYXWVUTS",
		"RQPONMLKJIHGFEDCB",
		"abcdefghijklmnopq",
		"rstuvwxyzABabCDcd",
		"wxyzABabCDcdrstuv",
		"STUVWXYZAZYXWVUTS",
		"HIJKPQABCDEFGLMNO",
		0
	};


	char **pb = &pstrings[0];
	UCHAR  xstring [64], ystring[64];
	int n;

	while (*pb) {
		n = strlen(*pb);
		memcpy (xstring, *pb, n);
		memcpy (ystring, *pb, n);
		RflEncryptByteString (xstring, n);
		RflDecryptByteString (xstring, n);
		if (memcmp (xstring, ystring, n) != 0) {
			printf ("memcmp failed  string  %s\n", *pb);
		}
		pb++;
	}
	printf ("All Done\n");
	gets ((char *)xstring);
	return 0;
}
#endif
#endif