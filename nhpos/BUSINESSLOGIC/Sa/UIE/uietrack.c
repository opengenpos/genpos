/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Key Track Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieTrack.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date      : Ver.     : Name       : Description
* Mar-20-92 : 00.00.01 : O.Nakada   : Initial
* Sep-29-92 : 00.01.05 : O.Nakada   : Modify key track function.
* Jun-05-93 : C1       : O.Nakada   : Modify key track function.
* Jul-05-93 : C3       : O.Nakada   : Removed UIE_TEST_INIT.
* Sep-16-93 : F1       : O.Nakada   : Divided the User Interface Engine.
* Mar-15-95 : G0       : O.Nakada   : Modified all modules, correspond to
*                                     hospitality release 3.0 functions.
* Jul-12-95  G1        : O.Nakada   : Added UieWakeupTrack() function.
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* May-01-00 : 01.00.00 : M.Teraki   : Remove PifTrackKey() calling.
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
#include "ecr.h"
#include "pif.h"
#include "fsc.h"
#include "uie.h"
#include "uiel.h"
#include "uielio.h"
#include "uieio.h"
#include "uiering.h"
#include "uietrack.h"


/*                                                                                
*===========================================================================
*   General Declarations
*===========================================================================
*/
STATIC  UIETEST UieTest;                    /* loop tester status */


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UieTracking(UCHAR uchMsg, UCHAR uchData1,
*                                 UCHAR uchData2);
*     Input:    uchMsg   - message ID
*               uchData1 - tracking data #1
*               uchData2 - tracking data #2
*
** Return:      UIE_SUCCESS - accept tracking
*               UIE_ERROR   - not accept tracking
*===========================================================================
*/
SHORT UieTracking(UCHAR uchMsg, UCHAR uchData1, UCHAR uchData2)
{
    /* --- do executing loop tester ? --- */

    if (pUieSysCfg->ausOption[UIE_TEST_OPTION8] == UIE_TEST_PASS &&
        UieTest.fchStatus & UIE_TEST_EXECUTE) {
		/* ignor drawer close event, 06/12/01 */
	    if (uchMsg == UIE_TKEY_DRAWER) {                   /* close drawer         */
	    	return (UIE_SUCCESS);
	    } else {
        	UieCancelTester();                  /* cancel loop tester   */
        }
    }

    switch (uchMsg) {
    case UIE_TKEY_AN:                       /* alpha/numeric key    */
        UieTrackKey(uchData1);

        /* --- update parameter of loop tester --- */

        if ('0' <= uchData1 && uchData1 <= '9') {
            UieTest.uchPara1000 = UieTest.uchPara100;
            UieTest.uchPara100  = UieTest.uchPara10;
            UieTest.uchPara10 = UieTest.uchPara1;
            UieTest.uchPara1  = (UCHAR)(uchData1 - '0');
        } else {
            UieTest.uchPara1  = 0xFF;     /* disable loop parameter */
        }
        return (UIE_ERROR);

    case UIE_TKEY_FUNC:                     /* function key         */
        if (UieTrackTrigger(uchData1) == UIE_SUCCESS) {
            return (UIE_SUCCESS);
        }
        UieTrackKey(FSC_TRACK_FUNC);
        UieTrackKey(uchData1);
        UieTrackKey(uchData2);
        break;

    case UIE_TKEY_POWER:                    /* power down recovery  */
        UieTrackKey(FSC_TRACK_SWITCH);
        UieTrackKey('P');
        UieTrackKey(uchData1);
        break;

    case UIE_TKEY_MODE:                     /* mode change          */
        UieTrackKey(FSC_TRACK_SWITCH);
        UieTrackKey('M');
        UieTrackKey(uchData1);
        break;

    case UIE_TKEY_DRAWER:                   /* close drawer         */
        UieTrackKey(FSC_TRACK_SWITCH);
        UieTrackKey('D');
        UieTrackKey(uchData1);              /* drawer number        */
        break;

    case UIE_TKEY_SCALE:                    /* scale data           */
        UieTrackKey(FSC_TRACK_SWITCH);
        UieTrackKey('A');
        UieTrackKey(uchData1);              /* result code          */
        break;

    case UIE_TKEY_WAITER:                   /* change waiter lock   */
        UieTrackKey(FSC_TRACK_SWITCH);
        UieTrackKey('W');
        UieTrackKey(uchData1);              /* waiter lock number   */
        break;

    case UIE_TKEY_ERROR:                    /* double dpression     */
        UieTrackKey(FSC_TRACK_SWITCH);
        UieTrackKey('E');
        UieTrackKey(uchData1);              /* device status        */
        break;

    default:                                /* not used             */
        break;
    }

    UieTrackKey('\n');

    UieTest.uchPara1 = 0xFF;              /* disable loop parameter */
    return (UIE_ERROR);
}


/*
*===========================================================================
** Synopsis:    VOID UieTrackKey(UCHAR uchData);
*     Input:    uchData - track data
*
** Return:      nothing
*===========================================================================
*/
VOID UieTrackKey(UCHAR uchData)
{
    /* --- check password of loop tester, tracking --- */

    if (pUieSysCfg->ausOption[UIE_TEST_OPTION8] == UIE_TEST_PASS &&
        UieTest.fchStatus & UIE_TEST_TRACK) {

        /* --- check length of tacking data --- */

        if (UieTest.usNumber < pUieSysCfg->pKeyTrack->usMaxCount) {
            UieTest.usNumber++;
        } else {                            /* buffer overflow */
            UieCancelTester();
        }
    }

    /* PifTrackKey(uchData); */
    pUieSysCfg->pKeyTrack->usCount++;
    if (pUieSysCfg->pKeyTrack->usCount >= pUieSysCfg->pKeyTrack->usMaxCount) {
        pUieSysCfg->pKeyTrack->usCount = 0;
    }
    pUieSysCfg->pKeyTrack->auchKeyTrack[pUieSysCfg->pKeyTrack->usCount] = uchData;
}


/*
*===========================================================================
** Synopsis:    SHORT UieTrackTrigger(UCHAR uchMajor);
*     Input:    uchMajor - major code of FSC
*
** Return:      UIE_SUCCESS - accept tracking
*               UIE_ERROR   - not accept tracking
*===========================================================================
*/
SHORT UieTrackTrigger(UCHAR uchMajor)
{
    /* --- check password of loop tester --- */

    if (pUieSysCfg->ausOption[UIE_TEST_OPTION8] != UIE_TEST_PASS) {
        return (UIE_ERROR);
    }

    /* --- check parameter of loop tester --- */

    if (UieTest.uchPara1  == 0xFF ||                /* disable loop tester */
        UieTest.uchPara10 == 0xFF) {
        return (UIE_ERROR);
    }

    if (uchMajor == FSC_RECEIPT_FEED) {

        UCHAR uchFuncWait = 0;

        if (UieTest.fchStatus & UIE_TEST_TRACK) {   /* already tracking    */
            return (UIE_ERROR);
        }

        UieTest.fchStatus |= UIE_TEST_TRACK;        /* enable tracking     */
        UieTest.uchBetween = (UCHAR)(UieTest.uchPara10 * 10 +
                                     UieTest.uchPara1); /* between timer   */

        if (UieTest.uchPara100 != 0xFF) {    /* upper 2 digit is not ready */
            uchFuncWait = UieTest.uchPara100;
        }
        if (UieTest.uchPara1000 != 0xFF) {
            uchFuncWait += UieTest.uchPara1000 * 10;
        }
        UieTest.uchFuncWait = uchFuncWait;

        UieTest.usNumber   = 0;
        UieTest.usRead     = 0;
        pUieSysCfg->pKeyTrack->usCount =            /* top of buffer       */
        pUieSysCfg->pKeyTrack->usMaxCount;
    } else if (uchMajor == FSC_JOURNAL_FEED) {
        if (! (UieTest.fchStatus & UIE_TEST_TRACK)) { /* disable tracking  */
            return (UIE_ERROR);
        }

        UieTest.fchStatus |= UIE_TEST_EXECUTE;      /* enable loop tester  */
        UieTest.uchError   = (UCHAR)(UieTest.uchPara10 * 10 +
                                     UieTest.uchPara1); /* error timer     */
        UieTest.usNumber  -= 2;                     /* delete parameter    */
    } else {
        return (UIE_ERROR);
    }

    UieSetStanza(2);
    return (UIE_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    SHORT UieReadTrack(USHORT *pusType, FSCTBL *pFsc,
*                                  UCHAR *puchData);
*     InOut:    pusType  - address of FSC type
*               pFsc     - address of FSC
*               puchData - address of misc. data
*
** Return:      UIE_SUCCESS - message exist
*               UIE_ERROR   - buffer empty
*===========================================================================
*/
SHORT UieReadTrack(USHORT *pusType, FSCTBL *pRing, UCHAR *puchData)
{
    UCHAR   uchData;

    /* --- check password of loop tester, executing --- */

    if (pUieSysCfg->ausOption[UIE_TEST_OPTION8] != UIE_TEST_PASS ||
        ! (UieTest.fchStatus & UIE_TEST_EXECUTE)) {
        return (UIE_ERROR);                     /* exit ... */
    }

    /* --- check active module --- */

	PifSleep (50);  // momentary wait to emulate human operator delay time

    if (fchUieActive & UIE_ACTIVE_GET) {        /* normal message      */
        for (;;) {
            switch (uchData = UieGetTrackKey()) {
            case FSC_TRACK_SWITCH:
                switch (UieGetTrackKey()) {
                case 'M':                       /* mode change         */
                    pRing->uchMajor = FSC_MODE;
                    pRing->uchMinor = UieGetTrackKey();
                    UieGetTrackKey();           /* skip '\n'           */
                    return (UIE_SUCCESS);       /* exit ...            */
                
                case 'W':                       /* waiter lock         */
                case 'D':                       /* drawer closed       */
                case 'P':                       /* power down recovery */
                case 'E':                       /* error occred        */
                default:                        /* other               */
                    while (UieGetTrackKey() != '\n');   /* skip '\n'   */
                }
                break;

            case FSC_TRACK_FUNC:                /* function key        */
                if (UieTest.uchFuncWait <= 65) {
                    PifSleep((USHORT)(UieTest.uchFuncWait * 1000));
                } else {
                    PifSleep(65 * 1000);
                    PifSleep((USHORT)((UieTest.uchFuncWait - 65) * 1000));
                }
                *pusType        = UIE_FUNC_KEY;
                pRing->uchMajor = FSC_KEYBOARD;
                pRing->uchMinor = 0;
                * puchData      = UieGetTrackKey();
                *(puchData + 1) = UieGetTrackKey();
                UieGetTrackKey();               /* skip '\n'           */
                return (UIE_SUCCESS);           /* exit ...            */
            
            default:
                *pusType        = UIE_AN_KEY;
                pRing->uchMajor = FSC_KEYBOARD;
                pRing->uchMinor = 0;
                * puchData      = uchData;
                *(puchData + 1) = 0;
                return (UIE_SUCCESS);           /* exit ...            */
            }
        }
    } else {                                    /* erro message        */
        /* --- check parameter of error condition --- */

        if (UieTest.uchError) {                 /* enable auto clear   */

            /* --- execute sleep of error cancel --- */

            if (UieTest.uchError <= 65) {
                PifSleep((USHORT)(UieTest.uchError * 1000)); /* ### Mod (2171 for Win32) V1.0 */
            } else {
                PifSleep(65 * 1000);
                PifSleep((USHORT)((UieTest.uchError - 65) * 1000)); /* ### Mod (2171 for Win32) V1.0 */
            }

            *pusType        = UIE_FUNC_KEY;
            pRing->uchMajor = FSC_KEYBOARD;
            pRing->uchMinor = 0;
            * puchData      = FSC_CLEAR;
            *(puchData + 1) = 0;
            return (UIE_SUCCESS);               /* exit ...            */
        } else {                                /* disable auto clear  */
            UieCancelTester();                  /* cancel loop tester  */
            return (UIE_ERROR);                 /* exit ...            */
        }
    }
}


/*
*===========================================================================
** Synopsis:    UCHAR UieGetTrackKey(VOID);
*
** Return:      character data in tracking buffer
*===========================================================================
*/
UCHAR UieGetTrackKey(VOID)
{
    /* --- check end of tracking --- */

    if (UieTest.usRead + 1 > UieTest.usNumber) {

        /* --- execute sleep of transaction --- */

        if (UieTest.uchBetween <= 65) {
            PifSleep((USHORT)(UieTest.uchBetween * 1000)); /* ### Mod (2171 for Win32) V1.0 */
        } else {
            PifSleep(65 * 1000);
            PifSleep((USHORT)((UieTest.uchBetween - 65) * 1000)); /* ### Mod (2171 for Win32) V1.0 */
        }

        UieTest.usRead = 0;                 /* tracking for top */
    }

    return (pUieSysCfg->pKeyTrack->auchKeyTrack[UieTest.usRead++]);
}


/*
*===========================================================================
** Synopsis:    VOID UieWakeupTrack(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID UieWakeupTrack(VOID)
{
    /* --- check password of loop tester, executing --- */

    if (pUieSysCfg->ausOption[UIE_TEST_OPTION8] != UIE_TEST_PASS ||
        ! (UieTest.fchStatus & UIE_TEST_EXECUTE)) {
        return;                                 /* exit ...            */
    }

    /* --- check active module --- */

    if (! (fchUieActive & UIE_ACTIVE_GET)) {    /* error message       */
        return;                                 /* exit ...            */
    }

    UieSwitchRingBuf();                         /* wake-up user thread */
}


/*
*===========================================================================
** Synopsis:    VOID UieCancelTester(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID UieCancelTester(VOID)
{
    UieSetStanza(4);
    UieTest.fchStatus   = 0;
    UieTest.uchPara1    = 0xFF;
    UieTest.uchPara10   = 0xFF;
    UieTest.uchPara100  = 0xFF;
    UieTest.uchPara1000 = 0xFF;
    pUieSysCfg->pKeyTrack->usCount = pUieSysCfg->pKeyTrack->usMaxCount;
}


/*
*===========================================================================
** Synopsis:    VOID UieSetStanza(USHORT usNumber);
*     Input:    usNumber - number of stanza
*
** Return:      nothing
*===========================================================================
*/
VOID UieSetStanza(USHORT usNumber)
{
    USHORT  usLoop, usOffset = 0;
    STANZA  Stanza[10];

    /* --- check argument --- */

    if (usNumber >= 5) {                        /* range error   */
        return;                                 /* exit ...      */
    }

    /* --- make stanza data --- */

    for (usLoop = 0; usLoop < usNumber; usLoop++) {
        Stanza[usOffset].usDuration    = 100;                        /* sleep         */
        Stanza[usOffset++].usFrequency = STANZA_FREQUENCY_SILENCE;
        Stanza[usOffset].usDuration    = 100;                        /* stanza        */
        Stanza[usOffset++].usFrequency = 2000;
    }
    Stanza[usOffset].usDuration = STANZA_DURATION_END_STANZA;        /* end of stanza */

    PifPlayStanza((CONST STANZA FAR *)&Stanza);
}


/* ======================================= */
/* ========== End of UieTrack.C ========== */
/* ======================================= */
