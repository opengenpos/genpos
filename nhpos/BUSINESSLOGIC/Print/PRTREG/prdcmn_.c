/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*                                                 **
**        *  NCR 2170   *             AT&T GIS Corporation, E&M OISO      **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1995              **
**    <|\/~             ~\/|>                                             **
**   _/^\_               _/^\_                                            **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print common routine for display on the fly                  
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrDCmn_.C
*               | ||  |
*               | ||  +-- not influcenced by print column
*               | |+----- comon routine
*               | +------ D:display on the fly
*               +-------- Pr:print module
* --------------------------------------------------------------------------
* Abstract:
*
*   PrtDflVoid()
*   PrtDflNumber()
*   PrtDflTaxMod()
*   PrtDflSeatNo()          add at R3.1
*   PrtDflWeight()
*#  PrtDflQty()             modify at R3.0
*   PrtDflItems()
*   PrtDflChild()
*   PrtDflMnem()
*   PrtDflPerDisc()
*#  PrtDflCpnItem()         add at R3.0
*   PrtDflWaiTaxMod()
*   PrtDflAmtMnem()
*   PrtDflWaiter()
*   PrtDflGuest()
*   PrtDflTranNum()
*   PrtDflAmtSym()
*   PrtDflMnemCount()
*   PrtDflZeroAmtMnem() 
*   PrtDflForeign1()
*   PrtDflForeign2()
*#  PrtDflTblPerson()       modify at R3.0
*#  PrtDflCustName()        add at R3.0
*   PrtDflMulChk()
*   PrtDflOffTend()
*   PrtDflCPRoomCharge()    add at R3.0
*   PrtDflOffline()         add at R3.0
*   PrtDflFolioPost()       add at R3.0
*   PrtDflCPRspMsgText()    add at R3.0
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jul-10-93 : 01.00.12 : R.Itoh     : initial
* Jan-24-95 : 03.00.00 : M.Ozawa    : Add CPM/EPT tender 
* Mar-03-95 : 03.00.00 : T.Nakahata : Add Coupon Feature (PrtDflCpnItem) and
*                                     Unique Trans No. (PrtDflTblPerson)
* May-30-95 : 03.00.00 : T.Nakahata : Add Promotional PLU with Condiment
* Aug-24-95 : 03.00.04 : T.Nakahata : FVT comment (unique tran#, 4 => 12 Char)
*           :          :            : customer name (max 16 chara)
* Aug-08-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
* Aug-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL
*           :          :            :       /GUEST_CHECK_MODEL
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
/**------- M S - C -------**/
#include	<tchar.h>
#include<stdlib.h>
#include<string.h>
/**------- 2170 local------**/
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h> 
/* #include <para.h> */
#include <rfl.h>
#include <dfl.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "para.h"
#include "csstbpar.h"
#include "pmg.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
extern CONST TCHAR FARCONST  aszPrtRJWeight1[];
extern CONST TCHAR FARCONST  aszPrtRJWeight2[];
extern CONST TCHAR FARCONST  aszPrtRJQuant[];
extern CONST TCHAR FARCONST  aszPrtRJAmtMnem[];
extern CONST TCHAR FARCONST  aszPrtRJChild[];
extern CONST TCHAR FARCONST  aszPrtRJChild2[];
extern CONST TCHAR FARCONST  aszPrtRJModLinkPLU1[];
extern CONST TCHAR FARCONST  aszPrtRJModLinkPLU2[];
extern CONST TCHAR FARCONST  aszPrtRJLinkPLU1[];
extern CONST TCHAR FARCONST  aszPrtRJLinkPLU2[];
extern CONST TCHAR FARCONST  aszPrtRJMnemMnem[];
extern CONST TCHAR FARCONST  aszPrtRJAmtDiscMnem[];
extern CONST TCHAR FARCONST  aszPrtRJWaiTaxMod[];
extern CONST TCHAR FARCONST  aszPrtRJWaiter[];
extern CONST TCHAR FARCONST  aszPrtRJGuest_WTCD[];
extern CONST TCHAR FARCONST  aszPrtRJGuest_WOCD[];
extern CONST TCHAR FARCONST  aszPrtRJMnemNum[];
extern CONST TCHAR FARCONST  aszPrtRJMnemCount[];
extern CONST TCHAR FARCONST  aszPrtRJForeign1[];
extern CONST TCHAR FARCONST  aszPrtRJForeign2[];
extern CONST TCHAR FARCONST  aszPrtRJTblPerson[];
extern CONST TCHAR FARCONST  aszItemAmt[];
extern CONST TCHAR FARCONST  aszPrtRJCPRoomCharge[];
extern CONST TCHAR FARCONST  aszPrtRJOffDate[];
extern CONST TCHAR FARCONST  aszPrtRJOffline[];
extern CONST TCHAR FARCONST  aszPrtJFolioPost[];
extern CONST TCHAR FARCONST  aszPrtRJCPRspMsgText[];
/*** Add for Unique Trans No. (REL 3.0) ***/
extern CONST TCHAR FARCONST  aszPrtRJTransNo[];
extern CONST TCHAR FARCONST  aszPrtRJPerson[];
extern CONST TCHAR FARCONST  aszPrtRJCustomerName[];
/*** Add for Unique Trans No. (REL 3.0) ***/
extern CONST TCHAR FARCONST  aszPrtRJSeat[];    /* R3.1 */
extern CONST TCHAR FARCONST  aszPrtAmtMnemShift[];

extern CONST TCHAR FARCONST  aszPrtRJPPIQuant[];

CONST TCHAR FARCONST  aszPrtDflPLUNo[] = _T("%s");

/**
;========================================================================
;+             P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/*
*===========================================================================
** Format  : USHORT  PrtDflVoid(UCHAR *pszWork, CHAR  fbMod);
*               
*    Input : UCHAR  *pszWork            -display data buffer pointer      
*            CHAR   fbMod               -Void status      
*   Output : none
*    InOut : none
*
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*
** Synopsis: This function sets "void" line with double wide chara.
*            if void status is off, does not set.
*===========================================================================
*/
USHORT PrtDflVoid(TCHAR *pszWork, USHORT  fbMod, USHORT usReasonCode)
{
    TCHAR         aszWork[PRT_DFL_LINE + 1];             /* print work area */

    if (fbMod & (VOID_MODIFIER | RETURNS_MODIFIER_1 | RETURNS_MODIFIER_2 | RETURNS_MODIFIER_3)) {
        memset(aszWork, '\0', sizeof(aszWork));
		if (0 > PrtGetReturns(fbMod, usReasonCode, aszWork, TCHARSIZEOF(aszWork))) {    
			/* -- get void mnemonics instead of returns mnemonic -- */
			PrtGetVoid(aszWork, TCHARSIZEOF(aszWork));
			_tcsncpy(pszWork, aszWork, _tcslen(aszWork));    
		}
		return(1);
    }
    return(0);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflNumber(UCHAR *pszWork, UCHAR  *pszNumber);
*               
*    Input : UCHAR *pszWork            -display data buffer pointer      
*            UCHAR *pszNumber          -number   
*   Output : none
*    InOut : none
*
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*
** Synopsis: This function sets "number" line.
*            if "number = 0", does not set.
*===========================================================================
*/
USHORT  PrtDflNumber(TCHAR *pszWork, TCHAR  *pszNumber)
{
    if (*pszNumber != 0) {
		USHORT  usLen;
		TCHAR   aszNumLine[PRT_DFL_LINE * 2 + 1] = { 0 };

		RflGetTranMnem(aszNumLine, TRN_NUM_ADR );
        PrtSetNumber(aszNumLine, pszNumber);

        usLen = _tcslen(aszNumLine);

        if ( usLen <= PRT_DFL_LINE ) {
            _tcsncpy(pszWork, aszNumLine, usLen);
            return(1);
        } else {
            _tcsncpy(pszWork, aszNumLine, PRT_DFL_LINE);
            pszWork += PRT_DFL_LINE + 1;
            _tcsncpy(pszWork, aszNumLine + PRT_DFL_LINE, usLen - PRT_DFL_LINE);
            return(2);
        }
    }

    return(0);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflNumber(UCHAR *pszWork, UCHAR  *pszNumber);
*               
*    Input : UCHAR *pszWork            -display data buffer pointer      
*            UCHAR *pszNumber          -number   
*   Output : none
*    InOut : none
*
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*
** Synopsis: This function sets "number" line.
*            if "number = 0", does not set.
*===========================================================================
*/
USHORT  PrtDflMnemNumber(TCHAR *pszWork, USHORT usAdr, TCHAR  *pszNumber)
{
    if (*pszNumber != 0) {
		USHORT  usLen;
		TCHAR   aszNumLine[PRT_DFL_LINE * 2 + 1] = { 0 };

        if (usAdr == 0) usAdr = TRN_NUM_ADR;

		RflGetTranMnem(aszNumLine, usAdr );
        PrtSetNumber(aszNumLine, pszNumber);

        usLen = _tcslen(aszNumLine);

        if ( usLen <= PRT_DFL_LINE ) {
            _tcsncpy(pszWork, aszNumLine, usLen);
            return(1);
        } else {
            _tcsncpy(pszWork, aszNumLine, PRT_DFL_LINE);
            pszWork += PRT_DFL_LINE + 1;
            _tcsncpy(pszWork, aszNumLine + PRT_DFL_LINE, usLen - PRT_DFL_LINE);
            return(2);
        }
    }

    return(0);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflTaxMod(UCHAR *pszWork, USHORT fsTax, UCHAR  fbMod);
*               
*    Input : UCHAR *pszWork       -display data buffer pointer      
*          : USHORT fsTax,        -US or Canadian Tax
*            UCHAR  fbMod         -modifier status
*   Output : none
*   InOut  : none
*
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*
** Synopsis: This function sets Tax Modifier line.
*===========================================================================
*/
USHORT  PrtDflTaxMod(TCHAR *pszWork, USHORT fsTax, USHORT  fbMod)
{
	TCHAR  aszTaxMod[PRT_DFL_LINE + 1] = { 0 };

    /* -- get tax modifier mnemonics -- */
    if ( PRT_WITHDATA == PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod) ) {
        _tcsncpy(pszWork, aszTaxMod, _tcslen(aszTaxMod)); 
        return(1);
    }

    return(0);
}

/*
*===========================================================================
** Format  : USHORT   PrtDflSeatNo(UCHAR *pszWork, ITEMSALES *pItem);
*
*   Input  : ITEMSALES *pItem           -seat no
*   Output : UCHAR *pszWork             -destination buffer address
*   InOut  : none
** Return  : USHORT usWriteLen          -write length
*
** Synopsis: This function formats seat no. line to . R3.1
*===========================================================================
*/
USHORT    PrtDflSeatNo(TCHAR *pszWork, ITEMSALES *pItem)
{
	TCHAR   aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };   /* PARA_... defined in "paraequ.h" */
	TCHAR   aszSrc[PRT_DFL_LINE + 1] = { 0 };

	RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */

    RflSPrintf(aszSrc, TCHARSIZEOF(aszSrc), aszPrtRJSeat, aszSpecMnem, pItem->uchSeatNo);

    _tcsncpy(pszWork, aszSrc, _tcslen(aszSrc));

    return (1);

}


/*
*===========================================================================
** Format  : USHORT  PrtDflWeight(UCHAR *pszWork, TRANINFORMATION  *pTran, 
*                                                           ITEMSALES *pItem);
*
*    Input : UCHAR *pszWork                 -display data buffer pointer      
*            TRANINFORMATION  *pTran        -Transaction information address
*            ITEMSALES *pItem               -Item information address
*            
*   Output : none
*   InOut  : none
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*            
** Synopsis: This function sets weight and unit price line.
*===========================================================================
*/
USHORT  PrtDflWeight(TCHAR *pszWork, TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{

	TCHAR  aszWeightSym[PARA_SPEMNEMO_LEN + 1] = { 0 };        /* PARA_... defined in "paraequ.h" */
	TCHAR  aszManuMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };       /* PARA_... defined in "paraequ.h" */
	TCHAR   aszSrc[PRT_DFL_LINE + 1] = { 0 };
    SHORT  sDecPoint;                           /* decimal point */
    LONG   lModQty;                             /* modified qty */

    /* -- get scalable symbol and adjust weight -- */
    PrtGetScale(aszWeightSym, &sDecPoint, &lModQty, pTran, pItem);

    /* -- get Manual weight mnemonics -- */
    if (pItem->fsPrintStatus & PRT_MANUAL_WEIGHT) {
		RflGetTranMnem(aszManuMnem, TRN_MSCALE_ADR);
    }

    /* -- send "weight " and Manual weight mnemonics -- */
    RflSPrintf(aszSrc, TCHARSIZEOF(aszSrc), aszPrtRJWeight1, sDecPoint, (DWEIGHT)lModQty, aszWeightSym, aszManuMnem);

    RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
    pszWork += PRT_DFL_LINE + 1;

    memset(aszSrc, '\0', sizeof(aszSrc));
    RflSPrintf(aszSrc, TCHARSIZEOF(aszSrc), aszPrtRJWeight2, (DWEIGHT)pItem->lUnitPrice, aszWeightSym);

    RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);

    return(2);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflQty(UCHAR *pszWork, ITEMSALES *pItem);
*
*    Input : UCHAR *pszWork             -display data buffer pointer      
*          : ITEMSALES *pItem           -item information
*            
*   Output : none
*   InOut  : none
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*            
** Synopsis: This function sets Qty and unit price line.
*===========================================================================
*/
USHORT  PrtDflQty(TCHAR *pszWork, ITEMSALES *pItem)
{
    DCURRENCY  lPrice = 0;
    USHORT     i;
    TCHAR      aszSrc[PRT_DFL_LINE + 1] = {0};
	TCHAR      auchDummy[NUM_PLU_LEN] = {0};
    USHORT     usNoOfChild;

    /* --- consolidate condiment PLU's price --- */
    usNoOfChild = pItem->uchCondNo + pItem->uchPrintModNo + pItem->uchChildNo;
    for ( i = pItem->uchChildNo, lPrice = pItem->lUnitPrice;
          i < usNoOfChild;
          i++) {

        if (_tcsncmp(pItem->Condiment[ i ].auchPLUNo, auchDummy, NUM_PLU_LEN) != 0 ) {
            lPrice += pItem->Condiment[i].lUnitPrice;
        }
    }

    if (labs(pItem->lQTY) == PLU_BASE_UNIT) {
        return(0);
    } else {
        if (pItem->uchPM) {    /* print "QTY * Product / PM", 2172 */
            RflSPrintf(aszSrc, PRT_DFL_LINE + 1, aszPrtRJPPIQuant, pItem->lQTY / PLU_BASE_UNIT, lPrice, pItem->uchPM);
        } else {
            RflSPrintf(aszSrc, PRT_DFL_LINE + 1, aszPrtRJQuant, pItem->lQTY / PLU_BASE_UNIT, lPrice);
        }
        RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
        return(1);
    }
}

/*
*===========================================================================
** Format  : USHORT  PrtDflQty(UCHAR *pszWork, ITEMSALES *pItem);
*
*    Input : UCHAR *pszWork             -display data buffer pointer      
*          : ITEMSALES *pItem           -item information
*            
*   Output : none
*   InOut  : none
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*            
** Synopsis: This function sets Qty and unit price line.
*===========================================================================
*/
USHORT  PrtDflLinkQty(TCHAR *pszWork, ITEMSALES *pItem)
{
    if (labs(pItem->lQTY) == PLU_BASE_UNIT) {
        return(0);
    } else {
		TCHAR      aszSrc[PRT_DFL_LINE + 1] = {0};
		DCURRENCY  lPrice = pItem->Condiment[0].lUnitPrice;

        RflSPrintf(aszSrc, PRT_DFL_LINE + 1, aszPrtRJQuant, pItem->lQTY / PLU_BASE_UNIT, lPrice);
        RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
        return(1);
    }
}

/*
*===========================================================================
** Format  : USHORT  PrtDflItems(UCHAR *pszWork, ITEMSALES  *pItem);
*
*    Input : UCHAR *pszWork             -display data buffer pointer      
*          : ITEMSALES  *pItem          -structure "itemsales" pointer
*   Output : none
*   InOut  : none
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*            
** Synopsis: This function sets adjective, noun mnemo.,  print modifier, conidiment line.
*===========================================================================
*/
USHORT  PrtDflItems(TCHAR *pszWork, ITEMSALES  *pItem)
{
	TCHAR       aszItem[PRT_MAXITEM + PRT_DFL_LINE] = {0};   /* "+PRT_DFL_LINE" for PrtGet1Line */
    TCHAR       aszSrc[PRT_DFL_LINE + 1] = {0};
    TCHAR       aszAmt[PRT_AMOUNT_LEN + 1] = {0};
    DCURRENCY   lProduct;
    TCHAR       *pszStart;
    TCHAR       *pszEnd;
    USHORT      usLineCo = 0;      
    USHORT      usMnemLen, usAmtLen;
    USHORT      fsStatusSave;

    pszStart = aszItem;

    /* -- set item mnenemonics to buffer -- */
    fsStatusSave = fsPrtStatus;
    fsPrtStatus = PRT_REQKITCHEN;           /* for condiment display */
    PrtSetItem(aszItem, pItem, &lProduct, PRT_ALL_ITEM, 0);
    fsPrtStatus = fsStatusSave;

    for (;;) {

        /* -- get 1 line -- */
        if ( PRT_LAST == PrtGet1Line(pszStart, &pszEnd, PRT_DFL_LINE - PRT_AMOUNTLEN) ) {
            break;
        }

        /* send adjective, noun mnemo, print modifier, condiment */
        _tcsncpy(pszWork, pszStart, (pszEnd - pszStart));
        if ( *pszEnd == PRT_SEPARATOR ) {
            *( pszWork + (pszEnd - pszStart)) = PRT_SEPARATOR;
        }

        /* -- "1" for NULL -- */
        pszWork += PRT_DFL_LINE + 1;

        /* -- "1" for space -- */
        pszStart = pszEnd + 1;
    
        /* -- 1 line set -- */
        usLineCo ++;

    }

    /* get the length of mnemonics */
    usMnemLen = _tcslen(pszStart);

    memset(aszAmt, '\0', sizeof(aszAmt));
    RflSPrintf(aszAmt, TCHARSIZEOF(aszAmt), aszItemAmt, lProduct);
    usAmtLen = _tcslen(aszAmt);
    
    if (usMnemLen + usAmtLen + 1 > PRT_DFL_LINE) {
        tcharnset(pszWork, 0x20, PRT_DFL_LINE);
		_tcsncpy(pszWork, pszStart, usMnemLen);
        usLineCo ++;
        pszWork += PRT_DFL_LINE + 1;
        *pszStart = '\0';
    }

    RflSPrintf(aszSrc, PRT_DFL_LINE + 1, aszPrtRJAmtMnem, pszStart, lProduct);
    RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
    usLineCo ++;
    return( usLineCo );
}

/*
*===========================================================================
** Format  : USHORT  PrtDflItems(UCHAR *pszWork, ITEMSALES  *pItem);
*
*    Input : UCHAR *pszWork             -display data buffer pointer      
*          : ITEMSALES  *pItem          -structure "itemsales" pointer
*   Output : none
*   InOut  : none
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*            
** Synopsis: This function sets adjective, noun mnemo.,  print modifier, conidiment line.
*===========================================================================
*/
USHORT  PrtDflItemsEx(TCHAR *pszWork, ITEMSALES  *pItem)
{
	TCHAR      aszItem[PRT_MAXITEM + PRT_DFL_LINE] = { 0 };   /* "+PRT_DFL_LINE" for PrtGet1Line */
	TCHAR      aszAmt[PRT_AMOUNT_LEN + 1] = { 0 };
    DCURRENCY  lProduct;
    TCHAR      *pszStart;
    USHORT     usLineCo = 0;      
    USHORT     usMnemLen, usAmtLen;
    USHORT     fsStatusSave;

    pszStart = aszItem;

    /* -- set item mnenemonics to buffer -- */
    fsStatusSave = fsPrtStatus;
    fsPrtStatus = PRT_REQKITCHEN;           /* for condiment display */
    PrtSetItem(aszItem, pItem, &lProduct, PRT_ALL_ITEM, 0);
    fsPrtStatus = fsStatusSave;


    /* get the length of mnemonics */
    usMnemLen = _tcslen(pszStart);

    memset(aszAmt, '\0', sizeof(aszAmt));
    RflSPrintf(aszAmt, TCHARSIZEOF(aszAmt), aszItemAmt, lProduct);
    usAmtLen = _tcslen(aszAmt);

	if (usMnemLen &&
		((pszStart[usMnemLen-1] == PRT_SPACE)  ||
		 (pszStart[usMnemLen-1] == PRT_RETURN) ||
		 (pszStart[usMnemLen-1] == PRT_SEPARATOR))) {
		usMnemLen--;
	}
	pszStart[usMnemLen] = PRT_SEPARATOR;

	_tcscat(aszItem, aszAmt);
	usMnemLen = _tcslen(aszItem);
	pszStart[usMnemLen] = PRT_RETURN;
	usMnemLen++;
   	_tcsncpy(pszWork, aszItem, usMnemLen);
	usLineCo = usMnemLen/(PRT_DFL_LINE + 1);
	usLineCo++;

    return( usLineCo );
}

/*
*===========================================================================
** Format  : USHORT  PrtDflChild(UCHAR *pszWork, UCHAR uchAdj, UCHAR *pszMnem);
*
*    Input : UCHAR *pszWork         -display data buffer pointer      
*            UCHAR uchAdj           -Adjective number
*            UCHAR *pszMnem         -Adjective mnemonics address
*            
*   Output : none
*   InOut  : none
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*            
** Synopsis: This function sets child plu line.
*            
*===========================================================================
*/
USHORT  PrtDflChild(TCHAR *pszWork, UCHAR uchAdj, TCHAR *pszMnem)
{
    if (uchAdj != 0) {
		TCHAR    aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = { 0 };
		TCHAR    aszPLUMnem[PARA_PLU_LEN + 1] = { 0 };

        /* --get adjective mnemocnics -- */
		RflGetAdj(aszAdjMnem,  uchAdj);
        
        /* 2172 */
        _tcsncpy(aszPLUMnem, pszMnem, PARA_PLU_LEN);
        if (_tcslen(aszPLUMnem) > 18) {  /* overs RJ column */
            aszPLUMnem[18] = 0;
        }

        /* -- set adjective mnemonics and child plu mnemonics -- */
        RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJChild, aszAdjMnem, aszPLUMnem);
    } else {
        /* print only child mnemonic, 2172 */
        RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJChild2, pszMnem);
    }

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflChild(UCHAR *pszWork, UCHAR uchAdj, UCHAR *pszMnem);
*
*    Input : UCHAR *pszWork         -display data buffer pointer      
*            UCHAR uchAdj           -Adjective number
*            UCHAR *pszMnem         -Adjective mnemonics address
*            
*   Output : none
*   InOut  : none
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*            
** Synopsis: This function sets child plu line.
*            
*===========================================================================
*/
USHORT  PrtDflLinkPLU(TCHAR *pszWork, USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice)
{
    if (uchAdj != 0) {
		TCHAR    aszAdjMnem[PARA_ADJMNEMO_LEN + 1] = {0};
		TCHAR    aszPLUMnem[PARA_PLU_LEN + 1] = {0};

        /* --get adjective mnemocnics -- */
		RflGetAdj(aszAdjMnem,  uchAdj);

        /* 2172 */
        _tcsncpy(aszPLUMnem, pszMnem, PARA_PLU_LEN);
        if (_tcslen(aszPLUMnem) > 18) {  /* overs RJ column */
            aszPLUMnem[18] = 0;
        }

        /* -- set adjective mnemonics and child plu mnemonics -- */
        if (fsModified) {
            RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJModLinkPLU1, aszAdjMnem, aszPLUMnem, lPrice);
        } else {
            RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJLinkPLU1, aszAdjMnem, aszPLUMnem, lPrice);
        }
    } else {
        /* print only child mnemonic, 2172 */
        if (fsModified) {
            RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJModLinkPLU2, pszMnem, lPrice);
        } else {
            RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJLinkPLU2, pszMnem, lPrice);
        }
    }
    return(2);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflMnem(UCHAR *pszWork, UCHAR uchAdr, BOOL fsType);
*               
*    Input : UCHAR *pszWork        -display data buffer pointer      
*            UCHAR uchAdr          -Transacion mnemonics address
*            SHORT fsType          -characvter type
*                                       PRT_SINGLE: single character
*                                       PRT_DOUBLE: double character
*   Output : none
*   InOut  : none
*
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*
** Synopsis: This function sets transaction mnemonics  line.
*===========================================================================
*/
USHORT  PrtDflMnem(TCHAR *pszWork, USHORT usAdr, BOOL fsType)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */
	TCHAR  aszPrintBuff[PRT_DFL_LINE + 1] = { 0 };

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- double wide chara? -- */
    if (fsType == PRT_DOUBLE) {
        /* -- convert single to double -- */
        PrtDouble(aszPrintBuff, TCHARSIZEOF(aszPrintBuff), aszTranMnem);

        /* -- set mnemonics -- */
        _tcsncpy(pszWork, aszPrintBuff, _tcslen(aszPrintBuff));
    } else {
        _tcsncpy(pszWork, aszTranMnem, _tcslen(aszTranMnem));
    }

    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflPerDisc(UCHAR *pszWork, UCHAR uchAdr, UCHAR uchRate, LONG lAmount);
*               
*    Input : UCHAR *pszWork             -display data buffer pointer      
*            UCHAR  uchAdr,             -Transaction mnemonics address
*            UCHAR  uchRate,            -rate
*            LONG   lAmount             -amount
*   Output : none
*   InOut  : none
*
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*
** Synopsis: This function sets % discount line.
*===========================================================================
*/
USHORT  PrtDflPerDisc(TCHAR *pszWork, USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszSrc[PRT_DFL_LINE + 1 + 1] = {0};
    
    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- send mnemoncis and amount -- */
    if (uchRate == 0) {
        /* not print % */
        RflSPrintf(aszSrc, PRT_DFL_LINE + 1, aszPrtRJAmtMnem, aszTranMnem, lAmount);
        RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
    } else {
        RflSPrintf(aszSrc, PRT_DFL_LINE + 1, aszPrtRJAmtDiscMnem, aszTranMnem, uchRate, lAmount);
        RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
    }

    return(1);
}


/*
*===========================================================================
** Format   : USHORT PrtDflCpnItem( UCHAR *pszWork,
*                                   UCHAR *pszMnemonic,
*                                   LONG  lAmount )
*
*    Input  : UCHAR *pszWork        - destination buffer address
*             UCHAR *pszMnemonic    - coupon mnemonic address
*             LONG  lAmount         - amount of coupon
*   Output  : none
*   InOut   : none
*
** Return   : USHORT usLineNo       - Number of lines to be displayed
*
** Synopsis : This function sets coupon mnemonic and its amount
*===========================================================================
*/
USHORT PrtDflCpnItem( TCHAR *pszWork, TCHAR *pszMnemonic, DCURRENCY lAmount )
{
	TCHAR szSource[ PRT_DFL_LINE + 1 + 1 ] = {0};

    /* --- set display string with coupon mnemonic and amount --- */
    RflSPrintf( szSource, ( PRT_DFL_LINE + 1 ), aszPrtRJAmtMnem, pszMnemonic, lAmount );

    RflStrAdjust( pszWork, ( PRT_DFL_LINE + 1 ), szSource, PRT_DFL_LINE, RFL_FEED_OFF );

    return ( 1 );
}

/*
*===========================================================================
** Format  : USHORT  PrtDflWaiTaxMod(UCHAR *pszWork, USHORT usID, USHORT fsTax, UCHAR  fbMod);
*               
*    Input : UCHAR *pszWork       -display data buffer pointer      
*            USHORT usID          -waiter ID
*            USHORT fsTax,        -US or Canadian Tax
*            UCHAR  fbMod         -modifier status
*   Output : none
*   InOut  : none
*
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*
** Synopsis: This function sets Waiter & Tax Modifier line.
*===========================================================================
*/
USHORT  PrtDflWaiTaxMod(TCHAR *pszWork, ULONG ulID, USHORT  fsTax, USHORT  fbMod)
{
	TCHAR  aszTaxMod[PRT_DFL_LINE + 1] = { 0 };
	TCHAR  aszWaiMnem[PARA_SPEMNEMO_LEN + 2] = { 0 };  /* PARA_... defined in "paraequ.h" */
    SHORT  sRet;

    if (ulID != 0) {
        /* -- get waiter mnemonics -- */
		RflGetSpecMnem(aszWaiMnem, SPC_CAS_ADR);
    }

    /* -- get tax modifier mnemonics -- */
    sRet = PrtGetTaxMod(aszTaxMod, TCHARSIZEOF(aszTaxMod), fsTax, fbMod);

    /* if ID=0 and not exist taxmodifer, not print line */
    if (ulID != 0 && sRet == PRT_WITHDATA) {
        RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJWaiTaxMod, aszWaiMnem, RflTruncateEmployeeNumber(ulID), aszTaxMod);
        return(1);
    } else if (ulID != 0) {
        RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJWaiter, aszWaiMnem, RflTruncateEmployeeNumber(ulID));
        return(1);
    } else if (sRet == PRT_WITHDATA) {
        _tcsncpy(pszWork, aszTaxMod, _tcslen(aszTaxMod));
        return(1);
    }

    return(0);
}


/*
*===========================================================================
** Format  : USHORT  PrtDflAmtMnem(UCHAR *pszWork, UCHAR uchAdr, LONG lAmout);
*               
*    Input : UCHAR *pszWork        -display data buffer pointer      
*            UCHAR uchAdr          -Transacion mnemonics address
*            LONG  lAmount         -Amount 
*   Output : none
*    InOut : none
*
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line.
*===========================================================================
*/
USHORT  PrtDflAmtMnem(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszSrc[PRT_DFL_LINE + 1] = { 0 };

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- send mnemoncis and amount -- */
    RflSPrintf(aszSrc, PRT_DFL_LINE + 1, aszPrtRJAmtMnem, aszTranMnem, lAmount);
    RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflWaiter(UCHAR *pszWork, USHORT usWaiID);
*               
*    Input : UCHAR *pszWork        -display data buffer pointer      
*            USHORT usWaiID        - waiter ID 
*   Output : none
*   InOut  : none
*
** Return  : USHORT  usLineNo      -Number of lines to be displayed
*
** Synopsis: This function sets Waiter line.
*            
*===========================================================================
*/
USHORT  PrtDflWaiter(TCHAR *pszWork, ULONG ulWaiID)
{
    if (ulWaiID != 0) {
		TCHAR  aszWaiMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */

        /* -- get waiter mnemonics -- */
		RflGetSpecMnem(aszWaiMnem, SPC_CAS_ADR);
        RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJWaiter, aszWaiMnem, RflTruncateEmployeeNumber(ulWaiID));
        return(1);
    }
    return(0);
}

/*
*===========================================================================
** Format  : UCHAR  PrtDflGuest(UCHAR *pszWork, USHORT usGuestNo, UCHAR uchCDV);
*
*    Input : UCHAR  *pszWork        -display data buffer pointer      
*            USHORT usGuestNo       -guest check No.      
*            UCHAR  uchCDV          -check digits      
*   Output : none
*   InOut  : none
*
** Return  : USHORT  usLineNo      -Number of lines to be displayed
*                
** Synopsis: This functions sets guest check line. 
*===========================================================================
*/
USHORT  PrtDflGuest(TCHAR *pszWork, USHORT usGuestNo, UCHAR uchCDV)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */

    if (usGuestNo == 0) {                       /* GCF# 0, doesnot print */
        return(0);
    }

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, TRN_GCNO_ADR);

    if (uchCDV == 0) {

        RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJGuest_WOCD, aszTranMnem, usGuestNo);
    } else {

        RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJGuest_WTCD, aszTranMnem, usGuestNo, (USHORT)uchCDV);
    }

    return(1);

}

/*
*===========================================================================
** Format  : USHORT  PrtDflTranNum(UCHAR *pszWork, UCHAR uchAdr, ULONG ulNumber);
*               
*    Input : UCHAR  *pszWork        -display data buffer pointer      
*            UCHAR uchAdr,           -Transactioh mnemonics Address
*            ULONG ulNumber           -Number
*            
*   Output : none
*   InOut  : none
*
** Return  : USHORT  usLineNo      -Number of lines to be displayed
*
** Synopsis: This function sets transaction mnemonics and number line.
*===========================================================================
*/
USHORT  PrtDflTranNum(TCHAR *pszWork, USHORT usAdr, ULONG ulNumber)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJMnemNum, aszTranMnem, ulNumber);
    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflAmtSym(UCHAR *pszWork, UCHAR uchAdr, 
*                                           LONG lAmount, BOOL fsType);
*               
*    Input : UCHAR  *pszWork            -display data buffer pointer      
*            UCHAR  uchAdr,             -Transaction mnemonics address
*            LONG   lAmount             -amount
*            SHORT  fsType              -character type
*                                        PRT_SINGLE: single character
*                                        PRT_DOUBLE: double character
*   Output : none
*   InOut  : none
*
** Return  : USHORT  usLineNo      -Number of lines to be displayed
*
** Synopsis: This function sets transaction mnemonic, native mnemonic  line.
*===========================================================================
*/
USHORT  PrtDflAmtSym(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, BOOL fsType)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
	TCHAR  aszSpecAmtS[PARA_SPEMNEMO_LEN +PRT_AMOUNT_LEN + 1] = {0};          /* spec. mnem & amount save area */
	TCHAR  aszSpecAmtD[(PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN) * 2  +1] = {0};   /* spec. mnem. & amount double wide */
	TCHAR  aszSrc[PRT_DFL_LINE*2 + 1] = {0};
    USHORT usMnemLen, usAmtSLen, usAmtDLen;     

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- adjust native mnemonic and amount sign(+.-) -- */
    PrtAdjustNative(aszSpecAmtS, lAmount);

    /* -- convert to double -- */
    PrtDouble(aszSpecAmtD, TCHARSIZEOF(aszSpecAmtD), aszSpecAmtS);

    usMnemLen = _tcslen(aszTranMnem);
    usAmtSLen = _tcslen(aszSpecAmtS);
    usAmtDLen = _tcslen(aszSpecAmtD);

    if ( fsType == PRT_SINGLE )  {
        RflSPrintf(aszSrc, PRT_DFL_LINE*2 + 1, aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtS);
        RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
        return(1);
    }

    if ( usMnemLen+usAmtDLen+1 <= PRT_DFL_LINE ) {
        /* -- double print -- */
        RflSPrintf(aszSrc, PRT_DFL_LINE*2 + 1, aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtD);
        RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
        return(1);
    }

    if ( usMnemLen+usAmtSLen+1 <= PRT_DFL_LINE ) {
        /* -- single print -- */
        RflSPrintf(aszSrc, PRT_DFL_LINE*2 + 1, aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtS);
        RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
    } else {
        if ( usAmtDLen <= PRT_DFL_LINE ) {
            /* -- double print -- */
            RflSPrintf(aszSrc, PRT_DFL_LINE*2 + 1, aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtD);
            RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
        } else {
            /* -- single print -- */
            RflSPrintf(aszSrc, PRT_DFL_LINE*2 + 1, aszPrtRJMnemMnem, aszTranMnem, aszSpecAmtS);
            RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
        }
    }
    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflMnemCount(UCHAR *pszWork, UCHAR uchAdr, SHORT sCount);
*               
*    Input : UCHAR  *pszWork      -display data buffer pointer      
*            UCHAR  uchAdr        -Transaction Mnemnics Address
*            SHORT  sCount        -Counter
*   Output : none
*   InOut  : none
*
** Return  : USHORT  usLineNo      -Number of lines to be displayed
*
** Synopsis: This function sets Trans. Menm.  and short value.
*===========================================================================
*/
USHORT  PrtDflMnemCount(TCHAR *pszWork, USHORT usAdr, SHORT sCount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszSrc[PRT_DFL_LINE * 2 + 1] = { 0 };

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- send mnemoncis and amount -- */
    RflSPrintf(aszSrc, PRT_DFL_LINE*2 + 1, aszPrtRJMnemCount, aszTranMnem, sCount);
    RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflZeroAmtMnem(UCHAR *pszWork, UCHAR uchAddress, LONG lAmount);
*
*    Input : UCHAR  *pszWork      -display data buffer pointer      
*            UCHAR uchAddress    -transaction mnemonics address
*            LONG  lAmount       -change
*   Output : none
*   InOut  : none
** Return  : USHORT  usLineNo      -Number of lines to be displayed
*            
** Synopsis: This function sets change line.
*===========================================================================
*/
USHORT  PrtDflZeroAmtMnem(TCHAR *pszWork, USHORT usAddress, DCURRENCY lAmount)
{
    if (lAmount == 0L) {
        return(0);
    }

    return(PrtDflAmtMnem(pszWork, usAddress, lAmount));
}

/*
*===========================================================================
** Format  : USHORT  PrtDflForeign1(UCHAR *pszWork, LONG lForeign, LONG lRate, 
*                                           UCHAR uchAdr, UCHAR fbStatus);
*                                                           
*    Input : UCHAR  *pszWork      -display data buffer pointer      
*            LONG  lForeign,      -Foreign tender amount
*            UCHAR uchAdr         -Foreign symbol address
*            UCHAR fbStatus       -Foreign currency decimal point status
*
*   Output : none
*   InOut  : none
*
** Return  : USHORT  usLineNo      -Number of lines to be displayed
*
** Synopsis: This function sets foreign tender line.
*            
*===========================================================================
*/
USHORT  PrtDflForeign1(TCHAR *pszWork, DCURRENCY lForeign, UCHAR uchAdr, UCHAR fbStatus)
{
	TCHAR  aszFMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszFAmt[ PRT_DFL_LINE + 1] = {0};
    USHORT usStrLen;
    SHORT  sDecPoint;

    /* -- check decimal point -- */
    if (fbStatus & ITEMTENDER_STATUS_0_DECIMAL_3) {
        if (fbStatus & ITEMTENDER_STATUS_0_DECIMAL_0) {
            sDecPoint = PRT_NODECIMAL;
        } else {
            sDecPoint = PRT_3DECIMAL;
        }
    } else {
        sDecPoint = PRT_2DECIMAL;
    }

    /* -- get foreign symbol -- */
	RflGetSpecMnem(aszFMnem, uchAdr);

    /* -- get string length -- */
    usStrLen = _tcslen(aszFMnem);

    /* -- format foreign amount -- */
    RflSPrintf(&aszFAmt[usStrLen], TCHARSIZEOF(aszFAmt), aszPrtRJForeign1, sDecPoint, lForeign);

    /* -- adjust sign ( + , - ) -- */
    if (lForeign < 0) {
        aszFAmt[0] = _T('-');
        _tcsncpy(&aszFAmt[1], aszFMnem, usStrLen);
    } else {
        _tcsncpy(aszFAmt, aszFMnem, usStrLen);
    }
/***  modified '93-5-27 
    PmgPrint(PrtChkPort(), aszFAmt, usPrtRJColumn);
***/
    _tcsncpy(pszWork, aszFAmt, _tcslen(aszFAmt));
    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflForeign2(UCHAR *pszWork, ULONG ulRate);
*
*    Input : UCHAR  *pszWork      -display data buffer pointer      
*          : ULONG  ulRate        -rate
*            
*   Output : none
*   InOut  : none
** Return  : USHORT  usLineNo      -Number of lines to be displayed
*            
** Synopsis: This function sets foreign tender line.
*===========================================================================
*/
USHORT  PrtDflForeign2(TCHAR *pszWork, ULONG ulRate, UCHAR fbStatus2)
{
    SHORT sDecPoint;

    if (fbStatus2 & ODD_MDC_BIT0) {
        sDecPoint = PRT_6DECIMAL;
    } else {
        sDecPoint = PRT_5DECIMAL;
    }

    RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJForeign2, sDecPoint, ulRate);
    return(1);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflForeign3(UCHAR *pszWork, UCHAR uchAdr, LONG lForeign,
*                                           UCHAR uchAdr, UCHAR fbStatus);
*                                                           
*    Input : UCHAR  *pszWork      -display data buffer pointer      
*            UCHAR uchTranAdr     -Transacion mnemonics address
*            LONG  lForeign,      -Foreign tender amount
*            UCHAR uchAdr         -Foreign symbol address
*            UCHAR fbStatus       -Foreign currency decimal point status
*
*   Output : none
*   InOut  : none
*
** Return  : USHORT  usLineNo      -Number of lines to be displayed
*
** Synopsis: This function sets foreign tender line.
*            
*===========================================================================
*/
USHORT  PrtDflForeign3(TCHAR *pszWork, USHORT usTranAdr, DCURRENCY lForeign, UCHAR uchAdr, UCHAR fbStatus)
{
	TCHAR  aszFMnem[PARA_SPEMNEMO_LEN + 1] = {0};
    TCHAR  aszFAmt[ PRT_DFL_LINE + 1] = {0};
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszSrc[PRT_DFL_LINE*2 + 1] = {0};
    USHORT usStrLen;
    SHORT  sDecPoint;

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usTranAdr);

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

    /* -- get foreign symbol -- */
	RflGetSpecMnem(aszFMnem, uchAdr);

    /* -- get string length -- */
    usStrLen = _tcslen(aszFMnem);

    /* -- format foreign amount -- */
    RflSPrintf(&aszFAmt[usStrLen], TCHARSIZEOF(aszFAmt), aszPrtRJForeign1, sDecPoint, lForeign);

    /* -- adjust sign ( + , - ) -- */
    if (lForeign < 0) {
        aszFAmt[0] = _T('-');
        _tcsncpy(&aszFAmt[1], aszFMnem, usStrLen);
    } else {
        _tcsncpy(aszFAmt, aszFMnem, usStrLen);
    }

    /* -- send mnemoncis and amount -- */
    RflSPrintf(aszSrc, PRT_DFL_LINE*2 + 1, aszPrtRJMnemMnem, aszTranMnem, aszFAmt);
    RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
    return(1);
}

/*
*===========================================================================
** Format   : USHORT PrtDflTblPerson( UCHAR  *pszWork,
*                                     USHORT usTblNo,
*                                     USHORT usNoPerson,
*                                     SHORT  sWidthType )
*
*    Input  : UCHAR  *pszWork   - Destination Data Buffer Pointer
*             USHORT usTblNo,   - Table Number or Unique Trans. No.
*             USHORT usNoPerson - No. of Person
*             SHORT  sWidthType - Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output  : none
*   InOut   : none
** Return   : USHORT usLineNo   - Number of lines to be displayed
*
** Synopsis : This function sets table no., no. of person line.
*            If both TableNo and NoofPerson are "0", function does not work.
*===========================================================================
*/
USHORT PrtDflTblPerson( TCHAR *pszWork, USHORT usTblNo, USHORT usNoPerson, SHORT sWidthType )
{
	TCHAR   aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 }; /* PARA_... defined in "paraequ.h" */
	TCHAR   aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };   /* PARA_... defined in "paraequ.h" */
	TCHAR   aszLeadThru[PARA_LEADTHRU_LEN + 1] = { 0 };
	TCHAR   aszTblNo[PRT_ID_LEN + 1] = { 0 };
	TCHAR   aszNoPer[PRT_ID_LEN + 1] = { 0 };
	TCHAR   aszTransNo[PRT_ID_LEN * 2 + 1] = { 0 };
    USHORT  usNoOfLines;

    /* --- set up special mnemonic of table no., and table no. --- */
    if ( usTblNo != 0 ) {
        _itot(usTblNo, aszTblNo, 10);

        if ( sWidthType == PRT_DOUBLE ) {
            /* --- set up unique transaction number string --- */
            PrtDouble( aszTransNo, ( PRT_ID_LEN * 2 + 1 ), aszTblNo );
            PrtGetLead( aszLeadThru, LDT_UNIQUE_NO_ADR );
        } else {
			RflGetSpecMnem(aszSpecMnem, SPC_TBL_ADR);
        }
    }

    /* --- set up special mnemonic of no. of person, and no. of person--- */
    if ( (usNoPerson != 0) && (!CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT3))) {
        _itot( usNoPerson, aszNoPer, 10 );
		RflGetTranMnem( aszTranMnem, TRN_PSN_ADR );
    }

    usNoOfLines = 0;

    if ( sWidthType == PRT_DOUBLE ) {   /* print with double width */
        /* --- print table no. with double width ( unique tran no. ) --- */
        if ( usTblNo ) {
            RflSPrintf( pszWork, ( PRT_DFL_LINE + 1 ), aszPrtRJTransNo, aszLeadThru, aszTransNo );
            usNoOfLines++;
        }

        /* --- print no. of person with single width --- */
        if ( usNoPerson ) {
            RflSPrintf( pszWork, ( PRT_DFL_LINE + 1 ), aszPrtRJPerson, aszTranMnem, aszNoPer );
            usNoOfLines++;
        }
    } else {                            /* print with single width */
        if ( usNoPerson || usTblNo ) {
            /* --- print table no. and no. of person with single width --- */
            RflSPrintf( pszWork, ( PRT_DFL_LINE + 1 ), aszPrtRJTblPerson, aszSpecMnem, aszTblNo, aszTranMnem, aszNoPer );
            usNoOfLines++;
        }
    }

    return( usNoOfLines );
}

/*
*===========================================================================
** Format   : USHORT PrtDflCustName( UCHAR *pszDest,
*                                    UCHAR *pszCustomerName )
*
*    Input  : UCHAR *pszDest         -  address of destination buffer
*             UCHAR *pszCustomerName -  address of customer name
*   Output  : none
*   InOut   : none
** Return   : USHORT usLineNo   - Number of lines to be displayed
*
** Synopsis : This function sets customer name to destination buffer.
*===========================================================================
*/
USHORT PrtDflCustName( TCHAR *pszDest, TCHAR *pszCustomerName )
{
    if ( *pszCustomerName == '\0' ) {
        return ( 0 );
    }

    if ( *( pszCustomerName + NUM_NAME - 2 ) == PRT_DOUBLE ) {
        *( pszCustomerName + NUM_NAME - 2 ) = '\0';
    }

    RflSPrintf( pszDest, ( PRT_DFL_LINE + 1 ), aszPrtRJCustomerName, pszCustomerName );

    return ( 1 );
}

/*
*===========================================================================
** Format  : USHORT  PrtDflMulChk(UCHAR *pszWork, USHORT usGuestNo, UCHAR uchCDV);
*
*    Input : UCHAR  *pszWork        -display data buffer pointer      
*            USHORT usGuestNo,      -guest no
*            UCHAR uchCDV           -check digit
*   Output : none
*   InOut  : none
** Return  : USHORT  usLineNo      -Number of lines to be displayed
*                
** Synopsis: This functions sets multi check line. 
*===========================================================================
*/
USHORT  PrtDflMulChk(TCHAR *pszWork, USHORT usGuestNo, UCHAR uchCDV)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1];/* PARA_... defined in "paraequ.h" */

    /* -- get transaction mnemonics -- */
	RflGetTranMnem(aszTranMnem, TRN_CKPD_ADR);

    if (usGuestNo == 0) {                       /* GCF# 0, doesnot print */
        return(0);
    }

    if (uchCDV == 0) {
        RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJGuest_WOCD, aszTranMnem, usGuestNo);
    } else {
        RflSPrintf(pszWork, PRT_DFL_LINE + 1, aszPrtRJGuest_WTCD, aszTranMnem, usGuestNo, (USHORT)uchCDV);
    }

    return(1);
}

/*
*===========================================================================
** Format  : VOID  PrtDflOffTend(UCHAR *pszWork, CHAR fbMod);
*               
*    Input : CHAR   fbMod                -Void status      
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints "off line tender" line with double wide chara.
*            if status is off, does not print.
*===========================================================================
*/
USHORT PrtDflOffTend(TCHAR *pszWork, USHORT fbMod)
{
    TCHAR   aszWork[PRT_DFL_LINE + 1];             /* print work area */

    if (fbMod & OFFCPTEND_MODIF) {          
        memset(aszWork, '\0', sizeof(aszWork));

        /* -- get off line tender mnemonics -- */
        PrtGetOffTend(fbMod, aszWork, TCHARSIZEOF(aszWork));
        _tcsncpy(pszWork, aszWork, _tcslen(aszWork));    
        return(1);
    }

    return(0);
}

/*
*===========================================================================
** Format  : VOID  PrtDflCPRoomCharge(UCHAR *pszWork, UCHAR *pRoomNo, UCHAR *pGuestID);
*                                   
*    Input : UCHAR  *pszWork        -display data buffer pointer      
*            UCHAR  *pRoomNo        -Pointer of Room Number 
*            UCHAR  *pGuestID       -Pointer of Guest ID 
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints room no., guest id for charge posting.
*            
*===========================================================================
*/
USHORT  PrtDflCPRoomCharge(TCHAR *pszWork, TCHAR *pRoomNo, TCHAR *pGuestID)
{
    if ((*pRoomNo != '\0') && (*pGuestID != '\0')) {
		TCHAR  aszSpecMnem1[PARA_SPEMNEMO_LEN + 1] = { 0 };
		TCHAR  aszSpecMnem2[PARA_SPEMNEMO_LEN + 1] = { 0 };
		TCHAR  aszSrc[PRT_DFL_LINE + 1] = { 0 };

		RflGetSpecMnem(aszSpecMnem1, SPC_ROOMNO_ADR);   /* get room no. mnemo */
		RflGetSpecMnem(aszSpecMnem2, SPC_GUESTID_ADR);  /* get guest id */

        RflSPrintf(aszSrc, TCHARSIZEOF(aszSrc), aszPrtRJCPRoomCharge, aszSpecMnem1, pRoomNo, aszSpecMnem2, pGuestID);

        RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);

        return(1);
    }
    return(0);
}

/*
*===========================================================================
** Format  : VOID  PrtDflOffline(UCHAR *pszWork, SHORT fsMod, USHORT usDate, UCHAR *aszApproval);
*               
*    Input : UCHAR  *pszWork        -display data buffer pointer      
*            SHORT fsMod           -offline modifier
*            USHORT usDate         -expiration date 
*            UCHAR  *aszApproval   -approval code
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints EPT offline symbol,
*            expiration date and approval code.
*
*===========================================================================
*/
USHORT  PrtDflOffline(TCHAR *pszWork, USHORT fbMod, TCHAR *auchDate, TCHAR *auchApproval)
{
	TCHAR  aszApproval[NUM_APPROVAL + 1] = { 0 };
	TCHAR  aszDate[NUM_EXPIRA + 1] = { 0 };
	TCHAR  aszOffMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszOffDate[5 + 1] = { 0 };
    USHORT usOffDate;

    _tcsncpy(aszApproval, auchApproval, NUM_APPROVAL);
    _tcsncpy(aszDate, auchDate, NUM_EXPIRA);

    if ( fbMod & OFFEPTTEND_MODIF) {                /* offline modifier */
        /* -- get special mnemonics -- */
		RflGetSpecMnem( aszOffMnem, SPC_OFFTEND_ADR);
    }           

    if ( *aszDate != '\0' ){                        /* expiration date  */
        usOffDate = _ttoi(aszDate);
        if (usOffDate != 0) {                       /* E-028 corr. 4/20     */
            RflSPrintf( aszOffDate, TCHARSIZEOF(aszOffDate), aszPrtRJOffDate, usOffDate / 100, usOffDate % 100 );
        }
    }

    /* -- send mnemoncis and exp.date -- */
    if (*aszDate != '\0' || *aszApproval != '\0' || *aszOffMnem != '\0') {
        RflSPrintf(pszWork, PRT_DFL_LINE+1, aszPrtRJOffline, aszOffMnem, aszOffDate, aszApproval );
        return(1);
    }
    return(0);
}

/*
*===========================================================================
** Format  : VOID  PrtDflFolioPost(UCHAR *pszWork, UCHAR *pszFolioNo, UCHAR *pszPostTranNo)
*                                   
*    Input : UCHAR  *pszWork        -display data buffer pointer      
*            UCHAR *pszFolioNo      -folio. number   
*            UCHAR *pszPostTranNo   -posted transaction number   
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints "folio. and posted transaction number" line.
*            if "0", does not print.
*===========================================================================
*/
USHORT  PrtDflFolioPost(TCHAR *pszWork, TCHAR *pszFolioNo, TCHAR *pszPostTranNo)
{
	if ((*pszFolioNo != '\0') && (*pszPostTranNo != '\0')) {
		TCHAR  aszSpecMnem1[PARA_SPEMNEMO_LEN + 1] = { 0 };
		TCHAR  aszSpecMnem2[PARA_SPEMNEMO_LEN + 1] = { 0 };
		TCHAR  aszSrc[PRT_DFL_LINE + 1] = { 0 };

		RflGetSpecMnem(aszSpecMnem1, SPC_FOLINO_ADR);   /* get folio no. */
		RflGetSpecMnem(aszSpecMnem2, SPC_POSTTRAN_ADR); /* get posted transaction no. */

        RflSPrintf(aszSrc, TCHARSIZEOF(aszSrc), aszPrtJFolioPost, aszSpecMnem1, pszFolioNo, aszSpecMnem2, pszPostTranNo);

        RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);

        return(1);
    }
    return(0);
}

/*
*===========================================================================
** Format  : VOID  PrtDflCPRspMsgText(UCHAR *pszWork, UCHAR *pRspMsgText);
*                                   
*    Input : UCHAR  *pszWork        -display data buffer pointer      
*            UCHAR  *pRspMsgText    -Pointer of Response Message Text 
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints response message text for charge posting.
*            
*===========================================================================
*/
USHORT  PrtDflCPRspMsgText(TCHAR *pszWork, TCHAR *pRspMsgText)
{
    USHORT  usLen;

    if (*pRspMsgText != '\0') {

        usLen = _tcslen(pRspMsgText);

        if ( usLen <= PRT_DFL_LINE ) {
            _tcsncpy(pszWork, pRspMsgText, usLen);
            return(1);
        } else {
            _tcsncpy(pszWork, pRspMsgText, PRT_DFL_LINE);
            pszWork += PRT_DFL_LINE + 1;
            _tcsncpy(pszWork, pRspMsgText + PRT_DFL_LINE, usLen - PRT_DFL_LINE);
            return(2);
        }
    }
    return(0);
}

/*
*===========================================================================
** Format  : VOID  PrtDflZAMnemShift(UCHAR uchAddress, LONG lAmount, USHOT usColumn);
*
*   Input  : UCHAR uchAddress    -transaction mnemonics address
*            LONG  lAmount       -change
*            USHORT usColumn     -shift column
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints mnemonics. and amount V3.3
*===========================================================================
*/
USHORT PrtDflZAMnemShift(TCHAR *pszWork, USHORT usTranAdr, DCURRENCY lAmount, USHORT usColumn)
{
    if (lAmount != 0L) {
        return(PrtDflAmtMnemShift(pszWork, usTranAdr, lAmount, usColumn));
    }
    return(0);
}

/*
*===========================================================================
** Format  : VOID  PrtDflAmtMnemShift(UCHAR uchAdr, LONG lAmount, USHORT usColumn);
*               
*    Input : UCHAR uchAdr          -Transacion mnemonics address
*            LONG  lAmount         -Amount
*            USHORT usColumn       -shift column
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line. V3.3
*===========================================================================
*/
USHORT  PrtDflAmtMnemShift(TCHAR *pszWork, USHORT usTranAdr, DCURRENCY lAmount, USHORT usColumn)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszSrc[(PRT_DFL_LINE*2) + 1] = {0};
    TCHAR  uchNull = '\0';

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usTranAdr);

    /* -- send mnemoncis and amount -- */
    RflSPrintf(aszSrc, (PRT_DFL_LINE*2) + 1, aszPrtAmtMnemShift, aszTranMnem, lAmount, usColumn, &uchNull);

    RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
    return(1);
}

/*
*===========================================================================
** Format  : VOID  PrtDflPLUNo (UCHAR *puchPLUNo);
*
*    Input : UCHAR *puchPLUNo      -PLU or coupon No
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints PLU or vendor coupon number on specified
*            printer.
*
*  Print format :
*
*       0        1         2    
*       123456789012345678901234
*      |^^^^^^^^^^^^^^^^^^^^^^^^|
*      |99999999999999          |
*      |                        |
*       ^^^^^^^^^^^^^^^^^^^^^^^^^
*===========================================================================
*/
USHORT  PrtDflPLUNo(TCHAR *pszDest, TCHAR *puchPLUCpn)
{
    if (*puchPLUCpn) {
        RflSPrintf( pszDest, PRT_DFL_LINE+1, aszPrtDflPLUNo, puchPLUCpn );
        return ( 1 );
    }
    
    return ( 0 );
}

/*
*===========================================================================
** Format   : USHORT PrtDflNumber( UCHAR *pszDest,
*                                    UCHAR *pszNmenonic, UCHAR *pszNumber )
*
*    Input  : UCHAR *pszDest         -  address of destination buffer
*             UCHAR *pszMnemonic     -  address of transaction nmenmonic
*             UCHAR *pszNumber       -  address of number
*   Output  : none
*   InOut   : none
** Return   : USHORT usLineNo   - Number of lines to be displayed
*
** Synopsis : This function sets number to destination buffer.
*===========================================================================
*/
USHORT PrtDflRandomNumber( TCHAR *pszDest, TCHAR *pszMnemonic, TCHAR *pszNumber )
{
    if ( *pszNumber == '\0' ) {
        return ( 0 );
    }

    RflSPrintf( pszDest, ( PRT_DFL_LINE + 1 ), aszPrtRJMnemMnem, pszMnemonic, pszNumber );

    return ( 1 );
}


/*
*===========================================================================
** Format  : USHORT  PrtDflMnemonic(UCHAR *pszWork, UCHAR  *pszMnemonic);
*               
*    Input : UCHAR *pszWork            -display data buffer pointer      
*            UCHAR *pszNmemonic          -number   
*   Output : none
*    InOut : none
*
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*
** Synopsis: This function sets "number" line.
*            if "number = 0", does not set.
*===========================================================================
*/
USHORT  PrtDflMnemonic(TCHAR *pszWork, TCHAR  *pszMnemonic)
{
    USHORT  usLen;
    TCHAR   aszNumLine[PRT_DFL_LINE * 2 + 1];

    if (*pszMnemonic != 0) {

        memset(aszNumLine, '\0', sizeof(aszNumLine));

        PrtSetNumber(aszNumLine, pszMnemonic);

        usLen = _tcslen(aszNumLine);
        if ( usLen <= PRT_DFL_LINE ) {
            _tcsncpy(pszWork, aszNumLine, usLen);
            return(1);
        } else {
            _tcsncpy(pszWork, aszNumLine, PRT_DFL_LINE);
            pszWork += PRT_DFL_LINE + 1;
            _tcsncpy(pszWork, aszNumLine + PRT_DFL_LINE, usLen - PRT_DFL_LINE);
            return(2);
        }
    }

    return(0);
}

/*
*===========================================================================
** Format  : USHORT  PrtDflAmtMnem(UCHAR *pszWork, UCHAR uchAdr, LONG lAmout);
*               
*    Input : UCHAR *pszWork        -display data buffer pointer      
*            UCHAR uchAdr          -Transacion mnemonics address
*            LONG  lAmount         -Amount 
*   Output : none
*    InOut : none
*
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*
** Synopsis: This function prints prints transaction mnemonics
*            and Amount line.
*===========================================================================
*/
USHORT  PrtDflPrtMod(TCHAR *pszWork, UCHAR uchAdr, DCURRENCY lAmount)
{
	TCHAR  aszPrtModMnem[PARA_PRTMODI_LEN + 1] = {0};
    TCHAR  aszSrc[PRT_DFL_LINE + 1] = {0};

    /* -- get print modifier  mnemonics -- */
	RflGetMnemonicByClass(CLASS_PARAPRTMODI, aszPrtModMnem, uchAdr);

    /* -- send mnemoncis and amount -- */
    RflSPrintf(aszSrc, PRT_DFL_LINE + 1, aszPrtRJAmtMnem, aszPrtModMnem, lAmount);
    RflStrAdjust(pszWork, PRT_DFL_LINE + 1, aszSrc, PRT_DFL_LINE, RFL_FEED_OFF);
    return(1);
}


/*
*===========================================================================
** Format  : USHORT  PrtDflTaxMod(UCHAR *pszWork, USHORT fsTax, UCHAR  fbMod);
*               
*    Input : UCHAR *pszWork       -display data buffer pointer      
*          : USHORT fsTax,        -US or Canadian Tax
*            UCHAR  fbMod         -modifier status
*   Output : none
*   InOut  : none
*
** Return  : USHORT  usLineNo           -Number of lines to be displayed
*
** Synopsis: This function sets Tax Modifier line.
*===========================================================================
*/
USHORT  PrtDflTaxMod2(TCHAR *pszWork, USHORT fsTax, UCHAR  uchMinorClass)
{
	TCHAR  aszTaxMod[PRT_DFL_LINE + 1] = { 0 };

    /* -- get tax modifier mnemonics -- */
    if ( PRT_WITHDATA == PrtGetTaxMod2(aszTaxMod, TCHARSIZEOF(aszTaxMod), uchMinorClass) ) {
        _tcsncpy(pszWork, aszTaxMod, _tcslen(aszTaxMod)); 
        return(1);
    }

    return(0);
}


/*
*===========================================================================
** Format  : USHORT   PrtDflSeatNo(UCHAR *pszWork, ITEMSALES *pItem);
*
*   Input  : ITEMSALES *pItem           -seat no
*   Output : UCHAR *pszWork             -destination buffer address
*   InOut  : none
** Return  : USHORT usWriteLen          -write length
*
** Synopsis: This function formats seat no. line to . 2172
*===========================================================================
*/
USHORT    PrtDflModSeatNo(TCHAR *pszWork, TCHAR uchSeatNo)
{
	TCHAR   aszSpecMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };   /* PARA_... defined in "paraequ.h" */
	TCHAR   aszSrc[PRT_DFL_LINE + 1] = { 0 };

	RflGetSpecMnem( aszSpecMnem, SPC_SEAT_ADR );    /* seat no. mnemonic */

    RflSPrintf(aszSrc, TCHARSIZEOF(aszSrc), aszPrtRJSeat, aszSpecMnem, uchSeatNo);

    _tcsncpy(pszWork, aszSrc, _tcslen(aszSrc));

    return (1);

}

/****** End Of Source *****/
