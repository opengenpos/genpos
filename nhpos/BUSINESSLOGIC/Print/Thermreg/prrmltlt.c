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
* Program Name: PrRMlTlT.C                                                      
* --------------------------------------------------------------------------
* Abstract:
*      PrtMultiTotal() : print items specified minor class "CLASS_MULTITOTAL"
*      PrtMulTotal_TH() : print multi check (thermal)
*      PrtMulTotal_EJ() : print multi check (electric journal)
*      PrtMulTotal_SP() : print multi check (slip)
*      PrtDflMultiTotal() : display items specified minor class "CLASS_MULTITOTAL"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 00.00.01 : R.Itoh     :                                    
* Jul-14-93 : 01.00.12 : R.Itoh     : add PrtDflMultiTotal()                                   
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDflMultiTotal()                                   
* Jan-26-95 : 03.00.00 : M.Ozawa    : recover PrtDflMultiTotal()  
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
#include <dfl.h>
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
** Format  : VOID   PrtMulTotal_TH(TRANINFORMATION  *pTran, ITEMMULTI *pItem);      
*
*   Input  : TRANINFORMATION   *pTran     -Transaction Infformation address
*            ITEMMULTI         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multi check grand total. (thermal)
*===========================================================================
*/
static VOID   PrtMulTotal_TH(CONST TRANINFORMATION  *pTran, CONST ITEMMULTI *pItem)
{
    PrtTHHead(pTran->TranCurQual.usConsNo);        /* print header if necessary */

    PrtTHMulChk(pItem->usGuestNo, pItem->uchCdv);  /* checkpaid mnemo, GCF# */

    for (USHORT i = 0; i < 3; i++) {
        if ( pItem->lService[i] == 0L ) {       /* if amount "0", not print */
            continue;
        }
        PrtTHAmtMnem((i+TRN_ADCK1_ADR), pItem->lService[i] );
    }

    PrtTHAmtSym(TRN_AMTTL_ADR, pItem->lMI, PRT_DOUBLE);   
}

/*
*===========================================================================
** Format  : VOID   PrtMulTotal_EJ(ITEMMULTI *pItem);      
*
*   Input  : ITEMMULTI         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multi check grand total. (electric journal)
*===========================================================================
*/
static VOID   PrtMulTotal_EJ(CONST ITEMMULTI *pItem)
{

    PrtEJMulChk(pItem->usGuestNo, pItem->uchCdv);  /* checkpaid mnemo, GCF# */

    for (USHORT i = 0; i < 3; i++) {

        if ( pItem->lService[i] == 0L ) {       /* if amount "0", not print */
            continue;
        }

        PrtEJAmtMnem( (UCHAR)(i+TRN_ADCK1_ADR), pItem->lService[i] );
    }

    PrtEJAmtSym(TRN_AMTTL_ADR, pItem->lMI, PRT_DOUBLE);   
}

/*
*===========================================================================
** Format  : VOID   PrtMulTotal_SP(TRANINFORMATION  *pTran, ITEMMULTI *pItem);      
*
*   Input  : ITEMMULTI         *pItem     -Item Data address
*            TRANINFORMATION   *pTran     -Transaction Data address
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multi check grand total. (slip)
*===========================================================================
*/
static VOID  PrtMulTotal_SP(CONST TRANINFORMATION  *pTran, CONST ITEMMULTI *pItem)
{
    TCHAR   aszSPPrintBuff[4][PRT_SPCOLUMN + 1] = { 0 }; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    
    /* -- set add check mnemonic and amount -- */
    for (USHORT i = 0; i < TRN_ADCK3_ADR + 1 - TRN_ADCK1_ADR; i++) {

        if ( pItem->lService[i] == 0L ) {       /* if amount "0", not print */
            continue;
        }

        usSlipLine += PrtSPServTaxSum(aszSPPrintBuff[usSlipLine], 
            (UCHAR)(i+TRN_ADCK1_ADR), pItem->lService[i], PRT_SINGLE);
    }
    /* -- set grand total mnemonic and amount -- */
    usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff[usSlipLine], 
        TRN_AMTTL_ADR, pItem->lMI, PRT_DOUBLE);

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
** Format  : VOID   PrtMultiTotal(TRANINFORMATION  *pTran,ITEMMULTI *pItem)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMMULTI        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multi check grand total.
*===========================================================================
*/
VOID PrtMultiTotal(CONST TRANINFORMATION  *pTran, CONST ITEMMULTI *pItem)
{


    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtMulTotal_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        PrtMulTotal_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
        PrtMulTotal_EJ(pItem);
    }

}



/*
*===========================================================================
** Format  : VOID   PrtDflMultiTotal(TRANINFORMATION  *pTran,ITEMMULTI *pItem)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMMULTI        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays multi check grand total.
*===========================================================================
*/
VOID PrtDflMultiTotal(CONST TRANINFORMATION  *pTran, CONST ITEMMULTI *pItem)
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

    usLineNo += PrtDflAmtSym(aszDflBuff[usLineNo], TRN_AMTTL_ADR, pItem->lMI, PRT_DOUBLE);
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
