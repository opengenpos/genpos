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
* Title       : Print  addcheck                   
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRAdd2_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtAddChk2() : print addcheck
*               PrtAddChk2_RJ() : prints addcheck (receipt & journal)
*               PrtAddChk2_SP() : prints addcheck (slip)
*               PrtDflAddChk2() : display addcheck
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-04-92 : 00.00.01 : K.Nakajima :                                    
* Jul-14-93 : 01.00.12 : R.Itoh     : add PrtDflAddChk2()                                   
* Oct-28-93 : 02.00.02 : K.You      : del. PrtDflAddChk2()                                   
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDflAddChk2() for display on the fly
* Jul-21-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
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
#include<ecr.h>
#include<paraequ.h>
/* #include<para.h> */
#include<regstrct.h>
#include<transact.h>
#include<pmg.h>
#include<dfl.h>
#include<prt.h>
#include"prtrin.h"
#include"prtdfl.h"
#include"prrcolm_.h"

/*
*===========================================================================
** Format  : VOID  PrtAddChk2(TRANINFORMATION  *pTran,
*                             ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints addcheck.
*===========================================================================
*/
VOID   PrtAddChk2(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem)
{

    if ( (pTran->TranCurQual.flPrintStatus & (CURQUAL_POSTCHECK | CURQUAL_STORE_RECALL) )
        && (pTran->TranCurQual.fsCurStatus & CURQUAL_CANCEL) ) {

        return;     
    }
    if (pTran->TranModeQual.fsModeStatus & MODEQUAL_CASINT) {

        /* not print add check line at cashier interrupt, R3.3 */
        return;
    }

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtAddChk2_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)  /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtAddChk2_RJ(pTran, pItem);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtAddChk2_RJ(TRANINFORMATION  *pTran,
*                                ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints addcheck line.
*===========================================================================
*/
VOID  PrtAddChk2_RJ(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem)
{

    if ( pItem->lAmount == 0L ) {               /* if amount "0", not print */
        return;
    }

    PrtRJTaxMod(pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    PrtRJAmtMnem((UCHAR)(pItem->uchAddNum + TRN_ADCK1_ADR), pItem->lAmount); 

}

/*
*===========================================================================
** Format  : VOID  PrtAddChk2_SP(TRANINFORMATION  *pTran,
*                                ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints addcheck line.
*===========================================================================
*/
VOID  PrtAddChk2_SP(TRANINFORMATION  *pTran, ITEMAFFECTION *pItem)
{
    TCHAR  aszSPPrintBuff[PRT_SPCOLUMN + 1];    /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */

    if ( pItem->lAmount == 0L ) {               /* if amount "0", not print */
        return;
    }

    memset(aszSPPrintBuff, '\0', sizeof(aszSPPrintBuff));
                                                 /* initialize the area */

    /* set add check, tax# 1-3 mnemonic and amount */

    usSlipLine += PrtSPMnemTaxAmt(aszSPPrintBuff, (UCHAR)(pItem->uchAddNum + TRN_ADCK1_ADR), 
        pTran->TranModeQual.fsModeStatus, pItem->fbModifier, pItem->lAmount);

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print the data in the buffer -- */ 
    if (usSlipLine != '\0') {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff, PRT_SPCOLUMN);       
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;    
}

/*
*===========================================================================
** Format  : VOID  PrtDflAddChk2(TRANINFORMATION  *pTran,
*                                            ITEMAFFECTION *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMAFFECTION    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays addcheck.
*===========================================================================
*/
VOID   PrtDflAddChk2(TRANINFORMATION  *pTran, ITEMAFFECTION  *pItem)
{
    TCHAR  aszDflBuff[5][PRT_DFL_LINE + 1];     /* display data save area */
    USHORT  usLineNo;                           /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;


    if ( pItem->lAmount == 0L ) {               /* if amount "0", not print */
        return;
    }

    if ( (pTran->TranCurQual.flPrintStatus & (CURQUAL_POSTCHECK | CURQUAL_STORE_RECALL) )
        && (pTran->TranCurQual.fsCurStatus & CURQUAL_CANCEL) ) {

        return;     
    }

    /* --- if this frame is 1st frame, display customer name --- */

    PrtDflCustHeader( pTran );

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflTaxMod(aszDflBuff[usLineNo], pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], (pItem->uchAddNum + TRN_ADCK1_ADR), pItem->lAmount);

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_TRANOPEN); 

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
