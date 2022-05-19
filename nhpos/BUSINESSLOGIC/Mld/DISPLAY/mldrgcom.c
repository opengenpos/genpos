/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*
*  NeighborhoodPOS Software Development Project
*  Department of Information Systems
*  College of Information Technology
*  Georgia Southern University
*  Statesboro, Georgia
*
*  Copyright 2014 Georgia Southern University Research and Services Foundation
* 
*===========================================================================
* Title       : Multiline Display common routine, reg mode
* Category    : Multiline Display
* Program Name: MLDRGCOM.C
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*    MldVoidOffline() : set 'VOID' and 'OFFL'
*    MldNumber() : set number
*    MldMnemAmt() : set transaction mnemonics and amount
*    MldDiscount() : set transaction mnemonics, discount rate, and amount
*    MldTaxMod() : set tax modifier mnemonics
*    MldWeight() : set weight
*    MldMnlWeight() : set "manual" mnemonic
*    MldTranMnem() : set transaction mnemonics
*    MldChild() : set child plu
*    MldForeign() : set foeign tender amount and conversion rate
*    MldDoubleMnemAmt() : set trans. mnemonic(double), and amount
*    MldChargeTips() : set waiter mnem. and ID, charge tips mnem., rate, and amount
*    MldTrayCount(): set tray total mnem. and counter
*    MldItems() : set adj., noun, print mod., cond., amount/ rate and amount
*    MldCPRoomCharge() : set room no. and guest id
*    MldCPRspMsgText() : set response message text
*    MldOffline()      : set EPT offline symbol & expiration date
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Mar-15-95 : 03.00.00 :  M.Ozawa   : Initial
* Aug-11-99 : 03.05.00 :  M.Teraki  : Remove WAITER_MODEL
* Aug-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL
*           :          :            :       /GUEST_CHECK_MODEL
*
* Aug-26-99 : 01.00.00  : M.Teraki  : initial (for Win32)
* Dec-15-15 : 02.02.01 : R.Chambers : replace MLD_ decimal type defines with standard PRT_ defines.
* Jul-21-21 : 02.04.01 : R.Chambers : USHORT for usAdr special mnemonic argument MldForeign() 
* Jul-21-21 : 02.04.01 : R.Chambers : replace _tcslen() with tcharlen() to eliminate compiler warnings.
* Jul-21-21 : 02.04.01 : R.Chambers : MldGetTaxMod() destination length now in character count not bytes
*===========================================================================
*/

/**
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- M S - C -------**/
#include <tchar.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <uie.h>
#include <fsc.h>
#include <rfl.h>
#include <mld.h>
#include <pmg.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"
#include <item.h>

/**
;========================================================================
;+             P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/

static USHORT  MldDriveTaxMod(TCHAR *pszWork, USHORT fsTax, USHORT fbMod);
static USHORT  MldPrechkForeign(TCHAR *pszWork, DCURRENCY lForeign, USHORT usAdr, UCHAR fbStatus, ULONG ulRate, UCHAR fbStatus2);
static USHORT  MldDriveForeign(TCHAR *pszWork, DCURRENCY lForeign, USHORT usAdr, UCHAR fbStatus, ULONG ulRate, UCHAR fbStatus2);
static USHORT  MldPrechkItems(TCHAR  *pszWork, ITEMSALES  *pItem, USHORT usPrice, LONG *plAttrib, USHORT usDispType); //US Customs
static USHORT  MldDriveItems(TCHAR  *pszWork, ITEMSALES  *pItem, LONG *plAttrib);
static USHORT  MldListItems(TCHAR  *pszWork, ITEMSALES  *pItem, LONG *plAttrib, USHORT usPrice);
static USHORT  MldPrechkAtFor(TCHAR  *pszWork, ITEMSALES  *pItem, USHORT usPrice, LONG *plAttrib, USHORT usDispType);//US Customs
static USHORT  MldDriveAtFor(TCHAR  *pszWork, ITEMSALES  *pItem, LONG *plAttrib);
static SHORT   MldGetTaxMod(TCHAR *pszDest, USHORT usDestLen, SHORT fsTax, USHORT fbMod);
static VOID    MldDouble(TCHAR *pszDest, USHORT usDestLen, TCHAR *pszSource);

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
CONST TCHAR   aszMldQty1[] = _T("%ld");                    /* quantity */
CONST TCHAR   aszMldQty2[] = _T("%-ld ");                  /* quantity */
CONST TCHAR   aszMldMnemAmt[] = _T("%-s \t%l$");           /* trans. mnem and amount */
CONST TCHAR   aszMldMnemAmtShift[] = _T("%-s \t%l$ ");     /* trans. mnem and amount, V3.3 */
CONST TCHAR   aszMldMnemNum[] = _T("%-s %-s");             /* trans. mnem and amount */
CONST TCHAR   aszMldDiscount[] = _T("%-8s %3d%%\t%9l$");        /* trans. mnem, rate, and amount */
CONST TCHAR   aszMldDiscount2[] = _T("%-8s\t%3d%%\n\t%9l$");    /* trans. mnem, rate, and amount */
CONST TCHAR   aszMldSeatMnemAmt[] = _T("   s%.1d %-s \t%l$");   //SR206   trans. mnem and amount */
CONST TCHAR   aszMldSeatDiscount[] = _T("   s%.1d %-8s%3d%%\t%l$"); //SR206   trans. mnem, rate, and amount */
CONST TCHAR   aszMldTaxMod[] = _T("   %s");              /* tax modifier mnemonics */
CONST TCHAR   aszMldDrvTaxMod1[] = _T("%s%s%s");         /* tax modifier mnemonics */
CONST TCHAR   aszMldDrvTaxMod2[] = _T("\t%s %s %s");     /* tax modifier mnemonics */
CONST TCHAR   aszMldDrvTaxMod3[] = _T("   %s%s%s");      /* tax modifier mnemonics */
CONST TCHAR   aszMldDrvFSMod[] = _T("\t%s");             /* FS modifier mnemonics */

CONST TCHAR   aszMldTranMnem[] = _T("   %-8s");          /* transaction mnemonics */

CONST TCHAR   aszMldGCNo[] = _T("   %-8s %4s");         /* check paid mnemonic and guest check No. */
CONST TCHAR   aszMldGCNoCdv[] = _T("   %-8s %4s%02d");  /* check paid mnemonic and guest check No. (with Cdv) */
CONST TCHAR   aszMldTrayCo[] = _T("%-8s \t%6d");        /* tray total co. mnemonic and counter */

CONST TCHAR   aszMldForeign1[] = _T("%.*l$");           /* foreign amount and conversion rate */
CONST TCHAR   aszMldForeign2[] = _T("   %-s");          /* foreign amount and conversion rate */
CONST TCHAR   aszMldForeign3[] = _T("   / %10.*l$");    /* foreign amount and conversion rate */
CONST TCHAR   aszMldForeign4[] = _T("/%10.*l$");        /* foreign amount and conversion rate */
CONST TCHAR   aszMldForeign5[] = _T("%-s\t%s");         /* foreign currency mnemonic and amount, no conversion rate */

CONST TCHAR   aszMldMnemAmtD[] = _T("%-s \t%l$");        /* transaction mnem, and native mnem and amount */
CONST TCHAR   aszMldItemAmt[] = _T("\t%9s ");            /* amount */
CONST TCHAR   aszMldModItemAmt[] = _T("\t%9s ");         /* amount */
extern CONST TCHAR   aszItemAmt[];                       /* amount */
CONST TCHAR   aszMldVoidOffline[] = _T("   %-4s %-4s");  /* void, offline */

CONST TCHAR   aszMldCPRoomCharge[] = _T("   %-4s %5s  %-4s %2s");        /* room charge */
CONST TCHAR   aszMldCPRoomCharge2[] = _T("  %-4s %5s  %-4s    %2s");     /* room charge */
/*****
CONST UCHAR FARCONST  aszMldFolioPost[] = "    %-4s %6s %-4s %6s";
                                                 / folio, posted transaction no. /
CONST UCHAR FARCONST  aszMldFolioPost2[] = " %-4s %6s %-4s %6s";
                                                 / folio, posted transaction no. /
*****/
CONST TCHAR   aszMldOffline[] = _T("   %5s   %6s");     /* mnemo.  date */
CONST TCHAR   aszMldOffline2[] = _T("%5s %6s");         /* mnemo.  date */
CONST TCHAR   aszMldOffDate[] = _T("%02d/%02d");        /* expiration date  */
CONST TCHAR   aszMldDriveRspMsg[] = _T("%-s \t%s");     //Response message printing

//============================================================================
//
//  Special Functions following.

//  The following special functions for counting double wide characters are
//  needed to work around an issue with the displaying of Chinese characters.
//  This issue is due to the font selection procedure used in FrameWorkText.cpp
//  and the way the font is manipulated to resemble the display of the older
//  NCR 7448 terminal.

//  This display issue needs further work to allow the receipt text windows to
//  be more compatible with Windows GUI standards.

/*
*===========================================================================
** Format   : USHORT CountDblWidthChar( TCHAR *pszCount)
*
*   Input   : TCHAR		*pszCount   - address of source string to count
*
** Return   : USHORT usDblChar	- Number of double width characters
*
** Synopsis : This function counts the number of double width characters
			in a string.  The current range the function counts as double width
			is 0x4E00 to 0x9FA5 this range corresponds to simplified chinese
			any other ranges may be added as this is not the only language, which
			has characters that print twice the size of the standard 
			ANSI set make note of the range and the language/characters to which
			it corresponds
*===========================================================================
*/
static USHORT CountDblWidthChar( TCHAR *pszCount)
{
	USHORT usDblChar = 0;
	TCHAR* pszStart = pszCount;

	for(; *(pszStart) != 0; pszStart++){
		if(*(pszStart) >= 0x4E00 && *(pszStart) <= 0x9FA5){
			usDblChar++;
		}
	}
	return usDblChar;
}

static LONG  DetermineQuantity (ITEMSALES  *pItem)
{
	LONG  lQTY = 0;
	ULONG flPrintStatus = 0;

	/*
		set qty depending on whether we are doing
		void consolidation or not.  void consolidation will
		display only the remaining amount without voids
		being displayed along with original amount entered.
	**/
    if (MldChkVoidConsolidation(pItem))
	{
        lQTY = (LONG)pItem->sCouponQTY;
    }
	else if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) != 0)
	{
		lQTY = pItem->lQTY;
    }
	else
	{
        lQTY = pItem->lQTY/1000L;
    }

	return lQTY;
}

static DCURRENCY  DeterminePrice (ITEMSALES  *pItem, LONG lQTY)
{
#if 1
	// The following function calculates the base price and
	// includes PLU Price Multiplier in the logic as well as
	// what was in this function before.
	// We are working towards a general purpose price calculation
	// function that will take the item as input and provide an
	// item price as output.
	return ItemSalesCalcDetermineBasePrice (pItem, lQTY);
#else
	DCURRENCY  lAllPrice = 0;

	if (pItem->uchPPICode == 0) {
		if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) != 0) {
			lAllPrice = pItem->lProduct;
		} else if( pItem->usFor != 0 && pItem->usForQty !=0 ) {
			lAllPrice = pItem->lProduct;
		} else {
			lAllPrice = pItem->lUnitPrice * lQTY;
		}
	} else {
		lAllPrice = pItem->lProduct;
	}

	return lAllPrice;
#endif
}

/*
*===========================================================================
** Format   : USHORT CountDblWidthChar( TCHAR *pszCount)
*
*   Input   : TCHAR		*pszCount   - address of source string to count
*             USHORT    usMaxLength - max displayable character length
*
** Return   : USHORT usDblChar	- Number of display width characters
*
** Synopsis : This function counts the number of display width characters
			in a string.  Display width is one per character if the character
			is a standard European character.  However some Asian character
			sets have characters which are actually double the width of a
			standard European character.
			
			The current range the function counts as double width
			is 0x4E00 to 0x9FA5 this range corresponds to Simplified Chinese.
			Other ranges may be added as this is not the only language, which
			has characters that print twice the size of the standard 
			ANSI set make note of the range and the language/characters to which
			it corresponds
*===========================================================================
*/
static USHORT CountDisplayWidthChar( TCHAR *pszCount, USHORT usMaxLength)
{
	USHORT usDblChar = 0, usCharCount = 0;

	for(; *pszCount != 0 && usDblChar < usMaxLength; pszCount++){
		usDblChar++; usCharCount++;
		if(*pszCount >= 0x4E00 && *pszCount <= 0x9FA5){
			usDblChar++;
		}
	}

	return usCharCount;
}

static USHORT CountPossibleDisplayWidthChar( TCHAR *pszCount, USHORT usMaxLength)
{
	USHORT usDblChar = 0, usCharCount = 0;

	for(; *pszCount != 0 && usDblChar < usMaxLength; pszCount++){
		usDblChar++; usCharCount++;
		if(*pszCount >= 0x4E00 && *pszCount <= 0x9FA5){
			usDblChar++;
		}
	}

	return usDblChar;
}

//  -----  End Special Functions  ---------------


/*
 *   WARNING:  condiment prices for condiments that are PPI PLUs are updated by using
 *             function ItemSalesCalcCondimentPPIReplace() so the condiment prices
 *			   in ITEMSALES pItem are updated if there are PPI PLUs specified in the
 *			   item data.
*/
static USHORT  MldBuildDisplayItemLine(TCHAR  *pszWork, ITEMSALES  *pItem, USHORT usPrice, LONG *plAttrib,
						USHORT usLineLength, USHORT usLineIdent, USHORT usMaxPluCondLen)
{
    TCHAR       *pColWork;
    USHORT      len = 0, len2 = 0;
    LONG        lQTY, lAttrib, lDefAttrib;
	DCURRENCY   lAllPrice = 0;
    BOOL        fIsDeptItem;
    USHORT      usSetLine = 0, usSetQty = 0;
    USHORT      usMaxChild;
    USHORT      i;
    USHORT      usPriority = 0, usPrintMod;
    TCHAR       szColWork[64] = {0};
    TCHAR       szModCond[32] = {0};

    /* ---- check mdc of print modifier & condiment plu priority --- */
    if ( CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT0 ) &&
         CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT2 ))
	{
         usPriority = 1;
    }

    /* ---- check mdc of reverse video of condiment & print modifier --- */
    if ((CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT2)) )
	{
        lDefAttrib = MLD_DEFAULT_REVERSE;
    }
	else
	{
        lDefAttrib = 0L;
    }
    lAttrib = lDefAttrib;

	lQTY = DetermineQuantity (pItem);

	// The first piece of information in the displayed item is either its quantity or
	// if it is an @/For item meaning that it is something like 4 items for a $1.00 then
	// instead of the quantity, we will display the @/For information.
	memset (szColWork, 0, sizeof(szColWork));
    tcharnset(szColWork, _T(' '), usLineLength);
	pColWork = &szColWork[usLineIdent];

	if(pItem->usFor)
	{
		TCHAR       *pFmt = NULL;
		USHORT      usDivisor = 1;
		PARAMDC		ParaMDC = {0};
		TCHAR       aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };

		// set "total qty @ package qty" in mnemonic"

		/* --- get transaction mnemonics --- */
		RflGetTranMnem(aszTranMnem, TRN_FOR_ADR);

		//set package price after retrieving decimal settings
		ParaMDC.uchMajorClass = CLASS_PARAMDC;
		ParaMDC.usAddress = MDC_SYSTEM3_ADR;
		ParaMDCRead(&ParaMDC);

		// Assume no decimal system so that something will print.
		pFmt = _T("%d @ %d %s %d");
		usDivisor = 1;

		if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x00)
		{
			// 2 decimal system
			pFmt = _T("%d @ %d %s %d.%.2d");
			usDivisor = 100;
		}
		if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x01)
		{
			// 3 decimal system
			pFmt = _T("%d @ %d %s %d.%.3d");
			usDivisor = 1000;
		}

		// this works because _stprintf is driven by the format string and if the
		// format string has only the single %d, only one of these arguments will
		// be used.  If the format string has both %d items, both arguments will be used.
		_stprintf (szColWork, pFmt, lQTY, pItem->usForQty, aszTranMnem, pItem->usFor / usDivisor, pItem->usFor % usDivisor);

		// Now we have to see if everything will fit on the same line.  If not,
		// then we will need to adjust the output to go to the next line.
		len = CountDisplayWidthChar (szColWork, usLineLength);
		if (tcharlen(szColWork) > len)
		{
			if (1 > len) len = 1;  // just in case len is zero <grin>
			for (i = len - 1; i > 0; i--)
			{
				if (szColWork[i] == _T(' '))
				{
					// Now move the working line buffer contents to the user's scroll buffer
					// and advance to the next line.  Reset our working buffer for the next line.
					tcharnset(pszWork, _T(' '), usLineLength);
					_tcsncpy( pszWork, &szColWork[0], i);
					pszWork += usLineLength + 1;
					usSetLine ++;
					plAttrib++;             /* feed reverse video control flag */
					lAttrib = lDefAttrib;   /* set default reverse video position */
					//tcharnset(szColWork, _T(' '), usLineLength);
					pColWork = &szColWork[usLineIdent];

					// we have output the first part of the string, now output the second
					// part.  We have split the line at a space.
					tcharnset(szColWork, _T(' '), usLineIdent);
					for (; szColWork[i]; i++)
					{
						if (szColWork[i] != _T(' '))
						{
							for ( ; szColWork[i]; i++)
							{
								*pColWork++ = szColWork[i];
							}
							*pColWork = _T('\0');
							break;
						}
					}
					break;
				}
			}
		}

		// Now move the working line buffer contents to the user's scroll buffer
		// and advance to the next line.  Reset our working buffer for the next line.
		tcharnset(pszWork, _T(' '), usLineLength);
        _tcsncpy( pszWork, &szColWork[0], _tcslen(szColWork));
        pszWork += usLineLength + 1;
        usSetLine ++;
		// Since this is a continuation line, we will need to indicate
		// this by using the higher order byte and incrementing the continuation
		// line count that is stored there.  See usage of MLD_LINESTATUS_CONTLINE
		// in MldWriteScrollDisplay1() and the other MldWriteScollDisplay functions.
		MLD_SETLINE_COUNTINCREMENT(usSetLine);
        plAttrib++;             /* feed reverse video control flag */
        lAttrib = lDefAttrib;   /* set default reverse video position */
        tcharnset(szColWork, _T(' '), usLineLength);
		pColWork = &szColWork[usLineIdent];
	}
	else
	{
		/* set over 1000 qty case after item consolidation case */
		if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE)
		{
			usSetQty = 1;
		}
		else if ((lQTY > 999L) || (lQTY < -99L))
		{
			TCHAR       szQty100Work[16] = {0};

			//======Begin Quantity Fix, JHHJ 5-24-04======//
			tcharnset(pszWork, _T(' '), usLineLength);/* Set buffer to all spaces*/
			RflSPrintf(szQty100Work, TCHARSIZEOF(szQty100Work), aszMldQty2, lQTY); 
			/* set the formatted string to the buffer to the correct format with the quantity*/																								
			len = tcharlen(szQty100Work);
			/*get the length of the actual buffer, and takes out the null character*/
			_tcsncpy(pszWork, &szQty100Work[0], len);
			/*copy the correct quantity to the buffer WITHOUT the null character, which was causing the string
			to be terminated too early*/
			pszWork += usLineLength + 1;
			usSetLine++;
			MLD_SETLINE_COUNTINCREMENT(usSetLine);
			plAttrib++;             // feed reverse video control flag to next line
			lAttrib = lDefAttrib;   // reset reverse video character position
			usSetQty = 1;
		}

		/* --- set qty, adjective and noun plu name --- */
		tcharnset(szColWork, _T(' '), usLineLength);
		pColWork = &szColWork[usLineIdent];
		len2 = 0;

		/*** set qty ***/
		if ((!usSetQty) && (pItem->uchMinorClass != CLASS_ITEMORDERDEC))
		{
			TCHAR       szQtyWork[16] = {0};

			RflSPrintf(szQtyWork, TCHARSIZEOF(szQtyWork), aszMldQty1, lQTY);
			len = tcharlen(szQtyWork);
			switch(len) {
				case 1:     /* 1 digit */
					_tcsncpy(&(szColWork[1]), szQtyWork, 1);
					break;

				case 2:     /* 2 digits or minus 1 digit */
					_tcsncpy(szColWork, szQtyWork, 2);
					break;

				case 3:     /* 3 digits or minus 2 digits */
					_tcsncpy(szColWork, szQtyWork, len);
					break;

				default:    /* invalid qty */
					break;
			}
		}
		else
		{
			len = MLD_QTY_LEN;
		}
	}

	// Insert the seat number, if there is one, into the display buffer
	// The seat number is either one or two digits with the letter s prepended.
    if ((pItem->uchSeatNo) && (pItem->uchMinorClass != CLASS_ITEMORDERDEC))
	{
		/*
			set seat no if entred R3.1 and also fix for SR 206
			to allow 2 digit seat numbers.
		*/
        if (usSetQty)
		{
			// if scalable or 4 digit qty then put seat number in qty area.
			pColWork = szColWork;
		}

		*pColWork = _T('s'); pColWork++;
		lAttrib <<= 1;

		// Convert the seat number from a binary number to a character string.
		// We divide by 10 since seat number is base 10 and add character zero.
		if (pItem->uchSeatNo < 10)
		{
			// Single digit for the seat number
			*pColWork = pItem->uchSeatNo % 10 + _T('0');
			pColWork++;
			lAttrib <<= 1;
		}
		else
		{
			// Two digits for the seat number
			*pColWork = pItem->uchSeatNo % 10 + _T('0');
			pColWork++;
			lAttrib <<= 1;
			*pColWork = pItem->uchSeatNo / 10 + _T('0');
			pColWork++;
			lAttrib <<= 1;
		}
		// add a space after the seat number
		pColWork++;
		lAttrib <<= 1;

		if (! usSetQty) {
			// Now move the working line buffer contents to the user's scroll buffer
			// and advance to the next line.  Reset our working buffer for the next line.
            _tcsncpy( pszWork, &szColWork[0], usLineLength);
            pszWork += usLineLength + 1;
            usSetLine ++;
			MLD_SETLINE_COUNTINCREMENT(usSetLine);
            plAttrib++;             /* feed reverse video control flag */
            lAttrib = lDefAttrib;   /* set default reverse video position */
            tcharnset(szColWork, _T(' '), usLineLength);
			pColWork = &szColWork[usLineIdent];
        }
    }

	// We first of all will create a display line or lines that will contain the
	// adjective, if there is an adjective, and the PLU mnemonic.  If both the
	// adjective and the mnemonic fit on the same line, then we put them together.
	// If the mnemonic is sufficiently long that both won't fit on the same line,
	// then we will use two lines, one for adjective then one for PLU mnemonic.

	// We have a standard procedure of put line contents into the line working buffer.
	// If the line working buffer is filled, then copy it into the scroll buffer provided
	// by the caller, increment to the next line in the scroll buffer, then clear the
	// line working buffer and reset the character position pointer of the line buffer.

	len2 = usLineIdent + 1;  // we indent the displayed line by 4 characters.

    fIsDeptItem = (( pItem->uchMinorClass == CLASS_DEPT ) ||
                   ( pItem->uchMinorClass == CLASS_DEPTITEMDISC ) ||
                   ( pItem->uchMinorClass == CLASS_DEPTMODDISC ));

    if ((_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) < 0) ||
        (_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) > 0))
	{
		len = CountDisplayWidthChar(pItem->aszMnemonic, usMaxPluCondLen);

        if (( pItem->uchAdjective ) && ( ! fIsDeptItem ) && pItem->uchMinorClass != CLASS_ITEMORDERDEC)
		{
			size_t  lenadj;
			TCHAR   szAdjWork[PARA_ADJMNEMO_LEN + 1] = {0};

            // Prepare adjective mnemonic, if main item is adjective.
			// Count the number of double wide characters in the adj
			// and put the adjective into the line working buffer.
			// Then increment the reverse video control flag for not reverse video.
            RflGetAdj (szAdjWork, pItem->uchAdjective);
			lenadj = CountDisplayWidthChar(szAdjWork, usMaxPluCondLen);
            _tcsncpy (pColWork, szAdjWork, lenadj);
            pColWork += lenadj + 1;

            for(i = 0; i < lenadj; i++) lAttrib <<= 1;

			//if the length of the main item + the adj is less than the width of the line
			//then copy that length
            if ((pColWork - szColWork) + len + 1 > usLineLength)
			{
				// Now move the working line buffer contents to the user's scroll buffer
				// and advance to the next line.  Reset our working buffer for the next line.
                _tcsncpy( pszWork, &szColWork[0], usLineLength);
                pszWork += usLineLength + 1;
                usSetLine ++;
				MLD_SETLINE_COUNTINCREMENT(usSetLine);
                plAttrib += (usLineLength + 1);             /* feed reverse video control flag */
                lAttrib = lDefAttrib;      // reset reverse video character position
                tcharnset(szColWork, _T(' '), usLineLength);
				pColWork = &szColWork[usLineIdent];
            }
        }

#if 1
		lAllPrice = DeterminePrice (pItem, lQTY);
		lAllPrice += ItemSalesCalcCondimentPPIReplaceQty (pItem, lQTY);
#else
//		lAllPrice = pItem->lUnitPrice * lQTY;
		lAllPrice = DeterminePrice (pItem, lQTY);
#endif

		_tcsncpy( pColWork, pItem->aszMnemonic, len);

		// Setting color attribute - CSMALL
		// 'plAttrib' offset as follows: ((length of line)*(number of lines))+qtyLength)
		memset( (plAttrib+MLD_QTY_LEN), pItem->uchColorPaletteCode, (len+1)*sizeof(plAttrib));
        
		plAttrib++;             /* feed reverse video control flag */

		pColWork += len;
		len2 = pColWork - szColWork;

#if 0
		// Now move the working line buffer contents to the user's scroll buffer
		// and advance to the next line.  Reset our working buffer for the next line.
        _tcsncpy( pszWork, &szColWork[0], usLineLength);
        pszWork += usLineLength + 1;
        usSetLine ++;
        plAttrib++;             /* feed reverse video control flag */
        lAttrib = lDefAttrib;   /* set default reverse video position */
        tcharnset(szColWork, _T(' '), usLineLength);
		pColWork = &szColWork[usLineIdent];
#endif
    }

    /* --- prepare print modifiers and condiment items --- */
    usMaxChild = ( pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo );

	NHPOS_ASSERT(usMaxChild <= sizeof(pItem->Condiment)/sizeof(pItem->Condiment[0]));
	NHPOS_ASSERT(usMaxChild <= sizeof(pItem->auchPrintModifier)/sizeof(pItem->auchPrintModifier[0]));

	if (usMaxChild > sizeof(pItem->Condiment)/sizeof(pItem->Condiment[0])) {
		usMaxChild = sizeof(pItem->Condiment)/sizeof(pItem->Condiment[0]);
	}

    for (usPrintMod = 0; usPrintMod < (USHORT)(usPriority ? 2 : 1); usPrintMod++)
	{
		size_t  lenTotal;
		USHORT  usI;

		for ( usI = (USHORT)((usPriority && !usPrintMod) ? 0 : pItem->uchChildNo); usI < usMaxChild; usI++ )
		{
			/* --- display print modifier before condiment at priority option ---- */
			if ( (!usPrintMod) && (pItem->auchPrintModifier[usI]) ) {
				RflGetMnemonicByClass(CLASS_PARAPRTMODI, szModCond, pItem->auchPrintModifier[usI]);
			} else {
				if ((usPriority) && (!usPrintMod)) continue;

				/* --- prepare condiment item's mnemonic --- */
				if ( ! fIsDeptItem ) {
					if (RflIsSpecialPlu(pItem->Condiment[usI].auchPLUNo, MLD_NO_DISP_PLU_DUMMY) != 0) {
						_tcsncpy(szModCond, pItem->Condiment[usI].aszMnemonic, usMaxPluCondLen);
					} else {
						continue;
					}
				} else {
					continue;
				}
			}

			lAllPrice += (lQTY * (DCURRENCY)pItem->Condiment[usI].lUnitPrice);

			len = CountDisplayWidthChar(szModCond, usMaxPluCondLen);
			lenTotal = CountPossibleDisplayWidthChar(szModCond, usMaxPluCondLen);
			lenTotal += CountPossibleDisplayWidthChar (&szColWork[0], len2);
			if (len > 0)
			{
				if (lenTotal + len + 1 > usLineLength)
				{
					// Now move the working line buffer contents to the user's scroll buffer
					// and advance to the next line.  Reset our working buffer for the next line.
					_tcsncpy( pszWork, &szColWork[0], usLineLength);
					pszWork += usLineLength + 1;
					usSetLine ++;

					tcharnset(szColWork, _T(' '), usLineLength);
					pColWork = &szColWork[usLineIdent];
					len2 = usLineIdent + 1;
				}
				else {
					pColWork++;   // add in a space between PLU and condiment or condiments
					lAttrib <<= 1;
				}

				_tcsncpy( pColWork, szModCond, len);
				pColWork += len + 1;
				len2 += len + 1;

				// Setting color attribute - CSMALL
				// 'plAttrib' offset as follows: ((length of line)*(number of lines))+qtyLength)
				memset( (plAttrib+((usI+1)*usLineLength)+MLD_QTY_LEN), pItem->Condiment[usI].uchCondColorPaletteCode, (len+1)*sizeof(plAttrib));
				plAttrib++;

			}
		} /* end for usI */
    }

#if 0
	// Removed this code since we are now doing the calculation above with
	// the lQTY variable rather than ( pItem->lQTY / 1000 ) which means that
	// void consolidation is now taken into account.
    if (MldChkVoidConsolidation(pItem))   // removed with #if 0
	{
		TCHAR  auchDummy[NUM_PLU_LEN] = {0};

        /* recalculate sales price from sCouponQTY */
        lAllPrice = pItem->lUnitPrice;
		for (i = pItem->uchChildNo; i < STD_MAX_COND_NUM; i++)
		{
			if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0)
			{
				/* if (pItem->Condiment[i].usPLUNo) { */
				lAllPrice += pItem->Condiment[i].lUnitPrice;
			}
        }
        /* --- calcurate product * qty --- */
        RflMul32by32(&huWork, (LONG)pItem->sCouponQTY, lAllPrice);
        lAllPrice =  *((LONG *)(&huWork));
    }
#endif

	/* display price of DEPT/PLU or not */
    if (usPrice)
	{
		USHORT  usAmtLen = 0;
		TCHAR   aszAmt[MLD_AMOUNT_LEN + 1 + 1] = { 0 };  // 1 + 1 ( space + terminate)

		RflSPrintf(aszAmt, TCHARSIZEOF(aszAmt), aszItemAmt, lAllPrice);
		usAmtLen = tcharlen(aszAmt);
		if (len2 + usAmtLen > usLineLength)
		{
			_tcsncpy( pszWork, szColWork, usLineLength);
			pszWork += usLineLength + 1;
			usSetLine ++;
			plAttrib++;             // feed reverse video control flag
			lAttrib = lDefAttrib;   // reset reverse video position
			tcharnset(szColWork, _T(' '), usLineLength);
			pColWork = &szColWork[usLineIdent];
		}
		pColWork = szColWork + usLineLength - 1;
		_tcsncpy (pColWork - usAmtLen - 1, aszAmt, usAmtLen);
		len2 = usLineLength;
    }

    if (len2 > (USHORT)(usLineIdent + 1)) {     /* restore condiments */
        _tcsncpy( pszWork, szColWork, usLineLength);
        pszWork += usLineLength + 1;
        usSetLine ++;
    }
    return(usSetLine);
}



/*
*===========================================================================
** Format  : USHORT  MldVoidOffline(UCHAR *pszWork, CHAR fbMod, USHORT usDispType);
*
*    Input : CHAR   fbMod            -void modifier flag
*            USHORT usDispType       -type of column length
*
*   Output : UCHAR  *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets 'VOID' or 'OFFL' to the buffer
*            'pszWork'.
*===========================================================================
*/
USHORT MldVoidOffline(TCHAR *pszWork, USHORT fbMod, USHORT usDispType)
{
	USHORT  usVoidReturnMask = (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3);
    TCHAR  aszSpecMnem1[PARA_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */
    TCHAR  aszSpecMnem2[PARA_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */

    /* -- get 'VOID' mnemonics -- */
    if (fbMod & usVoidReturnMask) {
		UCHAR  ucAddr;

		if (fbMod & RETURNS_MODIFIER_1) {
			ucAddr = SPC_RETURNS_1;
		} else if (fbMod & RETURNS_MODIFIER_2) {
			ucAddr = SPC_RETURNS_2;
		} else if (fbMod & RETURNS_MODIFIER_3) {
			ucAddr = SPC_RETURNS_3;
		} else {
			ucAddr = SPC_VOID_ADR;
		}
		RflGetSpecMnem(aszSpecMnem1, ucAddr);
    }

    if ((fbMod & OFFEPTTEND_MODIF) || (fbMod & OFFCPTEND_MODIF)) {
		RflGetSpecMnem(aszSpecMnem2, SPC_OFFTEND_ADR);
    }

    /* -- get 'VOID' mnemonics -- */
    if (fbMod & (usVoidReturnMask | OFFEPTTEND_MODIF | OFFCPTEND_MODIF)) {
		// if Condiment Edit turned on or we are not a drive thru then use MLD_PRECOLUMN1
		if (CliParaMDCCheck (MDC_DEPT3_ADR, ODD_MDC_BIT3) || (usDispType != MLD_DRIVE_THROUGH_3SCROLL)) {
            MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldVoidOffline, aszSpecMnem1, aszSpecMnem2);
        } else {
            MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldVoidOffline, aszSpecMnem1, aszSpecMnem2);
        }
        return(1);
    }

    return(0);

}

/*
*===========================================================================
** Format  : USHORT  MldNumber(UCHAR *pszWork, UCHAR *pszNumber, USHORT usDispType);
*
*    Input : UCHAR  *pszNumber       -number
*            USHORT usDispType       -type of column length
*
*   Output : UCHAR  *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets number to the buffer
*            'pszWork'.
*===========================================================================
*/
USHORT MldNumber(TCHAR *pszWork, TCHAR *pszNumber, USHORT usDispType)
{
    size_t  len;

    /* -- get 'number' mnemonics -- */
    if (*pszNumber) {
        if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
            //tcharnset(&(pszWork[0]), 0x20, MLD_QTY_LEN);
	        tcharnset(pszWork, _T(' '), MLD_PRECOLUMN1);
            pszWork[3] = _T('#');
            _tcsncpy(&(pszWork[4]), pszNumber, _tcslen(pszNumber));
            return(0x101);
        } else {
            pszWork[0] = _T('#');
            if ((len = _tcslen(pszNumber)) > 10) {
                _tcsncpy(&(pszWork[1]), pszNumber, 10);
				pszWork += 11;
                *pszWork = _T(' ');
				pszWork++;
                *pszWork = _T('\0');
				pszWork++;
                *pszWork = _T(' ');
				len -= 10;
				if(len <= MLD_DR3COLUMN)
				{
					_tcsncpy(pszWork, &(pszNumber[10]), len);
					pszWork+= (len);
					tcharnset(pszWork, _T(' '), (MLD_DR3COLUMN - len));
					return (0x202);
				} else //we are going to be longer than 2 rows
				{
					//copy a full row of numbers
					_tcsncpy(pszWork, &(pszNumber[10]), MLD_DR3COLUMN);
					len -= MLD_DR3COLUMN;
					pszWork += MLD_DR3COLUMN + 1;
					*pszWork = _T('\0');
					_tcsncpy(pszWork, &(pszNumber[10 + MLD_DR3COLUMN]), len);
					pszWork+= len;
					tcharnset(pszWork, _T(' '), (MLD_DR3COLUMN - (len - 1)));
					return (0x303);
				}
            } else {
                _tcsncpy(&(pszWork[1]), pszNumber, len);
				pszWork+= len+1;
				tcharnset(pszWork, _T(' '), (MLD_DR3COLUMN - (len + 1)));
                return(0x101);
            }
        }
    }

    return(0);
}

/*
*===========================================================================
** Format  : USHORT  MldNumber(UCHAR *pszWork, UCHAR *pszNumber, USHORT usDispType);
*
*    Input : UCHAR  *pszNumber       -number
*            USHORT usDispType       -type of column length
*
*   Output : UCHAR  *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets number to the buffer
*            'pszWork'.
*===========================================================================
*/
USHORT MldMnemNumber(TCHAR *pszWork, USHORT usAdr, TCHAR *pszNumber, USHORT usDispType)
{
    if (*pszNumber) {
		TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* NUM_... defined in "regstrct.h" */

        /* -- get transaction mnemonics -- */
        if (usAdr == 0) usAdr = TRN_NUM_ADR;
		RflGetTranMnem(aszTranMnem, usAdr);

        if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
			USHORT  usRow;

            /* -- set transaction mnemonics and amount -- */
            usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldMnemNum, aszTranMnem, pszNumber);
            return(usRow);
        } else {
			USHORT  usRow;

            /* -- set transaction mnemonics and amount -- */
            usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldMnemNum, aszTranMnem, pszNumber);
            return(usRow);
        }
    }

    return(0);
}

/*
*===========================================================================
** Format  : USHORT  MldMnemAmt(UCHAR *pszWork, UCHAR uchAdr, LONG lAmount
*                               USHORT usDispType);
*
*    Input : UCHAR  uchAdr          -Transacion mnemonics address
*            LONG   lAmount         -Amount
*            USHORT usDispType       -type of column length
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonics and amount to the buffer
*            'pszWork'.
*===========================================================================
*/
static USHORT  MldMnemAmtTwoMnem(TCHAR *pszWork, USHORT usAdr1, USHORT usAdr2, DCURRENCY lAmount, TCHAR *pCardLabel, USHORT usDispType)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN * 2 + 5] = {0};  /* NUM_... defined in "regstrct.h" */
	TCHAR  aszTranMnem2[PARA_TRANSMNEMO_LEN + 1] = {0};
    USHORT  usRow;

    /* -- get transaction mnemonics -- */
	if ((usAdr1 & STD_FORMATMASK_INDENT_4) != 0) {
		/* -- get transaction  mnemonics -- */
		aszTranMnem [0] = aszTranMnem [1] = aszTranMnem [2] = aszTranMnem [3] = _T(' ');
		RflGetTranMnem(aszTranMnem + 4, usAdr1);
	} else {
		RflGetTranMnem(aszTranMnem, usAdr1);
	}

	if (usAdr2) {
		/* -- get transaction  mnemonics -- */
		RflGetTranMnem(aszTranMnem2, usAdr2);
		_tcscat (aszTranMnem, aszTranMnem2);
	} else if (pCardLabel) {
		_tcscat (aszTranMnem, _T(" ("));
		_tcsncat (aszTranMnem, pCardLabel, PARA_TRANSMNEMO_LEN);
		_tcscat (aszTranMnem, _T(")"));
	}

	if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
        /* -- set transaction mnemonics and amount -- */
        usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldMnemAmt, aszTranMnem, lAmount);
    } else {
        /* -- set transaction mnemonics and amount -- */
        usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldMnemAmt, aszTranMnem, lAmount);
    }

    return(usRow);
}

USHORT  MldMnemAmt(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, USHORT usDispType)
{
	return MldMnemAmtTwoMnem(pszWork, usAdr, 0, lAmount, 0, usDispType);
}

/*
*===========================================================================
** Format  : USHORT  MldFSMnemAmt(UCHAR *pszWork, UCHAR uchAdr, LONG lAmount
*                               USHORT usDispType)
*
*    Input : UCHAR  uchAdr          -Transacion mnemonics address
*            LONG   lAmount         -Amount
*            USHORT usDispType       -type of column length
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonics and amount to the buffer
*            'pszWork'.
*===========================================================================
*/
USHORT  MldFSMnemAmt(TCHAR *pszWork, USHORT usAdr, LONG lFS, DCURRENCY lAmount, USHORT usDispType)
{
	static CONST TCHAR   aszMldFSMnemAmt[] = _T("%-s \t%d    %l$");   /* trans. mnem and amount for FS,   Saratoga */
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* NUM_... defined in "regstrct.h" */
    USHORT  usRow;

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
        /* -- set transaction mnemonics and amount -- */
        usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldFSMnemAmt, aszTranMnem, lFS, lAmount);
    } else {
        /* -- set transaction mnemonics and amount -- */
        usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldFSMnemAmt, aszTranMnem, lFS, lAmount);
    }

    return(usRow);
}

/*
*===========================================================================
** Format  : USHORT  MldMnemAmtShift(UCHAR *pszWork, UCHAR uchAdr, LONG lAmount
*                               USHORT usDispType);
*
*    Input : UCHAR  uchAdr          -Transacion mnemonics address
*            LONG   lAmount         -Amount
*            USHORT usDispType       -type of column length
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonics and amount to the buffer
*            'pszWork'.V3.3
*===========================================================================
*/
USHORT  MldMnemAmtShift(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, USHORT usDispType)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* NUM_... defined in "regstrct.h" */
    USHORT  usRow = 0;

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
        /* -- set transaction mnemonics and amount -- */
        usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldMnemAmtShift, aszTranMnem, lAmount);
    } else {
        /* -- set transaction mnemonics and amount -- */
        if (lAmount <= STD_MLD_MINUS_10_DIGITS) {
            /* digit overflow case */
            usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldMnemAmt, aszTranMnem, lAmount);
        } else {
            usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldMnemAmtShift, aszTranMnem, lAmount);
        }
    }

    return(usRow);
}

/*
*===========================================================================
** Format  : USHORT  MldDiscount(UCHAR *pszWork, UCHAR uchAdr, UCHAR uchRate,
*                                       LONG lAmount, USHORT usDispType);
*
*    Input : UCHAR  uchAdr          -Transacion mnemonics address
*            UCHAR  uchRate         -Discount rate
*            LONG   lAmount         -Amount
*            USHORT usDispType       -type of column length
*
*   Output : UCHAR  *pszWork        -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonics, discount rate,
*            and amount to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  MldDiscount(TCHAR *pszWork, UCHAR uchSeatNo, USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount,
                        USHORT usDispType)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* NUM_... defined in "regstrct.h" */
    USHORT  usRow;

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- set transaction mnemonics, rate, and amount -- */
    if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
        if (uchRate == 0) {                  /* amount override ? */
            if (uchSeatNo == 0) {
                usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldMnemAmt, aszTranMnem, lAmount);
            } else {
                usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldSeatMnemAmt, uchSeatNo, aszTranMnem, lAmount);
            }
        } else {
            if (uchSeatNo == 0) {
                usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldDiscount, aszTranMnem, uchRate, lAmount);
            } else {
                usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldSeatDiscount, uchSeatNo, aszTranMnem, uchRate, lAmount);
            }
        }
    } else {
        if (uchRate == 0) {                  /* amount override ? */
            usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldMnemAmt, aszTranMnem, lAmount);
        } else {
            usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldDiscount2, aszTranMnem, uchRate, lAmount);
        }
    }

    return(usRow);
}


/*
*===========================================================================
** Format  : USHORT  MldCpn( UCHAR *pszWork, UCHAR *pszMnemonic,
*                                 LONG  lAmount, USHORT usDispType)
*
*    Input : UCHAR  *pszWork        -   point to destination buffer
*            UCHAR  *pszMnemonic    -   point to coupon mnemonic
*            LONG   lAmount         -   amount of coupon
*            USHORT usDispType       -type of column length
*
*   Output : UCHAR  *pszDest        -   point to created string
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis:    This function sets coupon mnemonics and its amount, and
*               then it stores created string to the buffer 'pszWork'
*===========================================================================
*/
USHORT  MldCpn( TCHAR *pszWork, UCHAR uchSeatNo, TCHAR *pszMnemonic, DCURRENCY lAmount,
                        USHORT usDispType)
{
    USHORT usRow = 0;

    /* -- set coupon mnemonics, and its amount -- */
    if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
        if (uchSeatNo == 0) {
            usRow = MldSPrintf( pszWork, MLD_PRECOLUMN1, aszMldMnemAmt, pszMnemonic, lAmount );
        } else {
            usRow = MldSPrintf( pszWork, MLD_PRECOLUMN1, aszMldSeatMnemAmt, uchSeatNo, pszMnemonic, lAmount );
        }
    } else {
        usRow = MldSPrintf( pszWork, MLD_DR3COLUMN, aszMldMnemAmt, pszMnemonic, lAmount );
    }

    return (usRow);
}

/*
*===========================================================================
** Format  : USHORT  MldTaxMod(UCHAR *pszWork, USHORT fsTax, UCHAR fbMod);
*
*   Input  : USHORT  fsTax,        -US or Canadian Tax
*            UCHAR   fbMod
*            USHORT usDispType       -type of column length
*
*   Output : UCHAR   *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : number of lines to be set
*
** Synopsis: This function sets tax modifier mnemonics to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  MldTaxMod(TCHAR *pszWork, USHORT fsTax, USHORT fbMod, USHORT usDispType)
{
    TCHAR  aszTaxMod[(PARA_SPEMNEMO_LEN + 1) * 4 + 1] = {0};

    if (CliParaMDCCheck(MDC_TAX_ADR, ODD_MDC_BIT0)) {   /* canadian tax R3.1 */

        fsTax = MODEQUAL_CANADIAN;

        if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
            /* -- get tax modifier mnemonics -- */
            if ( PRT_WITHDATA == MldGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod) ) {
                MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldTaxMod, aszTaxMod);
            }
            if (*pszWork != '\0') {
                return(1);
            }
        } else {
            /* -- get tax modifier mnemonics -- */
            if ( PRT_WITHDATA == MldGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod) ) {
                MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldTaxMod, aszTaxMod);
            }
            if (*pszWork != '\0') {
                return(1);
            }
        }
        return(0);
    }

    if (fbMod & (TAX_MODIFIER1|TAX_MODIFIER2|TAX_MODIFIER3|FOOD_MODIFIER)) {/* Saratoga */
        if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
            /* -- get tax modifier mnemonics -- */
            if ( PRT_WITHDATA == MldGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod) ) {
                MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldTaxMod, aszTaxMod);
            }
            if (*pszWork != '\0') {
                return(1);
            }
        } else {
            return (MldDriveTaxMod(pszWork, fsTax, fbMod));
        }
    }

    return(0);
}


/*
*===========================================================================
** Format  : USHORT  MldTaxMod(UCHAR *pszWork, USHORT fsTax, UCHAR fbMod);
*
*   Input  : USHORT  fsTax,        -US or Canadian Tax
*            UCHAR   fbMod
*
*   Output : UCHAR   *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : number of lines to be set
*
** Synopsis: This function sets tax modifier mnemonics to the buffer 'pszWork'.
*===========================================================================
*/
static USHORT  MldDriveTaxMod(TCHAR *pszWork, USHORT fsTax, USHORT fbMod)
{
    TCHAR   aszMnem1[MLD_DR3COLUMN + 1] = {0};
    TCHAR   aszMnem2[MLD_DR3COLUMN + 1] = {0};
    USHORT  i = 0;

    /* get tax modifier1 mnemo -- */
    if (fbMod & TAX_MODIFIER1) {
		TCHAR   aszSpec[PARA_SPEMNEMO_LEN + 1] = { 0 };

		RflGetSpecMnem(aszSpec, SPC_TXMOD1_ADR);
        _tcsncat(aszMnem1, aszSpec, PARA_SPEMNEMO_LEN);
        i++;
    }
    /* get tax modifier2 mnemo -- */
    if (fbMod & TAX_MODIFIER2) {
		TCHAR   aszSpec[PARA_SPEMNEMO_LEN + 1] = { 0 };

		RflGetSpecMnem(aszSpec, SPC_TXMOD2_ADR);
		_tcsncat(aszMnem1, aszSpec, PARA_SPEMNEMO_LEN);
        i++;
    }
    /* get tax modifier3 mnemo -- */
    if (fbMod & TAX_MODIFIER3) {
		TCHAR   aszSpec[PARA_SPEMNEMO_LEN + 1] = { 0 };

		RflGetSpecMnem(aszSpec, SPC_TXMOD3_ADR);
		_tcsncat(aszMnem1, aszSpec, PARA_SPEMNEMO_LEN);
        i++;
    }
    /* --- get FS Mod. mnemo,   Saratoga --- */
    if (fbMod & FOOD_MODIFIER) {
		TCHAR   aszSpec[PARA_SPEMNEMO_LEN + 1] = { 0 };

		RflGetSpecMnem(aszSpec, SPC_FSMOD_ADR);
        if (tcharlen(aszMnem1) + PARA_SPEMNEMO_LEN > MLD_DR3COLUMN) {
			_tcsncat(aszMnem2, aszSpec, PARA_SPEMNEMO_LEN);
            i++;
        } else {
			_tcsncat(aszMnem1, aszSpec, PARA_SPEMNEMO_LEN);
        }
    }

    switch (i) {
    case    4:
        MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldDrvFSMod, aszMnem1);
        MldSPrintf(pszWork + MLD_DR3COLUMN + 1, MLD_DR3COLUMN, aszMldDrvFSMod, aszMnem2);
        return(2);

    case    3:
    case    2:
    case    1:
        MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldDrvFSMod, aszMnem1);
        return(1);
    }

    return(0);
    fsTax = fsTax;
}

/*
*===========================================================================
** Format  : USHORT  MldWeight(UCHAR *pszWork, ULONG flScale, ITEMSALES *pItem);
*
*    Input : USHORT     fsTax           -US or Canadian Tax
*            ULONG      flScale         -auto scale print type ".00" or ".000"
*            ITEMSALES  *pItem          -Item Data address
*
*   Output : UCHAR      *pszWork        -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets tax modifier, weight, and unit price
*            to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  MldWeight(TCHAR *pszWork, ULONG flScale, ITEMSALES *pItem, USHORT usDispType)
{
	CONST TCHAR   aszMldWeight1[] = _T("   %8.3l$ %s %8l$"); /* weight, and unit price */
	CONST TCHAR   aszMldWeight2[] = _T("   %8.2l$ %s %8l$"); /* weight, and unit price */
	CONST TCHAR   aszMldWeight3[] = _T("%.3l$\t %s %l$ ");   /* weight, and unit price */
	CONST TCHAR   aszMldWeight4[] = _T("%.2l$\t %s %l$ ");   /* weight, and unit price */
	USHORT usRow = 0;
	TCHAR  aszSpecMnem2[STD_SPEMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */

	RflGetSpecMnem(aszSpecMnem2, SPC_EC_ADR);

    if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
        /* -- display ".000" type ? -- */
        if ( flScale & CURQUAL_SCALE3DIGIT ) {
            MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldWeight1, (DWEIGHT)pItem->lQTY, aszSpecMnem2, (DCURRENCY)pItem->lUnitPrice);
        /* -- display ".00" type  -- */
        } else {
            MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldWeight2, (DWEIGHT)pItem->lQTY / 10L, aszSpecMnem2, (DCURRENCY)pItem->lUnitPrice);
        }
		usRow = 1;
    } else {
        if ( flScale & CURQUAL_SCALE3DIGIT ) {
			/* -- display ".000" type ? -- */
            usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldWeight3, (DWEIGHT)pItem->lQTY, aszSpecMnem2, (DCURRENCY)pItem->lUnitPrice);
        } else {
			/* -- display ".00" type  -- */
            usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldWeight4, (DWEIGHT)pItem->lQTY / 10L, aszSpecMnem2, (DCURRENCY)pItem->lUnitPrice);
        }
    }
    return(usRow);
}

/*
*===========================================================================
** Format  : USHORT  MldTranMnem(UCHAR *pszWork ,UCHAR uchAdr);
*
*    Input : UCHAR  uchAdr          -Transacion mnemonics address
*
*   Output : UCHAR  *pszWork        -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonics to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  MldTranMnem(TCHAR *pszWork, USHORT usAdr, USHORT usDispType)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0}; /* PARA_... defined in "paraequ.h" */

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- set transaction mnemonics and amount -- */
    // MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldTranMnem, aszTranMnem);
	if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
		MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldTranMnem, aszTranMnem);	//single reciept window
	}
	else {
		MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldTranMnem, aszTranMnem);	//tri-screen reciept window
	}

    return(1);

    usDispType = usDispType;
}

/*
*===========================================================================
** Format  : USHORT  MldSeatNo(UCHAR *pszWork ,UCHAR uchSeatNo, USHORT usDispType);
*
*    Input : UCHAR  uchSeatNo       -seat no
*
*   Output : UCHAR  *pszWork        -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets seat no to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  MldSeatNo(TCHAR *pszWork, UCHAR uchSeatNo, USHORT usDispType)
{
	CONST TCHAR   aszMldSeatNo[] = _T("   s%.1d");     //SR206 _T("   s%.2d");   seat no */
	CONST TCHAR   aszMldSeatNo2[] = _T("    s%.1d");   //SR206 _T("    s%.2d");   seat no */

    if (uchSeatNo) {
        if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
            MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldSeatNo, uchSeatNo);
        } else {
            MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldSeatNo2, uchSeatNo);
        }
        return(1);
    }
    return(0);
}

/*
*===========================================================================
** Format  : USHORT  MldChild(UCHAR uchAdj, UCHAR *pszMnem);
*
*   Input  : UCHAR  uchAdj          -Adjective number
*            UCHAR  *pszMnem        -Adjective mnemonics address
*
*   Output : UCHAR  *pszWork        -destination buffer address
*   InOut  : none
*
** Return  : USHORT  usSetLine      -number of lines to be set
*
** Synopsis: This function sets child plu to the buffer 'pszWork'.
*
*===========================================================================
*/
USHORT  MldChild(TCHAR *pszWork, UCHAR uchAdj, TCHAR *pszMnem, USHORT usDispType)
{
	CONST TCHAR   aszMldChildPLU[] = _T("    %-4s %-.17s");    /* adj. mnem., and child PLU mnem. */
	CONST TCHAR   aszMldChildPLU2[] = _T("  %-4s\n  %-.9s");   /* adj. mnem., and child PLU mnem. */
	CONST TCHAR   aszMldChildPLU3[] = _T("  %-.9s");           /* adj. mnem., and child PLU mnem. */
	CONST TCHAR   aszMldChildPLU4[] = _T("  %s %s");           /* adj. mnem., and child PLU mnem. */
	CONST TCHAR   aszMldChildPLU5[] = _T("    %-s");           /* adj. mnem., and child PLU mnem. */
	TCHAR    aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = {0};

    if (uchAdj != 0) {
        /* --get adjective mnemocnics -- */
        RflGetAdj (aszAdjMnem, uchAdj);
    }

    /* -- set adjective mnemonics and child plu mnemonics -- */
    if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
        /* 2172 */
        if (uchAdj) {
            MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldChildPLU, aszAdjMnem, pszMnem);
        } else {
            MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldChildPLU5, pszMnem);
        }
        return(1);
    } else {
        if (uchAdj != 0) {
            if ((_tcslen(aszAdjMnem) + _tcslen(pszMnem) + 1) > (MLD_DRIVE_DEPT_PLU_LEN)) {
                MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldChildPLU2, aszAdjMnem, pszMnem);
                return(2);
            } else {
                MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldChildPLU4, aszAdjMnem, pszMnem);
                return(1);
            }
        } else {
            MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldChildPLU3, pszMnem);
            return(1);
        }
    }
}

/*
*===========================================================================
** Format  : USHORT  MldChild(UCHAR uchAdj, UCHAR *pszMnem);
*
*   Input  : UCHAR  uchAdj          -Adjective number
*            UCHAR  *pszMnem        -Adjective mnemonics address
*
*   Output : UCHAR  *pszWork        -destination buffer address
*   InOut  : none
*
** Return  : USHORT  usSetLine      -number of lines to be set
*
** Synopsis: This function sets child plu to the buffer 'pszWork'.
*
*===========================================================================
*/
USHORT  MldLinkPlu(TCHAR *pszWork, USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, LONG lQTY, DCURRENCY lPrice, USHORT usDispType)
{
	static CONST TCHAR   aszMldLinkPLU[] = _T("%2d %-4s %-.17s");     /* adj. mnem., and child PLU mnem. */
	static CONST TCHAR   aszMldLinkPLU2[] = _T("%2d %-4s\n  %-.9s");  /* adj. mnem., and child PLU mnem. */
	static CONST TCHAR   aszMldLinkPLU3[] = _T("%2d %-.9s");          /* adj. mnem., and child PLU mnem. */
	static CONST TCHAR   aszMldLinkPLU4[] = _T("%2d %s %s");          /* adj. mnem., and child PLU mnem. */
	static CONST TCHAR   aszMldLinkPLU5[] = _T("%2d %-s");            /* adj. mnem., and child PLU mnem. */
	static CONST TCHAR   aszMldLinkPLU6[] = _T("%2d %-4s\t%s%l$");    /* adj. mnem., and child PLU mnem. */
	static CONST TCHAR   aszMldLinkPLU7[] = _T("%2d %s\t%l$");        /* adj. mnem., and child PLU mnem. */
	static CONST TCHAR   aszMldLinkPLU8[] = _T("%2d %-4s\t%s%l$ ");   /* adj. mnem., and child PLU mnem. */
	static CONST TCHAR   aszMldLinkPLU9[] = _T("%2d %s\t%l$ ");       /* adj. mnem., and child PLU mnem. */

	TCHAR    aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = {0};
    USHORT   usRow = 0;

    if (uchAdj != 0) {
        /* --get adjective mnemocnics -- */
        RflGetAdj (aszAdjMnem, uchAdj);
    }

    /* -- set adjective mnemonics and child plu mnemonics -- */
    if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
        /* display price of dept/plu at precheck/post check system */
        if (!CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT1)) {
            if (uchAdj) {
                if (fsModified) {
                    usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldLinkPLU8, lQTY, aszAdjMnem, pszMnem, lPrice);
                } else {
                    usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldLinkPLU6, lQTY, aszAdjMnem, pszMnem, lPrice);
                }
            } else {
                if (fsModified) {
                    usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldLinkPLU9, lQTY, pszMnem, lPrice);
                } else {
                    usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldLinkPLU7, lQTY, pszMnem, lPrice);
                }
            }
        } else {
            if (uchAdj) {
                usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldLinkPLU, lQTY, aszAdjMnem, pszMnem);
            } else {
                usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldLinkPLU5, lQTY, pszMnem);
            }
        }
    } else {
        if (uchAdj != 0) {
            if ((_tcslen(aszAdjMnem) + _tcslen(pszMnem) + 1) > (MLD_DRIVE_DEPT_PLU_LEN)) {
                usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldLinkPLU2, lQTY, aszAdjMnem, pszMnem);
            } else {
                MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldLinkPLU4, lQTY, aszAdjMnem, pszMnem);
            }
        } else {
            usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldLinkPLU3, lQTY, pszMnem);
        }
    }

    return(usRow);
}

/*
*===========================================================================
** Format  : USHORT  MldGstChkNo(UCHAR *pszWork, ITEMMULTI *pItem,
*                                   USHORT usDispType);
*
*   Input  : ITEMMULTI  *pItem          -item data buffer address
*
*   Output : UCHAR  *pszWork            -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets guest check no. mnemonic and amount
*            to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  MldGstChkNo(TCHAR *pszWork, ITEMMULTI *pItem, USHORT usDispType)
{
    if (pItem->usGuestNo != 0 && (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL)) {
		TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};    /* PARA_... defined in "paraequ.h" */
		TCHAR  aszGCNo[16] = {0};  // [MLD_GC_LEN + 1];

        /* convert int to ascii */
        _itot(pItem->usGuestNo, aszGCNo, 10);
        /* -- get check paid mnemonics -- */
		RflGetTranMnem(aszTranMnem, TRN_CKPD_ADR);

        /* -- set guest check No. mnemonic and amount -- */
        if (pItem->uchCdv == 0) {               /* without CDV */
            MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldGCNo, aszTranMnem, aszGCNo);
        } else {                                /* with CDV */
            MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldGCNoCdv, aszTranMnem, aszGCNo, pItem->uchCdv);
        }
        return(1);
    }

    return(0);
}

/*
*===========================================================================
** Format  : USHORT  MldForeign(UCHAR  *pszWork, LONG lForeign, LONG lRate,
*                               UCHAR uchAdr, UCHAR fbStatus, USHORT usDispType);
*
*   Input  : LONG    lForeign      -Foreign tender amount
*            UCHAR   uchAdr        -Foreign symbol address
*            UCHAR   fbStatus      -Foreign currency decimal point status
*            ULONG   ulRate        -rate
*
*   Output : UCHAR   *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets foeign tender amount and conversion rate
*            to the buffer 'pszWork'.
*
*===========================================================================
*/
USHORT  MldForeign(TCHAR *pszWork, DCURRENCY lForeign, USHORT usAdr, UCHAR fbStatus,
                        ULONG ulRate, UCHAR fbStatus2, USHORT usDispType)
{
    if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
        return (MldPrechkForeign(pszWork, lForeign, usAdr, fbStatus, ulRate, fbStatus2));
    } else {
        return (MldDriveForeign(pszWork, lForeign, usAdr, fbStatus, ulRate, fbStatus2));
    }
}

/*
*===========================================================================
** Format  : USHORT  MldForeign(UCHAR  *pszWork, LONG lForeign, LONG lRate,
*                               UCHAR uchAdr, UCHAR fbStatus);
*
*   Input  : LONG    lForeign      -Foreign tender amount
*            UCHAR   uchAdr        -Foreign symbol address
*            UCHAR   fbStatus      -Foreign currency decimal point status
*            ULONG   ulRate        -rate
*
*   Output : UCHAR   *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets foeign tender amount and conversion rate
*            to the buffer 'pszWork'.
*
*===========================================================================
*/
static USHORT  MldPrechkForeign(TCHAR *pszWork, DCURRENCY lForeign, USHORT usAdr,
                        UCHAR fbStatus, ULONG ulRate, UCHAR fbStatus2)
{
    USHORT sDecPoint;
	USHORT usRow = 0;


    /* -- check decimal point -- */
    if (fbStatus & ODD_MDC_BIT1) {
        if (fbStatus & ODD_MDC_BIT2) {
            sDecPoint = PRT_NODECIMAL;
        } else {
            sDecPoint = PRT_3DECIMAL;
        }
    } else {
        sDecPoint = PRT_2DECIMAL;
    }

	if (CliParaMDCCheckField(MDC_TAX2_ADR, MDC_PARAM_BIT_A) == 0) {
		TCHAR  aszFMnem[PARA_SPEMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
		TCHAR  aszFAmt[MLD_PRECOLUMN1 + 1] = {0};
		USHORT usStrLen;

		/* -- get foreign symbol -- */
		RflGetSpecMnem(aszFMnem, usAdr);

		/* -- get string length -- */
		usStrLen = tcharlen(aszFMnem);

		/* -- format foreign amount -- */
		RflSPrintf(&aszFAmt[usStrLen], (TCHARSIZEOF(aszFAmt) - usStrLen), aszMldForeign1, sDecPoint, lForeign);

		/* -- adjust sign ( + , - ) -- */
		if (lForeign < 0) {
			aszFAmt[0] = _T('-');
			_tcsncpy(&aszFAmt[1], aszFMnem, usStrLen);
		} else {
			_tcsncpy(&aszFAmt[0], aszFMnem, usStrLen);
		}

		/* -- set foreign amount and conversion rate */
		MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldForeign2, aszFAmt);
	} else {
		TCHAR  aszFMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
		TCHAR  aszFAmt[MLD_PRECOLUMN1 + 1] = {0};

		RflGetTranMnem(aszFMnem, usAdr);
		/* -- format foreign amount -- */
		RflSPrintf(aszFAmt, TCHARSIZEOF(aszFAmt), aszMldForeign1, sDecPoint, lForeign);

		MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldForeign5, aszFMnem, aszFAmt);
	}
	pszWork += MLD_PRECOLUMN1+1;
	usRow++;

	if (CliParaMDCCheckField(MDC_TAX2_ADR, MDC_PARAM_BIT_A) == 0) {
		/* 6 digit decimal point for Euro, V3.4 */
		if (fbStatus2 & ODD_MDC_BIT0) {
			sDecPoint = PRT_6DECIMAL;
		} else {
			sDecPoint = PRT_5DECIMAL;
		}
		MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldForeign3, sDecPoint, (DCURRENCY)ulRate);  // format %10.*l$ requires DCURRENCY
		usRow++;
	}
    return(usRow);
}

/*
*===========================================================================
** Format  : USHORT  MldForeign(UCHAR  *pszWork, LONG lForeign, LONG lRate,
*                               UCHAR uchAdr, UCHAR fbStatus);
*
*   Input  : LONG    lForeign      -Foreign tender amount
*            UCHAR   uchAdr        -Foreign symbol address
*            UCHAR   fbStatus      -Foreign currency decimal point status
*            ULONG   ulRate        -rate
*
*   Output : UCHAR   *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets foeign tender amount and conversion rate
*            to the buffer 'pszWork'.
*
*===========================================================================
*/
static USHORT  MldDriveForeign(TCHAR *pszWork, DCURRENCY lForeign, USHORT usAdr,
                        UCHAR fbStatus, ULONG ulRate, UCHAR fbStatus2)
{
    USHORT usStrLen;
    USHORT usStrLen2;
    USHORT sDecPoint;
    USHORT usRow=0;
    TCHAR  aszFMnem[PARA_SPEMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
    TCHAR  aszFMnem2[PARA_SPEMNEMO_LEN + 2] = {0};  /* PARA_... defined in "paraequ.h" */
    TCHAR  aszFAmt[MLD_DR3COLUMN + 1] = {0};

    /* -- check decimal point derived from the Foreign Currency MDC address -- */
    if (fbStatus & ODD_MDC_BIT1) {
		// if Bit C is set then we are not 2 decimal places so are either 3 or 0
		// depending on the setting of Bit B.
        if (fbStatus & ODD_MDC_BIT2) {
            sDecPoint = PRT_NODECIMAL;
        } else {
            sDecPoint = PRT_3DECIMAL;
        }
    } else {
        sDecPoint = PRT_2DECIMAL;
    }

    /* -- get foreign symbol -- */
	RflGetSpecMnem(aszFMnem, usAdr);

    /* -- get string length -- */
    usStrLen = tcharlen(aszFMnem);

    /* -- adjust sign ( + , - ) -- */
    if (lForeign < 0) {
        aszFMnem2[0] = _T('-');
        _tcsncpy(&(aszFMnem2[1]), aszFMnem, usStrLen);
        usStrLen++;
        lForeign = lForeign * (-1L);    /* adjust sign */

    } else {
        _tcsncpy(&aszFMnem2[0], aszFMnem, usStrLen);
    }


	RflGetSpecMnem(aszFMnem, usAdr);

	//SR 564, we now do the same type of calculation when formatting the amount and
	//foreign mnemonic. JHHJ

    /* -- get string length -- */
    usStrLen = tcharlen(aszFMnem);

    /* -- format foreign amount -- */
    RflSPrintf(&aszFAmt[0], (TCHARSIZEOF(aszFAmt) - usStrLen), aszMldForeign1, sDecPoint, lForeign);
    
	usStrLen2 = tcharlen(aszFAmt);

    if ((usStrLen+usStrLen2+1) > MLD_DR3COLUMN) {
        _tcsncpy(pszWork, aszFMnem2, usStrLen);
		
		//SR 564, we need to add spaces after the foreign amount so that we can see the entire thing.
		// if we dont, it will not show the rest of the transaction.JHHJ
		pszWork+= usStrLen;
		tcharnset(pszWork, _T(' '), (MLD_DR3COLUMN+1)-usStrLen);

        pszWork += (MLD_DR3COLUMN+1)-usStrLen;     /* display FC symbol */
        usRow++;

        _tcsncpy(pszWork, aszFAmt, usStrLen2);
        pszWork += MLD_DR3COLUMN+1;     /* display FC total */
        usRow++;
    } else {
        _tcsncpy(pszWork, aszFMnem2, usStrLen);
        _tcsncpy(&(pszWork[usStrLen]), aszFAmt, usStrLen2);
		pszWork += (usStrLen2 + usStrLen);

		//SR 564, we need to add spaces after the foreign amount so that we can see the entire thing.
		// if we dont, it will not show the rest of the transaction. JHHJ
		tcharnset(pszWork, _T(' '), MLD_DR3COLUMN - (usStrLen2 + usStrLen));
        pszWork += (MLD_DR3COLUMN - (usStrLen2 + usStrLen)+1);     /* display FC symbol and amount */
        usRow++;
    }

    /* --- format foreign currency conversion rate --- */
	if (CliParaMDCCheckField(MDC_TAX2_ADR, MDC_PARAM_BIT_A) == 0) {
		/* 6 digit decimal point for Euro, V3.4 */
		if (fbStatus2 & ODD_MDC_BIT0) {
			sDecPoint = PRT_6DECIMAL;
		} else {
			sDecPoint = PRT_5DECIMAL;
		}
		MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldForeign4, sDecPoint, (DCURRENCY)ulRate);  // format %10.*l$ requires DCURRENCY
		usRow++;
	}

    return(usRow);
}

/*
*===========================================================================
** Format  : USHORT  MldDoubleMnemAmt(UCHAR *pszWork, UCHAR uchAdr,
*                                     LONG lAmount, USHORT usDispType);
*
*    Input : UCHAR  uchAdr        -Transaction mnemonics address
*            LONG   lAmount       -amount
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets transaction mnemonic, native mnemonic
*            and amount.
*===========================================================================
*/
USHORT  MldDoubleMnemAmt(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, USHORT usDispType)
{
    USHORT  usRow = 0;
    USHORT  usLen;
    TCHAR   aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};       /* PARA_... defined in "paraequ.h" */

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- total mnemonic printed double -- */
    usLen = ((usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) ? MLD_PRECOLUMN1/2 : MLD_DR3COLUMN/2);

    if (tcharlen(aszTranMnem) <= usLen) {
		TCHAR   aszTranMnem2[PARA_TRANSMNEMO_LEN * 2 + 1] = {0};    /* PARA_... defined in "paraequ.h" */

		/* -- convert single char. to double -- */
        MldDouble(aszTranMnem2, PARA_TRANSMNEMO_LEN * 2, aszTranMnem);

        if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
            usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldMnemAmtD, aszTranMnem2, lAmount);
        } else {
            usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldMnemAmtD, aszTranMnem2, lAmount);
        }
    } else {
        if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
            usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldMnemAmt, aszTranMnem, lAmount);
        } else {
            usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldMnemAmt, aszTranMnem, lAmount);
        }
    }

    return(usRow);
}

/*
*===========================================================================
** Format  : USHORT  MldChargeTips(UCHAR *pszWork, USHORT usWaiID);
*
*    Input : USHORT  usWaiID         -waiter ID
*
*   Output : UCHAR  *pszWork         -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets waiter mnem. and ID, charge tips mnem.,
*            rate, and amount to the buffer 'pszWork'.
*===========================================================================
*/
USHORT  MldChargeTips(TCHAR *pszWork, ULONG ulWaiID, USHORT usDispType)
{
    if (ulWaiID) {
		TCHAR  aszWaiMnem[PARA_SPEMNEMO_LEN + 1] = {0};    /* PARA_... defined in "paraequ.h" */

        /* -- get waiter mnemonics -- */
		RflGetSpecMnem(aszWaiMnem, SPC_CAS_ADR); /* V3.3 */
		if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
			CONST TCHAR   *aszMldChgTps = _T("  %-4s %08d");    /* charge tips mnem., waiter mnem and ID, and amount */
			MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldChgTps, aszWaiMnem, ulWaiID);
		} else {
			CONST TCHAR   *aszMldChgTps = _T("  %-4s %04d");    /* charge tips mnem., waiter mnem and ID, and amount */
			MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldChgTps, aszWaiMnem, (ulWaiID % 10000));
		}
        return(1);
    } else {
        return(0);
    }

    usDispType = usDispType;
}

/*
*===========================================================================
** Format  : USHORT  MldTrayCount(UCHAR *pszWork, UCHAR uchAdr, SHORT sTrayCo);
*
*   Input  : UCHAR  uchAdr             -tray total counter mnemonic
*            SHORT  TrayCo             -tray total counter
*
*   Output : UCHAR  *pszWork           -destination buffer address
*   InOut  : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets tray .
*===========================================================================
*/
USHORT MldTrayCount(TCHAR *pszWork, USHORT usAdr, SHORT sTrayCo, USHORT usDispType)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
    USHORT  usRow = 0;

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* set check paid mnemonic */
    if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
        usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldTrayCo, aszTranMnem, sTrayCo);
    } else {
        usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldTrayCo, aszTranMnem, sTrayCo);
    }
    return(usRow);

}

/*
*===========================================================================
** Format  : USHORT  MldItems(UCHAR  *pszWork, ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : USHORT usSetLine     -number of line(s) to be set
*
** Synopsis: This function sets the mnemonics of adjective, noun,
*            print modifier, conidiment, and product amount.

  A change for the US Customs SCER in MldPrechkDeptPLU was causing a display
  problem when the scroll up or scroll down got to the top or bottom of the list
  in the receipt window, when changing the display/redisplaying

  Before the US Customs SCER the usDispType was signifying what type of reciept Window.
  After the US Customs SCER the usDispType was being used for the US Customs SCER, to
  indicate additional MLD types.
  An additional parameter should have been added to match the additional parameter
  used with other functions (ie MldPrechkItems, others?).  The additional parameter
  has now been added and is now 
  USHORT usType RPH 1-9-4 COURTESY BUG FIX

  WARNING:  condiment prices for condiments that are PPI PLUs are updated by using
            function ItemSalesCalcCondimentPPIReplace() so the condiment prices
			in ITEMSALES pItem are updated if there are PPI PLUs specified in the
			item data.
*===========================================================================
*/
USHORT  MldItems(TCHAR  *pszWork, ITEMSALES  *pItem,
                        USHORT usPrice, LONG *plAttrib, USHORT usDispType, USHORT usType)
{
	if ((CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT3) &&
		(pItem->uchMinorClass != CLASS_DEPT) &&
		(pItem->uchMinorClass != CLASS_DEPTITEMDISC))&&
		uchMldSystem != MLD_DRIVE_THROUGH_3SCROLL)
	{
		return (MldListItems(pszWork, pItem, plAttrib, usPrice));
	}
    if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) { 
        return (MldPrechkItems(pszWork, pItem, usPrice, plAttrib, usType));
    } else {
        return (MldDriveItems(pszWork, pItem, plAttrib));
    }
}
/*
*===========================================================================
** Format  : USHORT  MldItems(UCHAR  *pszWork, ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : USHORT usSetLine     -number of line(s) to be set
*
** Synopsis: This function sets the mnemonics of adjective, noun,
*            print modifier, conidiment, and product amount.
*===========================================================================
*/
static USHORT  MldPrechkItems(TCHAR  *pszWork, ITEMSALES  *pItem,
                                                USHORT usPrice, LONG *plAttrib,
												USHORT usDispType)//US Customs
{
    DCURRENCY   lProduct;
    LONG        lQTY, lAttrib;
    TCHAR       *pszStart;
    TCHAR       *pszEnd;
    USHORT      usAmtLen;
    USHORT      usMnemLen;
    USHORT      usSetLine = 0, usSetQty = 0, i;
	USHORT	    usPhysicalColPos = 0, usActualColPos = 0;
    SHORT       sReturn;
	TCHAR       aszItem[PRT_MAXITEM + MLD_PRECOLUMN1] = {0};
    TCHAR       aszAmt[MLD_AMOUNT_LEN + 1 + 1] = {0};  // 1 + 1 ( space + terminate)
	UCHAR	    bChineseChars = 0;   //whether we are displaying chinese characters

    pszStart = aszItem;

	if(pItem->uchMinorClass == CLASS_ITEMORDERDEC)
	{
		usPrice = 0;
	}
    /* display price of DEPT/PLU or not */
    usAmtLen = (usPrice ? MLD_ITEMAMT_LEN : 0);

	lQTY = DetermineQuantity (pItem);

    /* set over 1000 qty case after item consolidation case */
    if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) || (pItem->uchMinorClass == CLASS_ITEMORDERDEC)) {
        usSetQty = 1;
    } else if ((lQTY > 999L) || (lQTY < -99L)) {
		size_t      len;
		TCHAR       szQty100Work[16] = {0};

		//======Begin Quantity Fix, JHHJ 5-24-04======//
		tcharnset(pszWork, _T(' '), MLD_PRECOLUMN1);/* Set buffer to all spaces*/
		RflSPrintf(szQty100Work, TCHARSIZEOF(szQty100Work), aszMldQty2, lQTY); 
		/* set the formatted string to the buffer to the correct format with the quantity*/																								
		len = _tcslen(szQty100Work);
		/*get the length of the actual buffer, and takes out the null character*/
		_tcsncpy(pszWork, &szQty100Work[0], len);
		/*copy the correct quantity to the buffer WITHOUT the null character, which was causing the string
		to be terminated too early*/
        pszWork += MLD_PRECOLUMN1 + 1;
        usSetLine++;
        usSetQty = 1;
    }

    /* -- set item mnenemonics to buffer -- */
    MldSetItem(aszItem, pItem, &lProduct, usAmtLen, (plAttrib+usSetLine), usDispType);//US Customs added usDispType

#if 0
	// Have corrected MldSetItem () so that it will calculate the
	// correct price if voic consolidation is turned on.
    /* void consolidation print */
    if (MldChkVoidConsolidation(pItem)) {   // removed with #if 0
		TCHAR  auchDummy[NUM_PLU_LEN] = {0};

        /* recalculate sales price from sCouponQTY */
        lProduct = pItem->lUnitPrice;
        for (i = pItem->uchChildNo; i < STD_MAX_COND_NUM; i++) {
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0) {
            /* if (pItem->Condiment[i].usPLUNo) { */
               lProduct += pItem->Condiment[i].lUnitPrice;
            }
        }
        /* --- calcurate product * qty --- */
        RflMul32by32(&huWork, (LONG)pItem->sCouponQTY, lProduct);
        lProduct =  *((LONG *)(&huWork));
    }
#endif

    for (;;) {
		//determine the Actual(pos in buffer) and the physical(pos on screen)
		//to be used for a single line
		//if the character is a double width(not double byte) character
		//(that is the width, when printed on screen is twice that of ANSI characters)
		//adjust the physical pos to accomodate the extra width
		//the range being checked pertains to chinese characters in the range of 0x4E00 to 0x9FA5
		//Start the Actual and Physical pos at 0 and increment the actual by one and the physical by one
		//adding an extra pos to the physical if the range condition is met
		//count until a null is found or until the Physical pos is greater than what will fill the line
		for(usActualColPos = 0, usPhysicalColPos = 0;*(pszStart + usActualColPos) != 0 && usPhysicalColPos < MLD_PRECOLUMN1-MLD_QTY_LEN; usPhysicalColPos++, usActualColPos++){
			if(*(pszStart + usActualColPos) >= 0x4E00 && *(pszStart + usActualColPos) <= 0x9FA5){
				usPhysicalColPos++;
				bChineseChars = 1; // make this true so that we do the next line of code
			}
		}

		//this is possible if the chinese characters fill up the buffer too far
		//and if becomes necessary to read a smaller amount of characters so that the 
		//characters do not get cut off JHHJ
		if(bChineseChars)
		{
			while(usPhysicalColPos >= MLD_PRECOLUMN1-MLD_QTY_LEN)
			{
				usActualColPos -- ;
				usPhysicalColPos--;
			}
		}

        // -- get 1 line -- 
		//send the Actual pos to get the end of the string to display on the line
         sReturn = MldGet1Line(pszStart, &pszEnd, usActualColPos);//MLD_PRECOLUMN1 - MLD_QTY_LEN);

        tcharnset(pszWork, _T(' '), MLD_PRECOLUMN1);

        /* set quantity (except scale sales) */
        if (!usSetLine) {   /* first line */
            if (!usSetQty){
				//display quantity if NOT performing @/For operation (added SR 143 @/For key by cwunn)
				if( !pItem->usFor){
					size_t      len;
					TCHAR       szQtyWork[16] = {0};

					RflSPrintf(szQtyWork, TCHARSIZEOF(szQtyWork), aszMldQty1, lQTY);
					len = _tcslen(szQtyWork);
					switch(len) {
					case 1:     /* 1 digit */
						_tcsncpy(pszWork+1, szQtyWork, len);
						break;

					default:    /* over 2 digits */
						_tcsncpy(pszWork, szQtyWork, len);
						break;
					}
                }
            }
        }

        /* keep qty area (3 char) */
        pszWork += MLD_QTY_LEN;

        /* set adjective, noun mnemo, print modifier, condiment */
        if (sReturn == PRT_LAST) {
            break;
        } else {
            _tcsncpy(pszWork, pszStart, pszEnd - pszStart);

            /* -- "1" for space -- */
			if( *(pszEnd) == 0x20 ) {
				pszStart = pszEnd + 1;
			}
			else {
				pszStart = pszEnd;	
			}
			pszWork += (MLD_PRECOLUMN1-MLD_QTY_LEN) + 1;
			usSetLine++;

        }
    }

    /* get the length of mnemonics */
	
    usMnemLen = tcharlen(pszStart);

    /* display price of DEPT/PLU or not */
	memset(aszAmt, 0, sizeof(aszAmt));
    if (usPrice){
			RflSPrintf(aszAmt, TCHARSIZEOF(aszAmt), aszItemAmt, lProduct);
			usAmtLen = tcharlen(aszAmt);
    } else {
        usAmtLen = 0;
    }

    /* if data length is over 1 line length, separate into 2 lines */
    if ((pItem->uchMinorClass == CLASS_DEPTMODDISC) ||
        (pItem->uchMinorClass == CLASS_PLUMODDISC) ||
        (pItem->uchMinorClass == CLASS_SETMODDISC) ||
        (pItem->uchMinorClass == CLASS_OEPMODDISC)) {

        if ((!usMnemLen) && (!usAmtLen)) {
            ;   /* not feed 1 line */
			//if the string/Mnemonic being displayed and the size of the amount
			//are larger than the space available only copy the mnemonic then
			//move the pointer to the next line and copy the amount to that line
        } else if ((usPhysicalColPos + usAmtLen +1) > (MLD_PRECOLUMN1-MLD_QTY_LEN)) {
            _tcsncpy(pszWork, pszStart, usMnemLen);
            pszWork += MLD_PRECOLUMN1-MLD_QTY_LEN + 1;//advance to the next line
            usSetLine ++;
            if (usPrice) {
				_tcsncpy(pszWork + (MLD_PRECOLUMN1 - usAmtLen), aszAmt, usAmtLen);
                //MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldModItemAmt, aszAmt);
                usSetLine ++;
            }
        } else {
			//the mnemonic data and the amount will fit on one line
            _tcsncpy(pszWork, pszStart, usMnemLen);
            if (usPrice) {
				//place the price at the end of the physical(screen) location
				//move the pointer to the end of the line taking into account
				//the difference between the usActualColPos(buffer pos) and the
				//usPhysicalColPos(screen pos) and the length of the amount
                pszWork += usActualColPos + ((MLD_PRECOLUMN1-MLD_QTY_LEN)-usPhysicalColPos)-usAmtLen-1;
                _tcsncpy(pszWork, aszAmt, usAmtLen);

                /* ---- reset reverse video flag at amount display ---- */
                if (!(pItem->fbModifier & VOID_MODIFIER)) { /* not reset at void */
                    for (i=0,lAttrib=MLD_PRECHK_AMT_REV2; i<usAmtLen; i++) {
                        lAttrib |= (lAttrib >> 1);
                    }
                    *(plAttrib+usSetLine) &= ~lAttrib;
                }
            }
            usSetLine ++;
        }
    } else {

        if ((!usMnemLen) && (!usAmtLen)) {
            ;   /* not feed 1 line */
        } else	//Mnemonic and amount lengths are longer than available space on row, seperate into two rows
			//the PhysicalColPos is used because that value takes into account double wide characters
			if ((usPhysicalColPos + usAmtLen) > (MLD_PRECOLUMN1-MLD_QTY_LEN)) {
            _tcsncpy(pszWork, pszStart, usMnemLen);	//write the mnemonic to pszWork
            pszWork += MLD_PRECOLUMN1 - MLD_QTY_LEN; // advance pointer to beginning of next line
            //pszWork is an array of null terminated strings
			//place null and move pointer to beginning of next line
			*pszWork = 0x00;
			pszWork++;
			usSetLine ++;							//increase line pointer
			tcharnset(pszWork, _T(' '), MLD_PRECOLUMN1);
            if (usPrice) {
				_tcsncpy(pszWork + (MLD_PRECOLUMN1 - usAmtLen), aszAmt, usAmtLen);
                //MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldItemAmt, aszAmt);//write amount to new line
				//*(plAttrib+usSetLine) = 0x00;//clears reverse video bits in plAttrib for new line US Customs by cwunn 4/8/03 //colorpalette-changes
                usSetLine ++;
            }
        } else {//Menmonic and Amount will fit on one line
            _tcsncpy(pszWork, pszStart, usMnemLen);
            if (usPrice) {
				//place the price at the end of the physical(screen) location
				//move the pointer to the end of the line taking into account
				//the difference between the usActualColPos(buffer pos) and the
				//usPhysicalColPos(screen pos) and the length of the amount

                pszWork += usActualColPos + ((MLD_PRECOLUMN1-MLD_QTY_LEN)-usPhysicalColPos)-usAmtLen;
                _tcsncpy(pszWork, aszAmt, usAmtLen);

				/* Colorpalette-changes
                // ---- reset reverse video flag at amount display ---- 
                if (!(pItem->fbModifier & VOID_MODIFIER)) { // not reset at void 
					//subtract one from the amount length because the "last" number is already set by
					//MLD_PRECHK_AMT_REV and only the rest have to be changed
                    for (i=0,lAttrib=MLD_PRECHK_AMT_REV; i<usAmtLen - 1 + usPhysicalColPos - usActualColPos; i++) {
                        lAttrib |= (lAttrib >> 1);
                    }
                    *(plAttrib+usSetLine) &= ~lAttrib;
                }
				*/ //Colorpalette-changes
            }
            usSetLine ++;
        }
    }

    return(usSetLine);
}

/*
*===========================================================================
** Format  : USHORT  MldItems(UCHAR  *pszWork, ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : USHORT usSetLine     -number of line(s) to be set
*
** Synopsis: This function sets the mnemonics of adjective, noun,
*            print modifier, conidiment, and product amount.
*===========================================================================
*/
static USHORT  MldDriveItems(TCHAR  *pszWork, ITEMSALES  *pItem, LONG *plAttrib)
{
	// constants used below.
	USHORT      usLineLength = MLD_DR3COLUMN;   // number of characters per line
	USHORT      usLineIdent = MLD_QTY_LEN;      // number of columns to indent a line - 0 based
	USHORT      usMaxPluCondLen = MLD_DRIVE_DEPT_PLU_LEN;   // max length of PLU or condiment mnemonic
	USHORT      usPrice = 0;                     // flag to display price or not on line.

	return MldBuildDisplayItemLine (pszWork, pItem, usPrice, plAttrib, usLineLength, usLineIdent, usMaxPluCondLen);
}


/*
*===========================================================================
** Format  : USHORT PrtMldRoomCharge(UCHAR *pszWork, UCHAR *pszWork, UCHAR *pRoomNo, UCHAR *pGuestID);
*
*   Input  : UCHAR  *pRoomNo        -Pointer of Room Number
*            UCHAR  *pGuestID       -Pointer of Guest ID
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets room number and guest id.
*===========================================================================
*/

USHORT MldCPRoomCharge(TCHAR *pszWork, TCHAR *pRoomNo, TCHAR *pGuestID,
                        USHORT usDispType)
{
    TCHAR  aszSpecMnem1[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszSpecMnem2[PARA_SPEMNEMO_LEN + 1] = {0};
    USHORT  usRow;

	RflGetSpecMnem(aszSpecMnem1, SPC_ROOMNO_ADR);   /* get room no. mnemo */
	RflGetSpecMnem(aszSpecMnem2, SPC_GUESTID_ADR);  /* get guest id */

    if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
        usRow = MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldCPRoomCharge, aszSpecMnem1, pRoomNo, aszSpecMnem2, pGuestID);
    } else {
        usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldCPRoomCharge2, aszSpecMnem1, pRoomNo, aszSpecMnem2, pGuestID);
    }
    return(usRow);
}

/*
*===========================================================================
** Format  : VOID  MldFolioPost(UCHAR *pszWork, UCHAR *pszFolioNo, UCHAR *pszPostTranNo)
*
*    Input : UCHAR *pszFolioNo      -folio. number
*            UCHAR *pszPostTranNo   -posted transaction number
*
*   Output : UCHAR  *pszWork         -destination buffer address
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints "folio. and posted transaction number" line.
*            if "0", does not print.
*===========================================================================
*/
/**********
USHORT  MldFolioPost(UCHAR *pszWork, UCHAR *pszFolioNo, UCHAR *pszPostTranNo,
                      USHORT usDispType)
{

    WCHAR  aszSpecMnem1[PARA_SPEMNEMO_LEN + 1];
    WCHAR  aszSpecMnem2[PARA_SPEMNEMO_LEN + 1];
    USHORT  usRow;

    *aszSpecMnem1 = '\0';
    *aszSpecMnem2 = '\0';


    if ((*pszFolioNo != '\0') && (*pszPostTranNo != '\0')) {

        RflGetSpecMnem(aszSpecMnem1, SPC_FOLINO_ADR);   / get folio no. /
        RflGetSpecMnem(aszSpecMnem2, SPC_POSTTRAN_ADR); / get posted transaction no. /

        if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {

            usRow = MldSPrintf(pszWork,
                              MLD_PRECOLUMN1,
                              aszMldFolioPost,
                              aszSpecMnem1,
                              pszFolioNo,
                              aszSpecMnem2,
                              pszPostTranNo);
        } else {

            usRow = MldSPrintf(pszWork,
                              MLD_DR3COLUMN,
                              aszMldFolioPost2,
                              aszSpecMnem1,
                              pszFolioNo,
                              aszSpecMnem2,
                              pszPostTranNo);
        }
        return(usRow);

    }
    return(0);
}
*************/
/*
*===========================================================================
** Format  : USHORT MldCPRspMsgText(UCHAR *pszWork, UCHAR *pszRspMsgText);
*
*   Input  : UCHAR  *pszRspMsgText  -Pointer of Response Message Text
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets response message text for chrge posting.
*===========================================================================
*/
USHORT MldCPRspMsgText(TCHAR *pszWork, TCHAR *pszRspMsgText, USHORT usDispType)
{
    if (*pszRspMsgText != _T('\0')) {
		USHORT   usDestSize;

        if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
            usDestSize = MLD_PRECOLUMN1 + 1;
        } else {
            usDestSize = MLD_DR3COLUMN + 1;
        }

        /* copy response message (size of column) */
        tcharnset(pszWork, _T(' '), usDestSize);

        for (USHORT i = 0; i < usDestSize; i++) {
            *pszWork = *pszRspMsgText++;

            if (*pszRspMsgText == '\0') break;

            *pszWork++;
        }
        return(1);
    }
    return(0);
}

/*
*===========================================================================
** Format  : USHORT MldCPRspMsgText(UCHAR *pszWork, UCHAR *pszRspMsgText);
*
*   Input  : UCHAR  *pszRspMsgText  -Pointer of Response Message Text
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function sets response message text for chrge posting.
*===========================================================================
*/
USHORT MldCPDriveRspMsgText(TCHAR *pszWork, TCHAR pszRspMsgText[], USHORT usDispType)
{
	USHORT  usRow = 0;
	
	//
	//need to right align the numbers on the second line just line the total type.
	//follow the breakpoints to the total display to see how to display 
	// on the second line
	if (*pszRspMsgText != _T('\0')) {
		TCHAR   rspMsg1[11] = {0};
		TCHAR   rspMsg2[21] = {0};
		SHORT   x,y;

		for(x = 0; x < 10; x++){ 
			rspMsg1[x] = pszRspMsgText[x];
		}
		x = 11;
		y = 0;
		while(y < 20 && pszRspMsgText[x] && pszRspMsgText[x] != _T(' ')){
			rspMsg2[y] = pszRspMsgText[x];
			x++;
			y++;
		}
		
		usRow = MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldDriveRspMsg, rspMsg1, rspMsg2);
	}

	return usRow;
}

/*
*===========================================================================
** Format  : USHORT  MldOffline(UCHAR *pszWork, UCHAR fbMod,
*                           USHORT usDate UCHAR *aszApproval );
*
*    Input : UCHAR fbMod           -offline modifier
*            USHORT usDate         -expiration date
*            UCHAR  *aszApproval   -approval code
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function prints prints EPT offline symbol
*            and expiration date.
*===========================================================================
*/
USHORT  MldOffline(TCHAR *pszWork, TCHAR *auchDate, TCHAR *auchApproval,
                        USHORT usDispType)
{
    USHORT usRetLine = 0;
    TCHAR  aszApproval[NUM_APPROVAL+1] = {0};
    TCHAR  aszDate[NUM_EXPIRA+1] = {0};
    TCHAR  aszOffDate[NUM_EXPIRA + 2] = {0};

    if (*auchDate == '\0' && *auchApproval == '\0') {
        return(0);
    }
    _tcsncpy(aszApproval, auchApproval, NUM_APPROVAL);

 //   if ( *aszDate != '\0' ){                        /* expiration date  */
		//we need to have the option to mask the 
		//expiration date, due to a new
		//law put into effect 12.04,06 JHHJ
		if(CliParaMDCCheck(MDC_EPT_MASK3_ADR,ODD_MDC_BIT1))
		{
			_tcsncpy(aszDate, auchDate, NUM_EXPIRA);
		}else
		{
			_tcsncpy(aszDate, _T("XXXX"), NUM_EXPIRA);
		}
		if (aszDate[0] == _T('X')) {
			_tcsncpy(aszOffDate,_T("XX/XX"),NUM_EXPIRA+1);
		} else {
			USHORT usOffDate = _ttoi(aszDate);
			if (usOffDate != 0) {                       /* E-028 corr. 4/20     */
				RflSPrintf( aszOffDate, TCHARSIZEOF(aszOffDate), aszMldOffDate, usOffDate / 100, usOffDate % 100 );
			}
		}
//    }

    /* -- send mnemoncis and exp.date -- */
    if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
        MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldOffline, aszOffDate, aszApproval);
    } else {
        MldSPrintf(pszWork, MLD_DR3COLUMN, aszMldOffline2, aszOffDate, aszApproval);
    }
    return(1);
}


/*
*===========================================================================
** Format  : VOID  MldDouble(UCHAR *pszDest, USHORT  usDestLen,
*                                            UCHAR *pszSource)
*
*   Input  : USHORT       usDestLen      -destination buffer length in characters
*            UCHAR        *pszSource     -source buffer address
*   Output : UCHAR        *pszDest       -destination buffer address
*   InOut  : none
** Return  : none
*
** Synopsis: This function converts ASCII strings to double wide.
*            (insert "0x20"  behind character.)
*===========================================================================
*/
VOID  MldDouble(TCHAR *pszDest, USHORT usDestLen, TCHAR *pszSource)
{
    TCHAR  *pszDst  = pszDest;
    TCHAR  *pszSorc = pszSource;

    if (usDestLen == 0) {
        return ;
    }
    /* if original menmonic is double wide case */
    if (*pszSorc == RFL_DOUBLE) {
        _tcsncpy(pszDest, pszSource, usDestLen);
        return;
    }

    for ( ; (*pszSorc != '\0') && usDestLen > 0; pszSorc++ , usDestLen--) {

        *pszDst++ = _T(' ');   /* space char. instead of double wide char. */

        if ( *pszSorc == RFL_DOUBLE ) {
            continue;
        } else {
            *pszDst++ = *pszSorc;
        }
    }

    *pszDst = '\0';
}

/*
*===========================================================================
** Format  : SHORT MldDummyScrollDisplay(USHORT usType, USHORT usScroll)
*
*   Input  : USHORT       usScroll       -destination scroll display
*            USHORT       usType         -type of display
*   Output : none
*   InOut  : none
** Return  : displayed or not
*
** Synopsis: This function send data to display buffer for dummy control
*===========================================================================
*/
SHORT MldDummyScrollDisplay(USHORT usType, USHORT usScroll)
{
    TCHAR   aszScrollBuff[1] = {0};
	LONG    lAttrib[1] = {0};

    return (MldWriteScrollDisplay(aszScrollBuff, usScroll, usType, 0, MLD_DR3COLUMN, lAttrib));
}

/*
*===========================================================================
** Format  : SHORT MldCheckReverseVideo(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : reverse video or not
*
** Synopsis: This function checks mdc option whether reverse video or not
*===========================================================================
*/
SHORT MldCheckReverseVideo(VOID)
{
    /* check mdc of reverse video option */
    return((SHORT)CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT2));
}

/*
*===========================================================================
** Format  : SHORT MldGetTaxMod(UCHAR *pszDest,
*                               USHORT usDestLen,
*                               BOOL   fTax,
*                               UCHAR  fbMod);
*
*   Input  : USHORT usDestLen       -destination buffer length in characters
*            USHORT fsTax,          -US or Canadian Tax
*            UCHAR  fbMod           -modifier status
*
*   Output : UCHAR *pszDest         -destination buffer address
*   InOut  : none
*
** Return  : SHORT  PRT_WITHDATA    -tax modifier status on
*                   PRT_WITHOUTDATA -all tax modifiers' statuses  are off.
*
** Synopsis: This function gets Tax Modifier mnemonics. This mnemonic is used
*            when a Tax Modifier key is pressed. This is not from the PLU
*            control code settings for affecting Tax Itemizer.
*
*            This function gets Tax Modifier mnemonics for either the
*            Canadian tax system or the US tax system.
*
*            Tax Modifier key for International/VAT tax system is not allowed
*            so we do not need to worry about that. See ItemModTax().
*
*            See also PrtGetTaxMod().
*===========================================================================
*/
static SHORT  MldGetTaxMod(TCHAR *pszDest, USHORT usDestLen, SHORT fsTax, USHORT fbMod)
{
    USHORT         usWriteLen = 0;          /* written length */

	tcharnset(pszDest, 0, usDestLen);

	/*--------------
	   canadian tax
	  -------------- */
    if (fsTax & MODEQUAL_CANADIAN) {
		PARASPEMNEMO   SM = {0};                /* special mnemonics */

		SM.uchMajorClass = CLASS_PARASPECIALMNEMO;
        switch (PRT_TAX_MASK & fbMod) {

        case  CANTAX_MEAL << 1:
            SM.uchAddress    = SPC_TXMOD1_ADR;
            break;

        case  CANTAX_CARBON << 1:
            SM.uchAddress    = SPC_TXMOD2_ADR;
            break;

        case  CANTAX_SNACK << 1:
            SM.uchAddress    = SPC_TXMOD3_ADR;
            break;

        case  CANTAX_BEER << 1:
            SM.uchAddress    = SPC_TXMOD4_ADR;
            break;

        case  CANTAX_LIQUOR << 1:
            SM.uchAddress    = SPC_TXMOD5_ADR;
            break;

        case  CANTAX_GROCERY << 1:
            SM.uchAddress    = SPC_TXMOD6_ADR;
            break;

        case  CANTAX_TIP << 1:
            SM.uchAddress    = SPC_TXMOD7_ADR;
            break;

        case  CANTAX_PST1 << 1:
            SM.uchAddress    = SPC_TXMOD8_ADR;
            break;

        case  CANTAX_BAKED << 1:
            SM.uchAddress    = SPC_TXMOD9_ADR;
            break;

        default:
            return (PRT_WITHOUTDATA);
        }

        /* -- get tax  modifier mnemonics -- */
        CliParaRead( &SM );

        if (tcharlen(SM.aszMnemonics)  <= usDestLen) {
            _tcsncpy(pszDest, SM.aszMnemonics, PARA_SPEMNEMO_LEN);
        } else {
            return (PRT_WITHOUTDATA);
        }

        return (PRT_WITHDATA);
    }

	/*--------------
		 U S  tax
	  -------------- */

    /* get taxmodifier1 mnemo -- */
    if (fbMod & TAX_MODIFIER1) {
		WCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };


		RflGetSpecMnem(aszSpecMnem, SPC_TXMOD1_ADR);
        /* -- check destination buffer length ("1" for space) -- */
        if ( ( usWriteLen += (tcharlen(aszSpecMnem) + 1) ) < usDestLen ) {
            _tcscpy(pszDest, aszSpecMnem);
            *(pszDest+usWriteLen-1) = _T(' ');

        } else {
            return (PRT_WITHOUTDATA);
        }
    }

    /* get taxmodifier2 mnemo -- */
    if (fbMod & TAX_MODIFIER2) {
		WCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };

		RflGetSpecMnem(aszSpecMnem, SPC_TXMOD2_ADR);

        /* -- check destination buffer length ("1" for space) -- */
        if ( ( usWriteLen += (tcharlen(aszSpecMnem) + 1) ) < usDestLen ) {
            _tcscat(pszDest, aszSpecMnem);
            *(pszDest+usWriteLen-1) = _T(' ');
        } else {
            return (PRT_WITHOUTDATA);
        }
    }

    /* get taxmodifier3 mnemo -- */
    if (fbMod & TAX_MODIFIER3) {
		WCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };

		RflGetSpecMnem(aszSpecMnem, SPC_TXMOD3_ADR);

        if ( (usWriteLen += tcharlen(aszSpecMnem)) < usDestLen ) {
            _tcscat(pszDest, aszSpecMnem);
            *(pszDest+usWriteLen-1) = 0x20;
        } else {
            return (PRT_WITHOUTDATA);
        }
    }
    /* --- get FS Mod. mnemo,   Saratoga --- */
    if (fbMod & FOOD_MODIFIER) {
		WCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };

		RflGetSpecMnem(aszSpecMnem, SPC_FSMOD_ADR);
		if ((usWriteLen += tcharlen(aszSpecMnem)) < usDestLen) {
            _tcscat(pszDest, aszSpecMnem);
        } else {
            return (PRT_WITHOUTDATA);
        }
    }

    /* -- delete last space characeter --*/
    if (pszDest[usWriteLen] == _T(' ')) {
        pszDest[usWriteLen] = '\0';
    }

    if ( *pszDest != '\0' ) {
        return (PRT_WITHDATA);

    } else {
        return (PRT_WITHOUTDATA);
    }
}

/*
*===========================================================================
** Format   : VOID PrtSetItem( UCHAR     *puchDest,
*                              ITEMSALES *pItem,
*                              LONG      *plAllPrice,
*                              SHORT     sFormatType )
*
*   Input   : ITEMSALES *pItem      - address of source sales item
*             SHORT     sType       - type of print format
*                                       PRT_ONLY_NOUN
*                                       PRT_ONLY_PMCOND
*                                       PRT_ALL_ITEM
*
*   Output  : UCHAR     *pDest      - destination buffer address
*             LONG      *plAllPrice - price of all products
*
*   InOut   : none
** Return   : none
*
** Synopsis : This function prepares a print format of a sales item with
*           adjective and mnemoic and print modifiers, condiments.
*
*   WARNING:  condiment prices for condiments that are PPI PLUs are updated by using
*             function ItemSalesCalcCondimentPPIReplace() so the condiment prices
*			  in ITEMSALES pItem are updated if there are PPI PLUs specified in the
*			  item data.
*===========================================================================
*/
VOID MldSetItem( TCHAR *puchDest, ITEMSALES *pItem, DCURRENCY *plAllPrice,
								USHORT usAmtLen, LONG *plAttrib, USHORT usDispType)
{
    TCHAR       *puchWrtPtr;
	LONG        lQTY;
    LONG        lAttrib, lDefAttrib;
    BOOL        fIsDeptItem;
    USHORT      usWrtLen, usWrtLen2, usSaveLen, usCondiLength,usQtyMainItem;
    USHORT      usMaxChild;
    USHORT      usI, i, j;
    USHORT      usPriority = 0, usPrintMod;
	USHORT		usDblChar;
    TCHAR       aszSeatNo[] = _T("s1 \0");
	
	//colorpalette-changes
	int filledLength = 0; 
	int condiCount = 0;
	int spacesNeeded = 0;

    /* set qty by void consolidation */
	lQTY = DetermineQuantity (pItem);

    /* ---- check mdc of print modifier & condiment plu priority --- */
    if ( CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT0 ) && CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT2 )) {
         usPriority = 1;
    }

    /* initialize print modifier/condiment reverse video control flag */
    usSaveLen = MLD_QTY_LEN;
    if ((CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT2)) ) {
        lDefAttrib = MLD_DEFAULT_REVERSE;
    } else {
        lDefAttrib = 0L;
    }
    lAttrib = lDefAttrib;

    puchWrtPtr = puchDest;

#if 1
	*plAllPrice = DeterminePrice (pItem, lQTY);
	*plAllPrice += ItemSalesCalcCondimentPPIReplaceQty (pItem, lQTY);
#else
//    *plAllPrice = pItem->lUnitPrice * lQTY;
	*plAllPrice = DeterminePrice (pItem, lQTY);
#endif

    /* --- set seat no if entred R3.1 --- */

    if ((pItem->uchSeatNo) && (pItem->uchMinorClass != CLASS_ITEMORDERDEC)) {
        aszSeatNo[1] = (TCHAR)(pItem->uchSeatNo | 0x30);
        _tcsncpy( puchWrtPtr, aszSeatNo, 3 );
        puchWrtPtr += 3;
        if (lAttrib) lAttrib = 0x00000040L;
        usSaveLen += 3;//usSaveLen += 4;
    }

    /* --- determine source item is department item or not --- */

    fIsDeptItem = (( pItem->uchMinorClass == CLASS_DEPT ) ||
                   ( pItem->uchMinorClass == CLASS_DEPTITEMDISC ) ||
                   ( pItem->uchMinorClass == CLASS_DEPTMODDISC ));

		/* --- prepare main sales item's mnemonic (and adjective) --- */

		//if PLUNo is in the reserved range, do NOT perform adjective mnemonics
		if ((_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) < 0) ||
			(_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) > 0)) {

			if (( pItem->uchAdjective ) && ( ! fIsDeptItem ) && pItem->uchMinorClass != CLASS_ITEMORDERDEC) {

				/* --- prepare adjective mnemonic, if main item is adjective --- */
				RflGetAdj ( puchWrtPtr, pItem->uchAdjective );

				/* --- make a space between adjective mnemonic
						and main sales item's mnemonic --- */
				usWrtLen = tcharlen( puchWrtPtr );
				*( puchWrtPtr + usWrtLen ) = _T(' ');

				//move the pointer one for the space just added
				puchWrtPtr += ( usWrtLen + 1 );

				//move the write length one for the space just added
				usWrtLen ++;

				/* set reverse video control flag */
		
				//set the number of double wide characters to zero
				//then count the number in the string
				usDblChar = 0;
				usDblChar += CountDblWidthChar(puchDest);
				//if the length of the qty+adj+amount is greater than what will fit on one line
				//taking into account the number of double wide characters
				//set up the next line of attributes(screen color) to the default
				//either reversed or not
				//in this case the savelen is the qty
				//the wrtlen is the adj
				if (usSaveLen + usWrtLen + usDblChar > MLD_PRECOLUMN1) {
					plAttrib++;
					lAttrib = lDefAttrib;
					usSaveLen = MLD_QTY_LEN;
				//otherwise adjust the current line's attributes(screen color)
				} else {
					for(i=0; i<usWrtLen; i++)
					{
						lAttrib = (lAttrib<<1);
					}
					plAttrib += usWrtLen; // move Attrib for Adj. length
					usSaveLen += usWrtLen;
				}
			}

			/* --- prepare main item mnemonic --- */

			usWrtLen = tcharlen( pItem->aszMnemonic );
			_tcsncpy( puchWrtPtr, pItem->aszMnemonic, usWrtLen );

			/* --- make a space between main sales item's mnemonic and
					   print modifier's mnemonic --- */

			*( puchWrtPtr + usWrtLen ) = _T(' ');

			//move the pointer one for the space just added
			puchWrtPtr += ( usWrtLen + 1 );

			usWrtLen++;
			/* set reverse video control flag */
			//set the number of double wide characters to zero
			//then count the number in the string
			usDblChar = 0;
			usDblChar += CountDblWidthChar(puchDest);
			//if the qty+adj+item mnem+amount is greater than what will fit on one line
			//taking into account the number of double wide characters
			//set up the next line of attributes(screen color) to the default
			//either reversed or not
			//in this case the savelen is qty+adj
			//the wrtlen is item mnemonic
			if (usSaveLen + usWrtLen + 1/*+usAmtLen*/+ usDblChar > MLD_PRECOLUMN1) {
				plAttrib++;
				lAttrib = lDefAttrib;
				usSaveLen = MLD_QTY_LEN;
			//otherwise continue changing the current line
			} else {
				for (i = 0; i < usWrtLen; i++){
					//lAttrib = (lAttrib<<1);
					//we know that this is the main item, so the correct
					//color code is already in our plAttrib Location, so we will
					//simply advance the pointer to the correct column for the next item,
					//if it exists.
					plAttrib++;
				}
				usSaveLen += usWrtLen;
			}
		}

    /* --- prepare print modifiers and condiment items --- */

	filledLength = 0;
	filledLength = _tcslen(pItem->aszMnemonic) + 1;

    usMaxChild = ( pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo );

	NHPOS_ASSERT(usMaxChild <= sizeof(pItem->Condiment)/sizeof(pItem->Condiment[0]));
	NHPOS_ASSERT(usMaxChild <= sizeof(pItem->auchPrintModifier)/sizeof(pItem->auchPrintModifier[0]));

	if (usMaxChild > sizeof(pItem->Condiment)/sizeof(pItem->Condiment[0])) {
		usMaxChild = sizeof(pItem->Condiment)/sizeof(pItem->Condiment[0]);
	}

    for (usPrintMod=0; usPrintMod<(USHORT)(usPriority ? 2 : 1); usPrintMod++) {
    for ( usI = 0; usI < usMaxChild; usI++ ) {

		condiCount = 0;

        if ((usPriority) && (usPrintMod)) {
            ;   /* if print modifier is already displayed */
        } else if ( pItem->auchPrintModifier[ usI ] ) {
			PARAPRTMODI PrtMod = {0};

            /* --- prepare print modifier mnemonic --- */
			PrtMod.uchMajorClass = CLASS_PARAPRTMODI;
            PrtMod.uchAddress = pItem->auchPrintModifier[ usI ];
            CliParaRead( &PrtMod );

            usWrtLen = tcharlen( PrtMod.aszMnemonics );
            if (!usWrtLen) continue;
            _tcsncpy( puchWrtPtr, PrtMod.aszMnemonics, usWrtLen );

            /* --- make a space between mnemonic --- */
            *( puchWrtPtr + usWrtLen ) = _T(' ');
			//move the pointer one for the space just added
            puchWrtPtr += ( usWrtLen + 1 );

			//move the write length for the space just added
			usWrtLen++;

			//set the number of double wide characters to zero
			//then count the number in the string
			usDblChar = 0;
			usDblChar += CountDblWidthChar(puchDest);

            /* set reverse video control flag */
            for (i=0,usWrtLen2=0; i<usWrtLen; i++) {
                usWrtLen2++;
                if ((PrtMod.aszMnemonics[i] != 0x20)&&(i<(usWrtLen-1))) {
                    continue;   /* check space in the mnemonic */
                }
                if ((usSaveLen+usWrtLen2-(i<(usWrtLen-1)?1:0)/*+usAmtLen*/+usDblChar) > MLD_PRECOLUMN1) {
                    plAttrib++;
                    lAttrib = lDefAttrib;
                    usSaveLen = MLD_QTY_LEN;
                }
                /**** reverse video between noun plu and amount ****/
                for (j=0; j<(USHORT)(MLD_PRECOLUMN1-(usSaveLen/*+usAmtLen*/)); j++) {
/***                for(j=0; j<usWrtLen2; j++) { ***/
                    *plAttrib |= lAttrib;   /* set reverse video flag */
                    lAttrib = (lAttrib << 1);
                }
                lAttrib = 0L;   /* to avoid duplicate reverse set in 1 line */
                usSaveLen += usWrtLen2;
                usWrtLen2 = 0;
            }
            usSaveLen++;    /* set space between mnemonic, not reverse video */
/***            lAttrib = (lAttrib<<1); ***/
        }

        if ((usPriority) && (!usPrintMod)) {
            ;   /* if print modifier is not displayed */
        } else if ( RflIsSpecialPlu (pItem->Condiment[ usI ].auchPLUNo, MLD_NO_DISP_PLU_DUMMY) != 0 ) {
			 //display mnemonics only if no #/Type entry OR if peforming 'display all transaction'
			  //OR if performing 'noun item entry' on a discouned item US Customs cwunn
	            /* --- prepare condiment item's mnemonic --- */
				if ((( USHORT )pItem->uchChildNo <= usI ) && ( ! fIsDeptItem )) {

					usWrtLen = tcharlen( pItem->Condiment[ usI ].aszMnemonic );
					if (!usWrtLen) continue;
					_tcsncpy( puchWrtPtr, pItem->Condiment[ usI ].aszMnemonic, usWrtLen );

					/* --- make a space between mnemonic --- */
					usWrtLen = tcharlen( puchWrtPtr );
					*( puchWrtPtr + usWrtLen ) = _T(' ');
					//move the pointer one for the space just added
					puchWrtPtr += ( usWrtLen + 1 );
					//move the write length one for the space just added
					usWrtLen++;

					/* set reverse video control flag */
					for (i = 0,usWrtLen2 = 0; i < usWrtLen; i++) 
					{
						usWrtLen2++;
						if ((pItem->Condiment[ usI ].aszMnemonic[i] != 0x20) && (i < (usWrtLen-1))) 
						{
							continue;   /* check space in the mnemonic */
						}

						//set the number of double wide characters to zero
						//then count the number in the string
						usDblChar = 0;
						usDblChar += CountDblWidthChar(puchDest);
						usQtyMainItem = usSaveLen;

						//usQtyMainItem(usSaveLen) is the count (in total) of main item(variable), quantity(3), and space after 
						//main item (1), and any condiment that it has ALREADY written(variable) and a space after it(1), not counting
						//the current Condiment located in array element usI. usWrtLen2 is the length of the current Condiment, so 
						//we do an addition to see if the current information plus the current condiment is longer than a display
						//line.
						if ((usQtyMainItem+usWrtLen2-(i<(usWrtLen-1)?1:0)/*+usAmtLen*/ +usDblChar) > MLD_PRECOLUMN1+1) {

							// add space for quantity(3) and then the spaces needed to fill the rest of the line
							spacesNeeded = MLD_QTY_LEN + (MLD_PRECOLUMN1+1 - usQtyMainItem);

							plAttrib += spacesNeeded;	// colorpalette-changes -- need to add for number of 
														//  spaces before condiment is displayed (so correct color is used)

							usSaveLen = MLD_QTY_LEN;
						}

						/**** reverse video between noun plu and amount ****/
						//Add +3 to shade all 29 columns appropriately US Customs SCER cwunn 4/8/03

						usCondiLength = usWrtLen2 - 1;

						for (j=0; j<(USHORT)(usCondiLength+1/*MLD_PRECOLUMN1-(usSaveLen+usWrtLen2/*+usAmtLen*//*)*/); j++) {
	/***                    for(j=0; j<usWrtLen2; j++) { ***/
							if(pItem->Condiment[ usI ].uchCondColorPaletteCode)
							{
								memset(plAttrib+usAmtLen, pItem->Condiment[ usI ].uchCondColorPaletteCode, /*usWrtLen2*/sizeof(*plAttrib)); //color-palette-changes
							}
							plAttrib++;

							// if there are double wide characters, subtract the number, because we have accounted for 
							// the double wide characters to be affected.
							if(usDblChar > 0)
							{
								usDblChar--;
							}
						}
						/* if there are left over double characters, that means that it has been pushed to the next line, 
							so we must now add one to the attribute pointer to move it to the next line, and switch the attributes 
							for the new line */
						if(usDblChar)
						{
							plAttrib++;
							lAttrib = lDefAttrib;
							while(usDblChar)
							{
								*plAttrib |= lAttrib;   /* set reverse video flag */
								lAttrib = (lAttrib << 1);
								usDblChar--;
							}
						}

						lAttrib = 0L;   /* to avoid duplicate reverse set in 1 line */
						usSaveLen += usWrtLen2;
						usWrtLen2 = 0;
					}

					/* --- get all product price --- */
// full price including condiments already calculated in ItemSalesCalcCondimentPPIReplace() above.
//					*plAllPrice += (lQTY * pItem->Condiment[usI].lUnitPrice);

				} /* end if */
        } /* end if else */
    } /* end for usI */
    } /* end for usPrintMod */

    /* --- delete last space chara, and make null terminator --- */
    *(puchWrtPtr--) = '\0';
}
/*
*===========================================================================
** Format  : USHORT  MldItems(UCHAR  *pszWork, ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*
** Return  : USHORT usSetLine     -number of line(s) to be set
*
** Synopsis: This function sets the mnemonics of adjective, noun,
*            print modifier, conidiment, and product amount.
*
*   WARNING:  condiment prices for condiments that are PPI PLUs are updated by using
*             function ItemSalesCalcCondimentPPIReplace() so the condiment prices
*			  in ITEMSALES pItem are updated if there are PPI PLUs specified in the
*			  item data.
*===========================================================================
*/
static USHORT  MldListItems(TCHAR  *pszWork, ITEMSALES  *pItem, LONG *plAttrib, USHORT usPrice)
{
    TCHAR  szColWork[MLD_PRECOLUMN1 + 1] = {0};
    TCHAR  szModCond[20 + 1] = {0};
    TCHAR   *pWork;
    USHORT  usSetLine = 0, usSetQty = 0;
    LONG        lQTY, lAttrib, lDefAttrib;
    BOOL        fIsDeptItem,fIsFirst = TRUE;
    USHORT      usMaxChild;
    USHORT      usI, i;
    size_t      len, len2, lenadj;
    USHORT      usPriority = 0, usPrintMod, usWrite;
	USHORT		usDblChar = 0, usDblItemChar = 0;
	USHORT	usAmtLen = 0;
	USHORT	usCondCount = 0, usWriteLength, usModCondMnemLen, usPLULength, usMnemonicLen;
    TCHAR       aszSeatNo[] = _T("s1 \0");
	TCHAR   aszAmt[MLD_AMOUNT_LEN + 1 + 1] = {0};  // 1 + 1 ( space + terminate)


	if (pItem->uchMinorClass == CLASS_ITEMORDERDEC)
	{
		usPrice = 0;
	}

	usWriteLength		= MLD_PRECOLUMN1;
	usModCondMnemLen	= 20;
	usPLULength			= 26;
	

    /* ---- check mdc of print modifier & condiment plu priority --- */
    if ( CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT0 ) && CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT2 )) {
         usPriority = 1;
    }

    /* ---- check mdc of reverse video of condiment & print modifier --- */
    if ((CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT2)) ) {
        lDefAttrib = MLD_DEFAULT_REVERSE;
    } else {
        lDefAttrib = 0L;
    }
    lAttrib = lDefAttrib;

    fIsDeptItem = ((pItem->uchMinorClass == CLASS_DEPT) || (pItem->uchMinorClass == CLASS_DEPTITEMDISC) || (pItem->uchMinorClass == CLASS_DEPTMODDISC));

    /* set qty by void consolidation */
	lQTY = DetermineQuantity (pItem);

    /* set over 1000 qty case after item consolidation case */
    if ((pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) || (pItem->uchMinorClass == CLASS_ITEMORDERDEC)) {
        usSetQty = 1;
    } else if ((lQTY > 999L) || (lQTY < -99L)) {
		TCHAR  szQty100Work[16] = {0};

		//======Begin Quantity Fix, JHHJ 5-24-04======//
		tcharnset(pszWork, _T(' '), MLD_PRECOLUMN1);/* Set buffer to all spaces*/
		RflSPrintf(szQty100Work, TCHARSIZEOF(szQty100Work), aszMldQty2, lQTY); 

		/* set the formatted string to the buffer to the correct format with the quantity*/																								
		len = _tcslen(szQty100Work);
		/*get the length of the actual buffer, and takes out the null character*/
		_tcsncpy(pszWork, &szQty100Work[0], len);

		/*copy the correct quantity to the buffer WITHOUT the null character, which was causing the string
		to be terminated too early*/
        pszWork += usWriteLength + 1;
        usSetLine++;
        plAttrib++;             /* feed reverse video control flag */
        lAttrib = lDefAttrib;   /* set default reverse video position */
        usSetQty = 1;
    }

    /* --- set qty, adjective and noun plu name --- */
    tcharnset(szColWork, _T(' '), usWriteLength);
    pWork = &szColWork[3];
    len2 = 0;

    /*** set qty ***/
    if (!usSetQty) {
		TCHAR  szQtyWork[16] = {0};

        RflSPrintf(szQtyWork, TCHARSIZEOF(szQtyWork), aszMldQty1, lQTY);
        len = _tcslen(szQtyWork);
        switch(len) {
        case 1:     /* 1 digit */
            _tcsncpy(&(szColWork[1]), szQtyWork, 1);
            break;

        case 2:     /* 2 digits or minus 1 digit */
            _tcsncpy(szColWork, szQtyWork, 2);
            break;

        case 3:     /* 3 digits or minus 2 digits */
            _tcsncpy(szColWork, szQtyWork, len);
            break;

        default:    /* invalid qty */
            break;
        }
    } else {
        len = MLD_QTY_LEN;
    }

    /* --- set seat no if entred R3.1 --- */

    if ((pItem->uchSeatNo) && (pItem->uchMinorClass != CLASS_ITEMORDERDEC)) {
        if (usSetQty) {     /* scalable/4digit qty */
            aszSeatNo[1] = (UCHAR)(pItem->uchSeatNo | 0x30); //SR206
            _tcsncpy( &szColWork[0], aszSeatNo, 2);   /* set seat# on qty area */			
        } else {
            aszSeatNo[1] = (UCHAR)(pItem->uchSeatNo | 0x30);
            _tcsncpy( &szColWork[4], aszSeatNo, 2);   /* set seat# after qty */
            _tcsncpy( pszWork, &szColWork[0], usWriteLength);
            pszWork += usWriteLength + 1;
            plAttrib++;
            usSetLine++;
			MLD_SETLINE_COUNTINCREMENT(usSetLine);
            tcharnset(szColWork, _T(' '), usWriteLength);
        }
    }

    /* display price of DEPT/PLU or not */
    if (usPrice) {
#if 1
		DCURRENCY   lPrice = DeterminePrice (pItem, lQTY);
		lPrice += ItemSalesCalcCondimentPPIReplaceQty (pItem, lQTY);
#else
		DCURRENCY   lPrice = DeterminePrice (pItem, lQTY);

		// We start with the lProduct price since this may be a PPI item.
		// If the item has a linked PLU then uchChildNo will be non-zero
		// and the linked PLU will be in the first element of the Condiment[] array.
		for( i= pItem->uchChildNo; i < pItem->uchCondNo; i++)
		{
			lPrice += (pItem->Condiment[i].lUnitPrice * lQTY);
		}
#endif
		RflSPrintf(aszAmt, TCHARSIZEOF(aszAmt), aszItemAmt, lPrice);
		usAmtLen = tcharlen(aszAmt);
    } else {
        usAmtLen = 0;
    }

	// Check to see if this is a No Display PLU meaning that the PLU number
	// is in the range of the PLUs that are used not as regular PLUs but to
	// drive the OEP window functionality so act as dummy PLUs.
    if ((_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) < 0) || (_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) > 0)) {

		//Count the number of double wide characters in the main Item
		usDblItemChar += CountDblWidthChar(pItem->aszMnemonic);

        if (( pItem->uchAdjective ) && ( ! fIsDeptItem ) && pItem->uchMinorClass != CLASS_ITEMORDERDEC) {
			TCHAR   szAdjWork[PARA_ADJMNEMO_LEN + 1] = {0};

            /* --- prepare adjective mnemonic, if main item is adjective --- */
            RflGetAdj ( szAdjWork, pItem->uchAdjective );
            lenadj = _tcslen(szAdjWork);

			//Count the number of double wide characters in the adj
			//and add thos to the number in the main item
			usDblItemChar += CountDblWidthChar(szAdjWork);

            _tcsncpy( pWork, szAdjWork, lenadj);

            /* increment reverse video control flag, for not reverse video */
            for (i = 0; i < lenadj + 1; i++) {
                lAttrib = (lAttrib << 1);
            }

			//if the lenght of the main item + the adj is less than the width of the line
			//then copy that length
            if ((len = _tcslen(pItem->aszMnemonic))+lenadj+1 < (size_t)(usPLULength-usDblItemChar)) {

                pWork += lenadj;
                pWork++;
                _tcsncpy( pWork, pItem->aszMnemonic, len);
				pWork += len;
				pWork += (MLD_PRECOLUMN1-(MLD_QTY_LEN + 1+ usAmtLen+lenadj + len));
                _tcsncpy( pWork, &aszAmt[0], usAmtLen);
                _tcsncpy( pszWork, &szColWork[0], usWriteLength);
                pszWork += usWriteLength;
                usSetLine ++;
				// Since this is a continuation line, we will need to indicate
				// this by using the higher order byte and incrementing the continuation
				// line count that is stored there.  See usage of MLD_LINESTATUS_CONTLINE
				// in MldWriteScrollDisplay1() and the other MldWriteScollDisplay functions.
				MLD_SETLINE_COUNTINCREMENT(usSetLine);
                plAttrib++;             /* feed reverse video control flag */
                lAttrib = lDefAttrib;   /* set default reverse video position */
            } else {
				//otherwise copy everything up to the width of the column
				//not including the Main Item
                _tcsncpy( pszWork, &szColWork[0], usWriteLength);
                pszWork += usWriteLength + 1;
                usSetLine ++;
                plAttrib++;             /* feed reverse video control flag */
                lAttrib = lDefAttrib;   /* set default reverse video position */

				//if the main item is longer than will fit only copy up to the width of the column
				//otherwise copy the length of the main item
                if ((len = _tcslen(pItem->aszMnemonic)) + usDblItemChar >= MLD_DRIVE_DEPT_PLU_LEN) {
                    tcharnset(pszWork, _T(' '), usWriteLength);
					usWrite = tcharlen(pItem->aszMnemonic);
                    _tcsncpy( &(pszWork[3]), pItem->aszMnemonic, usWrite/*MLD_DRIVE_DEPT_PLU_LEN*/ - usDblItemChar);
                    pszWork += usWriteLength + 1;
                    usSetLine ++;
					MLD_SETLINE_COUNTINCREMENT(usSetLine);
                    plAttrib++;             /* feed reverse video control flag */
                    lAttrib = lDefAttrib;   /* set default reverse video position */
                } else {
                    tcharnset(szColWork, _T(' '), usWriteLength);
                    _tcsncpy( pWork, pItem->aszMnemonic, len);
                    pWork += len + 1;
                    len2 = len;
                    for(i=0; i<len+1; i++) {
                        lAttrib = (lAttrib << 1);
                    }
                }
            }
        } else 
		{
			usWrite = tcharlen(pItem->aszMnemonic);
			_tcsncpy( pWork, pItem->aszMnemonic, usWrite - usDblItemChar);
			_tcsncpy( pszWork, &szColWork[0], (usWriteLength- usAmtLen));
			pszWork += (usWriteLength - usAmtLen);
			_tcsncpy( pszWork, &aszAmt[0], usAmtLen);
			pszWork += usAmtLen;

            usSetLine ++;
			MLD_SETLINE_COUNTINCREMENT(usSetLine);
            plAttrib++;             /* feed reverse video control flag */
            lAttrib = lDefAttrib;   /* set default reverse video position */
			usDblItemChar = 0;
        }
    } else {
        len = 0;
        len2=0;
    }

    if ((!len2) && (!len)) {     /* for plu mnemonics is null case */
        len2++;
        lAttrib = (lAttrib << 1);   /* reverse video */
    }
    if (!len2) {
        tcharnset(szColWork, _T(' '), usWriteLength);
        pWork = &szColWork[3];
    }

    /* --- prepare print modifiers and condiment items --- */

    usMaxChild = (pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo);

    for (usPrintMod=0; usPrintMod<(USHORT)(usPriority?2:1); usPrintMod++) {
		for ( usCondCount, usI = (USHORT)((usPriority&&!usPrintMod)?0:pItem->uchChildNo); usI < usMaxChild; usI++, usCondCount++) {

			memset(aszAmt, 0, sizeof(aszAmt));
			RflSPrintf(aszAmt, TCHARSIZEOF(aszAmt), aszItemAmt, (DCURRENCY)pItem->Condiment[usI].lUnitPrice );
			usAmtLen = tcharlen(aszAmt);

		usDblChar = 0;//initaize the number of double width characters to 0
        /* --- display print modifier before condiment at priority option ---- */
        if ( (!usPrintMod) && (pItem->auchPrintModifier[usI]) ) {
			RflGetMnemonicByClass(CLASS_PARAPRTMODI, szModCond, pItem->auchPrintModifier[ usI ]);
			//count the number of double wide characters and add
			usDblChar += CountDblWidthChar(szModCond);
        } else {
            if ((usPriority) && (!usPrintMod)) continue;

            /* --- prepare condiment item's mnemonic --- */
            if ( ! fIsDeptItem ) {
                if (RflIsSpecialPlu (pItem->Condiment[usI].auchPLUNo, MLD_NO_DISP_PLU_DUMMY) != 0) {
                    _tcsncpy(szModCond, pItem->Condiment[usI].aszMnemonic, usModCondMnemLen);

					//count the number of double wide characters and add
					usDblChar += CountDblWidthChar(szModCond);
                } else {
                    continue;
                }
            } else {
                continue;
            }
        }
        len = _tcslen(szModCond);
        if (!len) continue;

		//Reset the Main Item double Characters to 0 because
		//we have already taken them into account
		usDblItemChar = 0;
        if (len2) 
		{     /* if priveous data is not restored */
            //if a value exist in len2 copy the item to the buffer
			//only copying up to the screen width
			if(  usCondCount)
			{
				tcharnset(pszWork, _T(' '), 1);
				pszWork++;	
			}
			_tcsncpy( pszWork, szColWork, usWriteLength);
            pszWork += usWriteLength /*+ 1JHHJ*/;
			//JHHJ BEGIN NEW
			/*tcharnset(pszWork, _T(' '), 1);
			pszWork++;*/
			//JHHJ BEGIN NEW
            usSetLine ++;
            plAttrib++;             /* feed reverse video control flag */
            lAttrib = lDefAttrib;   /* set default reverse video position */
            tcharnset(szColWork, _T(' '), usWriteLength);
            pWork = &szColWork[3];
            len2=0;
			//if the length of the condiment is greater than can be displayed
			//only copy up to the screen width
            if (len + usDblChar >= MLD_DRIVE_DEPT_PLU_LEN)
			{
				tcharnset(pszWork, _T(' '), usWriteLength);
				usMnemonicLen = tcharlen(szModCond);
				_tcsncpy( &(pszWork[4]), szModCond, usMnemonicLen);

				pszWork += usWriteLength;
				tcharnset(pszWork, _T(' '), 1);
                    				
				usSetLine ++;

				// Setting color attribute - CSMALL
				// 'plAttrib' offset as follows: ((length of line)*(number of lines))+qtyLength)
				memset( (plAttrib+((usI+1)*usWriteLength)+MLD_QTY_LEN), pItem->Condiment[usI].uchCondColorPaletteCode, (len+1)*sizeof(plAttrib));
                
                lAttrib = lDefAttrib;   /* set default reverse video position */
                continue;
            //otherwise only copy the length of the condiment
			} else 
			{
                _tcsncpy( pWork, szModCond, len);
                 pWork += usPLULength;
				//_tcsncpy( pWork, &aszAmt[0], usAmtLen);
                if (len2) len2++;
				//len2 is used to determine if it plus any new items
				//will fit on the screen so it needs to take into account
				//any double width characters it has because we will reset
				//usDblChar for the next condiment
                len2 += len + usDblChar;

				// Setting color attribute - CSMALL
				// 'plAttrib' offset as follows: ((length of line)*(number of lines))+qtyLength)
				memset( (plAttrib+((usI+1)*usWriteLength)+MLD_QTY_LEN), pItem->Condiment[usI].uchCondColorPaletteCode, (len+1)*sizeof(plAttrib));
       
				continue;
            }
        } else 
		{
			if(usCondCount /*&& pItem->Condiment[usI].lUnitPrice*/)
			{
				tcharnset(pszWork, _T(' '), 1);
				pszWork++;	
			}

			usMnemonicLen = tcharlen(szModCond);
            tcharnset(pszWork, _T(' '), usWriteLength);
            _tcsncpy( &(pszWork[4]), szModCond, usMnemonicLen);
			pszWork += usWriteLength;
			tcharnset(pszWork, _T(' '), 1);
           
            usSetLine ++;

			// Setting color attribute - CSMALL
			// 'plAttrib' offset as follows: ((length of line)*(number of lines))+qtyLength)
			memset( (plAttrib+((usI+1)*usWriteLength)+MLD_QTY_LEN), pItem->Condiment[usI].uchCondColorPaletteCode, usMnemonicLen*sizeof(plAttrib));
            
			plAttrib++;             /* feed reverse video control flag */

            lAttrib = lDefAttrib;   /* set default reverse video position */
            tcharnset(szColWork, _T(' '), usWriteLength);
            pWork = &szColWork[3];
            len2=0;
            continue;
        }
		usDblItemChar = 0;

    } /* end for */
    }

    if (len2)      /* restore condiments */
	{
		if(usCondCount)
		{
			tcharnset(pszWork, _T(' '), 1);		
			pszWork++;
		}
		_tcsncpy( pszWork, szColWork, usWriteLength);
        pszWork += usWriteLength + 1;
        usSetLine ++;
    }

    return(usSetLine);
}


/*
*===========================================================================
** Format  : SHORT  PrtGet1Line(UCHAR *pStart, UCHAR *pEnd, USHORT usColumn);
*
*   Input  : UCHAR      *pStart        -search start address
*            USHORT     usColumn       -Column Number
*
*   Output : UCHAR      **pEnd,        -end address
*
*   InOut  : none
** Return  : SHORT      PRT_CONTINUE   -string continue
*                       PRT_LAST       -string end
*
** Synopsis: This function searches last space or null with specified column no.
*            when function searches space or null, it moves current pointer to the point
*            that current pointer + usColumn.
*
*            The function works by following rules.
*            -moved pointer points "space"
*               function returns immediatery
*            -moved pointer points "NULL"
*               function decides last strings
*            -moved pointer points other character
*               function moves pointer back until pointer points "space"
*
*            So when function searches last 1 line, there needs enough
*            work space (ALL ITEM's bytes + Column's bytes) filled with NULL
*            to point NULL.
*
*===========================================================================
*/
SHORT  MldGet1Line(TCHAR *pStart, TCHAR **pEnd, USHORT usColumn)
{
    TCHAR  *puchWork = pStart + usColumn;
    USHORT i;

    if ( *puchWork == _T(' ')) {
        *pEnd = puchWork;
        return (PRT_CONTINUE);
    } else if ( *puchWork == '\0' ) {
        return (PRT_LAST);
    }

    for ( i = 0; i <=usColumn ; i++ ) {
        if ( *(puchWork - i)  == _T(' ')) {
            *pEnd = puchWork - i;
            return (PRT_CONTINUE);
        }
    }
	
	*pEnd = pStart + usColumn;
	
	return (PRT_CONTINUE);
}


/*
*===========================================================================
** Format  : SHORT  MldChkVoidConsolidation(ITEMSALES *pItem);
*
*   Input  : ITEMSALES *pItem
*   Output : none
*   InOut  : none
** Return  :
*
** Synopsis:  Determine whether void consolidation is enabled and should be
*             applied to this item.  Basically what we do is to check to 
*             see if the void consolidation option is turned on and if it is
*             turned on we check to see if this particular item is subject
*             to a void consolidation.
*===========================================================================
*/
SHORT  MldChkVoidConsolidation(ITEMSALES *pItem)
{
    if ((uchMldSystem2 == MLD_SPLIT_CHECK_SYSTEM) || (CliParaMDCCheckField(MDC_DEPT3_ADR, MDC_PARAM_BIT_B) && CliParaMDCCheckField(MDC_DISPLAY_ADR, MDC_PARAM_BIT_D))) {
		// Check to see if this is a non-void item that has a void item
		// associated with it.  It it is a normal (no-void item) and there
		// is a value for the usOffset then this item has a void item associated
		// with it.  So then lets determine if this item is a candidate for
		// void consolidation by checking its minor class as well as whether
		// it is a PPI item
        if (pItem->usItemOffset && (pItem->fbModifier & VOID_MODIFIER) == 0) {
            if ((pItem->uchMinorClass == CLASS_DEPTMODDISC) ||
                (pItem->uchMinorClass == CLASS_PLUMODDISC) ||
                (pItem->uchMinorClass == CLASS_SETMODDISC) ||
                (pItem->uchMinorClass == CLASS_OEPMODDISC) ||
                (pItem->uchPM) ||   /* saratoga */
                (pItem->uchPPICode))
			{
                return (FALSE); /* not void consolidation display */
            } else {
                return (TRUE);      /* void consolidation display */
            }
        }
    }
    return (FALSE); /* not void consolidation display */
}
//**SR 143 cwunn Adds MldAtFor(), MldPrechkAtFor(), MldDriveAtFor(), MldSetAtFor()**

/*
*===========================================================================
** Format  : USHORT  MldAtFor(UCHAR  *pszWork, ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*            
** Return  : USHORT usSetLine     -number of line(s) to be set
*            
** Synopsis: This function sets the mnemonics of adjective, noun,  
*            print modifier, conidiment, and product amount. 
*===========================================================================
*/
//See comments for MldItem about US Customs SCER
USHORT  MldAtFor(TCHAR  *pszWork, ITEMSALES  *pItem,
                        USHORT usPrice, LONG *plAttrib, USHORT usDispType, USHORT usType)
{
    if (usDispType == MLD_PRECHECK_SYSTEM || usDispType == MLD_DRIVE_THROUGH_1SCROLL) {
        return (MldPrechkAtFor(pszWork, pItem, usPrice, plAttrib, usType));
    } else {
        return (MldDriveAtFor(pszWork, pItem, plAttrib));
    }
}


/*
*===========================================================================
** Format  : USHORT  MldItems(UCHAR  *pszWork, ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*            
** Return  : USHORT usSetLine     -number of line(s) to be set
*            
** Synopsis: This function sets the mnemonics of adjective, noun,  
*            print modifier, conidiment, and product amount. 
*===========================================================================
*/
static USHORT  MldPrechkAtFor(TCHAR  *pszWork, ITEMSALES  *pItem,
                                                USHORT usPrice, LONG *plAttrib,
												USHORT usDispType)//US Customs
{
    TCHAR  aszItem[PRT_MAXITEM + MLD_PRECOLUMN1] = {0};
    TCHAR  aszAmt[MLD_AMOUNT_LEN + 1 + 1] = {0};
    DCURRENCY   lProduct = 0;
    LONG   lQTY, lAttrib;
    TCHAR  *pszStart = aszItem;
    TCHAR  *pszEnd;
    USHORT  usAmtLen;
    USHORT  usMnemLen;
    USHORT  usSetLine = 0, usSetQty = 0, i;
    SHORT   sReturn;
    size_t  len;

    /* display price of DEPT/PLU or not */
    usAmtLen = (usPrice ? MLD_ITEMAMT_LEN : 0);

	lQTY = DetermineQuantity (pItem);

    /* set over 1000 qty case after item consolidation case */
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        usSetQty = 1;
    } else if ((lQTY > 999L) || (lQTY < -99L)) {
		TCHAR  szQty100Work[16] = {0};

		//======Begin Quantity Fix, JHHJ 5-24-04======//
		tcharnset(pszWork, _T(' '), MLD_PRECOLUMN1);/* Set buffer to all spaces*/
		RflSPrintf(szQty100Work, TCHARSIZEOF(szQty100Work), aszMldQty2, lQTY); 
		/* set the formatted string to the buffer to the correct format with the quantity*/																								
		len = _tcslen(szQty100Work);
		/*get the length of the actual buffer, and takes out the null character*/
		_tcsncpy(pszWork, &szQty100Work[0], len);
		/*copy the correct quantity to the buffer WITHOUT the null character, which was causing the string
		to be terminated too early*/
        pszWork += MLD_PRECOLUMN1 + 1;
        usSetLine++;
        usSetQty = 1;
    }

    /* -- set item mnenemonics to buffer -- */
    MldSetAtFor(aszItem, pItem, &lProduct, usAmtLen, (plAttrib+usSetLine), usDispType);//US Customs added usDispType

#if 0
	// Fixed function MldSetAtFor () to calculate price correctly
	// with adjustment for void consolidation being turned on.
    /* void consolidation print */
    if (MldChkVoidConsolidation(pItem)) {   // removed with #if 0
		TCHAR  auchDummy[NUM_PLU_LEN] = {0};

        /* recalculate sales price from sCouponQTY */
        lProduct = pItem->lUnitPrice;
        for (i = pItem->uchChildNo; i < STD_MAX_COND_NUM; i++) {
            if (_tcsncmp(pItem->Condiment[i].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0) {
            /* if (pItem->Condiment[i].usPLUNo) { */
               lProduct += pItem->Condiment[i].lUnitPrice;
            }
        }
        /* --- calcurate product * qty --- */
        RflMul32by32(&huWork, (LONG)pItem->sCouponQTY, lProduct);
        lProduct =  *((LONG *)(&huWork));
    }
#endif

    for (;;) {
        // -- get 1 line -- CWUNN ADDED +3 in US Customs SCER to push characters to 29th column
        sReturn = MldGet1Line(pszStart, &pszEnd, (USHORT)(MLD_PRECOLUMN1 - usAmtLen - MLD_QTY_LEN + 3));

        tcharnset(pszWork, _T(' '), MLD_PRECOLUMN1); //Change to tcharnset for 2.0 wide conversion JHHJ//

        /* set quantity (except scale sales) */
        if (!usSetLine) {   /* first line */
            if (!usSetQty){
				TCHAR  szQtyWork[16] = {0};

				//display quantity if no #/Type entry, OR if 'disp all tran' OR 'disp all tran by reverse'
				// OR item was discounted
					RflSPrintf(szQtyWork, TCHARSIZEOF(szQtyWork), aszMldQty1, lQTY);
					len = _tcslen(szQtyWork);
					switch(len) {
					case 1:     /* 1 digit */
						_tcsncpy(pszWork+1, szQtyWork, 1);
						break;

					default:    /* over 2 digits */
						_tcsncpy(pszWork, szQtyWork, len);
						break;
					}
                }
            }

        /* keep qty area (3 char) */
        pszWork += MLD_QTY_LEN;

        /* set adjective, noun mnemo, print modifier, condiment */
        if (sReturn == PRT_LAST) {
            break;
        } else {
            _tcsncpy(pszWork, pszStart, pszEnd - pszStart);

            /* -- "1" for space -- */
            pszStart = pszEnd + 1;
            pszWork += (MLD_PRECOLUMN1 - MLD_QTY_LEN) + 1;
            usSetLine++;
        }        
    }

    /* get the length of mnemonics */
    usMnemLen = tcharlen(pszStart);

    /* display price of DEPT/PLU or not */
    if (usPrice){
			memset(aszAmt, 0, sizeof(aszAmt));
			RflSPrintf(aszAmt, TCHARSIZEOF(aszAmt), aszItemAmt, lProduct);
			usAmtLen = tcharlen(aszAmt);
    } else {
        usAmtLen = 0;
    }

    /* if data length is over 1 line length, separate into 2 lines */
    if ((pItem->uchMinorClass == CLASS_DEPTMODDISC) 
        || (pItem->uchMinorClass == CLASS_PLUMODDISC) 
        || (pItem->uchMinorClass == CLASS_SETMODDISC)
        || (pItem->uchMinorClass == CLASS_OEPMODDISC)) {

        if ((!usMnemLen) && (!usAmtLen)) {
            ;   /* not feed 1 line */
        } else if ((usMnemLen + usAmtLen +1) > (MLD_PRECOLUMN1 - MLD_QTY_LEN)) {
			_tcsncpy(pszWork, pszStart, usMnemLen);
            pszWork += MLD_PRECOLUMN1 - MLD_QTY_LEN + 1;
            usSetLine ++;
            if (usPrice) {
                MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldModItemAmt, aszAmt);
                usSetLine ++;
            }
        } else {
			_tcsncpy(pszWork, pszStart, usMnemLen);
            if (usPrice) {
                pszWork += (MLD_PRECOLUMN1 - MLD_QTY_LEN) - usAmtLen - 1;
				_tcsncpy(pszWork, aszAmt, usAmtLen);

                /* ---- reset reverse video flag at amount display ---- */
                if (!(pItem->fbModifier & VOID_MODIFIER)) { /* not reset at void */
                    for (i = 0, lAttrib = MLD_PRECHK_AMT_REV2; i < usAmtLen; i++) {
                        lAttrib |= (lAttrib >> 1);
                    }
                    *(plAttrib+usSetLine) &= ~lAttrib;
                }

            }
            usSetLine ++;
        }
    } else {

        if ((!usMnemLen) && (!usAmtLen)) {
            ;   /* not feed 1 line */				
        } else	//Mnemonic and amount lengths are longer than available space on row, seperate into two rows
			if ((usMnemLen + usAmtLen) > (MLD_PRECOLUMN1 - MLD_QTY_LEN)) {
			_tcsncpy(pszWork, pszStart, usMnemLen);
            pszWork += MLD_PRECOLUMN1 - MLD_QTY_LEN; // advance pointer to beginning of next line
            usSetLine ++;							//increase line pointer
            if (usPrice) {							
                MldSPrintf(pszWork, MLD_PRECOLUMN1, aszMldItemAmt, aszAmt);//write amount to new line
				*(plAttrib + usSetLine) = 0x00;//clears reverse video bits in plAttrib for new line US Customs by cwunn 4/8/03  
                usSetLine ++;
            }
        } else {//Menmonic and Amount will fit on one line
			_tcsncpy(pszWork, pszStart, usMnemLen);
            if (usPrice && !pItem->usFor) { //&& !pItem->usFor added SR 143 @/For key by cwunn to not show price on "## @ ## For ##" line
                pszWork += (MLD_PRECOLUMN1 - MLD_QTY_LEN) - usAmtLen - 1; //-1 places amount correctly on 29 digit wide screen CWUNN US Customs SCER
				_tcsncpy(pszWork, aszAmt, usAmtLen);

                /* ---- reset reverse video flag at amount display ---- */
                if (!(pItem->fbModifier & VOID_MODIFIER)) { /* not reset at void */
                    for (i = 0, lAttrib = MLD_PRECHK_AMT_REV; i < usAmtLen; i++) {
                        lAttrib |= (lAttrib >> 1);
                    }
                    *(plAttrib+usSetLine) &= ~lAttrib;
                }
            }
            usSetLine ++;
        }
    }

    return(usSetLine);
}

/*
*===========================================================================
** Format  : USHORT  MldItems(UCHAR  *pszWork, ITEMSALES  *pItem);
*
*   Input  : ITEMSALES  *pItem    -structure "itemsales" pointer
*
*   Output : UCHAR  *pszWork      -destination buffer address
*   InOut  : none
*            
** Return  : USHORT usSetLine     -number of line(s) to be set
*            
** Synopsis: This function sets the mnemonics of adjective, noun,  
*            print modifier, conidiment, and product amount. 
*===========================================================================
*/
static USHORT  MldDriveAtFor(TCHAR  *pszWork, ITEMSALES  *pItem, LONG *plAttrib)
{
    TCHAR  szColWork[MLD_DR3COLUMN + 1] = {0};
    TCHAR  szModCond[MLD_DR3COLUMN + 1] = {0};
    TCHAR   *pWork;
    USHORT  usSetLine = 0, usSetQty = 0;
    LONG    lQTY, lAttrib, lDefAttrib;
    BOOL        fIsDeptItem;
    USHORT      usMaxChild;
    USHORT      usI, i;
    size_t      len, len2, lenadj;
    USHORT      usPriority = 0, usPrintMod;
    TCHAR       aszSeatNo[] = _T("s1 \0");  // digit in "s1" will be set to actual seat number

    /* ---- check mdc of print modifier & condiment plu priority --- */
    if ( CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT0 ) &&
         CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT2 )) {

         usPriority = 1;
    }

    /* ---- check mdc of reverse video of condiment & print modifier --- */
    if ((CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT2)) ) {
        lDefAttrib = MLD_DEFAULT_REVERSE;
    } else {
        lDefAttrib = 0L;
    }
    lAttrib = lDefAttrib;

    fIsDeptItem = (( pItem->uchMinorClass == CLASS_DEPT ) ||
                   ( pItem->uchMinorClass == CLASS_DEPTITEMDISC ) ||
                   ( pItem->uchMinorClass == CLASS_DEPTMODDISC ));

	lQTY = DetermineQuantity (pItem);

    /* set over 1000 qty case after item consolidation case */
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {
        usSetQty = 1;
    } else if ((lQTY > 999L) || (lQTY < -99L)) {
		TCHAR  szQty100Work[16] = {0};

		//======Begin Quantity Fix, JHHJ 5-24-04======//
		tcharnset(pszWork, _T(' '), MLD_PRECOLUMN1);/* Set buffer to all spaces*/
		RflSPrintf(szQty100Work, TCHARSIZEOF(szQty100Work), aszMldQty2, lQTY); 
		/* set the formatted string to the buffer to the correct format with the quantity*/																								
		len = _tcslen(szQty100Work);
		/*get the length of the actual buffer, and takes out the null character*/
		_tcsncpy(pszWork, &szQty100Work[0], len);
		/*copy the correct quantity to the buffer WITHOUT the null character, which was causing the string
		to be terminated too early*/
        pszWork += MLD_DR3COLUMN + 1;
        usSetLine++;
        plAttrib++;             /* feed reverse video control flag */
        lAttrib = lDefAttrib;   /* set default reverse video position */
        usSetQty = 1;
    }

    /* --- set qty, adjective and noun plu name --- */
    tcharnset(szColWork, _T(' '), MLD_DR3COLUMN);
    pWork = &szColWork[3];
    len2 = 0;

    /*** set qty ***/
    if (!usSetQty) {
		TCHAR  szQtyWork[16] = {0};

        RflSPrintf(szQtyWork, TCHARSIZEOF(szQtyWork), aszMldQty1, lQTY);
        len = _tcslen(szQtyWork);
        switch(len) {
        case 1:     /* 1 digit */
			_tcsncpy(&(szColWork[1]), &szQtyWork[0], 1);
            break;

        case 2:     /* 2 digits or minus 1 digit */
			_tcsncpy(szColWork, &szQtyWork[0], 2);
            break;

        case 3:     /* 3 digits or minus 2 digits */
			_tcsncpy(szColWork, &szQtyWork[0], len);
            break;

        default:    /* invalid qty */
            break;
        }
    } else {
        len = MLD_QTY_LEN;
    }

    /* --- set seat no if entred R3.1 --- */

    if ((pItem->uchSeatNo) &&
		(pItem->uchMinorClass != CLASS_ITEMORDERDEC)) {
        if (usSetQty) {     /* scalable/4digit qty */
            aszSeatNo[1] = (pItem->uchSeatNo | _T('0'));
			_tcsncpy( &szColWork[0], aszSeatNo, 2);   /* set seat# on qty area */
			//_itow(pItem->uchSeatNo,&aszSeatNo[1],10); //SR206
			//_tcsncpy( &szColWork[0], aszSeatNo, 3);   /* set seat# on qty area */
        } else {
            aszSeatNo[1] = (pItem->uchSeatNo | _T('0'));
			_tcsncpy( &szColWork[4], aszSeatNo, 2);
            //_itow(pItem->uchSeatNo,&aszSeatNo[1],10); //SR206
			//_tcsncpy( &szColWork[4], aszSeatNo, 3);
			_tcsncpy( pszWork, &szColWork[0], MLD_DR3COLUMN);
            pszWork += MLD_DR3COLUMN + 1;
            plAttrib++;
            usSetLine++;
            tcharnset(szColWork, _T(' '), MLD_DR3COLUMN);
        }
    }

    if ((_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) < 0) ||
        (_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) > 0)) {

        if (( pItem->uchAdjective ) && ( ! fIsDeptItem ) && pItem->uchMinorClass != CLASS_ITEMORDERDEC) {
			TCHAR   szAdjWork[PARA_ADJMNEMO_LEN + 1] = {0};

            /* --- prepare adjective mnemonic, if main item is adjective --- */

            RflGetAdj ( szAdjWork, pItem->uchAdjective );
            lenadj = _tcslen(szAdjWork);

			_tcsncpy( pWork, &szAdjWork[0], lenadj);

            /* increment reverse video control flag, for not reverse video */
            for(i = 0; i < lenadj + 1; i++) {
                lAttrib = (lAttrib << 1);
            }

            if ((len = _tcslen(pItem->aszMnemonic)) < (size_t)(MLD_DRIVE_DEPT_PLU_LEN-lenadj-1)) {

                pWork += lenadj;
                pWork++;
				_tcsncpy( pWork, pItem->aszMnemonic, len);

				_tcsncpy( pszWork, &szColWork[0], MLD_DR3COLUMN);
                pszWork += MLD_DR3COLUMN + 1;
                usSetLine ++;
                plAttrib++;             /* feed reverse video control flag */
                lAttrib = lDefAttrib;   /* set default reverse video position */

            } else {
				_tcsncpy( pszWork, &szColWork[0], MLD_DR3COLUMN);
                pszWork += MLD_DR3COLUMN + 1;
                usSetLine ++;
                plAttrib++;             /* feed reverse video control flag */
                lAttrib = lDefAttrib;   /* set default reverse video position */

                if ((len = _tcslen(pItem->aszMnemonic)) >= MLD_DRIVE_DEPT_PLU_LEN) {
                    tcharnset(pszWork, _T(' '), MLD_DR3COLUMN);
					_tcsncpy( &(pszWork[3]), pItem->aszMnemonic, MLD_DRIVE_DEPT_PLU_LEN);
                    pszWork += MLD_DR3COLUMN + 1;
                    usSetLine ++;
                    plAttrib++;             /* feed reverse video control flag */
                    lAttrib = lDefAttrib;   /* set default reverse video position */
                } else {
                    tcharnset(szColWork, _T(' '), MLD_DR3COLUMN);
					_tcsncpy( pWork, pItem->aszMnemonic, len);
                    pWork += len+1;
                    len2 = len;
                    for(i = 0; i < len + 1; i++) {
                        lAttrib = (lAttrib << 1);
                    }
                }
            }
        } else {
            if ((len = _tcslen(pItem->aszMnemonic)) >= MLD_DRIVE_DEPT_PLU_LEN) {
                _tcsncpy( pWork, pItem->aszMnemonic, MLD_DRIVE_DEPT_PLU_LEN);
				_tcsncpy( pszWork, &szColWork[0], MLD_DR3COLUMN);
                pszWork += MLD_DR3COLUMN + 1;
                usSetLine ++;
                plAttrib++;             /* feed reverse video control flag */
                lAttrib = lDefAttrib;   /* set default reverse video position */
            } else {
				 _tcsncpy( pWork, pItem->aszMnemonic, len);
                pWork += len+1;
                len2 = len;

                /* increment reverse video control flag, for not reverse video */
                for(i = 0; i < len + 1; i++) {
                    lAttrib = (lAttrib << 1);
                }
            }
        }
    } else {
        len = 0;
        len2=0;
    }

    if ((!len2) && (!len)) {     /* for plu mnemonics is null case */
        len2++;
        lAttrib = (lAttrib << 1);   /* reverse video */
    }

    if (!len2) {
        tcharnset(szColWork, _T(' '), MLD_DR3COLUMN);
        pWork = &szColWork[3];
    }

	/* --- prepare print modifiers and condiment items --- */
    usMaxChild = ( pItem->uchChildNo + pItem->uchCondNo + pItem->uchPrintModNo );

    for (usPrintMod=0; usPrintMod<(USHORT)(usPriority?2:1); usPrintMod++) {
    for ( usI = (USHORT)((usPriority&&!usPrintMod)?0:pItem->uchChildNo); usI < usMaxChild; usI++ ) {

        /* --- display print modifier before condiment at priority option ---- */
        if ( (!usPrintMod) && (pItem->auchPrintModifier[usI]) ) {
			RflGetMnemonicByClass(CLASS_PARAPRTMODI, szModCond, pItem->auchPrintModifier[ usI ]);
		} else {
            if ((usPriority) && (!usPrintMod)) continue;

            /* --- prepare condiment item's mnemonic --- */
            if ( ! fIsDeptItem ) {
                if (RflIsSpecialPlu (pItem->Condiment[usI].auchPLUNo, MLD_NO_DISP_PLU_DUMMY) != 0) {
					_tcsncpy(szModCond, pItem->Condiment[usI].aszMnemonic, MLD_DRIVE_DEPT_PLU_LEN);
                } else {
                    continue;
                }
            } else {
                continue;
            }
        }
        len = _tcslen(szModCond);
        if (!len) continue;

        if (len >= ((size_t)MLD_DRIVE_DEPT_PLU_LEN-len2)) {
            if (len2) {     /* if priveous data is not restored */
				_tcsncpy( pszWork, szColWork, MLD_DR3COLUMN);
                pszWork += MLD_DR3COLUMN + 1;
                usSetLine ++;
                plAttrib++;             /* feed reverse video control flag */
                lAttrib = lDefAttrib;   /* set default reverse video position */
                tcharnset(szColWork, _T(' '), MLD_DR3COLUMN);
                pWork = &szColWork[3];
                len2=0;
                if (len >= MLD_DRIVE_DEPT_PLU_LEN) {
                    tcharnset(pszWork, _T(' '), MLD_DR3COLUMN);
					_tcsncpy( &(pszWork[3]), szModCond, MLD_DRIVE_DEPT_PLU_LEN);
                    pszWork += MLD_DR3COLUMN + 1;
                    usSetLine ++;
                    /* set reverse video control flag */
                    for(i=0; i<MLD_DRIVE_DEPT_PLU_LEN; i++) {
/***                    for(i=0; i<len; i++) { ***/
                        *plAttrib |= lAttrib;
                        lAttrib = (lAttrib << 1);
                    }
                    plAttrib++;             /* feed reverse video control flag */
                    lAttrib = lDefAttrib;   /* set default reverse video position */
                    continue;
                } else {
                    _tcsncpy( pWork, szModCond, len);
                    pWork += len+1;
                    if (len2) len2++;
                    len2 += len;
                    /* set reverse video control flag */
                    for(i=0; i<MLD_DRIVE_DEPT_PLU_LEN; i++) {
/***                    for(i=0; i<len; i++) { ***/
                        *plAttrib |= lAttrib;
                        lAttrib = (lAttrib << 1);
                    }
/***                    if (len2) lAttrib = (lAttrib << 1); ***/
                    continue;
                }
            } else {
                tcharnset(pszWork, _T(' '), MLD_DR3COLUMN);
				_tcsncpy( &(pszWork[3]), szModCond, MLD_DRIVE_DEPT_PLU_LEN);
                pszWork += MLD_DR3COLUMN + 1;
                usSetLine ++;
                /* set reverse video control flag */
                for(i=0; i<MLD_DRIVE_DEPT_PLU_LEN; i++) {
/***                for(i=0; i<len; i++) { ***/
                    *plAttrib |= lAttrib;
                    lAttrib = (lAttrib << 1);
                }
                plAttrib++;             /* feed reverse video control flag */
                lAttrib = lDefAttrib;   /* set default reverse video position */
                tcharnset(szColWork, _T(' '), MLD_DR3COLUMN);
                pWork = &szColWork[3];
                len2=0;
                continue;
            }
        } else {
            _tcsncpy( pWork, szModCond, len);
            pWork += len+1;
            if (len2) len2++;
            len2 += len;
            /* set reverse video control flag */
            for(i=0; i<MLD_DRIVE_DEPT_PLU_LEN; i++) {
/***            for(i=0; i<len; i++) { ***/
                *plAttrib |= lAttrib;
                lAttrib = (lAttrib << 1);
            }
/***            if (len2) lAttrib = (lAttrib << 1); ***/
            continue;
        }

    } /* end for usI */
    } /* end for usPrintMod */

    if (len2) {     /* restore condiments */
		_tcsncpy( pszWork, szColWork, MLD_DR3COLUMN);
        pszWork += MLD_DR3COLUMN + 1;
        usSetLine ++;
    }

    return(usSetLine);
}


/*
*===========================================================================
** Format   : VOID PrtSetItem( UCHAR     *puchDest,
*                              ITEMSALES *pItem,
*                              LONG      *plAllPrice,
*                              SHORT     sFormatType )
*
*   Input   : ITEMSALES *pItem      - address of source sales item
*             SHORT     sType       - type of print format
*                                       PRT_ONLY_NOUN
*                                       PRT_ONLY_PMCOND
*                                       PRT_ALL_ITEM
*
*   Output  : UCHAR     *pDest      - destination buffer address
*             LONG      *plAllPrice - price of all products
*
*   InOut   : none
** Return   : none
*
** Synopsis : This function prepares a print format of a sales item with
*           adjective and mnemoic and print modifiers, condiments.
*
*   WARNING:  condiment prices for condiments that are PPI PLUs are updated by using
*             function ItemSalesCalcCondimentPPIReplace() so the condiment prices
*			  in ITEMSALES pItem are updated if there are PPI PLUs specified in the
*			  item data.
*===========================================================================
*/
VOID MldSetAtFor( TCHAR *puchDest, ITEMSALES *pItem, DCURRENCY *plAllPrice,
								USHORT usAmtLen, LONG *plAttrib, USHORT usDispType)
{
    BOOL        fIsDeptItem;
    TCHAR       *puchWrtPtr;
    USHORT      usWrtLen, usSaveLen;
    USHORT      usMaxChild;
    USHORT      usPriority = 0;
    LONG        lAttrib, lDefAttrib;
	LONG        lQTY;

	lQTY = DetermineQuantity (pItem);

    /* ---- check mdc of print modifier & condiment plu priority --- */
    if ( CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT0 ) &&
         CliParaMDCCheck( MDC_PRINTPRI_ADR, EVEN_MDC_BIT2 ))
	{
         usPriority = 1;
    }

    /* initialize print modifier/condiment reverse video control flag */
    usSaveLen = MLD_QTY_LEN;
    if ((CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT2)) ) {
        lDefAttrib = MLD_DEFAULT_REVERSE;
    } else {
        lDefAttrib = 0L;
    }
    lAttrib = lDefAttrib;

    puchWrtPtr = puchDest;

#if 1
	*plAllPrice = DeterminePrice (pItem, lQTY);
	*plAllPrice += ItemSalesCalcCondimentPPIReplaceQty (pItem, lQTY);
#else
//    *plAllPrice = pItem->lUnitPrice * lQTY;
	*plAllPrice = DeterminePrice (pItem, lQTY);
#endif

    /* --- set seat no if entred R3.1 --- */
    if ((pItem->uchSeatNo) && (pItem->uchMinorClass != CLASS_ITEMORDERDEC)) {
		TCHAR       aszSeatNo[] = _T("s1 \0");

		aszSeatNo[1] = (UCHAR)(pItem->uchSeatNo | 0x30);
        _tcsncpy( puchWrtPtr, aszSeatNo, 3 );
        puchWrtPtr += 3;
        if (lAttrib) lAttrib = 0x00000040L;
        usSaveLen += 3;//usSaveLen += 4;
    }

    /* --- determine source item is department item or not --- */

    fIsDeptItem = (( pItem->uchMinorClass == CLASS_DEPT ) ||
                   ( pItem->uchMinorClass == CLASS_DEPTITEMDISC ) ||
                   ( pItem->uchMinorClass == CLASS_DEPTMODDISC ));

	    /* --- prepare main sales item's mnemonic (and adjective) --- */
	
		//if PLUNo is in the reserved range, perform adjective mnemonics (comment by cwunn, could be incorrect)
		if ((memcmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_LOW, (NUM_PLU_LEN * sizeof(TCHAR))) < 0) ||
			(memcmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_HIGH, (NUM_PLU_LEN * sizeof(TCHAR))) > 0))
		{
			TCHAR       aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };
			PARAMDC		ParaMDC = { 0 };
			TCHAR		ucForQty[16] = { 0 }, ucFor[16] = { 0 }, ucForTemp[16] = { 0 };

			if (( pItem->uchAdjective ) && ( ! fIsDeptItem ) && pItem->uchMinorClass != CLASS_ITEMORDERDEC) {

				/* --- prepare adjective mnemonic, if main item is adjective --- */
				RflGetAdj ( puchWrtPtr, pItem->uchAdjective );

				/* --- make a space between adjective mnemonic
						and main sales item's mnemonic --- */
				usWrtLen = tcharlen( puchWrtPtr );
				*( puchWrtPtr + usWrtLen ) = _T(' ');

				puchWrtPtr += ( usWrtLen + 1 );

				/* set reverse video control flag */
				if (usSaveLen + usWrtLen + 1 + usAmtLen > MLD_PRECOLUMN1) {
					plAttrib++;
					lAttrib = lDefAttrib;
					usSaveLen = MLD_QTY_LEN;
				} else {
					for(USHORT i = 0; i < usWrtLen+1; i++) lAttrib = (lAttrib << 1);
					usSaveLen += usWrtLen+1;
				}
			}

			// set "total qty @ package qty" in mnemonic"
			usWrtLen = 0;
			_tcscat( puchWrtPtr, _T("@ "));
			usWrtLen += 2;

			_itot((int)pItem->usForQty, ucForQty, 10);
			_tcscat( puchWrtPtr, ucForQty);
			usWrtLen += tcharlen(ucForQty);
			*( puchWrtPtr + usWrtLen++ ) = _T(' '); /* --- add a space between package qty and trn mnem--- */

			/* --- get transaction mnemonics --- */
			RflGetTranMnem(aszTranMnem, TRN_FOR_ADR);
			usWrtLen += tcharlen(aszTranMnem);
			_tcsncat( puchWrtPtr, aszTranMnem, usWrtLen );

			*( puchWrtPtr + usWrtLen++ ) = _T(' '); /* --- add a space between package qty and trn mnem--- */

			//set package price after retrieving decimal settings
			ParaMDC.uchMajorClass = CLASS_PARAMDC;
			ParaMDC.usAddress = MDC_SYSTEM3_ADR;
			ParaMDCRead(&ParaMDC);

			_itot((int)pItem->usFor, ucForTemp, 10);
			if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x00){
				//2 decimal system
				_stprintf(ucFor, _T("%d.%.2d"), pItem->usFor/100, pItem->usFor%100);
			}
			if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x01){
				//3 decimal system
				_stprintf(ucFor, _T("%d.%.3d"), pItem->usFor/1000, pItem->usFor%1000);
			}
			if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x02){
				//no decimal system 1
				_stprintf(ucFor, _T("%d"), pItem->usFor);
			}
			if((ParaMDC.uchMDCData & MDC_ODD_MASK) == 0x03){
				//no decimal system 1
				_stprintf(ucFor, _T("%d"), pItem->usFor);
			}
			_tcscat( puchWrtPtr, ucFor);
			usWrtLen += tcharlen(ucFor);

			/* --- make a space between main sales item's mnemonic and
					   print modifier's mnemonic --- */
			*( puchWrtPtr + usWrtLen ) = _T(' ');
        
			puchWrtPtr += ( usWrtLen + 1 );

			/* set reverse video control flag */
			if (usSaveLen + usWrtLen + 1 + usAmtLen > MLD_PRECOLUMN1) {
				plAttrib++;
				lAttrib = lDefAttrib;
				usSaveLen = MLD_QTY_LEN;
			} else {
				for(USHORT i = 0; i < usWrtLen + 1; i++) lAttrib = (lAttrib<<1);
				usSaveLen += usWrtLen+1;
			}
		}

    /* --- prepare print modifiers and condiment items --- */
    usMaxChild = ( pItem->uchChildNo + pItem->uchCondNo  + pItem->uchPrintModNo );

    for (USHORT usPrintMod = 0; usPrintMod < (USHORT)(usPriority ? 2 : 1); usPrintMod++) {
    for (USHORT usI = 0; usI < usMaxChild; usI++ ) {

        if ((usPriority) && (usPrintMod)) {
            ;   /* if print modifier is already displayed */
        } else if ( pItem->auchPrintModifier[ usI ] ) {
			PARAPRTMODI PrtMod = {0};

            /* --- prepare print modifier mnemonic --- */
			PrtMod.uchMajorClass = CLASS_PARAPRTMODI;
            PrtMod.uchAddress = pItem->auchPrintModifier[ usI ];
            CliParaRead( &PrtMod );

            usWrtLen = tcharlen( PrtMod.aszMnemonics );
            if (!usWrtLen) continue;
            _tcsncpy( puchWrtPtr, PrtMod.aszMnemonics, usWrtLen );

            /* --- make a space between mnemonic --- */
            *( puchWrtPtr + usWrtLen ) = 0x20;
            puchWrtPtr += ( usWrtLen + 1 );

            /* set reverse video control flag */
            for (USHORT i = 0,usWrtLen2 = 0; i < usWrtLen; i++) {
                usWrtLen2++;
                if ((PrtMod.aszMnemonics[i] != 0x20)&&(i<(usWrtLen-1))) {
                    continue;   /* check space in the mnemonic */
                }
                if ((usSaveLen+usWrtLen2-(i<(usWrtLen-1)?1:0)+usAmtLen) > MLD_PRECOLUMN1) {
                    plAttrib++;
                    lAttrib = lDefAttrib;
                    usSaveLen = MLD_QTY_LEN;
                }
                /**** reverse video between noun plu and amount ****/
                for (USHORT j = 0; j < (USHORT)(MLD_PRECOLUMN1 - (usSaveLen+usAmtLen)); j++) {
/***                for(j=0; j<usWrtLen2; j++) { ***/
                    *plAttrib |= lAttrib;   /* set reverse video flag */
                    lAttrib = (lAttrib << 1);
                }
                lAttrib = 0L;   /* to avoid duplicate reverse set in 1 line */
                usSaveLen += usWrtLen2;
                usWrtLen2 = 0;
            }
            usSaveLen++;    /* set space between mnemonic, not reverse video */
/***            lAttrib = (lAttrib<<1); ***/
        }

        if ((usPriority) && (!usPrintMod)) {
            ;   /* if print modifier is not displayed */
        } else if ( RflIsSpecialPlu (pItem->Condiment[ usI ].auchPLUNo, MLD_NO_DISP_PLU_DUMMY) != 0 ) {
	            /* --- prepare condiment item's mnemonic --- */
				if ((pItem->uchChildNo <= usI ) && ( ! fIsDeptItem )) {

					usWrtLen = tcharlen( pItem->Condiment[ usI ].aszMnemonic );
					if (!usWrtLen) continue;
					_tcsncpy( puchWrtPtr, pItem->Condiment[ usI ].aszMnemonic, usWrtLen );

					/* --- make a space between mnemonic --- */

					usWrtLen = tcharlen( puchWrtPtr );
					*( puchWrtPtr + usWrtLen ) = _T(' ');

					puchWrtPtr += ( usWrtLen + 1 );

					/* set reverse video control flag */
					for (USHORT i = 0, usWrtLen2=0; i < usWrtLen; i++) {
						usWrtLen2++;
						if ((pItem->Condiment[ usI ].aszMnemonic[i] != _T(' '))
										&&(i<(usWrtLen-1))) {
							continue;   /* check space in the mnemonic */
						}
						if ((usSaveLen+usWrtLen2-(i<(usWrtLen-1)?1:0)+usAmtLen) > MLD_PRECOLUMN1) {
							plAttrib++;
							lAttrib = lDefAttrib;
							usSaveLen = MLD_QTY_LEN;
						}
						/**** reverse video between noun plu and amount ****/
						//Add +3 to shade all 29 columns appropriately US Customs SCER cwunn 4/8/03 
						for (USHORT j = 0; j < (USHORT)(MLD_PRECOLUMN1 - (usSaveLen + usAmtLen) + 3); j++) {
	/***                    for(j=0; j<usWrtLen2; j++) { ***/
							*plAttrib |= lAttrib;   /* set reverse video flag */
							lAttrib = (lAttrib << 1);
						}
						lAttrib = 0L;   /* to avoid duplicate reverse set in 1 line */
						usSaveLen += usWrtLen2;
						usWrtLen2 = 0;
					}
					usSaveLen++;    /* set space between mnemonic, not reverse video */
	/***                lAttrib = (lAttrib<<1); ***/

					/* --- get all product price --- */

// full price including condiments already calculated in ItemSalesCalcCondimentPPIReplace() above.
//					*plAllPrice += (lQTY * pItem->Condiment[usI].lUnitPrice);

				} /* end if */
        } /* end if else */
    } /* end for usI */
    } /* end for usPrintMod */

    /* --- delete last space chara, and make null terminator --- */
    *(puchWrtPtr--) = _T('\0');
}

#if defined(POSSIBLE_DEAD_CODE)
// following code for what looks to be gift card display functionality does not
// seem to be actually used anywhere. I am marking this as possible dead code
// in order to help with eliminating this source if it is indeed dead code and
// not used and not really usable.
//
// As a side note, the gift card code was written by a student, Steve, who was not
// the sharpest knife in the drawer nor the brightest bulb in the chandelier. That
// was some time around 2005 when Dan Parliman was still with NCR.
//
//      Richard Chambers, Oct-17-2017  GenPOS Rel 2.2.2

/*
*===========================================================================
** Format  : USHORT  MldGiftBalance(UCHAR *pszWork, UCHAR fbMod,
*                           USHORT usDate UCHAR *aszApproval );
*
*    Input : UCHAR fbMod           -offline modifier
*            USHORT usDate         -expiration date
*            UCHAR  *aszApproval   -approval code
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function prints prints EPT offline symbol
*            and expiration date.							   SSTOLTZ
*===========================================================================
*/
USHORT  MldGiftBalance(ITEMTENDER *pItem, USHORT usType,USHORT usScroll)
{
//Still having problems, do a search for regtemp file,  find out how to display on one side of screen or format it to make it on right colomn
	//needs lots of work.  amount is not working.
	static  const TCHAR FARCONST    fmt010ld[] = _T("%l$");
	TCHAR  aszSpaces[] = _T("                             ");

    TCHAR   aszScrollBuff[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1] = {0}; /* print data save area */
    LONG    alAttrib[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1] = {0};
    LONG    lFS = 0L;
    USHORT  usScrollLine = 1;            /* number of lines to be printed */
    USHORT  i;
	TCHAR  *pWritePointer;
	int    iMaskedChars = 0;  //number of characters to mask
	int    iShownChars = 0;   //number of characerst MDC indicates should not
	SHORT  numSpaces;
	TCHAR	auchNumber[NUM_NUMBER] = {0};
	TCHAR   amount[MLD_PRECOLUMN1 + 1] = {0};
    TCHAR   auchTmpBuff[MLD_PRECOLUMN1 + 1] = {0};

//   	TCHAR RefNum[MLD_PRECOLUMN1 + 1];

	memset(&amount, ' ', sizeof(amount));
	
//clear text
	ItemCommonMldInit();

	//Gets Description label
	RflSPrintf(amount, sizeof(amount)/sizeof(amount[0]) - 1, fmt010ld, pItem->lTenderAmount);
	RflGetLead (auchTmpBuff, LDT_GIFTCARD_BALANCE);

	pWritePointer = auchTmpBuff;
	pWritePointer += _tcslen(auchTmpBuff);
	numSpaces = (MLD_PRECOLUMN1) - (_tcslen(auchTmpBuff) + _tcslen(amount));
	
	//tcharnset(pWritePointer, _T(" "), numSpaces);
	_tcsncpy(pWritePointer, aszSpaces,numSpaces);
	//_tcsncat(auchTmpBuff," ",numSpaces);
	pWritePointer += numSpaces;
	_tcsncpy(pWritePointer, amount, _tcslen(amount));

    /* -- set response message text, Saratoga -- */
//    usScrollLine += MldCPRspMsgText(aszScrollBuff[usScrollLine],
//                auchTmpBuff, MLD_PRECHECK_SYSTEM);

//	_tcsncpy(aszScrollBuff[0],_T("     "),sizeof(_T("     ")));
	tcharnset(aszScrollBuff[0], _T(' '), MLD_PRECOLUMN1+1);

	/* -- set response message text, Saratoga -- */
    for (i = 0; i < NUM_CPRSPCO_DISP; i++) {
        usScrollLine += MldCPRspMsgText(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszCPMsgText[i], MLD_PRECHECK_SYSTEM);
	}

	    /* -- set cursor position & display all data in the buffer -- */
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : USHORT  MldGiftVoidIssue(UCHAR *pszWork, UCHAR fbMod,
*                           USHORT usDate UCHAR *aszApproval );
*
*    Input : UCHAR fbMod           -offline modifier
*            USHORT usDate         -expiration date
*            UCHAR  *aszApproval   -approval code
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function prints prints EPT offline symbol
*            and expiration date.							   SSTOLTZ
*===========================================================================
*/
USHORT  MldGiftIssue(ITEMTENDER *pItem, USHORT usType,USHORT usScroll)
{
	static  const TCHAR FARCONST    fmt010ld[] = _T("%l$");
	TCHAR  aszSpaces[] = _T("                             ");
    //TCHAR  aszApproval[10+1];

    TCHAR  aszScrollBuff[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1] = {0}; /* print data save area */
    LONG   alAttrib[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1] = {0};
    LONG   lFS = 0L;
	int    iMaskedChars = 0;  //number of characters to mask
	int    iShownChars = 0;   //number of characerst MDC indicates should not
	TCHAR  *pWritePointer;
    USHORT  usScrollLine = 1;            /* number of lines to be printed */
    USHORT  i;
	SHORT  numSpaces;
	TCHAR	auchNumber[PIF_LEN_TRACK2] = {0};
	TCHAR   amount[MLD_PRECOLUMN1 + 1] = {0};
	TCHAR   auchTmpBuff[MLD_PRECOLUMN1 + 1] = {0};

	memset(&amount, ' ', sizeof(amount));
	
//clear text
//	ItemCommonMldInit();

	//Gets Description label
	RflSPrintf(amount, sizeof(amount)/sizeof(amount[0]) - 1, fmt010ld, pItem->lTenderAmount);
	RflGetLead (auchTmpBuff, LDT_ISSUE);

	pWritePointer = auchTmpBuff;
	pWritePointer += _tcslen(auchTmpBuff);
	numSpaces=(MLD_PRECOLUMN1) - (_tcslen(auchTmpBuff) + _tcslen(amount));
	
	//tcharnset(pWritePointer, _T(" "), numSpaces);
	_tcsncpy(pWritePointer, aszSpaces,numSpaces);
	//_tcsncat(auchTmpBuff," ",numSpaces);
	pWritePointer += numSpaces;
	_tcsncpy(pWritePointer,amount,_tcslen(amount));

	tcharnset(aszScrollBuff[0], _T(' '), MLD_PRECOLUMN1+1);

	    /* -- set response message text, Saratoga -- */
    usScrollLine += MldCPRspMsgText(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], auchTmpBuff, MLD_PRECHECK_SYSTEM);

	    /* -- set response message text, Saratoga -- */
    for (i = 0; i < NUM_CPRSPCO_DISP; i++) {
        usScrollLine += MldCPRspMsgText(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszCPMsgText[i], MLD_PRECHECK_SYSTEM);
	}

    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}


/*
*===========================================================================
** Format  : USHORT  MldGiftVoidIssue(UCHAR *pszWork, UCHAR fbMod,
*                           USHORT usDate UCHAR *aszApproval );
*
*    Input : UCHAR fbMod           -offline modifier
*            USHORT usDate         -expiration date
*            UCHAR  *aszApproval   -approval code
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function prints prints EPT offline symbol
*            and expiration date.							   SSTOLTZ
*===========================================================================
*/
USHORT  MldGiftVoidIssue(ITEMTENDER *pItem, USHORT usType,USHORT usScroll)
{
	static  const TCHAR FARCONST    fmt010ld[] = _T("%l$");
	TCHAR  aszSpaces[] = _T("                             ");
    //TCHAR  aszApproval[10+1];

    TCHAR   aszScrollBuff[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1] = {0}; /* print data save area */
    LONG    alAttrib[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1] = {0};
    LONG    lFS = 0L;
	int     iMaskedChars = 0;  //number of characters to mask
	int     iShownChars = 0;   //number of characerst MDC indicates should not
	TCHAR   *pWritePointer;
    USHORT  usScrollLine = 1;            /* number of lines to be printed */
    USHORT  i;
	SHORT   numSpaces;
	TCHAR	auchNumber[PIF_LEN_TRACK2] = {0};
	TCHAR   amount[MLD_PRECOLUMN1 + 1] = {0};
    TCHAR   auchTmpBuff[MLD_PRECOLUMN1 + 1] = {0};

	memset(&amount, ' ', sizeof(amount));
	
//clear text
//	ItemCommonMldInit();

	//Gets Description label
	RflSPrintf(amount, sizeof(amount)/sizeof(amount[0]) - 1, fmt010ld, pItem->lTenderAmount);
	RflGetLead (auchTmpBuff, LDT_VOID_ISSUE);

	pWritePointer = auchTmpBuff;
	pWritePointer += _tcslen(auchTmpBuff);
	numSpaces=(MLD_PRECOLUMN1) - (_tcslen(auchTmpBuff)+_tcslen(amount));
	
	//tcharnset(pWritePointer, _T(" "), numSpaces);
	_tcsncpy(pWritePointer, aszSpaces,numSpaces);
	//_tcsncat(auchTmpBuff," ",numSpaces);
	pWritePointer += numSpaces;
	_tcsncpy(pWritePointer,amount,_tcslen(amount));

	tcharnset(aszScrollBuff[0], _T(' '), MLD_PRECOLUMN1+1);
	
	/* -- set response message text, Saratoga -- */
    usScrollLine += MldCPRspMsgText(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], auchTmpBuff, MLD_PRECHECK_SYSTEM);

	/* -- set response message text, Saratoga -- */
    for (i = 0; i < NUM_CPRSPCO_DISP; i++) {
        usScrollLine += MldCPRspMsgText(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszCPMsgText[i], MLD_PRECHECK_SYSTEM);
	}

    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}

/*
*===========================================================================
** Format  : USHORT  MldGiftReload(UCHAR *pszWork, UCHAR fbMod,
*                           USHORT usDate UCHAR *aszApproval );
*
*    Input : UCHAR fbMod           -offline modifier
*            USHORT usDate         -expiration date
*            UCHAR  *aszApproval   -approval code
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function prints prints EPT offline symbol
*            and expiration date.							   SSTOLTZ
*===========================================================================
*/
USHORT  MldGiftReload(ITEMTENDER *pItem, USHORT usType,USHORT usScroll)
{
	static  const TCHAR FARCONST    fmt010ld[] = _T("%l$");
	TCHAR  aszSpaces[] = _T("                             ");
//    TCHAR  aszApproval[10+1];

    TCHAR   aszScrollBuff[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1] = {0}; /* print data save area */
    LONG    alAttrib[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1] = {0};
    LONG    lFS = 0L;
	TCHAR   *pWritePointer;
	int     iMaskedChars = 0;  //number of characters to mask
	int     iShownChars = 0;   //number of characerst MDC indicates should not
    USHORT  usScrollLine = 1;            /* number of lines to be printed */
    USHORT  i;
	SHORT   numSpaces;
	TCHAR	auchNumber[PIF_LEN_TRACK2] = {0};
	TCHAR   amount[MLD_PRECOLUMN1 + 1] = {0};
    TCHAR   auchTmpBuff[MLD_PRECOLUMN1 + 1] = {0};

	memset(&amount, ' ', sizeof(amount));
	
//clear text
//	ItemCommonMldInit();

	//Gets Description label
	RflSPrintf(amount, sizeof(amount)/sizeof(amount[0]) - 1, fmt010ld, pItem->lTenderAmount);
	RflGetLead (auchTmpBuff, LDT_RELOAD);

	pWritePointer = auchTmpBuff;
	pWritePointer += _tcslen(auchTmpBuff);
	numSpaces=(MLD_PRECOLUMN1) - (_tcslen(auchTmpBuff) + _tcslen(amount));
	
	//tcharnset(pWritePointer, _T(" "), numSpaces);
	_tcsncpy(pWritePointer, aszSpaces,numSpaces);
	//_tcsncat(auchTmpBuff," ",numSpaces);
	pWritePointer += numSpaces;
	_tcsncpy(pWritePointer,amount,_tcslen(amount));

	tcharnset(aszScrollBuff[0], _T(' '), MLD_PRECOLUMN1+1);

	/* -- set response message text, Saratoga -- */
    usScrollLine += MldCPRspMsgText(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], auchTmpBuff, MLD_PRECHECK_SYSTEM);

    /* -- set response message text, Saratoga -- */
    for (i = 0; i < NUM_CPRSPCO_DISP; i++) {
        usScrollLine += MldCPRspMsgText(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszCPMsgText[i], MLD_PRECHECK_SYSTEM);
	}
    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}
/*
*===========================================================================
** Format  : USHORT  MldGiftVoidReload(UCHAR *pszWork, UCHAR fbMod,
*                           USHORT usDate UCHAR *aszApproval );
*
*    Input : UCHAR fbMod           -offline modifier
*            USHORT usDate         -expiration date
*            UCHAR  *aszApproval   -approval code
*
*   Output : UCHAR *pszWork         -destination buffer address
*    InOut : none
*
** Return  : number of line(s) to be set
*
** Synopsis: This function prints prints EPT offline symbol
*            and expiration date.							   SSTOLTZ
*===========================================================================
*/
USHORT  MldGiftVoidReload(ITEMTENDER *pItem, USHORT usType,USHORT usScroll)
{
	static  const TCHAR FARCONST    fmt010ld[] = _T("%l$");
	TCHAR  aszSpaces[] = _T("                             ");
//    TCHAR  aszApproval[10+1];

    TCHAR   aszScrollBuff[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1] = {0}; /* print data save area */
    LONG    alAttrib[NUM_CPRSPCO_DISP + 10][MLD_PRECOLUMN1 + 1] = {0};
    LONG    lFS = 0L;
	int     iMaskedChars = 0;  //number of characters to mask
	int     iShownChars = 0;   //number of characerst MDC indicates should not
	TCHAR   *pWritePointer;
	SHORT   numSpaces;
    USHORT  usScrollLine = 0;            /* number of lines to be printed */
    USHORT  i;
	TCHAR	auchNumber[PIF_LEN_TRACK2] = {0};
	TCHAR   amount[MLD_PRECOLUMN1 + 1] = {0};
    TCHAR   auchTmpBuff[MLD_PRECOLUMN1 + 1] = {0};

	memset(&amount, ' ', sizeof(amount));
	
//clear text
//	ItemCommonMldInit();

	//Gets Description label
	RflSPrintf(amount, sizeof(amount)/sizeof(amount[0]) - 1, fmt010ld, pItem->lTenderAmount);
	RflGetLead (auchTmpBuff, LDT_VOID_RELOAD);

	pWritePointer = auchTmpBuff;
	pWritePointer += _tcslen(auchTmpBuff);
	numSpaces = (MLD_PRECOLUMN1) - (_tcslen(auchTmpBuff)+_tcslen(amount));
	
	//tcharnset(pWritePointer, _T(" "), numSpaces);
	_tcsncpy(pWritePointer, aszSpaces,numSpaces);
	//_tcsncat(auchTmpBuff," ",numSpaces);
	pWritePointer += numSpaces;
	_tcsncpy(pWritePointer, amount,_tcslen(amount));

	/* -- set response message text, Saratoga -- */
    usScrollLine += MldCPRspMsgText(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], auchTmpBuff, MLD_PRECHECK_SYSTEM);

	/* -- set response message text, Saratoga -- */
    for (i = 0; i < NUM_CPRSPCO_DISP; i++) {
        usScrollLine += MldCPRspMsgText(aszScrollBuff[MLD_SETLINE_BASE(usScrollLine)], pItem->aszCPMsgText[i], MLD_PRECHECK_SYSTEM);
	}

    return (MldWriteScrollDisplay(aszScrollBuff[0], usScroll, usType, usScrollLine, MLD_PRECOLUMN1, &alAttrib[0][0]));
}
#endif

/*
*===========================================================================
** Format   : VOID MldCheckEPTAcctPrint(PARAMDC TenderMDC, UCHAR	localMDCBitA,UCHAR localMDCMask)
*
*
*
*   Input   : ITEMSALES 
*             SHORT     
*                                       
*                                      
*                                      
*
*   Output  : UCHAR     
*             LONG      
*
*   InOut   : none
** Return   : none
*
** Synopsis : Checks MDC 377 to see if the account number should be printed
*	if its set to not print for EPT or CP, then it checks the tender number to see if its set up to 
*	Perform EPT and if its set to Not Perform EPT, it prints the account number if one exists
*	Retuns a 1 to print the Account Number and a 0 to not print the Account Number
*===========================================================================
*/

SHORT MldCheckEPTAcctPrint(PARAMDC TenderMDC, UCHAR	localMDCBitA,UCHAR localMDCMask, TCHAR *pszNumber){

	UCHAR oddOrEven;

    //SR 131 cwunn 7/15/2003 Add MDC check to determine if tender number data should be printed.
	//if tender status indicates EPT
	if((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA){
		//then check if MDC indicates print acct # on EPT Tender,	
		if(!ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT0)){
			//then print the acct #
			return 1;
		}
	}
	else {	//if tender status indicates CP
		if((TenderMDC.uchMDCData & localMDCMask) != localMDCBitA){ //only bit A, ept is not CP
			//then check if MDC indicates print acct # on CP Tender
			if(!ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT1)){
				return 1;
			}
		}
	}
	
	oddOrEven = (TenderMDC.usAddress) % 2;
	oddOrEven = oddOrEven ? ODD_MDC_BIT3 : EVEN_MDC_BIT3;

	//Check to see if the tender is set to use EPT and if its not
	//then its prints the account number if there is one.
	if(!ParaMDCCheck(TenderMDC.usAddress, oddOrEven)){ 
		if(pszNumber[0] != '/0'){
			return 1;
		}
	}
	//does not print account number
	return 0;
}
/****** End of Source ******/
