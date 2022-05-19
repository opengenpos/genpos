/*
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
* Title       : Print  Foreign Tender key                
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRFTnd_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*  PrtForeignTender() : print foreign tender operation 
*  PrtForeignTender_RJ() : print foreign tender operation (receipt & journal)
*  PrtForeignTender_SP() : print foreign tender operation (slip)
*  PrtDflForeignTender() : display foreign tender operation 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-04-92 : 00.00.01 : K.Nakajima :                                    
* Jul-13-93 : 01.00.12 : R.Itoh     : add PrtDflForeignTender()                                   
* Oct-28-93 : 02.00.02 : K.You      : del. PrtDflForeignTender()                                   
* Apr-08-94 : 00.00.04 : K.You      : add validation slip print feature.(mod. PrtForeignTender())
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDflForeignTender() for display on the fly
* Jul-21-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Dec-06-99 : 01.00.00 : hrkato     : Saratoga
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>
/**------- 2170 local------**/
#include <ecr.h>
#include <rfl.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <pmg.h>
#include <dfl.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"


/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtForeignTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints foreign tender operation
*===========================================================================
*/
VOID PrtForeignTender(TRANINFORMATION  *pTran, ITEMTENDER  *pItem)
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
            PrtForeignTender_SP(pTran, pItem);
            PrtSPVLTrail(pTran);
        } else {
            PrtForeignTender_VL(pTran, pItem);
        }
        return ;
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtForeignTender_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)     /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtForeignTender_RJ(pItem);
    }

}


/*
*===========================================================================
** Format  : VOID  PrtForeignTender_RJ(ITEMTENDER *pItem);      
*
*   Input  : ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function prints foreign tender operation
*===========================================================================
*/
VOID PrtForeignTender_RJ(ITEMTENDER  *pItem)
{
    UCHAR  uchAdr1;
	USHORT usTranAdr2;

	PrtGetMoneyMnem (pItem->uchMinorClass, &usTranAdr2, &uchAdr1);

    PrtRJVoid(pItem->fbModifier);               /* void line */

    PrtRJNumber(pItem->aszNumber);              /* number line */

    PrtRJForeign1(pItem->lForeignAmount, uchAdr1, pItem->auchTendStatus[0]);
    
    PrtRJForeign2(pItem->ulFCRate, pItem->auchTendStatus[1]);             /* foreign tender */

    PrtRJAmtMnem(usTranAdr2, pItem->lTenderAmount);/* normal tender */

    PrtRJZeroAmtMnem(TRN_REGCHG_ADR, pItem->lChange);
                                                /* change */
} 

/*
*===========================================================================
** Format  : VOID  PrtForeignTender_VL(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints foreign tender operation
*===========================================================================
*/
VOID  PrtForeignTender_VL(TRANINFORMATION  *pTran, ITEMTENDER *pItem)
{

    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */


    /* -- in case of 24 char printer -- */

    if ( usPrtVLColumn == PRT_VL_24 ) {

        PrtVLHead(pTran, pItem->fbModifier);    /* send header lines */

        PrtVLTender1(pItem->aszNumber, pItem->lRound);
                                                /* tender 1st line */
            
        PrtVLForeignTend(pItem);                /* tender 2nd line */

        PrtVLTrail(pTran);                      /* send trailer lines */

    }

    /* -- in case of 21 char printer -- */

    if ( usPrtVLColumn == PRT_VL_21 ) {

        PrtVSFCTotal(pItem, pTran);

    }

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */

} 

/*
*===========================================================================
** Format  : VOID  PrtForeignTender_SP(TRANINFORMATION *pTran, 
*                                                   ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints foreign tender operation
*===========================================================================
*/
VOID PrtForeignTender_SP(TRANINFORMATION  *pTran, ITEMTENDER *pItem) 
{
    TCHAR  aszSPPrintBuff[4][PRT_SPCOLUMN + 1]; /* print data save area */
    UCHAR  uchAdr1;            /* set foreign symbol and mnem. */
	USHORT usTranAdr2;
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   

	PrtGetMoneyMnem (pItem->uchMinorClass, &usTranAdr2, &uchAdr1);

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- set void mnemonic and number -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
    /* -- set foreign amount and rate -- */
    usSlipLine += PrtSpForeign(aszSPPrintBuff[usSlipLine], pItem->lForeignAmount, uchAdr1, pItem->auchTendStatus[0], pItem->ulFCRate, pItem->auchTendStatus[1]);
                            
    /* -- set foreign tender mnemonic and amount -- */
    usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], usTranAdr2, pItem->lTenderAmount);

    /* -- set change mnemonic and amount -- */
    if (pItem->lChange) { 
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_REGCHG_ADR, pItem->lChange);
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
** Format  : VOID  PrtDflForeignTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints foreign tender operation
*===========================================================================
*/
VOID PrtDflForeignTender(TRANINFORMATION  *pTran, ITEMTENDER  *pItem)
{
    TCHAR  aszDflBuff[11][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       
    USHORT usTranAdr2;
    UCHAR  uchAdr1;

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

	PrtGetMoneyMnem (pItem->uchMinorClass, &usTranAdr2, &uchAdr1);

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);   

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflForeign1(aszDflBuff[usLineNo],pItem->lForeignAmount, uchAdr1, pItem->auchTendStatus[0]); 

    usLineNo += PrtDflForeign2(aszDflBuff[usLineNo], pItem->ulFCRate, pItem->auchTendStatus[1]); 

    usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], TRN_FT_EQUIVALENT, pItem->lTenderAmount);

    /* display balance due data R3.1 */
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_CURB_ADR, pItem->lBalanceDue);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_REGCHG_ADR, pItem->lChange);

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- check void status -- */
    PrtDflCheckVoid(pItem->fbModifier);

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_TENDER); 

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
** Format  : VOID  PrtEuroTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints Euro tender operation, V3.4
*===========================================================================
*/
VOID PrtEuroTender(TRANINFORMATION  *pTran, ITEMTENDER  *pItem)
{

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtEuroTender_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)     /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtEuroTender_RJ(pItem);
    }

}

/*
*===========================================================================
** Format  : VOID  PrtEuroTender_RJ(ITEMTENDER *pItem);      
*
*   Input  : ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function prints Euro tender operation, V3.4
*===========================================================================
*/
VOID PrtEuroTender_RJ(ITEMTENDER  *pItem)
{
    UCHAR  uchAdr1, uchAdr2, uchAdr3;

    uchAdr1 = SPC_CNSYMNTV_ADR;
    uchAdr2 = SPC_CNSYMFC1_ADR;
/****
    if (CliParaMDCCheck(MDC_EURO_ADR, ODD_MDC_BIT2)) {

        / after transition to Euro /
        uchAdr1 = SPC_CNSYMFC1_ADR;
        uchAdr2 = SPC_CNSYMNTV_ADR;

    } else {

        uchAdr1 = SPC_CNSYMNTV_ADR;
        uchAdr2 = SPC_CNSYMFC1_ADR;
    }
****/
    uchAdr3 = SPC_CNSYMFC1_ADR;

    PrtRFeed(1);                      /* 1 line feed (Receipt) */

    PrtRJEuro(uchAdr1, 1L, uchAdr2, pItem->ulFCRate, pItem->auchTendStatus[1]);

    PrtRJAmtSymEuro(TRN_TTL2_ADR, uchAdr3, pItem->lForeignAmount, pItem->auchTendStatus[0]);
} 

/*
*===========================================================================
** Format  : VOID  PrtEuroTender_SP(TRANINFORMATION *pTran, 
*                                                   ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints Euro tender operation, V3.4
*===========================================================================
*/
VOID PrtEuroTender_SP(TRANINFORMATION  *pTran, ITEMTENDER *pItem) 
{
    TCHAR  aszSPPrintBuff[4][PRT_SPCOLUMN + 1]; /* print data save area */
    UCHAR  uchAdr1, uchAdr2, uchAdr3;           /* set foreign symbol and mnem. */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   

    /* get foreign 1 or 2 */
    uchAdr1 = SPC_CNSYMNTV_ADR;
    uchAdr2 = SPC_CNSYMFC1_ADR;
/****
    if (CliParaMDCCheck(MDC_EURO_ADR, ODD_MDC_BIT2)) {

        / after transition to Euro /
        uchAdr1 = SPC_CNSYMFC1_ADR;
        uchAdr2 = SPC_CNSYMNTV_ADR;

    } else {

        uchAdr1 = SPC_CNSYMNTV_ADR;
        uchAdr2 = SPC_CNSYMFC1_ADR;
    }
****/
    uchAdr3 = SPC_CNSYMFC1_ADR;

    /* initialize the buffer */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- set foreign amount and rate -- */
    usSlipLine += PrtSPEuro(aszSPPrintBuff[usSlipLine], uchAdr1, 1L, uchAdr2, pItem->ulFCRate, pItem->auchTendStatus[1]);
                            
    usSlipLine += PrtSPAmtSymEuro(aszSPPrintBuff[usSlipLine], TRN_TTL2_ADR, uchAdr3, pItem->lForeignAmount, pItem->auchTendStatus[0]);

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
** Format  : VOID  PrtDflForeignTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints foreign tender operation
*===========================================================================
*/
USHORT PrtDflForeignTenderForm(TRANINFORMATION  *pTran, ITEMTENDER  *pItem, TCHAR *puchBuffer)
{
    TCHAR  aszDflBuff[11][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    UCHAR  uchAdr1;
	USHORT usTranAdr2;

    /* --- if this frame is 1st frame, display customer name --- */

    PrtDflCustHeader( pTran );

	PrtGetMoneyMnem (pItem->uchMinorClass, &usTranAdr2, &uchAdr1);

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);   

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflForeign1(aszDflBuff[usLineNo],pItem->lForeignAmount, uchAdr1, pItem->auchTendStatus[0]); 

    usLineNo += PrtDflForeign2(aszDflBuff[usLineNo], pItem->ulFCRate, pItem->auchTendStatus[1]); 

    usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], TRN_FT_EQUIVALENT, pItem->lTenderAmount);

    /* display balance due data R3.1 */
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_CURB_ADR, pItem->lBalanceDue);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_REGCHG_ADR, pItem->lChange);

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;

}


/***** End Of Source *****/