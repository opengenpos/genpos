/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Pick Up Module
* Category    : User Interface For Supervisor, NCR 2170 US GP Application
* Program Name: MATPICK.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen)
*       2012  -> GenPOS Rel 2.2.0
*       2014  -> GenPOS Rel 2.2.1
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               MaintPickupAmountSet()    : Loan Amount Set
*               MaintPickupErrorCorrect() : Loan Amount Error Correct
* --------------------------------------------------------------------------
* Update Histories
*    Date   : Ver.Rev.  :   Name     : Description
* Mar-09-93 : 00.00.01  : M.Ozawa    : initial
* Mar-11-94 :           : M.INOUE    : GPUS r1.1
* Oct-15-96 : 02.00.00  : T.Yatuhasi : Change struct MAINTALLPICKUP,
*                                      MaintPickupAmountSet,
*                                      MaintAllPickupAmountRead,
*                                      MaintAllPickupAmountSet1,
*                                      MaintAllPickupAmountSet2,
*                                      MaintPickupFCAmountSet
*                                      for Foreign Currency #3-8
* Nov-30-99 : 01.00.00  : hrkato     : Saratoga
* Apr-04-15 : 02.02.01  : R.Chambers : source cleanup for AC 10/11, Connection Engine
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
==============================================================================
;                      I N C L U D E     F I L E s
;=============================================================================
*/

#include	<tchar.h>
#include <string.h>

#include "ecr.h"
#include "uie.h"
#include "pif.h"
#include "rfl.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "csstbpar.h"
#include "cscas.h"
#include "csstbcas.h"
#include "csttl.h"
#include "csstbttl.h"
#include "display.h"
#include "item.h"

#include "maintram.h"

#include  "ConnEngineObjectIf.h"

typedef struct {
    TOTAL       aTender[STD_TENDER_MAX];                 /* Tender #1 to #12             */
    TOTAL       aForeignCurrency[STD_NO_FOREIGN_TTL];    /* Foreign Currency #1- 8   * R2.0  */
} MAINTALLPICKUP;                                        /* All Pickup Amoun Save Area   */

MAINTALLPICKUP  MaintAllPickup;

/*
*======================================================================================
**   Synopsis:  SHORT MaintPickupAmountSet(MAINTLOANPICKUP *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for MAINTLOANPICKUP
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: Pickup Amount Set
======================================================================================
*/
SHORT   MaintPickupAmountSet(MAINTLOANPICKUP *pData )
{
    SHORT           sError;
    ITEMAFFECTION   Affect;
	CHAR uchTmpMinorClass; //saves minor class data US Customs SCER 3/27/03 cwunn

    /* compulsory for/qty check */
    if ( CliParaMDCCheckField(MDC_LOANPICK1_ADR, MDC_PARAM_BIT_C)) {
        if ( pData->lForQty == 0L){
            return(LDT_PROHBT_ADR);
        }
    }

    /* Caluculate Pickup Amount */
    if (pData->uchLoanPickStatus & MAINT_WITHOUT_DATA){      /* If All Pickup */
        if ( !CliParaMDCCheckField (MDC_LOANPICK1_ADR, MDC_PARAM_BIT_D)) { /* prohibit all pickup */
			NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: Pickup all prohibit, MDC 331 Bit D");
            return(LDT_BLOCKEDBYMDC_ADR);
        }
        MaintAllPickupAmountSet1(pData);
    } else if ( pData->usModifier & MAINT_MODIFIER_QTY){       /* QTY Caluculation */
        if ( pData->lForQty){
			D13DIGITS      d13ProdWork;

            if (pData->lForQty > STD_ATFOR_MAX_QTY) {
                return(LDT_KEYOVER_ADR);
            }
            d13ProdWork = pData->lForQty;
			d13ProdWork *= pData->lAmount;

            if (d13ProdWork >= 0L && d13ProdWork <= STD_PLU_MAX_PRICE) {
                if (d13ProdWork < pData->lAmount) {
                    return(LDT_KEYOVER_ADR);
                }
				pData->lUnitAmount = pData->lAmount;             /* set unit amount */
                pData->lAmount = d13ProdWork;
            } else {
                return(LDT_KEYOVER_ADR);
            }
        }
    }

	MaintWork.LoanPickup.uchLoanPickStatus = pData->uchLoanPickStatus;
    switch(pData->uchMinorClass){
    case CLASS_MAINTFOREIGN1:
    case CLASS_MAINTFOREIGN2:
    case CLASS_MAINTFOREIGN3:
    case CLASS_MAINTFOREIGN4:
    case CLASS_MAINTFOREIGN5:
    case CLASS_MAINTFOREIGN6:
    case CLASS_MAINTFOREIGN7:
    case CLASS_MAINTFOREIGN8:
        sError = MaintPickupFCAmountSet(pData);
        if (sError) {
            return(sError);
        }
    }

    pData->lAmount *= -1;                             /* Set Pickup Sign */
    pData->lForQty *= -1;
    pData->lNativeAmount *= -1;

    if(pData->usModifier & MAINT_MODIFIER_VOID){      /* Void Calculation */
        pData->lAmount *= -1;
        pData->lForQty *= -1;
        pData->lNativeAmount *= -1;
        pData->usModifier |= VOID_MODIFIER;/* */
    }

    MaintAllPickupAmountSet2(pData);     /* reset all pickup total */

    /* Save Pickup Amount Data */
    MaintWork.LoanPickup.uchMinorClass = pData->uchMinorClass;
    MaintWork.LoanPickup.usModifier = pData->usModifier;
    if (pData->usModifier & MAINT_MODIFIER_QTY) {
        MaintWork.LoanPickup.lForQty = pData->lForQty;
        MaintWork.LoanPickup.lUnitAmount = pData->lUnitAmount;
    } else {
        MaintWork.LoanPickup.lForQty = 0;
        MaintWork.LoanPickup.lUnitAmount = 0;
    }
    MaintWork.LoanPickup.ulFCRate = pData->ulFCRate;
    MaintWork.LoanPickup.uchFCMDC = pData->uchFCMDC;
    MaintWork.LoanPickup.uchFCMDC2 = pData->uchFCMDC2;
    MaintWork.LoanPickup.lNativeAmount = pData->lNativeAmount;
    MaintWork.LoanPickup.lAmount = pData->lAmount;

    switch (pData->uchMinorClass) {
    case CLASS_MAINTFOREIGN1:   /* add pickup total by native currency */
    case CLASS_MAINTFOREIGN2:
    case CLASS_MAINTFOREIGN3:
    case CLASS_MAINTFOREIGN4:
    case CLASS_MAINTFOREIGN5:
    case CLASS_MAINTFOREIGN6:
    case CLASS_MAINTFOREIGN7:
    case CLASS_MAINTFOREIGN8:
        MaintWork.LoanPickup.lTotal += pData->lNativeAmount;
        break;

    default:
        MaintWork.LoanPickup.lTotal += pData->lAmount;
    }

    /* Display Pickup Amount */
    MaintLoanPickupDisp(&(MaintWork.LoanPickup));

	if (MaintWork.LoanPickup.uchLoanPickStatus & MAINT_DATA_INPUT_CE) {
		MaintWork.LoanPickup.HeadTrail.usPrintControl = PRT_JOURNAL;  /* Saratoga */
	} else {
		MaintWork.LoanPickup.HeadTrail.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;  /* Saratoga */
		if (CliParaMDCCheck(MDC_LOANPICK2_ADR, EVEN_MDC_BIT1)) {
			MaintWork.LoanPickup.HeadTrail.usPrintControl |= PRT_SLIP;
		}
	}
    /* Print & Memory Affection Pickup Amount */
    if (TrnLoanPickup(&(MaintWork.LoanPickup)) == TRN_SUCCESS) {
        MaintWork.LoanPickup.uchLoanPickStatus |= MAINT_DATA_INPUT;
    }

	//Print Pickup mnemonic and amount US Customs SCER 3/27/03 cwunn
	uchTmpMinorClass = MaintWork.LoanPickup.uchMinorClass; //save minor class data
	MaintWork.LoanPickup.uchMinorClass = CLASS_MAINTPICKUP;
	TrnLoanPickup(&(MaintWork.LoanPickup));
	//Return MinorClass data to what it was before SCER code (just in case its needed elsewhere)
	MaintWork.LoanPickup.uchMinorClass = uchTmpMinorClass;

    memset(&Affect, 0, sizeof(ITEMAFFECTION));
    Affect.uchMajorClass = CLASS_ITEMAFFECTION;
    Affect.uchMinorClass = CLASS_PICKAFFECT;
    Affect.uchTotalID    = pData->uchMinorClass;
    Affect.lAmount       = MaintWork.LoanPickup.lAmount;
    Affect.lService[0]   = pData->lNativeAmount;
    
    /* check storage full, V1.0.12 */
    sError = TrnLoanPickup(&Affect);
	if (sError == TRN_SUCCESS) {
		ULONG           ulUserId = 0;
		TRANMODEQUAL    ModeQualRcvBuff;   
		TCHAR           aszSapId[16];
		TCHAR           tcsAddOnString[512], *ptcsBuffer;
		int             nLength;

		ptcsBuffer = tcsAddOnString;

#if defined(DCURRENCY_LONGLONG)
		nLength = _stprintf(ptcsBuffer, _T("<lAmount>%lld</lAmount>\r"), MaintWork.LoanPickup.lAmount);
#else
		nLength = _stprintf (ptcsBuffer, _T("<lAmount>%d</lAmount>\r"), MaintWork.LoanPickup.lAmount);
#endif
		if (nLength >= 0)
			ptcsBuffer += nLength;

		nLength = _stprintf (ptcsBuffer, _T("<usVoidIndic>%d</usVoidIndic>\r"), ((pData->usModifier & MAINT_MODIFIER_VOID)?1:0));
		if (nLength >= 0)
			ptcsBuffer += nLength;

		nLength = _stprintf (ptcsBuffer, _T("<usTotalID>%d</usTotalID>\r"), Affect.uchTotalID);
		if (nLength >= 0)
			ptcsBuffer += nLength;

		TrnGetModeQual(&ModeQualRcvBuff);

	    if (ModeQualRcvBuff.ulCashierID != 0)
			ulUserId = ModeQualRcvBuff.ulCashierID;
		else if (ModeQualRcvBuff.ulWaiterID != 0)
			ulUserId = ModeQualRcvBuff.ulWaiterID;

		_itot (ulUserId, aszSapId, 10);

		ConnEngineSendCashierActionWithAddon (CONNENGINE_ACTIONTYPE_PICKUP, aszSapId, tcsAddOnString);
	}

    return(sError);
}


/*
*======================================================================================
**   Synopsis:  VOID MaintAllPickupAmountRead(USHORT usCashierNumber)
*
*       Input:  USHORT  usCashierNumber
*      Output:  Nothing
*
**  Return:     Nothing
*
**  Description: All Pickup Amount Read from Cashier File
======================================================================================
*/
VOID    MaintAllPickupAmountRead(ULONG ulCashierNumber)
{
    SHORT           i;
	TTLCASTENDER    TtlCas = {0};

/*    CliTtlTotalRead(&TtlCas);     Saratoga */
    CliCasTtlTenderAmountRead(ulCashierNumber, &TtlCas);

    MaintAllPickup.aTender[0].lAmount = TtlCas.lCashTender;
    MaintAllPickup.aTender[1].lAmount = TtlCas.lCheckTender;
    MaintAllPickup.aTender[2].lAmount = TtlCas.lChargeTender;
	for (i = 0; i < sizeof(TtlCas.lMiscTender)/sizeof(TtlCas.lMiscTender[0]); i++) {
        MaintAllPickup.aTender[i+3].lAmount = TtlCas.lMiscTender[i];
    }
	for (i = 0; i < sizeof(TtlCas.lForeignTotal)/sizeof(TtlCas.lForeignTotal[0]); i++) {
        MaintAllPickup.aForeignCurrency[i].lAmount = TtlCas.lForeignTotal[i];
    }
}

struct {
	UCHAR  uchMinorClass;   // the minor class of the tender
	USHORT usOffset;        // offset to the array element for the tender
	USHORT usType;          // type of the tender, 1 = CLASS_MAINTTENDERn, 2 = CLASS_MAINTFOREIGNn
} myTenderOffsetTable [] = {
	{CLASS_MAINTTENDER1,   0, 1},
	{CLASS_MAINTTENDER2,   1, 1},
	{CLASS_MAINTTENDER3,   2, 1},
	{CLASS_MAINTTENDER4,   3, 1},
	{CLASS_MAINTTENDER5,   4, 1},
	{CLASS_MAINTTENDER6,   5, 1},
	{CLASS_MAINTTENDER7,   6, 1},
	{CLASS_MAINTTENDER8,   7, 1},
	{CLASS_MAINTTENDER9,   8, 1},
	{CLASS_MAINTTENDER10,  9, 1},
	{CLASS_MAINTTENDER11, 10, 1},
	{CLASS_MAINTTENDER12, 11, 1},
	{CLASS_MAINTTENDER13, 12, 1},
	{CLASS_MAINTTENDER14, 13, 1},
	{CLASS_MAINTTENDER15, 14, 1},
	{CLASS_MAINTTENDER16, 15, 1},
	{CLASS_MAINTTENDER17, 16, 1},
	{CLASS_MAINTTENDER18, 17, 1},
	{CLASS_MAINTTENDER19, 18, 1},
	{CLASS_MAINTTENDER20, 19, 1},
	{CLASS_MAINTFOREIGN1,  0, 2},
	{CLASS_MAINTFOREIGN2,  1, 2},
	{CLASS_MAINTFOREIGN3,  2, 2},
	{CLASS_MAINTFOREIGN4,  3, 2},
	{CLASS_MAINTFOREIGN5,  4, 2},
	{CLASS_MAINTFOREIGN6,  5, 2},
	{CLASS_MAINTFOREIGN7,  6, 2},
	{CLASS_MAINTFOREIGN8,  7, 2}
};

/*
*======================================================================================
**   Synopsis:  VOID MaintAllPickupAmountSet1(MAINTLOANPICKUP *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for MAINTLOANPICKUP
*      Output:  Nothing
*
**  Return:     Nothing
*
**  Description: SetAll Pickup Amount
======================================================================================
*/
VOID    MaintAllPickupAmountSet1(MAINTLOANPICKUP *pData)
{
    SHORT   i;

	pData->lForQty = 0;
	pData->lAmount = 0;

	for (i = 0; i < sizeof(myTenderOffsetTable)/sizeof(myTenderOffsetTable[0]); i++) {
		if (pData->uchMinorClass == myTenderOffsetTable[i].uchMinorClass) {
			if (myTenderOffsetTable[i].usType == 1) {
				SHORT iIndex = myTenderOffsetTable[i].usOffset;

				pData->lForQty = (LONG)MaintAllPickup.aTender[iIndex].sCounter;
				pData->lAmount = MaintAllPickup.aTender[iIndex].lAmount;
			}
			else if (myTenderOffsetTable[i].usType == 2) {
				SHORT iIndex = myTenderOffsetTable[i].usOffset;

				pData->lForQty = (LONG)MaintAllPickup.aForeignCurrency[iIndex].sCounter;
				pData->lAmount = MaintAllPickup.aForeignCurrency[iIndex].lAmount;
			}
			break;
		}
	}
}

/*
*======================================================================================
**   Synopsis:  VOID MaintAllPickupAmountSet2(MAINTLOANPICKUP *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for MAINTLOANPICKUP
*      Output:  Nothing
*
**  Return:     Nothing
*
**  Description: SetAll Pickup Amount
======================================================================================
*/
VOID    MaintAllPickupAmountSet2(MAINTLOANPICKUP *pData)
{
    SHORT   i;

	for (i = 0; i < sizeof(myTenderOffsetTable)/sizeof(myTenderOffsetTable[0]); i++) {
		if (pData->uchMinorClass == myTenderOffsetTable[i].uchMinorClass) {
			if (myTenderOffsetTable[i].usType == 1) {
				SHORT iIndex = myTenderOffsetTable[i].usOffset;

				MaintAllPickup.aTender[i].sCounter += (SHORT)pData->lForQty;
				MaintAllPickup.aTender[i].lAmount += pData->lAmount;
			}
			else if (myTenderOffsetTable[i].usType == 2) {
				SHORT iIndex = myTenderOffsetTable[i].usOffset;

				MaintAllPickup.aForeignCurrency[i].sCounter += (SHORT)pData->lForQty;
				MaintAllPickup.aForeignCurrency[i].lAmount += pData->lAmount;
			}
			break;
		}
	}

}

/*
*======================================================================================
**   Synopsis:  VOID MaintPickupFCAmountSet(MAINTLOANPICKUP *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for MAINTLOANPICKUP
*      Output:  Nothing
*
**  Return:     SUCCESS
*               LDT_PROGHBT_ADR
*
**  Description: Pickup Foreign Amount Caluculation
======================================================================================
*/
SHORT   MaintPickupFCAmountSet(MAINTLOANPICKUP *pData)
{
    PARAMDC         ParaMDC = {0};
	PARACURRENCYTBL ParaFCRate = {0};
    USHORT          usClassOffset;
    UCHAR           uchFCType;

    ParaFCRate.uchMajorClass = CLASS_PARACURRENCYTBL;

    ParaMDC.uchMajorClass = CLASS_PARAMDC;

    switch (pData->uchMinorClass) {
    case CLASS_MAINTFOREIGN1:
        ParaFCRate.uchAddress = CNV_NO1_ADR;
        uchFCType = RND_FOREIGN1_1_ADR;
        ParaMDC.usAddress = MDC_FC1_ADR;
        break;

    case CLASS_MAINTFOREIGN2:
        ParaFCRate.uchAddress = CNV_NO2_ADR;
        uchFCType = RND_FOREIGN2_1_ADR;
        ParaMDC.usAddress = MDC_FC2_ADR;
        break;

    case CLASS_MAINTFOREIGN3:
        ParaFCRate.uchAddress = CNV_NO3_ADR;
        uchFCType = RND_FOREIGN3_1_ADR;
        ParaMDC.usAddress = MDC_FC3_ADR;
        break;

    case CLASS_MAINTFOREIGN4:
        ParaFCRate.uchAddress = CNV_NO4_ADR;
        uchFCType = RND_FOREIGN4_1_ADR;
        ParaMDC.usAddress = MDC_FC4_ADR;
        break;

    case CLASS_MAINTFOREIGN5:
        ParaFCRate.uchAddress = CNV_NO5_ADR;
        uchFCType = RND_FOREIGN5_1_ADR;
        ParaMDC.usAddress = MDC_FC5_ADR;
        break;

    case CLASS_MAINTFOREIGN6:
        ParaFCRate.uchAddress = CNV_NO6_ADR;
        uchFCType = RND_FOREIGN6_1_ADR;
        ParaMDC.usAddress = MDC_FC6_ADR;
        break;

    case CLASS_MAINTFOREIGN7:
        ParaFCRate.uchAddress = CNV_NO7_ADR;
        uchFCType = RND_FOREIGN7_1_ADR;
        ParaMDC.usAddress = MDC_FC7_ADR;
        break;

    case CLASS_MAINTFOREIGN8:
        ParaFCRate.uchAddress = CNV_NO8_ADR;
        uchFCType = RND_FOREIGN8_1_ADR;
        ParaMDC.usAddress = MDC_FC8_ADR;
        break;

    default:
        break;
    }

    CliParaRead(&ParaMDC);
    CliParaRead(&ParaFCRate);

    switch(pData->uchMinorClass){
    case CLASS_MAINTFOREIGN2:
    case CLASS_MAINTFOREIGN4:
    case CLASS_MAINTFOREIGN6:
    case CLASS_MAINTFOREIGN8:
		// Need to shift down since even numbered MDC addresses stored in upper nibble
		// The print logic assumes MDC bits are in the lower nibble.
        pData->uchFCMDC = (UCHAR)(ParaMDC.uchMDCData >> 4);
        break;

    default:
        pData->uchFCMDC = ParaMDC.uchMDCData;
    }
    usClassOffset     = pData->uchMinorClass - CLASS_MAINTFOREIGN1;
    usClassOffset     /= 2;
    ParaMDC.usAddress = MDC_EUROFC_ADR + usClassOffset;
    CliParaRead(&ParaMDC);

    if ( (pData->uchMinorClass == CLASS_MAINTFOREIGN2) ||
         (pData->uchMinorClass == CLASS_MAINTFOREIGN6) ) {
        ParaMDC.uchMDCData >>= 2;
    }
    if ( (pData->uchMinorClass == CLASS_MAINTFOREIGN3) ||
         (pData->uchMinorClass == CLASS_MAINTFOREIGN7) ) {
        ParaMDC.uchMDCData >>= 4;
    }
    if ( (pData->uchMinorClass == CLASS_MAINTFOREIGN4) ||
         (pData->uchMinorClass == CLASS_MAINTFOREIGN8) ) {
        ParaMDC.uchMDCData >>= 6;
    }
    pData->uchFCMDC2 = ParaMDC.uchMDCData;

    if (ParaFCRate.ulForeignCurrency == 0L) {             /* check fc rate */
		NHPOS_NONASSERT_NOTE("==PROVISIONING", "==PROVISIONING: LDT_PROHBT_ADR - ParaFCRate.ulForeignCurrency == 0L");
        return(LDT_PROHBT_ADR);
    }
    pData->ulFCRate = ParaFCRate.ulForeignCurrency;   /* save rate for print */

    /* convert foreign amount to native amount */
    if (CliParaMDCCheck(MDC_EURO_ADR, EVEN_MDC_BIT0)) {
		DCURRENCY    lNative = 0;
		UCHAR        uchAddress;
		UCHAR        uchMDCData;

		/* Euro enhancement, V2.2 */
        uchAddress = (UCHAR)(pData->uchMinorClass - CLASS_MAINTFOREIGN1 + 1);
		uchMDCData = MaintCurrencyMDCRead(uchAddress);
        if ((uchMDCData & ODD_MDC_BIT1) && (pData->uchMinorClass != CLASS_MAINTFOREIGN1)) {
            /* convert FC to FC through Euro */
            ItemTendFCRateCalcByEuroB(&lNative, pData->lAmount, pData->ulFCRate, uchFCType, FC_EURO_TYPE1, uchMDCData);
        } else {
            ItemTendFCRateCalc1(uchMDCData, &lNative, pData->lAmount, pData->ulFCRate, uchFCType);
        }
		pData->lNativeAmount = lNative;
    } else {
		DCURRENCY    lNative = 0;
        RflRateCalc2(&lNative, pData->lAmount, pData->ulFCRate, uchFCType);
		pData->lNativeAmount = lNative;
    }

    return(SUCCESS);
}


/* --- End of Source File --- */