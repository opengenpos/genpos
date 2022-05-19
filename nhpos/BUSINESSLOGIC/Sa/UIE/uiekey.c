/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Keyboard Module
* Category    : User Interface Enigne, NCR 2170 System Application
* Program Name: UieKey.C
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
* Abstract:     The provided function names are as follows:
*
*                   UieSetCvt()     - set conversion table of user mode
*                   UieSetFsc()     - set FSC table of user mode
*                   UieSetAbort()   - set cancel condition of abort key
*                   UieModeChange() - update status of mode changable
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* May-25-93  C1        O.Nakada     Removed input device control functions.
* Mar-15-95  G0        O.Nakada     Modified all modules, correspond to
*                                   hospitality release 3.0 functions.
*** NCR 2171 ***
* Sep-29-99  01.00.00  M.Teraki     Initial
*
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2 R.Chambers  removed commented out source for UieKeyboardThread()
*                                   code not used since conversion from NCR 2172 to NCR 7448
* Apr-14-15  GenPOS 2.2 R.Chambers  refresh screen with default layout when key lock change to REG MODE.
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
#include <tchar.h>
#include <string.h>
#include <stdio.h>

#include "ecr.h"
#include "uie.h"
#include "pif.h"
#include "fsc.h"
#include "cvt.H"
#include <plu.h> 
#include "vos.h"
#include <regstrct.h>
#include <uireg.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include "uiel.h"
#include "uieio.h"
#include <display.h>
#include "uiering.h"
#include "uiedisp.h"
#include "uieerror.h"
#include "uietrack.h"
#include "UIELIO.H"
#include "BlFWif.h"

                                                                                
/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
UIEKBD  UieKbd;                             /* conversion and FSC table */
UIEMODE UieMode;                            /* mode lock status         */

static  UCHAR   fchUieAbort;                       /* abort key control */
static  UCHAR   fsUieRunTrack = UIE_ENABLE;        /* tracking control  */

TCHAR	aszUieKeyboardBuff[256];			// TOUCHSCREEN Buffer for WCHAR data from Keyboard/OS

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieSetCvt(CONST CVTTBL FAR *pCvt);
*     Input:    pCvt - address of conversion table
*
** Return:      nothing
*===========================================================================
*/
VOID  UieSetCvt(CONST CVTTBL *pCvt)
{
    UieKbd.uchCType                               = UIE_KBD_USER;
    UieKbd.pCvt[UieMode.uchAppl][UieKbd.uchCType] = pCvt;
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieSetFsc(CONST FSCTBL FAR *pFsc);
*     Input:    pFsc - address of FSC table
*
** Return:      nothing
*===========================================================================
*/
VOID  UieSetFsc(CONST FSCTBL *pFsc)
{
    UieKbd.uchFType                               = UIE_KBD_USER;
    UieKbd.pFsc[UieMode.uchAppl][UieKbd.uchFType] = pFsc;
}


/*
*===========================================================================
**  Synopsis:   UCHAR DLLENTRY UieSetAbort(UCHAR fchAbort);
*      Input:   fchAbort - Mode Position
*
**  Return:     Previous Mode Position
*===========================================================================
*/
UCHAR  UieSetAbort(UCHAR fchAbort)
{
    UCHAR   fchPrevious;
    
    fchPrevious = fchUieAbort;
    fchUieAbort = fchAbort;

    return (fchPrevious);
}


/*
*===========================================================================
**  Synopsis:   USHORT DLLENTRY UieModeChange(BOOL fStatus);
*      Input:   fStatus - mode change Stauts
*
**  Return:     Previous Status
*===========================================================================
*/
USHORT  UieModeChange(BOOL fStatus)
{
    BOOL    fPrevious;

    /* --- get current status of mode change --- */
    fPrevious = (UieMode.fchStatus & UIE_MODE_CHG) ? (UIE_ENABLE) : (UIE_DISABLE);

    /* --- set new status of mode change --- */
    if (fStatus == UIE_ENABLE) {
        UieMode.fchStatus |= UIE_MODE_CHG;
		//if building for touchscreen
		//enable the keylock when mode change
		//is allowed
		UieCtrlDevice(UIE_ENA_KEYLOCK);
    } else {
        UieMode.fchStatus &= ~UIE_MODE_CHG;
		//if building for touchscreen
		//disable the keylock when mode change
		//is NOT allowed
		//this keeps the user from inadvertently
		//changing the "key position" and seemingly
		//"locking up" the terminal
		UieCtrlDevice(UIE_DIS_KEYLOCK);
    }

    return (fPrevious);
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieSetStatusKeyboard(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID  UieSetStatusKeyboard(VOID)
{
    /* --- set default keyboard --- */
    UieKbd.uchCType = UIE_KBD_DEFAULT;
    UieKbd.uchFType = UIE_KBD_DEFAULT;
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieSetModeS2A(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID  UieSetModeS2A(VOID)
{
    /* --- check mode position in macro function --- */
    if (UieMacro.fsControl & UIE_MACRO_MODE) {
        UieMode.uchAppl = UieMode.uchMacro;     /* set macro mode */
    } else {
        UieMode.uchAppl = UieMode.uchEngine;    /* set appl. mode */
    }
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieSetModeS2D(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID  UieSetModeS2D(VOID)
{
	extern UCHAR iuchUieFuncSts;                       /* Function Status Buffer */
    /* --- backup to current mode --- */
    UieMode.uchDialog[iuchUieFuncSts] = UieMode.uchEngine;
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieSetModeD2S(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID  UieSetModeD2S(VOID)
{
	extern UCHAR iuchUieFuncSts;                       /* Function Status Buffer */
    /* --- resotre to previous mode --- */
    UieMode.uchEngine = UieMode.uchDialog[iuchUieFuncSts];
}


/*
*===========================================================================
** Synopsis:    VOID UieKeyboardInit(CONST CVTTBL FAR * FAR *pCvt,
*                                    CONST FSCTBL FAR * FAR *pFsc,
*                                    UCHAR uchMax);
*     Input:    pCvt   - address of conversion table
*               pFsc   - addres of FSC table
*               uchMax - number of mode lock position
*
** Return:      nothing
*===========================================================================
*/
VOID UieKeyboardInit(CONST CVTTBL  * *pCvt, CONST FSCTBL  * *pFsc, UCHAR uchMax)
{
    const   SYSCONFIG *pSysConfig = PifSysConfig();
    USHORT  usLoop;

    /* --- set conversion and FSC table --- */
    UieKbd.uchCType = UIE_KBD_DEFAULT;          /* default conversion */
    UieKbd.uchFType = UIE_KBD_DEFAULT;          /* default FSC        */

    for (usLoop = 0; usLoop < uchMax; usLoop++) {
        UieKbd.pCvt[usLoop][UieKbd.uchCType] = *(pCvt + usLoop);
        UieKbd.pFsc[usLoop][UieKbd.uchFType] = *(pFsc + usLoop);
    }

    /* --- mode lock --- */

    UieMode.fchStatus = UIE_MODE_CHG;           /* mode changable     */
#if 0
    UieMode.uchReally = PifGetKeyLock() - (UCHAR)PIF_KEYLOCK;   /* !!! NOT CONFIRMED YET !!! */

    if (UieKbd.pCvt[UieMode.uchReally][UIE_KBD_DEFAULT] == NULL) {
        UieMode.fchStatus |= UIE_MODE_LOCK;     /* disable keyboard   */
    } else {
        UieMode.fchStatus &= ~UIE_MODE_LOCK;    /* enable keyboard    */
    }
#else
    UieMode.fchStatus |= UIE_MODE_LOCK;     /* disable keyboard   */
#endif

    UieMode.uchReally = UIE_INIT_MODE;
    UieMode.uchEngine = UIE_INIT_MODE;
    UieMode.uchMacro  = UIE_INIT_MODE;
}

/*
*===========================================================================
** Synopsis:    USHORT UieGetKeyboardLockMode (VOID)
*
*     Input:    None
*
*    Output:    Nothing
*
** Return:      indicator as to current keyboard lock position.  One of
*                  PIF_NCRKLOCK_MODE_REG
*                  PIF_NCRKLOCK_MODE_SUP
*                  PIF_NCRKLOCK_MODE_PROG
*                  PIF_NCRKLOCK_MODE_LOCK
*                  PIF_NCRKLOCK_MODE_EX
*===========================================================================
*/
USHORT  UieGetKeyboardLockMode (VOID)
{
	return UieMode.uchReally;
}


/*
*===========================================================================
** Synopsis:    USHORT UieConvertFsc(CONST CVTTBL FAR *pCvt,
*                                    CONST FSCTBL FAR *pFsc,
*                                    CHARDATA Char, FSCTBL *pData);
*     Input:    pCvt  - address of conversion table
*               pFsc  - address of FSC table
*               Char  - keyboard data
*    Output:    pData - address of FSC
*
** Return:      type of FSC data
*
** Description: This function converts the keyboard input into FSC according
*               to the conversion table.
*===========================================================================
*/
USHORT UieConvertFsc(CONST CVTTBL *pCvt, CONST FSCTBL *pFsc, CHARDATA Char, FSCTBL *pData)
{
    USHORT   uchScan, uchBias;

    if (pCvt == NULL) {                     /* lock mode               */
        return (UIE_ERROR);                 /* exit ...                */
    }

    if ('0' <= Char.uchASCII && Char.uchASCII <= '9') { /* 10 key      */
        pData->uchMajor = Char.uchASCII;
        pData->uchMinor = 0;
        return (UIE_AN_KEY);
    }

	if ((Char.uchFlags & CHARDATA_FLAGS_HARDKEY) != 0) {
        NHPOS_ASSERT(Char.uchScan <= 0xFF);//MAXBYTE
		pData->uchMajor = Char.uchASCII;
        pData->uchMinor = Char.uchScan;
		return (UIE_FUNC_KEY);
	} else if ((Char.uchFlags & CHARDATA_FLAGS_TOUCHUTTON) != 0) {
		if (UieMode.uchReally == UIE_POSITION5)
		{
			pCvt = CvtProgTouch;                         
		} else
		{
			pCvt = CvtRegTouch;                         
		}
		pFsc = &ParaFSC[uchDispCurrMenuPage-1].TblFsc[0];
	}

    uchBias = Char.uchASCII + Char.uchScan - (UCHAR)UIE_FSC_BIAS;
	uchScan = pCvt[uchBias].uchASCII;
    if (uchScan) {                          /* aplha key               */
        NHPOS_ASSERT(uchScan <= 0xFF);//MAXBYTE
		pData->uchMajor = (UCHAR)uchScan;
        pData->uchMinor = 0;
        return (UIE_AN_KEY);
    }

	uchScan = pCvt[uchBias].uchScan;        /* get offset of FSC table */
    if (uchScan < UIE_FSC_BIAS || pFsc == NULL) {   /* fixed key       */
        NHPOS_ASSERT(uchScan <= 0xFF);//MAXBYTE
        pData->uchMajor = (UCHAR)uchScan;
        pData->uchMinor = 0;
    } else {                                        /* FSC             */
		// the UNINIRAM  Para.ParaFSC tables are arrays of FSCDATA however
		// the array we use for the lookup is an array of FSCTBL instead.
		//  - FSCDATA uchFsc is the same as FSCTBL uchMajor
		//  - FSCDATA uchExt is the same as FSCTBL uchMinor
		//
		// The value stored in Para.ParaFSC.TblFsc[].uchFsc is the unbiased
		// FSC value from the list in fsc.h such as the define FSC_PRESET_AMT
		// though the define is "UIE_FSC_BIAS + 53" while the value in the
		// the table is just 53 or FSC_PRESET_AMT - UIE_FSC_BIAS.
		//
		// The value stored in the Conversion Table is UIE_FSC_BIAS + offset
		// in the current menu of Para.ParaFSC.TblFsc[] so in order to find
		// the correct offset in Para.ParaFSC.TblFsc[] we subtract UIE_FSC_BIAS
		// from the uchScan value of the entry in the keyboard conversion table
		// and then use this value to index into the current menu of Para.ParaFSC.TblFsc[].
        pData->uchMajor = pFsc[uchScan - UIE_FSC_BIAS].uchMajor + (UCHAR)UIE_FSC_BIAS;
        pData->uchMinor = pFsc[uchScan - UIE_FSC_BIAS].uchMinor;

		if (pData->uchMajor == FSC_CLEARFSC) {
			pData->uchMajor = FSC_CLEAR;
		}
    }

    return (UIE_FUNC_KEY);
}


/*
*===========================================================================
**  Synopsis:   USHORT UieCheckAbort(UCHAR uchMajor, UCHAR uchMode);
*      Input:   uchMajor - major code
*               uchMode  - position of mode key
*
**  Return:     0     - no Abort Key
*               not 0 - Abort Key
*===========================================================================
*/
USHORT UieCheckAbort(UCHAR uchMajor, UCHAR uchMode)
{
    if (uchMajor != FSC_CANCEL && uchMajor != FSC_P5) {
        return (0);
    }

    switch (uchMode) {
    case UIE_POSITION0:
        return (fchUieAbort & UIE_POS0);
    case UIE_POSITION1:
        return (fchUieAbort & UIE_POS1);
    case UIE_POSITION2:
        return (fchUieAbort & UIE_POS2);
    case UIE_POSITION3:
        return (fchUieAbort & UIE_POS3);
    case UIE_POSITION4:
        return (fchUieAbort & UIE_POS4);
    case UIE_POSITION5:
        return (fchUieAbort & UIE_POS5);
    default:
        return (0);
    }
}


/*
*===========================================================================
** Synopsis:    SHORT UieCheckMode(VOID);
*
** Return:      UIE_SUCCESS - mode changable
*               UIE_ERROR   - mode unchangable
*===========================================================================
*/
SHORT UieCheckMode(VOID)
{
    /* --- check status of mode change --- */
    if (UieMode.fchStatus & UIE_MODE_CHG) {     /* mode changable */
        return (UIE_SUCCESS);
    }

    /* --- check mode position in macro function --- */
    if (UieMacro.fsControl & UIE_MACRO_MODE) {
        if (UieMode.uchAppl != UieMode.uchMacro) {  /* mismatch mode */
            return (UIE_ERROR);
        }
    } else {
        if (UieMode.uchAppl != UieMode.uchEngine) { /* mismatch mode */
            return (UIE_ERROR);
        }
    }

    return (UIE_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    LONG UiePutKeyData(ULONG ulLength,
*                                  CONST CHARDATA *pChar,
*                                  VOID *pResult);
*
*     Input:    ulLength - length of data from pChar
*               pChar    - KeyData from external module
*
*    Output:    pResult - reserved
*
** Return:      reserved
*
** Description: Accept keydata and write to the ring buffer
*===========================================================================
*/
LONG UiePutKeyData(ULONG ulLength, CONST CHARDATA *pChar, VOID *pResult)
{
    UCHAR       uchMode;        /* Lock-Key position (zero-orgin) */
    FSCTBL      Ring;           /* for write into ring-buffer */
    FSCTBL      Fsc;            /* for judgement of [CLEAR], [ABORT], ... */
    USHORT      usType;         /* keytype (functionkey, alpha-num, ...) */
    LONG        lRet = -1;      /* return value for caller */

    Ring.uchMajor = FSC_KEYBOARD;           /* initialization */

    if (pChar->uchASCII) {                  /* keyboard data          */

        /* --- if not initialized yet, trash it away --- */
        if (UieMode.uchReally == UIE_INIT_MODE) {   /* not initialized */
            return lRet;
        }

        /* --- if keyboard is disabled, trash it away --- */
        if (UieMode.fchStatus & UIE_MODE_LOCK) {
            return lRet;
        }
        
        /* --- check previous status for back light of LCD --- */
        if (VosSetBLight(VOS_BLIGHT_ON) == VOS_BLIGHT_OFF) { /* off   */
            return lRet;
        }

        /* 2172 RS */
        if (!(fsUieKeyDevice & UIE_ENA_KEYBOARD)) {
            return lRet;
        }

        /* --- Judge if the received key has special meaning --- */
        usType = UieConvertFsc(UieKbd.pCvt[UieMode.uchReally][UIE_KBD_DEFAULT],
                               UieKbd.pFsc[UieMode.uchReally][UIE_KBD_DEFAULT],
                               *pChar,
                               &Fsc);

        if (usType == UIE_FUNC_KEY) {               /* function key   */
            if (Fsc.uchMajor == FSC_CLEAR) {        /* clear key      */
                Ring.uchMinor = FSC_CLEAR;
				if ((fchUieActive & UIE_EVENT_WAIT) != 0) {
					UifSignalStringWaitEvent (CNTRL_STRING_EVENT_CLEAR_QUEUE);    // signal the wait that a Clear has been pressed.
				}
            } else if (UieCheckAbort(Fsc.uchMajor, UieMode.uchReally)) {
                Ring.uchMinor = FSC_CANCEL;
				if ((fchUieActive & UIE_EVENT_WAIT) != 0) {
					UifSignalStringWaitEvent (CNTRL_STRING_EVENT_CLEAR_QUEUE);    // signal the wait that a Clear has been pressed.
				}
            } else {
                Ring.uchMinor = 0;
            }
        } else if (usType == UIE_AN_KEY) {      /* alpha/numeric key  */
            Ring.uchMinor = 0;
        } else {                                /* lock position      */
            return lRet;
        }

        /* tone, 2172 saratoga, V1.0.05 */
		if(PifSysConfig()->uchKeyType != KEYBOARD_TOUCH){
			PifBeep(UIE_CLICK_TONE);
		}

        /* --- write 'Ring' to ring buffer. not 'Fsc' --- */
        UieWriteRingBuf(Ring, pChar, sizeof(CHARDATA));
    } else {                                    /* not keyboard data  */
        if (pChar->uchScan >= PIF_KEYLOCK && pChar->uchScan <= PIF_KEYLOCK + 5) {
			UCHAR uchReallyOld = UieMode.uchReally;

			/* mode or keylock change */
            /* 2172 RS */
            if (!(fsUieKeyDevice & UIE_ENA_KEYLOCK)) {
                return lRet;
            }

            /* --- update drawer status --- */
            UieCheckDrawerStatus(1);

            /* --- update new position of mode lock --- */
            uchMode           = pChar->uchScan - (UCHAR)PIF_KEYLOCK;
            UieMode.uchReally = uchMode;

            /* --- update 'keyboard lock' status --- */
            if (! (UieMode.fchStatus & UIE_MODE_LOCK)) {    /* enable -> disable */
                if (UieKbd.pCvt[uchMode][UIE_KBD_DEFAULT] == NULL) {
                    UieMode.fchStatus |= UIE_MODE_LOCK;
                }
            } else {                                        /* disable -> enable */
                if (UieKbd.pCvt[uchMode][UIE_KBD_DEFAULT] != NULL) {
                    UieMode.fchStatus &= ~UIE_MODE_LOCK;
                }
            }

			if (UieMode.uchReally == PIF_NCRKLOCK_MODE_REG) {
				// if switching to Register Mode then redisplay the default user screen.
				// we could check to see if this is a key lock change however we will just
				// go ahead and redisplay anyway in case someone is pressing the REG MODE
				// key to trigger this behavior.
				BlFwIfDefaultUserScreen ();
			}

            UieSwitchRingBuf();                 /* wake-up user thread */
			{
				char  xBuff[128];
				sprintf (xBuff, "==STATE: keylock change from %d to %d.", uchReallyOld, UieMode.uchReally);
				NHPOS_NONASSERT_NOTE("==STATE", xBuff);
			}
        } else if (pChar->uchScan == PIF_VIRTUALKEYEVENT) {
			UieWriteRingBuf(Ring, pChar, sizeof(CHARDATA));
        } else if (pChar->uchScan == PIF_ERROR_KEY_POWER_FAILURE) {
            UieCoinInit();
            UieRefreshDisplay();
            if (fsUieRunTrack == UIE_ENABLE) {  /* special tracking    */
                UieTracking(UIE_TKEY_POWER, 1, 0);
            }
        } else if (pChar->uchScan == PIF_KEYERROR) { /* double dpression */
            PifBeep(UIE_ERR_TONE);
            UieTracking(UIE_TKEY_ERROR, UIE_TKEY_KEYDP, 0);
        }

        VosSetBLight(VOS_BLIGHT_ON);            /* back light on       */
    }

    return lRet;
}


/*
*===========================================================================
** Synopsis:    long UieSendKeyboardInput(ULONG ulLength, CONST TCHAR* pData);
*     Input:    ulLength - length of data
				pData - Character to write
*     InOut:    
*
** Return:      UIE_SUCCESS - Written to buffer
				UIE_ERROR - buffer full, not in alpha mode

  verify that the buffer is not full
  if not write character data to buffer place null terminator
  to signify end of buffer
  let getmessage know so UieReadKeyboardData gets the data
  and refreshes display

  Added for TOUCHSCREEN support.
*===========================================================================
*/

LONG UieSendKeyboardInput(ULONG ulLength, CONST TCHAR* pData)
{
	ULONG  i = 0;
	ULONG  iBuffSize = sizeof(aszUieKeyboardBuff)/sizeof(aszUieKeyboardBuff[0]) - 1;

	//not in Alpha Mode do not write to keyboard buff
	if(UieKbd.uchCType != UIE_KBD_USER)
	{
		return UIE_ERROR;
	}

	i = _tcslen(aszUieKeyboardBuff); //Get the length of the Buffer so the next WCHAR can go on the end
	if(i >= iBuffSize)
	{
		PifBeep(UIE_ERR_TONE);  //Buffer FULL beep and don't write data
		return UIE_ERROR;
	}

	//if the current buffer plus new data exceeds buffer shorten 
	//the data to be written
	if(i + ulLength >= iBuffSize)
	{
		ulLength = iBuffSize - i;
	}

	_tcsncpy(&aszUieKeyboardBuff[i], pData, ulLength);	//Set the TCHAR to the end of the Buffer
	aszUieKeyboardBuff[i + ulLength] = 0x00; //put Null at end

	if(aszUieKeyboardBuff[0])
	{
		PifReleaseSem(usUieSyncNormal); //Wake up GetMessage
	}

	return UIE_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    VOID UieClearKeyboardBuff();
*     Input:    Nothing
*     InOut:    
*
** Return:      Nothing

  Clear the Keyboard buffer

  Added for TOUCHSCREEN support.
*===========================================================================
*/
VOID UieClearKeyboardBuf()
{
	//Clear the Keyboard Buffer
	memset(aszUieKeyboardBuff, 0x00, sizeof(aszUieKeyboardBuff));
}

USHORT UieSetKBMode (USHORT  usNewMode)
{
    SYSCONFIG    *pSysConfig = PifSysConfig();    /* get system config, will generate warning C4090 */
	USHORT       usOldMode;

	usOldMode = pSysConfig->uchKBMode;  // UifACChgKBType() 
	pSysConfig->uchKBMode = (UCHAR)(usNewMode & 0x00ff);

	return usOldMode;
}

/* ===================================== */
/* ========== End of UieKey.C ========== */
/* ===================================== */
