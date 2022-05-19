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
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1997-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  Loan/Pickup Amount Print
* Category    : Print, NCR 2170 GP R2.0 Application
* Program Name: PrSLP_.C
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtSupLoanPickup() : prints loan/pickup amount
*               PrtSupLoanPickup_RJ() : prints loan/pickup (receipt & journal)
*               PrtSupForeignTender_RJ() : prints loan/pickup (receipt & journal)
*               PrtSupLoanPickup_VL() : prints loan/pickup (validation)
*               PrtSupLoanPickup_SP() : prints loan/pickup (slip)
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Mar-16-93 : 00.00.01 :  M.Ozawa   :
* Oct-15-96 : 02.00.00 :  S.Kubota  : Change for Foreign Currency #3-8
* Dec-03-99 : 01.00.00 :  hrkato    : Saratoga
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
#include "ecr.h"
#include "rfl.h"
/* #include <uie.h> */
#include "regstrct.h"
#include "transact.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "csttl.h"
#include "csop.h"
#include "report.h"
#include "pmg.h"
#include "prtrin.h"
#include "prtsin.h"
#include "prrcolm_.h"

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtSupLoanPickup(VOID *pData1, MAINTLOANPICKUP *pData2)
*
*   Input  : TRNINFORMATION   *pTran     -Transaction Information address
*            MAINTLOANPICKUP  *pItem     -Loan/Pickup Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function checks receipt/jurnal/validation/slip status.
*===========================================================================
*/
VOID    PrtSupLoanPickup(VOID *pData1, MAINTLOANPICKUP *pData2)
{
    SHORT  sPrintType = PRT_SLIP;
    TRANINFORMATION *pTran = pData1;

    switch (pData2->uchMinorClass) {
    case CLASS_MAINTTENDER1:          // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER2:          // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER3:          // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER4:          // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER5:          // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER6:          // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER7:          // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER8:          // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER9:          // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER10:         // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER11:         // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER12:         // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER13:         // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER14:         // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER15:         // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER16:         // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER17:         // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER18:         // function Thermsup:: PrtSupLoanPickup) for AC10/AC11.
    case CLASS_MAINTTENDER19:         // function Thermsup:: PrtSupLoanPickup) for AC10/AC11.
    case CLASS_MAINTTENDER20:         // function Thermsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTFOREIGN1:
    case CLASS_MAINTFOREIGN2:
    case CLASS_MAINTFOREIGN3:
    case CLASS_MAINTFOREIGN4:
    case CLASS_MAINTFOREIGN5:
    case CLASS_MAINTFOREIGN6:
    case CLASS_MAINTFOREIGN7:
    case CLASS_MAINTFOREIGN8:
    case CLASS_MAINTCOUNTUP:
	//US Customs SCER cwunn 4/8/03
	case CLASS_MAINTLOAN:
	case CLASS_MAINTPICKUP:
	//End US Customs SCER cwunn
        break;

    default:
        return;
    }

    /* -- set print portion to static area "fsPrtPrintPort",    Saratoga -- */
    PrtPortion(pData2->HeadTrail.usPrintControl);

	/* modified for slip printer is not connected case, 09/19/01 */
    if ( fsPrtPrintPort & PRT_SLIP ) {              /* slip print           */
/*    if ((pData2->HeadTrail.usPrintControl & PRT_SLIP) ||            / slip print /
        (fsPrtCompul & PRT_SLIP)) { */
        PrtSupLoanPickup_SP(pTran, pData2);
	}
    /*} else {*/
    if (fsPrtPrintPort & PRT_RECEIPT) {
        PrtSupLoanPickup_TH(pData2);
    }
    /*}*/
    if (fsPrtPrintPort & PRT_JOURNAL) {
        PrtSupLoanPickup_EJ(pData2);
    }
}

/*
*===========================================================================
** Format  : VOID  PrtSupLoanPickup_TH(MAINTLOANPICKUP *pData);
*
*   Input  :    *pData      : Data address
*               usFuncType  : function switch
*                               PRT_RJ           - for R/J
*                               PRT_EJ           - for E/J
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints loan/pickup amount (receipt & journal)
*
*            See function PrtSupLoanPickup() which calls into here along with
*            calls to other versions of this function depending on the printer
*            setting.
*
*            See also functions PrtSupLoanPickup_EJ() and PrtSupLoanPickup_SP().
*===========================================================================
*/
VOID    PrtSupLoanPickup_TH(MAINTLOANPICKUP *pData)
{
    static const TCHAR FARCONST  aszPrtTHLPQty[]   = _T(" %5ld X\t%s ");
/*    static const TCHAR FARCONST  aszPrtRJLPFor[]   = _T(" %5ld /");*/
    static const TCHAR FARCONST  aszPrtTHLPAmount[] = _T("%s\t%s%l$");

	TCHAR      aszSpecAmt[PARA_SPEMNEMO_LEN +PRT_AMOUNT_LEN + 1] = {0};
    TCHAR      aszTransBuff[PARA_TRANSMNEMO_LEN + 1] = {0};
    DCURRENCY  lAmount = 0;
    USHORT     usPrtType = PRT_RECEIPT;   /*usPrtType = (PRT_RECEIPT | PRT_JOURNAL);*/

    /* void  print */
    PrtSupTHVoid(pData->usModifier, 0);

	/* e/c print */
	if(pData->usModifier & MAINT_MODIFIER_EC){
		PrtTHMnem(TRN_EC_ADR, PRT_DOUBLE);     /* E/C with double wide */
	}
		

    /* qty print */
    if(pData->usModifier & MAINT_MODIFIER_QTY){
		UCHAR      uchFCAdr = 0;

        memset(&aszSpecAmt, 0x00, sizeof(aszSpecAmt));
        switch (pData->uchMinorClass) {
        case CLASS_MAINTFOREIGN1:
           uchFCAdr  = SPC_CNSYMFC1_ADR;
           break;

        case CLASS_MAINTFOREIGN2:
           uchFCAdr  = SPC_CNSYMFC2_ADR;
           break;

       case CLASS_MAINTFOREIGN3:
           uchFCAdr  = SPC_CNSYMFC3_ADR;
           break;

       case CLASS_MAINTFOREIGN4:
           uchFCAdr  = SPC_CNSYMFC4_ADR;
           break;

       case CLASS_MAINTFOREIGN5:
           uchFCAdr  = SPC_CNSYMFC5_ADR;
           break;

       case CLASS_MAINTFOREIGN6:
           uchFCAdr  = SPC_CNSYMFC6_ADR;
           break;

       case CLASS_MAINTFOREIGN7:
           uchFCAdr  = SPC_CNSYMFC7_ADR;
           break;

       case CLASS_MAINTFOREIGN8:
           uchFCAdr  = SPC_CNSYMFC8_ADR;
           break;

        default:
			uchFCAdr = 0;
            break;
        }
		if (uchFCAdr) {
			/* -- adjust foreign mnemonic and amount sign(+.-) -- */
			PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
		} else {
			/* -- adjust native mnemonic and amount sign(+.-) -- */
			PrtAdjustNative(aszSpecAmt, pData->lUnitAmount);
		}
                                                       /* R2.0 End   */
        PrtPrintf(usPrtType, aszPrtTHLPQty, pData->lForQty, aszSpecAmt);
    }

    /* amount print */
    lAmount = pData->lAmount;

    switch ( pData->uchMinorClass ) {
    case CLASS_MAINTFOREIGN1:
    case CLASS_MAINTFOREIGN2:
    case CLASS_MAINTFOREIGN3:
    case CLASS_MAINTFOREIGN4:
    case CLASS_MAINTFOREIGN5:
    case CLASS_MAINTFOREIGN6:
    case CLASS_MAINTFOREIGN7:
    case CLASS_MAINTFOREIGN8:
        PrtSupForeignTender_TH(pData);
        break;

    case CLASS_MAINTCOUNTUP:
        lAmount = pData->lTotal;                    /* set total amount */

    default:
		memset(aszSpecAmt, 0x00, sizeof(aszSpecAmt));
		RflGetTranMnem(aszTransBuff, PrtChkLoanPickupAdr(pData->uchMajorClass, pData->uchMinorClass));
		RflGetSpecMnem(aszSpecAmt, SPC_CNSYMNTV_ADR);
        PrtPrintf(usPrtType, aszPrtTHLPAmount, aszTransBuff, aszSpecAmt, lAmount);
        break;
    }
}

/*
*===========================================================================
** Format  : VOID  PrtSupForeignTender_TH(MAINTLOANPICKUP *pData);
*
*   Input  : MAINTLOANPICKUP      *pData     -loan/pick up data
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints foreign tender loan/pickup
*===========================================================================
*/
VOID    PrtSupForeignTender_TH(CONST MAINTLOANPICKUP *pData)
{
	PrtTHForeign(pData->lAmount, pData->uchMinorClass, pData->uchFCMDC,pData->ulFCRate, pData->uchLoanPickStatus);
    PrtTHAmtSym(TRN_FT_EQUIVALENT, pData->lNativeAmount, PRT_SINGLE);      /* native tender */
}

/*
*===========================================================================
** Format  : VOID  PrtSupLoanPickup_VL(VOID *pData1, MAINTLOANPICKUP *pData2)
*
*   Input  : TRNINFORMATION   *pTran,    -transaction information
*            MAINTLOANPICKUP  *pData     -Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints loan/pickup amount (validation)
*===========================================================================
*/
#if 0
VOID  PrtSupLoanPickup_VL(VOID *pData1, MAINTLOANPICKUP *pData2)
{
    USHORT usACNumber;
    TRANINFORMATION *pTran;

    pTran = (TRANINFORMATION *)pData1;

    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    /* -- in case of 24 char printer -- */
    if (usPrtVLColumn == PRT_VL_24) {
        if(pData2->uchMajorClass == CLASS_MAINTLOAN){
            usACNumber = 10;
        } else {
            usACNumber = 11;
        }
        PrtSupVLHead(&pData2->HeadTrail);           /* send header line */
        PrtSupVLLoanPickup(pTran, pData2);          /* loan/pickup line */
        PrtSupVLTrail(pTran, &pData2->HeadTrail);   /* send trailer lines */
    }

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */
}
#endif
/*
*===========================================================================
** Format  : VOID  PrtSupLoanPickup_SP(VOID *pData1, MAINTLOANPICKUP *pData2)
*
*   Input  : TRNINFORMATION   *pTran,    -transaction information
*            MAINTLOANPICKUP  *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints loan/pickup (slip)
*
*            See function PrtSupLoanPickup() which calls into here along with
*            calls to other versions of this function depending on the printer
*            setting.
*
*            See also functions PrtSupLoanPickup_EJ() and PrtSupLoanPickup_TH().
*===========================================================================
*/
VOID    PrtSupLoanPickup_SP(VOID *pData1, MAINTLOANPICKUP *pData2)
{
	TCHAR            aszSPPrintBuff[5][PRT_SPCOLUMN + 1] = {0}; /* print data save area */
    TCHAR            aszNum[NUM_NUMBER] = {0};
    USHORT           usSlipLine = 0;            /* number of lines to be printed */
    USHORT           usSaveLine;                /* save slip lines to be added */
    USHORT           i;
    LONG             lQTY = 0;
    UCHAR            uchSymAdr = 0, uchTrnsAdr = 0;
    TRANINFORMATION  *pTran = pData1;

    /* -- set void mnemonic  -- */
    usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[0], pData2->usModifier, 0, aszNum);

    /* -- set for/qty mnemonic and amount */
    lQTY = pData2->lForQty * PLU_BASE_UNIT;
    usSlipLine += PrtSPQtyFor(aszSPPrintBuff[usSlipLine], lQTY, 0L, pData2->lUnitAmount);


    switch (pData2->uchMinorClass) {
    case CLASS_MAINTFOREIGN1:                       /* R2.0 Start */
        uchSymAdr  = SPC_CNSYMFC1_ADR;
        uchTrnsAdr = TRN_FT1_ADR;
        break;

    case CLASS_MAINTFOREIGN2:
        uchSymAdr  = SPC_CNSYMFC2_ADR;
        uchTrnsAdr = TRN_FT2_ADR;
        break;

    case CLASS_MAINTFOREIGN3:
        uchSymAdr  = SPC_CNSYMFC3_ADR;
        uchTrnsAdr = TRN_FT3_ADR;
        break;

    case CLASS_MAINTFOREIGN4:
        uchSymAdr  = SPC_CNSYMFC4_ADR;
        uchTrnsAdr = TRN_FT4_ADR;
        break;

    case CLASS_MAINTFOREIGN5:
        uchSymAdr  = SPC_CNSYMFC5_ADR;
        uchTrnsAdr = TRN_FT5_ADR;
        break;

    case CLASS_MAINTFOREIGN6:
        uchSymAdr  = SPC_CNSYMFC6_ADR;
        uchTrnsAdr = TRN_FT6_ADR;
        break;

    case CLASS_MAINTFOREIGN7:
        uchSymAdr  = SPC_CNSYMFC7_ADR;
        uchTrnsAdr = TRN_FT7_ADR;
        break;

    case CLASS_MAINTFOREIGN8:
        uchSymAdr  = SPC_CNSYMFC8_ADR;
        uchTrnsAdr = TRN_FT8_ADR;
        break;

    case CLASS_MAINTCOUNTUP:
    default:
		uchSymAdr = uchTrnsAdr = 0;    // print native amount without foreign amount
		{
			DCURRENCY        lAmount = 0;

			if (pData2->uchMinorClass == CLASS_MAINTCOUNTUP) {
				lAmount = pData2->lTotal;
			}
			else {
				lAmount = pData2->lAmount;
			}
			usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], PrtChkLoanPickupAdr(pData2->uchMajorClass, pData2->uchMinorClass), lAmount);
		}
		break;
    }

	if (uchSymAdr && uchTrnsAdr) {
		/* -- set foreign amount and rate -- */
		usSlipLine += PrtSpForeign(aszSPPrintBuff[usSlipLine],
			pData2->lAmount,
			uchSymAdr,
			pData2->uchFCMDC,
			pData2->ulFCRate,
			pData2->uchFCMDC2);

		/* -- set foreign tender mnemonic and amount -- */
		usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], uchTrnsAdr, pData2->lNativeAmount);
	}

    /* -- check if paper change is necessary or not -- */
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */
    for ( i = 0; i < usSlipLine; i++ ) {
        PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;
}

/*
*===========================================================================
** Format  : UCHAR  PrtChkLoanPickupAdr(UCHAR uchMajirClass, UCHAR uchMinorClass)
*
*   Input  : UCHAR uchMinorClass    -minor class of loan/pickup
*   Output : none
*   InOut  : none
** Return  : transacton mnemonic address of each tender
*
** Synopsis: This function checks transaction mnemonics address of each media
*===========================================================================
*/
USHORT   PrtChkLoanPickupAdr(UCHAR uchMajorClass, UCHAR uchMinorClass)
{
	struct {
		UCHAR  uchMinorClass;
		USHORT  uchAddress;           // address of the mnemonic
	}  MyMnemonicList[] = {
		{CLASS_MAINTTENDER1, TRN_TEND1_ADR},
		{CLASS_MAINTTENDER2, TRN_TEND2_ADR},
		{CLASS_MAINTTENDER3, TRN_TEND3_ADR},
		{CLASS_MAINTTENDER4, TRN_TEND4_ADR},
		{CLASS_MAINTTENDER5, TRN_TEND5_ADR},
		{CLASS_MAINTTENDER6, TRN_TEND6_ADR},
		{CLASS_MAINTTENDER7, TRN_TEND7_ADR},
		{CLASS_MAINTTENDER8, TRN_TEND8_ADR},
		{CLASS_MAINTTENDER9, TRN_TEND9_ADR},
		{CLASS_MAINTTENDER10, TRN_TEND10_ADR},
		{CLASS_MAINTTENDER11, TRN_TEND11_ADR},
		{CLASS_MAINTTENDER12, TRN_TENDER12_ADR},
		{CLASS_MAINTTENDER13, TRN_TENDER13_ADR},
		{CLASS_MAINTTENDER14, TRN_TENDER14_ADR},
		{CLASS_MAINTTENDER15, TRN_TENDER15_ADR},
		{CLASS_MAINTTENDER16, TRN_TENDER16_ADR},
		{CLASS_MAINTTENDER17, TRN_TENDER17_ADR},
		{CLASS_MAINTTENDER18, TRN_TENDER18_ADR},
		{CLASS_MAINTTENDER19, TRN_TENDER19_ADR},
		{CLASS_MAINTTENDER20, TRN_TENDER20_ADR},
		{CLASS_MAINTFOREIGN1, TRN_FT1_ADR},
		{CLASS_MAINTFOREIGN2, TRN_FT2_ADR},
		{CLASS_MAINTFOREIGN3, TRN_FT3_ADR},
		{CLASS_MAINTFOREIGN4, TRN_FT4_ADR},
		{CLASS_MAINTFOREIGN5, TRN_FT5_ADR},
		{CLASS_MAINTFOREIGN6, TRN_FT6_ADR},
		{CLASS_MAINTFOREIGN7, TRN_FT7_ADR},
		{CLASS_MAINTFOREIGN8, TRN_FT8_ADR},
		{0, 0}
	};
    USHORT  usAdr = 0;
	SHORT  sIndex;

	for (sIndex = 0; MyMnemonicList[sIndex].uchMinorClass > 0; sIndex++) {
		if (MyMnemonicList[sIndex].uchMinorClass == uchMinorClass) {
			usAdr = MyMnemonicList[sIndex].uchAddress;
			break;
		}
	}

    if (usAdr == 0) {
        if ( uchMajorClass == CLASS_MAINTLOAN ) {
            usAdr = TRN_LOAN_ADR;
        } else {
            usAdr = TRN_PICKUP_ADR;
        }
    }

    return(usAdr);
}

/* --- End of Source File --- */