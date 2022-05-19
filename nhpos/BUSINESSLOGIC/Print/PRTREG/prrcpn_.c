/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *        AT&T GIS Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Coupon Item
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRCpn_.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtItemCoupon() : main procedure for print coupon item
*               PrtDispCoupon() : main procedure for display coupon item
*               PrtCoupon()     : print coupon item on each printers.
*               PrtCoupon_RJ()  : print coupon on receipt & journal printer
*               PrtCoupon_VL()  : print coupon on validation printer
*               PrtCoupon_SP()  : print coupon slip printer
*               PrtDflCoupon()  : display coupon item ( disp. on the fly)
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Feb-27-95 : 00.00.01 : T.Nakahata : Initial
* Jul-21-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
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
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <dfl.h>
#include <pmg.h>
#include <prt.h>
#include "rfl.h"
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
** Format  : VOID  PrtItemCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
*
*   Input  : TRANINFORMATION    *pTran  -   Transaction Information address
*            ITEMCOUPON         *pItem  -   Coupon Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function is main procedure for print coupon item.
*===========================================================================
*/
VOID   PrtItemCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
{
    if (pItem->fbReduceStatus & REDUCE_ITEM) {
        return;
    }

    switch ( pItem->uchMinorClass ) {

    case CLASS_COMCOUPON:
    case CLASS_UPCCOUPON:   /* saratoga */
        PrtCoupon( pTran, pItem );
        break;

    default:
        break;
    }
}

/*
*===========================================================================
** Format  : VOID  PrtDispCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
*
*   Input  : TRANINFORMATION    *pTran  -   Transaction Information address
*            ITEMCOUPON         *pItem  -   Coupon Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function is main procedure for display coupon item.
*===========================================================================
*/
VOID   PrtDispCoupon( TRANINFORMATION  *pTran, ITEMCOUPON *pItem )
{
    switch ( pItem->uchMinorClass ) {

    case CLASS_COMCOUPON:
    case CLASS_UPCCOUPON:   /* saratoga */
        PrtDflCoupon( pTran, pItem );
        break;

    default:
        break;
    }
}

/*
*===========================================================================
**  Format : VOID    PrtCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
*
*    Input : TRANINFORMATION  *pTran    -   Transaction Information address
*            ITEMCOUPON       *pItem    -   coupon item address
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints a coupon item on each printers.
*===========================================================================
*/
VOID PrtCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion( pItem->fsPrintStatus );

    /* --- determine this item is validation print --- */
    if ( pItem->fsPrintStatus & PRT_VALIDATION ) {
        if ( CliParaMDCCheck( MDC_VALIDATION_ADR, EVEN_MDC_BIT2 )) {
            /* --- initialize current pages and lines of slip printer --- */
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}

            /* --- print slip validation header lines --- */
            PrtSPVLHead( pTran );

            /* --- print coupon item line to slip printer --- */
            PrtCoupon_SP( pTran, pItem );

            /* --- print slip validation trailer --- */
            PrtSPVLTrail( pTran );
        } else {
            PrtCoupon_VL( pTran, pItem );
        }
        return;
    }

    /* --- determine this item is printed on slip printer --- */
    if ( fsPrtPrintPort & PRT_SLIP ) {
        PrtCoupon_SP( pTran, pItem );
    }

    /* --- determine this item is printed on receipt or journal printer */
    if (( fsPrtPrintPort & PRT_RECEIPT ) || ( fsPrtPrintPort & PRT_JOURNAL )) {
        PrtCoupon_RJ(pTran, pItem);
    }
}



/*
*===========================================================================
** Format  : VOID PrtCoupon_RJ( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMCOUPON       *pItem     -Coupon item address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints coupon item on receipt and journal printer.
*===========================================================================
*/
VOID  PrtCoupon_RJ( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
{
    TCHAR  aszPLUNo[PLU_MAX_DIGIT + 1];

	memset(aszPLUNo, 0x00, sizeof(aszPLUNo));

    /* --- print void line --- */
    PrtRJVoid( pItem->fbModifier );

    /* --- print number line --- */
    PrtRJNumber( pItem->aszNumber );

    /* --- print tax modifier line --- */
    PrtRJTaxMod( pTran->TranModeQual.fsModeStatus, pItem->fbModifier );

    if (pItem->uchMinorClass == CLASS_UPCCOUPON) {
        RflConvertPLU(aszPLUNo, pItem->auchUPCCouponNo);
        PrtJPLUNo(aszPLUNo);
    }
    /* --- print coupon item line --- */
    PrtRJCoupon( pItem );
}



/*
*===========================================================================
** Format  : VOID PrtCoupon_VL( TRANINFORMATION *pTran, ITEMCOUPON *pItem );
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMCOUPON       *pItem     -Coupon data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints coupon item on validation.
*===========================================================================
*/
VOID  PrtCoupon_VL( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
{
    /* --- begin validation print --- */
    PmgBeginValidation( PMG_PRT_RCT_JNL );

    switch ( usPrtVLColumn ) {

    case PRT_VL_24:         /* 24 char printer */
        /* --- print validation header lines and void mnemonics --- */
        PrtVLHead( pTran, pItem->fbModifier );

        /* --- print number line --- */
        PrtVLNumber( pItem->aszNumber );

        /* --- print coupon mnemonics, and its amount --- */
        PrtVLCoupon( pItem, pTran->TranModeQual.fsModeStatus );

        /* --- print validation trailer lines --- */
        PrtVLTrail( pTran );
        break;

    case PRT_VL_21:         /* 21 char printer */
        /* --- print trailer and coupon mnemonics, its amount --- */
        PrtVSCoupon( pTran, pItem );
        break;

    default:
        break;
    }

    /* --- end validation print --- */
    PrtEndValidation( PMG_PRT_RCT_JNL );
}

/*
*===========================================================================
** Format  : VOID PrtCoupon_SP( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
*
*    Input : TRANINFORMATION    *pTran  - Transaction information address
*            ITEMCOUPON         *pItem  - coupon data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints coupon item on slip.
*===========================================================================
*/
VOID PrtCoupon_SP( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
{
    TCHAR   aszSPPrintBuff[ 3 ][ PRT_SPCOLUMN + 1 ]; /* print data save area */
    USHORT  usSlipLine;         /* number of lines to be printed */
    USHORT  usSaveLine;         /* save slip lines to be added */
    USHORT  usIndex;

    /* -- set void mnemonic and number -- */
    memset( aszSPPrintBuff[ 0 ], 0x00, sizeof( aszSPPrintBuff ));

    usSlipLine = PrtSPVoidNumber( aszSPPrintBuff[ 0 ], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);

    /* -- set tax modifier mnemonic -- */
    usSlipLine += PrtSPTaxMod( aszSPPrintBuff[ usSlipLine ], pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    /* -- set coupon mnemonic, and its amount --- */
    usSlipLine += PrtSPCoupon( aszSPPrintBuff[ usSlipLine ], pItem->aszMnemonic, pItem->lAmount);

    /* -- determine whether the paper is necessary to be changed --- */
    usSaveLine = PrtCheckLine( usSlipLine, pTran );

    /* -- print all data in the buffer -- */ 
    for ( usIndex = 0; usIndex < usSlipLine; usIndex++ ) {
        PmgPrint( PMG_PRT_SLIP, aszSPPrintBuff[ usIndex ], PRT_SPCOLUMN );
    }

   /* -- update current line No. -- */
    usPrtSlipPageLine += ( usSlipLine + usSaveLine );
}

/*
*===========================================================================
** Format  : VOID PrtDflCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Coupon data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function displays coupon item.
*===========================================================================
*/
VOID PrtDflCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
{
    TCHAR   aszDflBuff[ 9 ][ PRT_DFL_LINE + 1 ];    /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;
    USHORT  usIndex;

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    memset( aszDflBuff, 0x00, sizeof( aszDflBuff ));

    /* -- set header -- */
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo );

    /* -- set item data -- */
    usLineNo += PrtDflVoid( aszDflBuff[ usLineNo ], pItem->fbModifier, pItem->usReasonCode );
    usLineNo += PrtDflNumber( aszDflBuff[ usLineNo ], pItem->aszNumber );
    usLineNo += PrtDflTaxMod( aszDflBuff[ usLineNo ], pTran->TranModeQual.fsModeStatus, pItem->fbModifier );
    usLineNo += PrtDflCpnItem( aszDflBuff[ usLineNo ], pItem->aszMnemonic, pItem->lAmount );

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- check void status -- */
    PrtDflCheckVoid( pItem->fbModifier );

    /* -- set display data in the buffer -- */ 
    PrtDflIType( usLineNo, DFL_COUPON );

    for ( usIndex = 0; usIndex < usLineNo; usIndex++ ) {
        PrtDflSetData( aszDflBuff[ usIndex ], &usOffset );
        if ( aszDflBuff[ usIndex ][ PRT_DFL_LINE ] != 0x00 ) {
            usIndex++;
        }        
    }

    /* -- send display data -- */
    PrtDflSend();
}

/*
*===========================================================================
** Format  : VOID  PrtDispCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
*
*   Input  : TRANINFORMATION    *pTran  -   Transaction Information address
*            ITEMCOUPON         *pItem  -   Coupon Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function is main procedure for display coupon item.
*===========================================================================
*/
USHORT   PrtDispCouponForm( TRANINFORMATION  *pTran, ITEMCOUPON *pItem, TCHAR *puchBuffer )
{
    USHORT usLine;
    
    switch ( pItem->uchMinorClass ) {

    case CLASS_COMCOUPON:
    case CLASS_UPCCOUPON:
        usLine = PrtDflCouponForm( pTran, pItem, puchBuffer );
        break;

    default:
        usLine = 0;
        break;
    }
    
    return usLine;
}

/*
*===========================================================================
** Format  : VOID PrtDflCoupon( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMSALES        *pItem     -Coupon data address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function displays coupon item.
*===========================================================================
*/
USHORT PrtDflCouponForm( TRANINFORMATION *pTran, ITEMCOUPON *pItem, TCHAR *puchBuffer )
{
    TCHAR   aszDflBuff[ 9 ][ PRT_DFL_LINE + 1 ];    /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;

    memset( aszDflBuff, 0x00, sizeof( aszDflBuff ));

    /* -- set header -- */
#if 0
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo );
#endif
    /* -- set item data -- */
    usLineNo += PrtDflVoid( aszDflBuff[ usLineNo ], pItem->fbModifier, pItem->usReasonCode );
    usLineNo += PrtDflNumber( aszDflBuff[ usLineNo ], pItem->aszNumber );
    usLineNo += PrtDflTaxMod( aszDflBuff[ usLineNo ], pTran->TranModeQual.fsModeStatus, pItem->fbModifier );
    usLineNo += PrtDflCpnItem( aszDflBuff[ usLineNo ], pItem->aszMnemonic, pItem->lAmount );

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/* ===== End of File ( PrRCpn_.C ) ===== */
