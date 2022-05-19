/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : MSR Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieMsr.C
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
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Oct-01-92  00.01.05  O.Nakada     Added PifOpenMsr(), PifCloseMsr()
*                                   function in UieMsr().
* May-25-93  C1        O.Nakada     Removed input device control functions.
* Mar-15-95  G0        O.Nakada     Modified all modules, correspond to
*                                   hospitality release 3.0 functions.
*
*
*** NCR 2171 ***
* Sep-29-99  01.00.00  M.Teraki     Initial
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
#include <memory.h>

#include "ecr.h"
#include "pif.h"
#include "fsc.h"
#include "uie.h"
#include "uiel.h"
#include "uieio.h"
#include "uiering.h"
#include "BLFWif.h"


/*
*===========================================================================
*   General Declarations
*===========================================================================
*/


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID UieMsrInit(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID UieMsrInit(VOID)
{
}


/*
*===========================================================================
** Synopsis:    LONG UiePutMsrData(ULONG ulLength,
*                                  CONST MSR_BUFFER *pData,
*                                  VOID *pResult);
*
*     Input:    ulLength - length of data from pData
*               pData    - MSR data from external module
*
*    Output:    pResult - reserved
*
** Return:      reserved
*
** Description: Accept MSR data and write to the ring buffer
*===========================================================================
*/
LONG UiePutMsrData(ULONG ulLength, CONST MSR_BUFFER *pData, VOID *pResult)
{
    FSCTBL      Ring;
    LONG	    lRet = -1;	/* return value to caller */

    /* --- if MSR is disabled, ignore data --- */

    if (!(fchUieDevice & UIE_ENA_MSR)) {
		return lRet;
    }

    /* --- write data into ring buffer --- */

    Ring.uchMajor = FSC_MSR;
    Ring.uchMinor = 0;

    UieWriteRingBuf(Ring, pData, sizeof(MSR_BUFFER));

    return lRet;
}

LONG  UiePutVirtualKeyData(ULONG ulLength, CONST VIRTUALKEY_BUFFER *pData, VOID *pResult)
{
    FSCTBL      Ring;
    LONG	lRet = -1;	/* return value to caller */

    /* --- if Virtual keyboard input is disabled, ignore data --- */
    if (!(fchUieDevice & UIE_ENA_VIRTUALKEY)) {
		return lRet;
    }

    /* --- write data into ring buffer --- */
    Ring.uchMajor = FSC_VIRTUALKEYEVENT;
    Ring.uchMinor = 0;

    UieWriteRingBuf(Ring, pData, sizeof(VIRTUALKEY_BUFFER));

    return lRet;
}

LONG UiePutPinPadData(ULONG ulLength, CONST PINPAD_BUFFER *pData, VOID *pResult)
{
    FSCTBL      Ring;
    LONG	    lRet = -1;	/* return value to caller */

    /* --- if MSR is disabled, ignore data --- */
    if (!(fchUieDevice & UIE_ENA_MSR)) {
		return lRet;
    }

    /* --- write data into ring buffer --- */
    Ring.uchMajor = FSC_PINPAD;
    Ring.uchMinor = 0;

    UieWriteRingBuf(Ring, pData, sizeof(PINPAD_BUFFER));

    return lRet;
}

LONG  UiePutSignatureCaptureData(ULONG ulLength, CONST UIE_SIGCAP *pData, VOID *pResult)
{
    FSCTBL      Ring;
    LONG	    lRet = -1;	/* return value to caller */

    /* --- if MSR is disabled, ignore data --- */
    if (!(fchUieDevice & UIE_ENA_MSR)) {
		return lRet;
    }

    /* --- write data into ring buffer --- */
    Ring.uchMajor = FSC_SIGNATURECAPTURE;
    Ring.uchMinor = 0;

    UieWriteRingBuf(Ring, pData, sizeof(UIE_SIGCAP));

    return lRet;
}

LONG  UiePutOperatorActionData(ULONG ulLength, CONST UIE_OPERATORACTION *pData, VOID *pResult)
{
    FSCTBL      Ring;
    LONG	    lRet = -1;	/* return value to caller */

    /* --- write data into ring buffer --- */
    Ring.uchMajor = FSC_OPERATOR_ACTION;
    Ring.uchMinor = 0;

    UieWriteRingBuf(Ring, pData, sizeof(UIE_OPERATORACTION));

    return lRet;
}
/*
*===========================================================================
** Synopsis:    SHORT DLLENTRY UieReadMsr(UIE_MSR DLLARG *pData);
*    Output:    pData - address of MSR data
*
** Return:      0 >= successful
*               0 <  failer
*===========================================================================
*/
SHORT  UieReadMsr(UIE_MSR DLLARG *pData)
{
	SHORT	   sResult;
	BLFMSRIF   MsrTrackData;

	sResult = BlFwIfReadMSR(&MsrTrackData);

    pData->uchLength1 = MsrTrackData.uchLength1;                                        /* ISO track-1 length      */
    memcpy (pData->auchTrack1, MsrTrackData.auchTrack1, sizeof(pData->auchTrack1));     /* ISO track-1 data        */
    pData->uchLength2 = MsrTrackData.uchLength2;                                        /* ISO track-2 length      */
    memcpy (pData->auchTrack2, MsrTrackData.auchTrack2, sizeof(pData->auchTrack2));     /* ISO track-2 data        */
    pData->uchLengthJ = MsrTrackData.uchLengthJ;                                        /* JIS track-II length     */
    memcpy (pData->auchTrackJ, MsrTrackData.auchTrackJ, sizeof(pData->auchTrackJ));     /* JIS track-II data       */

	// Zero out the track data per PABP guidelines for card track security
	memset (&MsrTrackData, 0, sizeof(MsrTrackData));
	return sResult;
}


/* ===================================== */
/* ========== End of UieMsr.C ========== */
/* ===================================== */
