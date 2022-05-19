/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-99          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Media Loan Module
* Category    : User Interface For Supervisor, NCR 2170 US GP Application
* Program Name: UIFAC10.C
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
*               UifAC10Function()       : A/C No.10 Function Entry Mode
*               UifAC10EnterCashier()   : A/C No.10 Enter Cashier ID Mode
*               UifAC10EnterAmount()    : A/C No.10 Enter Loan Amount Mode
*               UifAC10ErrorCorrect()   : A/C No.10 Error Correct Mode
*               UifAC10Exit()           : A/C No.10 Exit
* --------------------------------------------------------------------------
* Update Histories
*    Date   : Ver.Rev.  :   Name     : Description
* Mar-08-93 : 00.00.01  : M.Ozawa    : initial
* Mar-01-94 : 00.01.01  : M.INOUE    : GPUS R1.1 F/C tender1,2
* Dec-01-96 : 02.00.00  : Y.Sakuma   : Change UifAC10EnterCashier() for R2.0
* Dec-02-99 : 01.00.00  : hrkato     : Saratoga
* Apr-02-15 : 02.02.01  : R.Chambers : GenPOS, comments, eliminate duplicate range check on tender.
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
#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <rfl.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <csstbpar.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>
#include <uireg.h>
#include <uifpgequ.h>
#include "appllog.h"

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
*=============================================================================
**  Synopsis: SHORT UifAC10Function(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No.10 Function Entry Mode
*===============================================================================
*/
/* Define Next Function at UIM_INIT */

UIMENU CONST aChildAC10Init[] = {{UifAC10EnterCashier, CID_AC10ENTERCASHIER},
                                    {NULL,             0                }};

UIMENU CONST aChildAC10NxtFunc1[] = {{UifAC10EnterAmount, CID_AC10ENTERAMOUNT},
                                       {UifAC10Exit, CID_AC10EXIT},
                                       {NULL,           0             }};

UIMENU CONST aChildAC10NxtFunc2[] = {{UifAC10EnterAmount, CID_AC10ENTERAMOUNT},
                                       {UifAC10ErrorCorrect, CID_AC10ERRORCORRECT},
                                       {UifAC10Exit, CID_AC10EXIT},
                                       {NULL,           0             }};

SHORT UifAC10Function(KEYMSG *pKeyMsg)
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC10Init);                            /* Open Next Function */
        return(SUCCESS);

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC10ENTERCASHIER:
        case CID_AC10ERRORCORRECT:
            UieNextMenu(aChildAC10NxtFunc1);                    /* Open Next Function (without E/C) */
            break;

        default:
            if (CliParaMDCCheck(MDC_LOANPICK1_ADR, ODD_MDC_BIT3)) { /* prohibit e/c */
                UieNextMenu(aChildAC10NxtFunc1);                    /* Open Next Function (without E/C) */
            } else {
                UieNextMenu(aChildAC10NxtFunc2);                    /* Open Next Function (with E/C) */
            }
            break;
        }

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}



/*
*=============================================================================
**  Synopsis: SHORT UifAC10EnterCashier(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No.10 Enter Cashier ID Mode to determine which Cashier is
*                getting the Loan. We also look at the Sign-in key used when the
*                Cashier Id is entered to determine which of the two possible
*                drawers will the currency be put into.
*===============================================================================
*/
UISEQ CONST aszSeqAC10EnterCashier[] = {FSC_AC, FSC_SIGN_IN, FSC_B, FSC_MSR, 0};

SHORT UifAC10EnterCashier(KEYMSG *pKeyMsg)
{
	MAINTLOANPICKUP LoanPickupData = {0};
    SHORT           sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC10EnterCashier);    /* Register Key Sequence */

		UieCtrlDevice(UIE_ENA_MSR);             /* enable MSR R3.1 */

        MaintDisp(AC_LOAN,                          /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_ID_ADR);                      /* "Enter Operator Id" Lead Through Address */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
		case FSC_MSR:
			{
				// Process the MSR swipe data to pull the employee id from the card swipe.
				if (pKeyMsg->SEL.INPUT.DEV.MSR.uchLength2 >= 17) {
					memset (pKeyMsg->SEL.INPUT.aszKey, 0, sizeof(pKeyMsg->SEL.INPUT.aszKey));
					_tcsncpy (pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.DEV.MSR.auchTrack2 + 5, 8);   // pull employee id from data
					pKeyMsg->SEL.INPUT.uchLen = tcharlen(pKeyMsg->SEL.INPUT.aszKey);
					pKeyMsg->SEL.INPUT.lData = _ttol(pKeyMsg->SEL.INPUT.aszKey);
				}
			}
        case FSC_AC:
        case FSC_SIGN_IN:
        case FSC_B:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {
				/* no data so repeat request for entry of operator id */
                return(LDT_ID_ADR);
            }
            if (pKeyMsg->SEL.INPUT.uchLen > UIFREG_MAX_ID_DIGITS) {    /* check number of digits */
                return(LDT_KEYOVER_ADR);
            }

            LoanPickupData.uchMajorClass = CLASS_MAINTLOAN;
            LoanPickupData.uchMinorClass = CLASS_MAINTCASIN;
            LoanPickupData.ulCashierNo = (ULONG)pKeyMsg->SEL.INPUT.lData;
			LoanPickupData.uchLoanPickStatus = 0;

            if( pKeyMsg->SEL.INPUT.uchMajor == FSC_SIGN_IN ) {
                sError = MaintLoanPickupCasSignIn(&LoanPickupData, UISUP_DRAWER_A);
                if( sError ) {
                    return( sError );
                }
            } else if ( pKeyMsg->SEL.INPUT.uchMajor == FSC_B ) {
                sError = MaintLoanPickupCasSignIn(&LoanPickupData, UISUP_DRAWER_B);
                if( sError ) {
                    return( sError );
                }
            } else {
                sError = MaintLoanPickupCasSignIn(&LoanPickupData, 0);
                if( sError ) {
                    return( sError );
                }
            }

			/* Print & Memory Affection Loan Amount */
			MaintLoanPickupHeaderCtl(&LoanPickupData); /* R/J Header */

            /** set leadthrough message for next operation */
            UieEchoBack(UIE_ECHO_ROW0_REG, UIF_DIGIT13); /* Echo Back with Decimal Point */
            MaintDisp(AC_LOAN,                          /* A/C Number */
                      CLASS_MAINTDSPCTL_AMT,            /* Decimal Point */
                      0,                                /* Page Number */
                      0,                                /* PTD Type */
                      0,                                /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      LDT_DATA_ADR);                    /* "Amount Entry" Lead Through Address */

            UieAccept();
            return(sError);
        }
        /* no break - fall through for default processing */
    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC10EnterAmount(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No.10 Enter Loan Amount Mode
*===============================================================================
*/
UISEQ CONST aszSeqAC10EnterAmount[] = {FSC_QTY, FSC_VOID, FSC_TENDER, FSC_FOREIGN, 0};

SHORT UifAC10EnterAmount(KEYMSG *pKeyMsg)
{
    MAINTLOANPICKUP LoanPickupData = {0};
    SHORT           sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UifAC1011KeyStatusClear();
        UieOpenSequence(aszSeqAC10EnterAmount);            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_QTY:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount */
                return(LDT_DATA_ADR);
            }
            if (pKeyMsg->SEL.INPUT.uchDec != 0xFF) {                  /* W/ Decimal Point */
                return(LDT_KEYOVER_ADR);
            }
            if (pKeyMsg->SEL.INPUT.uchLen > 4) {                      /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            if (UifAC1011KeyStatus.usModifier & (MAINT_MODIFIER_QTY)){
                return(LDT_SEQERR_ADR);
            }

            /* Set Modifier Status */
            UifAC1011KeyStatus.usModifier |= MAINT_MODIFIER_QTY;

            LoanPickupData.uchMajorClass = CLASS_MAINTLOAN;        /* Set Major Class */
            LoanPickupData.uchMinorClass = CLASS_MAINTOTHER;          /* Set Minor Class */
            LoanPickupData.usModifier = UifAC1011KeyStatus.usModifier;
            LoanPickupData.lAmount = pKeyMsg->SEL.INPUT.lData;
			LoanPickupData.uchLoanPickStatus = 0;
            UifAC1011KeyStatus.lForQty = pKeyMsg->SEL.INPUT.lData;

            MaintLoanPickupDisp(&LoanPickupData);                               /* Display For, Qty */
            return(SUCCESS);

        case FSC_VOID:
            if (CliParaMDCCheck(MDC_LOANPICK1_ADR, ODD_MDC_BIT2)) { /* prohibit void */
                return(LDT_BLOCKEDBYMDC_ADR);
            }
            if (UifAC1011KeyStatus.usModifier & (MAINT_MODIFIER_VOID)){
                return(LDT_SEQERR_ADR);
            }

            /* Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                          /* Amount not allowed with void */
                return(LDT_SEQERR_ADR);
            }

            LoanPickupData.uchMajorClass = CLASS_MAINTLOAN;           /* Set Major Class */
            LoanPickupData.uchMinorClass = CLASS_MAINTOTHER;          /* Set Minor Class */
            UifAC1011KeyStatus.usModifier |= MAINT_MODIFIER_VOID;
            LoanPickupData.usModifier = UifAC1011KeyStatus.usModifier;
			LoanPickupData.uchLoanPickStatus = 0;

            if (pKeyMsg->SEL.INPUT.uchLen) {                      /* display amount */
                LoanPickupData.lAmount = pKeyMsg->SEL.INPUT.lData;
            } else {
                LoanPickupData.lAmount = UifAC1011KeyStatus.lForQty;
            }

            MaintLoanPickupDisp(&LoanPickupData);                     /* Display Void */
            return(SUCCESS);

        case FSC_TENDER:
        case FSC_FOREIGN:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount */
				// no data so repeat the request for the currency amount
                return(LDT_DATA_ADR);
            }
            if (pKeyMsg->SEL.INPUT.uchDec != 0xFF) {                  /* W/ Decimal Point */
                return(LDT_KEYOVER_ADR);
            }
            if (pKeyMsg->SEL.INPUT.uchLen > 7) {                      /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

			// amount input is followed by the tender key indicating the tender type that is the target of the Loan.
			{
				int i;
				for (i = 0; UifAC1011FscLookupTable[i].nFsc != 0; i++) {
					if (pKeyMsg->SEL.INPUT.uchMajor == UifAC1011FscLookupTable[i].nFsc &&
						pKeyMsg->SEL.INPUT.uchMinor == UifAC1011FscLookupTable[i].nVal) {
							LoanPickupData.uchMinorClass = UifAC1011FscLookupTable[i].nMaint;
							break;
					}
				}
				if (UifAC1011FscLookupTable[i].nFsc == 0) {
					return(LDT_SEQERR_ADR);
				}
			}
            LoanPickupData.usModifier = UifAC1011KeyStatus.usModifier;
            LoanPickupData.lForQty = UifAC1011KeyStatus.lForQty;
            LoanPickupData.lAmount = pKeyMsg->SEL.INPUT.lData;
			LoanPickupData.uchLoanPickStatus = 0;

            if ((sError = MaintLoanAmountSet(&LoanPickupData)) == SUCCESS) {
                 UieAccept();                                          /* return UifAC10EnterCashier() */
            }
            return(sError);
        }
        /* no break - fall through for default processing */
    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC10ErrorCorrect(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Key Sequence Error
*
**  Description: A/C No.10 Error Correct Mode
*===============================================================================
*/
SHORT   UifAC10ErrorCorrect(KEYMSG *pKeyMsg)
{
    SHORT           sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_EC);                            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        if (pKeyMsg->SEL.INPUT.uchMajor == FSC_EC) {
			MAINTLOANPICKUP LoanPickupData = {0};

            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount Input Case */
                return(LDT_SEQERR_ADR);
            }

            LoanPickupData.uchMinorClass = CLASS_MAINTOTHER;
            LoanPickupData.usModifier |= MAINT_MODIFIER_EC;
			LoanPickupData.uchLoanPickStatus = 0;
            sError = MaintLoanPickupErrorCorrect(&LoanPickupData);
            if (sError) return(sError);
            UieAccept();                                        /* Return to UifAC10CashierEntry() */
            return(SUCCESS);
        }
        /* no break - fall through for default processing */
    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC10Exit(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No. 114 Exit Function Mode
*===============================================================================
*/
UISEQ CONST aszSeqAC10Exit[] = {FSC_AC, 0};

SHORT   UifAC10Exit(KEYMSG *pKeyMsg)
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC10Exit);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
		if  (pKeyMsg->SEL.INPUT.uchMajor == FSC_AC) {
			MAINTLOANPICKUP LoanPickupData = {0};

            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }

			PifLog (MODULE_PRINT_SUPPRG_ID, LOG_EVENT_UIFUIE_ACTIONCODE);
			PifLog (MODULE_DATA_VALUE(MODULE_PRINT_SUPPRG_ID), 10);

			LoanPickupData.uchMinorClass = CLASS_MAINTCASOUT;
			LoanPickupData.uchLoanPickStatus = 0;
            MaintLoanPickupCasSignOut(&LoanPickupData);
            UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT13); /* Start Echo Back and Set Max Input Digit */
            UieExit(aACEnter);                              /* Return to UifACEnter() */
            return(SUCCESS);
        }
        /* no break - fall through for default processing */
    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: VOID UifAC1011KeyStatusClear( VOID)
*
*       Input:  nothing
*      Output:  nothing
**  Return:     nothing
*
**  Description: A/C No.10/11 Key Status Save Area Clear
*===============================================================================
*/

VOID UifAC1011KeyStatusClear( VOID )
{
    memset(&UifAC1011KeyStatus, 0x00, sizeof(UIFAC1011KEYSTATUS));
}

/* --- End of Source File --- */