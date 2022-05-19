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
* Title       : Print VAT                 
* Category    : Print, NCR 2170 INTER Hospitarity Application
* Program Name: PrRVAT_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*      PrtItemAffection() : print items specified class "CLASS_ITEMAFFECTION"
*      PrtVATSubtotal     : print subtoatl of VAT
*      PrtVATServ()       : print Service total
*      PrtVATServ_TH()    : print Service total (thermal)
*      PrtVATServ_EJ()    : print Service total (thermal)
*      PrtVATServ_SP()    : print Service total (slip)
*      PrtVAT()           : print VAT
*      PrtVAT_TH()        : print VAT (thermal)
*      PrtVAT_EJ()        : print VAT (electric journal)
*      PrtVAT_SP()        : print VAT (slip)
*
*      PrtDflVATSubtotal()   : display subtotal of VAT
*      PrtDflVATServ()       : displau Service total
*      PrtDflVAT()           : display VAT
*      PrtDflVATServForm()       : displau Service total
*      PrtDflVATForm()           : display VAT
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jan-07-93 : 00.00.01 : K.Nakajima :                                    
* Aug-21-98 : 03.03.00 : M.Ozawa    : Migrated to HPUS V3.3
* Dec-15-99 : 00.00.01 : M.Ozawa    : Add PrtDflVATServForm, PrtDflVATForm
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

/**-------  M S - C  ------**/
#include	<tchar.h>
#include<string.h>

/**------- 2170 local------**/
#include<ecr.h>
#include<regstrct.h>
#include<transact.h>
#include<paraequ.h>
#include<pmg.h>
#include <prt.h>
#include <dfl.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"

/*
*===========================================================================
** Format  : VOID   PrtVATSubtotal(TRANINFORMATION  *pTran,
*                          ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints VAT Subtotal
*===========================================================================
*/
VOID PrtVATSubtotal(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem)
{
    SHORT   fsStatusSave;

    fsStatusSave = fsPrtStatus;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print */
        PrtVATSubtotal_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {           /* thermal */
        PrtVATSubtotal_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {           /* electric journal */
        PrtVATSubtotal_EJ(pTran, pItem);
    }

    /* --- set fsPrtStatus flag to control "0.00" total line print --- */
    if (pItem->lAmount) {
        fsPrtStatus |= PRT_REC_SLIP;   /* print total line */
    } else if (!(fsStatusSave & PRT_REC_SLIP)) {   /* anything is printed */
        fsPrtStatus &= ~PRT_REC_SLIP;   /* not print "0.00" total line */
    }
}

/*
*===========================================================================
** Format  : VOID    PrtVATSubtotal_TH(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);      
*
*   Input  : ITEMAFFECTION          *pItem     -Item Data address
*            TRANINFORMATION        *pTran     -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints VAT Subtotal.(thermal)
*===========================================================================
*/
VOID  PrtVATSubtotal_TH(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem)
{
/**
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_NOT_PRINT_SER ) {
        
        return;

    }
**/
    if (pItem->lAmount) {
        PrtFeed(PMG_PRT_RECEIPT, 1);                 /* 1 line feed(Receipt) */
        PrtTHHead(pTran);                            /* print header if necessary */
        PrtTHAmtMnem(TRN_TTL1_ADR, pItem->lAmount);  /* print subtotal */
    }
}
/*
*===========================================================================
** Format  : VOID    PrtVATSubtotal_EJ(ITEMAFFECTION *pItem);      
*
*   Input  : ITEMAFFECTION          *pItem     -Item Data address
*            TRANINFORMATION        *pTran     -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints VAT Subtotal.(electric journal)
*===========================================================================
*/
VOID  PrtVATSubtotal_EJ(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem)
{
    PrtEJAmtMnem(TRN_TTL1_ADR, pItem->lAmount);       /* print subtotal total */
    pTran = pTran;
}

/*
*===========================================================================
** Format  : VOID    PrtVATSubtotal_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints VAT Subtotal. (slip)
*===========================================================================
*/
VOID  PrtVATSubtotal_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem)
{
    TCHAR   aszSPPrintBuff[1][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */

    if (pItem->lAmount == 0L) {
        return ;
    }

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- set subtotal mnem. and amount.  -- */
    usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[0], TRN_TTL1_ADR, pItem->lAmount);

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[0], PRT_SPCOLUMN);       

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

}

/*
*===========================================================================
** Format  : VOID   PrtVATServ(TRANINFORMATION  *pTran,
*                          ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints VAT Service.
*===========================================================================
*/
VOID PrtVATServ(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem)
{
    SHORT   fsStatusSave;

    fsStatusSave = fsPrtStatus;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print */
        PrtVATServ_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {           /* thermal */
        PrtVATServ_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {           /* electric journal */
        PrtVATServ_EJ(pTran, pItem);
    }

    /* --- set fsPrtStatus flag to control "0.00" total line print --- */
    if (!(pTran->TranCurQual.flPrintStatus & CURQUAL_NOT_PRINT_SER) && (pItem->lAmount)) {
        fsPrtStatus |= PRT_REC_SLIP;   /* print total line */
    } else
    if (!(fsStatusSave & PRT_REC_SLIP)) {   /* anything is printed */
        fsPrtStatus &= ~PRT_REC_SLIP;   /* not print "0.00" total line */
    }
}

/*
*===========================================================================
** Format  : VOID    PrtVATServ_TH(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);      
*
*   Input  : ITEMAFFECTION          *pItem     -Item Data address
*            TRANINFORMATION        *pTran     -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints VAT Service.(thermal)
*===========================================================================
*/
VOID  PrtVATServ_TH(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem)
{
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_NOT_PRINT_SER ) {
        return;
    }

    PrtTHHead(pTran);               /* print header if necessary */

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_EXCLUDE_SERV){
        PrtTHAmtMnem(TRN_TX4_ADR, pItem->lAmount);              /* print service total */
    } else {
        PrtTHMAmtShift(TRN_TX4_ADR, pItem->lAmount, PRT_VATCOLUMN);
    }
}
/*
*===========================================================================
** Format  : VOID    PrtVATServ_EJ(ITEMAFFECTION *pItem);      
*
*   Input  : ITEMAFFECTION          *pItem     -Item Data address
*            TRANINFORMATION        *pTran     -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints VAT Service.(electric journal)
*===========================================================================
*/
VOID  PrtVATServ_EJ(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem)
{
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_EXCLUDE_SERV){
        PrtEJZeroAmtMnem(TRN_TX4_ADR, pItem->lAmount);              /* print service total */
    } else {
        PrtEJZAMnemShift(TRN_TX4_ADR, pItem->lAmount, PRT_VATCOLUMN);
    }
}

/*
*===========================================================================
** Format  : VOID    PrtVATServ_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints VAT Service. (slip)
*===========================================================================
*/
VOID  PrtVATServ_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem)
{

    TCHAR   aszSPPrintBuff[1][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */

    if (pItem->lAmount == 0L) {
        return ;
    }

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_NOT_PRINT_SER) {
        return ;
    }

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- set service mnem. and amount.  -- */
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_EXCLUDE_SERV ) {
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[0], TRN_TX4_ADR, pItem->lAmount);
    } else {
        usSlipLine += PrtSPMAmtShift(aszSPPrintBuff[0], TRN_TX4_ADR, pItem->lAmount, PRT_VATCOLUMN);
    }

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[0], PRT_SPCOLUMN);       

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

}

/*
*===========================================================================
** Format  : VOID   PrtVAT(TRANINFORMATION  *pTran,
*                          ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints VAT.
*===========================================================================
*/
VOID PrtVAT(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem)
{

    SHORT   fsStatusSave;

    fsStatusSave = fsPrtStatus;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print */
        PrtVAT_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {           /* thermal */
        PrtVAT_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {           /* electric journal */
        PrtVAT_EJ(pTran, pItem);
    }

    /* --- set fsPrtStatus flag to control "0.00" total line print --- */
    if (!(pTran->TranCurQual.flPrintStatus & CURQUAL_NOT_PRINT_VAT) &&
        !(pTran->TranCurQual.flPrintStatus & CURQUAL_NOT_PRINT_VATAPP) &&
         ((pItem->USCanVAT.ItemVAT[0].lVATRate) ||
          (pItem->USCanVAT.ItemVAT[1].lVATRate) ||
          (pItem->USCanVAT.ItemVAT[2].lVATRate))) {

        fsPrtStatus |= PRT_REC_SLIP;   /* print total line */
    } else if (!(fsStatusSave & PRT_REC_SLIP)) {   /* anything is printed */
        fsPrtStatus &= ~PRT_REC_SLIP;   /* not print "0.00" total line */
    }
}

/*
*===========================================================================
** Format  : VOID    PrtVAT_TH(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints VAT .(thermal)
*===========================================================================
*/
VOID  PrtVAT_TH(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem)
{
    USHORT  i, j;   

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_NOT_PRINT_VAT) {
        return;                                 /* not print VAT */
    }

    PrtTHHead(pTran);                           /* print header if necessary */

    /* -- if not print vat applicable -- */
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_NOT_PRINT_VATAPP) {
        /* -- excluded vat -- */
        if (pTran->TranCurQual.flPrintStatus & CURQUAL_EXCLUDE_VAT) {
            for (i = 0, j = 0; i < NUM_VAT; i++, j+=3) {
                if (pItem->USCanVAT.ItemVAT[i].lVATAmt == 0L) {
                    continue;
                }
                PrtTHTaxRateAmt((TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATRate, pItem->USCanVAT.ItemVAT[i].lVATAmt, 0);
            }
        /* -- included vat -- */
        } else {
            for (i = 0, j = 0; i < NUM_VAT; i++, j+=3) {
                if (pItem->USCanVAT.ItemVAT[i].lVATAmt == 0L) {
                    continue;
                }
                PrtTHTaxRateAmt((TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATRate, pItem->USCanVAT.ItemVAT[i].lVATAmt, PRT_VATCOLUMN);
            }
        }
        return ;
    }

    /* -- if print vat applicable -- */
    PrtTHVatMnem(pItem);    
    for (i = 0; i < NUM_VAT; i++) {
        PrtTHVatAmt(pItem->USCanVAT.ItemVAT[i]);
    }
}
/*
*===========================================================================
** Format  : VOID    PrtVAT_EJ(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);      
*
*   Input  : TRANINFORMATION        *pTran      -transaction information
*            ITEMAFFECTION          *pItem      -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints VAT .(electric journal)
*===========================================================================
*/
VOID  PrtVAT_EJ(TRANINFORMATION *pTran, ITEMAFFECTION *pItem)
{
    USHORT i,j;

    for (i=j=0; i < NUM_VAT; i++, j+=3) {
        /* -- vatable  -- */
        PrtEJZAMnemShift((TRN_TXBL1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATable, PRT_VATCOLUMN);

        if (pItem->USCanVAT.ItemVAT[i].lVATRate == 0) {
            continue;
        }

        if (pTran->TranCurQual.flPrintStatus & CURQUAL_EXCLUDE_VAT) {
            /* -- execluded vat -- */
            PrtEJZeroAmtMnem((TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATAmt);
        } else {
            /* -- included vat -- */
            PrtEJZAMnemShift((TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATAmt, PRT_VATCOLUMN);
        }
    }
}

/*
*===========================================================================
** Format  : VOID    PrtVAT_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints VAT. (slip)
*===========================================================================
*/
VOID  PrtVAT_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem)
{
    TCHAR   aszSPPrintBuff[4][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i, j;

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_NOT_PRINT_VAT) {
        return ;
    }

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    if (pTran->TranCurQual.flPrintStatus & CURQUAL_NOT_PRINT_VATAPP) {
        /* -- excluded vat -- */
        if (pTran->TranCurQual.flPrintStatus & CURQUAL_EXCLUDE_VAT) {
            for (i = 0, j = 0; i < NUM_VAT; i++, j+=3) {

                if (pItem->USCanVAT.ItemVAT[i].lVATAmt == 0L) {
                    continue;
                }
                usSlipLine += PrtSPTaxRateAmt(aszSPPrintBuff[usSlipLine], (TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATRate, pItem->USCanVAT.ItemVAT[i].lVATAmt, 0);
            }
        /* -- included vat -- */
        } else {
            for (i = 0, j = 0; i < NUM_VAT; i++, j+=3) {
                if (pItem->USCanVAT.ItemVAT[i].lVATAmt == 0L) {
                    continue;
                }
                usSlipLine += PrtSPTaxRateAmt(aszSPPrintBuff[usSlipLine], (TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATRate, pItem->USCanVAT.ItemVAT[i].lVATAmt, PRT_VATCOLUMN);
            }
        }
    } else {
        /* -- set VATmnem.  -- */
        usSlipLine += PrtSPTaxMnem(aszSPPrintBuff[0], pItem);

        /* -- set VAT amount -- */
        for (i = 0; i < NUM_VAT; i++) {
            usSlipLine += PrtSPTaxAmt(aszSPPrintBuff[usSlipLine],pItem->USCanVAT.ItemVAT[i]);
        }
    }

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
}

/*
*===========================================================================
** Format  : VOID   PrtDflVATSubtotal(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);   
*            
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT Subtotal.
*            
*===========================================================================
*/
VOID  PrtDflVATSubtotal(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem)
{
    TCHAR  aszDflBuff[7][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo, usSav;                   /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
    usSav = usLineNo;

    /* -- set item data -- */
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TTL1_ADR, pItem->lAmount);    /* print subtotal total */

    /* -- not send if no tax data -- */ 
    if ( usLineNo == usSav ) {
        return;
    }

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_TOTAL); 
/*    PrtDflIType(usLineNo, DFL_TAX);  */

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
** Format  : VOID   PrtDflVATServ(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);   
*            
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT Service.
*            
*===========================================================================
*/
VOID  PrtDflVATServ(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem)
{
    TCHAR  aszDflBuff[7][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo, usSav;                   /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
    usSav = usLineNo;

    /* -- set item data -- */
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_EXCLUDE_SERV){
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TX4_ADR, pItem->lAmount);    /* print service total */
    } else {
        usLineNo += PrtDflZAMnemShift(aszDflBuff[usLineNo], TRN_TX4_ADR, pItem->lAmount, PRT_VATCOLUMN);
    }

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
** Format  : VOID   PrtDflVAT(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);   
*            
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT .
*            
*===========================================================================
*/
VOID  PrtDflVAT(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem)
{
    TCHAR  aszDflBuff[11][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo, usSav;                   /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i,j;

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
    usSav = usLineNo;

    /* -- set item data -- */
    for (i=j=0; i < NUM_VAT; i++, j+=3) {
        /* -- vatable  -- */
        usLineNo += PrtDflZAMnemShift(aszDflBuff[usLineNo], (TRN_TXBL1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATable, PRT_VATCOLUMN);

        if (pItem->USCanVAT.ItemVAT[i].lVATRate == 0) {
            continue;
        }

        if (pTran->TranCurQual.flPrintStatus & CURQUAL_EXCLUDE_VAT) {
            /* -- execluded vat -- */
            usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], (TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATAmt);
        } else {
            /* -- included vat -- */
            usLineNo += PrtDflZAMnemShift(aszDflBuff[usLineNo], (TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATAmt, PRT_VATCOLUMN);
        }
    }

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
** Format  : VOID   PrtDflVATServ(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);   
*            
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT Service.
*            
*===========================================================================
*/
USHORT  PrtDflVATServForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer)
{
    TCHAR  aszDflBuff[7][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo=0, i;                   /* number of lines to be displayed */

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    if (pTran->TranCurQual.flPrintStatus & CURQUAL_EXCLUDE_SERV){
        usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_TX4_ADR, pItem->lAmount);    /* print service total */
    } else {
        usLineNo += PrtDflZAMnemShift(aszDflBuff[usLineNo], TRN_TX4_ADR, pItem->lAmount, PRT_VATCOLUMN);
    }
    
    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/*
*===========================================================================
** Format  : VOID   PrtDflVAT(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem);   
*            
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays VAT .
*            
*===========================================================================
*/
USHORT  PrtDflVATForm(TRANINFORMATION  *pTran, ITEMAFFECTION   *pItem, TCHAR *puchBuffer)
{
    TCHAR  aszDflBuff[11][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo=0;                   /* number of lines to be displayed */
    USHORT  i,j;

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

#endif
    /* -- set item data -- */
    i = pItem->sItemCounter;
    j = i*3;
    /* -- vatable  -- */
    usLineNo += PrtDflZAMnemShift(aszDflBuff[usLineNo], (TRN_TXBL1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATable, PRT_VATCOLUMN);

    if (pItem->USCanVAT.ItemVAT[i].lVATRate) {
        if (pTran->TranCurQual.flPrintStatus & CURQUAL_EXCLUDE_VAT) {
            /* -- execluded vat -- */
            usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], (TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATAmt);
        } else {
            /* -- included vat -- */
            usLineNo += PrtDflZAMnemShift(aszDflBuff[usLineNo], (TRN_TX1_ADR + j), pItem->USCanVAT.ItemVAT[i].lVATAmt, PRT_VATCOLUMN);
        }
    }
    
    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/***** End of File ***/
