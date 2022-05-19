/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Ring Buffer Module
* Category    : User Interface Engine, 2170 System Application
* Program Name: UieRing.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
*                   UieReadClearKey() - read and check a clear key
*                   UieReadAbortKey() - read and check a abort key
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Sep-25-92  00.01.05  O.Nakada     Modify Ring Buffer and Auxiliary Ring
*                                   Buffer Status.
* Sep-25-92  00.01.05  O.Nakada     Modify Abort function.
* Sep-29-92  00.01.05  O.Nakada     Modify key track function.
* Sep-30-92  00.01.05  O.Nakada     Added abort skip function in
*                                   UieReadRingMsg().
* Oct-01-92  00.01.05  O.Nakada     Modify keyboard disable check from
*                                   disable flag to pCvt pointer in
*                                   UieReadRingBuf().
* Oct-01-92  00.01.05  O.Nakada     Added null pointer check in
*                                   UieConvertFsc().
* May-25-93  C1        O.Nakada     Removed input device control functions.
* Jun-05-93  C1        O.Nakada     Modify key tracking function.
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Mar-15-95  G0        O.Nakada     Modified all modules, correspond to
*                                   hospitality release 3.0 functions.
*
*** NCR 2171 ***
* Sep-29-99  01.00.00  M.Teraki     Changed function prototype declaration
*                                   of WriteRingBuf() <2nd param.>
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
#include <stdio.h>
#include "ecr.h"
#include "pif.h"
#include "paraequ.h"
#include "uie.h"
#include "fsc.h"
#include "uiel.h"
#include "uielio.h"
#include "uieio.h"
#include "uiering.h"
#include "uieerror.h"
#include "uietrack.h"


/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
STATIC  USHORT      usUieRingSem = -1;          /* ring buffer semaphore  */
STATIC  UIERINGCNT  UieRingCnt;                 /* clear and abort count  */
STATIC  UIERINGSTS  UieRingSts;                 /* status of ring buffer  */
STATIC  UIERINGINF  UieRingInf[UIE_RING_NUM_INF];   /* device information */
STATIC  UCHAR       UieRingBuf[UIE_RING_LEN_BUF];   /* device ring buffer */


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT DLLENTRY UieReadClearKey(VOID);
*
** Return:      UIE_ENABLE  - abort key found
*               UIE_DISABLE - abort key not found
*===========================================================================
*/
SHORT DLLENTRY UieReadClearKey(VOID)
{
    FSCTBL  Fsc;

    PifRequestSem(usUieRingSem);

    /* --- check status of ring buffer full --- */

    if (UieRingCnt.fchControl & UIE_CNT_CLEAR) { /* exist clear key */
        UieClearRingBufSub();               /* clear ring buffer    */

        PifReleaseSem(usUieRingSem);
        return (UIE_ENABLE);                /* exit ...             */
    }

    if (UieRingCnt.uchClear == 0) {         /* not exist clear key  */
        PifReleaseSem(usUieRingSem);
        return (UIE_DISABLE);               /* exit ...             */
    }

    /* --- skip to until clear key --- */

    for (;;) {
        if (UieFscRingBuf(&Fsc) != UIE_SUCCESS) { /* buffer empty   */
            PifReleaseSem(usUieRingSem);
            return (UIE_DISABLE);                 /* exit ...       */
        }

        if (Fsc.uchMajor == FSC_KEYBOARD && /* read clear key       */
            Fsc.uchMinor == FSC_CLEAR) {
            break;                          /* exit ...             */
        }
    }

    PifReleaseSem(usUieRingSem);
    return (UIE_ENABLE);
}


/*
*===========================================================================
** Synopsis:    SHORT DLLENTRY UieReadAbortKey(VOID);
*
** Return:      UIE_ENABLE  - abort key found
*               UIE_DISABLE - abort key not found
*===========================================================================
*/
SHORT DLLENTRY UieReadAbortKey(VOID)
{
    FSCTBL  Fsc;

    PifRequestSem(usUieRingSem);

    /* --- check status of ring buffer full --- */

    if (UieRingCnt.fchControl & UIE_CNT_ABORT) { /* exist abort key */
        UieClearRingBufSub();               /* clear ring buffer    */

        PifReleaseSem(usUieRingSem);
        return (UIE_ENABLE);                /* exit ...             */
    }

    if (UieRingCnt.uchAbort == 0) {         /* not exist abort key  */
        PifReleaseSem(usUieRingSem);
        return (UIE_DISABLE);               /* exit ...             */
    }

    /* --- skip to until abort key --- */

    for (;;) {
        if (UieFscRingBuf(&Fsc) != UIE_SUCCESS) { /* buffer empty   */
            PifReleaseSem(usUieRingSem);
            return (UIE_DISABLE);                 /* exit ...       */
        }

        if (Fsc.uchMajor == FSC_KEYBOARD && /* read abort key       */
            Fsc.uchMinor == FSC_CANCEL) {
            break;                          /* exit ...             */
        }
    }

    PifReleaseSem(usUieRingSem);
    return (UIE_ENABLE);
}


/*
*===========================================================================
** Synopsis:    VOID UieRingBufInit(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID UieRingBufInit(VOID)
{
    usUieRingSem = PifCreateSem(1);

	PifHighLowSem(usUieRingSem, 0, 0, 0);  //  This semaphore used for pushing keystrokes into ring buffer
    UieClearRingBuf();
}


/*
*===========================================================================
** Synopsis:    VOID UieWriteRingBuf(FSCTBL Fsc, CONST VOID *pvData,
*                                    USHORT usLength);
*     Input:    Fsc      - function selection code
*               pvData   - address of input data
*               usLength - length of input data
*
** Return:      nothing
*
** Description: This function write input data in the ring buffer.
*===========================================================================
*/
VOID UieWriteRingBuf(FSCTBL Fsc, CONST VOID *pvData, USHORT usLength)
{
    USHORT  usOffset, usRemain, usWrite;

    PifRequestSem(usUieRingSem);

    /* --- check ring buffer status --- */
    if (UieRingSts.fchControl & UIE_RING_FULL ||    /* already buffer full  */
        UieRingSts.uchNumber == UIE_RING_NUM_INF || /* device info. full    */
        UieRingSts.usLength + usLength > UIE_RING_LEN_BUF) { /* buffer full */

        /* --- update error information --- */

        UieRingSts.fchControl |= UIE_RING_FULL;     /* set buffer full     */
        PifBeep(UIE_ERR_TONE);                      /* error tone          */
        UieTracking(UIE_TKEY_ERROR, UIE_TKEY_RB, 0);

        /* --- check input device --- */

        if (Fsc.uchMajor != FSC_KEYBOARD) {         /* not keyboard        */
            PifReleaseSem(usUieRingSem);
            return;                                 /* exit ...            */
        }

        if (Fsc.uchMinor == FSC_CLEAR) {            /* clear key           */
            UieRingCnt.fchControl |= UIE_CNT_CLEAR; /* set error status    */
        } else if (Fsc.uchMinor == FSC_CANCEL) {    /* abort key           */
            UieRingCnt.fchControl |= UIE_CNT_ABORT; /* set error status    */
        }

        PifReleaseSem(usUieRingSem);
        return;                                     /* exit ...            */
    }
    
    /* --- check keyboard device --- */
    if (Fsc.uchMajor == FSC_KEYBOARD) {             /* keyboard input      */
        if (Fsc.uchMinor == FSC_CLEAR) {            /* clear key           */
            UieRingCnt.uchClear++;                  /* increment counter   */
        } else if (Fsc.uchMinor == FSC_CANCEL) {    /* abort key           */
            UieRingCnt.uchAbort++;                  /* increment counter   */
        } else if (Fsc.uchMinor == FSC_AUTO_SIGN_OUT) {
			UieMessageBoxSendMessage (WM_COMMAND, IDABORT, 0);
		}
    }

    usWrite  = (USHORT)UieRingSts.uchWrite;     /* offset of device info.  */
    usOffset = UieRingSts.usOffset;             /* offset of ring buffer   */
    usRemain = UIE_RING_LEN_BUF - usOffset;     /* length of remain        */

    /* --- copy from user buffer to device info. --- */
    UieRingInf[usWrite].Fsc      = Fsc;
    UieRingInf[usWrite].uchMode  = UieMode.uchReally;
    UieRingInf[usWrite].usLength = usLength;
    UieRingInf[usWrite].usOffset = usOffset;

    /* --- copy from user buffer to ring buffer --- */
	if (pvData) {
		if (usLength <= usRemain) {
			memcpy(UieRingBuf + usOffset, pvData, usLength);
			usOffset = usOffset + usLength;         /* offset for next         */
		} else {
			memcpy(UieRingBuf + usOffset, pvData, usRemain);
			usOffset = usLength - usRemain;         /* offset for next         */
			memcpy(UieRingBuf, (UCHAR *)pvData + usRemain, usOffset);
		}
	}

    /* --- update device info. and ring buffer status --- */
    UieRingSts.uchNumber++;                     /* number of device info.  */
    UieRingSts.usLength += usLength;            /* length of ring buffer   */
    if (++UieRingSts.uchWrite == UIE_RING_NUM_INF) {/* end of device info. */
        UieRingSts.uchWrite = 0;                /* offset for top          */
    }
    if (usOffset == UIE_RING_LEN_BUF) {         /* end of ring buffer      */
        UieRingSts.usOffset = 0;                /* offset for top          */
    } else {
        UieRingSts.usOffset = usOffset;         /* offset for next         */
    }

    UieSwitchRingBuf();                         /* wake-up user thread     */

    PifReleaseSem(usUieRingSem);
}


/*
*===========================================================================
** Synopsis:    SHORT UieReadRingBuf(FSCTBL *pFsc, VOID *pvData,
*                                    USHORT *pusLength);
*    Output:    pFsc      - address of FSC
*               pvData    - address of input data
*               pusLength - address of length of input data
*
** Return:      UIE_SUCCESS - successful
*               UIE_ERROR   - error
*
** Description: This function takes out one data written in the ring buffer.
*===========================================================================
*/
SHORT UiePeekRingBuf(FSCTBL *pFsc, VOID *pvData, USHORT *pusLength)
{
    UCHAR   uchMode;
    USHORT  usOffset, usEffect, usRead;

    PifRequestSem(usUieRingSem);

    /* --- check position of mode key --- */

    if (UieRingSts.uchNumber) {                 /* exist input data       */
        uchMode = UieRingInf[UieRingSts.uchRead].uchMode; /* get position */
    } else {
        uchMode = UieMode.uchReally;            /* get really position    */
    }

    if (UieMode.uchEngine != uchMode) {         /* mode change            */
        UieMode.uchEngine = uchMode;            /* set engine position    */
        
        pFsc->uchMajor = FSC_MODE;
        pFsc->uchMinor = uchMode;
        *pusLength     = 0;

        PifReleaseSem(usUieRingSem);
        return (UIE_SUCCESS);                   /* exit ...               */
    }

    if (UieRingSts.uchNumber == 0) {            /* not exist input data   */
        PifReleaseSem(usUieRingSem);
        return (UIE_ERROR);                     /* exit ...               */
    }

    usRead = (USHORT)UieRingSts.uchRead;        /* offset of device info. */

    /* --- copy from device info. to user buffer --- */
    *pFsc      = UieRingInf[usRead].Fsc;
    *pusLength = UieRingInf[usRead].usLength;
    usOffset   = UieRingInf[usRead].usOffset;
    usEffect   = UIE_RING_LEN_BUF - usOffset;
	if (pvData) {
		if (usEffect >= *pusLength) {
			memcpy(pvData, UieRingBuf + usOffset, *pusLength);
		} else {
			memcpy(pvData, UieRingBuf + usOffset, usEffect);
			memcpy((UCHAR *)pvData + usEffect, UieRingBuf, *pusLength - usEffect);
		}
	}

    PifReleaseSem(usUieRingSem);
    return (UIE_SUCCESS);
}

SHORT UieReadRingBuf(FSCTBL *pFsc, VOID *pvData, USHORT *pusLength)
{
    UCHAR   uchMode;
    USHORT  usOffset, usEffect, usRead;

    PifRequestSem(usUieRingSem);

    /* --- check position of mode key --- */
    if (UieRingSts.uchNumber) {                 /* exist input data       */
        uchMode = UieRingInf[UieRingSts.uchRead].uchMode; /* get position */
    } else {
        uchMode = UieMode.uchReally;            /* get really position    */
    }

    if (UieMode.uchEngine != uchMode) {         /* mode change            */
        UieMode.uchEngine = uchMode;            /* set engine position    */
        
        pFsc->uchMajor = FSC_MODE;
        pFsc->uchMinor = uchMode;
        *pusLength     = 0;

        PifReleaseSem(usUieRingSem);
        return (UIE_SUCCESS);                   /* exit ...               */
    }

    if (UieRingSts.uchNumber == 0) {            /* not exist input data   */
        PifReleaseSem(usUieRingSem);
        return (UIE_ERROR);                     /* exit ...               */
    }

    usRead = (USHORT)UieRingSts.uchRead;        /* offset of device info. */

    /* --- copy from device info. to user buffer --- */
    *pFsc      = UieRingInf[usRead].Fsc;
    *pusLength = UieRingInf[usRead].usLength;
    usOffset   = UieRingInf[usRead].usOffset;
    usEffect   = UIE_RING_LEN_BUF - usOffset;
	if (pvData != 0) {
		if (usEffect >= *pusLength) {
			memcpy(pvData, UieRingBuf + usOffset, *pusLength);
		} else {
			memcpy(pvData, UieRingBuf + usOffset, usEffect);
			memcpy((UCHAR *)pvData + usEffect, UieRingBuf, *pusLength - usEffect);
		}
	}

    /* --- update read offset of device information --- */
    UieRingSts.uchNumber--;                     /* number of device info.  */
    UieRingSts.usLength -= *pusLength;          /* length of ring buffer   */
    if (++UieRingSts.uchRead == UIE_RING_NUM_INF) { /* end of device info. */
        UieRingSts.uchRead = 0;                     /* offset for top      */
    }

    /* --- check status of ring buffer full --- */
    if (UieRingSts.fchControl & UIE_RING_FULL) {    /* full                */
        UieRingSts.fchControl &= ~UIE_RING_FULL;    /* reset full          */
        UieRingCnt.fchControl &= ~(UIE_CNT_CLEAR |  /* reset status        */
                                   UIE_CNT_ABORT);
    }

    /* --- check clear and abort key --- */
    if (pFsc->uchMajor == FSC_KEYBOARD) {           /* keyboard data       */
        if (pFsc->uchMinor == FSC_CLEAR) {          /* clear key           */
            UieRingCnt.uchClear--;
        } else if (pFsc->uchMinor == FSC_CANCEL) {  /* abort key           */
            UieRingCnt.uchAbort--;
        }
	} else if (pFsc->uchMajor == FSC_OPERATOR_ACTION && pvData != 0) {                   /* CLEAR Key */
		UIE_OPERATORACTION  *pUieOperatorActionData = (UIE_OPERATORACTION  *)pvData;

		PifReleaseSem(usUieRingSem);
		{
			char xBuff[128];
			sprintf (xBuff, "FSC_OPERATOR_ACTION:  usOptions 0x%x  usClass %d  %d.", pUieOperatorActionData->usOptions, pUieOperatorActionData->usClass, pUieOperatorActionData->usAddress);
			NHPOS_ASSERT_TEXT(0, xBuff);
		}
		if (pUieOperatorActionData->usControlStringId < 1) {
			if (pUieOperatorActionData->usMdcAddress == MDC_SF_ALERT_TIME) {
				UieErrorMsgWithText(pUieOperatorActionData->usAddress, (UIE_WITHOUT | UIE_EM_DIALOG_2), _T(" Store & Forward storing is Active"));
			} else if (pUieOperatorActionData->usMdcAddress == MDC_SF_AUTOOFF_TIME) {
				UieErrorMsgWithText(pUieOperatorActionData->usAddress, (UIE_WITHOUT | UIE_EM_DIALOG_3), _T(" Store & Forward storing turned off."));
			}
		}
		return (UIE_SUCCESS);
	}

    PifReleaseSem(usUieRingSem);
    return (UIE_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    VOID UieClearRingBuf(VOID);
*
** Return:      nothing
*
** Description: This function deletes the data written in the device
*               information and the ring buffer.
*===========================================================================
*/
VOID UieClearRingBuf(VOID)
{
    PifRequestSem(usUieRingSem);
    UieClearRingBufSub();
    PifReleaseSem(usUieRingSem);
}


/*
*===========================================================================
** Synopsis:    VOID UieClearRingBufSub(VOID);
*
** Return:      nothing
*
** Description: This function deletes the data written in the device
*               information and the ring buffer.
*===========================================================================
*/
VOID UieClearRingBufSub(VOID)
{
    memset(&UieRingSts, 0, sizeof(UieRingSts));
    memset(&UieRingInf, 0, sizeof(UieRingInf));
    memset(&UieRingBuf, 0, sizeof(UieRingBuf));
    memset(&UieRingCnt, 0, sizeof(UieRingCnt));
}


/*
*===========================================================================
** Synopsis:    SHORT UieFscRingBuf(FSCTBL *pFsc);
*    Output:    pFsc - address of FSC
*
** Return:      UIE_SUCCESS - successful
*               UIE_ERROR   - error
*
** Description: This function takes out one data written in the ring buffer.
*===========================================================================
*/
SHORT UieFscRingBuf(FSCTBL *pFsc)
{
    USHORT  usLength;

    if (UieRingSts.uchNumber == 0) {            /* not exist input data    */
        return (UIE_ERROR);                     /* exit ...                */
    }

    /* --- copy from device info. to user buffer --- */

    *pFsc    = UieRingInf[UieRingSts.uchRead].Fsc;
    usLength = UieRingInf[UieRingSts.uchRead].usLength;

    /* --- update read offset of device information --- */

    UieRingSts.uchNumber--;                     /* number of device info.  */
    UieRingSts.usLength -= usLength;            /* length of ring buffer   */
    if (++UieRingSts.uchRead == UIE_RING_NUM_INF) { /* end of device info. */
        UieRingSts.uchRead = 0;                 /* offset for top          */
    }

    /* --- check clear and abort key --- */

    if (pFsc->uchMajor == FSC_KEYBOARD) {           /* keyboard data       */
        if (pFsc->uchMinor == FSC_CLEAR) {          /* clear key           */
            UieRingCnt.uchClear--;
        } else if (pFsc->uchMinor == FSC_CANCEL) {  /* abort key           */
            UieRingCnt.uchAbort--;
        }
    }

    return (UIE_SUCCESS);                       /* exit ...                */
}


/*
*===========================================================================
** Synopsis:    VOID UieSwitchRingBuf(VOID);
*
** Return:      nothing
*
** Description: This function tells the thing that there is a message in the
*               function to which UieDeviceGetToken() or UieErrorMsg() is
*               effective either.
*===========================================================================
*/
VOID UieSwitchRingBuf(VOID)
{
    if (fchUieActive & UIE_ACTIVE_GET) {    /* active module for normal */
        PifReleaseSem(usUieSyncNormal);
    } else {                                /* active module for error  */
        PifReleaseSem(usUieSyncError);
    }
}

/*
*===========================================================================
** Synopsis:    UieReadMode(VOID);
*
** Return:      nothing
*
** Description: This function returns the current mode that the application
*				is in. The mode indicates if the terminal is in Supervisor Mode,
*               Register Mode, Program Mode, etc.
*
*               See the defines
*                   UIE_POSITION2        lock mode       
*                   UIE_POSITION3        register mode   
*                   UIE_POSITION4        supervisor mode 
*                   UIE_POSITION5        program mode    
*===========================================================================
*/
UCHAR	UieReadMode(VOID)
{	
	UCHAR	uchMode;

	uchMode = UieMode.uchAppl;
	
	return uchMode;
}
/* ====================================== */
/* ========== End of UieRing.C ========== */
/* ====================================== */
