/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Item module I/F
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDOTHER.C                                                      
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Apr-13-95 : 03.00.00 : M.Ozawa    : Initial
* Apr-16-99 : 03.05.00 : M.Teraki   : Merge STORE_RECALL_MODEL
*           :          :            :       /GUEST_CHECK_MODEL
* Dec-14-15 : 02.02.01 : R.Chambers : eliminated MLD_NUM_SEAT using standard NUM_SEAT.
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

/**
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <regstrct.h>
#include <transact.h>
#include <csttl.h>
#include <rfl.h>
#include <prtprty.h>
#include <vos.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/*
============================================================================
+                  S T A T I C   R A M s
============================================================================
*/

#if DISPTYPE==1

TCHAR   aszMldOrderNo[] = _T(" %s\t%03d");

TCHAR   aszMldWaiting[] = _T("%s");   /* waiting mnemonic */

extern CONST TCHAR   aszMldSignIn[];

extern CONST TCHAR   aszMldGuest_WTCD[];
extern CONST TCHAR   aszMldGuest_WOCD[];

CONST TCHAR   aszMldGuest_More[] = _T("%-s");

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s                      +
;========================================================================
**/

/*
*===========================================================================
** Format  : USHORT   MldDispSeatNum(USHORT usScroll, MLDSEAT pSeatNum);
*               
*    Input : USHORT     usScroll
*          : MLDSEATNUM pSeatNum
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays seat number on order no area
*===========================================================================
*/
SHORT MldDispSeatNum(USHORT usScroll, MLDSEAT *pSeatNum)
{
    TCHAR   uchStrBuff[11] = { 0 };
    USHORT  usWrtLen = 0;

    if (pSeatNum->auchSeatNum[0] == 0) return(MLD_SUCCESS);

    if (pSeatNum->auchSeatNum[3] == 0) {
		TCHAR   szMnemonic[PARA_SPEMNEMO_LEN + 1] = { 0 };

		RflGetSpecMnem(szMnemonic, SPC_SEAT_ADR);

        usWrtLen = tcharlen(szMnemonic);
        _tcsncpy( uchStrBuff, szMnemonic, PARA_SPEMNEMO_LEN);
        uchStrBuff[usWrtLen] = _T(' ');
        usWrtLen++;
        for (SHORT i = 0; i < 4; i++) {
            if (pSeatNum->auchSeatNum[i] == 0) {
                usWrtLen--;
                break;
            }
            uchStrBuff[usWrtLen] = (UCHAR)(pSeatNum->auchSeatNum[i] | 0x30);
            usWrtLen++;
            uchStrBuff[usWrtLen] = _T(' ');
            usWrtLen++;
        }
    } else {
        uchStrBuff[0] = _T('S');
        uchStrBuff[1] = _T(' ');
        usWrtLen = 2;
        for (SHORT i = 0; i < NUM_SEAT; i++) {
            if (pSeatNum->auchSeatNum[i] == 0) {
                break;
            }
            uchStrBuff[usWrtLen] = (UCHAR)(pSeatNum->auchSeatNum[i] | 0x30);
            usWrtLen++;
        }
    }

    if (usScroll & MLD_DRIVE_1) {
		UCHAR   uchAttr = (UCHAR)(MldOtherHandle1.usOrderAttrib ? MLD_C_WHITE : MLD_C_BLACK);

        VosSetCurPos(MldOtherHandle1.usOrderHandle, 0, 1);

        VosStringAttr(MldOtherHandle1.usOrderHandle, uchStrBuff, usWrtLen, uchAttr);

    }
    if (usScroll & MLD_DRIVE_2) {
		UCHAR   uchAttr = (UCHAR)(MldOtherHandle2.usOrderAttrib ? MLD_C_WHITE : MLD_C_BLACK);

        VosSetCurPos(MldOtherHandle2.usOrderHandle, 0, 1);

        VosStringAttr(MldOtherHandle2.usOrderHandle, uchStrBuff, usWrtLen, uchAttr);

    }
    if (usScroll & MLD_DRIVE_3) {
		UCHAR   uchAttr = (UCHAR)(MldOtherHandle3.usOrderAttrib ? MLD_C_WHITE : MLD_C_BLACK);

        VosSetCurPos(MldOtherHandle3.usOrderHandle, 0, 1);

        VosStringAttr(MldOtherHandle3.usOrderHandle, uchStrBuff, usWrtLen, uchAttr);

    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : USHORT   MldDispGuestNum(MLDGUEST pGuestNum);
*               
*    Input : MLDGUESTNUM pGuestNum
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays guest number at sign-in, V3.3
*===========================================================================
*/
SHORT MldDispGuestNum(MLDGUESTNUM *pGuestNum)
{
    TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */

	RflGetTranMnem(aszTranMnem, TRN_GCNO_ADR);

    for (SHORT i = 0; i < MLD_GUESTNO_NUM ; i++) {
        if (pGuestNum->GuestNo[i].usGuestNo == 0) break;
		if (pGuestNum->GuestNo[i].usGuestNo == (USHORT)-1){
			MldPrintf(MLD_SCROLL_2, MLD_SCROLL_APPEND, aszMldGuest_More, _T("+"));
			break;
		}
        if (pGuestNum->GuestNo[i].uchCdv) {
            MldPrintf(MLD_SCROLL_2, MLD_SCROLL_APPEND, aszMldGuest_WTCD, aszTranMnem, pGuestNum->GuestNo[i].usGuestNo, pGuestNum->GuestNo[i].uchCdv);
        } else {
            MldPrintf(MLD_SCROLL_2, MLD_SCROLL_APPEND, aszMldGuest_WOCD, aszTranMnem, pGuestNum->GuestNo[i].usGuestNo);
        }
    }

    return (MLD_SUCCESS);
}

SHORT MldDispTableNum(MLDGUESTNUM *pGuestNum)
{
	CONST TCHAR  aszMldGuestTable_WOCD[] = _T("%-8.8s %d\n   %5.5s %d\n");
	CONST TCHAR  aszMldGuestTable_WTCD[] = _T("%-6s%d%02d\n   %5.5s %d\n");
	TCHAR  aszTranMnemTbl[PARA_TRANSMNEMO_LEN + 1] = { 0 }; /* mnemonic for "Table"  */
    TCHAR  aszTranMnemGc[PARA_TRANSMNEMO_LEN + 1] = { 0 }; /* mnemonic for "Guest Check #"  */

	RflGetSpecMnem ( aszTranMnemTbl, SPC_TBL_ADR );
	RflGetTranMnem(aszTranMnemGc, TRN_GCNO_ADR);

    for (SHORT i = 0; i < MLD_GUESTNO_NUM ; i++) {
        if (pGuestNum->GuestNo[i].usGuestNo == 0) break;
		if (pGuestNum->GuestNo[i].usGuestNo == (USHORT)-1){
			MldPrintf(MLD_SCROLL_2, MLD_SCROLL_APPEND, aszMldGuest_More, _T("+"));
			break;
		}
		if (pGuestNum->GuestNo[i].usTableNo) {
			if (pGuestNum->GuestNo[i].uchCdv) {
				MldPrintf(MLD_SCROLL_2, MLD_SCROLL_APPEND, aszMldGuestTable_WTCD, aszTranMnemGc, pGuestNum->GuestNo[i].usGuestNo, pGuestNum->GuestNo[i].uchCdv, aszTranMnemTbl, pGuestNum->GuestNo[i].usTableNo);
			} else {
				MldPrintf(MLD_SCROLL_2, MLD_SCROLL_APPEND, aszMldGuestTable_WOCD, aszTranMnemGc, pGuestNum->GuestNo[i].usGuestNo, aszTranMnemTbl, pGuestNum->GuestNo[i].usTableNo);
			}
		} else {
			if (pGuestNum->GuestNo[i].uchCdv) {
				MldPrintf(MLD_SCROLL_2, MLD_SCROLL_APPEND, aszMldGuest_WTCD, aszTranMnemGc, pGuestNum->GuestNo[i].usGuestNo, pGuestNum->GuestNo[i].uchCdv);
			} else {
				MldPrintf(MLD_SCROLL_2, MLD_SCROLL_APPEND, aszMldGuest_WOCD, aszTranMnemGc, pGuestNum->GuestNo[i].usGuestNo);
			}
		}
    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : USHORT   MldDispSubTotal(USHORT usScroll, LONG lTotal);
*               
*    Input : USHORT     usScroll    -Destination Scroll Display
*            LONG       lTotal      -Total Amount
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function display total amount
*===========================================================================
*/
SHORT MldDispSubTotal(USHORT usScroll, DCURRENCY lTotal)
{
	CONST TCHAR   aszMldSubTtl[] = _T("%-8.8s\n\t%11l$"); /* subtotal */ /*8 characters JHHJ*/
	CONST TCHAR   aszMldSubTtl2[] = _T("\t%11l$"); /* subtotal */
	CONST TCHAR   aszMldSubTtl3[] = _T("\t%10l$"); /* subtotal */

    TCHAR   pszWork[MLD_DRV3_DSP1_CLM_LEN +1] = {0};

    if ((uchMldSystem == MLD_PRECHECK_SYSTEM) || (uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL)) {
        if (usScroll & MLD_TOTAL_1) {   /* display only current transaction */
			TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = {0};

            /* -- get transaction mnemonics -- */
			RflGetTranMnem(aszTranMnem, TRN_AMTTL_ADR);
            MldPrintf(MLD_SCROLL_2, MLD_SCROLL_TOP+17, aszMldSubTtl, aszTranMnem, lTotal); /* RFC, 01/30/2001 */
            /***
            MldPrintf(MLD_SCROLL_2, MLD_SCROLL_TOP+19, 
                        aszMldSubTtl, aszTranMnem, lTotal); ***/
        }
    } else {    /* drive through system */
        if (usScroll & MLD_TOTAL_1) {
			UCHAR   uchAttr = (UCHAR)(MldOtherHandle1.usTotalAttrib ? MLD_C_WHITE : MLD_C_BLACK);
            if (lTotal < -1000000000L) {
                VosPrintfAttr(MldOtherHandle1.usTotalHandle, uchAttr, aszMldSubTtl2, lTotal);
                MldScroll1Buff.uchStatus |= MLD_MINUS_10DIGITS;
            } else {
                MldSPrintf(pszWork, MLD_DRV3_DSP1_CLM_LEN-1, aszMldSubTtl3, lTotal);
                VosSetCurPos(MldOtherHandle1.usTotalHandle, 0, 1);
                VosStringAttr(MldOtherHandle1.usTotalHandle, pszWork, MLD_DRV3_DSP1_CLM_LEN-1, uchAttr);
                MldScroll1Buff.uchStatus &= ~MLD_MINUS_10DIGITS;
            }
        }
        if (usScroll & MLD_TOTAL_2) {
			UCHAR   uchAttr = (UCHAR)(MldOtherHandle2.usTotalAttrib ? MLD_C_WHITE : MLD_C_BLACK);
            if (lTotal < -1000000000L) {
                VosPrintfAttr(MldOtherHandle2.usTotalHandle, uchAttr, aszMldSubTtl2, lTotal);
                MldScroll2Buff.uchStatus |= MLD_MINUS_10DIGITS;
            } else {
                MldSPrintf(pszWork, MLD_DRV3_DSP1_CLM_LEN-1, aszMldSubTtl3, lTotal);
                VosSetCurPos(MldOtherHandle2.usTotalHandle, 0, 1);
                VosStringAttr(MldOtherHandle2.usTotalHandle, pszWork, MLD_DRV3_DSP1_CLM_LEN-1, uchAttr);
                MldScroll2Buff.uchStatus &= ~MLD_MINUS_10DIGITS;
            }
        }
        if (usScroll & MLD_TOTAL_3) {
			UCHAR   uchAttr = (UCHAR)(MldOtherHandle3.usTotalAttrib ? MLD_C_WHITE : MLD_C_BLACK);
            if (lTotal < -1000000000L) {
                VosPrintfAttr(MldOtherHandle3.usTotalHandle, uchAttr, aszMldSubTtl2, lTotal);
                MldScroll3Buff.uchStatus |= MLD_MINUS_10DIGITS;
            } else {
                MldSPrintf(pszWork, MLD_DRV3_DSP1_CLM_LEN-1, aszMldSubTtl3, lTotal);
                VosSetCurPos(MldOtherHandle3.usTotalHandle, 0, 1);
                VosStringAttr(MldOtherHandle3.usTotalHandle, pszWork, MLD_DRV3_DSP1_CLM_LEN-1, uchAttr);
                MldScroll3Buff.uchStatus &= ~MLD_MINUS_10DIGITS;
            }
        }
    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : USHORT   MldMoveCursor(USHORT usType);
*               
*    Input : USHORT     usType      -type of cursor movement
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function moves cursor from current scroll to next scroll.
*===========================================================================
*/
SHORT MldMoveCursor(USHORT usType)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (uchMldSystem != MLD_DRIVE_THROUGH_3SCROLL) {

        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return (MLD_SUCCESS);
    }

    /* turn off cursor of current scroll */
    switch (uchMldCurScroll) {
    case MLD_SCROLL_1:
        pMldCurTrnScrol = &MldScroll1Buff;
        break;

    case MLD_SCROLL_2:
        pMldCurTrnScrol = &MldScroll2Buff;
        break;

    case MLD_SCROLL_3:
        pMldCurTrnScrol = &MldScroll3Buff;
        break;

    default:
        /* relese semaphore */
        PifReleaseSem(husMldSemaphore);
        return(MLD_ERROR);
    }

    VosSetCurType(pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);
    VosSetWinType(pMldCurTrnScrol->usWinHandle, VOS_WIN_NORMAL);

    /* turn on cursor of next scroll */
    if (usType == 0) {      /* move to right */
        switch (uchMldCurScroll) {
        case MLD_SCROLL_1:
            uchMldCurScroll = MLD_SCROLL_2;
            break;

        case MLD_SCROLL_2:
            uchMldCurScroll = MLD_SCROLL_3;
            break;

        case MLD_SCROLL_3:
        default:
            uchMldCurScroll = MLD_SCROLL_1;
            break;
        }
    } else {            /* move to left */
        switch (uchMldCurScroll) {
        case MLD_SCROLL_1:
            uchMldCurScroll = MLD_SCROLL_3;
            break;

        case MLD_SCROLL_3:
            uchMldCurScroll = MLD_SCROLL_2;
            break;

        case MLD_SCROLL_2:
        default:
            uchMldCurScroll = MLD_SCROLL_1;
            break;
        }
    }

    switch (uchMldCurScroll) {
    case MLD_SCROLL_1:
        pMldCurTrnScrol = &MldScroll1Buff;
        break;

    case MLD_SCROLL_2:
        pMldCurTrnScrol = &MldScroll2Buff;
        break;

    case MLD_SCROLL_3:
        pMldCurTrnScrol = &MldScroll3Buff;
        break;
    }

    VosSetWinType(pMldCurTrnScrol->usWinHandle, VOS_WIN_OVERLAP);
    VosSetCurType(pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);
    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : USHORT   MldSetCursor(USHORT usScroll);
*               
*    Input : USHORT     usScroll      -destination scroll display
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function moves cursor to directed scroll.
*===========================================================================
*/
SHORT MldSetCursor(USHORT usScroll)
{
    SHORT sReturn;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    sReturn = MldLocalSetCursor(usScroll);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);
    return (sReturn);

}

SHORT MldLocalSetCursor(USHORT usScroll)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    SHORT   sFileHandle;
    SHORT   sIndexHandle;

    if (uchMldSystem != MLD_DRIVE_THROUGH_3SCROLL) {
        return (MLD_ERROR);
    }

    /* turn off cursor of current scroll */
    switch (uchMldCurScroll) {
    case MLD_SCROLL_1:
        pMldCurTrnScrol = &MldScroll1Buff;
        break;

    case MLD_SCROLL_2:
        pMldCurTrnScrol = &MldScroll2Buff;
        break;

    case MLD_SCROLL_3:
        pMldCurTrnScrol = &MldScroll3Buff;
        break;

    default:
        return(MLD_SUCCESS);
    }

    VosSetCurType(pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);
    VosSetWinType(pMldCurTrnScrol->usWinHandle, VOS_WIN_NORMAL);

    /* turn on cursor of next scroll */

    uchMldCurScroll = (UCHAR)usScroll;

    switch (uchMldCurScroll) {
    case MLD_SCROLL_1:
        pMldCurTrnScrol = &MldScroll1Buff;
        break;

    case MLD_SCROLL_2:
        pMldCurTrnScrol = &MldScroll2Buff;
        break;

    case MLD_SCROLL_3:
        pMldCurTrnScrol = &MldScroll3Buff;
        break;
    }

    if (uchMldSystem2 == MLD_SPLIT_CHECK_SYSTEM) {

        if (uchMldCurScroll & (MLD_SCROLL_2|MLD_SCROLL_3)) {

            /* consolidate display data for scroll control */

            if (pMldCurTrnScrol->uchStatus & MLD_SCROLL_UP) {

                sFileHandle = pMldCurTrnScrol->sFileHandle;     /* save for initialize */
                sIndexHandle = pMldCurTrnScrol->sIndexHandle;

                MldLocalDisplayInit((USHORT)uchMldCurScroll,
                                    pMldCurTrnScrol->usAttrib);

                pMldCurTrnScrol->sFileHandle = sFileHandle;
                pMldCurTrnScrol->sIndexHandle = sIndexHandle;

                /* display from end of storage */
                MldDispAllReverseData((USHORT)uchMldCurScroll);
            }
        }
    }

    VosSetWinType(pMldCurTrnScrol->usWinHandle, VOS_WIN_OVERLAP);
    VosSetCurType(pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT  MldDispCursor(VOID);
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : SHORT  
*
** Synopsis: This function displays cursor on scroll, call from display module
*===========================================================================
*/
SHORT MldDispCursor(VOID)
{
    USHORT usHandle;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (uchMldCurScroll) {

        switch (uchMldCurScroll) {
        case MLD_SCROLL_1:
            usHandle = MldScroll1Buff.usWinHandle;
            break;

        case MLD_SCROLL_2:
            usHandle = MldScroll2Buff.usWinHandle;
            break;

        case MLD_SCROLL_3:
            usHandle = MldScroll3Buff.usWinHandle;
            break;

        default:
            /* release semaphore */
            PifReleaseSem(husMldSemaphore);
            return(MLD_SUCCESS);
        }

        VosSetCurType(usHandle, VOS_VISIBLE);

    }

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);
    return (MLD_SUCCESS);
    
}

/*
*===========================================================================
** Format  : USHORT   MldDispWaitKey(FDTWAITORDER *pData);
*               
*    Input : FDTWAITORDER           -waiting order no
*   Output : none
*    InOut : none
** Return  : USHORT  
*
** Synopsis: This function displays waiting order no
*===========================================================================
*/
SHORT MldDispWaitKey(FDTWAITORDER *pData)
{
    TCHAR  szMnemonic[PARA_TRANSMNEMO_LEN + 1];

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    if (uchMldSystem != MLD_DRIVE_THROUGH_3SCROLL) {

        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return (MLD_ERROR);
    }

    MldLocalDisplayInit(MLD_DRIVE_3, 0);

    /* --- draw waiting order number --- */

	RflGetTranMnem(szMnemonic, TRN_GCNO_ADR);

    for (USHORT i = 0; i < FDT_QUEUE_SIZE; i++) {

        if (pData->uchOrderNo[i] == 0) {
            break;
        }

        MldPrintf(MLD_SCROLL_3, MLD_SCROLL_APPEND, aszMldOrderNo,
                szMnemonic, pData->uchOrderNo[i]);

    }

    /* set waiting status to scroll #3 buffer */
    if (pData->uchOrderNo[0]) {
		RflGetTranMnem(szMnemonic, TRN_WAITING_ADR);

        VosPrintf(MldOtherHandle3.usOrderHandle, aszMldWaiting, szMnemonic);

        MldScroll3Buff.uchStatus |= MLD_WAIT_STATUS;
    } else {
        MldScroll3Buff.uchStatus &= ~MLD_WAIT_STATUS;
    }

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (MLD_SUCCESS);
}

VOID MldDispContinue(USHORT usScroll, USHORT usType)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        MldDriveDispContinue(usScroll, usType);
    } else {
        MldPrechkDispContinue(usType);
    }
}

VOID MldPrechkDispContinue(USHORT usType)
{
    if (usType == MLD_UPPER_ARROW) {  /* upper contine */
		TCHAR uchSymbol[1] = { MLD_UPPER_SYMBOL };

        VosSetCurPos(MldOtherHandle1.usOrderHandle, 0, 0);
        VosStringAttr(MldOtherHandle1.usOrderHandle, uchSymbol, 1, MLD_C_WHITE);
    } else {
		TCHAR uchSymbol[1] = { MLD_LOWER_SYMBOL };

        VosSetCurPos(MldOtherHandle1.usOrderHandle, MLD_NOR_DSP1A_ROW_LEN-1, 0);
        VosStringAttr(MldOtherHandle1.usOrderHandle, uchSymbol, 1, MLD_C_WHITE);
    }
}

VOID MldDriveDispContinue(USHORT usScroll, USHORT usType)
{
    if (usType == MLD_UPPER_ARROW) {  /* upper contine */
		TCHAR uchSymbol[1] = { MLD_UPPER_SYMBOL };

        if (usScroll & MLD_DRIVE_1) {
            /* display attow by reverse video */
			UCHAR uchAttr = (UCHAR)(MldOtherHandle1.usOrderAttrib ? MLD_C_BLACK : MLD_C_WHITE);

            VosSetCurPos(MldOtherHandle1.usOrderHandle, 0, 0);
            VosStringAttr(MldOtherHandle1.usOrderHandle, uchSymbol, 1, uchAttr);
            VosSetCurPos(MldOtherHandle1.usOrderHandle, 0, 0);
        }
        if (usScroll & MLD_DRIVE_2) {
            /* display attow by reverse video */
			UCHAR uchAttr = (UCHAR)(MldOtherHandle2.usOrderAttrib ? MLD_C_BLACK : MLD_C_WHITE);

            VosSetCurPos(MldOtherHandle2.usOrderHandle, 0, 0);
            VosStringAttr(MldOtherHandle2.usOrderHandle, uchSymbol, 1, uchAttr);
            VosSetCurPos(MldOtherHandle2.usOrderHandle, 0, 0);
        }
        if (usScroll & MLD_DRIVE_3) {
            /* display attow by reverse video */
			UCHAR uchAttr = (UCHAR)(MldOtherHandle3.usOrderAttrib ? MLD_C_BLACK : MLD_C_WHITE);

            VosSetCurPos(MldOtherHandle3.usOrderHandle, 0, 0);
            VosStringAttr(MldOtherHandle3.usOrderHandle, uchSymbol, 1, uchAttr);
            VosSetCurPos(MldOtherHandle3.usOrderHandle, 0, 0);

        }
    } else {                            /* lower continue */
		TCHAR uchSymbol[1] = { MLD_LOWER_SYMBOL };

        if (usScroll & MLD_DRIVE_1) {
            /* display attow by reverse video */
			UCHAR uchAttr = (UCHAR)(MldOtherHandle1.usOrderAttrib ? MLD_C_BLACK : MLD_C_WHITE);

            if (!(MldScroll1Buff.uchStatus & MLD_MINUS_10DIGITS)) {
                VosSetCurPos(MldOtherHandle1.usTotalHandle, 0, 0);
                VosStringAttr(MldOtherHandle1.usTotalHandle, uchSymbol, 1, uchAttr);
                VosSetCurPos(MldOtherHandle1.usTotalHandle, 0, 0);
            }
        }
        if (usScroll & MLD_DRIVE_2) {
            /* display attow by reverse video */
			UCHAR uchAttr = (UCHAR)(MldOtherHandle2.usOrderAttrib ? MLD_C_BLACK : MLD_C_WHITE);

            if (!(MldScroll2Buff.uchStatus & MLD_MINUS_10DIGITS)) {
                VosSetCurPos(MldOtherHandle2.usTotalHandle, 0, 0);
                VosStringAttr(MldOtherHandle2.usTotalHandle, uchSymbol, 1, uchAttr);
                VosSetCurPos(MldOtherHandle2.usTotalHandle, 0, 0);
            }
        }
        if (usScroll & MLD_DRIVE_3) {
            /* display attow by reverse video */
			UCHAR uchAttr = (UCHAR)(MldOtherHandle3.usOrderAttrib ? MLD_C_BLACK : MLD_C_WHITE);

            if (!(MldScroll3Buff.uchStatus & MLD_MINUS_10DIGITS)) {
                VosSetCurPos(MldOtherHandle3.usTotalHandle, 0, 0);
                VosStringAttr(MldOtherHandle3.usTotalHandle, uchSymbol, 1, uchAttr);
                VosSetCurPos(MldOtherHandle3.usTotalHandle, 0, 0);
            }
        }
    }
}

VOID MldClearContinue(USHORT usScroll, USHORT usType)
{
    if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
        MldDriveClearContinue(usScroll, usType);
    } else {
        MldPrechkClearContinue(usType);
    }
}

VOID MldPrechkClearContinue(USHORT usType)
{
	TCHAR uchSymbol[1] = { MLD_CLEAR_SYMBOL };

    if (usType == MLD_UPPER_ARROW) {  /* upper contine */
        VosSetCurPos(MldOtherHandle1.usOrderHandle, 0, 0);
        VosString(MldOtherHandle1.usOrderHandle, uchSymbol, 1);
    } else {
        VosSetCurPos(MldOtherHandle1.usOrderHandle, MLD_NOR_DSP1A_ROW_LEN-1, 0);
        VosString(MldOtherHandle1.usOrderHandle, uchSymbol, 1);
    }
}

VOID MldDriveClearContinue(USHORT usScroll, USHORT usType)
{
	TCHAR uchSymbol[1] = { MLD_CLEAR_SYMBOL };

    if (usType == MLD_UPPER_ARROW) {  /* upper contine */
        if (usScroll & MLD_DRIVE_1) {
			UCHAR uchAttr = (UCHAR)(MldOtherHandle1.usOrderAttrib ? MLD_C_WHITE : MLD_C_BLACK);

            VosSetCurPos(MldOtherHandle1.usOrderHandle, 0, 0);
            VosStringAttr(MldOtherHandle1.usOrderHandle, uchSymbol, 1, uchAttr);
            VosSetCurPos(MldOtherHandle1.usOrderHandle, 0, 0);
        }
        if (usScroll & MLD_DRIVE_2) {
			UCHAR uchAttr = (UCHAR)(MldOtherHandle2.usOrderAttrib ? MLD_C_WHITE : MLD_C_BLACK);

            VosSetCurPos(MldOtherHandle2.usOrderHandle, 0, 0);
            VosStringAttr(MldOtherHandle2.usOrderHandle, uchSymbol, 1, uchAttr);
            VosSetCurPos(MldOtherHandle2.usOrderHandle, 0, 0);
        }
        if (usScroll & MLD_DRIVE_3) {
			UCHAR uchAttr = (UCHAR)(MldOtherHandle3.usOrderAttrib ? MLD_C_WHITE : MLD_C_BLACK);

            VosSetCurPos(MldOtherHandle3.usOrderHandle, 0, 0);
            VosStringAttr(MldOtherHandle3.usOrderHandle, uchSymbol, 1, uchAttr);
            VosSetCurPos(MldOtherHandle3.usOrderHandle, 0, 0);
        }
    } else {                            /* lower continue */
        if (usScroll & MLD_DRIVE_1) {
			UCHAR uchAttr = (UCHAR)(MldOtherHandle1.usTotalAttrib ? MLD_C_WHITE : MLD_C_BLACK);

            if (!(MldScroll1Buff.uchStatus & MLD_MINUS_10DIGITS)) {
                VosSetCurPos(MldOtherHandle1.usTotalHandle, 0, 0);
                VosStringAttr(MldOtherHandle1.usTotalHandle, uchSymbol, 1, uchAttr);
                VosSetCurPos(MldOtherHandle1.usTotalHandle, 0, 0);
            }
        }
        if (usScroll & MLD_DRIVE_2) {
			UCHAR uchAttr = (UCHAR)(MldOtherHandle2.usTotalAttrib ? MLD_C_WHITE : MLD_C_BLACK);

            if (!(MldScroll2Buff.uchStatus & MLD_MINUS_10DIGITS)) {
                VosSetCurPos(MldOtherHandle2.usTotalHandle, 0, 0);
                VosStringAttr(MldOtherHandle2.usTotalHandle, uchSymbol, 1, uchAttr);
                VosSetCurPos(MldOtherHandle2.usTotalHandle, 0, 0);
            }
        }
        if (usScroll & MLD_DRIVE_3) {
			UCHAR uchAttr = (UCHAR)(MldOtherHandle3.usTotalAttrib ? MLD_C_WHITE : MLD_C_BLACK);

            if (!(MldScroll3Buff.uchStatus & MLD_MINUS_10DIGITS)) {
                VosSetCurPos(MldOtherHandle3.usTotalHandle, 0, 0);
                VosStringAttr(MldOtherHandle3.usTotalHandle, uchSymbol, 1, uchAttr);
                VosSetCurPos(MldOtherHandle3.usTotalHandle, 0, 0);
            }
        }
    }
}
#endif

VOID MldSetSpecialStatus(USHORT usScroll, UCHAR uchStatus)
{
    switch (usScroll) {
    case MLD_SCROLL_1:
        MldScroll1Buff.uchSpecialStatus |= uchStatus;
        break;

#if DISPTYPE==1
    case MLD_SCROLL_2:
        MldScroll2Buff.uchSpecialStatus |= uchStatus;
        break;

    case MLD_SCROLL_3:
        MldScroll3Buff.uchSpecialStatus |= uchStatus;
        break;
#endif
    }
}

VOID MldResetSpecialStatus(USHORT usScroll, UCHAR uchStatus)
{
    switch (usScroll) {
    case MLD_SCROLL_1:
        MldScroll1Buff.uchSpecialStatus &= ~uchStatus;
        break;

#if DISPTYPE==1
    case MLD_SCROLL_2:
        MldScroll2Buff.uchSpecialStatus &= ~uchStatus;
        break;

    case MLD_SCROLL_3:
        MldScroll3Buff.uchSpecialStatus &= ~uchStatus;
        break;
#endif
    }
}

SHORT MldCheckSpecialStatus(USHORT usScroll, UCHAR uchStatus)
{
    switch (usScroll) {
    case MLD_SCROLL_1:
        if (MldScroll1Buff.uchSpecialStatus & uchStatus) {
            return (1);
        }
        break;

#if DISPTYPE==1
    case MLD_SCROLL_2:
        if (MldScroll2Buff.uchSpecialStatus & uchStatus) {
            return (1);
        }
        break;

    case MLD_SCROLL_3:
        if (MldScroll3Buff.uchSpecialStatus & uchStatus) {
            return (1);
        }
#endif
        break;
    }

    return (0);
}

/*
*===========================================================================
** Format   : BOOL MldIsSalesIDisc( UCHAR *puchItem )
*
*    Input  : UCHAR *puchItem - address of item data to determine
*   Output  : none
*    InOut  : none
** Return   : TRUE  -   item is sales item with item discount
*             FALSE -   item is not sales item with item discount
*
** Synopsis : 
*       determine specified item is sales item with item discount.
*===========================================================================
*/
BOOL MldIsSalesIDisc( VOID *puchItem )
{
#if 1
	return RflIsSalesItemDisc (puchItem);
#else
    if ( puchItem[ 0 ] == CLASS_ITEMSALES ) {

        switch ( puchItem[ 1 ] ) {

        case CLASS_DEPTITEMDISC:
        case CLASS_PLUITEMDISC:
        case CLASS_SETITEMDISC:
        case CLASS_OEPITEMDISC:
            return ( TRUE );
            break;

        default:
            break;
        }
    }
    return ( FALSE );
#endif
}

/*
*===========================================================================
** Format   : VOID MldSetItemSalesQtyPrice(ITEMSALES *pItem, PRTIDX *pPrtIdx )
*
*    Input  : ITEMSALES *pItem
*           : PRTIDX    *pPrtIdx
*   Output  : none
*    InOut  : none
**  Return  : none
*
** Synopsis : 
*       Set qty and price of storage data from index data
*===========================================================================
*/
VOID MldSetItemSalesQtyPrice(ITEMSALES *pItem, VOID *pData)
{
    PRTIDX *pPrtIdx = pData;

	// update the unique ID to what ever the index contains.  We do this because the item information
	// may be from some other item with its unique ID and the selected item is a consolidation index
	// in which case the item information will have the unique ID of the root item and not the selected item index.
	pItem->usUniqueID = pPrtIdx->usUniqueID;
	pItem->usKdsLineNo = pPrtIdx->usKdsLineNo;

	pItem->lQTY     = pPrtIdx->lQTY;
    pItem->lProduct = pPrtIdx->lProduct;

    /* set coupon qty for non-void, non-consoli print, V3.3 */
/*
    if (pItem->ControlCode.auchStatus[2] & PLU_SCALABLE) {
        pItem->sCouponQTY = 1;
    } else {
        pItem->sCouponQTY = pPrtIdx->sNetQTY;
    }
*/
}


/*
*===========================================================================
** Format  : SHORT  MldSetMoveCursor(USHORT usScroll);
*               
*    Input : USHORT usScroll - which scroll window
*   Output : SHORT
*    InOut : none
** Return  :	TRUE
				FALSE  
*
** Synopsis: This function sets the cursor status to moved.
*===========================================================================
*/
SHORT MldSetMoveCursor(USHORT usScroll)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;

    PifRequestSem(husMldSemaphore);

	//if not a 3 scroll setup and not the main window num
	//return FALSE
    if (uchMldSystem != MLD_DRIVE_THROUGH_3SCROLL) {

        if (usScroll != MLD_SCROLL_1) {
            PifReleaseSem(husMldSemaphore);
            return(FALSE);
        }

    }

    /* turn off cursor of current scroll */
    switch (usScroll) {
    case MLD_SCROLL_1:
        pMldCurTrnScrol = &MldScroll1Buff;
        break;

    case MLD_SCROLL_2:
        pMldCurTrnScrol = &MldScroll2Buff;
        break;

    case MLD_SCROLL_3:
        pMldCurTrnScrol = &MldScroll3Buff;
        break;

    default:
		//doesn't match scroll windows
        /* relese semaphore */
        PifReleaseSem(husMldSemaphore);
        return(FALSE);
    }

    pMldCurTrnScrol->uchStatus2 |= MLD_MOVE_CURSOR;//set cursor to moved

    PifReleaseSem(husMldSemaphore);
    
	return(TRUE);
}
/*
*===========================================================================
** Format  : USHORT  MldGetVosWinHandle(USHORT usScroll);
*               
*    Input : USHORT usScroll - which scroll window
*   Output : USHORT
*    InOut : none
** Return  :	USHORT - VOS Window Handle
*
** Synopsis: This function gets the VOS Window Handle.
*===========================================================================
*/
USHORT MldGetVosWinHandle(USHORT usScroll)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;

    PifRequestSem(husMldSemaphore);
	//if not a 3 scroll setup and not the main window num
	//return FALSE
    if (uchMldSystem != MLD_DRIVE_THROUGH_3SCROLL) {

        if (usScroll != MLD_SCROLL_1) {
            PifReleaseSem(husMldSemaphore);
            return(FALSE);
        }

    }

    /* turn off cursor of current scroll */
    switch (usScroll) {
    case MLD_SCROLL_1:
        pMldCurTrnScrol = &MldScroll1Buff;
        break;

    case MLD_SCROLL_2:
        pMldCurTrnScrol = &MldScroll2Buff;
        break;

    case MLD_SCROLL_3:
        pMldCurTrnScrol = &MldScroll3Buff;
        break;

    default:
		//doesn't match scroll windows
        /* relese semaphore */
        PifReleaseSem(husMldSemaphore);
        return(FALSE);
    }

    PifReleaseSem(husMldSemaphore);

	return pMldCurTrnScrol->usWinHandle;

}

/*
*===========================================================================
** Format  : SHORT  MldQueryMoveScroll(VOID);
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : SHORT  
*
** Synopsis: This function returns current active window.
*===========================================================================
*/
SHORT MldQueryMoveCursor(USHORT usScroll)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;

    PifRequestSem(husMldSemaphore);

    if (uchMldSystem != MLD_DRIVE_THROUGH_3SCROLL) {

        if (usScroll != MLD_SCROLL_1) {
            PifReleaseSem(husMldSemaphore);
            return(FALSE);
        }

    }

    /* turn off cursor of current scroll */
    switch (usScroll) {
    case MLD_SCROLL_1:
        pMldCurTrnScrol = &MldScroll1Buff;
        break;

#if DISPTYPE==1
    case MLD_SCROLL_2:
        pMldCurTrnScrol = &MldScroll2Buff;
        break;

    case MLD_SCROLL_3:
        pMldCurTrnScrol = &MldScroll3Buff;
        break;

#endif
    default:
        /* relese semaphore */
        PifReleaseSem(husMldSemaphore);
        return(FALSE);
    }

    if (pMldCurTrnScrol->uchStatus2 & MLD_MOVE_CURSOR) {

        PifReleaseSem(husMldSemaphore);
        return(TRUE);

    } else {

        PifReleaseSem(husMldSemaphore);
        return(FALSE);
    }
}

/*
*===========================================================================
** Format  : SHORT  MldQueryCurScroll(VOID);
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : SHORT  
*
** Synopsis: This function returns current active window.
*===========================================================================
*/
SHORT MldQueryCurScroll(VOID)
{
#if DISPTYPE==1
    return((SHORT)uchMldCurScroll);
#else
    return(MLD_SCROLL_1);
#endif
}

/*
*===========================================================================
** Format  : SHORT MldChkDispPluNo(ITEMSALES *pItem);
*
*   Input  : ITEMSALES        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : TRUE   -enable display
*            FALSE  -not display
*
** Synopsis: 
*===========================================================================
*/
SHORT MldChkDisplayPluNo(CONST ITEMSALES *pItem)
{

    if (pItem->uchMajorClass != CLASS_ITEMSALES) {
        return(TRUE);
    }

    /*---- not display after the split check tender ----*/
    if (pItem->lQTY == 0L) {

        return(FALSE);
    }

    if (pItem->fbReduceStatus & REDUCE_ITEM) {  /* transfered item */

        return(FALSE);
    }

    if ((pItem->uchMinorClass == CLASS_DEPTMODDISC) ||
        (pItem->uchMinorClass == CLASS_PLUMODDISC) ||
        (pItem->uchMinorClass == CLASS_SETMODDISC) ||
        (pItem->uchMinorClass == CLASS_OEPMODDISC)) {

        return(TRUE);
    }

    /*---- not display if void consolidation ----*/
#if DISPTYPE==1
    if ((uchMldSystem2 == MLD_SPLIT_CHECK_SYSTEM) ||
        (CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT2))) {
#else
    if (CliParaMDCCheck(MDC_DEPT3_ADR, ODD_MDC_BIT2)) {
#endif
        if (pItem->usItemOffset) {
            if (pItem->sCouponQTY == 0) {
                if ((pItem->uchPPICode == 0) &&
                    (pItem->uchPM == 0)) {  /* saratoga */

                    return(FALSE);
                }
            }
        }
    }

    /*---- not display if plu no is over 9950 ----*/
    if ((_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) >= 0) &&
        (_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) <= 0)) {
    /* if (pItem->usPLUNo >= MLD_NOT_DISPLAY_PLU_NO) { */

        if ((pItem->uchPrintModNo + pItem->uchCondNo ) == 0) {
            return (FALSE);
        }
/****
        for (i = 0,  uchCondNo = pItem->uchCondNo; i < STD_MAX_COND_NUM; i++) {
            if (pItem->Condiment[i].usPLUNo >= MLD_NOT_DISPLAY_PLU_NO) {
                uchCondNo--;
            }
        }
        if ((pItem->uchPrintModNo + uchCondNo ) == 0) {
            return (FALSE);
        }
****/
    }

    return(TRUE);
}

/*
*===========================================================================
** Format  : VOID MldSetSalesCouponQTY(ITEMSALES *pItem, SHORT sFileHandle);
*
*   Input  : SHORT            sFileHandle -Storage File handle
*   Output : none
*   InOut  : ITEMSALES        *pItem     -Item Data address
** Return  : nothing
*
** Synopsis: 
*===========================================================================
*/
VOID MldSetSalesCouponQTY(ITEMSALES *pItem, SHORT sFileHandle, USHORT usScroll)
{
    if (pItem->ControlCode.auchPluStatus[2] & PLU_SCALABLE) {

        pItem->sCouponQTY = 1;      /* always display */

    } else {
		TRANCURQUAL   TranCurQual;
		CHAR          achBuff[sizeof(ITEMDATASIZEUNION)] = {0};
		CHAR          achBuffDisc[sizeof(ITEMSALES) + 10] = {0};    // decompression area needs to be size of ITEMSALES as a minimum
	    
		TrnGetCurQual(&TranCurQual);

        if (pItem->ControlCode.auchPluStatus[0] & PLU_MINUS) {
            pItem->sCouponQTY = (SHORT)(pItem->lQTY/1000L);
        }

        if (pItem->usItemOffset) {
			ITEMSALES       *pItemVoidSales = (ITEMSALES *)achBuff;
			ITEMDISC        *pItemVoidDisc = (ITEMDISC *)achBuffDisc;
			ULONG			ulActualBytesRead;
			USHORT			usReadLen;
			UCHAR			auchVoidWork[ sizeof(ITEMSALES) + sizeof(ITEMDISC) + 10];

			//11-1103- SR 201 JHHJ
            TrnReadFile( pItem->usItemOffset, auchVoidWork, sizeof( auchVoidWork ), sFileHandle, &ulActualBytesRead);

            usReadLen = RflGetStorageItem(achBuff, auchVoidWork, RFL_WITHOUT_MNEM );

			if ( MldIsSalesIDisc (achBuff)) {
				/* decompress itemize discount data */
				RflGetStorageItem(achBuffDisc, auchVoidWork + usReadLen, RFL_WITHOUT_MNEM);
			}

            /* minus plu void consolidation */
            if (pItemVoidSales->ControlCode.auchPluStatus[0] & PLU_MINUS) {

                pItemVoidSales->sCouponQTY = (SHORT)(pItemVoidSales->lQTY/1000L);
                pItem->sCouponQTY += pItemVoidSales->sCouponQTY;

                if ((MldCheckSpecialStatus(usScroll, MLD_TVOID_STATUS)) ||
                    ((usScroll == MLD_SCROLL_1) && (TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK)) ) {

                    if (pItem->sCouponQTY >= 0) {
                        pItemVoidSales->sCouponQTY = pItem->sCouponQTY;
                        pItem->sCouponQTY = 0;
                    } else {
                        pItemVoidSales->sCouponQTY = 0;
                    }
                } else {
                    if (pItem->sCouponQTY <= 0) {
                        pItemVoidSales->sCouponQTY = pItem->sCouponQTY;
                        pItem->sCouponQTY = 0;
                    } else {
                        pItemVoidSales->sCouponQTY = 0;
                    }
                }
            }

            if ((MldCheckSpecialStatus(usScroll, MLD_TVOID_STATUS)) ||
                ((usScroll == MLD_SCROLL_1) && (TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK)) ) {
                /* transaction void or transaction return */

                if (pItem->fbModifier & VOID_MODIFIER) {
                    if ((pItemVoidSales->lQTY + pItem->lQTY) < 0L) {
                        /* clear sales qty */
                        pItem->sCouponQTY = 0;
                    } else {
                        pItem->sCouponQTY = pItemVoidSales->sCouponQTY;
                    }
                } else {
                    if ((pItemVoidSales->lQTY + pItem->lQTY) >= 0L) {
                        /* clear sales qty */
                        pItem->sCouponQTY = 0;
                    }
                }
            } else {

                if (pItem->fbModifier & VOID_MODIFIER) {
                    if ((pItemVoidSales->lQTY + pItem->lQTY) >= 0L) {
                        /* clear sales qty */
                        pItem->sCouponQTY = 0;
                    } else {
                        pItem->sCouponQTY = pItemVoidSales->sCouponQTY;
                    }
                } else {
                    if ((pItemVoidSales->lQTY + pItem->lQTY) < 0L) {
                        /* clear sales qty */
// Remove this assignment as it causes problems with the display of item
// with a quantity.  After doing an Item Discount on an item that had several
// duplicates and then doing a couple of voids, we would not see the correct
// number of items displayed.  Richard Chambers, Mar-02-2011  VCS, Rel 2.2.0
//                        pItem->sCouponQTY = 0;
                    }
                }
            }
        }
    }
}

/*
*===========================================================================
** Format   : VOID MldSetItemSalesQtyPrice(ITEMSALES *pItem, PRTIDX *pPrtIdx )
*
*    Input  : ITEMSALES *pItem
*           : PRTIDX    *pPrtIdx
*   Output  : none
*    InOut  : none
**  Return  : none
*
** Synopsis : 
*       Set qty and price of storage data from coupon qty
*===========================================================================
*/
VOID MldResetItemSalesQtyPrice(ITEMSALES *pItem)
{
    if (MldChkVoidConsolidation (pItem) ) {
		/* void consolidation by coupon qty */
		pItem->lQTY     = ((LONG)pItem->sCouponQTY * 1000L);
		pItem->lProduct = ((DCURRENCY)pItem->lUnitPrice * (LONG)pItem->sCouponQTY);
    }
}

/***** End Of Source *****/
