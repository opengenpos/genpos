/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : "printf" Format Function
* Category    : Reentrant Functions Library, 2170 Application
* Program Name: RFLFORM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Small/Midium/Compact/Large
* Options     : /c /A? /W4 /G1 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: A necessary function for the conversion of the format of the
*           reentrant functions library for this file is described.
*
*           This file containts:
*
*               _RflSetDecimal()      - set the space of the comma
*               _RflSetComma()        - set the position of decimal point
*               _RflFormStr()         - convert the character string with
*                                       the format
*               RflD13ToDecimalStr()  - convert D13DIGITS number to decimal string
*                   RflCommaDecimal() - insert comma and decimal point
*
* --------------------------------------------------------------------------
* Update Histories
*
* Data       Ver.      Name         Description
* Dec-19-94  0.00.01   O.Nakada     Initial
* Feb-24-93  01.00.1?  O.Nakada     Modify %d, %ld, %$, %l$ and %L$ for
*                                   minimum data handling.
* Feb-27-95  R3.0      O.Nakada     Refreshed all functions.
*
** NCR2171 **
* Sep-17-99 : 1.00.00  : M.Teraki   : Changed type definition of 'D13DIGITS'
*                                     Add RflD13ToDecimalStr()
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


/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <tchar.h>
#include <string.h>
#include <stdlib.h>
#include "ecr.h"
#include "pif.h"
#ifdef  SYSUTIL
#include "piftable.h"
#endif
#include "log.h"
#include "rfl.h"
#include "rflform.h"


/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
_RFLFMTCTRL _RflEdit = {_RFL_INIT_DECIMAL,
                        _RFL_INIT_COMMA};

static  VOID RflCommaDecimal(TCHAR *pszStr, SHORT sComma, SHORT sDecimal);

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    UCHAR _RflSetCommaDecimal(UCHAR uchSet);
*     Input:    uchPos - Decimal or Comma
*
** Return:      Previous for Decimal or Comma
*
** Description: This function sets whether to use a comma or a decimal
*				for decimal point values
*===========================================================================
*/ //ESMITH CURRENCY
UCHAR  _RflSetCommaDecimal(UCHAR uchFlag, UCHAR uchSet)
{
	UCHAR uchReturn;

	if ( uchFlag == _RFL_DECIMAL_SEP ) {
		_RflEdit.uchDecimal = uchSet;
		uchReturn = (UCHAR)_RflEdit.uchDecimal;
	} else if ( uchFlag == _RFL_DIGITGROUP_SEP ) {
		_RflEdit.uchDigitGroup = uchSet;
		uchReturn = (UCHAR)_RflEdit.uchDigitGroup;
	} else {
		uchReturn = _RFL_USE_ERROR;
	}

	return (uchReturn);
}

/*
*===========================================================================
** Synopsis:    UCHAR _RflSetDecimal(UCHAR uchPos);
*     Input:    uchPos - Deciaml Point
*
** Return:      Previous for Decimal Point
*
** Description: This function sets the position of decimal point of the
*               default which _RflFormStr() function uses.
*===========================================================================
*/
UCHAR _RflSetDecimal(UCHAR uchPos)
{
    UCHAR   uchReturn;

    uchReturn = (UCHAR)_RflEdit.sDecimal;

    if (uchPos != 0xFF) {
        _RflEdit.sDecimal = (SHORT)uchPos;
    } else {
        _RflEdit.sDecimal = _RFL_INIT_DECIMAL;
    }

    return (uchReturn);
}


/*
*===========================================================================
** Synopsis:    UCHAR _RflSetComma(UCHAR uchSpace);
*     Input:    uchSpace - Space of Comma
*
** Return:      Previous for Comma
*
** Description: This function sets the interval of the comma of the default
*               which _RflFormStr() function uses.
*===========================================================================
*/
UCHAR _RflSetComma(UCHAR uchSpace)
{
    UCHAR   uchReturn;

    uchReturn = (UCHAR)_RflEdit.sComma;

    if (uchSpace != 0xFF) {
        _RflEdit.sComma = (SHORT)uchSpace;
    } else {
        _RflEdit.sComma = _RFL_INIT_COMMA;
    }

    return (uchReturn);
}


/*
*===========================================================================
** Synopsis:    USHORT _RflFormStr(CONST TCHAR FAR *lpszFormat,
*                                 SHORT *psArgs, TCHAR *pszUser,
*                                 USHORT usLength);
*     Input:    lpszFormat - format string of printf
*               psArgs     - arguments of printf
*    Output:    pszUser    - destination buffer
*     Input:    usLength  - Length of destination Buffer (Max. 128)
*
** Return:      Converted number of character
*
** Description: This function converts the character string with the format
*               which corresponds to the number of the pull into the user
*               buffer.
*===========================================================================
*/
#if defined(_RflFormStr)
USHORT _RflFormStr_Special(CONST TCHAR *lpszFormat, VOID *pvArgs, TCHAR *pszUser, USHORT usLength);
USHORT _RflFormStr_Debug(CONST TCHAR *lpszFormat, VOID *pvArgs, TCHAR *pszUser, USHORT usLength, char *aszFilePath, int nLineNo)
{
	USHORT   RflSPrintf_Debug_usRetStatus;

	RflSPrintf_Debug_usRetStatus = _RflFormStr_Special(lpszFormat, pvArgs, pszUser, usLength);

	if (RflSPrintf_Debug_usRetStatus >= 0x7fff) {
		int iLen = 0;
		char xBuffer[128];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}
		sprintf (xBuffer, "_RflFormStr_Debug(): File %s, lineno = %d", aszFilePath + iLen, nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
#if !defined(RflSPrintf)
		PifAbort(MODULE_RFL_FORMSTR, FAULT_BUFFER_OVERFLOW);
#endif
	}
	return RflSPrintf_Debug_usRetStatus;
}

USHORT _RflFormStr_Special(CONST TCHAR *lpszFormat, VOID *pvArgs, TCHAR *pszUser, USHORT usLength)
#else
USHORT _RflFormStr(CONST TCHAR *lpszFormat, VOID *pvArgs, TCHAR *pszUser, USHORT usLength)
#endif
{
    TCHAR   uchChar, uchForm;
	TCHAR	uchFill, uchSign, uchMask = 0;
    SHORT   sFmin, sFmax, sDecimal, sComma, sLeading, sLength, sCount;
    SHORT   fsDecimal, fsLeftJust;
    USHORT  usLong, usPos = 0;
    TCHAR   *pszBuf, szBuf[_RFL_LEN_BUF + 1], szWork[10 + 1];
    CONST   TCHAR FAR *lpszFmt = lpszFormat;
    SHORT   sHigh;
    LONG    lLow;
    D13DIGITS d13ArgVal;

    /* Cast to CPU native integer size */
    LONG *plArgs = (LONG *)pvArgs;

	if (usLength < 1)
		return 0;

    memset(szBuf,0x00,sizeof(szBuf));
	memset(szWork,0x00,sizeof(szWork));

	if (lpszFormat == 0 || pszUser == 0) {
		PifAbort(MODULE_RFL_FORMSTR, FAULT_INVALID_DATA);
	}

    for (;;) {
        /* --- skip until '%' character --- */
        while ((uchChar = *lpszFmt++) != _T('%')) { /* not '%' character    */
            if (uchChar == '\0')    {           /* end of format string */
                if (usPos >= usLength) {		//usPos is 0 based while usLength is 1 based
                    PifLog(MODULE_RFL_FORMSTR, FAULT_BUFFER_OVERFLOW);
					PifLog(MODULE_DATA_VALUE(MODULE_RFL_FORMSTR), usPos);
					PifLog(MODULE_DATA_VALUE(MODULE_RFL_FORMSTR), usLength);
                    PifLog(MODULE_LINE_NO(MODULE_RFL_FORMSTR), (USHORT)__LINE__);
#if defined(RflSPrintf) || defined(_RflFormStr)
					usPos = usLength - 1;
					*(pszUser + usPos) = _T('\0');
					return 0x7fff;
#else
                    PifAbort(MODULE_RFL_FORMSTR, FAULT_BUFFER_OVERFLOW);
#endif
                }
                *(pszUser + usPos) = _T('\0');      /* terminate character  */
                return (usPos);
            }
            *(pszUser + usPos++) = uchChar;
			if (usPos >= usLength) {			//usPos is 0 based while usLength is 1 based
                PifLog(MODULE_RFL_FORMSTR, FAULT_BUFFER_OVERFLOW);
                PifLog(MODULE_DATA_VALUE(MODULE_RFL_FORMSTR), usPos);
                PifLog(MODULE_DATA_VALUE(MODULE_RFL_FORMSTR), usLength);
                PifLog(MODULE_LINE_NO(MODULE_RFL_FORMSTR), (USHORT)__LINE__);
#if defined(RflSPrintf) || defined(_RflFormStr)
				usPos = usLength - 1;
				*(pszUser + usPos) = _T('\0');
				return 0x7fff;
#else
                PifAbort(MODULE_RFL_FORMSTR, FAULT_BUFFER_OVERFLOW);
#endif
                *(pszUser + usPos) = _T('\0');      /* terminate character  */
				return usPos;
            }
        }

        /* --- check "%%" format string --- */
        if (*lpszFmt == _T('%')) {                  /* '%' character */
            *(pszUser + usPos++) = uchChar;
            lpszFmt++;
            continue;                           /* next ...      */
        }

        /* ---------- flags ---------- */
        /* --- check left justify --- */
        fsLeftJust = (*lpszFmt == _T('-')) ? (TRUE) : (FALSE);
        if (fsLeftJust) {
            lpszFmt++;
        }

        /* --- check fill character --- */
        sFmin   = 0;                                /* initialize            */
        uchFill = (*lpszFmt == _T('0')) ? (*lpszFmt++) :/* character in argument */
                                      _T(' '); /* space character       */
        if (*lpszFmt == _T('*')) {                      /* value in argument     */
            sFmin = (SHORT)(*plArgs++);
            lpszFmt++;
        } else {                                    /* value in format       */
            while (_T('0') <= *lpszFmt && *lpszFmt <= _T('9')) {
                sFmin = sFmin * 10 + *lpszFmt++ - 0x30;
            }
        }

        /* --- check maximum of number of string --- */
        sFmax = 0;                                  /* initialize          */
        if (*lpszFmt == _T('.')) {                      /* use max. number     */
            fsDecimal = TRUE;
            if (*(++lpszFmt) == _T('*')) {              /* value in argument   */
                sFmax = (SHORT)(*plArgs++);
                lpszFmt++;
            } else {                                /* value in format     */
                while (_T('0') <= *lpszFmt && *lpszFmt <= _T('9')) {
                    sFmax = sFmax * 10 + *lpszFmt++ - 0x30;
                }
            }
        } else {                                    /* not use max. number */
            fsDecimal = FALSE;
        }

        /* --- check comma option --- */
        if (*lpszFmt == _T('#')) {
            if (*(++lpszFmt) == _T('*')) {              /* value in argument */
                sComma = (*plArgs != 0xFF) ? (*plArgs) : (_RFL_INIT_COMMA);
                plArgs++;
                lpszFmt++;
            } else if (_T('0') <= *lpszFmt && *lpszFmt <= _T('9')) {
                sComma = 0;                        /* initialize        */
                while (_T('0') <= *lpszFmt && *lpszFmt <= _T('9')) {
                    sComma = sComma * 10 + *lpszFmt++ - 0x30;
                }
            } else {
                sComma = _RFL_INIT_COMMA;
            }
        } else {                                    /* use defaule value */
            sComma = _RflEdit.sComma;
        }

        /* ---------- type prefix ---------- */
		uchMask = 0;  // init the mask indicator to show no masking.

        if (*lpszFmt == _T('l')) {
            usLong = 1;
            lpszFmt++;
        } else if (*lpszFmt == _T('L')) {
            usLong = 2;
            lpszFmt++;
        } else if (*lpszFmt == _T('Q')) {    // format for DCURRENCY amount which depends on DCURRENCY setting
#if defined(DCURRENCY_LONGLONG)
            usLong = 2;             // DCURRENCY is a long long
#else
            usLong = 1;             // DCURRENCY is a long
#endif
            lpszFmt++;
        } else if (*lpszFmt == _T('M')) {
			// flag used with RflTruncateEmployeeNumber() for masking employee id
			// should only be used for employee ids of 8 digits or less.
            usLong = 2;
			uchMask = 1;
            lpszFmt++;
        } else if (*lpszFmt == _T('N')) {
            usLong = 1;
            lpszFmt++;
        } else if (*lpszFmt == _T('F')) {
            usLong = 0;
            lpszFmt++;
        } else {
            usLong = 0;
        }

        if ((uchForm = *lpszFmt++) == _T('\0')) {
            if (usPos >= usLength) {		//usPos is 0 based while usLength is 1 based
                PifLog(MODULE_RFL_FORMSTR, FAULT_BUFFER_OVERFLOW);
                PifLog(MODULE_DATA_VALUE(MODULE_RFL_FORMSTR), usPos);
                PifLog(MODULE_DATA_VALUE(MODULE_RFL_FORMSTR), usLength);
                PifLog(MODULE_LINE_NO(MODULE_RFL_FORMSTR), (USHORT)__LINE__);
#if defined(RflSPrintf) || defined(_RflFormStr)
				usPos = usLength - 1;
				*(pszUser + usPos) = _T('\0');
				return 0x7fff;
#else
                PifAbort(MODULE_RFL_FORMSTR, FAULT_BUFFER_OVERFLOW);
#endif
            }
            *(pszUser + usPos) = _T('\0');
            return (usPos);
        }

        /* --- initialize --- */
        pszBuf  = szBuf;
        uchSign = _T('\0');

        /* ---------- format type ---------- */
        switch (uchForm) {
        case _T('c'):                               /* single character */
            *pszBuf       = (TCHAR)*plArgs++;
            *(pszBuf + 1) = _T('\0');
            uchFill       = _T(' ');
            sFmax         = 0;
            break;

        case _T('s'):                               /* string */
            pszBuf  = *(TCHAR **)plArgs;
            plArgs  = (LONG *)((TCHAR **)plArgs + 1);
            uchFill = _T(' ');
            break;

        case _T('d'):                               /* singed decimal */
            if (usLong) {                       /* long  */
                _ltot(*(LONG *)plArgs, pszBuf, 10);
                if (*(LONG *)plArgs < 0) {      /* minus */
                    uchSign = _T('-');              /* set sign character  */
                    pszBuf++;                   /* skip sign character */
                }
                plArgs = ((LONG *)plArgs + 1);
            } else {                            /* short */
                _itot(*plArgs, pszBuf, 10);
                if (*plArgs < 0) {              /* minus */
                    uchSign = _T('-');              /* set sign character  */
                    pszBuf++;                   /* skip sign character */
                }
                plArgs++;
            }
            sFmax = 0;
            break;

        case _T('u'):                               /* unsigned deciaml integer */
            if (usLong) {                       /* long  */
				ULONG  ulValue = *(ULONG *)plArgs;

				if (uchMask == 1) {  // if Masking format indicated, check most significant bit to determine if masking should be done.
					if (ulValue & 0x80000000) { // Is Mask Flag set by RflTruncateEmployeeNumber().
						ulValue = 0;
					} else {
						uchMask = 2;
					}
					ulValue &= 0x7fffffff;     // Ensure that the masking bit possibley set by RflTruncateEmployeeNumber(), if set, is removed.
				}

                _ultot(ulValue, pszBuf, 10);
                plArgs = ((ULONG *)plArgs + 1);
            } else {                            /* short */
				if (*(USHORT *)plArgs != 0 && uchMask == 1)  // if Masking is on but value is non-zero then do not mask.
					uchMask = 0;

                _ultot((ULONG)(*(USHORT *)plArgs), pszBuf, 10);
                plArgs++;
            }
            sFmax = 0;
            break;

        case _T('X'):                               /* unsigned hex integer */
        case _T('x'):                               /* unsigned hex integer */
            if (usLong) {                       /* long  */
                _ultot(*(ULONG *)plArgs, pszBuf, 16);
                plArgs = ((ULONG *)plArgs + 1);
            } else {                            /* short */
                _ultot((ULONG)(*(USHORT *)plArgs), pszBuf, 16);
                plArgs++;
            }
            if (uchForm == _T('X')) {
                pszBuf = _tcsupr(pszBuf);
            }
            sFmax = 0;
            break;

        case _T('p'):                               /* pointer */
            if (usLong == 0) {                  /* far pointer  */
                tcharnset(pszBuf, _T('0'), 9);         /* fill '0'     */
                _ultot((ULONG)FP_SEG(*plArgs), szWork, 16);
                _tcsncpy(pszBuf + 4 - _tcslen(szWork), _tcsupr(szWork), _tcslen(szWork));
                *(pszBuf + 4) = _T(':');
                _ultot((ULONG)FP_OFF(*plArgs), szWork, 16);
                _tcscpy(pszBuf + 5 + 4 - _tcslen(szWork), _tcsupr(szWork));
                plArgs++;
            } else {                            /* near pointer */
                tcharnset(pszBuf, _T('0'), 4);
                _ultot((ULONG)FP_OFF(*plArgs), szWork, 16);
                _tcscpy(pszBuf + 4 - _tcslen(szWork), _tcsupr(szWork));
            }
            plArgs++;
            break;

        case _T('$'):                               /* signed amount integer */
            if (fsDecimal) {
                sDecimal = (sFmax != 0xFF) ? (sFmax) : (_RFL_INIT_DECIMAL);
            } else {
                sDecimal = _RflEdit.sDecimal;
            }
            sFmax = 0;                          /* reset */

#if defined(DCURRENCY_LONGLONG)
            if (usLong == 1) usLong = 2;        // if signed amount currency, DCURRENCY is a long long
#endif
            if (usLong == 2) {                  /* long long with 13 digits max, D13DIGITS */
                d13ArgVal = *(D13DIGITS *)plArgs;
                if (d13ArgVal < 0) {            /* minus */
                    uchSign   = _T('-');            /* set minus flag */
                    d13ArgVal = -d13ArgVal;     /* reset minus    */
                }
                RflD13ToDecimalStr(&d13ArgVal, pszBuf);
                /* do not call pifabort, becaues D13DIGITS is changed to __int64, 08/14/01 */
                /* if (strlen(pszBuf) > 13)
                    PifAbort(MODULE_RFL_FORMSTR, FAULT_INVALID_DATA); */
                plArgs = (LONG *)((D13DIGITS *)plArgs + 1);
            } else if (usLong == 1) {           /* long with 9 digits max, D9DIGITS */
                lLow = *(LONG *)plArgs;
                if (*(LONG *)plArgs < 0) {      /* minus */
                    uchSign = '-';              /* set minus flag */
                    lLow    = -lLow;            /* reset minus    */
                }
                _ltot(lLow, pszBuf, 10);
                plArgs = (LONG *)((LONG *)plArgs + 1);
            } else {                            /* short with 4 digits max, D4DIGITS */
                sHigh = (SHORT)*plArgs;
                if (*plArgs < 0) {              /* minus */
                    uchSign = _T('-');              /* set minus flag */
                    sHigh   = -sHigh;           /* reset minus    */
                }
                _itot(sHigh, pszBuf, 10);
                plArgs++;
            }

            RflCommaDecimal(pszBuf, sComma, sDecimal);
            break;

        default :                               /* unsupported */
            *(pszUser + usPos++) = uchForm;
            break;
        }

        sLength = tcharlen(pszBuf);

        if (sFmin > _RFL_LEN_BUF || sFmin < 0) {
            sFmin = 0;
        }

        if (sFmax > _RFL_LEN_BUF || sFmax < 0) {
            sFmax = 0;
        }

        sLeading = 0;

        if (sFmax != 0 || sFmin != 0) {
            if (sFmax != 0) {
                if (sLength > sFmax) {
                    sLength = sFmax;
                }
            }

            if (sFmin != 0) {
                sLeading = sFmin - sLength;
            }

            if (uchSign == _T('-')) {
                sLeading--;
            }
        }

        if (uchSign == _T('-') && uchFill == _T('0')) {
            *(pszUser + usPos++) = uchSign;
        }

        if (! fsLeftJust) {                 /* right just */
			TCHAR  uchFillChar = uchFill;

			if (uchMask == 1)
				uchFillChar = _T('X');
			else if (uchMask == 2)
				uchFillChar = _T('0');

            for (sCount = 0; sCount < sLeading && usPos < usLength; sCount++) {
                *(pszUser + usPos++) = uchFillChar;
            }
        }

		if (uchMask == 1) {
			int iCount;

			// if we are masking then mask out leading spaces and zero digits.
			for (iCount = 0; pszBuf[iCount]; iCount++) {
				if (pszBuf[iCount] == _T(' ') || pszBuf[iCount] == _T('0')) {
					pszBuf[iCount] = _T('X');
				} else {
					break;
				}
			}
		}

        if (uchSign == _T('-') && uchFill == _T(' ')) {
            *(pszUser + usPos++) = uchSign;
        }

        for (sCount = 0 ; sCount < sLength && usPos < usLength; sCount++) {
            *(pszUser + usPos++) = *(pszBuf + sCount);
        }

        if (fsLeftJust) {                   /* left just */
            for (sCount = 0; sCount < sLeading && usPos < usLength; sCount++)   {
                *(pszUser + usPos++) = _T(' ');
            }
        }
    }
}

/*
*===========================================================================
** Synopsis:    static VOID RflCommaDecimal(UCHAR *pszStr, SHORT sComma,
*                                           SHORT sDecimal);
*     InOut:    lpszStr  -
*     Input:    sDecimal -
*               sComma   -
*
** Return:      nothing
*===========================================================================
*/
static VOID RflCommaDecimal(TCHAR *pszStr, SHORT sComma, SHORT sDecimal)
{
    SHORT   sOffset, sCount, sBuf = 0;
    USHORT  fComma   = (sComma   != _RFL_INIT_COMMA)   ? (TRUE) : (FALSE);
    USHORT  fDecimal = (sDecimal != _RFL_INIT_DECIMAL) ? (TRUE) : (FALSE);
    TCHAR   szBuf[_RFL_LEN_BUF];

    if (fComma) {
        if (fDecimal) {
            sCount = 0 - sDecimal;
        } else {
            sCount = 0;
        }
    }

    for (sOffset = tcharlen(pszStr) - 1; sOffset >= 0; sOffset--) {
        if (fDecimal && sBuf == sDecimal) {
			 //ESMITH CURRENCY
			if (_RflEdit.uchDecimal == _RFL_USE_DECIMAL) {
				szBuf[sBuf++] = _T('.');
			} else {
			  szBuf[sBuf++] = _T(',');
			}
        }
        if (fComma) {
            if (sCount == sComma) {
                if (*(pszStr + sOffset) != _T('-')) {
                    szBuf[sBuf++] = _T(',');
                    sCount = 1;
                }
            } else {
                sCount++;
            }
        }
        szBuf[sBuf++] = *(pszStr + sOffset);
    }

    /* --- check "0.00" format --- */

    if (fDecimal) {
        while (sBuf <= sDecimal) {
            if (sBuf == sDecimal) {
				 //ESMITH CURRENCY
				if (_RflEdit.uchDecimal == _RFL_USE_DECIMAL) {
					szBuf[sBuf++] = _T('.');
				} else {
				  szBuf[sBuf++] = _T(',');
				}
                szBuf[sBuf++] = _T('0');
                break;
            }
            szBuf[sBuf++] = _T('0');
        }
    }

    szBuf[sBuf] = _T('\0');

    _tcsrev(szBuf);
    _tcscpy(pszStr, szBuf);
}

/*
*===========================================================================
** Synopsis:    CHAR *RflD13ToDecimalStr(D13DIGITS *d13Value, CHAR *pszBuf)
*
*     Input:    d13Value   - number to be converted
*    Output:    pszBuf     - destination buffer
*
**   Return:    Pointer to string. No error return.
*
** Description: This function convert the digits of the given D13Value
*               argument to a null-terminated character string and stores
*               the result in pszBuf. Radix is fixed 10. If d13Value is
*               negative, the first character of the stored string is the
*               minus sign '-'.
*===========================================================================
*/
TCHAR *RflD13ToDecimalStr(D13DIGITS *pd13Value, TCHAR *pszBuf)
{
    TCHAR *p;         /* pointer to traverse along string */
    TCHAR *first;     /* pointer to first digit */
    TCHAR tmp;        /* temporary character */
    ULONG digit;     /* value of digit */
    D13DIGITS d13Value = *pd13Value;

    p = pszBuf;

    if (d13Value < 0) {
        *p = _T('-');
        p++;
        d13Value = -d13Value;   /* make it absolute value*/
    }

    first = p;          /* save first digit */

    do {
        digit = (ULONG)(d13Value % 10);      /* get lowest digit */
        d13Value /= 10;                 /* shift right decimally */
        *p = (TCHAR)(digit + _T('0'));       /* convert to ascii and store */
        p++;
    } while (d13Value > 0);

    *p = _T('\0');            /* terminate string */

    /* reverse numeric string */
    p--;        /* p points to last digit */
    do {
        tmp = *p;
        *p = *first;
        *first = tmp;           /* swap *p and *firstdig */
        --p;
        ++first;                /* advance to next two digits */
    } while (first < p);        /* repeat until halfway */

    return pszBuf;
}
/* ====================================== */
/* ========== End of RFLFORM.C ========== */
/* ====================================== */
