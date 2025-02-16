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
* Title       : Print  Reorder
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRReodT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*   PrtReorder()    : print in re-order operation
*   PrtReorder_TH() : print in re-order operation (thermal)
*   PrtReorder_EJ() : print in re-order operation (electric journal)
*   PrtReorder_SP() : print in re-order operation (slip)
*   PrtDflReorder() : display in re-order operation
*   PrtDflReorderForm() : display in re-order operation
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 01.00.12 :  R.Itoh    : initial
* Jul-14-93 : 01.00.12 :  R.Itoh    : add PrtDflReorder()
* Oct-29-93 : 02.00.02 :  K.You     : del. PrtDflReorder()
* Jan-26-95 : 03.00.00 :  M.Ozawa   : recover PrtDflReorder()
* Mar-06-95 : 03.00.00 : T.Nakahata : Modify for Unique Transaction Number
* Jul-21-95 : 03.00.04 : T.Nakahata : FVT Comment (Trans# Mnemo 4 => 12Char)
*           :          :            : add customer name (16 chara)
* Dec-15-99 : 00.00.01 :  M.Ozawa   : add PrtDflReorderForm()
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
*/
 
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
#include <prt.h>
#include <dfl.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"
#include "csstbpar.h"

/*
*===========================================================================
** Format  : VOID  PrtReorder( TRANINFORMATION *pTran,
*                              ITEMTRANSOPEN   *pItem )
*
*   Input  : TRANINFORMATION *pTran -   Transaction Information address
*            ITEMTRANSOPEN   *pItem -   Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in re-order operation, CLASS_REORDER.
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
VOID PrtReorder( TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem )
{
    SHORT   sWidthType;

    /* --- if current system uses unique transaction no.,
        print it in double width character. --- */
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        sWidthType = PRT_DOUBLE;
    } else {
        sWidthType = PRT_SINGLE;
    }

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion( pItem->fsPrintStatus );

    if ( fsPrtPrintPort & PRT_SLIP ) {
        PrtReorder_SP( pTran, pItem, sWidthType );
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {
        PrtReorder_TH( pTran, pItem, sWidthType );
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {
        PrtReorder_EJ( pTran, pItem, sWidthType );
    } else if ( fsPrtStatus & PRT_SERV_JOURNAL ) {
		SHORT   fsPortSave = fsPrtPrintPort;

        fsPrtPrintPort = PRT_JOURNAL;
        PrtReorder_EJ( pTran, pItem, sWidthType );
        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID  PrtReorder_TH( TRANINFORMATION *pTran,
*                                 ITEMTRANSOPEN   *pItem,
*                                 SHORT           sWidthType )
*
*   Input  : TRANINFORMATION *pTran     -   Transaction Information address
*            ITEMTRANSOPEN   *pItem     -   Item Data address
*            SHORT           sWidthType -   Type of Print Character Width
*                               PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in re-order operation (thermal).
*===========================================================================
*/
VOID PrtReorder_TH( TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem, SHORT sWidthType )
{
    USHORT usTable = 0;
    USHORT usNoPer = 0;

    /* --- determine print type is single width or double width --- */
    if ( sWidthType == PRT_SINGLE ) {   /* single width print */
        /* --- store table no., if guest moved other table --- */
        if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TABLECHANGE ) {
            usTable = pTran->TranGCFQual.usTableNo;
        }
    } else {                            /* double width print */
        /* --- store unique transaction number --- */
        usTable = pTran->TranGCFQual.usTableNo;
    }

    /* --- store no. of person, if no. of person is changed --- */
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_PERSONCHANGE ) {
        usNoPer = pTran->TranGCFQual.usNoPerson;
    }

    PrtTHHead( pTran );

    PrtTHCustomerName( pTran->TranGCFQual.aszName );
    PrtTHTblPerGC( usTable, usNoPer, pTran->TranGCFQual.usGuestNo, pTran->TranGCFQual.uchCdv, 0, sWidthType );

    PrtTHAmtTaxMnem( TRN_PB_ADR, pTran->TranModeQual.fsModeStatus, pItem->fbModifier, pItem->lAmount); 
}

/*
*===========================================================================
** Format  : VOID  PrtReorder_EJ( TRANINFORMATION *pTran,
*                                 ITEMTRANSOPEN   *pItem,
*                                 SHORT           sWidthType )
*
*   Input  : TRANINFORMATION *pTran     -   Transaction Information address
*            ITEMTRANSOPEN   *pItem     -   Item Data address
*            SHORT           sWidthType -   Type of Print Character Width
*                               PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in re-order operation (electric journal).
*===========================================================================
*/
VOID PrtReorder_EJ( TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem, SHORT sWidthType )
{
    USHORT usTable = 0;
    USHORT usNoPer = 0;

    /* --- determine print type is single width or double width --- */
    if ( sWidthType == PRT_SINGLE ) {   /* single width print */
        /* --- store table no., if guest moved other table --- */
        if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TABLECHANGE ) {
            usTable = pTran->TranGCFQual.usTableNo;
        }
    } else {                            /* double width print */
        /* --- store unique transaction number --- */
        usTable = pTran->TranGCFQual.usTableNo;
    }

    /* --- store no. of person, if no. of person is changed --- */
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_PERSONCHANGE ) {
        usNoPer = pTran->TranGCFQual.usNoPerson;
    }

    PrtEJCustomerName( pTran->TranGCFQual.aszName );
    PrtEJTblPerson( usTable, usNoPer, 0, sWidthType );

    PrtEJGuest( pTran->TranGCFQual.usGuestNo, pTran->TranGCFQual.uchCdv );

    if (((pTran->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) &&
        ( CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3 ) == 0) &&
        ( CliParaMDCCheck( MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2 ) == 0)) {

        ;   /* not print PB amount at split guest check system */
    } else {
        PrtEJTaxMod( pTran->TranModeQual.fsModeStatus, pItem->fbModifier );
        PrtEJAmtMnem( TRN_PB_ADR, pItem->lAmount );
    }
}

/*
*===========================================================================
** Format  : VOID  PrtReorder_SP(TRANINFORMATION  *pTran,
*                               ITEMTRANSOPEN *pItem,
*                               SHORT sWidthType )
*               PRT_DOUBLE, PRT_SINGLE
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in re-order operation (slip).
*===========================================================================
*/
VOID PrtReorder_SP(TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem, SHORT sWidthType)
{
    TCHAR   aszSPPrintBuff[ 3 ][ PRT_SPCOLUMN + 1 ];    /* print data save area */
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines */
    USHORT  i;
    USHORT  usTable = 0;
    USHORT  usNoPer = 0;

    memset( aszSPPrintBuff[ 0 ], 0x00, sizeof( aszSPPrintBuff ));

    /* --- determine print type is single width or double width --- */
    if ( sWidthType == PRT_SINGLE ) {   /* single width print */
        /* --- store table no., if guest moved other table --- */
        if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TABLECHANGE ) {
            usTable = pTran->TranGCFQual.usTableNo;
        }
    } else {                            /* double width print */
        /* --- store unique transaction number --- */
        usTable = pTran->TranGCFQual.usTableNo;
    }

    /* --- store no. of person, if no. of person is changed --- */
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_PERSONCHANGE ) {
        usNoPer = pTran->TranGCFQual.usNoPerson;
    }

    /* --- prepare table no.(unique trans #) and no. of person string --- */
    usSlipLine += PrtSPCustomerName( aszSPPrintBuff[ 0 ], pTran->TranGCFQual.aszName );
    usSlipLine += PrtSPTblPerson( aszSPPrintBuff[ usSlipLine ], usTable, usNoPer, sWidthType );

    /* --- set P/B, tax# 1-3 mnemonic and amount --- */
    usSlipLine += PrtSPMnemTaxAmt( aszSPPrintBuff[ usSlipLine ], TRN_PB_ADR, pTran->TranModeQual.fsModeStatus, pItem->fbModifier, pItem->lAmount );

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine( usSlipLine, pTran );

    /* --- if just after header printed,
        do not print table No. and No. of person --- */
    if ( usSaveLine != 0 ) {
        if ( usNoPer || usTable ) {
            PrtPrint( PMG_PRT_SLIP, aszSPPrintBuff[ 1 ], PRT_SPCOLUMN );
        } else {
            PrtPrint( PMG_PRT_SLIP, aszSPPrintBuff[ 0 ], PRT_SPCOLUMN );
        }
    } else {
        /* -- print all data in the buffer -- */ 
        for ( i = 0; i < usSlipLine; i++ ) {
            PrtPrint( PMG_PRT_SLIP, aszSPPrintBuff[ i ], PRT_SPCOLUMN );
        }
    }

    /* --- update current line No. --- */
    usPrtSlipPageLine += ( usSlipLine + usSaveLine );
}


/*
*===========================================================================
** Format  : VOID    PrtDflReorder(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays reorder.
*===========================================================================
*/
VOID PrtDflReorder(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem)
{
    TCHAR   aszDflBuff[ 6 ][ PRT_DFL_LINE + 1 ];    /* display data save area */
    USHORT  usLineNo;                   /* number of lines to be displayed */
    USHORT  usOffset = 0;
    USHORT  i;

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    memset( aszDflBuff, 0x00, sizeof( aszDflBuff ));

    /* -- set header -- */
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflTaxMod( aszDflBuff[ usLineNo ], pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    usLineNo += PrtDflAmtMnem( aszDflBuff[ usLineNo ], TRN_PB_ADR, pItem->lAmount);

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- set display data in the buffer -- */
    PrtDflIType( usLineNo, DFL_TRANOPEN );

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData( aszDflBuff[ i ], &usOffset );
        if ( aszDflBuff[ i ][ PRT_DFL_LINE ] != '\0' ) {
            i++;
        }
    }

    /* -- send display data -- */
    PrtDflSend();
}

/*
*===========================================================================
** Format  : VOID    PrtDflReorder(TRANINFORMATION  *pTran, ITEMTRANSOPEN *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays reorder.
*===========================================================================
*/
USHORT PrtDflReorderForm(TRANINFORMATION  *pTran, ITEMTRANSOPEN  *pItem, TCHAR *puchBuffer)
{
    TCHAR   aszDflBuff[ 6 ][ PRT_DFL_LINE + 1 ];    /* display data save area */
    USHORT  usLineNo=0, i;                   /* number of lines to be displayed */

    memset( aszDflBuff, 0x00, sizeof( aszDflBuff ));

    /* -- set header -- */
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
#if 0
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    if (pItem->lAmount) {
        usLineNo += PrtDflTaxMod( aszDflBuff[ usLineNo ], pTran->TranModeQual.fsModeStatus, pItem->fbModifier);
        usLineNo += PrtDflAmtMnem( aszDflBuff[ usLineNo ], TRN_PB_ADR, pItem->lAmount);
    }

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/***** End Of Source *****/
