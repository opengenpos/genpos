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
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Charge Tips                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRTipT.c                                                      
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtChrgTip() : prints charge tips
*               PrtChrgTip_TH() : prints charge tips (thermal)
*               PrtChrgTip_EJ() : prints charge tips (electric journal)
*               PrtChrgTip_SP() : prints charge tips (slip)
*               PrtChrgTipID()  : retrieve waiter ID
*
*               PrtDflChrgTip() : displays charge tips
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-10-93 : 01.00.12 :  R.Itoh    : initial                                   
* Jul-13-93 : 01.00.12 :  R.Itoh    : add PrtDflChrgTip()                                   
* Oct-29-93 : 02.00.02 :  K.You     : del. PrtDflChrgTip()                                   
* Apr-10-94 : 00.00.04 :  K.You     : add validation slip print feature.(mod. PrtChrgTip())
* Jan-26-95 : 03.00.00 :  M.Ozawa   : add PrtDflChrgTip()                                   
* Jun-27-95 : 03.00.01 : T.Nakahata : bug fixed (no print S-ID at Drive Thru)
* Aug-08-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
* Dec-13-99 : 00.00.01 :  M.Ozawa   : add PrtDflChrgTipForm()
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
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- M S - C -------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <paraequ.h>
/* #include <para.h> */
/* #include <csstbpar.h> */
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <pmg.h>
#include <dfl.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"
#include "para.h"
#include "csstbpar.h"
#include <rfl.h>

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s 
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtChrgTip(TRANINFORMATION  *pTran, ITEMDISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMDISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints charge tips
*===========================================================================
*/
VOID PrtChrgTip(TRANINFORMATION  *pTran, ITEMDISC  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
            PrtSPVLHead(pTran);    
            PrtChrgTip_SP(pTran, pItem);
            PrtSPVLTrail(pTran);
            return;
        }
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtChrgTip_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        PrtChrgTip_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
        PrtChrgTip_EJ(pTran, pItem);
    }

}

/*
*===========================================================================
** Format  : VOID  PrtChrgTip_TH(TRANINFORMATION  *pTran, ITEMDISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran,    -transaction information
*            ITEMDISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function prints charge tips (thermal)
*===========================================================================
*/
VOID  PrtChrgTip_TH(TRANINFORMATION  *pTran, ITEMDISC *pItem)
{
    USHORT  usAdr;

    usAdr = RflChkDiscAdr(pItem);      /* set discount mnemonic */

    PrtTHHead(pTran);                           /* print header if necessary */

    PrtTHVoid(pItem->fbDiscModifier, pItem->usReasonCode);               /* void line */

    PrtTHNumber(pItem->aszNumber);              /* number line */

    PrtTHWaiTaxMod(PrtChrgTipID(pTran, pItem), pTran->TranModeQual.fsModeStatus, pItem->fbDiscModifier);  /* waiter & taxmodifier */

    PrtTHPerDisc(usAdr, pItem->uchRate, pItem->lAmount);   /* % discount */
}

/*
*===========================================================================
** Format  : VOID  PrtChrgTip_EJ(TRANINFORMATION  *pTran, ITEMDISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran,    -transaction information
*            ITEMDISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function prints charge tips (electric journal)
*===========================================================================
*/
VOID  PrtChrgTip_EJ(TRANINFORMATION  *pTran, ITEMDISC *pItem)
{
    USHORT   usAdr;

    usAdr = RflChkDiscAdr(pItem);      /* set discount mnemonic */

    PrtEJVoid(pItem->fbDiscModifier, pItem->usReasonCode);               /* void line */

    PrtEJNumber(pItem->aszNumber);              /* number line */

    PrtEJWaiTaxMod(PrtChrgTipID(pTran, pItem), pTran->TranModeQual.fsModeStatus, pItem->fbDiscModifier);  /* waiter & taxmodifier */

    PrtEJPerDisc(usAdr, pItem->uchRate, pItem->lAmount);   /* % discount */
}

/*
*===========================================================================
** Format  : VOID  PrtChrgTip_SP(TRANINFORMATION *pTran, ITEMDISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*          : ITEMDISC         *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function prints charge tips
*===========================================================================
*/
VOID PrtChrgTip_SP(TRANINFORMATION *pTran, ITEMDISC *pItem)
{
    TCHAR  aszSPPrintBuff[2][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   

    USHORT  usAdr;

    usAdr = RflChkDiscAdr(pItem);      /* set discount mnemonic */

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- set void mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbDiscModifier, pItem->usReasonCode, pItem->aszNumber);
    /* -- set charge tips mnemonic and amount -- */
    usSlipLine += PrtSPChargeTips(aszSPPrintBuff[usSlipLine], usAdr, PrtChrgTipID(pTran, pItem), pTran->TranModeQual.fsModeStatus, pItem);

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
}

/*
*===========================================================================
** Format  : USHORT  PrtChrgTipID(TRANINFORMATION  *pTran, ITEMDISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran,    -transaction information
*            ITEMDISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : USHORT            usID      -waiter ID or '0' 
*            
** Synopsis: This function prints charge tips
*===========================================================================
*/
ULONG  PrtChrgTipID(TRANINFORMATION  *pTran, ITEMDISC *pItem)
{
    if (( pTran->TranGCFQual.ulCashierID == pTran->TranModeQual.ulCashierID ) ||
        ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) ||    /* R3.3 */
        ( pTran->TranGCFQual.fsGCFStatus & GCFQUAL_DRIVE_THROUGH )) {
        return (0);
    }

    return (pItem->ulID);
}

/*
*===========================================================================
** Format  : VOID  PrtDflChrgTip(TRANINFORMATION  *pTran, ITEMDISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMDISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays charge tips
*===========================================================================
*/
VOID PrtDflChrgTip(TRANINFORMATION  *pTran, ITEMDISC  *pItem)
{
    TCHAR  aszDflBuff[9][PRT_DFL_LINE + 1]; /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       
    USHORT  usAdr;

    usAdr = RflChkDiscAdr(pItem);      /* set discount mnemonic */

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbDiscModifier, pItem->usReasonCode);   

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflWaiTaxMod(aszDflBuff[usLineNo], PrtChrgTipID(pTran, pItem), pTran->TranModeQual.fsModeStatus, pItem->fbDiscModifier);

    usLineNo += PrtDflPerDisc(aszDflBuff[usLineNo], usAdr, pItem->uchRate, pItem->lAmount);

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- check void status -- */
    PrtDflCheckVoid(pItem->fbDiscModifier);

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_DISC); 

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData(aszDflBuff[i], &usOffset);
        if ( aszDflBuff[i][PRT_DFL_LINE] != '\0' ) {
            i++;
        }    
    }

    /* -- send display data -- */
    PrtDflSend();
}

/*
*===========================================================================
** Format  : VOID  PrtDflChrgTip(TRANINFORMATION  *pTran, ITEMDISC *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMDISC         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays charge tips
*===========================================================================
*/
USHORT PrtDflChrgTipForm(TRANINFORMATION  *pTran, ITEMDISC  *pItem, TCHAR *puchBuffer)
{
    TCHAR  aszDflBuff[9][PRT_DFL_LINE + 1]; /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  usAdr;

    usAdr = RflChkDiscAdr(pItem);      /* set discount mnemonic */

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbDiscModifier, pItem->usReasonCode);   

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflWaiTaxMod(aszDflBuff[usLineNo], PrtChrgTipID(pTran, pItem), pTran->TranModeQual.fsModeStatus, pItem->fbDiscModifier);

    usLineNo += PrtDflPerDisc(aszDflBuff[usLineNo], usAdr, pItem->uchRate, pItem->lAmount);

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/***** End Of Source *****/
