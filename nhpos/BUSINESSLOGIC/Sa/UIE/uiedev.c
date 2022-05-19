/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Input Device Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieDev.C
* --------------------------------------------------------------------------
* Abstract:
*
*   The data from the device of each input is displayed on the operator
*   display according to the specification of echo back.
*   And, when the function key is input, the data will be set in a user's
*   structur buffer and returns.
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Sep-16-92  00.01.02  O.Nakada     Removed decrement retry buffer counter
*                                   for FSC_BS key in UieSelectData().
* Sep-25-92  00.01.04  O.Nakada     Modify error code from LDT_ERR_ADR to
*                                   LDT_SEQERR_ADR in UieSelectData().
* Sep-25-92  00.01.04  O.Nakada     Removed case constant-expression of
*                                   UIE_RING_ERR in UieSetData().
* Sep-25-92  00.01.05  O.Nakada     Modify case constant-expression from
*                                   UIE_RING_SUCCESS to UIE_RING_READ in
*                                   UieReadData().
* Sep-30-92  00.01.05  O.Nakada     Removed abort skip funcion in
*                                   UieCheckCancel2Mode().
* Nov-10-92  00.01.11  O.Nakada     Modify UieGetRetryBuf().
*           (00.00.02) for T-POS Version.
* Dec-08-92  01.00.03  O.Nakada     Modify drawer close message.
* Jan-07-92  01.00.07  O.Nakada     Modify display enable flag in
*                                   UieGetRetryBuf() and UieClearMsg().
* May-25-93  C1        O.Nakada     Removed input device control functions.
* May-25-93  C1        O.Nakada     Add PifLog() after retry buffer full.
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Mar-15-95  G0        O.Nakada     Modified all modules, correspond to
*                                   hospitality release 3.0 functions.
* Feb-14-96  G4        O.Nakada     Added PERIPHERAL_DEVICE_OTHER.
*
* 2171 for Win32
* Aug-26-99  01.00.00  K.Iwata      V1.0 
* Oct-13-99            M.Teraki     Changed 'min()' -> '__min()'
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
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <stdlib.h>
#include "ecr.h"
#include "regstrct.h"
#include "pif.h"
#include "fsc.h"
#include "paraequ.h"
#include "uie.h"
#include "uireg.h"
#include "../../uifreg/uiregloc.h"
#include "uiel.h"
#include "uielio.h"
#include "uieio.h"
#include "uiering.h"
#include "uiedisp.h"
#include "uieecho.h"
#include "uiedev.h"
#include "uietrack.h"
#include <appllog.h>
#include "item.h"

/*                                                                                
*===========================================================================
*   General Declarations
*===========================================================================
*/
PifSemHandle      usUieSyncNormal = PIF_SEM_INVALID_HANDLE;   /* synchronous semaphore */
USHORT            fchUieDevice;                               /* device status         */
UIEMACRO          UieMacro;                                   /* macro information     */

extern	TCHAR		aszUieKeyboardBuff[256];			      // TOUCHSCREEN Hold keyboard input for GetMessage

        USHORT      fsUieKeyDevice = UIE_ENA_KEYBOARD|UIE_ENA_KEYLOCK;
        
STATIC  KEYMSG      UieMsg;                 /* keep message buffer   */
STATIC	KEYMSG		UieMsgCopy[4];			// copy of the kept message buffer


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
**  Synopsis:   VOID DLLENTRY UieCtrlDevice(USHORT fsDevice);
*      Input:   fsDevice - Input Device
*
**  Return:     nothing
*===========================================================================
*/
USHORT DLLENTRY UieCtrlDevice(USHORT fsDevice)
{
    USHORT  fsPrevious = 0;
   
    if (fchUieDevice & UIE_ENA_DRAWER) {
        fsPrevious |= UIE_ENA_DRAWER;
    } else {
        fsPrevious |= UIE_DIS_DRAWER;
    }
    if (fsDevice & UIE_DIS_DRAWER) {
        fchUieDevice &= ~UIE_ENA_DRAWER;
    } else if (fsDevice & UIE_ENA_DRAWER) {
        fchUieDevice |= UIE_ENA_DRAWER;
    }

#if defined(PERIPHERAL_DEVICE_WAITER_LOCK)
#pragma message("Support for waiter lock.")
    if (fchUieDevice & UIE_ENA_WAITER) {
        fsPrevious |= UIE_ENA_WAITER;
    } else {
        fsPrevious |= UIE_DIS_WAITER;
    }
    if (fsDevice & UIE_DIS_WAITER) {
        fchUieDevice &= ~UIE_ENA_WAITER;
    } else if (fsDevice & UIE_ENA_WAITER) {
        fchUieDevice |= UIE_ENA_WAITER;
    }
#endif

#if defined(PERIPHERAL_DEVICE_SCANNER)
#pragma message("Support for scanner.")
    if (fchUieDevice & UIE_ENA_SCANNER) {
        fsPrevious |= UIE_ENA_SCANNER;
    } else {
        fsPrevious |= UIE_DIS_SCANNER;
    }
    if (fsDevice & UIE_DIS_SCANNER) {
        fchUieDevice &= ~UIE_ENA_SCANNER;
        UieScannerDisable();
    } else if (fsDevice & UIE_ENA_SCANNER) {
        fchUieDevice |= UIE_ENA_SCANNER;
        UieScannerEnable();
    }
#endif

#if defined(PERIPHERAL_DEVICE_MSR)
#pragma message("Support for MSR.")
    if (fchUieDevice & UIE_ENA_MSR) {
        fsPrevious |= UIE_ENA_MSR;
    } else {
        fsPrevious |= UIE_DIS_MSR;
    }
    if (fsDevice & UIE_DIS_MSR) {
        fchUieDevice &= ~UIE_ENA_MSR;
    } else if (fsDevice & UIE_ENA_MSR) {
        fchUieDevice |= UIE_ENA_MSR;
    }
#endif

#if defined(PERIPHERAL_DEVICE_VIRTUALKEY)
#pragma message("Support for Virtual Keyboard Device such as FreedomPay.")
    if (fchUieDevice & UIE_ENA_VIRTUALKEY) {
        fsPrevious |= UIE_ENA_VIRTUALKEY;
    } else {
        fsPrevious |= UIE_DIS_VIRTUALKEY;
    }
    if (fsDevice & UIE_DIS_VIRTUALKEY) {
        fchUieDevice &= ~UIE_ENA_VIRTUALKEY;
    } else if (fsDevice & UIE_ENA_VIRTUALKEY) {
        fchUieDevice |= UIE_ENA_VIRTUALKEY;
    }
#endif

    /* 2172 RS */
    if (fsUieKeyDevice & UIE_ENA_KEYBOARD) {
        fsPrevious |= UIE_ENA_KEYBOARD;
    } else {
        fsPrevious |= UIE_DIS_KEYBOARD;
    }
    if (fsDevice & UIE_DIS_KEYBOARD) {
        fsUieKeyDevice &= ~UIE_ENA_KEYBOARD;
    } else if (fsDevice & UIE_ENA_KEYBOARD) {
        fsUieKeyDevice |= UIE_ENA_KEYBOARD;
    }

    if (fsUieKeyDevice & UIE_ENA_KEYLOCK) {
        fsPrevious |= UIE_ENA_KEYLOCK;
    } else {
        fsPrevious |= UIE_DIS_KEYLOCK;
    }
    if (fsDevice & UIE_DIS_KEYLOCK) {
        fsUieKeyDevice &= ~UIE_ENA_KEYLOCK;
    } else if (fsDevice & UIE_ENA_KEYLOCK) {
        fsUieKeyDevice |= UIE_ENA_KEYLOCK;
    }

    return (fsPrevious);
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieSetMacroTbl(CONST UCHAR FAR *puchMnemo,
*                                            UCHAR uchRecordSize);
*     Input:    puchMnemo    - address of lead through
*               usRecordSize - record size of lead through
*
** Return:      nothing
*===========================================================================
*/
VOID DLLENTRY UieSetMacroTbl(CONST UCHAR *puchMnemo, UCHAR uchRecordSize)
{
    UieMacro.puchMnemo     = puchMnemo;
    UieMacro.uchRecordSize = uchRecordSize;
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieSetMacro(CONST FSCTBL FAR *pFsc,
*                                         USHORT usNumber);
*     Input:    pFsc     - address of FSC table
*               usNumber - number of FSC
*
** Return:      nothing
*===========================================================================
*/
VOID DLLENTRY UieSetMacro(USHORT usControlStringNumber, CONST FSCTBL *pFsc, USHORT usNumber)
{
    /* --- cancel macro function ? --- */

    if (pFsc == NULL && usNumber == 0 &&        /* cancel macro    */
        UieMacro.usNumber != UieMacro.usRead) { /* executing macro */
        UieMacro.fsControl |= UIE_MACRO_FINAL;  /* set final flag  */
    }

    UieMacro.pFsc     = pFsc;
    UieMacro.usNumber = usNumber;
    UieMacro.usRead   = 0;
    UieMacro.usControlStringNumber   = usControlStringNumber;
}

SHORT DLLENTRY UieMacroRunning (VOID)
{
	return (UieMacro.fsControl & UIE_MACRO_RUNNING) ? 1 : 0;
}

SHORT UieAddToEndMacro(VOID)
{
	USHORT  usSize = UieMacro.usNumber * sizeof(FSCTBL);
	USHORT  usSizeSave = UieMacro.usNumber * sizeof(FSCTBL);
	SHORT   sStatus;

	/*----- Call Control String -----*/
	if ((sStatus = UifRegCallString(&usSize)) < 0) {
		PifLog (MODULE_UIC_LOG, LOG_EVENT_STRING_CALL_ERROR);
		PifLog (MODULE_ERROR_NO(MODULE_UIC_LOG), (USHORT)abs(sStatus));
		return(LDT_KEYOVER_ADR);
	}

	/*----- Pause Control String -----*/
	sStatus = UifRegPauseString(&usSize, 0x0001);   // keep any pause strings that have already been processed.
	if (sStatus == -1) {
		ItemOtherClear();
		UifRegWorkClear();
		return(SUCCESS);
	} else if (sStatus != 0) {
		PifLog (MODULE_UIC_LOG, LOG_EVENT_STRING_PAUSE_ERROR);
		PifLog (MODULE_ERROR_NO(MODULE_UIC_LOG), (USHORT)abs(sStatus));
		return(LDT_KEYOVER_ADR);
	}

    UieMacro.usNumber = usSize/sizeof(FSCTBL);
	UieMacro.usRead--;
	return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieGetMessage(KEYMSG DLLARG *pData);
*    Output:    pData - Message Save Pointer
*
** Return:      nothing
*
** Description: This function reads the message from the input device.

  UieReadKeyboardData will get the Character data from the keyboard input buffer (keyboardbuff)
*===========================================================================
*/
#if defined(UieGetMessage)
VOID DLLENTRY UieGetMessage_special(KEYMSG DLLARG *pMsg, USHORT usUieGetErrorActionFlags);

VOID DLLENTRY UieGetMessage_Debug(KEYMSG DLLARG *pMsg, USHORT usUieGetErrorActionFlags, char *aszFilePath, int nLineNo)
{
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	sprintf (xBuffer, "UieGetMessage_Debug(): File %s, lineno = %d", aszFilePath + iLen, nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);

	UieGetMessage_special(pMsg, usUieGetErrorActionFlags);
}

VOID DLLENTRY UieGetMessage_special(KEYMSG DLLARG *pMsg, USHORT usUieGetErrorActionFlags)
#else
VOID DLLENTRY UieGetMessage(KEYMSG DLLARG *pMsg, USHORT  usUieGetErrorActionFlags)
#endif
{
    SHORT   sResult;
#if defined(PERIPHERAL_DEVICE_WAITER_LOCK)
    FSCTBL  Ring;
#endif

    /* --- initialization ? --- */
    if (UieEcho.fchStatus & UIE_KEEP_MSG) { /* keep previous message */
        UieEcho.fchStatus &= ~UIE_KEEP_MSG; /* reset keep flag       */
        *pMsg              = UieMsg;        /* resotre message       */
    } else {                                /* not keep              */
        UiePreEchoBack(pMsg);
        memset(&UieMsg, 0, sizeof(KEYMSG));
        UieMacro.fsControl &= ~UIE_MACRO_WAIT;
    }

    for (;;) {
        /* --- check drawer close trigger --- */
        if (UieReadDrawer(pMsg) == UIE_SUCCESS) {   /* closed        */
            return;                                 /* exit ...      */
        }

        /* --- check whether the UI thread is the active module --- */
        if (! (fchUieActive & UIE_ACTIVE_GET)) {
			// other thread, error dialog, UieErrorMsgWithText(), so we need
			// to wait until it is done.
            PifRequestSem(usUieSyncNormal);         /* block ...     */
            continue;                               /* next ...      */
        }

        /* --- read data form macro buffer if not paused --- */
		if ((usUieGetErrorActionFlags & UIEGETMESSAGE_FLAG_MACROPAUSE) == 0) {
			sResult = UieReadMacro(pMsg);
			if (sResult == UIE_SUCCESS) {               /* message exist */
				break;                                  /* exit ...      */
			} else if (sResult == UIE_ERROR_NEXT) {
				continue;                               /* next ...      */
			}
		}

#if defined(PERIPHERAL_DEVICE_WAITER_LOCK)

        /* --- read ID of waiter lock --- */
        if (UieReadWaiterLock(&Ring) == UIE_SUCCESS) {  /* changed   */
            UieMakeGeneralMsg(Ring, pMsg);
            break;                                  /* exit ...      */
        }
#endif

        /* --- read data form ring buffer --- */
        sResult = UieReadData(pMsg);
        if (sResult == UIE_SUCCESS) {               /* message exist */
            break;                                  /* exit ...      */
		} else if (sResult == UIE_ERROR_CNTRL_STING_GRD) {
            UieMacro.fsControl &= ~UIE_MACRO_WAIT;
			if ((usUieGetErrorActionFlags & UIEGETMESSAGE_FLAG_EMPTYRINGBUF) != 0) {
				pMsg->uchMsg = FSC_STRING;
				UieEmptyRingBuf();
				break;
			} else {
				continue;
			}
        } else if (sResult == UIE_ERROR_NEXT) {
            UieMacro.fsControl &= ~UIE_MACRO_WAIT;
            continue;                               /* next ...      */
        }

		//RPH allow keyboard input on touchscreen
		//read data from keyboard input
		sResult = UieReadKeyboardData(pMsg);

        if (sResult == UIE_SUCCESS) {               /* message exist */
            break;                                  /* exit ...      */
        } else if (sResult == UIE_ERROR_NEXT) {
            UieMacro.fsControl &= ~UIE_MACRO_WAIT;
            continue;                               /* next ...      */
        }

        /* --- read data form tracking buffer --- */
        sResult = UieReadTester(pMsg);
        if (sResult == UIE_SUCCESS) {               /* message exist */
            break;                                  /* exit ...      */
        } else if (sResult == UIE_ERROR_NEXT) {
            UieMacro.fsControl &= ~UIE_MACRO_WAIT;
            continue;                               /* next ...      */
        }

        PifRequestSem(usUieSyncNormal);             /* block ...     */
    }

    UieMsg = *pMsg;                                /* backup message */
}


/*
*===========================================================================
** Synopsis:    SHORT UieReadMacro(KEYMSG *pMsg);
*     InOut:    pMsg - address of message buffer
*
** Return:      UIE_SUCCESS    - message exist
*               UIE_ERROR      - buffer empty
*               UIE_ERROR_NEXT - wait for next data
*===========================================================================
*/
SHORT UieReadMacro(KEYMSG *pMsg)
{
    FSCTBL  Fsc;
    USHORT  sResult = UIE_SUCCESS;

	if (UieMacro.pFsc == 0 || UieMacro.usRead > 4000 || UieMacro.usNumber < 1) {
		// if there has been a clear of the control string environment using UieSetMacro()
		// or if the value of UieMacro.usRead is too much probably due to a subtraction on an
		// unsigned value of zero that caused an underflow resulting in 0xFFFF then just error out.
		UieMacro.fsControl |= UIE_MACRO_FINAL;   // finalize this macro due to an error in the environment.
	}

    /* --- check final macro --- */
    if (UieMacro.fsControl & UIE_MACRO_FINAL) {     /* finalize            */

		UieMacro.fsControl &= ~UIE_MACRO_RUNNING;   // turn off the control string running flag

        /* --- check mode position in macro function --- */
        if (UieMode.uchEngine == UieMode.uchAppl ||     /* mode returned   */
            ! (UieMacro.fsControl & UIE_MACRO_MODE)) {  /* mode unchanged  */
            UieMacro.fsControl &= ~(UIE_MACRO_FINAL | UIE_MACRO_MODE); /* reset both mode changed and final flags   */
            return (UIE_ERROR);                     /* exit ...            */
        }

        if (! (UieMode.fchStatus & UIE_MODE_CHG)) { /* mode unchangable    */
            return (UIE_ERROR);                     /* exit ...            */
        }

        if (UieMode.uchEngine != UieMode.uchMacro) {/* mismatch mode       */
            UieMode.uchEngine = UIE_INIT_MODE;      /* initialization      */
        }

        UieMacro.fsControl &= ~(UIE_MACRO_FINAL | UIE_MACRO_MODE);     /* reset both mode changed and final flags   */
        UieMode.uchMacro    = UIE_INIT_MODE;        /* initialization      */
        return (UIE_ERROR);                         /* exit ...            */
    }

    /* --- check macro function --- */
    if (UieMacro.usNumber <= UieMacro.usRead ||     /* disable macro       */
        UieMacro.fsControl & UIE_MACRO_WAIT) {      /* wait a 1 key        */
        return (UIE_ERROR);
    }

    Fsc = UieMacro.pFsc[UieMacro.usRead++];

    /* --- check end of macro function --- */
    if (UieMacro.usNumber <= UieMacro.usRead) {     /* end of macro        */
        UieMacro.fsControl |= UIE_MACRO_FINAL;      /* set final flag      */
    }

	if (Fsc.uchMajor == FSC_CALL_STRING) {
		/*----- Key Sequence -----*/
		UieAddToEndMacro();
		if (UieMacro.usNumber > UieMacro.usRead) {
			UieMacro.fsControl &= ~(UIE_MACRO_FINAL);   // clear final flag in case it was set
			Fsc = UieMacro.pFsc[UieMacro.usRead++];
		} else {
			return (UIE_ERROR);
		}
	}

    switch (Fsc.uchMajor) {
    case FSC_MODE:                                  /* mode change         */
        if (Fsc.uchMinor != UieMode.uchAppl) {      /* other mode          */
            if (! (UieMode.fchStatus & UIE_MODE_CHG)) {/* mode unchangable */
                return (UieMakeErrorMsg(LDT_PROHBT_ADR, pMsg));
            }

            UieMacro.fsControl |= UIE_MACRO_MODE;   /* set mode changed    */
        }

        UieMode.uchMacro = Fsc.uchMinor;            /* set macro mode      */
        sResult = UieModeData(Fsc.uchMinor, pMsg);
        break;

    case FSC_AN:
        Fsc.uchMajor = Fsc.uchMinor;                /* set AN key          */
        sResult = UieKeyData(UIE_AN_KEY, Fsc, pMsg);
        break;

    case FSC_PAUSE:
        UieMacro.fsControl |= UIE_MACRO_WAIT;

        if (Fsc.uchMinor == 0) {                /* disable lead through    */
            return (UIE_ERROR);
        }

        /* --- display lead through --- */
        UieEchoBackMacro((USHORT)(Fsc.uchMinor - 1), UieMacro.puchMnemo, (USHORT)UieMacro.uchRecordSize);

        return (UIE_ERROR);

    default:                                            /* function key    */
		UieMacro.fsControl |= UIE_MACRO_RUNNING;       // turn on the control string running flag
        sResult = UieKeyData(UIE_FUNC_KEY, Fsc, pMsg);
        break;
    }

    return (sResult);
}


/*
*===========================================================================
** Synopsis:    SHORT UieReadData(KEYMSG *pMsg);
*     InOut:    pMsg - address of message buffer
*
** Return:      UIE_SUCCESS    - message exist
*               UIE_ERROR      - buffer empty
*               UIE_ERROR_NEXT - wait for next data
*===========================================================================
*/
VOID UieEmptyRingBufClearMacro (VOID)
{
    UieSetMacro(0, NULL, 0);               /* disable macro function */
	UieEmptyRingBuf();
}

VOID UieEmptyRingBuf(VOID)
{
    SHORT   sResult;
    FSCTBL  Ring;
    USHORT  usLength;
    UCHAR   auchBuf[UIE_RING_LEN_BUF];

	do {
		/* --- read data form ring buffer and ignore it --- */
		sResult = UieReadRingBuf(&Ring, auchBuf, &usLength);
	} while (sResult == UIE_SUCCESS);
}

SHORT UieReadData(KEYMSG *pMsg)
{
    SHORT   sResult;
    FSCTBL  Ring, Fsc;
    USHORT  usLength, usType;
    UCHAR   uchMode, auchBuf[UIE_RING_LEN_BUF];

    /* --- read data form ring buffer --- */
    sResult = UieReadRingBuf(&Ring, auchBuf, &usLength);

    if (sResult != UIE_SUCCESS) {               /* data not exist    */
        return (UIE_ERROR);                     /* block ...         */
    }

    switch (Ring.uchMajor) {
    case FSC_MODE:                              /* mode change       */
        UieTracking(UIE_TKEY_MODE, Ring.uchMinor, 0);

        return (UieModeData(Ring.uchMinor, pMsg));

    case FSC_KEYBOARD:                          /* keyboard data     */
        /* --- convert form char data to FSC --- */
        uchMode = UieMode.uchAppl;
        usType  = UieConvertFsc(UieKbd.pCvt[uchMode][UieKbd.uchCType],
                                UieKbd.pFsc[uchMode][UieKbd.uchFType],
                                *(CHARDATA *)auchBuf,
                                &Fsc);

        /* --- update tracking data --- */
        if (usType == UIE_AN_KEY) {         /* alpha/numeric key */
            UieTracking(UIE_TKEY_AN, Fsc.uchMajor, 0);
        } else if (usType == UIE_FUNC_KEY) {/* function key      */
			//QTY GUARD RPH 2-2-4
			//If the quantity guard is used check and see if
			//an amount was pressed before it
			//if there is an amount change the FSC_QTY_GUARD
			//to FSC_QTY if not return to get next item off buffer
			if(Fsc.uchMajor == FSC_QTY_GUARD){
				if(pMsg->SEL.INPUT.lData){
					Fsc.uchMajor = FSC_QTY;
				}else{
					return (UIE_ERROR_NEXT);  // UIE_ERROR_QTY_GUARD
				}
			//PRICE_GUARD 2-18-4
			//The Price Guard is used to check if an amount
			//has been entered before it was pressed
			//if so the FSC_PRICE_GUARD is changed to 
			//FSC_PRICE_ENTER if not return to get next item off buffer
			}else if(Fsc.uchMajor == FSC_PRICE_GUARD){
				if(pMsg->SEL.INPUT.lData){
					Fsc.uchMajor = FSC_PRICE_ENTER;
				}else{
					return (UIE_ERROR_NEXT);    // UIE_ERROR_PRICE_GUARD
				}
			}else if(Fsc.uchMajor == FSC_CNTRL_STING_GRD){
				if(pMsg->SEL.INPUT.lData){
					return (UIE_SUCCESS);
				}else{
					return (UIE_ERROR_CNTRL_STING_GRD);
				}
			}

            if (UieTracking(UIE_TKEY_FUNC, Fsc.uchMajor, Fsc.uchMinor) == UIE_SUCCESS) {
                return (UieMakeClearMsg(pMsg));     /* exit ...  */
            }
        } else {                            /* mode lock         */
            return (UIE_ERROR_NEXT);        /* next ...          */
        }

        return (UieKeyData(usType, Fsc, pMsg));

	case FSC_OPERATOR_ACTION:
		{
			UIE_OPERATORACTION  *pUieOperatorActionData = (UIE_OPERATORACTION  *)auchBuf;

			pMsg->SEL.INPUT.DEV.OPERATOR_ACTION = *pUieOperatorActionData;
		}
		if (pMsg->SEL.INPUT.DEV.OPERATOR_ACTION.usControlStringId != 0) {
			SHORT  sError;
			NHPOS_ASSERT_TEXT(0, "FSC_OPERATOR_ACTION: run control string.");
			pMsg->SEL.INPUT.uchMajor = FSC_STRING;
			pMsg->SEL.INPUT.lData = pMsg->SEL.INPUT.DEV.OPERATOR_ACTION.usControlStringId;

			sError = UifRegString(pMsg);
			NHPOS_ASSERT_TEXT((sError == 0), "UifRegString() error.");
		} else {
			NHPOS_ASSERT_TEXT(0, "FSC_OPERATOR_ACTION: no control string to run.");
		}
		return (UIE_SUCCESS);

    default:
        return (UieMiscData(&Ring, auchBuf, usLength, pMsg));
    }
}

/*
*===========================================================================
** Synopsis:    SHORT UieReadKeyboardData(KEYMSG *pMsg);
*     InOut:    pMsg - address of message buffer
*
** Return:      UIE_SUCCESS    - message exist
*               UIE_ERROR      - buffer empty
*               UIE_ERROR_NEXT - wait for next data

  Get the input character off the keyboard buffer
  move any remaining charaters

  Added for TOUCHSCREEN support
*===========================================================================
*/
SHORT UieReadKeyboardData(KEYMSG *pMsg)
{
    UCHAR   uchLength;
	TCHAR	keyBuff;
    struct _INPUT   *pKey = &pMsg->SEL.INPUT;

	//if nothing on buffer return
	if(aszUieKeyboardBuff[0] == 0x00)
	{
		return UIE_ERROR;
	}

	//set local keyBuff to input set to aszUieKeyboardBuff
	keyBuff = aszUieKeyboardBuff[0];

	//move data in buffer up so the next is ready to take
	_tcsncpy(&aszUieKeyboardBuff[0], &aszUieKeyboardBuff[1], _tcslen(aszUieKeyboardBuff));

    /* --- check length of total data --- */
    uchLength = pKey->uchLen + (UCHAR)((UieEcho.fchStatus & UIE_DOUBLE_WIDE) ? (2) : (1));

	if(keyBuff == VK_BACK){
		uchLength--;
	}
    if (uchLength > UieEcho.uchMax || uchLength >= UIE_MAX_KEY_BUF) {
        return (UieMakeErrorMsg(LDT_KEYOVER_ADR, pMsg));
    }

    /* --- calculation --- */

    if (_T('0') <= keyBuff && keyBuff <= _T('9')) {
        pKey->lData = pKey->lData * 10 + keyBuff - _T('0');
    }

    /* --- set character in message buffer --- */
    if (UieEcho.fchStatus & UIE_DOUBLE_WIDE) {  /* double wide mode */
        pKey->aszKey[pKey->uchLen++] = FSC_DOUBLE_WIDE;
    }

	//Backspace pressed remove last charater and decrement count of characters
	if(keyBuff == VK_BACK)
	{
		if(pKey->uchLen){
			pKey->aszKey[--pKey->uchLen] = _T('\0');
		}
	}
	else
	{
		//set character and add one to count place null terminator
		pKey->aszKey[pKey->uchLen++] = keyBuff;
		pKey->aszKey[pKey->uchLen  ] = _T('\0');
	}

	switch (uchUieEcho) {               /* echo back type      */
        case UIE_ECHO_ROW0_REG:
        case UIE_ECHO_ROW1_REG:
            /*if ( pKey->uchLen > 9 ||
                (pKey->uchLen == 1 && keyBuff == _T('0'))) {*/
                uchUieEcho = UIE_ECHO_CA;   /* set new type        */
                UieRefreshEchoBack();       /* refresh back ground */
            //}
            break;
        
        case UIE_ECHO_WEIGHT:
            if ((pKey->aszKey[0] != '0' && pKey->uchLen > 10) ||
                (pKey->aszKey[0] == '0' && pKey->uchLen > 11)) {
                return (UieMakeErrorMsg(LDT_KEYOVER_ADR, pMsg));
            }

        default:
            break;
    }

    UieDisplayEchoBack(pMsg);           /* echo back           */
    return (UIE_ERROR_NEXT);            /* next ...            */
}

/*
*===========================================================================
** Synopsis:    SHORT UieReadTester(KEYMSG *pMsg);
*     InOut:    pMsg - address of message buffer
*
** Return:      UIE_SUCCESS    - message exist
*               UIE_ERROR      - buffer empty
*               UIE_ERROR_NEXT - wait for next data
*===========================================================================
*/
SHORT UieReadTester(KEYMSG *pMsg)
{
    SHORT   sResult;
    FSCTBL  Ring;
    USHORT  usType, usLength;
    UCHAR   auchBuf[UIE_RING_LEN_BUF];

    usLength = 0; /* ### ADD (2171 for Win32 V1.0) */

    /* --- read data from tracking buffer --- */
    sResult = UieReadTrack(&usType, &Ring, auchBuf);
    if (sResult != UIE_SUCCESS) {           /* buffer empty  */
        return (UIE_ERROR);                 /* block ...     */
    }

    switch (Ring.uchMajor) {
    case FSC_MODE:                          /* mode change   */
        return (UieModeData(Ring.uchMinor, pMsg));

    case FSC_KEYBOARD:                      /* keyboard data */
        return (UieKeyData(usType, *(FSCTBL *)auchBuf, pMsg));

    default:
        return (UieMiscData(&Ring, auchBuf, usLength, pMsg));
    }
}


/*
*===========================================================================
** Synopsis:    SHORT UieModeData(UCHAR uchMode, KEYMSG *pMsg);
*     Input:    uchMode - position of new mode
*     InOut:    pMsg    - address of message buffer
*
** Return:      UIE_SUCCESS    - message exist
*               UIE_ERROR_NEXT - wait for next data
*===========================================================================
*/
SHORT UieModeData(UCHAR uchMode, KEYMSG *pMsg)
{
    pMsg->uchMsg      = UIM_MODE;           /* set message ID   */
    pMsg->SEL.uchMode = uchMode;            /* set new position */

    if (UieMode.fchStatus & UIE_MODE_CHG) { /* mode changable   */
        return (UIE_SUCCESS);               /* exit ...         */
    } else {                                /* mode unchangable */
        return (UIE_ERROR_NEXT);            /* next ...         */
    }
}


/*
*===========================================================================
** Synopsis:    SHORT UieMiscData(FSCTBL Ring, UCHAR *puchData,
*                                 USHORT usLength, KEYMSG *pMsg);
*     Input:    Ring     - 
*               puchData -
*               usLength -
*     InOut:    pMsg     - address of message buffer
*
** Return:      UIE_SUCCESS    - message exist
*               UIE_ERROR_NEXT - wait for next data
*===========================================================================
*/
SHORT UieMiscData(FSCTBL *pRing, UCHAR *puchData, USHORT usLength,
                  KEYMSG *pMsg)
{
    USHORT  usCopy;
	int i;

    /* --- check device type --- */
    switch (pRing->uchMajor) {
    case FSC_MODE:                          /* mode key         */
        return (UieModeData(pRing->uchMinor, pMsg));

#if defined(PERIPHERAL_DEVICE_SCANNER)
    case FSC_SCANNER:                       /* scanner          */
        pMsg->SEL.INPUT.DEV.SCA.uchStrLen = (UCHAR)usLength;
        for(i = 0; i < usLength; i++)
		{
			pMsg->SEL.INPUT.DEV.SCA.auchStr[i] = puchData[i];
		}
        break;
#endif

#if defined(PERIPHERAL_DEVICE_MSR)
    case FSC_MSR:                           /* MSR              */
        memcpy(&pMsg->SEL.INPUT.DEV.MSR, puchData, usLength);
        break;

    case FSC_PINPAD:                           /* MSR              */
        memcpy(&pMsg->SEL.INPUT.DEV.PINPAD, puchData, usLength);
        break;

    case FSC_SIGNATURECAPTURE:                           /* MSR              */
        memcpy(&pMsg->SEL.INPUT.DEV.SIGCAP, puchData, usLength);
        break;
#endif

#if defined(PERIPHERAL_DEVICE_VIRTUALKEY)
    case FSC_VIRTUALKEYEVENT:                //Virtual keyboard event
        memcpy(&pMsg->SEL.INPUT.DEV.VIRTUALKEY, puchData, usLength);
		if (pMsg->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType == UIE_VIRTUALKEY_EVENT_MSR) {
			// If this virtual keyboard event is an MSR event then we are going to create
			// an FSC_MSR event using the data from the virtual key event.  Doing this for
			// sign-in and other things that may involve an MSR swipe.
			UIE_MSR     MSR;

			memset (&MSR, 0, sizeof(MSR));

			MSR.uchPaymentType = SWIPE_PAYMENT;

			memcpy (MSR.auchTrack1, pMsg->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.auchTrack1, sizeof(MSR.auchTrack1));
			MSR.uchLength1 = pMsg->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength1;
			memcpy (MSR.auchTrack2, pMsg->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.auchTrack2, sizeof(MSR.auchTrack2));
			MSR.uchLength2 = pMsg->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength2;

			pMsg->SEL.INPUT.uchLen = 0;
			pMsg->SEL.INPUT.DEV.MSR = MSR;
			pRing->uchMajor = pMsg->SEL.INPUT.uchMajor = FSC_MSR;
		}
        break;
#endif

#ifdef  FSC_RESET_LOG                                /* TAR #128895, from Dollar Tree */
#pragma message("Support for FSC_RESET_LOG.")
        case FSC_RESET_LOG:
            return (UieMakeGeneralMsg(*pRing, pMsg));  /* exit ... */
#endif

#ifdef  FSC_POWER_DOWN                               /* TAR #128895, from Dollar Tree */
#pragma message("Support for FSC_POWER_DOWN.")
        case FSC_POWER_DOWN:
            return (UieMakeGeneralMsg(*pRing, pMsg));  /* exit ... */
#endif

    default:                                /* other device     */
#if defined(PERIPHERAL_DEVICE_OTHER)
#pragma message("Support for other device.")
        usCopy = __min(PERIPHERAL_DEVICE_OTHER, usLength);
        memcpy(&pMsg->SEL.INPUT.DEV.auchOther, puchData, usCopy);
        return (UieMakeGeneralMsg(*pRing, pMsg)); /* exit ...     */
#else
        break;
#endif
    }

    /* --- check status of mode changable --- */
    if (UieCheckMode() != UIE_SUCCESS) {    /* mode unchangable */
        PifBeep(UIE_ERR_TONE);
        return (UIE_ERROR_NEXT);            /* next ...         */
    }

    /* --- check status of compulsory drawer --- */
    if (UieDrawerCompulsory(pRing) == UIE_SUCCESS) {
        return (UieMakeErrorMsg(LDT_DRAWCLSE_ADR, pMsg));
    }

    return (UieMakeGeneralMsg(*pRing, pMsg)); /* exit ...         */

    puchData = puchData;                    /* dummy            */
    usLength = usLength;                    /* dummy            */
    usCopy   = usCopy;                      /* dummy            */
}


/*
*===========================================================================
** Synopsis:    SHORT UieKeyData(USHORT usType, FSCTBL Fsc, KEYMSG *pMsg);
*     Input:    usType - type of FSC
*               Fsc    - FSC data
*     InOut:    pMsg   - address of message buffer
*
** Return:      UIE_SUCCESS    - message exist
*               UIE_ERROR_NEXT - wait for next data
*===========================================================================
*/
SHORT UieKeyData(USHORT usType, FSCTBL Fsc, KEYMSG *pMsg)
{
    FSCTBL  Ring;

    if (usType == (USHORT)UIE_ERROR) {          /* mode lock     *//* ### MOD (2172 Rel1.0) */
        return (UIE_ERROR_NEXT);                /* next ...      */
    }

    /* --- check clear key --- */
    if (usType == UIE_FUNC_KEY && Fsc.uchMajor == FSC_CLEAR) {
        return (UieMakeClearMsg(pMsg));     /* exit ...         */
    }

    /* --- check status of mode changable --- */
    if (UieCheckMode() != UIE_SUCCESS) {    /* mode unchangable */
        PifBeep(UIE_ERR_TONE);
        return (UIE_ERROR_NEXT);            /* next ...         */
    }

    if (UieCheckKeyData(usType, Fsc, pMsg) == UIE_SUCCESS) {
        return (UIE_SUCCESS);               /* exit ...         */
    }

    if (UieEcho.usFigure &&               /* enable set message */
        (USHORT)pMsg->SEL.INPUT.uchLen >= UieEcho.usFigure) {
        Ring.uchMajor = FSC_AN;
        Ring.uchMinor = 0;
        return (UieMakeGeneralMsg(Ring, pMsg));
    }

    return (UIE_ERROR_NEXT);
}


/*
*===========================================================================
** Synopsis:    SHORT UieCheckKeyData(USHORT usType, FSCTBL Fsc,
*                                     KEYMSG *pMsg);
*     Input:    usType - type of key data
*               Fsc    - FSC data
*     InOut:    pMsg   - address of message buffer
*
** Return:      UIE_SUCCESS    - message exist
*               UIE_ERROR_NEXT - wait for next data
*===========================================================================
*/
SHORT UieCheckKeyData(USHORT usType, FSCTBL Fsc, KEYMSG *pMsg)
{
    struct _INPUT   *pKey = &pMsg->SEL.INPUT;

    if (usType == UIE_AN_KEY) {             /* alpha/numeric key   */
        if (UieWriteANKey(Fsc.uchMajor,     /* error occured       */
                          pMsg) == UIE_SUCCESS) {
            return (UIE_SUCCESS);           /* exit ...            */
        }

        switch (uchUieEcho) {               /* echo back type      */
        case UIE_ECHO_ROW0_REG:
        case UIE_ECHO_ROW1_REG:
            if ( pKey->uchLen > 9 ||
                (pKey->uchLen == 1 && Fsc.uchMajor == '0')) {
                uchUieEcho = UIE_ECHO_CA;   /* set new type        */
                UieRefreshEchoBack();       /* refresh back ground */
            }
            break;
        
        case UIE_ECHO_WEIGHT:
            if ((pKey->aszKey[0] != '0' && pKey->uchLen > 10) ||
                (pKey->aszKey[0] == '0' && pKey->uchLen > 11)) {
                return (UieMakeErrorMsg(LDT_KEYOVER_ADR, pMsg));
            }

        default:
            break;
        }
        
        UieDisplayEchoBack(pMsg);           /* echo back           */
        return (UIE_ERROR_NEXT);            /* next ...            */
    } else {                                /* function key        */
        switch (Fsc.uchMajor) {             /* major code          */
        case FSC_DECIMAL:                   /* decimal point key   */
            switch (uchUieEcho) {           /* echo back type      */
            case UIE_ECHO_NO:               /* no echo back        */
            case UIE_ECHO_ROW0_NUMBER:      /* number entry        */
            case UIE_ECHO_ROW1_NUMBER:
            case UIE_ECHO_WEIGHT:           /* weight              */
                return (UieMakeErrorMsg(LDT_SEQERR_ADR, pMsg));

            case UIE_ECHO_ROW0_REG:         /* currency            */
            case UIE_ECHO_ROW1_REG:
            case UIE_ECHO_CA:               /* code, acount        */
                if (pKey->uchLen > 9) {     /* over length         */
                    return (UieMakeErrorMsg(LDT_KEYOVER_ADR, pMsg));
                }

                UieRefreshEchoBack();       /* clear back ground   */
                pKey->uchDec = pKey->uchLen;
                uchUieEcho   = UIE_ECHO_WEIGHT; /* set new type    */
                /* break */                 /* not use             */

            default:
                if (UieWriteANKey('.',      /* error occured       */
                                  pMsg) == UIE_SUCCESS) {
                    return (UIE_SUCCESS);   /* exit ...            */
                }
                break;
            }
            UieDisplayEchoBack(pMsg);       /* echo back           */
            return (UIE_ERROR_NEXT);

        case FSC_00:                        /* double '0' key      */
            switch (uchUieEcho) {           /* echo back type      */
            case UIE_ECHO_ROW0_REG:
            case UIE_ECHO_ROW1_REG:
                if (pKey->uchLen == 0 ||    /* leading '0'         */
                    pKey->uchLen >= 8) {    /* over length         */
                    uchUieEcho = UIE_ECHO_CA; /* set new type      */
                }
                break;

            case UIE_ECHO_WEIGHT:
                if (pKey->uchLen >= 9) {    /* over length         */
                    return (UieMakeErrorMsg(LDT_KEYOVER_ADR, pMsg));
                }
                break;

            default:
                break;
            }
            if (UieWriteANKey('0', pMsg) == UIE_SUCCESS) {
                return (UIE_SUCCESS);
            }
            if (UieWriteANKey('0', pMsg) == UIE_SUCCESS) {
                return (UIE_SUCCESS);
            }
            UieDisplayEchoBack(pMsg);       /* echo back           */
            return (UIE_ERROR_NEXT);

        case FSC_ALPHANUM:
            if (UieWriteANKey (Fsc.uchMinor, pMsg) == UIE_SUCCESS) {
                return (UIE_SUCCESS);
            }
            UieDisplayEchoBack(pMsg);       /* echo back           */
            return (UIE_ERROR_NEXT);

        case FSC_BS:                        /* back space Key      */
            if (uchUieEcho != UIE_ECHO_ROW0_AN &&
                uchUieEcho != UIE_ECHO_ROW1_AN) {
                break;
            }
            if (pKey->uchLen == 0) {        /* not keyboard data   */
                PifBeep(UIE_ERR_TONE);      /* error tone          */
                return (UIE_ERROR_NEXT);
            }
            if (pKey->uchLen >= 2 &&        /* double wide         */
                pKey->aszKey[pKey->uchLen - 2] == FSC_DOUBLE_WIDE) {
                --pKey->uchLen;             /* position for left   */
            }
            pKey->aszKey[--pKey->uchLen] = '\0';
            UieRefreshEchoBack();           /* refresh back ground */
            UieDisplayEchoBack(pMsg);       /* echo back           */
            return (UIE_ERROR_NEXT);

        case FSC_P3:                        /* double wide key     */
            if (uchUieEcho != UIE_ECHO_ROW0_AN &&
                uchUieEcho != UIE_ECHO_ROW1_AN) {
                break;
            }

            UieEcho.fchStatus ^= UIE_DOUBLE_WIDE;
            UieDisplayEchoBackWide(&uchUieEcho);
            UieDisplayEchoBack(pMsg);       /* echo back           */
            return (UIE_ERROR_NEXT);        /* exit ...            */

        case FSC_RECEIPT_FEED:
        case FSC_JOURNAL_FEED:
#ifdef  FSC_COMPUT
#pragma message("Support for FSC_COMPUT.")
        case FSC_COMPUT:
#endif
            return (UieMakeGeneralMsg(Fsc, pMsg));  /* exit ...    */
            return (UIE_SUCCESS);

        case FSC_NO_FUNC:                   /* not defined         */
            return (UieMakeErrorMsg(LDT_SEQERR_ADR, pMsg));

        default:
            break;
        }

        /* --- check status of compulsory drawer --- */

        if (UieDrawerCompulsory(&Fsc) == UIE_SUCCESS) {
            return (UieMakeErrorMsg(LDT_DRAWCLSE_ADR, pMsg));
        }

        return (UieMakeGeneralMsg(Fsc, pMsg));  /* exit ...        */
    }
}


/*
*===========================================================================
** Synopsis:    SHORT UieWriteANKey(UCHAR uchChar, KEYMSG *pMsg);
*     Input:    uchChar - character data
*     InOut:    pMsg    - address of message buffer
*
** Return:      UIE_SUCCESS    - message exist
*               UIE_ERROR_NEXT - wait for next data
*===========================================================================
*/
SHORT UieWriteANKey(UCHAR uchChar, KEYMSG *pMsg)
{
    UCHAR           uchLength;
    struct _INPUT   *pKey = &pMsg->SEL.INPUT;

    /* --- check length of total data --- */

    uchLength = pKey->uchLen + (UCHAR)((UieEcho.fchStatus & UIE_DOUBLE_WIDE) ? (2) : (1));

    if (uchLength > UieEcho.uchMax || uchLength >= UIE_MAX_KEY_BUF) {
        return (UieMakeErrorMsg(LDT_KEYOVER_ADR, pMsg));
    }

    /* --- calculation --- */

    if ('0' <= uchChar && uchChar <= '9') {
        pKey->lData = pKey->lData * 10 + uchChar - '0';
    }

    /* --- set character in message buffer --- */

    if (UieEcho.fchStatus & UIE_DOUBLE_WIDE) {  /* double wide mode */
        pKey->aszKey[pKey->uchLen++] = FSC_DOUBLE_WIDE;
    }

    pKey->aszKey[pKey->uchLen++] = uchChar;
    pKey->aszKey[pKey->uchLen  ] = '\0';

    return (UIE_ERROR_NEXT);
}

    
/*
*===========================================================================
** Synopsis:    SHORT UieMakeGeneralMsg(FSCTBL Fsc, KEYMSG *pMsg);
*     Input:    Fsc  - FSC data
*     InOut:    pMsg - address of message buffer
*
** Return:      UIE_SUCCESS  - message exist
*===========================================================================
*/
SHORT UieMakeGeneralMsg(FSCTBL Fsc, KEYMSG *pMsg)
{
    pMsg->uchMsg             = UIM_INPUT;
    pMsg->SEL.INPUT.uchMajor = Fsc.uchMajor;
    pMsg->SEL.INPUT.uchMinor = Fsc.uchMinor;

    if (pMsg->SEL.INPUT.uchDec != UIE_DEC_INIT) {
        pMsg->SEL.INPUT.uchDec =
            pMsg->SEL.INPUT.uchLen - pMsg->SEL.INPUT.uchDec - (UCHAR)1;
    }

    return (UIE_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    VOID UieMakeClearMsg(KEYMSG *pMsg);
*     InOut:    pMsg - address of message buffer
*
** Return:      UIE_SUCCESS  - message exist
*===========================================================================
*/
SHORT UieMakeClearMsg(KEYMSG *pMsg)
{
    pMsg->uchMsg             = UIM_INPUT;
    pMsg->SEL.INPUT.uchMajor = FSC_CLEAR;
    pMsg->SEL.INPUT.uchMinor = (UCHAR)(!(UieEcho.fchStatus & UIE_INIT_ECHO_BACK));

    PifRequestSem(usUieSyncDisplay);
    fchUieActive |= UIE_DISPLAY;            /* disaply enable */
    PifReleaseSem(usUieSyncDisplay);

    return (UIE_SUCCESS);                   /* exit ...       */
}


/*
*===========================================================================
** Synopsis:    VOID UieMakeErrorMsg(USHORT usErrorCode, KEYMSG *pMsg);
*     Input:    usErrorCode - error code
*     InOut:    pMsg        - address of message buffer
*
** Return:      UIE_SUCCESS  - message exist
*===========================================================================
*/
SHORT UieMakeErrorMsg(USHORT usErrorCode, KEYMSG *pMsg)
{
    pMsg->uchMsg     = UIM_ERROR;           /* error message */
    pMsg->SEL.usFunc = usErrorCode;         /* error code    */

    return (UIE_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID	UieCopyKeyMsg();
*  
*
** Return:      nothing
** Description:	This function makes a copy of UieMsg for cases such as
*				Order Declaration such that when an Event triggers an interruption
*				in the Event process, the event can be saved and then processed
*				once Order Declaration has been compeleted.
*===========================================================================
*/
SHORT	UieCopyKeyMsg()
{
	// If the first position in the array is empty then copy the data else
	// if the 2nd position is empty then copy the data else return error.
	if(UieMsgCopy[0].uchMsg == 0)
	{
		UieMsgCopy[0] = UieMsg;
		return (SUCCESS);
	} else if(UieMsgCopy[1].uchMsg == 0)
	{
		UieMsgCopy[1] = UieMsg;
		return (SUCCESS);
	} else if(UieMsgCopy[2].uchMsg == 0)
	{
		UieMsgCopy[2] = UieMsg;
		return (SUCCESS);
	} else if(UieMsgCopy[3].uchMsg == 0)
	{
		UieMsgCopy[3] = UieMsg;
		return (SUCCESS);
	} else
	{
		return (UIE_ERROR);
	}
}

/*
*===========================================================================
** Synopsis:    SHORT	UieGetKeyMsg(KEYMSG *pMsg);
*		Input:	pMsg - ptr to copy UieMsgCopy to
*
** Return:      SUCCESS - if Data exists
*				UIE_ERROR - if no data existed
** Description:	This function copies UieMsgCopy for cases such as
*				Order Declaration such that when an Event triggers an interruption
*				in the Event process, the event can be saved and then processed
*				once Order Declaration has been compeleted.
*===========================================================================
*/

SHORT	UieGetKeyMsg(KEYMSG *pMsg, USHORT i)
{
	// If data exists in the array at position 'i' then copy and clear
	// the data from that position in the array. If no data exists then
	// return error.
	if(UieMsgCopy[i].uchMsg != 0)
	{
		*pMsg = UieMsgCopy[i];
		memset(&UieMsgCopy[i], 0x00, sizeof(KEYMSG));
		return (SUCCESS);
	}else
	{
		return (UIE_ERROR);
	}
}

/* ===================================== */
/* ========== End of UieDev.C ========== */
/* ===================================== */
