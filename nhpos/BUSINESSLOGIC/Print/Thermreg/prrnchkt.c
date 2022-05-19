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
* Title       : Print  Newcheck
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRNchkT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*   PrtNewChk()     :   print in new check operation
*   PrtNewChk_TH()  :   print in new check operation (thermal)
*   PrtNewChk_EJ()  :   print in new check operation (electric journal)
*   PrtNewChk_SP()  :   print in new check operation (slip)
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 00.00.01 :  R.Itoh    :
* Mar-06-95 : 03.00.00 : T.Nakahata : Modify for Unique Transaction Number.
* Jul-20-95 : 03.00.04 : T.Nakahata : FVT Comment (Trans# Mnemo 4 => 12Char)
*           :          :            : add customer name (16 chara)
* Jul-26-95 : 03.00.04 : T.Nakahata : print on journal by service total key
* Aug-08-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
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
#include "prtrin.h"
#include "prrcolm_.h"

/*
*===========================================================================
** Format  : VOID  PrtNewChk( TRANINFORMATION *pTran,
*                             ITEMTRANSOPEN   *pItem );
*
*   Input  : TRANINFORMATION *pTran -   Transaction Information address
*            ITEMTRANSOPEN   *pItem -   Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in new check operation, CLASS_NEWCHECK.
*            This function is one of several functions that are used to print
*            for a CLASS_ITEMTRANSOPEN message when printing a receipt. The
*            particular function will depend on several criteria such as:
*              - system type (Post Guest Check, Store Recall, etc.)
*              - key press used to start a new guest check or transaction
*
*            See function PrtItemTransOpen() for the handling of a CLASS_ITEMTRANSOPEN
*            message and how the minor code is used to determine which of the
*            functions to use.
*===========================================================================
*/
VOID PrtNewChk( TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem )
{
    SHORT   sWidthType;

    if ((!(fsPrtStatus & PRT_SPLIT_PRINT)) || (pTran->TranCurQual.uchPrintStorage == PRT_CONSOL_CP_EPT)) {
        pItem->uchSeatNo = 0;   /* not print seat# at not seat# tender */
    }

    /* --- if current system uses unique transaction no., print it in double width character. --- */
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        sWidthType = PRT_DOUBLE;
    } else {
        sWidthType = PRT_SINGLE;
    }

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion( pItem->fsPrintStatus );

    if ( fsPrtPrintPort & PRT_SLIP ) {
        PrtNewChk_SP( pTran, pItem, sWidthType );
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {
        PrtNewChk_TH( pTran, pItem, sWidthType );
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {
        PrtNewChk_EJ( pTran, pItem, sWidthType );
    } else if ( fsPrtStatus & PRT_SERV_JOURNAL ) {
		SHORT   fsPortSave = fsPrtPrintPort;

        fsPrtPrintPort = PRT_JOURNAL;
        PrtNewChk_EJ( pTran, pItem, sWidthType );
        fsPrtPrintPort = fsPortSave;
    }

    uchPrtSeatNo = pItem->uchSeatNo;    /* for continual slip print */

    if (fsPrtStatus & PRT_SPLIT_PRINT) {
        if (pItem->uchSeatNo == 0) {
            fsPrtStatus &= ~PRT_REC_SLIP;   /* not print "0.00" total line */
        }
    }
}

/*
*===========================================================================
** Format  : VOID   PrtNewChk_TH( TRANINFORMATION *pTran,
*                                 SHORT           sWidthType )
*
*   Input  : TRANINFORMATION *pTran     -   Transaction Information address
*            SHORT           sWidthType -   Type of Print Character Width
*                               PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in new check operation (thermal).
*===========================================================================
*/
VOID PrtNewChk_TH( TRANINFORMATION *pTran, ITEMTRANSOPEN  *pItem, SHORT sWidthType )
{
    if ((auchPrtSeatCtl[0] == 0)||(auchPrtSeatCtl[1] == 0)) {

        /* --- print thermal header line --- */

        PrtTHHead( pTran );

        /* --- print table no., no. of person and GC no. --- */

        PrtTHCustomerName( pTran->TranGCFQual.aszName );
        if ((pTran->TranGCFQual.auchFinSeat[1]) && (pItem->uchSeatNo)) {
            PrtTHTblPerGC( pTran->TranGCFQual.usTableNo,
                           pTran->TranGCFQual.usNoPerson,
                           pTran->TranGCFQual.usGuestNo,
                           pTran->TranGCFQual.uchCdv,
                           0,
                           sWidthType );
        } else {
            PrtTHTblPerGC( pTran->TranGCFQual.usTableNo,
                           pTran->TranGCFQual.usNoPerson,
                           pTran->TranGCFQual.usGuestNo,
                           pTran->TranGCFQual.uchCdv,
                           (UCHAR)(auchPrtSeatCtl[0] ? 0 : pItem->uchSeatNo),
                           sWidthType );
        }

        if ((pTran->TranGCFQual.auchFinSeat[1]) && (pItem->uchSeatNo)) {

            PrtTHMultiSeatNo(pTran);                    /* multiple seat no */
        } else
        if (auchPrtSeatCtl[0]) {    /* individual seat no print */
            PrtTHSeatNo(pItem->uchSeatNo);
        }

    } else {
        /* service ttl 2nd print */

        if (pTran->TranCurQual.uchPrintStorage != PRT_CONSOL_CP_EPT) {
            if ( fsPrtPrintPort & PRT_RECEIPT ) {

                PrtFeed(PMG_PRT_RECEIPT, 1);        /* feed */

                PrtTHSeatNo(pItem->uchSeatNo);
            }
        }
    }
}

/*
*===========================================================================
** Format  : VOID   PrtNewChk_EJ( TRANINFORMATION *pTran,
*                                 SHORT           sWidthType )
*
*   Input  : TRANINFORMATION *pTran     -   Transaction Information address
*            SHORT           sWidthType -   Type of Print Character Width
*                               PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in new check operation (electric journal).
*===========================================================================
*/
VOID PrtNewChk_EJ( TRANINFORMATION *pTran, ITEMTRANSOPEN  *pItem, SHORT sWidthType )
{
    if ((auchPrtSeatCtl[0] == 0)||(auchPrtSeatCtl[1] == 0)) {
        /* service ttl 1st print */

        /* -- table no., no. of person -- */

        PrtEJCustomerName( pTran->TranGCFQual.aszName );
        if ((pTran->TranGCFQual.auchFinSeat[1]) && (pItem->uchSeatNo)) {
            PrtEJTblPerson( pTran->TranGCFQual.usTableNo,
                            pTran->TranGCFQual.usNoPerson,
                            0,
                            sWidthType );
        } else {
            PrtEJTblPerson( pTran->TranGCFQual.usTableNo,
                            pTran->TranGCFQual.usNoPerson,
                            (UCHAR)(auchPrtSeatCtl[0] ? 0 : pItem->uchSeatNo),
                            sWidthType );
        }

        /* -- guest check mnem., number, check digit -- */

        PrtEJGuest( pTran->TranGCFQual.usGuestNo, pTran->TranGCFQual.uchCdv );

        if ((pTran->TranGCFQual.auchFinSeat[1]) && (pItem->uchSeatNo)) {

            PrtEJMultiSeatNo(pTran);
        } else
        if (auchPrtSeatCtl[0]) {    /* individual seat no print */
            PrtEJSeatNo(pItem->uchSeatNo);
        }

    } else {
        /* service ttl 2nd print */

        if (pItem->uchSeatNo) {

            PrtEJSeatNo(pItem->uchSeatNo);
        }
    }

}

/*
*===========================================================================
** Format  : VOID   PrtNewChk_SP( TRANINFORMATION *pTran,
*                                 SHORT           sWidthType )
*
*   Input  : TRANINFORMATION *pTran     -   Transaction Information address
*            SHORT           sWidthType -   Type of Print Character Width
*                               PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in new check oepration (slip).
*===========================================================================
*/
VOID  PrtNewChk_SP( TRANINFORMATION *pTran, ITEMTRANSOPEN  *pItem, SHORT sWidthType )
{
    TCHAR   aszSPPrintBuff[ 5 ][ PRT_SPCOLUMN + 1 ];    /* print data save area */
    USHORT  usSlipLine;                 /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */
    USHORT  i;

    /* --- set header line --- */

    memset( aszSPPrintBuff[ 0 ], 0x00, sizeof( aszSPPrintBuff ));

    if ((auchPrtSeatCtl[0] == 0)||(auchPrtSeatCtl[1] == 0)) {
        /* service ttl 1st print */

        usSlipLine  = PrtSPHeader3( aszSPPrintBuff[ 0 ], pTran );
        usSlipLine += PrtSPHeader1( aszSPPrintBuff[ usSlipLine ], pTran, sWidthType );
        if ((pTran->TranGCFQual.auchFinSeat[1]) && (pItem->uchSeatNo)) {
            usSlipLine += PrtSPHeader2( aszSPPrintBuff[ usSlipLine ], pTran , 0);
        } else {
            usSlipLine += PrtSPHeader2( aszSPPrintBuff[ usSlipLine ], pTran ,
                (UCHAR)(auchPrtSeatCtl[0] ? 0 : pItem->uchSeatNo));
        }

        usSlipLine += PrtSPCustomerName( aszSPPrintBuff[ usSlipLine ],
                                         pTran->TranGCFQual.aszName );

        /* -- set multiple seat no -- */
        if ((pTran->TranGCFQual.auchFinSeat[1]) && (pItem->uchSeatNo)) {
            usSlipLine += PrtSPMultiSeatNo( aszSPPrintBuff[ usSlipLine ], pTran);

        } else 
        /* -- set seat no -- */
        if (auchPrtSeatCtl[0]) {
            usSlipLine += PrtSPSeatNo(aszSPPrintBuff[ usSlipLine], pItem->uchSeatNo);    /* seat no. */
        }

    } else {
        /* service ttl 2nd print */

        usSlipLine = PrtSPSeatNo(aszSPPrintBuff[0], pItem->uchSeatNo);    /* seat no. */

    }

    /* -- check if paper change is necessary or not -- */

    usSaveLine = PrtCheckLine( usSlipLine, pTran );

    /* -- not print header line if just after paper change -- */

    if ( usSaveLine != 0 ) {

        usSlipLine = 0;

    } else {

        /* -- print all data in the buffer -- */

        for ( i = 0; i < usSlipLine; i++ ) {

/*  --- fix a glitch (05/15/2001)
            PmgPrint( PMG_PRT_SLIP, aszSPPrintBuff[ i ], PRT_SPCOLUMN ); */
            PrtPrint( PMG_PRT_SLIP, aszSPPrintBuff[ i ], PRT_SPCOLUMN );
        }
    }

    /* --- update current line No. --- */

    usPrtSlipPageLine += ( usSlipLine + usSaveLine );
}

/* ===== End of File ( PrRNchkT.C ) ===== */
