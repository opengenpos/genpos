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
* Title       : Media Pick Up Module
* Category    : User Interface For Supervisor, NCR 2170 US GP Application
* Program Name: UIFAC11.C
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
*               UifAC11Function()       : A/C No.11 Function Entry Mode
*               UifAC11EnterCashier()   : A/C No.11 Enter Cashier ID Mode
*               UifAC11EnterAmount()    : A/C No.11 Enter Loan Amount Mode
*               UifAC11ErrorCorrect()   : A/C No.11 Error Correct Mode
*               UifAC11Exit()           : A/C No.11 Exit
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev.  :   Name     : Description
* Mar-09-93: 00.00.01  : M.Ozawa    : initial
* Mar-02-94: 01.01.00  : M.INOUE    : GP US r1.1
* Oct-03-96: 02.00.00  : Y.Sakuma   : Change UifAC11EnterAmount() for
*                                     Foreign Currency #3-8
* Dec-01-96: 02.00.00  : Y.Sakuma   : Change UifAC11
* Nov-30-99: 01.00.00  : hrkato     : Saratoga
* Apr-03-15: 02.02.01  : R.Chambers : cleanup source for Loan and Pickup in SUP mode.
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
**  Synopsis: SHORT UifAC11Function(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No.11 Function Entry Mode
*===============================================================================
*/
/* Define Next Function at UIM_INIT */

UIMENU CONST aChildAC11Init[] = {{UifAC11EnterCashier, CID_AC11ENTERCASHIER},
                                    {NULL,             0                }};

UIMENU CONST aChildAC11NxtFunc1[] = {{UifAC11EnterAmount, CID_AC11ENTERAMOUNT},
                                       {UifAC11Exit, CID_AC11EXIT},
                                       {NULL,           0             }};

UIMENU CONST aChildAC11NxtFunc2[] = {{UifAC11EnterAmount, CID_AC11ENTERAMOUNT},
                                       {UifAC11ErrorCorrect, CID_AC11ERRORCORRECT},
                                       {UifAC11Exit, CID_AC11EXIT},
                                       {NULL,           0             }};

SHORT   UifAC11Function(KEYMSG *pKeyMsg)
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC11Init);                            /* Open Next Function */
        return(SUCCESS);

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC11ENTERCASHIER:
        case CID_AC11ERRORCORRECT:
            UieNextMenu(aChildAC11NxtFunc1);                    /* Open Next Function (without E/C) */
            break;

        default:
            if (CliParaMDCCheckField (MDC_LOANPICK1_ADR, MDC_PARAM_BIT_A)) { /* prohibit e/c */
                UieNextMenu(aChildAC11NxtFunc1);                    /* Open Next Function (with E/C) */
            } else {
                UieNextMenu(aChildAC11NxtFunc2);                    /* Open Next Function (with E/C) */
            }
            break;
        }

	default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC11EnterCashier(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No.11 Enter Cashier ID Mode
*===============================================================================
*/

UISEQ CONST aszSeqAC11EnterCashier[] = {FSC_AC, FSC_SIGN_IN, FSC_B, FSC_MSR, 0};

SHORT UifAC11EnterCashier(KEYMSG *pKeyMsg)
{
	MAINTLOANPICKUP LoanPickupData = {0};
    SHORT           sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC11EnterCashier);    /* Register Key Sequence (R2.0) */

		UieCtrlDevice(UIE_ENA_MSR);             /* enable MSR R3.1 */

        MaintDisp(AC_PICKUP,                        /* A/C Number */
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
        case FSC_SIGN_IN:                   /* Added for R2.0 */
        case FSC_B:                         /* Added for R2.0 */
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {
				/* no data so repeat request for entry of operator id */
                return(LDT_ID_ADR);
            }
            if (pKeyMsg->SEL.INPUT.uchLen > UIFREG_MAX_ID_DIGITS) {    /* check number of digits */
                return(LDT_KEYOVER_ADR);
            }
            LoanPickupData.uchMajorClass = CLASS_MAINTPICKUP;      /* Set Major Class */
            LoanPickupData.uchMinorClass = CLASS_MAINTCASIN;       /* Set Minor Class */
            LoanPickupData.ulCashierNo = (ULONG)pKeyMsg->SEL.INPUT.lData;

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

			/* Print & Memory Affection Pickup Amount */
			MaintLoanPickupHeaderCtl(&LoanPickupData); /* R/J Header */
            MaintAllPickupAmountRead(LoanPickupData.ulCashierNo);  /* Read Tender Amount for All Pickup */

            UieEchoBack(UIE_ECHO_ROW0_REG, UIF_DIGIT13); /* Echo Back with Decimal Point */
            MaintDisp(AC_PICKUP,                        /* A/C Number */
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



TenderFscLookup UifAC1011FscLookupTable [] = {
	{ FSC_TENDER,  1, CLASS_MAINTTENDER1 },    // AC10/11 amount entry using tender key #1 indicates CLASS_TENDER1 which is normally Cash
	{ FSC_TENDER,  2, CLASS_MAINTTENDER2 },    // AC10/11 amount entry using tender key #2 indicates CLASS_TENDER2 which is normally Check
	{ FSC_TENDER,  3, CLASS_MAINTTENDER3 },
	{ FSC_TENDER,  4, CLASS_MAINTTENDER4 },
	{ FSC_TENDER,  5, CLASS_MAINTTENDER5 },
	{ FSC_TENDER,  6, CLASS_MAINTTENDER6 },
	{ FSC_TENDER,  7, CLASS_MAINTTENDER7 },
	{ FSC_TENDER,  8, CLASS_MAINTTENDER8 },
	{ FSC_TENDER,  9, CLASS_MAINTTENDER9 },
	{ FSC_TENDER, 10, CLASS_MAINTTENDER10 },
	{ FSC_TENDER, 11, CLASS_MAINTTENDER11 },
	{ FSC_TENDER, 12, CLASS_MAINTTENDER12 },
	{ FSC_TENDER, 13, CLASS_MAINTTENDER13 },
	{ FSC_TENDER, 14, CLASS_MAINTTENDER14 },
	{ FSC_TENDER, 15, CLASS_MAINTTENDER15 },
	{ FSC_TENDER, 16, CLASS_MAINTTENDER16 },
	{ FSC_TENDER, 17, CLASS_MAINTTENDER17 },
	{ FSC_TENDER, 18, CLASS_MAINTTENDER18 },
	{ FSC_TENDER, 19, CLASS_MAINTTENDER19 },
	{ FSC_TENDER, 20, CLASS_MAINTTENDER20 },
	{ FSC_FOREIGN, 1, CLASS_MAINTFOREIGN1 },
	{ FSC_FOREIGN, 2, CLASS_MAINTFOREIGN2 },
	{ FSC_FOREIGN, 3, CLASS_MAINTFOREIGN3 },
	{ FSC_FOREIGN, 4, CLASS_MAINTFOREIGN4 },
	{ FSC_FOREIGN, 5, CLASS_MAINTFOREIGN5 },
	{ FSC_FOREIGN, 6, CLASS_MAINTFOREIGN6 },
	{ FSC_FOREIGN, 7, CLASS_MAINTFOREIGN7 },
	{ FSC_FOREIGN, 8, CLASS_MAINTFOREIGN8 },
	{           0, 0, 0}
};

/*
*=============================================================================
**  Synopsis: SHORT UifAC11EnterAmount(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No.11 Enter Loan Amount Mode
*===============================================================================
*/

UISEQ CONST aszSeqAC11EnterAmount[] = {FSC_QTY, FSC_VOID, FSC_TENDER, FSC_FOREIGN, 0};

SHORT UifAC11EnterAmount(KEYMSG *pKeyMsg)
{
    MAINTLOANPICKUP LoanPickupData = {0};
    SHORT           sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UifAC1011KeyStatusClear();
        UieOpenSequence(aszSeqAC11EnterAmount);            /* Register Key Sequence */
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
            /* Check Modifier Status */
            if(UifAC1011KeyStatus.usModifier & (MAINT_MODIFIER_QTY)){
                return(LDT_SEQERR_ADR);
            }

            /* Set Modifier Status & Data */
            switch(pKeyMsg->SEL.INPUT.uchMajor){
                case FSC_QTY:
                    UifAC1011KeyStatus.usModifier |= MAINT_MODIFIER_QTY;
                    UifAC1011KeyStatus.lForQty = pKeyMsg->SEL.INPUT.lData;
                    break;
            }

            LoanPickupData.uchMajorClass = CLASS_MAINTPICKUP;      /* Set Major Class */
            LoanPickupData.uchMinorClass = CLASS_MAINTOTHER;          /* Set Minor Class */
            LoanPickupData.usModifier = UifAC1011KeyStatus.usModifier;
            LoanPickupData.lAmount = pKeyMsg->SEL.INPUT.lData;
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
            if (pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            LoanPickupData.uchMajorClass = CLASS_MAINTPICKUP;      /* Set Major Class */
            LoanPickupData.uchMinorClass = CLASS_MAINTOTHER;          /* Set Minor Class */
            UifAC1011KeyStatus.usModifier |= MAINT_MODIFIER_VOID;
            LoanPickupData.usModifier = UifAC1011KeyStatus.usModifier;

            if (pKeyMsg->SEL.INPUT.uchLen) {                      /* display amount */
                LoanPickupData.lAmount = pKeyMsg->SEL.INPUT.lData;
            } else {
                LoanPickupData.lAmount = UifAC1011KeyStatus.lForQty;
            }
            MaintLoanPickupDisp(&LoanPickupData);                     /* Display Void */
            return(SUCCESS);

        case FSC_TENDER:
        case FSC_FOREIGN:
            if (pKeyMsg->SEL.INPUT.uchDec != 0xFF) {                  /* W/ Decimal Point */
                return(LDT_KEYOVER_ADR);
            }
            if (pKeyMsg->SEL.INPUT.uchLen > 7) {                      /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            /* Check W/ Amount or W/o Amount for All pick up determination */
            LoanPickupData.uchLoanPickStatus = 0;
            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount */
				// no data however perhaps we are doing a Pickup of all currency of this type.
				// function MaintPickupAmountSet() will perform checks on this.
                if(UifAC1011KeyStatus.usModifier & (MAINT_MODIFIER_QTY)) {
                    return(LDT_SEQERR_ADR);
                }
                LoanPickupData.uchLoanPickStatus |= MAINT_WITHOUT_DATA;     /* All Pickup */
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

            if ((sError = MaintPickupAmountSet(&LoanPickupData)) == SUCCESS) {
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
**  Synopsis: SHORT UifAC11ErrorCorrect(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Key Sequence Error
*
**  Description: A/C No.11 Error Correct Mode
*===============================================================================
*/
SHORT   UifAC11ErrorCorrect(KEYMSG *pKeyMsg)
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
            }                                                /* W/o Amount Input Case */

            LoanPickupData.uchMinorClass = CLASS_MAINTOTHER;
            LoanPickupData.usModifier |= MAINT_MODIFIER_EC;
            sError = MaintLoanPickupErrorCorrect(&LoanPickupData);
            if (sError) return(sError);
            UieAccept();
            return(SUCCESS);
        }
        /* no break - fall through for default processing */
    default:
        return(UifACDefProc(pKeyMsg));
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC11Exit(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No. 11 Exit Function Mode
*===============================================================================
*/
UISEQ CONST aszSeqAC11Exit[] = {FSC_AC, 0};

SHORT   UifAC11Exit(KEYMSG *pKeyMsg)
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC11Exit);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
		if (pKeyMsg->SEL.INPUT.uchMajor == FSC_AC) {
			MAINTLOANPICKUP LoanPickupData = {0};

            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
                return(LDT_SEQERR_ADR);
			}

			PifLog (MODULE_PRINT_SUPPRG_ID, LOG_EVENT_UIFUIE_ACTIONCODE);
			PifLog (MODULE_DATA_VALUE(MODULE_PRINT_SUPPRG_ID), 11);

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

/* --- End of source File --- */