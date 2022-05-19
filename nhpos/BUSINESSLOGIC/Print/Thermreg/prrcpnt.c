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
* Program Name: PrRCpnT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtItemCoupon() : main procedure for print coupon item
*               PrtDispCoupon() : main procedure for display coupon item
*               PrtCoupon()     : print coupon on each printers.
*               PrtCoupon_TH()  : print coupon on thermal printer
*               PrtCoupon_EJ()  : print coupon on electric journal
*               PrtCoupon_SP()  : print coupon slip printer
*               PrtDflCoupon()  : display coupon item ( disp. on the fly)
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Mar-01-95 : 00.00.01 : T.Nakahata : Initial
*
** GenPOS **
* Feb-19-18 : v2.2.2.0 : R.Chambers  : fix buffer size used with function PrtGetSISymCoupon().
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

	pItem->aszMnemonic[STD_DEPTPLUNAME_LEN] = 0;  // ensure that the PLU mnemonic is zero terminated.

    switch ( pItem->uchMinorClass ) {

    case CLASS_COMCOUPON:
    case CLASS_UPCCOUPON:
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
    case CLASS_UPCCOUPON:
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
        }
        return;
    }

    /* --- determine this item is printed on slip printer --- */
    if ( fsPrtPrintPort & PRT_SLIP ) {
        PrtCoupon_SP( pTran, pItem );
    }

    /* --- determine this item is printed on thermal printer */
    if ( fsPrtPrintPort & PRT_RECEIPT ) {
        PrtCoupon_TH( pTran, pItem );
    }

    /* --- determine this item printed on electric jornal printer */
    if ( fsPrtPrintPort & PRT_JOURNAL ) {
        PrtCoupon_EJ( pTran, pItem );
    }
}



/*
*===========================================================================
** Format  : VOID PrtCoupon_TH( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMCOUPON       *pItem     -Coupon item address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints coupon item on thermal printer.
*===========================================================================
*/

VOID  PrtCoupon_TH( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
{
    /* --- print header line --- */
    PrtTHHead( pTran );

    /* --- print void line --- */
    PrtTHVoid( pItem->fbModifier, pItem->usReasonCode );

    /* --- print number line --- */
    PrtTHNumber( pItem->aszNumber );

    /* --- print tax modifier line --- */
    PrtTHTaxMod( pTran->TranModeQual.fsModeStatus, pItem->fbModifier );

    /* --- print coupon item line --- */
    PrtTHCoupon( pItem );
}



/*
*===========================================================================
** Format  : VOID PrtCoupon_EJ( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMCOUPON       *pItem     -Coupon item address
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function prints coupon item on electric jornal printer.
*===========================================================================
*/

VOID  PrtCoupon_EJ( TRANINFORMATION *pTran, ITEMCOUPON *pItem )
{
    TCHAR  aszPLUNo[PLU_MAX_DIGIT + 1];

    /* --- print void line --- */
	PrtEJVoid( pItem->fbModifier, pItem->usReasonCode );

    /* --- print number line --- */
    PrtEJNumber( pItem->aszNumber );

    /* --- print tax modifier line --- */
    PrtEJTaxMod( pTran->TranModeQual.fsModeStatus, pItem->fbModifier );

	memset(aszPLUNo, 0x00, sizeof(aszPLUNo));
    if (pItem->uchMinorClass == CLASS_UPCCOUPON) {
        RflConvertPLU(aszPLUNo, pItem->auchUPCCouponNo);
        PrtEJPLUNo(aszPLUNo);
	} else {
		RflSPrintf (aszPLUNo, PLU_MAX_DIGIT, _T("CCP#%d"), pItem->uchCouponNo);
        PrtEJPLUNo(aszPLUNo);
	}
    /* --- print coupon item line --- */
    PrtEJCoupon( pItem );
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
    TCHAR   aszSPPrintBuff[ 3 ][ PRT_SPCOLUMN + 1 ] = {0}; /* print data save area */
    USHORT  usSlipLine;         /* number of lines to be printed */
    USHORT  usSaveLine;         /* save slip lines to be added */
    USHORT  usIndex;

    /* -- set void mnemonic and number -- */
    usSlipLine = PrtSPVoidNumber( aszSPPrintBuff[ 0 ], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);

    /* -- set tax modifier mnemonic -- */
    usSlipLine += PrtSPTaxMod( aszSPPrintBuff[ usSlipLine ], pTran->TranModeQual.fsModeStatus, pItem->fbModifier);

    /* -- set coupon mnemonic, and its amount --- */
	if ( CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT1) ) {
		TCHAR  aszSpecMnem[PARA_SPEMNEMO_LEN * 2 + 1] = {0};  /* times 2 for possible double in PrtGetSISymCoupon() */

	    if (PrtGetSISymCoupon(aszSpecMnem, pItem)) {						/* si symbol for item disc. */
		    usSlipLine += PrtSPCouponSISym( aszSPPrintBuff[ usSlipLine ], pItem->aszMnemonic, pItem->lAmount, aszSpecMnem, CliParaMDCCheck(MDC_TAX2_ADR, EVEN_MDC_BIT2) ); 
		} else {
			usSlipLine += PrtSPCoupon( aszSPPrintBuff[ usSlipLine ], pItem->aszMnemonic, pItem->lAmount);
    	}
    } else {
	    usSlipLine += PrtSPCoupon( aszSPPrintBuff[ usSlipLine ], pItem->aszMnemonic, pItem->lAmount);
	}
	
    /* -- determine whether the paper is necessary to be changed --- */
    usSaveLine = PrtCheckLine( usSlipLine, pTran );

    /* -- print all data in the buffer -- */ 
    for ( usIndex = 0; usIndex < usSlipLine; usIndex++ ) {
        PrtPrint( PMG_PRT_SLIP, aszSPPrintBuff[ usIndex ], PRT_SPCOLUMN );
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
    USHORT  usLineNo=0, i,j;                       /* number of lines to be displayed */
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
		for (j=0; j<(PRT_DFL_LINE+1); j++) {
        	if (aszDflBuff[i][j] == _T('\0')) {
        		aszDflBuff[i][j] = _T(' ');
			}
        }
	}

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
	_tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;

}

/* ===== End of File ( PrRCpn_.C ) ===== */
