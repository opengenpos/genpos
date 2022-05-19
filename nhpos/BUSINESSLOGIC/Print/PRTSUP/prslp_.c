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
#include "csstbpar.h"

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
    TRANINFORMATION *pTran = pData1;

    switch (pData2->uchMinorClass) {
    case CLASS_MAINTTENDER1:          // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER2:          // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER3:          // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER4:          // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER5:          // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER6:          // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER7:          // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER8:          // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER9:          // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER10:         // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER11:         // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER12:         // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER13:         // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER14:         // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER15:         // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER16:         // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER17:         // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTTENDER18:         // function Prtsup:: PrtSupLoanPickup) for AC10/AC11.
    case CLASS_MAINTTENDER19:         // function Prtsup:: PrtSupLoanPickup) for AC10/AC11.
    case CLASS_MAINTTENDER20:         // function Prtsup:: PrtSupLoanPickup() for AC10/AC11.
    case CLASS_MAINTFOREIGN1:
    case CLASS_MAINTFOREIGN2:
    case CLASS_MAINTFOREIGN3:
    case CLASS_MAINTFOREIGN4:
    case CLASS_MAINTFOREIGN5:
    case CLASS_MAINTFOREIGN6:
    case CLASS_MAINTFOREIGN7:
    case CLASS_MAINTFOREIGN8:
    case CLASS_MAINTCOUNTUP:
        break;

    default:
        return;
    }

    if ((pData2->uchMajorClass == CLASS_MAINTLOAN && CliParaMDCCheck(MDC_LOANPICK2_ADR, EVEN_MDC_BIT0)) ||
        (pData2->uchMajorClass == CLASS_MAINTPICKUP && CliParaMDCCheck(MDC_LOANPICK2_ADR, EVEN_MDC_BIT1))) {
        PrtSupLoanPickup_VL(pTran, pData2);
        return;
    }

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion((USHORT)pTran->TranCurQual.fbCompPrint);

    PrtSupLoanPickup_RJ(pData2, PRT_RECEIPT);
}

/*
*===========================================================================
** Format  : VOID  PrtSupLoanPickup_RJ(MAINTLOANPICKUP *pData,UCHAR usFuncType);
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
*===========================================================================
*/
VOID    PrtSupLoanPickup_RJ(MAINTLOANPICKUP *pData, UCHAR usFuncType)
{
    static const TCHAR   aszPrtRJLPQty[]   = _T(" %5ld X\t%s ");
    static const TCHAR   aszPrtRJLPFor[]   = _T(" %5ld /");
    static const TCHAR   aszPrtRJLPAmount[] = _T("%s\t %l$");

	TCHAR       aszTransBuff[PARA_TRANSMNEMO_LEN +1] = {0};
    DCURRENCY   lAmount;
    USHORT      usPrtType;
    UCHAR       uchFCAdr;

    /* Check Print Control */
    usPrtType = (PRT_RECEIPT | PRT_JOURNAL);

    /* void, e/c print */
    PrtSupRJVoid(pData->usModifier);

    /* qty print */
    if(pData->usModifier & MAINT_MODIFIER_QTY){
		TCHAR       aszSpecAmt[PARA_SPEMNEMO_LEN + PRT_AMOUNT_LEN + 1] = {0};

		switch (pData->uchMinorClass) {
        case CLASS_MAINTFOREIGN1:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC1_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

        case CLASS_MAINTFOREIGN2:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC2_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

       case CLASS_MAINTFOREIGN3:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC3_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

       case CLASS_MAINTFOREIGN4:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC4_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

       case CLASS_MAINTFOREIGN5:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC5_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

       case CLASS_MAINTFOREIGN6:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC6_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

       case CLASS_MAINTFOREIGN7:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC7_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

       case CLASS_MAINTFOREIGN8:
           uchFCAdr  =(UCHAR)(SPC_CNSYMFC8_ADR);
            /* -- adjust foreign mnemonic and amount sign(+.-) -- */
            PrtAdjustForeign(aszSpecAmt, pData->lUnitAmount, uchFCAdr, pData->uchFCMDC);
           break;

        default:
            /* -- adjust native mnemonic and amount sign(+.-) -- */
            PrtAdjustNative(aszSpecAmt, pData->lUnitAmount);
            break;
        }
                                                       /* R2.0 End   */
        PmgPrintf(usPrtType, aszPrtRJLPQty, pData->lForQty, aszSpecAmt);
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
        PrtSupForeignTender_RJ(pData);
        break;

    case CLASS_MAINTCOUNTUP:
        lAmount = pData->lTotal;                    /* set total amount */

    default:
		RflGetTranMnem(aszTransBuff, PrtChkLoanPickupAdr(pData->uchMajorClass, pData->uchMinorClass));
        PmgPrintf(usPrtType, aszPrtRJLPAmount, aszTransBuff, lAmount);
        break;
    }
}

/*
*===========================================================================
** Format  : VOID  PrtSupForeignTender_RJ(MAINTLOANPICKUP *pData);
*
*   Input  : MAINTLOANPICKUP      *pData     -loan/pick up data
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints foreign tender loan/pickup
*===========================================================================
*/
VOID    PrtSupForeignTender_RJ(MAINTLOANPICKUP *pData)
{
    UCHAR     uchSymAdr;
    USHORT    usTrnsAdr;

    switch(pData->uchMinorClass){
    case CLASS_MAINTFOREIGN1:
        uchSymAdr  = (SPC_CNSYMFC1_ADR);
        usTrnsAdr = (TRN_FT1_ADR);
        break;

    case CLASS_MAINTFOREIGN2:
        uchSymAdr  = (SPC_CNSYMFC2_ADR);
        usTrnsAdr = (TRN_FT2_ADR);
        break;

    case CLASS_MAINTFOREIGN3:
        uchSymAdr  = (SPC_CNSYMFC3_ADR);
        usTrnsAdr = (TRN_FT3_ADR);
        break;

    case CLASS_MAINTFOREIGN4:
        uchSymAdr  = (SPC_CNSYMFC4_ADR);
        usTrnsAdr = (TRN_FT4_ADR);
        break;

    case CLASS_MAINTFOREIGN5:
        uchSymAdr  = (SPC_CNSYMFC5_ADR);
        usTrnsAdr = (TRN_FT5_ADR);
        break;

    case CLASS_MAINTFOREIGN6:
        uchSymAdr  = (SPC_CNSYMFC6_ADR);
        usTrnsAdr = (TRN_FT6_ADR);
        break;

    case CLASS_MAINTFOREIGN7:
        uchSymAdr  = (SPC_CNSYMFC7_ADR);
        usTrnsAdr = (TRN_FT7_ADR);
        break;

    case CLASS_MAINTFOREIGN8:
        uchSymAdr  = (SPC_CNSYMFC8_ADR);
        usTrnsAdr = (TRN_FT8_ADR);
        break;
    }
    PrtRJForeign1(pData->lAmount, uchSymAdr, pData->uchFCMDC);
    PrtRJForeign2(pData->ulFCRate, pData->uchFCMDC2);               /* foreign tender */
    PrtRJAmtSym(TRN_FT_EQUIVALENT, pData->lNativeAmount, PRT_SINGLE);      /* native tender */
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
VOID  PrtSupLoanPickup_VL(VOID *pData1, MAINTLOANPICKUP *pData2)
{
    USHORT usACNumber;
    TRANINFORMATION *pTran = pData1;

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
UCHAR   PrtChkLoanPickupAdr(UCHAR uchMajorClass, UCHAR uchMinorClass)
{
	struct {
		UCHAR  uchMinorClass;
		UCHAR  uchAddress;           // address of the mnemonic
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
    UCHAR  uchAdr = 0;
	SHORT  sIndex;

	for (sIndex = 0; MyMnemonicList[sIndex].uchMinorClass > 0; sIndex++) {
		if (MyMnemonicList[sIndex].uchMinorClass == uchMinorClass) {
			uchAdr = MyMnemonicList[sIndex].uchAddress;
			break;
		}
	}

    if (uchAdr == 0) {
        if ( uchMajorClass == CLASS_MAINTLOAN ) {
            uchAdr = TRN_LOAN_ADR;
        } else {
            uchAdr = TRN_PICKUP_ADR;
        }
    }

    return(uchAdr);
}

/* --- End of Source File --- */