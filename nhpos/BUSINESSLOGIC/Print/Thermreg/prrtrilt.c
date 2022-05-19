/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
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
* Title       : Print  Trailer                
* Category    : Print, NCR 2170  US Hospitarity Application
* Program Name: PrRTrilT.C                                                      
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtTrail1()       : print normal trailer
*               PrtTrail1_TH()    : print trailer to thermal
*               PrtTrail1_EJ()    : print trailer to electric journal
*               PrtTrail1_SP()    : print trailer to slip
*               PrtDoubleHeader() : print double receipt's trailer 
*               PrtTrail2()       : print ticket trailer
*               PrtTrail3()       : print only header
*               PrtTrayTrail()    : print trailer for tray total
*               PrtSoftProm()     : promotional message for softcheck
*               PrtChkPaid()      : print check paid
*               PrtChkPaid_SP()   : print check paid to slip
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 01.00.12 :  R.Itoh    : initial                                   
* Aug-23-93 : 01.00.13 :  R.Itoh    : add PrtSoftProm()                                    
* Apr-08-94 : 00.00.04 : Yoshiko.J  : add EPT feature in GPUS
* Apr-28-94 : 02.05.00 : K.You      : bug fixed E-75 (mod. PrtSoftProm)
* May-02-94 : 02.05.00 : K.You      : bug fixed E-48 (mod. PrtTrail1_SP, PrtTrail1)
* Jul-26-95 : 03.00.04 : T.Nakahata : print on journal by service total key
* Sep-04-95 : 03.00.05 : T.Nakahata : bug fixed.
* Aug-12-99 : 03.05.00 : K.Iwata    : Marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
/**------- M S  - C  ------**/
#include	<tchar.h>
#include <string.h>
#include <memory.h>
/**------- 2170 local------**/
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
#include <trans.h>
#include <paraequ.h>
#include <para.h>
#include <plu.h>
#include <pararam.h>
#include <csstbpar.h>
#include <pmg.h>
#include <shr.h>
#include <prt.h>
#include "prtcom.h"
#include "prtrin.h"
#include "prtshr.h"
#include "prrcolm_.h"
#include <csstbttl.h>
#include <ej.h>
#include <rfl.h>

#include "csetk.h"
#include "uie.h"
#include "uireg.h"
#include "..\item\include\itmlocal.h"
#include "..\..\UifReg\uiregloc.h"

/**
;========================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/

VOID  PrtBarCodeCheckNumber (TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
	TCHAR  tcsBarCodeString [48];
	if( CliParaMDCCheckField(MDC_RECEIPT_RTN_ADR, MDC_PARAM_BIT_C) ) {
		ULONG  ulBarcodeCode = PRT_BARCODE_CODE_CODE39;
		if( CliParaMDCCheckField(MDC_RECEIPT_RTN_ADR, MDC_PARAM_BIT_D) ) {
			SHORT  i;
			UCHAR  *uchUniqueIdentifier;

			if (pItem)
				uchUniqueIdentifier = &(pItem->uchUniqueIdentifier[0]);
			else
				uchUniqueIdentifier = &(pTran->TranGCFQual.uchUniqueIdentifier[0]);

			// use the settings that are from PifMain.c that were read in from PARAMINI file.
			// this allows us to change out the bar code symbology easier.
			ulBarcodeCode = UifPrinterBarcodeSettings.ulSymbology;
			memset (tcsBarCodeString, 0, sizeof(tcsBarCodeString));
			for (i = 0; i < 24; i++) {
				tcsBarCodeString[i] = (uchUniqueIdentifier[i] & 0x0f) + _T('0');
			}
		} else {
			ulBarcodeCode = PRT_BARCODE_CODE_CODE39;
			_stprintf (tcsBarCodeString, _T("%d"), pTran->TranGCFQual.usGuestNo);
		}

		PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed */
		PmgPrintBarCode (PMG_PRT_RECEIPT, tcsBarCodeString, (UifPrinterBarcodeSettings.ulAlignment | PRT_BARCODE_TEXT_BELOW), ulBarcodeCode);
	}
}

/*
*===========================================================================
** Format  : VOID   PrtSoftCHK(UCHAR uchMinorClass)      
*
*   Input  : UCHAR        uchMinorClass     -Minor Class address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints promotional message and ept trailer on 
*			 receipt in soft check system.
*===========================================================================
*/
VOID PrtSoftCHK(UCHAR uchMinorClass)
{  
    UCHAR        uchI, uchImax = 5;
	BOOL         bPrintedSoftchk = FALSE;
	PARASOFTCHK  ST = {0};

	ST.uchMajorClass = CLASS_PARASOFTCHK;
    for ( uchImax += uchI = uchMinorClass; uchI < uchImax; uchI++ ) {       /* E-047 corr.4/20  */
        ST.uchAddress = uchI;
        CliParaRead(&ST);
        if (uchI < SOFTCHK_EPT1_ADR) {              /* soft check only  */
            if ( PRT_FULL_SP == PrtChkFullSP(ST.aszMnemonics) ) {
                return;                             /* not print        */
            }

            PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed      */
        } else if (uchI < SOFTCHK_RTN1_ADR) {
			// we want to allow specifying empty lines in order to remove the
			// EPT Logo under some circumstances such as when using EMV credit.
			//    Richard Chambers, Oct-29-2015
			if (ST.aszMnemonics[0] == '\0') {
				continue;
			}
			if (!bPrintedSoftchk) {
				// if this is our first printed line then do a line feed first.
				bPrintedSoftchk = TRUE;
				PrtFeed(PMG_PRT_RECEIPT, 1);
			}
		} else {
			if (ST.aszMnemonics[0] == '\0') {
	            ST.aszMnemonics[0] = _T(' ');
	            ST.aszMnemonics[1] = _T('\0');
			}
		}

		if ((ST.aszMnemonics[0] == _T('x') || ST.aszMnemonics[0] == _T('X')) && ST.aszMnemonics[1] == _T('_')) {
			TCHAR tchCardHolder[NUM_CPRSPTEXT + 1];

            PrtFeed(PMG_PRT_RECEIPT, 2);            /* 2 line feed to provide space for signature     */
			PrtPrint((USHORT)PMG_PRT_RECEIPT, (TCHAR *)ST.aszMnemonics, (USHORT)_tcslen(ST.aszMnemonics)); /* ### Mod (2171 for Win32) V1.0 */

			tchCardHolder[0] = PRT_CENTERED;
			TrnGetTransactionCardHolder(tchCardHolder+1);
			tcharTrimRight (tchCardHolder+1);
			if (tchCardHolder[1]) {
				PrtPrint(PMG_PRT_RECEIPT, tchCardHolder, (USHORT)_tcslen(tchCardHolder)); /* ### Mod (2171 for Win32) V1.0 */
				PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed      */
			}
		} else {
			PrtPrint((USHORT)PMG_PRT_RECEIPT, (TCHAR *)ST.aszMnemonics, (USHORT)_tcslen(ST.aszMnemonics)); /* ### Mod (2171 for Win32) V1.0 */
		}
    }

	if (uchMinorClass == SOFTCHK_EPT1_ADR && bPrintedSoftchk) {
		// if this is n EPT logo and we printed something then do a line feed following the text.
		PrtFeed(PMG_PRT_RECEIPT, 1);
	}
}      


/*===========================================================================
** Format  : PrtExtraCopyMnemonic(USHORT *usPrintMerch, USHORT usPrtType)
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints merchant/customer copy mnemonic on 
*			 receipt
*===========================================================================
*/
VOID  PrtExtraCopyMnemonic(ULONG *fsCurStatus, ULONG *fsCurStatus2, USHORT usPrtType)
{
    TCHAR  aszMnem[STD_TRANSMNEM_LEN + 1] = {0};
	TCHAR  aszBuffer[PRT_SPCOLUMN + 1] = {0};

	if (fsCurStatus && (*fsCurStatus & CURQUAL_PRINT_EPT_LOGO)) {
		PrtSoftCHK(SOFTCHK_EPT1_ADR);
	}

	aszMnem[0] = 0;
	if (fsCurStatus && (*fsCurStatus & CURQUAL_MERCH_DUP)) {
		RflGetTranMnem(aszMnem, TRN_MERC_CPY_ADR);
		*fsCurStatus &= ~CURQUAL_MERCH_DUP;
	} else if (fsCurStatus && (*fsCurStatus & CURQUAL_CUST_DUP)) {
		RflGetTranMnem(aszMnem, TRN_CUST_CPY_ADR);
		*fsCurStatus &= ~CURQUAL_CUST_DUP;
	}

	if (aszMnem[0]) {
		if (usPrtType == PMG_PRT_JOURNAL) {
			PrtPrint(usPrtType, aszMnem, PRT_SPCOLUMN);
		} else {
			aszBuffer[0] = PRT_CENTERED;
			PrtDouble(aszBuffer+1, (PRT_SPCOLUMN - 1), aszMnem);
			PrtPrint(usPrtType, aszBuffer, PRT_SPCOLUMN);
			PrtFeed(usPrtType, 1);
		}
	}

	aszMnem[0] = 0;
	if(fsCurStatus2 && (*fsCurStatus2 & PRT_GIFT_RECEIPT))
	{
		RflGetTranMnem(aszMnem, TRN_GIFT_RECEIPT_ADR);
	} else if(fsCurStatus2 && (*fsCurStatus2 & PRT_POST_RECEIPT))
	{
		RflGetTranMnem(aszMnem, TRN_POST_ADR);
	} else if(usPrtType == PMG_PRT_JOURNAL && fsCurStatus2 && (*fsCurStatus2 & PRT_DEMAND_COPY))
	{
		// do not print the TRN_EJ_PODREPRINT transaction mnemonic on a printed receipt if Print on Demand set
		RflGetTranMnem(aszMnem, TRN_EJ_PODREPRINT);
	} else if (fsCurStatus && (*fsCurStatus & CURQUAL_DUPLICATE_COPY)) {
		// this needs to be last in case there is a reprint of a duplicated receipt.
		// if there is a reprint then we need to use one of the above transaction mnemonics instead.
		RflGetTranMnem(aszMnem, TRN_TNDR_PRT_DUP_COPY);
		*fsCurStatus &= ~CURQUAL_DUPLICATE_COPY;
	}

	if (aszMnem[0]) {
		if (usPrtType == PMG_PRT_JOURNAL) {
			PrtPrint(usPrtType, aszMnem, PRT_SPCOLUMN);
		} else {
			aszBuffer[0] = PRT_CENTERED;
			PrtDouble(aszBuffer+1, (PRT_SPCOLUMN - 1), aszMnem);
			PrtPrint(usPrtType, aszBuffer, PRT_SPCOLUMN);
			PrtFeed(usPrtType, 1);
		}
	}

}

/*
*===========================================================================
** Format  : VOID   PrtTrail1(TRANINFORMATION  *pTran,
*                              ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints Trailer & Header.
*===========================================================================
*/
VOID  PrtTrail1(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {      /* slip print */

/*        if ( ! ((pTran->TranGCFQual.fsGCFStatus & GCFQUAL_DRIVE_THROUGH) &&
               ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK)
                == CURQUAL_CASHIER))) {*/
        if ( !((( pTran->TranCurQual.auchTotalStatus[ 0 ] / CHECK_TOTAL_TYPE ) == PRT_SERVICE1 ) ||
               (( pTran->TranCurQual.auchTotalStatus[ 0 ] / CHECK_TOTAL_TYPE ) == PRT_SERVICE2 ))  /* except service total */
            || (auchPrtSeatCtl[0])) {  /* seat no. consolidation print */

            /* not print trailer line when new check/ reorder */
            if ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER ||           /* not Guest Check operation */
                (pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK ||          /* not new check or reorder operation */
                (auchPrtSeatCtl[0]) ||  /* seat no. consolidation print */
                pTran->TranCurQual.fsCurStatus & CURQUAL_CANCEL ||
                (CliParaMDCCheck(MDC_NEWKEYSEQ_ADR, ODD_MDC_BIT1))) {   /* 8-2-2000  TAR# 148299 */
            
                if ( !( pTran->TranCurQual.fsCurStatus & CURQUAL_MULTICHECK ) ) {
                    PrtTrail1_SP(pTran, pItem);
                    usPrtSlipPageLine++;                    /* K */
                }
            }
        }
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {   /* thermal print */
		if(CliParaMDCCheckField(MDC_RECEIPT_RTN_ADR, MDC_PARAM_BIT_B) == 0) {
			PrtBarCodeCheckNumber (pTran, pItem);
		}
        PrtTrail1_TH(pTran, pItem->ulStoreregNo);
        PrtShrEnd();
    }

    if ( (fsPrtPrintPort & PRT_JOURNAL) || (fsPrtStatus & PRT_EJ_JOURNAL) ) {   /* electric journal */
        PrtTrail1_EJ(pTran, pItem->ulStoreregNo);
        PrtEJSend();                        /* send to E/J module */
    } else if ( fsPrtStatus & PRT_SERV_JOURNAL ) {
		SHORT   fsPortSave = fsPrtPrintPort;

        fsPrtPrintPort = PRT_JOURNAL;
        PrtTrail1_EJ( pTran, pItem->ulStoreregNo );
        PrtEJSend();                        /* send to E/J module */
        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID   PrtTrail1_TH(TRANINFORMATION  *pTran,
*                                ULONG   ulStRegNo);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ULONG         ulStRegNo     -Store Reg number
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints Trailer & Header.(thermal)
*===========================================================================
*/
VOID  PrtTrail1_TH(TRANINFORMATION  *pTran, ULONG   ulStRegNo)
{
    PrtFeed(PMG_PRT_RECEIPT, 1);            /* 1 line feed */

	if( (pTran->TranItemizers.sItemCounter != 0)  //SR 217 JHHJ Item Count Option
		&& CliParaMDCCheck(MDC_NEWKEYSEQ_ADR, ODD_MDC_BIT3))
	{
		PrtTHItemCount(pTran->TranItemizers.sItemCounter, PMG_PRT_RECEIPT);
		
		PrtFeed(PMG_PRT_RECEIPT, 1);
	}	else
	{
		if ( ( pTran->TranGCFQual.sItemCount != 0) //SR 1216 Print Item Count in PrntDMD2
		&& CliParaMDCCheck(MDC_NEWKEYSEQ_ADR, ODD_MDC_BIT3) )
		{
		PrtTHItemCount(pTran->TranGCFQual.sItemCount, PMG_PRT_RECEIPT);

		PrtFeed(PMG_PRT_RECEIPT, 1);
		}
	}

	// Print any text associated with this particular tender key that is needed to be
	// printed before we print the actual trailer lines identifying the operator, date, etc.
	// This includes things like DUPLICATE COPY as well as EPT logo, MERCHANT COPY, etc.
	{
		ULONG  fsCurStatus = pTran->TranCurQual.fsCurStatus;
		if (pTran->TranCurQual.fbNoPrint & CURQUAL_NO_EPT_LOGO_SIG) {
			// if we are not to print an EPT logo then ensure that the print logo flag is off.
			fsCurStatus &= ~CURQUAL_PRINT_EPT_LOGO;
		}
		PrtExtraCopyMnemonic(&fsCurStatus, &pTran->TranCurQual.fsCurStatus2, PMG_PRT_RECEIPT);
	}

    PrtTHTrail1(pTran);                     /* send 1st line */
    
    PrtTHTrail2(pTran, ulStRegNo);          /* send 2nd line */

//    PrtFeed(PMG_PRT_RECEIPT, 5+1);          /* feed, for 7158, saratoga */

	//PrtCut is removed and will now be put into TransactionEnd JHHJ
	PrtTransactionEnd(PRT_REGULARPRINTING,pTran->TranGCFQual.usGuestNo);

    fbPrtTHHead = 0;                        /* header not printed */

}

/*
*===========================================================================
** Format  : VOID   PrtTrail1_EJ(TRANINFORMATION  *pTran,
*                                ULONG   ulStRegNo);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ULONG         ulStRegNo     -Store Reg number
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints Trailer & Header.(electric journal)
*===========================================================================
*/
VOID  PrtTrail1_EJ(TRANINFORMATION  *pTran, ULONG   ulStRegNo)
{
	{
		ULONG  fsCurStatus = 0;

		PrtExtraCopyMnemonic(&fsCurStatus, &pTran->TranCurQual.fsCurStatus2, PMG_PRT_JOURNAL);
	}

	if(uchMaintDelayBalanceFlag)
	{
		PrtPrint( PMG_PRT_JOURNAL,_T("***DELAY BALANCE TRAN***"), EJ_COLUMN);
	}

    PrtEJTrail1(pTran, ulStRegNo);      /* send 1st line */
    PrtEJTrail2(pTran);                 /* send 2nd line */ 
}

/*
*===========================================================================
** Format  : VOID   PrtTrail1_SP(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints Trailer.(normal)
*===========================================================================
*/
VOID PrtTrail1_SP(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
	TCHAR  aszSPPrintBuff[10][PRT_SPCOLUMN + 1] = { 0 };    /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  i;
    ULONG   ulStoreRegSave;
    
    /* -- feed & print EPT logo -- */
    if ( pItem->uchMinorClass == CLASS_EPT_TRAILER ) {
        PrtTrailEPT_SP(aszSPPrintBuff[0]);              /* EPT trailer logo */
        usSlipLine += 5;
    }

    if ( pItem->uchMinorClass == CLASS_RETURNS_TRAILER ) {
        PrtTrailEPT_SP(aszSPPrintBuff[0]);              /* EPT trailer logo */
        usSlipLine += 5;
    }

	if( (pTran->TranItemizers.sItemCounter != 0)  //SR 217 JHHJ Item Count Option
		&& CliParaMDCCheck(MDC_NEWKEYSEQ_ADR, ODD_MDC_BIT3))
	{
		PrtTHItemCount(pTran->TranItemizers.sItemCounter, PMG_PRT_SLIP);
	}

    /* -- set trailer data -- */
    ulStoreRegSave = pTran->TranCurQual.ulStoreregNo;
    pTran->TranCurQual.ulStoreregNo = pItem->ulStoreregNo;
    if (usSlipLine == 0) {
        usSlipLine += PrtSPTrail1(aszSPPrintBuff[0], pTran, usSlipLine);
    } else {
        usSlipLine += PrtSPTrail1(aszSPPrintBuff[usSlipLine], pTran, usSlipLine);
    }
    pTran->TranCurQual.ulStoreregNo = ulStoreRegSave;

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
/*  --- fix a glitch (05/15/2001)
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN); */
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }
    usPrtSlipPageLine += usSlipLine;        /* update current line No. */

    /* -- slip release -- */
    PrtSlpRel();                            
}

/*
*===========================================================================
** Format  : VOID   PrtDoubleHeader(TRANINFORMATION  *pTran, ITEMPRINT  *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints Trailer & Header.(double header)
*===========================================================================
*/
VOID PrtDoubleHeader(TRANINFORMATION  *pTran)
{
    
    PrtTrail2(pTran);                   
    
    PrtShrEnd();

    PrtTHHead(pTran);                           /* print header if necessary */    
    
    PrtFeed(PMG_PRT_RECEIPT, 1);                /* 1 line feed */

    PrtTHDoubleHead();                          /* double header mnemo */

}

/*
*===========================================================================
** Format  : VOID   PrtTrail2(TRANINFORMATION  *pTran);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints ticket trailer on thermal.
*===========================================================================
*/
VOID PrtTrail2(TRANINFORMATION  *pTran)
{

	if(CliParaMDCCheckField(MDC_RECEIPT_RTN_ADR, MDC_PARAM_BIT_B) == 0) {
		PrtBarCodeCheckNumber (pTran, 0);
	}

    PrtFeed(PMG_PRT_RECEIPT, 1);                /* 1 line feed */

    PrtTHTickTrail1(pTran);                     /* ticket 1st line */

    PrtTHTickTrail2(pTran);                     /* ticket 2nd line */

//    PrtFeed(PMG_PRT_RECEIPT, 5+1);                /* feed, for 7158, saratoga */

//    PrtCut();                                   /* cut */

    fbPrtTHHead = 0;                            /* header not printed */

}
/*
*===========================================================================
** Format  : VOID   PrtTrail3(TRANINFORMATION  *pTran,ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints only Header on receipt & journal.
*            This function is called after "service total print".     
*            "service total print" includes own trailer.      
*===========================================================================
*/
VOID PrtTrail3(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_RECEIPT ) {       /* thermal print */
		if(CliParaMDCCheckField(MDC_RECEIPT_RTN_ADR, MDC_PARAM_BIT_B) == 0) {
			PrtBarCodeCheckNumber (pTran, pItem);
		}
        PrtTrail1_TH(pTran, pItem->ulStoreregNo);
        PrtShrEnd();
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {       /* electric journal */

        PrtTrail1_EJ(pTran, pItem->ulStoreregNo);
        PrtEJSend();                        /* send to E/J module */
    }
}

/*
*===========================================================================
** Format  : VOID   PrtTrayTrail(TRANINFORMATION  *pTran,ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints  trailer and header on receipt.
*            when printed on slip, this function does not print.
*===========================================================================
*/
VOID PrtTrayTrail(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( !(fsPrtPrintPort & PRT_SLIP) ) {       /* slip printing ?  */

        if ( fsPrtPrintPort & PRT_RECEIPT ) {       /* thermal print */

            PrtTrail1_TH(pTran, pItem->ulStoreregNo);
            PrtShrEnd();
        }

        if ( fsPrtPrintPort & PRT_JOURNAL ) {       /* electric journal */

            PrtTrail1_EJ(pTran, pItem->ulStoreregNo);
            PrtEJSend();                        /* send to E/J module */
        }
    }
}

/*
*===========================================================================
** Format  : VOID   PrtSoftProm(ITEMPRINT *pItem);      
*
*   Input  : ITEMPRINT        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function prints promotional message on receipt in soft check system.
*===========================================================================
*/
VOID PrtSoftProm(ITEMPRINT *pItem)
{     
    UCHAR   uchI;
	
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);   

    if ( fsPrtPrintPort & PRT_RECEIPT ) {       /* thermal print */
        if (pItem->uchMinorClass == CLASS_RETURNS_TRAILER) {
            uchI = SOFTCHK_RTN1_ADR;                    /* Returns logo message */
		} else if (pItem->uchMinorClass == CLASS_EPT_TRAILER) {
            uchI = SOFTCHK_EPT1_ADR;                    /* EPT logo message */
        } else {
            uchI = SOFTCHK_MSG1_ADR;                    /* Soft Check msg   */
			PrtSoftCHK(uchI);
        }
	}
}

/*
*===========================================================================
** Format  : VOID   PrtSoftProm_EPT(ITEMPRINT *pItem);      
*
*   Input  : ITEMPRINT        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none 
*                 
** Synopsis: This function request print of promotional message and ept trailer
*===========================================================================
*/      
VOID  PrtSoftProm_EPT(ITEMPRINT  *pItem)
{   
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);   

    if ( fsPrtPrintPort & PRT_RECEIPT ) {       /* thermal print */
//		PrtFeed(PMG_PRT_RECEIPT, 1);
//		PrtSoftCHK(SOFTCHK_EPT1_ADR);
		PrtFeed(PMG_PRT_RECEIPT, 1);
		PrtSoftCHK(SOFTCHK_MSG1_ADR);
    }
}


/*
*===========================================================================
** Format  : VOID   PrtChkPaid(ITEMPRINT *pItem);
*
*   Input  : ITEMPRINT        *pItem     -Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints Trailer & Header.
*===========================================================================
*/
VOID PrtChkPaid(ITEMPRINT  *pItem)
{

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtChkPaid_SP();
    }

}

/*
*===========================================================================
** Format  : VOID   PrtChkPaid_SP(TRANINFORMATION  *pTran);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints Trailer & Header.
*===========================================================================
*/
VOID PrtChkPaid_SP(VOID)
{
	TCHAR  aszSPPrintBuff[PRT_SPCOLUMN + 1] = { 0 };    /* print data save area */
    
    /* -- set check paid data -- */
    PrtSPChkPaid(aszSPPrintBuff, TRN_CKPD_ADR);

    /* -- print all data in the buffer -- */ 
/*  --- fix a glitch (05/15/2001)
    PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff, PRT_SPCOLUMN); */
    PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff, PRT_SPCOLUMN);

    /* -- slip release -- */
    PrtSlpRel();                            
}


