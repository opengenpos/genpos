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
* Title       : Print  Store/Recall head
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRSRec_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*    PrtStoreRecall()    : print store/recall
*    PrtStoreRecall_RJ() : print store/recall (receipt & journal)
*    PrtStoreRecall_SP() : print store/recall (slip)
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-31-95 : 00.00.01 : T.Nakahata : Initial ( Add Release 3.0 )
* Jul-20-95 : 03.00.04 : T.Nakahata : FVT Comment (Trans# Mnemo 4 => 12Char)
*           :          :            : add customer name (16 chara)
* Jul-26-95 : 03.00.04 : T.Nakahata : print on journal by service total key
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
/**------- M S - C -------**/
#include	<tchar.h>
#include <string.h>


/**------- 2170 local------**/
#include <ecr.h>
/* #include <pif.h> */
#include <pmg.h>
#include <paraequ.h>
/* #include <para.h> */
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include "prtrin.h"
#include "prrcolm_.h"

/*
*===========================================================================
** Format  : VOID  PrtStoreRecall( TRANINFORMATION  *pTran,
*                                  ITEMTRANSOPEN    *pItem )
*   Input  : TRANINFORMATION *pTran - Transaction Information address
*            ITEMTRANSOPEN   *pItem - Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis:    This function prints store/recall.
*===========================================================================
*/
VOID PrtStoreRecall( TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem )
{
    SHORT   sWidthType;
    SHORT   fsPortSave;

    if (( pTran->TranCurQual.flPrintStatus & CURQUAL_STORE_RECALL ) &&
        ( pTran->TranCurQual.fsCurStatus   & CURQUAL_CANCEL )) {
        return;     
    }

    /* --- determine current system uses unique trans. no. --- */

    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        sWidthType = PRT_DOUBLE;
    } else {
        sWidthType = PRT_SINGLE;
    }
    
    /* -- set print portion to static area "fsPrtPrintPort" -- */

    PrtPortion( pItem->fsPrintStatus );

    if ( fsPrtPrintPort & PRT_SLIP ) {
        /* --- slip print --- */
        PrtStoreRecall_SP( pTran, sWidthType );
    }

    if ( fsPrtPrintPort & ( PRT_RECEIPT | PRT_JOURNAL )) {
        /* --- receipt, journal --- */
        PrtStoreRecall_RJ( pTran, sWidthType );

    }
    if (( ! ( fsPrtPrintPort & PRT_JOURNAL )) &&
        ( fsPrtStatus & PRT_SERV_JOURNAL )) {

        fsPortSave = fsPrtPrintPort;
        fsPrtPrintPort = PRT_JOURNAL;

        /* --- print new check trans on journal --- */

        PrtStoreRecall_RJ( pTran, sWidthType );

        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID PrtStoreRecall_RJ( TRANINFORMATION *pTran,
*                                    SHORT           sWidthType);
*
*   Input  : TRANINFORMATION *pTran     - Transaction Information address
*            SHORT           sWidthType - Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in store/recall. (receipt & journal)
*===========================================================================
*/
VOID PrtStoreRecall_RJ( TRANINFORMATION *pTran, SHORT sWidthType )
{
    /* -- table no., no. of person -- */

    PrtRJCustomerName( pTran->TranGCFQual.aszName );
    PrtRJTblPerson( pTran->TranGCFQual.usTableNo,
                    pTran->TranGCFQual.usNoPerson,
                    0,
                    sWidthType );

    /* --- print out order number --- */

    PrtRJGuest( pTran->TranGCFQual.usGuestNo, pTran->TranGCFQual.uchCdv );
}

/*
*===========================================================================
** Format  : VOID PrtStoreRecall_SP( TRANINFORMATION *pTran,
*                                    SHORT           sWidthType )
*
*   Input  : TRANINFORMATION *pTran     -   Transaction Information address
*            SHORT           sWidthType -   Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints store/recall (slip)
*===========================================================================
*/
VOID PrtStoreRecall_SP( TRANINFORMATION *pTran, SHORT sWidthType )
{
    TCHAR   aszSPPrintBuff[ 3 ][ PRT_SPCOLUMN + 1 ];
    USHORT  i;
    USHORT  usTable = 0;
    USHORT  usNoPer = 0;
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */

    /* --- set header line --- */

    memset( aszSPPrintBuff[ 0 ], 0x00, sizeof( aszSPPrintBuff ));

    usSlipLine =  PrtSPHeader3( aszSPPrintBuff[ 0 ], pTran );
    usSlipLine += PrtSPHeader1( aszSPPrintBuff[ usSlipLine ], pTran, sWidthType );
    usSlipLine += PrtSPHeader2( aszSPPrintBuff[ usSlipLine ], pTran , 0);

    /* --- check if paper change is necessary or not --- */ 

    usSaveLine = PrtCheckLine( usSlipLine, pTran );
 
    /* -- not print header line if just after paper change -- */

    if ( usSaveLine != 0 ) {

        usSlipLine -= 3;

    } else {

        /* -- print all data in the buffer -- */

        for ( i = 0; i < usSlipLine; i++ ) {

            PmgPrint( PMG_PRT_SLIP, aszSPPrintBuff[ i ], PRT_SPCOLUMN );
        }
    }

    /* --- update current line No. --- */

    usPrtSlipPageLine += ( usSaveLine + usSlipLine );
}

/* ===== End of File ( PrRSRec_.C ) ===== */
