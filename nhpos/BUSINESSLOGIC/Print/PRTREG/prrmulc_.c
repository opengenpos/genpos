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
* Title       : Print  MultiCheck                
* Category    : Print, NCR 2170  US Hospitarity Application
* Program Name: PrRMulC_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*      PrtMulti() : print items specified minor class "CLASS_MULTICHECKPAYMENT"
*      PrtMulChk_RJ() : print multi check (receipt & journal)
*      PrtMulChk_SP() : print multi check (slip)
*      PrtDflMulti() : display items specified minor class "CLASS_MULTICHECKPAYMENT"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-12-92 : 00.00.01 : K.Nakajima :                                    
* Jul-14-93 : 01.00.12 : R.Itoh     : add PrtDflMulti()                                   
* Oct-28-93 : 02.00.02 : K.You      : del. PrtDflMulti()                                   
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDflMulti() for display on the fly
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
#include <dfl.h>
#include <pmg.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"

/**
;========================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID   PrtMulti(TRANINFORMATION  *pTran,ITEMMULTI *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMMULTI        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multi check.
*===========================================================================
*/
VOID PrtMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem)
{


    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtMulChk_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)     /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtMulChk_RJ(pItem);
    }

}

/*
*===========================================================================
** Format  : VOID   PrtMulChk_RJ(ITEMMULTI *pItem);      
*
*   Input  : ITEMMULTI         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multi check. (receipt & journal)
*===========================================================================
*/
VOID   PrtMulChk_RJ(ITEMMULTI *pItem)
{
    UCHAR i;

    PrtRJMulChk(pItem->usGuestNo, pItem->uchCdv);  /* checkpaid mnemo, GCF# */

    for (i = 0; i < 3; i++) {

        if ( pItem->lService[i] == 0L ) {       /* if amount "0", not print */
            continue;
        }
        PrtRJAmtMnem( (UCHAR)(i+TRN_ADCK1_ADR), pItem->lService[i] );
    }

    PrtRJAmtSym(TRN_TTL2_ADR, pItem->lMI, PRT_DOUBLE);       /* check total */
}

/*
*===========================================================================
** Format  : VOID   PrtMulChk_SP(TRANINFORMATION  *pTran, ITEMMULTI *pItem);      
*
*   Input  : ITEMMULTI         *pItem     -Item Data address
*            TRANINFORMATION   *pTran     -Transaction Data address
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multi check. (slip)
*===========================================================================
*/
VOID  PrtMulChk_SP(TRANINFORMATION  *pTran, ITEMMULTI *pItem)
{
    TCHAR   aszSPPrintBuff[5][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   

    /* -- initialize the buffer -- */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));
    
    /* -- set check paid mnemonic and guest check No. -- */
    usSlipLine += PrtSPGstChkNo(aszSPPrintBuff[0], pItem);

    /* -- set add check mnemonic and amount -- */
    for (i = 0; i < TRN_ADCK3_ADR + 1 - TRN_ADCK1_ADR; i++) {
        if ( pItem->lService[i] == 0L ) {       /* if amount "0", not print */
            continue;
        }
        usSlipLine += PrtSPServTaxSum(aszSPPrintBuff[usSlipLine], 
            (UCHAR)(i+TRN_ADCK1_ADR), pItem->lService[i], PRT_SINGLE);
    }
    /* -- set check total mnemonic and amount -- */
    usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], 
        TRN_TTL2_ADR, pItem->lMI, PRT_DOUBLE);

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
** Format  : VOID   PrtDflMulti(TRANINFORMATION  *pTran,ITEMMULTI *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMMULTI        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays multi check.
*===========================================================================
*/
VOID PrtDflMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem)
{
    TCHAR  aszDflBuff[9][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflMulChk(aszDflBuff[usLineNo], pItem->usGuestNo, pItem->uchCdv);

    for (i = 0; i < 3; i++) {
        if ( pItem->lService[i] == 0L ) {       /* if amount "0", not print */
            continue;
        }
        usLineNo += PrtDflAmtMnem( aszDflBuff[usLineNo], (i+TRN_ADCK1_ADR), pItem->lService[i] );
    }

    usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_TTL2_ADR, pItem->lMI, PRT_DOUBLE);
    /* -- set destination CRT -- */
    PrtDflSetTtlNo(pTran);

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_TOTAL); 

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

