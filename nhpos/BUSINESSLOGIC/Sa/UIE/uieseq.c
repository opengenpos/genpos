/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1993       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Key Seqeunce Module
* Category    : User Interface Enigne, NCR 2170 High Level System Application
* Program Name: UIESEQ.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Meduim/Large Model
* Options     : /c /A[M|Lw] /W4 /G[1|2]s /Os /Zap
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Sep-04-92  00.01.00  O.Nakada     Changed number of feed lines.
* Sep-08-92  00.01.01  O.Nakada     Add UieDisplayEchoBackWide() function
*                                   in UieDefProc().
* Jan-07-93  01.00.07  O.Nakada     Modify display enable flag in
*                                   UieSendRedisplay().
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Apr-17-95  G0        O.Nakada     Added macro function control in
*                                   UieCheckSeq().
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2 R.Chambers  Source cleanup to remove old, before NCR 7448,
*                                   code that applied to NCR 2170 back in the 1990s.
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
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "ecr.h"
#include "regstrct.h"
#include "paraequ.h"
#include "para.h"
#include "log.h"
#include <appllog.h>
#include "pif.h"
#include "pmg.h"
#include "uie.h"
#include "uiel.h"
#include "fsc.h"
#include "paraequ.h"
#include "uiefunc.h"
#include "uieseq.h"
#include "uireg.h"
#include "uiedialo.h"
#include "UIELIO.H"
#include "uieio.h"
#include "display.h"
#include <rfl.h>
#include "uiedev.h"
#include "csstbpar.h"
#include "uiedisp.h"
#include "uieecho.h"
#include "uieerror.h"

/*
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/
UIEFUNCLINK UieFuncTbl[UIE_MAX_FUNC_TBL];   /* Function Link Table */

UCHAR iuchUieRootFunc[UIE_MAX_FUNC];        /* Root Function Pointer */
UCHAR iuchUieActiveFunc[UIE_MAX_FUNC];      /* Active Function Pointer */
UCHAR iuchUieCurrentFunc[UIE_MAX_FUNC];     /* Current Function Pointer */
UCHAR iuchUieFuncSts;                       /* Function Status Buffer */

TCHAR UieRetryBuf[UIE_MAX_RETRY_BUF];       /* Retry Buffer */


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UieCheckSequence(KEYMSG *pData)
*     Input:    pData - Message Data
*
** Return:      Function Status
*                1 >= Error Code
*                0 == Successful
*               -1 == Successful Dialog Function
*               -2 == Cancel Dialog Function
*
** Description:
*===========================================================================
*/
SHORT UieCheckSequence(KEYMSG *pData)
{
    SHORT   sRet;

    for (;;) {
        sRet = UieCheckSeq(pData);

		if (sRet == LDT_POST_ODRDEC_EVENT)
		{
			int iMsgCount = 0;

			sRet = UieGetKeyMsg(pData, iMsgCount);							// get previous key info
			while (sRet == SUCCESS) {
				sRet = UieCheckSeq(pData);						// process event
				if (sRet == SUCCESS)
				{
					iMsgCount++;
					sRet = UieGetKeyMsg (pData, iMsgCount);				// attempt to get key info from 2nd position
				}
			}
		}
    	if (UieRetry.fchStatus & UIE_CLEAR_RETRY_BUF) {         /* retry buffer full */
		    UieClearRetryBuf();
			UieClearKeyboardBuf();	//Clear the KeyBoard Buffer
            break;                                              /* exit loop */
    	} else {
    	    if (UieRetry.fchStatus & UIE_RETRY) {               /* execute retry */
    	        UieGetRetryBuf(pData);
        	} else {
                if (UieRetry.fchStatus & UIE_SKIP_RETRY_WRT ||
                    UieRetry.fchStatus & UIE_DEF_PROC) {              /* exexuted default process */
                    UieRetry.fchStatus &= ~(UIE_SKIP_RETRY_WRT |
                                            UIE_DEF_PROC);
                } else {
                    if (pData->uchMsg == UIM_INPUT) {           /* input message */
                        UieWriteRetryBuf(pData);
                    }
                }
                break;                                          /* exit loop */
            }
    	}
    }

    return (sRet);
}


/*
*===========================================================================
** Synopsis:    SHORT UieCheckSeq(KEYMSG *pData)
*     Input:    pData - Message Data
*
** Return:      Function Status
*                1 >= Error Code
*                0 == Successful
*               -1 == Successful Dialog Function
*               -2 == Cancel Dialog Function
*
** Description:
*===========================================================================
*/
static  SHORT sExtendedStatus = 0;
SHORT UieSetExtendedStatus (SHORT sRetStatus)
{
	sExtendedStatus = sRetStatus;
	return sExtendedStatus;
}

SHORT UieCheckSeq(KEYMSG *pData)
{
    SHORT sRet;
    UCHAR iuchCurrent = iuchUieCurrentFunc[iuchUieFuncSts];

    if (UieFuncTbl[iuchCurrent].pFunction == NULL) {    /* invalid table */
        PifAbort(MODULE_UIE_CHECKSEQ, FAULT_INVALID_FUNCTION);
    }

    switch (pData->uchMsg) {
    case UIM_MODE:
    case UIM_DRAWER:
        sRet = (*UieFuncTbl[iuchUieCurrentFunc[iuchUieFuncSts]].pFunction->sFunc)(pData);
        break;

    case UIM_ERROR:
        sRet = pData->SEL.usFunc;
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_CLEAR) {                   /* CLEAR Key */
            UieSendCancel(iuchCurrent);
            sRet = UieSendRedisplay((USHORT)(!(UieGetEchoStatus() & UIE_INIT_ECHO_BACK))); /* ### Mod (2171 for Win32) V1.0 */
            UieRetry.fchStatus |= UIE_CLEAR_RETRY_BUF;                  /* Set to Clear Retry Buffer Flag */
        } else if (UieRetry.fchStatus & UIE_ACCEPT) {                   /* Accept */
            sRet = UieFindSeq(pData, iuchUieCurrentFunc[iuchUieFuncSts], UIE_LINK_EMPTY);
            if (UIE_ERR_KEY_SEQ < sRet && sRet <= UIE_SUC_KEY_SEQ) {  	/* Successful */
                UieRetry.fchStatus &= ~UIE_ACCEPT;                      /* Reset to Accept Flag */
			} else if (UIE_SUC_KEY_TRANSPARENT == sRet) {
				sRet = UIE_SUC_KEY_SEQ;
			}
        } else {                                                        /* not Accept */
            if (((sRet = UieCurrentFunc(pData)) == UIE_ERR_KEY_SEQ) && !(UieRetry.fchStatus & UIE_RETRY_FULL)) {
                sRet = UieRetryFunc(pData);
            }
        }
        break;

    default:                                                            /* not Use */
		sRet = 0;
		{
			char xBuff[128];
			sprintf (xBuff, "Bad Switch uchMsg=%d, usFunc=%d, uchMajor=%d, uchMinor=%d", pData->uchMsg, pData->SEL.usFunc, pData->SEL.INPUT.uchMajor, pData->SEL.INPUT.uchMinor);
			NHPOS_ASSERT_TEXT((pData->uchMsg == UIM_INPUT),xBuff);
		}
        break;
    }

    if (sRet == UIE_ERR_KEY_SEQ) {
        iuchUieCurrentFunc[iuchUieFuncSts] = iuchCurrent;
        sRet = (*UieFuncTbl[iuchCurrent].pFunction->sFunc)(pData);      /* Default Procedure */
        UieRetry.fchStatus |= UIE_DEF_PROC;                             /* Set to Default Procedure Flag */
    }

    if (sRet > UIE_SUC_KEY_SEQ) {										/* Error */
		if (sRet == LDT_POST_ODRDEC_EVENT)
		{
			return sRet;
		}

		if (UieMacro.fsControl & UIE_MACRO_RUNNING) {
			PifLog (MODULE_UIE_CHECKSEQ, LOG_EVENT_STRING_SEQ_ERROR);
			PifLog (MODULE_ERROR_NO(MODULE_UIE_CHECKSEQ), (USHORT)abs(sRet));
			PifLog (MODULE_ERROR_NO(MODULE_UIE_CHECKSEQ), (USHORT)UieMacro.usControlStringNumber);
			PifLog (MODULE_ERROR_NO(MODULE_UIE_CHECKSEQ), (USHORT)UieMacro.usRead);
			PifLog (MODULE_ERROR_NO(MODULE_UIE_CHECKSEQ), (USHORT)UieMacro.usNumber);
		}

		if (pData->uchMsg == UIM_ERROR && pData->SEL.usFunc == LDT_DRAWCLSE_ADR) {
			// This is a close cash drawer error message so
			UifRegCloseDrawerWarning(-1);
		} else {
	        UieErrorMsgExtended((USHORT)sRet, UIE_WITHOUT, sExtendedStatus);
			sExtendedStatus = 0;
		}
        UieSendCancel(iuchUieCurrentFunc[iuchUieFuncSts]);
        UieSendRedisplay(1);
        UieSetMacro(0, NULL, 0);               /* disable macro function */
    }

    return (sRet);
}


/*
*===========================================================================
** Synopsis:    SHORT UieFindSeq(KEYMSG *pData, UCHAR iuchCurrent,
*                                UCHAR iuchExclude)
*     Input:    pData       - Message Data
*               iuchCurrent - Current Function Index
*               iuchExluce  - Exclude Function Index
*
** Return:      UIE_ERR_KEY_SEQ - Key Sequence Error
*               other           - Function Status
*
** Description:
*===========================================================================
*/
SHORT UieFindSeq(KEYMSG *pData, UCHAR iuchCurrent, UCHAR iuchExclude)
{
    UCHAR iuchExit;
	SHORT sRetStatus;

    iuchExit = iuchUieActiveFunc[iuchUieFuncSts];

    for (;;) {
        if (UieFuncTbl[iuchCurrent].iuchChild != UIE_LINK_EMPTY) {
            iuchCurrent = UieFuncTbl[iuchCurrent].iuchChild;
            continue;
        }

        if (iuchCurrent != iuchExclude) {                               /* Exclude Function */
            if (UieFuncTbl[iuchCurrent].pSequence) {
                if (strchr(UieFuncTbl[iuchCurrent].pSequence, (SHORT)pData->SEL.INPUT.uchMajor) != NULL) {
                    iuchUieCurrentFunc[iuchUieFuncSts] = iuchCurrent;

                    if (UieRetry.fchStatus & UIE_RETRY) {
                        return (UIE_SUC_KEY_SEQ);						/* Successful */
                    }

                    sRetStatus = ((*UieFuncTbl[iuchCurrent].pFunction->sFunc)(pData));
					if (sRetStatus != UIE_SUC_KEY_IGNORE)
						return sRetStatus;
                }
            }
        }

        for (;;) {
            if (UieFuncTbl[iuchCurrent].iuchYBro != UIE_LINK_EMPTY) {
                iuchCurrent = UieFuncTbl[iuchCurrent].iuchYBro;
                break;
            } else {
                if (iuchCurrent == iuchExit) {
                    return (UIE_ERR_KEY_SEQ);
                }

                iuchCurrent = UieFuncTbl[iuchCurrent].iuchParent;
            }
        }
    }
}


/*
*===========================================================================
** Synopsis:    SHORT UieCurrentFunc(KEYMSG *pData)
*     Input:    pData - Message Data
*
** Return:      UIE_ERR_KEY_SEQ - Key Sequence Error
*               other           - Function Status
*
** Description:
*===========================================================================
*/
SHORT UieCurrentFunc(KEYMSG *pData)
{
    UCHAR iuchCurrent = iuchUieCurrentFunc[iuchUieFuncSts];

    if (UieFuncTbl[iuchCurrent].pSequence) {
        if (strchr(UieFuncTbl[iuchCurrent].pSequence, (SHORT)pData->SEL.INPUT.uchMajor) != NULL) {
            return ((*UieFuncTbl[iuchCurrent].pFunction->sFunc)(pData));
        }
    }
    return (UIE_ERR_KEY_SEQ);
}


/*
*===========================================================================
** Synopsis:    SHORT UieRetryFunc(KEYMSG *pData)
*     Input:    pData - Message Data
*
** Return:      UIE_SUC_KEY_SEQ - Successful
*               UIE_ERR_KEY_SEQ - Retry Error
*
** Description:
*===========================================================================
*/
SHORT UieRetryFunc(KEYMSG *pData)
{
    UCHAR iuchCurrent = iuchUieCurrentFunc[iuchUieFuncSts];

    UieRetry.fchStatus |= UIE_RETRY;                            /* Set to Retry Flag */
    for (;;) {
        if (UieFindSeq(pData, iuchUieCurrentFunc[iuchUieFuncSts], iuchUieCurrentFunc[iuchUieFuncSts]) == UIE_ERR_KEY_SEQ) {                                  /* Key Sequence Error */
            UieRetry.fchStatus &= ~UIE_RETRY;                   /* Reset to Retry Flag */
            return (UIE_ERR_KEY_SEQ);
        }

        if (UieCheckRetryBuf(iuchUieCurrentFunc[iuchUieFuncSts]) == UIE_SUC_KEY_SEQ) {
            UieWriteRetryBuf(pData);
            UieSendCancel(iuchCurrent);
            return (UIE_SUC_KEY_SEQ);
        }
    }
}


/*
*===========================================================================
** Synopsis:    SHORT UieCheckRetryBuf(UCHAR iuchCurrent)
*     Input:    iuchCurrent - Current Function Index
*
** Return:      UIE_SUC_KEY_SEQ - Successful
*               UIE_ERR_KEY_SEQ - Error
*
** Description:
*===========================================================================
*/
SHORT UieCheckRetryBuf(UCHAR iuchCurrent)
{
    UCHAR iuchPos;

    for (iuchPos = UieRetry.iuchRead; iuchPos < UieRetry.iuchWrite; iuchPos++) {
        if (UieRetryBuf[iuchPos] != UIE_END_CHAR) {
            continue;                                       /* Next Character */
        } else {
            iuchPos++;                                      /* Skip to End Character */
        }

        if (strchr(UieFuncTbl[iuchCurrent].pSequence, (SHORT)UieRetryBuf[iuchPos]) == NULL) {
            return (UIE_ERR_KEY_SEQ);
        }
        iuchPos++;                                          /* Next Character */
    }
    return (UIE_SUC_KEY_SEQ);
}


/*
*===========================================================================
** Synopsis:    VOID UieSendCancel(UCHAR iuchCurrent)
*     Input:    iuchCurrent -
*
** Return:
*
** Description:
*===========================================================================
*/
VOID UieSendCancel(UCHAR iuchCurrent)
{
	UIESUBMSG SubMsg = {0};

    UieSetStatusDisplay(UIE_DISABLE);       /* Disable Display */

    if (UieFuncTbl[iuchCurrent].pFunction) {
        SubMsg.uchMsg = UIM_CANCEL;         /* Cancel Message */
        SubMsg.usFunc = 0;

        (*UieFuncTbl[iuchCurrent].pFunction->sFunc)((KEYMSG *)&SubMsg);
    }
}


/*
*===========================================================================
** Synopsis:    SHORT UieSendRedisplay(USHORT usProgress)
*     Input:    usProgress -
*
** Return:
*
** Description:
*===========================================================================
*/
SHORT UieSendRedisplay(USHORT usProgress)
{
    SHORT     sRet;
	UIESUBMSG SubMsg = {0};

    UieSetStatusDisplay(UIE_ENABLE);            /* Enable Display */
    iuchUieCurrentFunc[iuchUieFuncSts] = iuchUieActiveFunc[iuchUieFuncSts];
    if (UieFuncTbl[iuchUieCurrentFunc[iuchUieFuncSts]].pFunction) {
        SubMsg.uchMsg = UIM_REDISPLAY;          /* Redisplay Message */
        SubMsg.usFunc = usProgress;
        sRet = (*UieFuncTbl[iuchUieCurrentFunc[iuchUieFuncSts]].pFunction->sFunc)((KEYMSG *)&SubMsg);
    } else {
        sRet = 0;
    }

    UieRetry.fchStatus |= UIE_CLEAR_RETRY_BUF;  /* Set to Clear Retry Buffer Flag */

    return (sRet);
}


static SHORT   sUifQueueStringWaitEvent = -1;        // used with processing FSC_CNTRL_STRING_EVENT

// FSC_CNTRL_STRING_EVENT
/*
*===========================================================================
** Synopsis:    SHORT   UifRegStringWaitEvent(USHORT *pusSize)
*     Input:    
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*
** Description: Wait for an event that is specified in the control string
*               argument.  The argument indicates which of the various kinds
*               of events to look for.
*
*               Event                      Description
*                  1       Forwarding operation completed event
*===========================================================================
*/
SHORT   UifRegStringWaitEvent(USHORT usEventNumber)
{
	VOID           *pParameters;
	REGDISPMSG      DispMsg = {0};

    /*----- Get String Parameter -----*/
	if (usEventNumber < CNTRL_STRING_EVENT_VALID_EVENT)
		return 0;

	if (sUifQueueStringWaitEvent < 0)
		return -2;

    DispMsg.uchMajorClass = CLASS_REGDISPCOMPLSRY;
    DispMsg.uchMinorClass = CLASS_REGDISP_PAUSE;
	RflGetLead (DispMsg.aszMnemonic, LDT_PAUSEMSG1_ADR);  // UifRegStringWaitEvent() lead thru message

    DispMsg.uchFsc = usEventNumber;
    DispMsg.fbSaveControl = 0;
    DispWrite(&DispMsg);

	fchUieActive |= UIE_EVENT_WAIT;

	// Read the queue Semaphore for read MSR data from device such as Zebra RW 220 Printer
	pParameters = PifReadQueue(sUifQueueStringWaitEvent);
	fchUieActive &= ~UIE_EVENT_WAIT;
	if (pParameters) {
		USHORT  usEvent = *(USHORT *)pParameters;

		if (usEvent == 0)
			return -3;
		else if (usEvent == usEventNumber)
			return -1;
		else
			return -2;
	}
	else {
		return 0;
	}
}

VOID UifSignalStringWaitEventInternal (USHORT usEventNumber)
{
	static  USHORT  usParms[10];
	static  USHORT  usParmsIndex = 0;

	usParmsIndex = ((usParmsIndex + 1) % 10);
	usParms[usParmsIndex] = usEventNumber;

	if (sUifQueueStringWaitEvent >= 0) {
		PifWriteQueue(sUifQueueStringWaitEvent, usParms + usParmsIndex);
	}
}

VOID  UifSignalStringWaitEvent (USHORT usEventNumber)
{
	UifSignalStringWaitEventInternal (usEventNumber);
}

VOID  UifSignalStringWaitEventIfWaiting (USHORT usEventNumber)
{

	if ((fchUieActive & UIE_EVENT_WAIT) != 0) {
		UifSignalStringWaitEventInternal (usEventNumber);
	}
}


SHORT  UifCreateQueueStringWaitEvent(VOID)
{
	extern  SHORT  sUifQueueStringWaitEvent;

	sUifQueueStringWaitEvent = PifCreateQueue(10);

	return sUifQueueStringWaitEvent;
}


/*
*===========================================================================
** Synopsis:    SHORT UieDefProc(KEYMSG *pData)
*     Input:    pData - Message Data
*
** Return:      UIE_SUC_KEY_SEQ - Successful
*               LDT_SEQERR_ADR  - Key Sequence Error
*
** Description:
*===========================================================================
*/
SHORT  UieDefProc(KEYMSG *pData)
{
    TCHAR auchReceiptFeed[] = {ESC, _T('|'), _T('8'), _T('l'), _T('F')/*UIE_RECEIPT_FEED_ROW*/}; /* Receipt Feed Command */
    TCHAR auchJournalFeed[] = {ESC, _T('|'), _T('8'), _T('l'), _T('F')/*UIE_JOURNAL_FEED_ROW*/}; /* Journal Feed Command */

    switch (pData->uchMsg) {
    case UIM_DRAWER:                                    /* Drawer Close Message */
		UieKeepMessage();
        UieRetry.fchStatus |= UIE_SKIP_RETRY_WRT;
        return (UIE_SUC_KEY_SEQ);                       /* Successful */

    case UIM_REDISPLAY:
        UieRedisplay();
        return (UIE_SUC_KEY_SEQ);

    case UIM_INPUT:                                     /* Input Message */
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_RECEIPT_FEED:                          /* Receipt Feed Key */
            PmgPrint(PMG_PRT_RECEIPT, auchReceiptFeed, (USHORT)_tcslen(auchReceiptFeed));
			UieKeepMessage();
            UieRetry.fchStatus |= UIE_SKIP_RETRY_WRT;
            return (UIE_SUC_KEY_SEQ);

        case FSC_JOURNAL_FEED:                          /* Journal Feed Key */
            PmgPrint(PMG_PRT_JOURNAL, auchJournalFeed, (USHORT)_tcslen(auchJournalFeed));
			UieKeepMessage();
            UieRetry.fchStatus |= UIE_SKIP_RETRY_WRT;
            return (UIE_SUC_KEY_SEQ);

					//SR 143 cwunn @/For cancel mechanism
		case FSC_CANCEL:
			UieSendCancel(iuchUieCurrentFunc[iuchUieFuncSts]);  //execute cancel for the current function
			UieSendRedisplay(1);	//Reset leadthrough display
			UieSetMacro(0, NULL, 0);               /* disable macro function */
			return(UIE_SUC_KEY_SEQ);

		case FSC_AUTO_SIGN_OUT:
			UieSetMacro(0, NULL, 0);               /* disable macro function */
			return(UIE_SUC_KEY_SEQ);

        case FSC_CNTRL_STRING_EVENT:           /* Journal Feed Key */
			{
				USHORT  usEventNumber = pData->SEL.INPUT.uchMinor;
				UifRegStringWaitEvent(usEventNumber);
			}
			// Clear the leadthru area as we are done.
			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGOTHER, CLASS_UICLEAR, DISP_SAVE_TYPE_0);

			UieKeepMessage();
            UieRetry.fchStatus |= UIE_SKIP_RETRY_WRT;
            return (UIE_SUC_KEY_SEQ);

        default:                                        /* other FSC */
            break;
        }

    default:                                            /* other Message */
        return (LDT_SEQERR_ADR);                        /* Key Sequence Error */
    }
}

/* ---------- End of UIESEQ.C ---------- */
