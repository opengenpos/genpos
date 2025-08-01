/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  MultiCheck                
* Category    : Print, NCR 2170  US Hospitarity Application
* Program Name: PrRMulPT.C                                                      
* --------------------------------------------------------------------------
* Abstruct:
*      PrtMultiPost() : print items specified minor class "CLASS_MULTIPOSTCHECK"
*      PrtMulPost_TH() : print multi check (thermal)
*      PrtMulPost_EJ() : print multi check (electric journal)
*      PrtMulPost_SP() : print multi check (slip)
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 00.00.01 : R.Itoh     :                                    
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

/*
============================================================================
+                        I N C L U D E   F I L E s
============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
/* #include <para.h> */
#include <pmg.h>
#include "prtrin.h"
#include "prrcolm_.h"

/**
;========================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/

/*
*===========================================================================
** Format  : VOID   PrtMulPost_TH(TRANINFORMATION *pTran, ITEMMULTI *pItem);      
*
*   Input  : TRANINFORMATION   *pTran     -Transaction Informaion address
*   Input  : ITEMMULTI         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multi check in post check. (thermal)
*===========================================================================
*/
static VOID   PrtMulPost_TH(CONST TRANINFORMATION *pTran, CONST ITEMMULTI *pItem)
{
    PrtTHHead(pTran->TranCurQual.usConsNo);        /* print header if necessary */

    PrtTHMulChk(pItem->usGuestNo, pItem->uchCdv);  /* checkpaid mnemo, GCF# */

    PrtTHAmtSym(TRN_TTL2_ADR, pItem->lMI, PRT_DOUBLE);       /* check total */
}

/*
*===========================================================================
** Format  : VOID   PrtMulPost_EJ(ITEMMULTI *pItem);      
*
*   Input  : ITEMMULTI         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multi check in post check. 
*                                               (electric journal)
*===========================================================================
*/
static VOID   PrtMulPost_EJ(CONST ITEMMULTI *pItem)
{
    PrtEJMulChk(pItem->usGuestNo, pItem->uchCdv);  /* checkpaid mnemo, GCF# */

    PrtEJAmtSym(TRN_TTL2_ADR, pItem->lMI, PRT_DOUBLE);       /* check total */
}

/*
*===========================================================================
** Format  : VOID   PrtMulPost_SP(TRANINFORMATION  *pTran, ITEMMULTI *pItem);      
*
*   Input  : ITEMMULTI         *pItem     -Item Data address
*            TRANINFORMATION   *pTran     -Transaction Data address
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multi check in post check. (slip)
*===========================================================================
*/
static VOID  PrtMulPost_SP(CONST TRANINFORMATION  *pTran, CONST ITEMMULTI *pItem)
{
    TCHAR   aszSPPrintBuff[2][PRT_SPCOLUMN + 1] = { 0 }; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */

    
    /* -- set check paid mnemonic and guest check No. -- */
    usSlipLine += PrtSPGstChkNo(aszSPPrintBuff[0], pItem);

    /* -- set check total mnemonic and amount -- */
    usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], 
        TRN_TTL2_ADR, pItem->lMI, PRT_DOUBLE);

    /* -- check if paper change is necessary or not -- */
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print the data in the buffer -- */ 
    for (USHORT i = 0; i < usSlipLine; i++) {
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
}

/*
*===========================================================================
** Format  : VOID   PrtMultiPost(TRANINFORMATION  *pTran,ITEMMULTI *pItem)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMMULTI        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multi check in post check.
*===========================================================================
*/
VOID PrtMultiPost(CONST TRANINFORMATION  *pTran, CONST ITEMMULTI *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtMulPost_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        PrtMulPost_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
        PrtMulPost_EJ(pItem);
    }

}
