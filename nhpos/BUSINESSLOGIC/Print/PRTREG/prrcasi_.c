/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  Cashier In data                  
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRCasI_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtCasIn() : print in cashier in operation
*               PrtCasIn_RJ() : print in cashier in operation (receipt & journal)
*               PrtCasIn_SP() : print in cashier in operation (slip)
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-26-92 : 00.00.01 : K.Nakajima :
* May-12-95 : 03.00.00 : T.Nakahata : add unique transaction no. feature
* Jul-20-95 : 03.00.04 : T.Nakahata : FVT Comment (Trans# Mnemo 4 => 12Char)
*           :          :            : add customer name (16 chara)
* Jun-09-98 : 03.03.00 : M.Ozawa    : add PB line for cashier interupt recall
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
#include "prtrin.h"
#include "prrcolm_.h"
#include "para.h"
#include "csstbpar.h"


/*
*===========================================================================
** Format  : VOID  PrtCasIn(TRANINFORMATION  *pTran,
*                             ITEMTRANSOPEN *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in new check operation.
*===========================================================================
*/
VOID   PrtCasIn(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem)
{
    SHORT   fsPortSave;
    
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtCasIn_SP(pTran, pItem);
    }

    if ((fsPrtPrintPort & PRT_RECEIPT)  /* receipt, journal print */
        || (fsPrtPrintPort & PRT_JOURNAL)) {

        PrtCasIn_RJ(pTran, pItem);
    }

    if (( ! ( fsPrtPrintPort & PRT_JOURNAL )) &&
        ( fsPrtStatus & PRT_SERV_JOURNAL )) {

        fsPortSave = fsPrtPrintPort;
        fsPrtPrintPort = PRT_JOURNAL;

        /* --- print cashier interrupt trans on journal, R3.3 --- */

        PrtCasIn_RJ(pTran, pItem);

        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID  PrtCasIn_RJ(TRANINFORMATION  *pTran,
*                             ITEMTRANSOPEN *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in cashier in operation (receipt & journal).
*===========================================================================
*/
VOID PrtCasIn_RJ( TRANINFORMATION *pTran, ITEMTRANSOPEN  *pItem )
{
    SHORT   fsPortSave;

    PrtRJCustomerName( pTran->TranGCFQual.aszName );

    /* --- determine system uses unique transaction no. --- */

    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {

        /* --- print unique trans no. and no. of person --- */

        PrtRJTblPerson( pTran->TranGCFQual.usTableNo,
                        pTran->TranGCFQual.usNoPerson,
                        0,
                        PRT_DOUBLE );

    } else {            /* unique transaction no. is NOT selected */

        if ( (pTran->TranGCFQual.usNoPerson != 0) 
			&& (!CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT3))) {

            /* --- print no. of person --- */

            PrtRJTranNum( TRN_PSN_ADR,
                          ( ULONG )( pTran->TranGCFQual.usNoPerson ));
        }
    }

    /* -- print guest check number for cashier interrupt on journal, R3.3 -- */

    if (fsPrtPrintPort & PRT_JOURNAL) {

        fsPortSave = fsPrtPrintPort;
        fsPrtPrintPort = PRT_JOURNAL;

        PrtRJGuest( pTran->TranGCFQual.usGuestNo, pTran->TranGCFQual.uchCdv );

        fsPrtPrintPort = fsPortSave;
    }

    /* print P.B. of cashier interurpt at precheck, post check system, R3.3 */
    if ((pItem->uchMinorClass == CLASS_CASINTRECALL) &&
        (pItem->lAmount)) {

        PrtRJTaxMod( pTran->TranModeQual.fsModeStatus, pItem->fbModifier );

        PrtRJAmtMnem( TRN_PB_ADR, pItem->lAmount );
    }

}

/*
*===========================================================================
** Format  : VOID  PrtCasIn_SP(TRANINFORMATION  *pTran,
*                             ITEMTRANSOPEN *pItem);   
*                                 
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in cashier in operation (slip).
*===========================================================================
*/
VOID PrtCasIn_SP( TRANINFORMATION *pTran, ITEMTRANSOPEN  *pItem)
{
    TCHAR  aszSPPrintBuff[ 2 ][ PRT_SPCOLUMN + 1 ];   /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  usTableNo;
    USHORT  i;
    SHORT   sWidthType;

    /* --- determine system uses unique transaction no. --- */

    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        usTableNo  = pTran->TranGCFQual.usTableNo;
        sWidthType = PRT_DOUBLE;
    } else {
        usTableNo  = 0;
        sWidthType = PRT_SINGLE;
    }

    /* -- initialize the area -- */
    memset(aszSPPrintBuff, '\0', sizeof(aszSPPrintBuff));

    /* -- set table No. and No. of person -- */
    usSlipLine += PrtSPCustomerName( aszSPPrintBuff[ 0 ],
                                     pTran->TranGCFQual.aszName );
    usSlipLine += PrtSPTblPerson(aszSPPrintBuff[ usSlipLine ],
                                 usTableNo,
                                 pTran->TranGCFQual.usNoPerson,
                                 sWidthType );

    /* print P.B. of cashier interurpt at precheck, post check system, R3.3 */
    if ((pItem->uchMinorClass == CLASS_CASINTRECALL) &&
        (pItem->lAmount)) {

        usSlipLine += PrtSPMnemTaxAmt( aszSPPrintBuff[ usSlipLine ],
                                       TRN_PB_ADR,
                                       pTran->TranModeQual.fsModeStatus,
                                       pItem->fbModifier, pItem->lAmount );
    }

    /* -- check if paper change is necessary or not -- */ 

    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* if just after header printed, not print table No., No. of person */  
    if ( usSaveLine != 0 ) {

        if ( pTran->TranGCFQual.usNoPerson || pTran->TranGCFQual.usTableNo ) {

            PmgPrint( PMG_PRT_SLIP, aszSPPrintBuff[ 1 ], PRT_SPCOLUMN );

        } else {

            PmgPrint( PMG_PRT_SLIP, aszSPPrintBuff[ 0 ], PRT_SPCOLUMN );
        }

    } else {

        /* -- print all data in the buffer -- */ 

        for ( i = 0; i < usSlipLine; i++ ) {

            PmgPrint( PMG_PRT_SLIP, aszSPPrintBuff[ i ], PRT_SPCOLUMN );
        }
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;    

}

/* ===== End of File ( PrRCasI_.C ) ===== */
