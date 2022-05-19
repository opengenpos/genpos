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
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print common routine, reg mode, validation 55 Char                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRCmnVS.C
*               | ||  ||
*               | ||  |+- S:21Char L:24Char _:not influcenced by print column
*               | ||  +-- R:recept & jounal S:slip V:valication K:kitchen 
*               | |+----- comon routine
*               | +------ R:reg mode S:super mode
*               +-------- Pr:print module
* --------------------------------------------------------------------------
* Abstract: The provided function whose names are as follows:
*   
*     PrtVSDeptPlu()     : dept/PLU mnemonics and amount
*     PrtVSAmtMnem()     : transaction mnemonics and amount
*     PrtVSTotal()       : total mnemonics, symbol, and amount
*     PrtVSTotalAudact() : total mnemonics, symbol, and amount in audaction
*     PrtVSSevTotal()    : service total mnemonics, symbol, and amount
*     PrtVSFCTotal()     : foreign currency mnemonics and amount
*     PrtVSTender()      : tender mnemonics and amount
*     PrtVSAmtPerMnem()  : transaction mnemonics, rate, and amount
*     PrtVSAmtIDMnem()   : transaction mnemonics, server ID, and amount
*
*     PrtVSGetMDC()      : consecutive No., C/S ID, time, or date, depending 
*                          on MDC status
*     PrtVSGetMode()     : symbol for training, transaction void, or void,
*                          depending on current mode        
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-19-92 : 00.00.01 :  R.Itoh    :                                    
* Jul-28-93 : 01.00.12 :  K.You     : add soft check feature. (mod. PrtVSSevTotal)
* Feb-28-95 : 03.00.00 : T.Nakahata : add coupon feature ( PrtVSCoupon )
* Jun-13-97 : 03.01.12 : M.Susuki   : correct sys error ( PrtVSGetMode )
* Aug-08-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
* Aug-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL
*           :          :            :       /GUEST_CHECK_MODEL
* Dec-06-99 : 01.00.00 : hrkato     : Saratoga
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

/* -- MS  -  C --*/
#include	<tchar.h>
#include<stdlib.h>
#include<string.h>

/* -- 2170 local --*/
#include <ecr.h>
#include<regstrct.h>
#include<transact.h>
#include<paraequ.h>
#include<para.h>
#include <csstbpar.h>
#include<pmg.h>
#include<rfl.h>
#include"prtrin.h"
#include"prrcolm_.h"

/*
============================================================================
+                       D E F I N I T I O N s
============================================================================
*/

#define  PRT_VS_TRL_SET         18
#define  PRT_VS_MODE_SET         3

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
CONST TCHAR FARCONST  aszPrtVSAmtMnem[] = _T("%-18s %-3s %-.12s %11l$");         /*8 characters JHHJ    mnem. and amount */
CONST TCHAR FARCONST  aszPrtVSTotal[] = _T("%-18s %-3s %-8.8s %15s");            /*8 characters JHHJ  total line */
CONST TCHAR FARCONST  aszPrtVSForeign[] = _T("%.*l$");                           /* foreign amount */
CONST TCHAR FARCONST  aszPrtVSAmtPerMnem[] = _T("%-18s %-3s %-8.8s %3d%%%11l$"); /*8 characters JHHJ   mnem., %, and amount */
CONST TCHAR FARCONST  aszPrtVSAmtIDMnem[] = _T("%-18s %-3s %-8.8s C%03d%11l$");  /*8 characters JHHJ    total line */

CONST TCHAR FARCONST  aszPrtVSModeSym[] = _T("%c%c%c");          /* symbol for training, T-void, or void */                                 
/**
;========================================================================
;+             P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
                                                            
/*
*===========================================================================
** Format  : VOID  PrtVSDeptPlu(TRANINFORMATION *pTran, CHAR fbMod
*                                           UCHAR *aszMnem, LONG lAmount);
*
*               
*    Input : TRANINFORMATION    *pTran       -transaction information
*            CHAR    fbMod                   -void status   
*            UCHAR   *aszMnem                -pointer of dept/PLU mnemonics 
*            LONG    lAmount                 -Amount 
*
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints dept/PLU mnemonics and amount.
*===========================================================================
*/
VOID  PrtVSDeptPlu(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
    TCHAR      aszMDCBuff[PRT_VS_TRL_SET + 1] = {0};
	TCHAR      aszModeBuff[PRT_VS_MODE_SET + 1] = {0};
    DCURRENCY  lProduct;

    /* -- check MDC status and get mnemonics -- */
    PrtVSGetMDC(aszMDCBuff, pTran);

    /* -- check current mode and get symbol -- */
    PrtVSGetMode(aszModeBuff, pTran, pItem->fbModifier);

    /* -- get noun and condiment 's  product price -- */
    PrtGetAllProduct(pItem, &lProduct);

    /* -- print mnemonics and amount -- */
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSAmtMnem, aszMDCBuff, aszModeBuff,
                       pItem->aszMnemonic, lProduct);
                                     
}

/*
*===========================================================================
** Format  : VOID  PrtVSCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
*
*    Input : TRANINFORMATION    *pTran  -   transaction information
*            ITEMCOUPON         *pItem  -   coupon item address
*
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints coupon item mnemonics and its amount.
*===========================================================================
*/
VOID  PrtVSCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
{
    TCHAR  aszMDCBuff[ PRT_VS_TRL_SET + 1 ];
    TCHAR  aszModeBuff[ PRT_VS_MODE_SET + 1 ];

    /* -- check MDC status and get mnemonics -- */

    memset( aszMDCBuff, 0x00, sizeof( aszMDCBuff ));

    PrtVSGetMDC( aszMDCBuff, pTran );

    /* -- check current mode and get symbol -- */

    memset( aszModeBuff, 0x00, sizeof( aszModeBuff ));

    PrtVSGetMode( aszModeBuff, pTran, pItem->fbModifier );

    /* -- print mnemonics and amount -- */

    PmgPrintf( PMG_PRT_RCT_JNL,
               aszPrtVSAmtMnem,
               aszMDCBuff,
               aszModeBuff,
               pItem->aszMnemonic,
               pItem->lAmount );
                                     
}

/*
*===========================================================================
** Format  : VOID  PrtVSAmtMnem(TRANINFORMATION *pTran, CHAR fbMod,
*                                           UCHAR uchAdr, LONG lAmount);
*
*               
*    Input : TRANINFORMATION    *pTran       -transaction information
*            CHAR    fbMod                   -void status   
*            UCHAR   uchAdr                  -Transacion mnemonics address
*            LONG    lAmount                 -Amount 
*
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints transaction mnemonics and amount.
*===========================================================================
*/
VOID  PrtVSAmtMnem(TRANINFORMATION *pTran, USHORT fbMod, USHORT usAdr, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszMDCBuff[PRT_VS_TRL_SET + 1] = {0};
    TCHAR  aszModeBuff[PRT_VS_MODE_SET + 1] = {0};

    /* -- check MDC status and get mnemonics -- */
    PrtVSGetMDC(aszMDCBuff, pTran);

    /* -- check current mode and get symbol -- */
    PrtVSGetMode(aszModeBuff, pTran, fbMod);

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    /* -- print mnemonics and amount -- */
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSAmtMnem, aszMDCBuff, aszModeBuff, aszTranMnem, lAmount);
}

/*
*===========================================================================
** Format  : VOID  PrtVSTotal(ITEMTOTAL *pItem, TRANINFORMATION *pTran);
*
*               
*    Input : ITEMTOTAL   *pItem              -item information
*            TRANINFORMATION    *pTran       -transaction information
*
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints total mnemonics, symbol and amount.
*===========================================================================
*/
VOID  PrtVSTotal(ITEMTOTAL *pItem, TRANINFORMATION *pTran)
{
	TCHAR  aszTotalMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */
	TCHAR  aszMDCBuff[PRT_VS_TRL_SET + 1] = { 0 };
	TCHAR  aszModeBuff[PRT_VS_MODE_SET + 1] = { 0 };
	TCHAR  aszAmountS[PRT_VLCOLUMN] = { 0 };
	TCHAR  aszAmountD[PRT_VLCOLUMN] = { 0 };
    TCHAR  *pszAmount;
    USHORT usDblAmtLen;

    /* -- check MDC status and get mnemonics -- */
    PrtVSGetMDC(aszMDCBuff, pTran);

    /* -- check current mode and get symbol -- */
    PrtVSGetMode(aszModeBuff, pTran, pItem->fbModifier);

    /* -- get total mnemonics (not total key mnemonics)--*/
	RflGetTranMnem(aszTotalMnem, RflChkTtlAdr(pItem));

    /* -- get native mnemo. and adjust -- */    
    PrtAdjustNative(aszAmountS, pItem->lMI);    

    /* -- get the amount length -- */    
    usDblAmtLen = _tcslen(aszAmountS) * 2;

    /* -- total amount printed double wide? -- */
    if ((pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE)
      && (usDblAmtLen <= PRT_NAT_AMT_LEN)) {

        PrtDouble(aszAmountD, PRT_VLCOLUMN, aszAmountS);
        pszAmount = aszAmountD;

    } else {

        pszAmount = aszAmountS;

    }

    /* -- print native mnemonics, symbol and amount -- */
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSTotal, aszMDCBuff, aszModeBuff,
                        aszTotalMnem, pszAmount);

}

/*
*===========================================================================
** Format  : VOID  PrtVSTotalAudact(ITEMTOTAL *pItem, TRANINFORMATION *pTran);
*
*               
*    Input : ITEMTOTAL   *pItem              -item information
*            TRANINFORMATION    *pTran       -transaction information
*
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints total mnemonics, symbol and amount in audaction.
*===========================================================================
*/
VOID  PrtVSTotalAudact(ITEMTOTAL *pItem, TRANINFORMATION *pTran)
{
	TCHAR  aszTotalMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */
	TCHAR  aszMDCBuff[PRT_VS_TRL_SET + 1] = { 0 };
	TCHAR  aszModeBuff[PRT_VS_MODE_SET + 1] = { 0 };
	TCHAR  aszAmountS[PRT_VLCOLUMN] = { 0 };
	TCHAR  aszAmountD[PRT_VLCOLUMN] = { 0 };
    TCHAR  *pszAmount;
    USHORT usDblAmtLen;

    /* -- check MDC status and get mnemonics -- */
    PrtVSGetMDC(aszMDCBuff, pTran);

    /* -- check current mode and get symbol -- */
    PrtVSGetMode(aszModeBuff, pTran, pItem->fbModifier);

    /* -- get total mnemonics (not total key mnemonics)--*/
	RflGetTranMnem(aszTotalMnem, TRN_AMTTL_ADR);

    /* -- get native mnemo. and adjust -- */    
    PrtAdjustNative(aszAmountS, pItem->lMI);    

    /* -- get the amount length -- */    
    usDblAmtLen = _tcslen(aszAmountS) * 2;

    /* -- total amount printed double wide? -- */
    if ((pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE)
      && (usDblAmtLen <= PRT_NAT_AMT_LEN)) {
        PrtDouble(aszAmountD, PRT_VLCOLUMN, aszAmountS);
        pszAmount = aszAmountD;
    } else {
        pszAmount = aszAmountS;
    }

    /* -- print native mnemonics, symbol and amount -- */
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSTotal, aszMDCBuff, aszModeBuff, aszTotalMnem, pszAmount);

}

/*
*===========================================================================
** Format  : VOID  PrtVSSevTotal(ITEMTOTAL *pItem, TRANINFORMATION *pTran, LONG lAmount);
*
*               
*    Input : ITEMTOTAL   *pItem              -item information
*            TRANINFORMATION    *pTran       -transaction information
*            LONG   lAmount                  -amount total   
*                   
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints service total mnemonics, symbol and amount.
*===========================================================================
*/
VOID  PrtVSSevTotal(ITEMTOTAL *pItem, TRANINFORMATION *pTran, DCURRENCY lAmount)
{
	TCHAR  aszTotalMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszMDCBuff[PRT_VS_TRL_SET + 1] = {0};
    TCHAR  aszModeBuff[PRT_VS_MODE_SET + 1] = {0};
    TCHAR  aszAmountS[PRT_VLCOLUMN] = {0};
    TCHAR  aszAmountD[PRT_VLCOLUMN] = {0};
    TCHAR  *pszAmount;
    USHORT usDblAmtLen;

    /* -- check MDC status and get mnemonics -- */
    PrtVSGetMDC(aszMDCBuff, pTran);

    /* -- check current mode and get symbol -- */
    PrtVSGetMode(aszModeBuff, pTran, pItem->fbModifier);

    /* -- get total mnemonics (not total key mnemonics)--*/
	RflGetTranMnem(aszTotalMnem, RflChkTtlAdr(pItem));

    /* -- get native mnemo. and adjust -- */    
    PrtAdjustNative(aszAmountS, lAmount);    

    /* -- get the amount length -- */    
    usDblAmtLen = _tcslen(aszAmountS) * 2;

    /* -- total amount printed double wide? -- */
    if ((pItem->auchTotalStatus[3] & CURQUAL_TOTAL_PRINT_DOUBLE) && (usDblAmtLen <= PRT_NAT_AMT_LEN)) {
        PrtDouble(aszAmountD, PRT_VLCOLUMN, aszAmountS);
        pszAmount = aszAmountD;
    } else {
        pszAmount = aszAmountS;
    }

    /* -- print native mnemonics, symbol and amount -- */
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSTotal, aszMDCBuff, aszModeBuff, aszTotalMnem, pszAmount);
}

/*
*===========================================================================
** Format  : VOID  PrtVSSevTotalPost(ITEMTOTAL *pItem, TRANINFORMATION *pTran);
*
*               
*    Input : ITEMTOTAL    *pItem             -item information
*            TRANINFORMATION    *pTran       -transaction information
*            SHORT    sType                  -print type single/double
*
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints check sum mnemonics, symbol and amount.
*===========================================================================
*/
VOID  PrtVSSevTotalPost(ITEMTOTAL *pItem, TRANINFORMATION *pTran, SHORT sType)
{
	TCHAR  aszTotalMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */
	TCHAR  aszMDCBuff[PRT_VS_TRL_SET + 1] = { 0 };
	TCHAR  aszModeBuff[PRT_VS_MODE_SET + 1] = { 0 };
	TCHAR  aszAmountS[PRT_VLCOLUMN] = { 0 };
	TCHAR  aszAmountD[PRT_VLCOLUMN] = { 0 };
    TCHAR  *pszAmount;
    USHORT usDblAmtLen;

    /* -- check MDC status and get mnemonics -- */
    PrtVSGetMDC(aszMDCBuff, pTran);

    /* -- check current mode and get symbol -- */
    PrtVSGetMode(aszModeBuff, pTran, pItem->fbModifier);

    /* -- get total mnemonics (not total key mnemonics)--*/
	RflGetTranMnem(aszTotalMnem, TRN_CKSUM_ADR);

    /* -- get native mnemo. and adjust -- */    
    PrtAdjustNative(aszAmountS, pItem->lMI);    

    /* -- get the amount length -- */    
    usDblAmtLen = _tcslen(aszAmountS) * 2;

    /* -- total amount printed double wide? -- */
    if ((sType == PRT_DOUBLE) && (usDblAmtLen <= PRT_NAT_AMT_LEN)) {
        PrtDouble(aszAmountD, PRT_VLCOLUMN, aszAmountS);
        pszAmount = aszAmountD;
    } else {
        pszAmount = aszAmountS;
    }

    /* -- print native mnemonics, symbol and amount -- */
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSTotal, aszMDCBuff, aszModeBuff, aszTotalMnem, pszAmount);

}

/*
*===========================================================================
** Format  : VOID  PrtVSFCTotal(ITEMTENDER  *pItem, TRANINFORMATION *pTran)
*               
*    Input : ITEMTENDER    *pItem           -item information
*            TRANINFORMATION    *pTran      -transaction information
*
*   Output : none                   
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints foreign tender mnemonics, symbol, and amount.
*===========================================================================
*/
VOID  PrtVSFCTotal(ITEMTENDER  *pItem, TRANINFORMATION *pTran)
{
	UCHAR  uchAdr1;
	USHORT usTranAdr2;
	USHORT usStrLen;
    SHORT  sDecPoint;
	TCHAR  aszMDCBuff[PRT_VS_TRL_SET + 1] = { 0 };
	TCHAR  aszModeBuff[PRT_VS_MODE_SET + 1] = { 0 };
	TCHAR  aszFAmt[PRT_VLCOLUMN] = { 0 };
	TCHAR  aszFSym[PARA_SPEMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszTendMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */

    /* -- check MDC status and get mnemonics -- */
    PrtVSGetMDC(aszMDCBuff, pTran);

    /* -- check current mode and get symbol -- */
    PrtVSGetMode(aszModeBuff, pTran, pItem->fbModifier);

    /* -- check decimal point -- */
    if (pItem->auchTendStatus[0] & ITEMTENDER_STATUS_0_DECIMAL_3) {
        if (pItem->auchTendStatus[0] & ITEMTENDER_STATUS_0_DECIMAL_0) {
            sDecPoint = PRT_NODECIMAL;
        } else {
            sDecPoint = PRT_3DECIMAL;
        }
    } else {
        sDecPoint = PRT_2DECIMAL;
    }

	PrtGetMoneyMnem (pItem->uchMinorClass, &usTranAdr2, &uchAdr1);

	RflGetSpecMnem(aszFSym, uchAdr1);
	RflGetTranMnem(aszTendMnem, usTranAdr2);

    usStrLen = _tcslen(aszFSym);            /* get foreign symbole length */

    RflSPrintf(&aszFAmt[usStrLen], TCHARSIZEOF(aszFAmt), aszPrtVSForeign, sDecPoint, pItem->lForeignAmount);

    /* -- adjust sign ( + , - ) -- */
    if (pItem->lForeignAmount < 0) {
        aszFAmt[0] = _T('-');
        _tcsncpy(&aszFAmt[1], aszFSym, usStrLen);
    } else {
        _tcsncpy(aszFAmt, aszFSym, usStrLen);
    }

    /* -- print foreign mnemonics, symbol and amount -- */
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSTotal, aszMDCBuff, aszModeBuff, aszTendMnem, aszFAmt);
}

/*
*===========================================================================
** Format  : VOID  PrtVSTender(ITEMTENDER *pItem, TRANINFORMATION *pTran);
*
*               
*    Input : ITEMTOTAL   *pItem              -item information
*            TRANINFORMATION    *pTran       -transaction information
*            CHAR    fbMod                   -void status   
*
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints total mnemonics, symbol and amount.
*===========================================================================
*/
VOID  PrtVSTender(ITEMTENDER *pItem, TRANINFORMATION *pTran)
{
	TCHAR  aszTendMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */
	TCHAR  aszMDCBuff[PRT_VS_TRL_SET + 1] = { 0 };
	TCHAR  aszModeBuff[PRT_VS_MODE_SET + 1] = { 0 };

    /* -- check MDC status and get mnemonics -- */
    PrtVSGetMDC(aszMDCBuff, pTran);

    /* -- check current mode and get symbol -- */
    PrtVSGetMode(aszModeBuff, pTran, pItem->fbModifier);

    /* -- get tender mnemonics -- */
	RflGetTranMnem(aszTendMnem, RflChkTendAdr(pItem));

    /* -- print native mnemonics, symbol and amount -- */
    PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSAmtMnem, aszMDCBuff, aszModeBuff, aszTendMnem, pItem->lTenderAmount);
                                                
}

/*
*===========================================================================
** Format  : VOID  PrtVSAmtPerMnem(TRANINFORMATION *pTran, CHAR fbMod, 
*                               UCHAR uchAdr, UCHAR uchRate, LONG lAmount);
*               
*    Input : TRANINFORMATION    *pTran       -transaction information
*            CHAR    fbMod                   -void status   
*            UCHAR   uchAdr                  -Transacion mnemonics address
*            UCHAR   uchRate                 -rate
*            LONG    lAmount                 -Amount 
*
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints transaction mnemonics, rate, and 
*            amount.
*===========================================================================
*/
VOID  PrtVSAmtPerMnem(TRANINFORMATION *pTran, USHORT fbMod, USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};
    TCHAR  aszMDCBuff[PRT_VS_TRL_SET + 1] = {0};
    TCHAR  aszModeBuff[PRT_VS_MODE_SET + 1] = {0};

    /* -- check MDC status and get mnemonics -- */
    PrtVSGetMDC(aszMDCBuff, pTran);

    /* -- check current mode and get symbol -- */
    PrtVSGetMode(aszModeBuff, pTran, fbMod);

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, usAdr);

    if (uchRate == 0) {
        /* -- print mnemonics and amount -- */
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSAmtMnem, aszMDCBuff, aszModeBuff, aszTranMnem, lAmount);
    } else {
        /* -- print mnemonics, rate and amount -- */
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSAmtPerMnem, aszMDCBuff, aszModeBuff, aszTranMnem, uchRate, lAmount);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtVSChgTips(TRANINFORMATION *pTran, ITEMDISC *pItem); 
*               
*    Input : TRANINFORMATION    *pTran       -transaction information
*            ITEMDISC           *pItem       -item information
*
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints charge tips mnemonics, server ID,  
*            and amount.
*===========================================================================
*/
VOID  PrtVSChgTips(TRANINFORMATION *pTran, ITEMDISC *pItem)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszMDCBuff[PRT_VS_TRL_SET + 1] = { 0 };
	TCHAR  aszModeBuff[PRT_VS_MODE_SET + 1] = { 0 };
    USHORT usAdr;

    /* -- check MDC status and get mnemonics -- */
    PrtVSGetMDC(aszMDCBuff, pTran);

    /* -- check current mode and get symbol -- */
    PrtVSGetMode(aszModeBuff, pTran, pItem->fbDiscModifier);

    /* -- get transaction  mnemonics -- */
    usAdr = RflChkDiscAdr(pItem);      /* set discount mnemonic */

	RflGetTranMnem(aszTranMnem, usAdr);

    if ((pItem->ulID != 0) && (pTran->TranGCFQual.ulCashierID != pTran->TranModeQual.ulCashierID)) {
        /* -- print mnemonics, ID, and amount -- */
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSAmtIDMnem, aszMDCBuff, aszModeBuff, aszTranMnem, pItem->ulID, pItem->lAmount);
    } else {    
        /* -- print mnemonics and amount -- */
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSAmtMnem, aszMDCBuff, aszModeBuff, aszTranMnem, pItem->lAmount);
    }

}

/*
*===========================================================================
** Format  : VOID  PrtVSTipsPO(TRANINFORMATION *pTran, ITEMMISC *pItem); 
*               
*    Input : TRANINFORMATION    *pTran       -transaction information
*            ITEMMISC           *pItem       -item information
*
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints prints tips p/o mnemonics, server ID, and 
*            amount.
*===========================================================================
*/
VOID  PrtVSTipsPO(TRANINFORMATION *pTran, ITEMMISC *pItem)
{
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszMDCBuff[PRT_VS_TRL_SET + 1] = { 0 };
	TCHAR  aszModeBuff[PRT_VS_MODE_SET + 1] = { 0 };

    /* -- check MDC status and get mnemonics -- */
    PrtVSGetMDC(aszMDCBuff, pTran);

    /* -- check current mode and get symbol -- */
    PrtVSGetMode(aszModeBuff, pTran, pItem->fbModifier);

    /* -- get transaction  mnemonics -- */
	RflGetTranMnem(aszTranMnem, TRN_TIPPO_ADR);

    if (pItem->ulID != 0) {
        /* -- print mnemonics, ID, and amount -- */
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSAmtIDMnem, aszMDCBuff, aszModeBuff, aszTranMnem, pItem->ulID, pItem->lAmount);
    } else {    
        /* -- print mnemonics and amount -- */
        PmgPrintf(PMG_PRT_RCT_JNL, aszPrtVSAmtMnem, aszMDCBuff, aszModeBuff, aszTranMnem, pItem->lAmount);
    }

}

/*                                                            
*===========================================================================
** Format  : VOID  PrtVSGetMDC( UCHAR *pszBuff, TRANINFORMATION *pTran);
*               
*    Input : TRANINFORMATION    *pTran       -transaction information
*   Output : UCHAR    *pszBuff               -destination buffer address 
*    InOut : none
** Return  : none
*
** Synopsis: This function sets consecutive No., C/S ID, time, or date, 
*            depending on MDC status.
*===========================================================================
*/
VOID  PrtVSGetMDC(TCHAR *pszBuff, TRANINFORMATION *pTran)
{
	CONST TCHAR   *aszPrtVSMDC1 = _T("%04d %c%8.8Mu %-7s");   /* cons. No., C/S mnem.& ID, and time */                            
	CONST TCHAR   *aszPrtVSMDC2 = _T("%04d %c%8.8Mu %-8s");   /* cons. No., C/S mnem.& ID, and date */                            
	CONST TCHAR   *aszPrtVSMDC3 = _T("%c%8.8Mu %-7s %-5s");   /* C/S mnem.& ID, time and date */                            
	TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
	TCHAR  aszDate[PRT_DATETIME_LEN + 1] = { 0 };   /* date & time mnemonics */
	TCHAR  aszDMnem[PRT_DATE_LEN + 1] = { 0 };     /* date mnemomics  */
	TCHAR  aszTMnem[PRT_TIME_LEN + 1] = { 0 };     /* time mnemomics "1" for space */
    TCHAR  uchSym;                  /* set 'C' (cashier) or 'S' (server) */                                                            
    ULONG  ulID = 0;                    /* set cashier/server ID */
                                                
    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        uchSym  = _T('C');                                                
        ulID    = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
    } else {
        uchSym  = _T('S');                                                
        ulID    = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
    }    
                                                
    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);   /* get date time */
                                                
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_MILITARY) {
        _tcsncpy(aszTMnem, aszDate, PRT_TIME_LEN - 2);    /* get time */                                            
        _tcsncpy(aszDMnem, aszDate + PRT_TIME_LEN - 2 + 1, PRT_DATE_LEN); /* get date */   
    } else {
        _tcsncpy(aszTMnem, aszDate, PRT_TIME_LEN);        /* get time */                                            
        _tcsncpy(aszDMnem, aszDate + PRT_TIME_LEN + 1, PRT_DATE_LEN); /* get date */   
    }                                                
                                                        
    if (( pTran->TranCurQual.flPrintStatus & CURQUAL_VAL_MASK ) == CURQUAL_VAL_TYPE1 ) {      
        RflSPrintf(pszBuff, PRT_VS_TRL_SET + 1, aszPrtVSMDC1, pTran->TranCurQual.usConsNo, uchSym, ulID, aszTMnem);
    } else if (( pTran->TranCurQual.flPrintStatus & CURQUAL_VAL_MASK ) == CURQUAL_VAL_TYPE2 ) {      
        RflSPrintf(pszBuff, PRT_VS_TRL_SET + 1, aszPrtVSMDC2, pTran->TranCurQual.usConsNo, uchSym, ulID, aszDMnem);
    } else if (( pTran->TranCurQual.flPrintStatus & CURQUAL_VAL_MASK ) == CURQUAL_VAL_TYPE3 ) {      
        aszDMnem[5] = _T('\0');     /* set null 6-13-97 */
        RflSPrintf(pszBuff, PRT_VS_TRL_SET + 1, aszPrtVSMDC3, uchSym, ulID, aszTMnem, aszDMnem);
    }                                    
                                                        
}

/*                                                            
*===========================================================================
** Format  : VOID  PrtVSGetMode(UCHAR *pszBuff, TRANINFORMATION *pTran, CHAR fbMod);
*               
*    Input : TRANINFORMATION    *pTran       -transaction information
*            CHAR    fbMod                   -void status   
*   Output : UCHAR   *pszBuff                -destination buffer address 
*    InOut : none
** Return  : none
*
** Synopsis: This function sets symbol for training, transaction void, or void, 
*            depending on current mode.
*===========================================================================
*/
VOID  PrtVSGetMode(TCHAR *pszBuff, TRANINFORMATION *pTran, USHORT fbMod)
{
    TCHAR  uchTrainSym = _T(' ');                               
    TCHAR  uchTVoidSym = _T(' ');                               
    TCHAR  uchVoidSym = _T(' ');                               
                                   
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAINING ) {
        uchTrainSym = _T('*');                          /* training mode ? */     
    }                               
                       
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_PVOID ) {
        uchTVoidSym = _T('T');                          /* transaction void ? */     
    }                               
                       
    if ( fbMod & VOID_MODIFIER ) {
        uchVoidSym = _T('V');                           /* void ? */    
    }                               

    RflSPrintf(pszBuff, PRT_VS_MODE_SET + 1, aszPrtVSModeSym, 
                              uchTrainSym, uchTVoidSym, uchVoidSym);

}

/****** End Of File *****/