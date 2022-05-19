/*************************************************************************
 *
 * UIESCALE.cpp
 *
 * $Workfile:: UIESCALE.cpp                                              $
 *
 * $Revision::                                                           $
 *
 * $Archive::                                                            $
 *
 * $Date::                                                               $
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 2000.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: UIEMSR.cpp                                                 $
 *
 ************************************************************************/

#include "stdafx.h"
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "UIEMSR.h"
#include "Device.h"
#include "DeviceIO.H"


/*
*===========================================================================
*   General Declarations
*===========================================================================
*/

#define UIE_MSR_READ            500         /* read timer of MSR before declaring time out   */

#define	STATIC	static

STATIC  PIF_DEVICE_IF   UieDevice;

STATIC	USHORT			s_usPort;
STATIC	USHORT			s_usBaud;
STATIC	UCHAR			s_uchFormat;
STATIC	HANDLE			s_hScale;


/*
*===========================================================================
*   Public Functions
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    BOOL UieMSRInit(HANDLE);
*
** Return:      nothing
*===========================================================================
*/
BOOL UieMSRInit(USHORT usPort, USHORT usBaud, UCHAR uchFormat, HANDLE hScale)
{
    SHORT       sResult;
	USHORT      usTimer;
    PROTOCOL    Protocol;

	// save scale handle
	s_usPort    = usPort;
	s_usBaud    = usBaud;
	s_uchFormat = uchFormat;
	s_hScale    = hScale;

    /* --- make protocol data --- */
    Protocol.fPip             = PIF_COM_PROTOCOL_NON;
    Protocol.usComBaud        = usBaud;
    Protocol.uchComByteFormat = uchFormat;
    Protocol.uchComTextFormat = PIF_COM_FORMAT_TEXT;

    /* --- open serial port --- */
    sResult = PifOpenCom(usPort, (PROTOCOL *)&Protocol);
    if (sResult < 0) {                  /* open error        */
        /* --- error logging --- */
        PifLog(MODULE_UIE_MSR, LOG_ERROR_DEVICE_OPENCOM);
        PifLog(MODULE_ERROR_NO(MODULE_UIE_MSR), -sResult);
        return FALSE;                   /* exit ...          */
    }

    UieDevice.usHandle = (USHORT)sResult;

    /* --- set receive timer --- */
    usTimer = UIE_MSR_READ;

    sResult = PifControlCom(UieDevice.usHandle, PIF_COM_SET_TIME, (USHORT *)&usTimer);
    if (sResult != PIF_OK && sResult != PIF_ERROR_COM_POWER_FAILURE) {
        PifCloseCom(sResult);

        /* --- error logging --- */
        PifLog(MODULE_UIE_MSR, LOG_ERROR_DEVICE_CONTROLCOM);
        PifLog(MODULE_ERROR_NO(MODULE_UIE_MSR), -sResult);
        return FALSE;                   /* exit ...          */
    }

    UieDevice.fchStatus |= PIF_DEVICE_IF_PROVIDED;

	return TRUE;
}


/*
*===========================================================================
** Synopsis:    BOOL UieMSRTerm(VOID);
*
** Return:      nothing
*===========================================================================
*/
BOOL UieMSRTerm(VOID)
{
	PifCloseCom(UieDevice.usHandle);

	return TRUE;
}


/*
*===========================================================================
** Synopsis:    SHORT UieReadMSR(UIE_MSR *pData);
*    Output:    pData - address of buffer to put the MSR data
*
** Return:      0 >= successful
*               0 <  failer
*===========================================================================
*/
SHORT UieReadMSR(UIE_MSR *pData)
{
    SHORT   sResult, sStatus;
    USHORT  usLoop;
    UCHAR   auchBuffer[512];
#if USED_FOR_TESTING_ONLY
	CHAR   *pTestCardSwipe = ";5499990123456781=09081015432198712345?\r";  // USED_FOR_TESTING_ONLY
	USHORT  usCounter = 0;                                                 // USED_FOR_TESTING_ONLY
#endif

    if (! (UieDevice.fchStatus & PIF_DEVICE_IF_PROVIDED)) {   /* not provide   */
        return (DEVICEIO_E_NOHARDWARE);
    }

    sResult = DEVICEIO_E_FAILURE;
    sStatus = UieMSRClear();                  /* clear receive buffer  */

	memset (pData, 0, sizeof(UIE_MSR));
    for (usLoop = 0; usLoop < 3; usLoop++) {

        /* --- check status of power failure --- */
        if (sStatus == PIF_ERROR_COM_POWER_FAILURE || sStatus == PIF_ERROR_COM_ACCESS_DENIED) {
            PifCloseCom(UieDevice.usHandle);
            if (!UieMSRInit(s_usPort, s_usBaud, s_uchFormat, s_hScale))
				return DEVICEIO_E_COMERROR;
        }

#if 0
		{
			// Card Read request for the IDT 3840 MSR Reader/Writer.
			// This is a card Reader/Writer in the Georgia Southern GenPOS lab
			// which can be used for COM port MSR testing.
			// The IDT 3840 MSR Reader/Writer can both read from a card or
			// write to a card. It has a command set with several different
			// commands such as to Initialize, read a card, write a card, clear
			// a card, reset the device, read the device firmware version, etc.
			// Until a Read command is sent to the device and a card is swiped,
			// a read of the COM port will return nothing.
			// See IDT3840-XX Series Program Manual version 2.9, March 1999.
			//    Richard Chambers, Oct-05-2018
			UCHAR request[] = { 0x1B, 0x72 };
			sStatus = PifWriteCom (UieDevice.usHandle, request, 2);
			PifSleep (100);
		}
#endif

		/* --- receive response message --- */
        sStatus = PifReadCom(UieDevice.usHandle, auchBuffer, sizeof(auchBuffer));

#if USED_FOR_TESTING_ONLY
		if (usCounter) {
			sStatus = strlen(pTestCardSwipe);
			memcpy (auchBuffer, pTestCardSwipe, sStatus+1);
		}
#endif
        if (sStatus == PIF_ERROR_COM_POWER_FAILURE || sStatus == PIF_ERROR_COM_ACCESS_DENIED) {   /* power failure */
//			PifLog(MODULE_UIE_MSR, LOG_ERROR_DEVICE_READCOM);
//			PifLog(MODULE_ERROR_NO(MODULE_UIE_MSR), (USHORT)abs(sStatus));
//			PifLog(MODULE_LINE_NO(MODULE_UIE_MSR), (USHORT)__LINE__);
            PifSleep(50);                               /* 50msec sleep */
            continue;                                   /* next ...      */
        } else if (sStatus == PIF_ERROR_COM_TIMEOUT) {  /* timeout       */
            sResult = DEVICEIO_E_TIMEOUT;
			continue;                                   /* next ...      */
        } else if (sStatus < 6) {                       /* receive error */
            PifSleep(300);                              /* 200msec sleep */
            continue;                                   /* next ...      */
        }

        /* --- analysis response message from scale --- */
        if ((sResult = UieMSRAnalysis(pData, auchBuffer, sStatus)) >= 0) {  /* successful */
			sResult = DEVICEIO_SUCCESS;
            break;                                      /* exit ...      */
        }

        sStatus = UieMSRClear();              /* clear receive buffer  */
        if (sStatus != PIF_ERROR_COM_POWER_FAILURE && sStatus != PIF_ERROR_COM_ACCESS_DENIED) {   /* other error   */
            PifSleep(300);                              /* 100msec sleep */
        }
    }

    /* --- key tracking --- */
    if (sResult < 0) {                                  /* error         */
//        UieTracking(UIE_TKEY_SCALE, (UCHAR)(-sResult), 0);
    }

    return (sResult);
}


/*
*===========================================================================
*   Internal Functions
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UieMSRClear(VOID);
*
** Return:      0 >= successful
*               0 <  failer
*===========================================================================
*/
SHORT UieMSRClear(VOID)
{
    SHORT   sResult;
    USHORT  usTimer;
    UCHAR   auchBuffer[512];

    /* --- set receive timer --- */
    usTimer = 50;                           /* 50 msec timer */

    sResult = PifControlCom(UieDevice.usHandle, PIF_COM_SET_TIME, (USHORT *)&usTimer);

    /* --- clear receive buffer --- */
    do {
        sResult = PifReadCom(UieDevice.usHandle, auchBuffer, sizeof(auchBuffer));
    } while ((sResult < 0) && (sResult != PIF_ERROR_COM_TIMEOUT) &&
				(sResult != PIF_ERROR_COM_POWER_FAILURE) &&
				(sResult != PIF_ERROR_COM_ACCESS_DENIED));

    /* --- set receive timer --- */
    usTimer = UIE_MSR_READ;

    sResult = PifControlCom(UieDevice.usHandle, PIF_COM_SET_TIME, (USHORT *)&usTimer);

    return (sResult);
}


/*
*===========================================================================
** Synopsis:    SHORT UieMSRAnalysis(UIE_MSR *pData, UCHAR *puchBuffer,
*                                      SHORT sLength);
*    Output:    pData      - address of scale data
*     Input:    puchBuffer - address of receive data
*               sLength    - length of receive data
*
** Return:      0 >= successful
*               0 <  failer
** Description: This function takes the string of text that an MSR has provided
*               which contains the card swipe data and then divides the text
*               into the necessary tracks.
*
*               There are three possible tracks of data, each of which is
*               bounded by particular characters.  We are mainly interested
*               in the Track 2 data.  An example swipe from a test card is:
*                  ";5499990123456781=09081015432198712345?\r"
*               where the \r indicates a carriage return character.
*
*               See also the Wedge Keyboard MSR device code in CDeviceEngine::ConvertKey()
*               for the Mobile Demand PC MSR that is a Magtek mag strip card reader.
*               Look for VIRTUALKEY_EVENT_MSR.
*===========================================================================
*/
SHORT UieMSRAnalysis(UIE_MSR *pData, UCHAR *puchBuffer, SHORT sLength)
{
    SHORT   sResult = 1;
	SHORT   i = 0;
	TCHAR  *pTrack = 0, *pTrackStart = 0;
	UCHAR  *pTrackLen = 0;

	for (i = 0; i < sLength; i++) {
		if (pTrack == 0) {
			if (puchBuffer[i] == _T('%')) {
				pTrackStart = pTrack = pData->auchTrack1;
				pTrackLen = &(pData->uchLength1);
				continue;
			}

			if (puchBuffer[i] == _T(';')) {
				pTrackStart = pTrack = pData->auchTrack2;
				pTrackLen = &(pData->uchLength2);
				continue;
			}
			if (pTrack == 0)
				continue;
		}

		if (puchBuffer[i] == _T('?')) {
			*pTrackLen = pTrack - pTrackStart;
			pTrackStart = pTrack = 0;
			pTrackLen = 0;
			continue;
		}
		*pTrack = puchBuffer[i]; pTrack++;
	}

    return sResult;
}


/*
*===========================================================================
** Synopsis:    SHORT UieMSRStatus(UCHAR *puchBuffer, SHORT sLength);
*     Input:    puchBuffer - address of receive data
*               sLength    - length of receive data
*
** Return:      0 >= successful
*               0 <  failer
*===========================================================================
*/
SHORT UieMSRStatus(UCHAR *puchBuffer, SHORT sLength)
{

    return (TRUE);
}


/* ======================================= */
/* ========== End of UieMsr.C ========== */
/* ======================================= */
