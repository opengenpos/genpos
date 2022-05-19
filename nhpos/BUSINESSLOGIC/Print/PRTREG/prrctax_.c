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
* Title       : Print Canada tax                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRCTax_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*           PrtCanadaTax() : print canada tax
*           PrtCanGst() : print canada Gst Pst tax
*           PrtCanGst_RJ() : print canada tax. (receipt & journal)
*           PrtCanGst_SP() : print canada tax. (slip)
*           PrtCanInd() : print canada individual tax 
*           PrtCanInd_RJ() : print canada individual tax (receipt & journal)
*           PrtCanInd_SP() : print canada individual tax (slip)
*           PrtCanAll() : print canada CanAll tax
*           PrtCanAll_RJ() : print canada CanAll tax (receipt & journal)
*           PrtCanInd_SP() : print canada CanAll tax (slip)
*
*           PrtDflCanadaTax() : display canada tax
*           PrtDflCanGst() : display canada Gst Pst tax
*           PrtDflCanInd() : display canada individual tax 
*           PrtDflCanAll() : display canada CanAll tax
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-01-92 : 00.00.01 : K.Nakajima :                                    
* Jul-14-93 : 01.00.12 : R.Itoh     : add PrtDflXXXX() 
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
#include <prt.h>
#include <dfl.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"

/*
*===========================================================================
** Format  : VOID   PrtCanadaTax(TRANINFORMATION  *pTran,
*                                     ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints canada tax.
*===========================================================================
*/      
VOID   PrtCanadaTax(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem)
{


    if (pTran->TranCurQual.flPrintStatus & CURQUAL_CANADA_GST_PST) {
         PrtCanGst(pTran, pItem);

    }  else if (pTran->TranCurQual.flPrintStatus & CURQUAL_CANADA_INDI) {
         PrtCanInd(pTran, pItem);

    }  else {
         PrtCanAll(pTran, pItem);

    }
}

/*
*===========================================================================
** Format  : VOID   PrtCanGst(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
*                                        
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints canada Gst Pst tax.
*===========================================================================
*/
VOID   PrtCanGst(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem)
{

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);


    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtCanGst_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)  /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtCanGst_RJ(pItem);
    }

}

/*
*===========================================================================
** Format  : VOID   PrtCanGst_RJ(ITEMAFFECTION *pItem);   
*            
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints canada tax. (receipt & journal)
*===========================================================================
*/
VOID   PrtCanGst_RJ(ITEMAFFECTION *pItem)
{
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- print subtotal line R3.1 -- */
    if (pItem->lAmount) {
        if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1]) {

            PrtRFeed(1);
            PrtRJAmtMnem(TRN_TTL1_ADR, pItem->lAmount);
        }
    }

    /* -- GST exempt, taxable, tax -- */
    PrtRJZeroAmtMnemP(TRN_TXEXM1_ADR, pUSCanTax->lTaxExempt[0], PRT_JOURNAL);
    PrtRJZeroAmtMnemP(TRN_TXBL1_ADR, pUSCanTax->lTaxable[0], PRT_JOURNAL);
    PrtRJZeroAmtMnem(TRN_TX1_ADR, pUSCanTax->lTaxAmount[0]);

    /* -- PST exempt, taxable, tax -- */
    PrtRJZeroAmtMnemP(TRN_TXEXM2_ADR, pUSCanTax->lTaxExempt[1], PRT_JOURNAL);
    PrtRJZeroAmtMnemP(TRN_TXBL2_ADR, pUSCanTax->lTaxable[1], PRT_JOURNAL);
    PrtRJZeroAmtMnem(TRN_TX2_ADR, pUSCanTax->lTaxAmount[1]);

    /* -- non taxable -- */
    PrtRJZeroAmtMnemP(TRN_NONTX_ADR, pUSCanTax->lTaxable[4], PRT_JOURNAL);
}

/*
*===========================================================================
** Format  : VOID   PrtCanGst_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);   
*            
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints canada tax. (slip)
*===========================================================================
*/
VOID  PrtCanGst_SP(TRANINFORMATION *pTran, ITEMAFFECTION *pItem)
{
    TCHAR  aszSPPrintBuff[3][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- print subtotal line R3.1 -- */
    if (pItem->lAmount) { 
        if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1]) {
            usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[0], TRN_TTL1_ADR, pItem->lAmount);
        }
    }
    /* -- set GST tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[0]) {
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_TX1_ADR, pUSCanTax->lTaxAmount[0]);
    }

    /* -- set PST tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[1]) {
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_PST_ADR, pUSCanTax->lTaxAmount[1]);
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
** Format  : VOID   PrtCanInd(TRANINFORMATION  *pTran,
*                                     ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints canada individual tax.
*===========================================================================
*/
VOID  PrtCanInd(TRANINFORMATION *pTran, ITEMAFFECTION *pItem)
{


    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);


    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtCanInd_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)  /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtCanInd_RJ(pItem);
    }

}
/*
*===========================================================================
** Format  : VOID  PrtCanInd_RJ(ITEMAFFECTION *pItem);
*
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints canada individual tax.
*===========================================================================
*/
VOID  PrtCanInd_RJ(ITEMAFFECTION *pItem)
{
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- print subtotal line R3.1 -- */
    if (pItem->lAmount) {
        if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1]||
            pUSCanTax->lTaxAmount[2]||pUSCanTax->lTaxAmount[3]) {

            PrtRFeed(1);
            PrtRJAmtMnem(TRN_TTL1_ADR, pItem->lAmount);
        }
    }

    /* -- GST exempt, taxable tax -- */
    PrtRJZeroAmtMnemP(TRN_TXEXM1_ADR, pUSCanTax->lTaxExempt[0], PRT_JOURNAL);
    PrtRJZeroAmtMnemP(TRN_TXBL1_ADR, pUSCanTax->lTaxable[0], PRT_JOURNAL);
    PrtRJZeroAmtMnem(TRN_TX1_ADR, pUSCanTax->lTaxAmount[0]);

    /* -- PST1 exempt, taxable tax -- */
    PrtRJZeroAmtMnemP(TRN_TXEXM2_ADR, pUSCanTax->lTaxExempt[1], PRT_JOURNAL);
    PrtRJZeroAmtMnemP(TRN_TXBL2_ADR, pUSCanTax->lTaxable[1], PRT_JOURNAL);
    PrtRJZeroAmtMnem(TRN_TX2_ADR, pUSCanTax->lTaxAmount[1]);

    /* -- PST2 exempt, taxable tax -- */
    PrtRJZeroAmtMnemP(TRN_TXEXM3_ADR, pUSCanTax->lTaxExempt[2], PRT_JOURNAL);
    PrtRJZeroAmtMnemP(TRN_TXBL3_ADR, pUSCanTax->lTaxable[2], PRT_JOURNAL);
    PrtRJZeroAmtMnem(TRN_TX3_ADR, pUSCanTax->lTaxAmount[2]);

    /* -- PST3 exempt, taxable tax -- */
    PrtRJZeroAmtMnemP(TRN_TXEXM4_ADR, pUSCanTax->lTaxExempt[3], PRT_JOURNAL);
    PrtRJZeroAmtMnemP(TRN_TXBL4_ADR, pUSCanTax->lTaxable[3], PRT_JOURNAL);
    PrtRJZeroAmtMnem(TRN_TX4_ADR, pUSCanTax->lTaxAmount[3]);

    /* -- non taxable -- */
    PrtRJZeroAmtMnemP(TRN_NONTX_ADR, pUSCanTax->lTaxable[4], PRT_JOURNAL);
}

/*
*===========================================================================
** Format  : VOID  PrtCanInd_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*          : ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints canada individual tax.
*===========================================================================
*/
VOID  PrtCanInd_SP(TRANINFORMATION *pTran, ITEMAFFECTION *pItem)
{
    TCHAR  aszSPPrintBuff[5][PRT_SPCOLUMN + 1]; /* print data save area */
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
        if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1]||
            pUSCanTax->lTaxAmount[2]||pUSCanTax->lTaxAmount[3]) {
            usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[0], TRN_TTL1_ADR, 
                                                    pItem->lAmount);
        }
    }
    /* -- set GST tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[0]) {
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_TX1_ADR, 
                                            pUSCanTax->lTaxAmount[0]);
    }

    /* -- PST1 tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[1]) {
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_TX2_ADR, 
                                            pUSCanTax->lTaxAmount[1]);
    }

    /* -- PST2 tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[2]) {
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_TX3_ADR, 
                                            pUSCanTax->lTaxAmount[2]);
    }

    /* -- PST3 tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[3]) {
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_TX4_ADR, 
                                            pUSCanTax->lTaxAmount[3]);
    }

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

    /* update current line No. */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
}

/*
*===========================================================================
** Format  : VOID   PrtCanAll(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);   
*            
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints canada CanAll tax.
*===========================================================================
*/
VOID  PrtCanAll(TRANINFORMATION *pTran, ITEMAFFECTION *pItem)
{

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);


    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtCanAll_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)  /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtCanAll_RJ(pItem);
    }

}
/*
*===========================================================================
** Format  : VOID  PrtCanAll_RJ(ITEMAFFECTION *pItem);
*
*   Input  : ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints canada all tax.
*===========================================================================
*/
VOID  PrtCanAll_RJ(ITEMAFFECTION *pItem)
{
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- print subtotal line R3.1 -- */
    if (pItem->lAmount) {
        if (pUSCanTax->lTaxAmount[0]) {

            PrtRFeed(1);
            PrtRJAmtMnem(TRN_TTL1_ADR, pItem->lAmount);
        }
    }

    /* -- GST exempt -- */
    PrtRJZeroAmtMnemP(TRN_TXEXM1_ADR, pUSCanTax->lTaxExempt[0], PRT_JOURNAL);

    /* -- PST exempt -- */
    PrtRJZeroAmtMnemP(TRN_TXEXM2_ADR, pUSCanTax->lTaxExempt[1], PRT_JOURNAL);

    /* -- CanAll taxable -- */
    PrtRJZeroAmtMnemP(TRN_TXBL1_ADR, pUSCanTax->lTaxable[0], PRT_JOURNAL);

    /* -- CanAll Tax -- */
    PrtRJZeroAmtMnem(TRN_CONSTX_ADR, pUSCanTax->lTaxAmount[0]);

    /* -- non taxable -- */
    PrtRJZeroAmtMnemP(TRN_NONTX_ADR, pUSCanTax->lTaxable[4], PRT_JOURNAL);
}

/*
*===========================================================================
** Format  : VOID  PrtCanAll_SP(TRANINFORMATION *pTran, ITEMAFFECTION *pItem);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*          : ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints canada all tax.
*===========================================================================
*/
VOID  PrtCanAll_SP(TRANINFORMATION *pTran, ITEMAFFECTION *pItem)
{
    TCHAR  aszSPPrintBuff[2][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;         /* number of lines to be printed */
    USHORT  usSaveLine;             /* save slip lines to be added */
    USHORT  i;
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    /* -- initialize the area -- */
    memset(aszSPPrintBuff, '\0', sizeof(aszSPPrintBuff));

    /* -- print subtotal line R3.1 -- */
    if (pItem->lAmount) { 
        if (pUSCanTax->lTaxAmount[0]) {
            usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[0], TRN_TTL1_ADR, 
                                                    pItem->lAmount);
        }
    }
    /* -- set Can All Tax mnemonic and amount -- */
    if (pUSCanTax->lTaxAmount[0]) {
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_CONSTX_ADR, 
                                                pUSCanTax->lTaxAmount[0]);
    }

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print the data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;
} 

/*
*===========================================================================
** Format  : VOID   PrtDflCanadaTax(TRANINFORMATION  *pTran,
*                                     ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays canada tax.
*===========================================================================
*/      
VOID   PrtDflCanadaTax(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem)
{


    if (pTran->TranCurQual.flPrintStatus & CURQUAL_CANADA_GST_PST) {
         PrtDflCanGst(pTran, pItem);

    }  else if (pTran->TranCurQual.flPrintStatus & CURQUAL_CANADA_INDI) {
         PrtDflCanInd(pTran, pItem);

    }  else {
         PrtDflCanAll(pTran, pItem);

    }
}

/*
*===========================================================================
** Format  : VOID   PrtDflCanGst(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);
*                                        
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays canada Gst Pst tax.
*===========================================================================
*/
VOID   PrtDflCanGst(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem)
{

    TCHAR  aszDflBuff[11][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo, usSav;                  /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
    usSav = usLineNo;

    /* -- set subtotal line R3.1 -- */
    if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1]) {
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TTL1_ADR, pItem->lAmount); 
    }

    /* -- set item data -- */
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXEXM1_ADR, pUSCanTax->lTaxExempt[0]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXBL1_ADR, pUSCanTax->lTaxable[0]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TX1_ADR, pUSCanTax->lTaxAmount[0]); 

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXEXM2_ADR, pUSCanTax->lTaxExempt[1]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXBL2_ADR, pUSCanTax->lTaxable[1]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TX2_ADR, pUSCanTax->lTaxAmount[1]); 

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
** Format  : VOID   PrtDflCanInd(TRANINFORMATION  *pTran,
*                                     ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays canada individual tax.
*===========================================================================
*/
VOID  PrtDflCanInd(TRANINFORMATION *pTran, ITEMAFFECTION *pItem)
{

    TCHAR  aszDflBuff[17][PRT_DFL_LINE + 1];  /* display data save area */
    USHORT  usLineNo, usSav;                  /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
    usSav = usLineNo;

    /* -- set subtotal line R3.1 -- */
    if (pUSCanTax->lTaxAmount[0]||pUSCanTax->lTaxAmount[1]||
        pUSCanTax->lTaxAmount[2]||pUSCanTax->lTaxAmount[3]) {
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

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXEXM4_ADR, pUSCanTax->lTaxExempt[3]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXBL4_ADR, pUSCanTax->lTaxable[3]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TX4_ADR, pUSCanTax->lTaxAmount[3]); 

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
** Format  : VOID   PrtDflCanAll(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);   
*            
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays canada CanAll tax.
*===========================================================================
*/
VOID  PrtDflCanAll(TRANINFORMATION *pTran, ITEMAFFECTION *pItem)
{
    TCHAR  aszDflBuff[9][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo, usSav;                  /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       
    USCANTAX    *pUSCanTax;

    /* -- set US tax structuer, V3.3 -- */
    pUSCanTax = &(pItem->USCanVAT.USCanTax);

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
    usSav = usLineNo;

    /* -- set subtotal line R3.1 -- */
    if (pUSCanTax->lTaxAmount[0]) {
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TTL1_ADR, pItem->lAmount); 
    }

    /* -- set item data -- */
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXEXM1_ADR, pUSCanTax->lTaxExempt[0]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TXBL1_ADR, pUSCanTax->lTaxable[0]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_CONSTX_ADR, pUSCanTax->lTaxAmount[0]); 
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_NONTX_ADR, pUSCanTax->lTaxable[4]); 

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

/***** End Of Source *****/
