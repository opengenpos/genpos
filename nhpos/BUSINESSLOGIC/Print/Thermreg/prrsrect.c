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
* Program Name: PrRSRecT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*    PrtStoreRecall()    : print store/recall
*    PrtStoreRecall_TH() : print store/recall (thermal)
*    PrtStoreRecall_EJ() : print store/recall (electric journal)
*    PrtStoreRecall_SP() : print store/recall (slip)
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-31-95 : 00.00.01 : T.Nakahata : Initial ( Add Release 3.0 )
* Jul-20-95 : 03.00.04 : T.Nakahata : FVT Comment (Trans# Mnemo 4 => 12Char)
*           :          :            : add customer name (16 chara)
* Jul-25-95 : 03.00.04 : T.Nakahata : print on journal by service total key
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
** Synopsis: This function prints store/recall, CLASS_STORE_RECALL.
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
VOID PrtStoreRecall( TRANINFORMATION *pTran, ITEMTRANSOPEN *pItem )
{
    SHORT   sWidthType;

//	Removed for Amtrak Rel 2.2.1
//    if (( pTran->TranCurQual.flPrintStatus & CURQUAL_STORE_RECALL ) &&
//        ( pTran->TranCurQual.fsCurStatus   & CURQUAL_CANCEL )) {
//        return;     
//    }

    /* --- determine current system uses unique trans. no. --- */
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        sWidthType = PRT_DOUBLE;
    } else {
        sWidthType = PRT_SINGLE;
    }
    
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion( pItem->fsPrintStatus );

    if ( fsPrtPrintPort & PRT_SLIP ) {
        PrtStoreRecall_SP( pTran, sWidthType );
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {
        PrtStoreRecall_TH( pTran, sWidthType );
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {
        PrtStoreRecall_EJ( pTran, sWidthType );
    } else if ( fsPrtStatus & PRT_SERV_JOURNAL ) {
		SHORT   fsPortSave = fsPrtPrintPort;

        fsPrtPrintPort = PRT_JOURNAL;
        PrtStoreRecall_EJ( pTran, sWidthType );
        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID PrtStoreRecall_TH( TRANINFORMATION *pTran,
*                                    SHORT           sWidthType);
*
*   Input  : TRANINFORMATION *pTran     - Transaction Information address
*            SHORT           sWidthType - Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in store/recall. (thermal)
*===========================================================================
*/
VOID PrtStoreRecall_TH( TRANINFORMATION *pTran, SHORT sWidthType )
{

    /* --- print header, if necessary --- */

    PrtTHHead( pTran );

	//Receipt ID printing   //PDINU
	//This code checks to see if there is a mnemonic entered and prints it
	//on one line

	/* --- print order no. and unique trans no --- */

	if (pTran->TranModeQual.aszTent[0] > 0){
		PrtTHTentName( pTran->TranGCFQual.aszName );
	}
	else{
		PrtTHCustomerName( pTran->TranGCFQual.aszName );
	}
    PrtTHTblPerGC( pTran->TranGCFQual.usTableNo,
                   pTran->TranGCFQual.usNoPerson,
                   pTran->TranGCFQual.usGuestNo,
                   pTran->TranGCFQual.uchCdv,
                   0,
                   sWidthType );
}

/*
*===========================================================================
** Format  : VOID  PrtStoreRecall_EJ( TRANINFORMATION *pTran,
*                                     SHORT            sWidthType )
*
*   Input  : TRANINFORMATION *pTran      -  Transaction Information address
*            SHORT            sWidthType -  Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints in store/recall. (electric journal)
*===========================================================================
*/
VOID PrtStoreRecall_EJ( TRANINFORMATION *pTran, SHORT sWidthType )
{
    /* --- print table no. and order no. --- */

    PrtEJCustomerName( pTran->TranGCFQual.aszName );
    PrtEJTblPerson( pTran->TranGCFQual.usTableNo,
                    pTran->TranGCFQual.usNoPerson,
                    0,
                    sWidthType );
    PrtEJGuest( pTran->TranGCFQual.usGuestNo, pTran->TranGCFQual.uchCdv );
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
    TCHAR   szSPPrintBuff[ 3 ][ PRT_SPCOLUMN + 1 ];
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */

    /* --- set order No. and No. of person --- */
    memset( szSPPrintBuff[ 0 ], '\0', sizeof( szSPPrintBuff ));

    usSlipLine =  PrtSPHeader3( szSPPrintBuff[ 0 ], pTran );
    usSlipLine += PrtSPHeader1( szSPPrintBuff[ usSlipLine ], pTran, sWidthType );
    usSlipLine += PrtSPHeader2( szSPPrintBuff[ usSlipLine ], pTran, 0 );

    /* --- check if paper change is necessary or not --- */ 
    usSaveLine = PrtCheckLine( usSlipLine, pTran );
 
    /* -- not print header line if just after paper change -- */
    if ( usSaveLine != 0 ) {
        usSlipLine -= 3;
    } else {
		USHORT  i;
        /* -- print all data in the buffer -- */
        for ( i = 0; i < usSlipLine; i++ ) {
	/*  --- fix a glitch (05/15/2001)
            PmgPrint( PMG_PRT_SLIP, szSPPrintBuff[ i ], PRT_SPCOLUMN ); */
            PrtPrint( PMG_PRT_SLIP, szSPPrintBuff[ i ], PRT_SPCOLUMN );
        }
    }

    /* --- update current line No. --- */
    usPrtSlipPageLine += ( usSaveLine + usSlipLine );
}

/* ===== End of File ( PrRSRecT.C ) ===== */
