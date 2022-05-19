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
* Program Name: PrRMlTP_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*      PrtMultiPostTotal() : print items specified minor class "CLASS_MULTIPOSTTOTAL"
*      PrtMulPostTotal_RJ() : print multi check (receipt & journal)
*      PrtMulPostTotal_SP() : print multi check (slip)
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-12-92 : 00.00.01 : K.Nakajima :                                    
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
** Format  : VOID   PrtMultiPostTotal(TRANINFORMATION  *pTran,ITEMMULTI *pItem)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMMULTI        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multi check grand total in post check.
*===========================================================================
*/
VOID PrtMultiPostTotal(TRANINFORMATION  *pTran, ITEMMULTI *pItem)
{

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtMulPostTotal_SP(pTran, pItem);
    }

    if (   (fsPrtPrintPort & PRT_RECEIPT)     /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtMulPostTotal_RJ(pItem);
    }

}

/*
*===========================================================================
** Format  : VOID   PrtMulPostTotal_RJ(ITEMMULTI *pItem);      
*
*   Input  : ITEMMULTI         *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multi check grand total in post check.
*                                                       (receipt & journal)
*
*===========================================================================
*/
VOID   PrtMulPostTotal_RJ(ITEMMULTI *pItem)
{
    PrtRJMulChk(pItem->usGuestNo, pItem->uchCdv);  /* checkpaid mnemo, GCF# */

    PrtRJAmtSym(TRN_AMTTL_ADR, pItem->lMI, PRT_DOUBLE);      /* grand total */
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
** Synopsis: This function prints multi check grand total in post check.
*                                                       (slip)
*
*===========================================================================
*/
VOID  PrtMulPostTotal_SP(TRANINFORMATION  *pTran, ITEMMULTI *pItem)
{
    TCHAR   aszSPPrintBuff[PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   

    /* -- initialize the buffer -- */
    memset(aszSPPrintBuff, '\0', sizeof(aszSPPrintBuff));

    /* -- set grand total mnemonic and amount -- */
    usSlipLine += PrtSPMnemNativeAmt(aszSPPrintBuff, 
        TRN_AMTTL_ADR, pItem->lMI, PRT_DOUBLE);

    /* -- check if paper change is necessary or not -- */
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print the data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff, PRT_SPCOLUMN);       
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
}

