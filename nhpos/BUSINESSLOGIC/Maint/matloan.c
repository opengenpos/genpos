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
* Title       : Loan Module
* Category    : User Interface For Supervisor, NCR 2170 US GP Application
* Program Name: MATLOAN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               MaintLoanAmountSet()    : Loan Amount Set
*               MaintLoanErrorCorrect() : Loan Amount Error Correct
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev.   :   Name    : Description
* Mar-09-93:00.00.01    :M.Ozawa    : initial
* Mar-11-94:            :M.INOUE    : GPUS r1.1
* Oct-15-96: 02.00.00   : T.Yatuhasi: Change MaintLoanAmountSet
*                                     for Foreign Currency #3-8
* Nov-30-99: 01.00.00   : hrkato    : Saratoga
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
#include "regstrct.h"
#include "uie.h"
#include "pif.h"
#include "rfl.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
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

/*
*======================================================================================
**   Synopsis:  SHORT MaintLoanAmountSet(MAINTLOANPICKUP *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for MAINTLOANPICKUP
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: Loan Amount Set
======================================================================================
*/
SHORT MaintLoanAmountSet(MAINTLOANPICKUP *pData )
{
    SHORT           sError, sFC = 0;
    ITEMAFFECTION   Affect;
	CHAR uchTmpMinorClass; //saves minor class data US Customs SCER 3/27/03 cwunn

    /* compulsory for/qty check */
    if (CliParaMDCCheck(MDC_LOANPICK1_ADR, ODD_MDC_BIT1)) {
        if(pData->lForQty == 0L){
            return(LDT_PROHBT_ADR);
        }
    }

	// The following is to work around a field issue seen on some Amtrak trains in which
	// the initialization sequence which beings with the Start Log In screen in which
	// the LSA enters Employee Id, Secret Code, and a Loan Amount followed by starting up
	// the Inventory app for other train trip startup activities does not complete properly.
	// The result is that the LSA is unable to complete the Start Log In screen task and
	// is stuck with it partially complete. A control string is used to automate this task
	// so as part of the sanity check as to whether to allow this workaround, check that a
	// control string is running.
	//
	// This change checks if the Loan has already been made and if so, just report success.
	//      Richard Chambers, Aug-17-2018, for Amtrak only
	if (ItemCheckCustomerSettingsFlags (SYSCONFIG_CUSTOMER_ENABLE_AMTRAK) && UieMacroRunning()) {
		ULONG  ulCashierNo = MaintWork.LoanPickup.ulCashierNo;

		if (ulCashierNo && ItemTotalCashierCheckLoanTotalZero (ulCashierNo) != TTL_SUCCESS)
			return TRN_SUCCESS;
	}

	/* Calculate Loan Amount */
    if(pData->usModifier & MAINT_MODIFIER_QTY){       /* QTY Caluculation */
        if(pData->lForQty){
			D13DIGITS      d13ProdWork;

            if (pData->lForQty > STD_ATFOR_MAX_QTY) {
                return(LDT_KEYOVER_ADR);
            }

            d13ProdWork = pData->lForQty;
			d13ProdWork *= pData->lAmount;
            if (d13ProdWork >= 0L && d13ProdWork <= STD_PLU_MAX_PRICE) {   /* below max. sales price ? */
                if (d13ProdWork < pData->lAmount) {                        /* if overflow is occured */
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

    switch(pData->uchMinorClass) {     /* Foreign Currency Caluculation */
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
				return(sError);    /* Foreign Currency rate is not setted */
			}
		default:
			break;
    }

    if(pData->usModifier & MAINT_MODIFIER_VOID){      /* Void Calculation */
        pData->lAmount *= -1;
        pData->lForQty *= -1;
        pData->lNativeAmount *= -1;
		pData->usModifier |= VOID_MODIFIER;/* */
    }

    /* Save Loan Amount Data */
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
        sFC = 1;
        break;

    default:
        MaintWork.LoanPickup.lTotal += pData->lAmount;
    }

	/* Display Loan Amount */
	MaintLoanPickupDisp(&(MaintWork.LoanPickup));

	if (MaintWork.LoanPickup.uchLoanPickStatus & MAINT_DATA_INPUT_CE) {
		MaintWork.LoanPickup.HeadTrail.usPrintControl = PRT_JOURNAL;
	} else {
		MaintWork.LoanPickup.HeadTrail.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;
		if (CliParaMDCCheck(MDC_LOANPICK2_ADR, EVEN_MDC_BIT0)) {
			MaintWork.LoanPickup.HeadTrail.usPrintControl |= PRT_SLIP;
		}
	}

	/* Print & Memory Affection Loan Amount */
    if (TrnLoanPickup(&(MaintWork.LoanPickup)) == TRN_SUCCESS) {
        MaintWork.LoanPickup.uchLoanPickStatus |= MAINT_DATA_INPUT;
    }

	//Print Pickup mnemonic and amount US Customs SCER 3/27/03 cwunn
	uchTmpMinorClass = MaintWork.LoanPickup.uchMinorClass; //save minor class data
	MaintWork.LoanPickup.uchMinorClass = CLASS_MAINTLOAN;
	TrnLoanPickup(&(MaintWork.LoanPickup));
	//Return MinorClass data to what it was before SCER code (just in case its needed elsewhere)
	MaintWork.LoanPickup.uchMinorClass = uchTmpMinorClass;

    memset(&Affect, 0, sizeof(ITEMAFFECTION));
    Affect.uchMajorClass = CLASS_ITEMAFFECTION;
    Affect.uchMinorClass = CLASS_LOANAFFECT;
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

		ConnEngineSendCashierActionWithAddon (CONNENGINE_ACTIONTYPE_LOAN, aszSapId, tcsAddOnString);
	}

    return(sError);
    
    /* return(SUCCESS); */
}

/* --- End of Source File --- */