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
* Program Name: PrRCasIT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtCasIn() : print in cashier in operation
*               PrtCasIn_TH() : print in cashier in operation (thermal)
*               PrtCasIn_EJ() : print in cashier in operation (electric journal)
*               PrtCasIn_SP() : print in cashier in operation (slip)
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 00.00.01 :  R.Itoh    :                                    
* May-12-95 : 30.00.00 : T.Nakahata : add unique transaction number.
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
#include <para.h>
#include <pmg.h>
#include <regstrct.h>
#include <transact.h>
#include "prtrin.h"
#include "prrcolm_.h"
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
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
        PrtCasIn_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT && !(pTran->TranCurQual.flPrintStatus & CURQUAL_EPAY_BALANCE)) {  /* thermal print */
        PrtCasIn_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL && !(pTran->TranCurQual.flPrintStatus & CURQUAL_EPAY_BALANCE)) {  /* electric journal */
        PrtCasIn_EJ(pTran, pItem);
    } else if ( fsPrtStatus & PRT_SERV_JOURNAL ) {
		SHORT   fsPortSave = fsPrtPrintPort;

        fsPrtPrintPort = PRT_JOURNAL;
        PrtCasIn_EJ(pTran, pItem);
        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID  PrtCasIn_TH(TRANINFORMATION  *pTran,
*                             ITEMTRANSOPEN *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in cashier in operation (thermal).
*===========================================================================
*/
VOID  PrtCasIn_TH(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem)
{
    PrtTHHead( pTran );                     /* print header if necessary */

    PrtTHCustomerName( pTran->TranGCFQual.aszName );

    /* --- determine system uses unique transaction no. --- */
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        /* --- print unique trans no. and no. of person --- */
        PrtTHTblPerson( pTran->TranGCFQual.usTableNo, pTran->TranGCFQual.usNoPerson, PRT_DOUBLE );
    } else {            /* unique transaction no. is NOT selected */
        if ( (pTran->TranGCFQual.usNoPerson != 0) && (!CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT3))) {
            /* --- print no. of person --- */
            PrtTHTranNum( TRN_PSN_ADR, ( ULONG )( pTran->TranGCFQual.usNoPerson ));
        }
    }

    /* print P.B. of cashier interurpt at precheck, post check system, R3.3 */
    if ((pItem->uchMinorClass == CLASS_CASINTRECALL) && (pItem->lAmount)) {
        PrtTHAmtTaxMnem( TRN_PB_ADR, pTran->TranModeQual.fsModeStatus, pItem->fbModifier, pItem->lAmount); 
    }
}

/*
*===========================================================================
** Format  : VOID  PrtCasIn_EJ(TRANINFORMATION  *pTran);   
*                             ITEMTRANSOPEN *pItem);   
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in cashier in operation (electric journal).
*===========================================================================
*/
VOID  PrtCasIn_EJ(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem)
{
    /* --- determine system uses unique transaction no. --- */
    PrtEJCustomerName( pTran->TranGCFQual.aszName );

    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        /* --- print unique trans no. and no. of person --- */
        PrtEJTblPerson( pTran->TranGCFQual.usTableNo, pTran->TranGCFQual.usNoPerson, 0, PRT_DOUBLE );
    } else {            /* unique transaction no. is NOT selected */
        if ( (pTran->TranGCFQual.usNoPerson != 0) && (!CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT3))) {
            /* --- print no. of person --- */
            PrtEJTranNum( TRN_PSN_ADR, ( ULONG )( pTran->TranGCFQual.usNoPerson ));
        }
    }

    /* --- print guest check number for cashier interrupt on journal, R3.3 --- */
    PrtEJGuest( pTran->TranGCFQual.usGuestNo, pTran->TranGCFQual.uchCdv );

    /* print P.B. of cashier interurpt at precheck, post check system, R3.3 */
    if ((pItem->uchMinorClass == CLASS_CASINTRECALL) && (pItem->lAmount)) {
        PrtEJTaxMod( pTran->TranModeQual.fsModeStatus, pItem->fbModifier );
        PrtEJAmtMnem( TRN_PB_ADR, pItem->lAmount );
    }
}

/*
*===========================================================================
** Format  : VOID  PrtCasIn_SP(TRANINFORMATION  *pTran)
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
VOID  PrtCasIn_SP(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem)
{
    TCHAR  aszSPPrintBuff[ 2 ][PRT_SPCOLUMN + 1];   /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  usTableNo;
    SHORT   sWidthType;
    USHORT  i;

    /* --- if current system uses unique transaction no.,
        print it in double width character. --- */
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
    usSlipLine += PrtSPCustomerName( aszSPPrintBuff[ 0 ], pTran->TranGCFQual.aszName );
    usSlipLine += PrtSPTblPerson(aszSPPrintBuff[ usSlipLine ], usTableNo, pTran->TranGCFQual.usNoPerson, sWidthType );

    /* print P.B. of cashier interurpt at precheck, post check system, R3.3 */
    if ((pItem->uchMinorClass == CLASS_CASINTRECALL) && (pItem->lAmount)) {
        usSlipLine += PrtSPMnemTaxAmt( aszSPPrintBuff[ usSlipLine ], TRN_PB_ADR, pTran->TranModeQual.fsModeStatus, pItem->fbModifier, pItem->lAmount );
    }

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* if just after header printed, not print table No., No. of person */  
    if ( usSaveLine != 0 ) {
        if ( pTran->TranGCFQual.usNoPerson || pTran->TranGCFQual.usTableNo ) {
/*  --- fix a glitch (05/15/2001)
            PmgPrint( PMG_PRT_SLIP, aszSPPrintBuff[ 1 ], PRT_SPCOLUMN );*/
            PrtPrint( PMG_PRT_SLIP, aszSPPrintBuff[ 1 ], PRT_SPCOLUMN );
        } else {
/*  --- fix a glitch (05/15/2001)
            PmgPrint( PMG_PRT_SLIP, aszSPPrintBuff[ 0 ], PRT_SPCOLUMN );*/
            PrtPrint( PMG_PRT_SLIP, aszSPPrintBuff[ 0 ], PRT_SPCOLUMN );
        }
    } else {
        /* -- print all data in the buffer -- */ 
        for ( i = 0; i < usSlipLine; i++ ) {
/*  --- fix a glitch (05/15/2001)
            PmgPrint( PMG_PRT_SLIP, aszSPPrintBuff[ i ], PRT_SPCOLUMN );*/
            PrtPrint( PMG_PRT_SLIP, aszSPPrintBuff[ i ], PRT_SPCOLUMN );
        }
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;    

}

/* ===== End of File ( PrRCasIT.C ) ===== */
