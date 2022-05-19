/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Retry Buffer Module
* Category    : User Interface Enigne, NCR 2170 High Level System Application
* Program Name: UIERETR.C
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
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
*
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
#include <stdlib.h>

#include "ecr.h"
#include "rfl.h"
#include "log.h"
#include "pif.h"
#include "uie.h"
#include "uiel.h"
#include "uieseq.h"


/*
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/
UIERETRYSTS UieRetry;                       /* Retry Status */
                                            /* Bit 0 - Accept Flag */
                                            /* Bit 1 - Retry Flag */
                                            /* Bit 2 - Retry Buffer Full Flag */
											/* Bit 3 - Clear Retry Buffer Flag */


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID UieGetRetryBuf(KEYMSG *pData)
*     InOut:    pData - Message Save Pointer
*
** Return:      nothing
*
** Description: This function takes out one message in the retry buffer.
*===========================================================================
*/
VOID UieGetRetryBuf(KEYMSG *pData)
{
    TCHAR *puchAddr;
    TCHAR *puchStr = pData->SEL.INPUT.aszKey;

    _tcsncpy(puchStr, &UieRetryBuf[UieRetry.iuchRead], UIE_MAX_KEY_BUF);      /* Copy to User Buffer form Retry Buffer */
	puchStr[UIE_MAX_KEY_BUF - 1] = 0;                                         // ensure string is zero terminated.

    pData->uchMsg            = UIM_INPUT;                       /* Set to Input Message */
    pData->SEL.INPUT.uchLen  = (UCHAR)_tcslen(puchStr);          /* Set to Key String Length */

    if ((puchAddr = (TCHAR *)_tcschr(puchStr, _T('.'))) != NULL) {
        pData->SEL.INPUT.uchDec  = pData->SEL.INPUT.uchLen -    /* Set to Decimal Point Length */
                                   (UCHAR)(puchAddr - puchStr) -
                                   (UCHAR)1;

        pData->SEL.INPUT.lData = 0;
        while (*puchStr) {
            if (_T('0') <= *puchStr && *puchStr <= _T('9')) {           /* Numeric Key */
                pData->SEL.INPUT.lData = pData->SEL.INPUT.lData * 10 + *puchStr - 0x30/*'0'*/;
            }
            puchStr++;
        }
    } else {                                                    /* not Find for Decimal Point */
        pData->SEL.INPUT.uchDec = UIE_DEC_INIT;
        pData->SEL.INPUT.lData  = _ttol(puchStr);                /* Set to Input Data */
    }

    UieRetry.iuchRead += pData->SEL.INPUT.uchLen + 1;
	NHPOS_ASSERT(UieRetryBuf[UieRetry.iuchRead] <= 0xFF);//MAXBYTE
    pData->SEL.INPUT.uchMajor = (UCHAR)UieRetryBuf[UieRetry.iuchRead++];   /* Set to Major FSC */
	NHPOS_ASSERT(UieRetryBuf[UieRetry.iuchRead] <= 0xFF);//MAXBYTE
    pData->SEL.INPUT.uchMinor = (UCHAR)UieRetryBuf[UieRetry.iuchRead++];   /* Set to Minor FSC */
                                        
    if (UieRetry.iuchRead == UieRetry.iuchWrite) {                  /* End of Retry Buffer */
        UieRetry.fchStatus &= ~UIE_RETRY;                           /* Reset to Retry Flag */

        UieSetStatusDisplay(UIE_ENABLE);                            /* Enable Display */
    }
}


/*
*===========================================================================
** Synopsis:    VOID UieWriteRetryBuf(KEYMSG *pData)
*     InOut:    pData - Message Save Pointer
*
** Return:      nothing
*
** Description: This function writes the message of the User Interface
*               Engine in the retry buffer.
*===========================================================================
*/
VOID UieWriteRetryBuf(KEYMSG *pData)
{
	USHORT usLen = tcharlen(pData->SEL.INPUT.aszKey) + 1; /* with NULL Terminate */
	
    if (UieRetry.iuchWrite + usLen + 2 <= UIE_MAX_RETRY_BUF) {
      	_tcsncpy(&UieRetryBuf[UieRetry.iuchWrite], pData->SEL.INPUT.aszKey, UIE_MAX_KEY_BUF);
        UieRetry.iuchWrite += usLen;
        UieRetryBuf[UieRetry.iuchWrite++] = pData->SEL.INPUT.uchMajor;
        UieRetryBuf[UieRetry.iuchWrite++] = pData->SEL.INPUT.uchMinor;
    } else {
        UieRetry.fchStatus |= UIE_RETRY_FULL;           /* Set to Retry Buffer Full */

#if defined(UIE_DEBUG)
        PifAbort(MODULE_UIE_RETRY, FAULT_BUFFER_OVERFLOW);
#else
        PifLog(MODULE_UIE_RETRY, FAULT_BUFFER_OVERFLOW);
#endif
	}
}


/*
*===========================================================================
** Synopsis:    VOID UieClearRetryBuf(VOID)
*
** Return:      no
*
** Description: Clear Retry Buffer  
*===========================================================================
*/
VOID UieClearRetryBuf(VOID)
{
    UieRetry.iuchWrite = 0;                         /* Clear Retry Buffer Write Index */
    UieRetry.iuchRead  = 0;                         /* Clear Retry Buffer Read Index */
    UieRetry.fchStatus |= UIE_ACCEPT;               /* Set to Accept Flag */
    UieRetry.fchStatus &= ~(UIE_RETRY |             /* Reset to Retry Flag */ 
                            UIE_RETRY_FULL |        /* Reset to Retry Buffer Full Flag */
					        UIE_CLEAR_RETRY_BUF |   /* Reset to Clear Retry Buffer Flag */
                            UIE_DEF_PROC);          /* Reset to Default Process Flag */

    UieResetKeepMessage();                          /* Reset to Keep Message Flag */
}



/* ---------- End of UIERETR.C ---------- */
