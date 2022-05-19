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
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Item  Header trailer                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRHeadT.C                                                      
* --------------------------------------------------------------------------
* Abstract:
*      PrtItemPrint() : print items specified class "CLASS_ITEMPRINT"
*      PrtDispPrint()
*
*      PrtChkCurSlip()
*      PrtTraining()
*      PrtTrain_TH()
*      PrtTrain_EJ()
*      PrtTrain_SP()
*      PrtPVoid()
*      PrtPVoid_TH()
*      PrtPVoid_EJ()
*      PrtPVoid_SP()
*      PrtPostRct()
*      PrtPostRct_TH()
*      PrtPostRct_EJ()
*      PrtPostRct_SP()
*      PrtCPGusLine()
*      PrtCPGusLine_TH()
*      PrtCPFuncErrCode_EJ()
*      PrtEPTError()
*      PrtEPTStub()
*      PrtEPTStub_TH()
*      PrtParkReceipt()
*      PrtParkReceipt_TH()
*      PrtParkReceipt_EJ()
*      PrtParkReceipt_SP()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 01.00.12 : R.Itoh     : initial                                   
* Jun-30-93 : 01.00.12 : K.You      : add CLASS_RESTART handling                                  
* Jul-02-93 : 01.00.12 : K.You      : add PrtPostRct, PrtPostRct_RJ, PrtPOSTRct_SP
*           :          :            : mod. PrtChkCurSlip, PrtItemPrint for post receipt feature.
* Jul-12-93 : 01.00.12 :  R.Itoh    : add PrtDispPrint()
* Aug-23-93 : 01.00.13 :  R.Itoh    : add CLASS_TRAILER4
* Oct-29-93 : 02.00.02 :  K.You     : del. PrtDispPrint()
* Apr-08-94 : 00.00.04 :  Yoshiko.J : add CLASS_EPT_TRAILER, CLASS_EPT_STUB,
*                                   : CLASS_ERROR
* Apr-08-94 : 00.00.05 :  Yoshiko.J : E-042 corrected
* May-23-94 : 02.05.00 :  Yoshiko.J : Q-013 corr.(mod PrtItemPrint())
* May-25-94 : 02.05.00 :  Yoshiko.J : mod.PrtItemPrint() case of CLASS_EPT_TRAILER)
* May-25-94 : 02.05.00 :  K.You     : mod.PrtEPTStub_TH() for duplicate print. 
* Jan-26-95 : 03.00.00 :  M.Ozawa   : recover PrtDispPrint()
* May-09-95 : 03.00.00 : T.Nakahata : add Parking Receipt feature.
* Jul-12-95 : 03.00.04 : T.Nakahata : fix PrtChkCurSlip() (CURQUAL_PARKING) 
* Jul-26-95 : 03.00.04 : T.Nakahata : print customer name on K/P
* Aug-28-95 : 03.00.05 : T.Nakahata : INVALID TRANS. on journal at service ttl
* Aug-12-99 : 03.05.00 : K.Iwata    : marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-09-99 : 01.00.00 : hrkato     : Saratoga
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
/**------- M S - C --------**/
#include	<tchar.h>
#include<string.h>
/**------- 2170 local------**/
#include<ecr.h>
#include<uie.h>
#include<paraequ.h>
#include<para.h>
#include<regstrct.h>
#include<transact.h>
#include<csstbpar.h>
#include<pmg.h>
#include<rfl.h>
#include<prt.h>
#include<shr.h>
#include "prtcom.h"
#include<dfl.h>
#include"prtrin.h"
#include"prtshr.h"
#include"prtdfl.h"
#include"prrcolm_.h"
#include"trans.h"
#include"uireg.h"
#include"..\..\Uifreg\uiregloc.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
extern VOID PrtSoftCHK(UCHAR uchMinorClass);

CONST TCHAR FARCONST  aszPrtKP1Space[]= _T("%s");

extern CONST TCHAR FARCONST  aszPrtTHMnemMnem[];
extern CONST TCHAR FARCONST  aszPrtSPMnemMnem[];
extern CONST TCHAR FARCONST  aszPrtEJMnemMnem[];
                                                 /* mnem. and mnem. */


/*
*===========================================================================
** Format  : VOID  Prt24Mnemonic_TH(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION    *pTran  -Transaction Information address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints PreVoid line. ( thermal )
*===========================================================================
*/
VOID  PrtGivenMnemonic_TH(TRANINFORMATION   *pTran, TCHAR *asz24Mnem, BOOL bDouble)
{
	TCHAR   aszSDblMn[PARA_CHAR24_LEN * 2 + 1] = { 0 };

    PrtTHHead(pTran);                             /* print header if necessary */

	if (bDouble) {
        PrtSDouble(aszSDblMn, TCHARSIZEOF(aszSDblMn), asz24Mnem);
	} else {
		_tcsncpy (aszSDblMn, asz24Mnem, PARA_CHAR24_LEN);   /* get 24char mnem. */
		aszSDblMn[PARA_CHAR24_LEN] = 0;      // ensure that string is terminated.
	}

    PrtPrint((USHORT)PMG_PRT_RECEIPT, aszSDblMn, tcharlen(aszSDblMn)); /* ### Mod (2171 for Win32) V1.0 */

    PrtFeed(PMG_PRT_RECEIPT, 1);                  /* 1 line feed */
}

VOID  Prt24Mnemonic_TH(TRANINFORMATION   *pTran, USHORT usP24MnemonicAdr, BOOL bDouble)
{
	TCHAR   aszSDblMn[PARA_CHAR24_LEN * 2 + 1] = { 0 };

    PrtTHHead(pTran);                             /* print header if necessary */

	if (bDouble) {
		TCHAR   asz24Mnem[PARA_CHAR24_LEN + 1] = { 0 };

		PrtGet24Mnem(asz24Mnem, usP24MnemonicAdr);                   /* get 24char mnem. */
        asz24Mnem[21] = '\0';            
        PrtSDouble(aszSDblMn, TCHARSIZEOF(aszSDblMn), asz24Mnem);
	} else {
		PrtGet24Mnem(aszSDblMn, usP24MnemonicAdr);   /* get 24char mnem. */
	}

    PrtPrint((USHORT)PMG_PRT_RECEIPT, aszSDblMn, tcharlen(aszSDblMn)); /* ### Mod (2171 for Win32) V1.0 */

	if (bDouble) {
		PrtFeed(PMG_PRT_RECEIPT, 1);                  /* 1 line feed */
	}
}

/*
*===========================================================================
** Format  : VOID  Prt24Mnemonic_EJ(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints PreVoid line. ( electric journal )
*===========================================================================
*/
VOID  PrtGivenMnemonic_EJ(TRANINFORMATION *pTran, TCHAR *aszMnemonics)
{
    /* line printed only once on journal */
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY ) {
        return;
    }

    PrtPrint((USHORT)PMG_PRT_JOURNAL, aszMnemonics, tcharlen(aszMnemonics)); /* ### Mod (2171 for Win32) V1.0 */
}

VOID  Prt24Mnemonic_EJ(TRANINFORMATION *pTran, USHORT usP24MnemonicAdr)
{
	TCHAR   aszMnemonics[PARA_CHAR24_LEN + 1] = { 0 };

    PrtGet24Mnem(aszMnemonics, usP24MnemonicAdr);   /* get 24char mnem. */

    /* line printed only once on journal */
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY ) {
        return;
    }

    PrtPrint((USHORT)PMG_PRT_JOURNAL, aszMnemonics, tcharlen(aszMnemonics)); /* ### Mod (2171 for Win32) V1.0 */
}

/*
*===========================================================================
** Format  : VOID  Prt24Mnemonic_SP(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints PreVoid line. ( Slip )
*===========================================================================
*/
VOID PrtGivenMnemonic_SP(TRANINFORMATION *pTran, TCHAR *aszMnemonics)
{
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */

    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY ) {
        return ;                       /* tray total */
    }                                  /* line printed only once on slip */

    /* -- set preselect void mnemonic -- */
    usSlipLine ++;

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);
    PrtPrint((USHORT)PMG_PRT_SLIP, aszMnemonics, (USHORT)_tcslen(aszMnemonics));

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
}

VOID Prt24Mnemonic_SP(TRANINFORMATION *pTran, USHORT usP24MnemonicAdr)
{
	TCHAR   aszMnemonics[PARA_CHAR24_LEN + 1] = { 0 };
    USHORT  usSlipLine = 0;            /* number of lines to be printed */

    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY ) {
        return ;                       /* tray total */
    }                                  /* line printed only once on slip */

    /* -- set preselect void mnemonic -- */
    PrtGet24Mnem(aszMnemonics, usP24MnemonicAdr);
	PrtGivenMnemonic_SP (pTran, aszMnemonics);
}


/*
*===========================================================================
** Format  : VOID  PrtItemPrint(TRANINFORMATION *pTran,ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints  Header & Trailer.
*===========================================================================
*/
VOID   PrtItemPrint(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
	SHORT		sRet;

    switch (pItem->uchMinorClass) {
    case CLASS_START:                           /* Print Start */
    case CLASS_LOANPICKSTART:                   /* Print Start, Saratoga */
        /* -- check "kp only"(prints to kp or take to kitchen) -- */
        if ( !(fsPrtStatus & PRT_KPONLY) ) {
			if (pTran->TranGCFQual.fsGCFStatus2 & PRT_DBL_POST_RECEIPT)
			{
				// When printing a post receipt, the double sided
				// printing needs to be handled diffrently   ***PDINU
				PrtTransactionBegin(PRT_POSTPRINTING, pTran->TranGCFQual.usGuestNo);
			}else
			{
				//Because this is the first thing printed, if we are using double sided
				//printing, we need to signal that this is the beginning of a transaction;
				PrtTransactionBegin(PRT_REGULARPRINTING, pTran->TranGCFQual.usGuestNo);
			}

            /*  set up no-print, compulsory print           */
            /* "fsPrtCompul" & "fsPrtNoPrtMask" initialized */
            /*  in TPM(PrcTran_.c)                          */
            if (fsPrtStatus & PRT_SPLIT_PRINT) {
                if (auchPrtSeatCtl[0]) {    /* at consolidation seat# print */
                    if (auchPrtSeatCtl[1]) {
                        break;  /* not initialize slip at consolidation print */
                    }               /* initialize slip each seat no print */
                }
            }

            if ((fsPrtStatus & PRT_SPLIT_PRINT) && (auchPrtSeatCtl[1])) {              /* after secondary print */
                ;   /* not set compulsory print status */
            } else {
                fsPrtCompul  = pTran->TranCurQual.fbCompPrint;
                fsPrtNoPrtMask = ~(pTran->TranCurQual.fbNoPrint);
            }

            if ( !(fsPrtStatus & PRT_DEMAND) ) {    /* not print on demand */
                if (pItem->uchMinorClass == CLASS_LOANPICKSTART) {  /* Oct/26/2000 */
                    PrtSlipInitialLP();
                } else {
                    PrtSlipInitial();                   /* Slip initialize */
                }
                if ( !(fsPrtStatus & PRT_SPLIT_PRINT) ) {
                    if ( (fsPrtCompul & fsPrtNoPrtMask) & PRT_SLIP ) {    
                        PrtChkCurSlip(pTran);          /* get current slip line and feed */
                    }
                }
            }
        }

        /* -- kitchen printer management -- */
        if (fsPrtStatus & PRT_REQKITCHEN) {
            PrtKPInit();                  /* kp initialize */
            PrtKPOperatorName( pTran);      /* print operator name on kp R3.1 */
            PrtKPHeader(pTran);           /* set kp header */

            /* --- print customer name on KP, if name is specified --- */
            PrtKPCustomerName( pTran->TranGCFQual.aszName );
            PrtKPTrailer(pTran, pTran->TranCurQual.ulStoreregNo);    /* set kp trailer */
        }

//		if (pItem->uchMinorClass == CLASS_START) {
//			PrtForceEJInit(pTran);                 /* initialize E/J print buffer if printing to EJ */
//		} else {
//			PrtEJInit(pTran);                 /* initialize E/J print buffer if printing to EJ */
//		}
		if (pTran->TranCurQual.usConsNo == 0) {
			PrtForceEJInit(0);                       /* initialize E/J print buffer */
		} else {
			PrtForceEJInit(pTran);                   /* initialize E/J print buffer */
		}
        break;

    case CLASS_RESTART:                         /* Print ReStart */
        /* -- restore flags for shared printer -- */
        fbPrtAbortStatus = fbPrtAbortSav;
        fbPrtAltrStatus = fbPrtAltrSav;

        break;

    case CLASS_PROMOTION:                 /* promotional header */
        if (fsPrtStatus & PRT_SPLIT_PRINT) {
            break;
        }

        PrtPromotion(pTran, pItem->fsPrintStatus);       /* print promotional header */
        break;

    case CLASS_MONEY_HEADER:                /* Money Header,    Saratoga */
        if (fsPrtStatus & PRT_SPLIT_PRINT) {
            break;
        }
        PrtMoneyHeader(pTran, pItem->fsPrintStatus);
        break;

    case CLASS_DOUBLEHEADER:              /* double receipt */
        PrtDoubleHeader(pTran);
        break;

    case CLASS_PRINT_TRAIN:               /* training line */
        if (fsPrtStatus & PRT_SPLIT_PRINT) {
            break;
        }

        PrtTraining(pTran, pItem->fsPrintStatus);
        break;

    case CLASS_PRINT_PVOID:               /* preselect void line */
        if (fsPrtStatus & PRT_SPLIT_PRINT) {
            break;
        }

        PrtPVoid(pTran, pItem->fsPrintStatus);
        break;

    case CLASS_PRINT_TRETURN:               /* return transaction line */
        if (fsPrtStatus & PRT_SPLIT_PRINT) {
            break;
        }

        PrtPReturn(pTran, pItem->fsPrintStatus);
        break;

    case CLASS_TRAILER1:                    /* trailer & header (normal) */
    case CLASS_TRAILER_MONEY:               /* Saratoga */
        if (fsPrtStatus & PRT_REQKITCHEN) {
            PrtKPEnd();
        }
        if ((fsPrtStatus & PRT_SPLIT_PRINT) && 
            ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK)) {

            PrtSoftProm(pItem);   
        }
        PrtTrail1( pTran, (ITEMPRINT *)pItem);
        break;

    case CLASS_TRAILER2:                  /* trailer & header (ticket) */
        PrtTrail2(pTran);
        PrtShrEnd();
		if (pTran->TranGCFQual.fsGCFStatus2 & PRT_DBL_POST_RECEIPT)
		{
			// When printing a post receipt, the double sided
			// printing needs to be handled diffrently   ***PDINU
			PrtTransactionEnd(PRT_POSTPRINTING, pTran->TranGCFQual.usGuestNo);
		}else
		{
			//Because this is the first thing printed, if we are using double sided
			//printing, we need to signal that this is the beginning of a transaction;
			PrtTransactionEnd(PRT_REGULARPRINTING, pTran->TranGCFQual.usGuestNo);
		}
        break;

    case CLASS_TRAILER3:                  /* trailer & header (newcheck,
                                                                reorder) */
        if (fsPrtStatus & PRT_REQKITCHEN) {
            PrtKPEnd();
        }
        PrtTrail3(pTran, (ITEMPRINT *)pItem);
        break;

    case CLASS_TRAILER4:                  /* soft check */ 
        if (fsPrtStatus & PRT_REQKITCHEN) {
            PrtKPEnd();
        }

        PrtSoftProm(pItem);   
        PrtTrail1( pTran, pItem);
        break;

	case CLASS_SPLITA_EPT:
        if (fsPrtStatus & PRT_REQKITCHEN) {
            PrtKPEnd();
        }
        if ((fsPrtStatus & PRT_SPLIT_PRINT) && 
            ((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK)) {

            PrtSoftProm_EPT(pItem);   
        }
        PrtTrail1( pTran, pItem);
		break;

	case CLASS_RETURNS_TRAILER:                         // Returns logo message
    case CLASS_EPT_TRAILER:                             /* EPT Logo Message */ 
        if (fsPrtStatus & PRT_REQKITCHEN) {             /* Q-013 corr. 5/23 */
            PrtKPEnd();
        }
	    if ( fsPrtPrintPort & PRT_RECEIPT ) {       /* thermal print, 09/11/01 */
    	    PrtFeed(PMG_PRT_RECEIPT, 1);                    /* add '94 5/25     */
    	}
		{
			PARATENDERKEYINFO pData = {0};

			//Obtain the address of the tender key that was pressed and
			//then obtain the tender4 key info. With this info we will
			//check to see if the tender key was of prepaid type and if
			//it is then we will check MDC 361 Address A, gift card
			//signature line printing.
			pData.uchAddress = UifRegData.regtender.uchMinorClass;
			ParaTendInfoRead(&pData);
			if(pData.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_PREPAID || pData.TenderKeyInfo.usTranType == TENDERKEY_TRANTYPE_FREEDOMPAY)
			{
				if(CliParaMDCCheck(MDC_ENHEPT_ADR, ODD_MDC_BIT3) && (TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT) == 0)
					PrtSoftProm(pItem);
			}
			else if ((TrnInformation.TranCurQual.fsCurStatus2 & PRT_GIFT_RECEIPT) == 0)
				PrtSoftProm(pItem);
			PrtTrail1( pTran, pItem);
		}
        break;

    case CLASS_TRAYTRAILER:               /* trailer & header (tray total) */
        PrtTrayTrail(pTran, pItem);
        break;


    case CLASS_CHECKPAID:                 /* multi check */
        PrtChkPaid(pItem);
        break;

    case CLASS_POST_RECPT:                /* post receipt mnem. */
        PrtPostRct(pTran, pItem);
        break;

    case CLASS_PARK_RECPT:                  /* parking receipt, R3.0 */
        PrtParkReceipt( pTran, pItem );
        break;

    case CLASS_CP_GUSLINE:                      /* charge posting guest line # */
        PrtCPGusLine(pTran, pItem);
        break;

    case CLASS_CP_ERROR:                        /* func, error code for charge posting */
        PrtCPFuncErrCode_EJ(pItem);
        break;

    case CLASS_ERROR:                           /* EPT error                */
        PrtEPTError(pItem);
        break;

    case CLASS_EPT_STUB:                        /* EPT duplicate(stub) amt  */
    case CLASS_CP_STUB:                         /* CP duplicate(stub) amt  */
        PrtEPTStub(pTran, pItem);
        break;
	case CLASS_EPT_STUB_NOACCT:
		PrtEPTStubNoAcct(pTran, pItem);
		break;

    case CLASS_NUMBER: /* 2172 */
        PrtNumber(pTran, pItem);
        break;
        
    case CLASS_AGELOG: /* 2172 */
        PrtAgeLog(pTran, pItem);
        break;

    case CLASS_RSPTEXT:                         /* EPT response message,    Saratoga */
        PrtCPRespText( pTran, pItem);
        break;

    case CLASS_FOOD_CREDIT:                     /* food stamp credit,   Saratoga */
        PrtFSCredit(pTran, pItem);
        break;

    case CLASS_CHECK_ENDORSE:               /* DTREE#2 Check Endorsement,   Dec/18/2000, 21RFC05402 */
        PrtEndorse( pTran, pItem);
        break;

	case CLASS_ORDERDEC:
		{
			ITEMSALES	OrderDec = {0};

			sRet = PrtOrderDecSearch(pTran, &OrderDec);
			if(sRet == SUCCESS)
			{
				PrtOrderDec(pTran,&OrderDec);
			}
		}
		break;

	case CLASS_BALANCE:
		PrtTHCPRespTextGiftCard(pTran, pItem);
		break;

    default:
        break;

    }
}

/*
*===========================================================================
** Format  : VOID   PrtChkCurSlip(TRANINFORMATION  *pTran);
*                                     
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTRANSOPEN    *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets slip page No. and line No. 
*===========================================================================
*/
VOID   PrtChkCurSlip(TRANINFORMATION  *pTran)
{

/*** bug fixed (95-7-12) ***
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_POSTRECT ) {
*** bug fixed (95-7-12) ***/

    if ( pTran->TranCurQual.flPrintStatus & ( CURQUAL_POSTRECT | CURQUAL_PARKING )) {
        usPrtSlipPageLine = 0;
        return;
    }

    if ((pTran->TranGCFQual.uchPageNo == 0) && (pTran->TranGCFQual.uchSlipLine == 0)) {
		PARASLIPFEEDCTL  FeedCtl = { 0 };

        FeedCtl.uchMajorClass = CLASS_PARASLIPFEEDCTL;
        FeedCtl.uchAddress = SLIP_NO1ST_ADR;   
        CliParaRead(&FeedCtl);    
        
        /* set the initial page No. and line No. */         
        usPrtSlipPageLine = 0 * PRT_SLIP_MAX_LINES + FeedCtl.uchFeedCtl;    /* start from 0 page and pre-feeded line */
        
        /* feed before print start */        
        PrtFeed(PMG_PRT_SLIP, (USHORT)FeedCtl.uchFeedCtl);
    } else {
        /* set the current page No. and line No. */    
        usPrtSlipPageLine = pTran->TranGCFQual.uchPageNo * PRT_SLIP_MAX_LINES + pTran->TranGCFQual.uchSlipLine;        
        PrtFeed(PMG_PRT_SLIP, (USHORT)pTran->TranGCFQual.uchSlipLine);
    }
}


/*
*===========================================================================
** Format  : VOID  PrtTraining(TRANINFORMATION *pTran, ITEMPRINT *pItem);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints training line. 
*            print INVALID TRANSACTION line on receipt and in journal
*===========================================================================
*/
VOID  PrtTraining(TRANINFORMATION *pTran, SHORT fsPrintStatus)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        Prt24Mnemonic_SP(pTran, CH24_TRNGHED_ADR);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        Prt24Mnemonic_TH(pTran, CH24_TRNGHED_ADR, FALSE);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
        Prt24Mnemonic_EJ(pTran, CH24_TRNGHED_ADR);
    } else if ( fsPrtStatus & PRT_SERV_JOURNAL ) {
		SHORT   fsPortSave = fsPrtPrintPort;

        fsPrtPrintPort = PRT_JOURNAL;
        Prt24Mnemonic_EJ( pTran, CH24_TRNGHED_ADR );
        fsPrtPrintPort = fsPortSave;
    }
}

/*
*===========================================================================
** Format  : VOID  PrtPVoid(TRANINFORMATION *pTran, ITEMPRINT *pItem);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints PreVoid line. 
*===========================================================================
*/
VOID  PrtPVoid(TRANINFORMATION *pTran, SHORT fsPrintStatus)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        Prt24Mnemonic_SP(pTran, CH24_PVOID_ADR);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        Prt24Mnemonic_TH(pTran, CH24_PVOID_ADR, FALSE);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
        Prt24Mnemonic_EJ(pTran, CH24_PVOID_ADR);
    }
}

VOID  PrtPReturn(TRANINFORMATION *pTran, SHORT fsPrintStatus)
{
	USHORT usReturnType = CH24_PRETURN_ADR;
	SHORT  i;
	TCHAR  pcsUniqueIdentifier[sizeof(pTran->TranGCFQual.uchUniqueIdentifierReturn)/sizeof(pTran->TranGCFQual.uchUniqueIdentifierReturn[0])+1];

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(fsPrintStatus);

	for (i = 0; i < sizeof(pTran->TranGCFQual.uchUniqueIdentifierReturn)/sizeof(pTran->TranGCFQual.uchUniqueIdentifierReturn[0]); i++) {
		pcsUniqueIdentifier[i] = pTran->TranGCFQual.uchUniqueIdentifierReturn[i] + _T('0');
	}
	pcsUniqueIdentifier[i] = 0;

	usReturnType = CH24_PRETURN_ADR;
	if ((pTran->ulCustomerSettingsFlags & SYSCONFIG_CUSTOMER_ENABLE_AMTRAK) == 0) {
		// if we are using Preselect Returns then TranGCFQual.usReturnType will indicate the type of return
		// otherwise we will just use what ever type has been used with Cursor Return.
		switch ((pTran->TranGCFQual.usReturnType & 0x00ff)) {
			case 0:
				// return type not set so no override to do.
				break;
			case 1:
				pTran->TranModeQual.usReturnType |= 0x100;    // set that RETURNS_MODIFIER_1 used in this transaction
				break;
			case 2:
				pTran->TranModeQual.usReturnType |= 0x200;    // set that RETURNS_MODIFIER_2 used in this transaction
				break;
			case 3:
				pTran->TranModeQual.usReturnType |= 0x400;    // set that RETURNS_MODIFIER_3 used in this transaction
				break;
			default:
				break;
		}
		if (pTran->TranModeQual.usReturnType & 0x100)    // check to see if RETURNS_MODIFIER_1 has been used in this transaction
			usReturnType = CH24_PRETURN_ADR;
		else if (pTran->TranModeQual.usReturnType & 0x200)    // check to see if RETURNS_MODIFIER_2 has been used in this transaction
			usReturnType = CH24_PEXCHANGE_ADR;
		else if (pTran->TranModeQual.usReturnType & 0x400)    // check to see if RETURNS_MODIFIER_3 has been used in this transaction
			usReturnType = CH24_PREFUND_ADR;
	}

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        Prt24Mnemonic_SP(pTran, usReturnType);
		if (CliParaMDCCheckField(MDC_SLIPVAL_ADR, MDC_PARAM_BIT_A)) {
			Prt24Mnemonic_SP(pTran, CH24_ORGTRAN_ADR);
			PrtGivenMnemonic_SP(pTran, pcsUniqueIdentifier);
		}
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        Prt24Mnemonic_TH(pTran, usReturnType, TRUE);
		if (CliParaMDCCheckField(MDC_SLIPVAL_ADR, MDC_PARAM_BIT_A)) {
			Prt24Mnemonic_TH(pTran, CH24_ORGTRAN_ADR, FALSE);
			PrtGivenMnemonic_TH(pTran, pcsUniqueIdentifier, FALSE);
		}
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
        Prt24Mnemonic_EJ(pTran, usReturnType);
		if (CliParaMDCCheckField(MDC_SLIPVAL_ADR, MDC_PARAM_BIT_A)) {
			Prt24Mnemonic_EJ(pTran, CH24_ORGTRAN_ADR);
			PrtGivenMnemonic_EJ(pTran, pcsUniqueIdentifier);
		}
    }
}

/*
*===========================================================================
** Format  : VOID  PrtMoneyHeader(TRANINFORMATION *pTran, ITEMPRINT *pItem)
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints Money Header.                 Saratoga 
*===========================================================================
*/
VOID    PrtMoneyHeader(TRANINFORMATION *pTran, SHORT fsPrintStatus)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(fsPrintStatus);

    if (fsPrtPrintPort & PRT_SLIP) {
        Prt24Mnemonic_SP(pTran, CH24_MONEY_ADR);
    }
    if (fsPrtPrintPort & PRT_RECEIPT) {
        Prt24Mnemonic_TH(pTran, CH24_MONEY_ADR, FALSE);
    }
    if (fsPrtPrintPort & PRT_JOURNAL) {
        Prt24Mnemonic_EJ(pTran, CH24_MONEY_ADR);
    }
}


/*
*===========================================================================
** Format  : VOID  PrtPostRct(TRANINFORMATION *pTran, ITEMPRINT *pItem);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints post receipt line. 
*===========================================================================
*/
VOID  PrtPostRct(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print */
        PrtPostRct_SP(pTran);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {           /* thermal print */
        PrtPostRct_TH(pTran);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {           /* electric journal */
        PrtPostRct_EJ(pTran);
    }

}

/*
*===========================================================================
** Format  : VOID  PrtPostRct_TH(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints post receipt mnem. line. ( thermal )
*===========================================================================
*/
VOID  PrtPostRct_TH(TRANINFORMATION *pTran)
{
	TCHAR   aszMnemonics[PARA_CHAR24_LEN + 1] = { 0 };

    PrtTHHead(pTran);                             /* print header if necessary */

    PrtGet24Mnem(aszMnemonics, CH24_POSTR_ADR);   /* get 24 char mnem. */

    PrtPrint((USHORT)PMG_PRT_RECEIPT, aszMnemonics, (USHORT)_tcslen(aszMnemonics)); /* ### Mod (2171 for Win32) V1.0 */

}

/*
*===========================================================================
** Format  : VOID  PrtPostRct_EJ(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints post receipt mnem. line. ( elctric journal )
*===========================================================================
*/
VOID  PrtPostRct_EJ(TRANINFORMATION *pTran)
{

	TCHAR   aszMnemonics[PARA_CHAR24_LEN + 1] = { 0 };
                                                
    PrtGet24Mnem(aszMnemonics, CH24_POSTR_ADR); /* get 24 char mnem. */

    /* post receipt line printed only once on journal */
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY ) {
        return;
    }

    PrtPrint((USHORT)PMG_PRT_JOURNAL, aszMnemonics, (USHORT)_tcslen(aszMnemonics)); /* ### Mod (2171 for Win32) V1.0 */

}


/*
*===========================================================================
** Format  : VOID  PrtPostRct_SP(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints post receit mnem. line. ( Slip )
*===========================================================================
*/
VOID PrtPostRct_SP(TRANINFORMATION *pTran)
{
	TCHAR   aszMnemonics[PARA_CHAR24_LEN + 1] = { 0 };
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */

    /* -- set post receipt mnemonic -- */
    PrtGet24Mnem(aszMnemonics, CH24_POSTR_ADR);
    usSlipLine ++;

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);
    PrtPrint((USHORT)PMG_PRT_SLIP, aszMnemonics, (USHORT)_tcslen(aszMnemonics));

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

}

/*
*===========================================================================
** Format  : VOID  PrtCPGusLine(TRANINFORMATION *pTran, ITEMPRINT *pItem);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*          : ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints charge posting guest line #. 
*===========================================================================
*/
VOID  PrtCPGusLine(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);
                                            
    if (fsPrtPrintPort & PRT_RECEIPT) {         /* receipt print */
        PrtCPGusLine_TH(pTran, pItem);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtCPGusLine_TH(TRANINFORMATION *pTran, ITEMPRINT *pItem);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*          : ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints charge posting guest line #. ( thermal )
*===========================================================================
*/
VOID  PrtCPGusLine_TH(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{

    PrtTHHead(pTran);                      /* print header if necessary */

    PrtPrint((USHORT)PMG_PRT_RECEIPT, pItem->aszCPText[0], (USHORT)_tcslen(pItem->aszCPText[0])); /* ### Mod (2171 for Win32) V1.0 */

}

/*
*===========================================================================
** Format  : VOID  PrtCPFuncErrCode_EJ(ITEMPRINT *pItem);
*                                    
*   Input  : ITEMPRINT       *pItem    -item information
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints "function and error code number" .
*
*            Generated by an ITEMPRINT struct containing the following:
*               ItemPrintData.uchMajorClass = CLASS_ITEMPRINT;
*               ItemPrintData.uchMinorClass = CLASS_CP_ERROR;
*
*             See function ItemTendCPPrintErr() and see use of CpmConvertError()
*             to generate the sErrorCode value.
*===========================================================================
*/
VOID  PrtCPFuncErrCode_EJ(ITEMPRINT *pItem)
{
	CONST TCHAR *aszPrtEJCPFuncErrCode = _T("   F-CODE%5d E-CODE%3d");  /* function,error code for charge posting */

	// SR 703, added this Init call this Init so that we put the initial data into the EJ buffer before 
	// we put the error code. JHHJ 9-05-05
	PrtEJInit(&TrnInformation);                       /* initialize E/J print buffer */
	PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJCPFuncErrCode, pItem->usFuncCode, pItem->sErrorCode);
}
/*
*===========================================================================
** Format  : VOID  PrtEPTError(ITEMPRINT  *pItem);
*
*   Input  : TEMPRINT  *pItem
*             
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints EPT Error. ( Journal )
*===========================================================================
*/
VOID  PrtEPTError(ITEMPRINT  *pItem)
{
    PrtTHEPTError(pItem);
}
/*
*===========================================================================
** Format  : VOID  PrtEPTStub( TRANINFORMATION *pTran, ITEMPRINT  *pItem );
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*             
*   Output : none
*   InOut  : none
*            
** Return  : 
*            
** Synopsis: 
*===========================================================================
*/
VOID    PrtEPTStub( TRANINFORMATION *pTran, ITEMPRINT  *pItem )
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);
    PrtEPTStub_TH( pTran, pItem );
}

/*
*===========================================================================
** Format  : VOID  PrtEPTStub_TH( TRANINFORMATION *pTran, ITEMPRINT  *pItem );
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*             
*   Output : none
*   InOut  : none
*            
** Return  : 
*            
** Synopsis: 
*===========================================================================
*/
VOID    PrtEPTStub_TH( TRANINFORMATION *pTran, ITEMPRINT *pItem )
{
    USHORT  i;

    PrtTHHead(pTran);                                   /* E-042 corr. 4/21 */
    PrtTHGuest(pTran->TranGCFQual.usGuestNo, pTran->TranGCFQual.uchCdv);
    PrtTHOffTend(pItem->fbModifier);                    /* CP off line      */
    PrtTHVoid(pItem->fbModifier, 0);                       /* void line        */
    PrtTHZeroAmtMnem( TRN_AMTTL_ADR, pItem->lAmount );  /* total amount     */
    PrtTHOffline( pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );
    PrtTHCPRoomCharge(pItem->aszRoomNo, pItem->aszGuestID); /* for charge posting */
    PrtTHNumber(pItem->aszNumber[0]);	//US Customs    /* Acct number line *//
    for (i = 0; i < NUM_CPRSPCO; i++) {
        PrtTHCPRspMsgText(pItem->aszCPText[i]);         /* for charge posting */
    }

	if ((TrnInformation.TranCurQual.fbNoPrint & CURQUAL_NO_EPT_LOGO_SIG) == 0) {
		PrtSoftCHK(SOFTCHK_EPT1_ADR);
	}
}

/*
*===========================================================================
** Format  : VOID  PrtEPTStubNoAcct( TRANINFORMATION *pTran, ITEMPRINT  *pItem );
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*             
*   Output : none
*   InOut  : none
*            
** Return  : 
*            
** Synopsis: 
*===========================================================================
*/
VOID    PrtEPTStubNoAcct( TRANINFORMATION *pTran, ITEMPRINT *pItem )
{
    USHORT  i;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    PrtTHHead(pTran);                                   /* E-042 corr. 4/21 */
    PrtTHGuest(pTran->TranGCFQual.usGuestNo, pTran->TranGCFQual.uchCdv);
    PrtTHOffTend(pItem->fbModifier);                    /* CP off line      */
    PrtTHVoid(pItem->fbModifier, 0);                       /* void line        */
    PrtTHZeroAmtMnem( TRN_AMTTL_ADR, pItem->lAmount );  /* total amount     */
    PrtTHCPRoomCharge(pItem->aszRoomNo, pItem->aszGuestID); /* for charge posting */
    for (i = 0; i < NUM_CPRSPCO; i++) {
        PrtTHCPRspMsgText(pItem->aszCPText[i]);         /* for charge posting */
    }

	if ((TrnInformation.TranCurQual.fbNoPrint & CURQUAL_NO_EPT_LOGO_SIG) == 0) {
		PrtSoftCHK(SOFTCHK_EPT1_ADR);
	}
}

/*
*===========================================================================
** Format  : VOID  PrtDispPrint(TRANINFORMATION *pTran,ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints  Header & Trailer.
*===========================================================================
*/
VOID   PrtDispPrint(TRANINFORMATION  *pTran, ITEMPRINT  *pItem)
{
    switch (pItem->uchMinorClass) {

    case CLASS_START:                           /* Print Start */

        /* -- set display initial data -- */
        PrtDflInit(pTran);
        break;

	case CLASS_RETURNS_TRAILER:           // trailer for returns transactions
    case CLASS_TRAILER1:                  /* trailer & header (normal) */
    case CLASS_TRAILER4:                  /* promotional message for soft check */ 
    case CLASS_EPT_TRAILER:               /* EPT Logo Message */ 

        /* -- set destination CRT -- */
        PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
        PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

        /* -- set display data in the buffer -- */ 
        PrtDflIType(0, DFL_END); 

        switch (pItem->uchMinorClass) {
		case CLASS_RETURNS_TRAILER:                         // Returns logo message
        case CLASS_TRAILER1:
        case CLASS_EPT_TRAILER:
            if (((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_CASHIER) ||      /* not Guest Check operation */
                (((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_ADDCHECK)
                 && !(pTran->TranCurQual.fsCurStatus & CURQUAL_CANCEL)) ||                      /* add check and not cancel operation */
                (((pTran->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) == CURQUAL_NEWCHECK)
                 && (pTran->TranCurQual.fsCurStatus & CURQUAL_CANCEL))) {                       /* new check and  cancel operation */

                PrtDflIf.Dfl.DflHead.uchFrameType = PRT_DFL_END_FRAME;
            } else {
                PrtDflIf.Dfl.DflHead.uchFrameType = PRT_DFL_SERVICE_END_FRAME;
            }
            break;

        default:
            PrtDflIf.Dfl.DflHead.uchFrameType = PRT_DFL_SERVICE_END_FRAME;
            break;
        }

        /* -- send display data -- */
        PrtDflSend();

    case CLASS_NUMBER: /* 2172 */
        PrtDflDispNumber(pTran, pItem);
        break;
        
    case CLASS_AGELOG: /* 2172 */
        PrtDflAgeLog(pTran, pItem);
        break;

    default:
        break;

    }

}

/*
*===========================================================================
** Format  : VOID  PrtDispPrint(TRANINFORMATION *pTran,ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints  Header & Trailer.
*===========================================================================
*/
USHORT   PrtDispPrintForm(TRANINFORMATION  *pTran, ITEMPRINT  *pItem, TCHAR *puchBuffer)
{
    USHORT usLine;
    
    switch (pItem->uchMinorClass) {

    case CLASS_NUMBER: /* 2172 */
        usLine = PrtDflDispNumberForm(pTran, pItem, puchBuffer);
        break;
        
    case CLASS_AGELOG: /* 2172 */
        usLine = PrtDflAgeLogForm(pTran, pItem, puchBuffer);
        break;

    default:
        usLine = 0;
        break;

    }

    return usLine;
}

/*
*===========================================================================
** Format  : VOID PrtPostRct( TRANINFORMATION *pTran, ITEMPRINT *pItem )
*
*   Input  : TRANINFORMATION *pTran - transaction information
*            ITEMPRINT       *pItem - item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints parking receipt.
*===========================================================================
*/
VOID PrtParkReceipt( TRANINFORMATION *pTran, ITEMPRINT *pItem )
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */

    PrtPortion( pItem->fsPrintStatus );

    if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print */
        PrtParkReceipt_SP( pTran );
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {           /* thermal print */
        PrtParkReceipt_TH( pTran );
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {           /* electric journal */
        PrtParkReceipt_EJ( pTran );
    }
}

/*
*===========================================================================
** Format  : VOID PrtParkReceipt_TH( TRANINFORMATION *pTran )
*
*   Input  : TRANINFORMATION *pTran - transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints parking receipt on journal.
*===========================================================================
*/
VOID  PrtParkReceipt_TH( TRANINFORMATION *pTran )
{
	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
    USHORT  usMnemLen;

    PrtTHHead( pTran );

	RflGetTranMnem( szMnemonic, TRN_PARKING_ADR );
    usMnemLen = tcharlen( szMnemonic );

    PrtPrint( PMG_PRT_RECEIPT, szMnemonic, usMnemLen );
}

/*
*===========================================================================
** Format  : VOID PrtParkReceipt_EJ( TRANINFORMATION *pTran )
*
*   Input  : TRANINFORMATION *pTran - transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints parking receipt on electric journal.
*===========================================================================
*/
VOID PrtParkReceipt_EJ( TRANINFORMATION *pTran )
{
	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
    USHORT  usMnemLen;
                                                
	RflGetTranMnem( szMnemonic, TRN_PARKING_ADR );
    usMnemLen = tcharlen( szMnemonic );

    /* --- parking receipt line printed only once on journal --- */

    if ( ! ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAY )) {

        PrtPrint( PMG_PRT_JOURNAL, szMnemonic, usMnemLen );
    }
}

/*
*===========================================================================
** Format  : VOID PrtParkReceipt_SP( TRANINFORMATION *pTran )
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints parking receipt on slip.
*===========================================================================
*/
VOID PrtParkReceipt_SP( TRANINFORMATION *pTran )
{
	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
    USHORT  usMnemLen;
    USHORT  usSlipLine = 0;             /* number of lines to be printed */
    USHORT  usSaveLine;                 /* save slip lines to be added */

    /* --- check if paper change is necessary or not --- */ 

    usSlipLine++;
    usSaveLine = PrtCheckLine( usSlipLine, pTran );

	RflGetTranMnem( szMnemonic, TRN_PARKING_ADR );
    usMnemLen = tcharlen( szMnemonic );
/*  --- fix a glitch (05/15/2001)
    PmgPrint( PMG_PRT_SLIP, szMnemonic, usMnemLen ); */
    PrtPrint( PMG_PRT_SLIP, szMnemonic, usMnemLen );

    /* --- update current line No. --- */

    usPrtSlipPageLine += ( usSlipLine + usSaveLine );
}

/*
*===========================================================================
** Format  : VOID  PrtNumber(TRANINFORMATION *pTran, ITEMPRINT *pItem);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints number line of random weight label, 2172. 
*===========================================================================
*/
VOID  PrtNumber(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print */
        PrtNumber_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {           /* thermal print */
        PrtNumber_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {           /* electric journal */
        PrtNumber_EJ(pTran, pItem);
    }

}

/*
*===========================================================================
** Format  : VOID  PrtNumber_TH(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints number line. ( thermal )
*===========================================================================
*/
VOID  PrtNumber_TH(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
	int numCounter; //US Customs
                                                
	RflGetTranMnem( szMnemonic, pItem->usFuncCode );
    
    PrtTHHead(pTran);                             /* print header if necessary */

	for(numCounter=0; numCounter<NUM_OF_NUMTYPE_ENT; numCounter++){ //cycle through all numbers US Customs cwunn
		if(!pItem->aszNumber[numCounter][0]){
			break; //stop looping on empty slot
		}
		PrtPrintf(PMG_PRT_RECEIPT, aszPrtTHMnemMnem, szMnemonic, pItem->aszNumber[numCounter]);
	}
}

/*
*===========================================================================
** Format  : VOID  PrtNumber_EJ(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints number line. ( elctric journal )
*===========================================================================
*/
VOID  PrtNumber_EJ(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{

	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
                                                
	RflGetTranMnem( szMnemonic, pItem->usFuncCode );
    
    PrtPrintf(PMG_PRT_JOURNAL, aszPrtEJMnemMnem, szMnemonic, pItem->aszNumber);
    
}


/*
*===========================================================================
** Format  : VOID  PrtNumber_SP(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints number line. ( Slip )
*===========================================================================
*/
VOID PrtNumber_SP(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
                                                
	RflGetTranMnem( szMnemonic, pItem->usFuncCode );
    
    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);
    PrtPrintf(PMG_PRT_SLIP, aszPrtSPMnemMnem, szMnemonic, pItem->aszNumber);
    usSlipLine ++;

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        

}

/*
*===========================================================================
** Format  : VOID  PrtDfDispNumber( TRANINFORMATION *pTran,
*                                     ITEMPRINT    *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays number of random weight label 2172.
*===========================================================================
*/
VOID PrtDflDispNumber( TRANINFORMATION *pTran, ITEMPRINT *pItem )
{
    USHORT  usLineNo;               /* number of lines to be displayed */
    USHORT  usOffset = 0;
    USHORT  i;
	TCHAR   aszDflBuff[6][PRT_DFL_LINE + 1] = { 0 };
	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
                                                
    if ( pItem->aszNumber[0] == '\0' ) {
        return;
    }

	RflGetTranMnem( szMnemonic, pItem->usFuncCode );

    /* -- set header -- */
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo );

    /* -- set item data -- */
    usLineNo += PrtDflRandomNumber( aszDflBuff[ usLineNo ], szMnemonic, pItem->aszNumber[0] );//US Customs

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- set display data in the buffer -- */ 
    PrtDflIType( usLineNo, DFL_CUSTNAME );

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
** Format  : VOID  PrtDfDispNumber( TRANINFORMATION *pTran,
*                                     ITEMPRINT    *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays number of random weight label 2172.
*===========================================================================
*/
USHORT PrtDflDispNumberForm( TRANINFORMATION *pTran, ITEMPRINT *pItem ,TCHAR *puchBuffer)
{
    USHORT  usLineNo=0, i;               /* number of lines to be displayed */
	TCHAR   aszDflBuff[6][PRT_DFL_LINE + 1] = { 0 };
	TCHAR   szMnemonic[PARA_TRANSMNEMO_LEN + 1] = { 0 };
                                                
    if ( pItem->aszNumber[0] == '\0' ) {
        return 0;
    }

	RflGetTranMnem( szMnemonic, pItem->usFuncCode );

    /* -- set header -- */
#if 0
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */

    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ],
                               pTran,
                               pTran->TranCurQual.ulStoreregNo );
#endif
    /* -- set item data -- */

    usLineNo += PrtDflRandomNumber( aszDflBuff[ usLineNo ], szMnemonic, pItem->aszNumber[0] );//US Customs

    for (i=0; i<usLineNo; i++) {

        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/*
*===========================================================================
** Format  : VOID  PrtAgeLog(TRANINFORMATION *pTran, ITEMPRINT *pItem);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints boundary age log, 2172. 
*===========================================================================
*/
VOID  PrtAgeLog(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( fsPrtPrintPort & PRT_JOURNAL ) {           /* electric journal */
        PrtAgeLog_EJ(pTran, pItem);
    }

}

/*
*===========================================================================
** Format  : VOID  PrtAgeLog_EJ(TRANINFORMATION *pTran);
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints boundary age log. ( elctric journal )
*===========================================================================
*/
VOID  PrtAgeLog_EJ(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    USHORT usMnemLen;

    usMnemLen = tcharlen( pItem->aszNumber[0] );//US Customs
    
    PrtPrint( PMG_PRT_JOURNAL, pItem->aszNumber[0], usMnemLen );//US Customs
    
    return;

    pTran = pTran;
}


/*
*===========================================================================
** Format  : VOID  PrtDflAgeLog( TRANINFORMATION *pTran,
*                                     ITEMPRINT    *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays boundary age log 2172.
*===========================================================================
*/
VOID PrtDflAgeLog( TRANINFORMATION *pTran, ITEMPRINT *pItem )
{
    TCHAR   aszDflBuff[ 6 ][ PRT_DFL_LINE + 1 ];
    USHORT  usLineNo;               /* number of lines to be displayed */
    USHORT  usOffset = 0;
    USHORT  i;
                                                

    if ( pItem->aszNumber[0] == '\0' ) {
        return;
    }

    memset( aszDflBuff, 0x00, sizeof( aszDflBuff ));

    /* -- set header -- */
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo );

    /* -- set item data -- */
    usLineNo += PrtDflMnemonic( aszDflBuff[ usLineNo ], pItem->aszNumber[0] );//US Customs

    /* -- set destination CRT -- */

    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- set display data in the buffer -- */ 
    PrtDflIType( usLineNo, DFL_CUSTNAME );

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
** Format  : VOID  PrtDflAgeLog( TRANINFORMATION *pTran,
*                                     ITEMPRINT    *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMPRINT        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays boundary age log 2172.
*===========================================================================
*/
USHORT PrtDflAgeLogForm( TRANINFORMATION *pTran, ITEMPRINT *pItem, TCHAR *puchBuffer)
{
    TCHAR   aszDflBuff[ 6 ][ PRT_DFL_LINE + 1 ];
    USHORT  usLineNo=0, i;               /* number of lines to be displayed */
                                                
    if ( pItem->aszNumber[0] == '\0' ) {
        return 0;
    }

    memset( aszDflBuff, 0x00, sizeof( aszDflBuff ));

    /* -- set header -- */
#if 0
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo );
#endif
    /* -- set item data -- */
    usLineNo += PrtDflMnemonic( aszDflBuff[ usLineNo ], pItem->aszNumber[0] );//US Customs

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/*
*===========================================================================
** Format  : VOID  PrtCPRespText(TRANINFORMATION *pTran, ITEMPRINT *pItem)
*
*   Input  : TRNINFORMATION  *pTran,    -transaction information
*          : ITEMPRINT       *pItem     -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints EPT Response Message Text,    Saratoga
*===========================================================================
*/
VOID    PrtCPRespText(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    USHORT  fsPrtStat;

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if (fsPrtStatus & PRT_REQKITCHEN) {         /* kitchen print */
        return;
    }

    fsPrtStat = pItem->fsPrintStatus;

    if ( !(pTran->TranCurQual.flPrintStatus & ( CURQUAL_POSTRECT | CURQUAL_PARKING ))) {
   		/* not execute slip validation at duplicated receipt, 09/11/01 */
	    if (fsPrtStat & PRT_SLIP) {
           	if ((usPrtSlipPageLine == 0) || ((fsPrtCompul & PRT_SLIP) == 0)) {            /* not listing slip */
				if(PRT_SUCCESS != PrtSPVLInit())
				{
					return;
				}
            	PrtSPVLHead(pTran);
	            PrtCPRespText_SP(pTran, pItem);
    	        PrtSPVLTrail(pTran);
        	    usPrtSlipPageLine = 0;
            	return;	/* 09/13/01 */
	        }
	    }
    }
    if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print           */
        PrtCPRespText_SP(pTran, pItem);
    }
    if (( fsPrtPrintPort & PRT_RECEIPT ) || ( fsPrtPrintPort & PRT_JOURNAL )) {     /* thermal print or electric journal */
        PrtTHCPRespText(pItem);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtCPRespText_SP(TRANINFORMATION* pTran, ITEMPRINT *pItem)
*
*   Input  : TRNINFORMATION *pTran
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints EPT response message,         Saratoga
*===========================================================================
*/
VOID    PrtCPRespText_SP(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    TCHAR   aszSPPrintBuff[NUM_CPRSPTEXT][PRT_SPCOLUMN + 1];
    TCHAR   aszPrint[NUM_CPRSPTEXT + 1];
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;

    /* -- check if paper change is necessary or not -- */
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    if (pItem->uchPrintSelect != 0) {
        for (i = 0; i < pItem->uchCPLineNo; i++) {
            memset(aszPrint, 0, sizeof(aszPrint));
            _tcsncpy(aszPrint, &pItem->aszCPText[0][0] + i * pItem->uchPrintSelect, pItem->uchPrintSelect);
            usSlipLine += PrtSPCPRspMsgText(aszSPPrintBuff[usSlipLine], aszPrint);
        }
    }

    for (i = 0; i < usSlipLine; i++) {
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += pItem->uchCPLineNo + usSaveLine;
}

/*
*===========================================================================
** Format  : VOID  PrtTHCPRespText(ITEMPRNT *pItem);
*
*   Input  : ITEMPRINT  *pItem
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints EPT response message,         Saratoga
*===========================================================================
*/
VOID    PrtTHCPRespText(ITEMPRINT *pItem)
{
    TCHAR   *puchStart, aszPrint[NUM_CPRSPTEXT + 1];
    USHORT  i;

    if (pItem->uchPrintSelect != 0) {
        puchStart = (TCHAR *)pItem->aszCPText;
        for (i = 0; i < pItem->uchCPLineNo; i++) {
            memset(aszPrint, 0, sizeof(aszPrint));
            _tcsncpy(aszPrint, puchStart + i * pItem->uchPrintSelect, pItem->uchPrintSelect);
		    if ((fsPrtPrintPort & PRT_RECEIPT ) && (pItem->fsPrintStatus & PRT_RECEIPT)) {
	            PrtTHCPRspMsgText(aszPrint);
	    	}
			if (pItem->fsPrintStatus & PRT_JOURNAL) {
	            PrtEJCPRspMsgText(aszPrint);	/* 09/13/01 */
	        }
        }
		if ((fsPrtPrintPort & PRT_RECEIPT ) && (pItem->fsPrintStatus & PRT_RECEIPT)) {
			if ((TrnInformation.TranCurQual.fbNoPrint & CURQUAL_NO_EPT_LOGO_SIG) == 0) {
				PrtSoftCHK(SOFTCHK_EPT1_ADR);
			}
		}
    } 
}

/*
*===========================================================================
** Format  : VOID  PrtFSCredit(TRANINFORMATION *pTran, ITEMPRINT *pItem)
*
*   Input  :  TRNINFORMATION  *pTran,
*             ITEMPRINT  *pItem
*
*   Output : none
*   InOut  : none
*
** Return  :
*
** Synopsis:
*
*===========================================================================
*/
VOID  PrtFSCredit(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    TCHAR       aszSPPrintBuff[PRT_SPCOLUMN + 1];
    USHORT      usRet = PRT_FAIL;
		
    PrtPortion(pItem->fsPrintStatus);
    if (pItem->fsPrintStatus & PRT_VALIDATION) {             /* validation print */
        if(PRT_SUCCESS != PrtSPVLInit())
		{
			return;
		}
        PrtSPVLHead(pTran);

        memset(aszSPPrintBuff, '\0', sizeof(aszSPPrintBuff));

        if (pItem->lAmount) {                  
            PrtSPMnemAmt(aszSPPrintBuff, TRN_FSCRD_ADR, pItem->lAmount);   /* FS credit */
        }
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff, PRT_SPCOLUMN);
        PrtSPVLTrail(pTran);
		PrtSlpRel(); //SR437
    } else if (pItem->fsPrintStatus & (PRT_RECEIPT | PRT_SPCL_PRINT)) {    /* Chit */
        PrtTHHead(pTran);                           /* added 2000.07.11 start */

        if ( fbPrtShrStatus & PRT_SHARED_SYSTEM ) { 
            fbPrtAltrStatus |= PRT_TOTAL_STUB;
        }                                           /* added 2000.07.11 end   */

        PrtTHAmtMnem(TRN_FSCRD_ADR, pItem->lAmount);
    } else if ((pItem->fsPrintStatus & PRT_RECEIPT) || (pItem->fsPrintStatus & PRT_JOURNAL)) {
//        PrtFSCredit_TH( pItem );
    }
}

/* Check Endorsement, 21RFC05402 */
/*
*===========================================================================
** Format  : VOID  PrtEndorse(TRANINFORMATION *pTran, ITEMPRINT *pItem)
*
*   Input  :  TRNINFORMATION  *pTran,
*             ITEMPRINT  *pItem
*
*   Output : none
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function prints check endorsement on slip.    DTREE#2 Dec/18/2000
*===========================================================================
*/
VOID    PrtEndorse(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
    PrtPortion(pItem->fsPrintStatus);

    if ((pItem->fsPrintStatus & PRT_VALIDATION) && CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {	/* only slip validation 03/23/01 */
		USHORT  usAdr;
		TCHAR   achWork[5] = {0}; 

        if(PRT_SUCCESS != PrtSPVLInit())
		{
			return;
		}

        if (pItem->uchStatus <= CLASS_TENDER8) {
            usAdr = (UCHAR)(TRN_TEND1_ADR + pItem->uchStatus - CLASS_TENDER1);
        } else if ((pItem->uchStatus >= CLASS_TENDER9) && (pItem->uchStatus <= CLASS_TENDER11)) {
            usAdr = (UCHAR)(TRN_TEND9_ADR + pItem->uchStatus - CLASS_TENDER9);
        } else if ((pItem->uchStatus >= CLASS_TENDER12) && (pItem->uchStatus <= CLASS_TENDER20)) {
			usAdr = (UCHAR)(TRN_TENDER12_ADR + pItem->uchStatus - CLASS_TENDER12);
		} else {
			NHPOS_ASSERT(pItem->uchStatus <= CLASS_TENDER20);
			return;
		}

        if(pItem->uchPrintSelect & PRT_ENDORSE_HORIZNTL) {
			/* horizontal insert */
			USHORT  usSlipLine = 0, i;
			TCHAR   aszPrint[8][PRT_SPCOLUMN + 1] = {0};

            PrtFeed(PMG_PRT_SLIP, 1);
            usSlipLine += PrtSPEndorsHeadH(aszPrint[usSlipLine], CH24_1STCE_ADR, pTran);
            usSlipLine += PrtSPEndorsHeadH(aszPrint[usSlipLine], CH24_2NDCE_ADR, pTran);
            usSlipLine += PrtSPEndorsMnemH1(aszPrint[usSlipLine], TRN_AMTTL_ADR, pItem->lMI);
            usSlipLine += PrtSPEndorsMnemH2(aszPrint[usSlipLine], usAdr, pItem->lAmount);
            usSlipLine += PrtSPEndorsTrail2(aszPrint[usSlipLine], pTran);
            usSlipLine += PrtSPEndorsTrail3(aszPrint[usSlipLine], pTran);
            for (i = 0; i < usSlipLine; i++) {
                PrtPrint(PMG_PRT_SLIP, aszPrint[i], PRT_SPCOLUMN);
            }
        } else {
			/* vertical insert */
			USHORT  usSlipLine = 0, i;
			TCHAR   aszPrint[6][PRT_SPCOLUMN + 1] = {0};

            usSlipLine += PrtSPEndorsHeadV(aszPrint[usSlipLine], pTran);
            usSlipLine += PrtSPEndorsMnemAmtV1(aszPrint[usSlipLine], TRN_AMTTL_ADR, pItem->lMI);
            usSlipLine += PrtSPEndorsMnemAmtV2(aszPrint[usSlipLine], usAdr, pItem->lAmount);
            usSlipLine += PrtSPEndorsTrail1(aszPrint[usSlipLine], pTran);
            /* -- print all data in the buffer -- */
            for (i = 0; i < usSlipLine; i++) {
                PrtPrint(PMG_PRT_SLIP, aszPrint[i], PRT_SPCOLUMN);
            }
        }
#if defined (PRT_7158SLIP)
		achWork[0] = ESC;           /* ESC: printer escape sequence */
		achWork[1] = _T('c');
		achWork[2] = _T('0');
		achWork[3] = 0x02;          /* return to receipt */
		PmgPrint(PMG_PRT_RECEIPT, achWork, (USHORT)_tcslen(achWork));
#else
    achWork[0] = ESC;
    achWork[1] = _T('q');
    achWork[2] = _T('\0');
    achWork[3] = _T('\0');
    PmgPrint(PMG_PRT_SLIP, achWork, (USHORT)_tcslen(achWork));  /* praten open */
#endif
    }
}

/*
*===========================================================================
** Format  : VOID  PrtTHCPRespText(ITEMPRNT *pItem);
*
*   Input  : ITEMPRINT  *pItem
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints EPT response message,         Saratoga
*===========================================================================
*/
VOID    PrtTHCPRespTextGiftCard(TRANINFORMATION *pTran, ITEMPRINT *pItem)
{
	TRANINFORMATION  pTranTmp;
	memset(&pTranTmp,0,sizeof(TRANINFORMATION));
	RflDecryptByteString((UCHAR *)&(pTran->TranGCFQual.TrnGiftCard[0].ItemTender.aszNumber[0]),
		sizeof(pTran->TranGCFQual.TrnGiftCard[0].ItemTender.aszNumber));

	if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print           */
		PrtTender_SP(&pTranTmp, &pTran->TranGCFQual.TrnGiftCard[0].ItemTender, 0);              /* normal slip          */
	}
	if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
		PrtTender_TH(&pTranTmp, &pTran->TranGCFQual.TrnGiftCard[0].ItemTender);
	}
	if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
		PrtTender_EJ(&pTran->TranGCFQual.TrnGiftCard[0].ItemTender);
	}

	RflEncryptByteString((UCHAR *)&(pTran->TranGCFQual.TrnGiftCard[0].ItemTender.aszNumber[0]), sizeof(pTran->TranGCFQual.TrnGiftCard[0].ItemTender.aszNumber));

}

/* dollar tree */
/****** End of Source ******/
