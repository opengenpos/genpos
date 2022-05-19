/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print US Tax                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRUTax_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtUSTax() : prints tax and addcheck
*               PrtUSTax_RJ() : prints us tax  (receipt & journal)
*               PrtUSTax_SP() : prints us tax  (slip)
*               PrtDflUSTax() : displays tax and addcheck
*
*               PrtDflUSCanTaxForm() : displays tax and addcheck
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-08-92 : 00.00.01 : K.Nakajima : initial                                   
* Jun-10-93 : 01.00.12 : K.You      : Add Tax Exempt feature (Mod. PrtUSTax_RJ)                                   
* Jul-14-93 : 01.00.12 : R.Itoh     : Add PrtDflUSTax()                                   
* Oct-28-93 : 02.00.02 : K.You      : Del. PrtDflUSTax()                                   
* Jan-17-95 : 03.00.00 : M.Ozawa    : Add PrtDflUSTax() for Display on the fly
* Jul-21-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Dec-15-99 : 00.00.01 : M.Ozawa    : Add PrtDflUSCanTaxForm()                                   
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
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <paraequ.h>
/* #include <para.h> */
#include <pmg.h>
#include <regstrct.h>
#include <transact.h>
#include <dfl.h>
#include <prt.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"

/*
*===========================================================================
** Format  : VOID   PrtUSTax(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);   
*            
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tax and addcheck .
*            
*===========================================================================
*/
VOID  PrtUSTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);
    
    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtUSTax_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT) || (fsPrtPrintPort & PRT_JOURNAL)) {  /* receipt, journal print */

        PrtUSTax_RJ(pItem);
    }
}
/*
*===========================================================================
** Format  : VOID PrtUSTax_RJ(ITEMAFFECTION *pItem);   
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints us tax.  (receipt & journal)
*===========================================================================
*/
VOID  PrtUSTax_RJ(ITEMAFFECTION   *pItem)
{
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- print subtotal line R3.1 -- */
    if (pItem->lAmount) {
        if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1]||pUSCanTax->lTaxAmount[2]) {
            PrtRFeed(1);
            PrtRJAmtMnem(TRN_TTL1_ADR, pItem->lAmount);
        }
    }

    PrtRJZeroAmtMnemP(TRN_TXEXM1_ADR, pUSCanTax->lTaxExempt[0], PRT_JOURNAL);
    PrtRJZeroAmtMnemP(TRN_TXBL1_ADR, pUSCanTax->lTaxable[0], PRT_JOURNAL);
    PrtRJZeroAmtMnem(TRN_TX1_ADR, pUSCanTax->lTaxAmount[0]);

    PrtRJZeroAmtMnemP(TRN_TXEXM2_ADR, pUSCanTax->lTaxExempt[1], PRT_JOURNAL);
    PrtRJZeroAmtMnemP(TRN_TXBL2_ADR, pUSCanTax->lTaxable[1], PRT_JOURNAL);
    PrtRJZeroAmtMnem(TRN_TX2_ADR, pUSCanTax->lTaxAmount[1]);

    PrtRJZeroAmtMnemP(TRN_TXEXM3_ADR, pUSCanTax->lTaxExempt[2], PRT_JOURNAL);
    PrtRJZeroAmtMnemP(TRN_TXBL3_ADR, pUSCanTax->lTaxable[2], PRT_JOURNAL);
    PrtRJZeroAmtMnem(TRN_TX3_ADR, pUSCanTax->lTaxAmount[2]);
}

            
/*
*===========================================================================
** Format  : VOID PrtUSTax_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);   
*            
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints us tax.  (slip)
*===========================================================================
*/
VOID PrtUSTax_SP(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem)
{
    TCHAR  aszSPPrintBuff[4][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- initialize the buffer -- */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- print subtotal line R3.1 -- */
    if (pItem->lAmount) { 
        if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1]||pUSCanTax->lTaxAmount[2]) {
            usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[0], TRN_TTL1_ADR, pItem->lAmount);
        }
    }
    /* -- set tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[0]) { 
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_TX1_ADR, pUSCanTax->lTaxAmount[0]);
    }
    if (pUSCanTax->lTaxAmount[1]) { 
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_TX2_ADR, pUSCanTax->lTaxAmount[1]);
    }
    if (pUSCanTax->lTaxAmount[2]) { 
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_TX3_ADR, pUSCanTax->lTaxAmount[2]);
    }

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
}

/*
*===========================================================================
** Format  : VOID   PrtDflUSTax(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);   
*            
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tax and addcheck .
*            
*===========================================================================
*/
VOID  PrtDflUSTax(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem)
{
    TCHAR  aszDflBuff[14][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo, usSav;                   /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* --- if this frame is 1st frame, display customer name --- */

    PrtDflCustHeader( pTran );

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
    usSav = usLineNo;

    /* -- set subtotal line R3.1 -- */
    if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1]||pUSCanTax->lTaxAmount[2]) {
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TTL1_ADR, pItem->lAmount); 
    }

    /* -- set item data -- */
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXEXM1_ADR, pUSCanTax->lTaxExempt[0]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXBL1_ADR, pUSCanTax->lTaxable[0]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TX1_ADR, pUSCanTax->lTaxAmount[0]); 

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXEXM2_ADR, pUSCanTax->lTaxExempt[1]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXBL2_ADR, pUSCanTax->lTaxable[1]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TX2_ADR, pUSCanTax->lTaxAmount[1]); 

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXEXM3_ADR, pUSCanTax->lTaxExempt[2]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXBL3_ADR, pUSCanTax->lTaxable[2]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TX3_ADR, pUSCanTax->lTaxAmount[2]); 

    /* -- not send if no tax data -- */ 
    if ( usLineNo == usSav ) {
        return;
    }

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_TAX); 

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
** Format  : VOID   PrtDflUSTax(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);   
*            
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tax and addcheck .
*            
*===========================================================================
*/
USHORT  PrtDflUSCanTaxForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer)
{
    TCHAR  aszDflBuff[17][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo=0, i;                   /* number of lines to be displayed */
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

#if 0
    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif

    /* -- set item data -- */
    if (pItem->sItemCounter== 0) {
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXEXM1_ADR, pUSCanTax->lTaxExempt[0]); 
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXBL1_ADR, pUSCanTax->lTaxable[0]); 
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TX1_ADR, pUSCanTax->lTaxAmount[0]); 
    } else
    if (pItem->sItemCounter==1) {
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXEXM2_ADR, pUSCanTax->lTaxExempt[1]); 
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXBL2_ADR, pUSCanTax->lTaxable[1]); 
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TX2_ADR, pUSCanTax->lTaxAmount[1]); 
    } else
    if (pItem->sItemCounter==2) {
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXEXM3_ADR, pUSCanTax->lTaxExempt[2]); 
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXBL3_ADR, pUSCanTax->lTaxable[2]); 
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TX3_ADR, pUSCanTax->lTaxAmount[2]); 
    } else {
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXEXM4_ADR, pUSCanTax->lTaxExempt[3]); 
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXBL4_ADR, pUSCanTax->lTaxable[3]); 
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TX4_ADR, pUSCanTax->lTaxAmount[3]); 
    }
    
    for (i=0; i<usLineNo; i++) {

        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    //memcpy(puchBuffer, aszDflBuff, usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/***** End Of Source *****/
