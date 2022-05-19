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
 * $History:: UIESCALE.cpp                                               $
 *
 ************************************************************************/

#include "stdafx.h"
#include <math.h>

#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "UieScale.h"
#include "Device.h"


/*
*===========================================================================
*   General Declarations
*===========================================================================
*/

#define UIE_SCALE_ETX           0x03        /* ETX character         */

#define UIE_SCALE_READ          500         /* read timer of scale   */

static  PIF_DEVICE_IF   UieScale;

static	USHORT			s_usPort;
static	USHORT			s_usBaud;
static	UCHAR			s_uchFormat;
static	HANDLE			s_hScale;

static SHORT   UieScaleClear(VOID);
static SHORT   UieScaleAnalysis(UIE_SCALE *pData, UCHAR *puchBuffer, SHORT sLength);
static SHORT   UieScaleStatus(UCHAR *puchBuffer, SHORT sLength);


/*
*===========================================================================
*   Public Functions
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    BOOL UieScaleInit(HANDLE);
*
** Return:      nothing
*===========================================================================
*/
BOOL UieScaleInit(USHORT usPort, USHORT usBaud, UCHAR uchFormat, HANDLE hScale)
{
    SHORT       sResult;
	USHORT      usTimer;
	PROTOCOL    Protocol = { 0 };

	// save scale handle
	s_usPort    = usPort;
	s_usBaud    = usBaud;
	s_uchFormat = uchFormat;
	s_hScale    = hScale;

    /* --- make protocol data --- */
    Protocol.fPip             = PIF_COM_PROTOCOL_NON;
    Protocol.ulComBaud = usBaud;
    Protocol.uchComByteFormat = uchFormat;
    Protocol.uchComTextFormat = PIF_COM_FORMAT_TEXT;

    /* --- open serial port --- */
    sResult = PifOpenCom(usPort, &Protocol);
    UieScale.usHandle = sResult;
    if (sResult < 0) {                  /* open error        */
        PifLog(MODULE_UIE_SCALE, LOG_ERROR_DEVICE_OPENCOM);
        PifLog(MODULE_ERROR_NO(MODULE_UIE_SCALE), (USHORT)abs(sResult));
		PifLog(MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
	} else {
		/* --- set receive timer --- */
		usTimer = UIE_SCALE_READ;
		sResult = PifControlCom(UieScale.usHandle, PIF_COM_SET_TIME, (USHORT *)&usTimer);
		if (sResult != PIF_OK && sResult != PIF_ERROR_COM_POWER_FAILURE) {
			PifLog(MODULE_UIE_SCALE, LOG_ERROR_DEVICE_CONTROLCOM);
			PifLog(MODULE_ERROR_NO(MODULE_UIE_SCALE), (USHORT)abs(sResult));
			PifLog(MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);

			PifCloseCom(UieScale.usHandle);  // error from PifControlCom() so close the handle
			UieScale.usHandle = -1;    // PIF_FILE_INVALID_HANDLE
		}
	}

	return TRUE;
}


/*
*===========================================================================
** Synopsis:    BOOL UieScaleTerm(VOID);
*
** Return:      nothing
*===========================================================================
*/
BOOL UieScaleTerm(VOID)
{
	PifCloseCom(UieScale.usHandle);

	return TRUE;
}


/*
*===========================================================================
** Synopsis:    SHORT UieReadScale(UIE_SCALE *pData);
*    Output:    pData - address of scale data
*
**Return: :  UIE_SUCCESS            read of scale data was successful.
*            UIE_SCALE_DATAFORMAT   format of scale weight data is invalid.
*            UIE_SCALE_DATAUNITS    units of weight are unknown
*            UIE_SCALE_TIMEOUT      time out while attempting reading from COM port
*            UIE_SCALE_ERROR_COM    error reading from COM port
*            UIE_SCALE_NOT_PROVIDE  scale not provisioned
*            UIE_SCALE_ERROR        an unknown error when trying to read the scale.
*         
** Description:   
*                
*===========================================================================
*/
SHORT UieReadScale(UIE_SCALE *pData)
{
    SHORT   sResult, sStatus;
    USHORT  usLoop;
    UCHAR   auchBuffer[24];

	// for a USB type of scale with a Virtual Serial Communication port we need to handle
	// an error of PIF_ERROR_COM_ACCESS_DENIED which may mean the USB cable was unplugged
	// and the Virtual Serial Communication port is no longer available.
    sResult = UIE_SCALE_ERROR;
    sStatus = UieScaleClear();                  /* clear receive buffer  */

    for (usLoop = 0; usLoop < 3; usLoop++) {
        /* --- check status of power failure --- */
        if (sStatus == PIF_ERROR_COM_POWER_FAILURE || sStatus == PIF_ERROR_COM_ACCESS_DENIED) {
			PifLog(MODULE_UIE_SCALE, LOG_ERROR_DEVICE_READCOM);
			PifLog(MODULE_ERROR_NO(MODULE_UIE_SCALE), (USHORT)abs(sStatus));
			PifLog(MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
            PifCloseCom(UieScale.usHandle);
            PifSleep(50);                               /* 50msec sleep */
            if (!UieScaleInit(s_usPort, s_usBaud, s_uchFormat, s_hScale))
				return UIE_SCALE_ERROR_COM;
        }

        /* --- send request message --- */
        * auchBuffer      = 'W';
        *(auchBuffer + 1) = '\r';

        sStatus = PifWriteCom(UieScale.usHandle, auchBuffer, 2);
        if (sStatus == PIF_ERROR_COM_POWER_FAILURE || sStatus == PIF_ERROR_COM_ACCESS_DENIED) {   /* power failure */
            PifSleep(50);                               /* 50msec sleep */
            continue;                                   /* next ...      */
        } else if (sStatus != 2) {                      /* send error    */
            PifSleep(200);                              /* 200msec sleep */
            continue;                                   /* next ...      */
        }

        /* --- receive response message --- */
        sStatus = PifReadCom(UieScale.usHandle, auchBuffer, sizeof(auchBuffer));
        if (sStatus == PIF_ERROR_COM_POWER_FAILURE || sStatus == PIF_ERROR_COM_ACCESS_DENIED) {   /* power failure */
			PifLog(MODULE_UIE_SCALE, LOG_ERROR_DEVICE_READCOM);
			PifLog(MODULE_ERROR_NO(MODULE_UIE_SCALE), (USHORT)abs(sStatus));
			PifLog(MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
            PifSleep(50);                               /* 50msec sleep */
            continue;                                   /* next ...      */
        } else if (sStatus == PIF_ERROR_COM_TIMEOUT) {  /* timeout       */
			// following PIFLOG entries commentted out to reduce logs generated due to scale is provisioned
			// but not being used so the PifReadCom() calls are timing out.
//			PifLog(MODULE_UIE_SCALE, LOG_ERROR_DEVICE_READCOM);
//			PifLog(MODULE_ERROR_NO(MODULE_UIE_SCALE), (USHORT)abs(sStatus));
//			PifLog(MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
            sResult = UIE_SCALE_TIMEOUT;
            continue;                                   /* next ...      */
        } else if (sStatus < 6) {                       /* receive error */
			PifLog(MODULE_UIE_SCALE, LOG_ERROR_DEVICE_READCOM);
			PifLog(MODULE_ERROR_NO(MODULE_UIE_SCALE), (USHORT)abs(sStatus));
			PifLog(MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
            PifSleep(300);                              /* 200msec sleep */
            continue;                                   /* next ...      */
        }

        /* --- analysis response message from scale --- */
        if ((sResult = UieScaleAnalysis(pData, auchBuffer, sStatus)) >= 0) {  /* successful */
            break;                                      /* exit ...      */
        }

        sStatus = UieScaleClear();              /* clear receive buffer  */
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
** Synopsis:    SHORT UieScaleClear(VOID);
*
** Return:      0 >= successful
*               0 <  failer
*===========================================================================
*/
static SHORT UieScaleClear(VOID)
{
    SHORT   sResult;
    USHORT  usTimer;
    UCHAR   auchBuffer[24];

    /* --- set receive timer --- */
    usTimer = 50;                           /* 50 msec timer */

    sResult = PifControlCom(UieScale.usHandle, PIF_COM_SET_TIME, (USHORT *)&usTimer);
    if (sResult != PIF_OK && sResult != PIF_ERROR_COM_POWER_FAILURE) {
        /* --- error logging --- */
        PifLog(MODULE_UIE_SCALE, LOG_ERROR_DEVICE_CONTROLCOM);
        PifLog(MODULE_ERROR_NO(MODULE_UIE_SCALE), (USHORT)abs(sResult));
		PifLog(MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);

        PifCloseCom(UieScale.usHandle);
        return sResult;                   /* exit ...          */
	}

    /* --- clear receive buffer --- */
    do {
        sResult = PifReadCom(UieScale.usHandle, auchBuffer, sizeof(auchBuffer));
    } while (((sResult < 0) && (sResult != PIF_ERROR_COM_TIMEOUT) &&
                               (sResult != PIF_ERROR_COM_POWER_FAILURE) &&
                               (sResult != PIF_ERROR_COM_ACCESS_DENIED)) ||
                               (sResult == sizeof(auchBuffer)));

    /* --- set receive timer --- */
    usTimer = UIE_SCALE_READ;

    sResult = PifControlCom(UieScale.usHandle, PIF_COM_SET_TIME, (USHORT *)&usTimer);

    return (sResult);
}


/*
*===========================================================================
** Synopsis:    SHORT UieScaleAnalysis(UIE_SCALE *pData, UCHAR *puchBuffer,
*                                      SHORT sLength);
*    Output:    pData      - address of scale data
*     Input:    puchBuffer - address of receive data
*               sLength    - length of receive data
*
**Return: :  UIE_SUCCESS            read of scale data was successful.
*            UIE_SCALE_DATAFORMAT   format of scale weight data is invalid.
*            UIE_SCALE_DATAUNITS    units of weight are unknown
*            UIE_SCALE_ERROR        an unknown error when trying to read the scale.
*
** Description:  Analyze the data returned by the scale and fill in the UIE_SCALE
*                struct with the data returned.
*                
*                The protocol used is to send a command to the scale and then read
*                the response from the scale. The response is a series of one byte
*                charaters that provide the current weight as a series of digit characters
*                as well as a set of status bytes.
*===========================================================================
*/
static SHORT UieScaleAnalysis(UIE_SCALE *pData, UCHAR *puchBuffer, SHORT sLength)
{
    UCHAR   uchByte;
    USHORT  usLoop;
    SHORT   sResult;
    ULONG   ulData = 0;

	memset (pData, 0, sizeof(*pData));

    if (sLength != 16) {
		/* non-standard message. is this a scale status message? */
        pData->uchUnit = UIE_SCALE_UNIT_UNKNOWN;
        if ((sResult = UieScaleStatus(puchBuffer, sLength)) >= 0) {
			PifLog(MODULE_UIE_SCALE, LOG_ERROR_DEVICE_STATUS);
			PifLog(MODULE_ERROR_NO(MODULE_UIE_SCALE), (USHORT)abs(sResult));
			PifLog(MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
            return (UIE_SCALE_ERROR);
        } else {
			PifLog(MODULE_UIE_SCALE, LOG_ERROR_DEVICE_STATUS);
			PifLog(MODULE_ERROR_NO(MODULE_UIE_SCALE), (USHORT)abs(sResult));
			PifLog(MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
            return (sResult);
        }
    }

    if (* puchBuffer != '\n' || *(puchBuffer + 9) != '\r')
	{
        pData->uchUnit = UIE_SCALE_UNIT_UNKNOWN;
		PifLog(MODULE_UIE_SCALE, LOG_ERROR_DEVICE_READCOM);
		PifLog(MODULE_ERROR_NO(MODULE_UIE_SCALE), UIE_SCALE_ERROR);
		PifLog(MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
        return (UIE_SCALE_ERROR);
    }

	// a normal scale weight message has a status section that follows the first part of
	// the message which contains the weight. move our pointer past the weight part of the
	// message and check the status section of the message.
    if ((sResult = UieScaleStatus(puchBuffer + 10, sLength - 10)) < 0)
	{
        pData->uchUnit = UIE_SCALE_UNIT_UNKNOWN;
		PifLog(MODULE_UIE_SCALE, LOG_ERROR_DEVICE_STATUS);
		PifLog(MODULE_ERROR_NO(MODULE_UIE_SCALE), UIE_SCALE_ERROR);
		PifLog(MODULE_ERROR_NO(MODULE_UIE_SCALE), (USHORT)abs(sResult));
		PifLog(MODULE_LINE_NO(MODULE_UIE_SCALE), (USHORT)__LINE__);
        return (UIE_SCALE_ERROR);
    }

    /* --- check data from the scale.
	 *     format is a series of digits with an embedded decimal point
	 *     followed by a set of letters indicating the unit of weight measurement.
	 *     examples would be 000.45LB or 00.30KG
	*/
    for (usLoop = 0; usLoop < 6; usLoop++) {
        ++puchBuffer;
        uchByte = *puchBuffer;

        if ('0' <= uchByte && uchByte <= '9') {
            ulData = ulData * 10 + (uchByte - 0x30);
        } else if (uchByte == '.') {
            pData->uchDec = (UCHAR)(5 - usLoop);
        } else {
			pData->uchUnit = UIE_SCALE_UNIT_UNKNOWN;
            return (UIE_SCALE_DATAFORMAT);
        }
    }

    pData->ulData = ulData;

    /* --- check unit --- */
    ++puchBuffer;

    if (*puchBuffer == 'L' && *(puchBuffer + 1) == 'B') {           /* LB */
        pData->uchUnit = UIE_SCALE_UNIT_LB;
    } else if (*puchBuffer == 'K' && *(puchBuffer + 1) == 'G') {    /* KG */
        pData->uchUnit = UIE_SCALE_UNIT_KG;
    } else {
        pData->uchUnit = UIE_SCALE_UNIT_UNKNOWN;
        return (UIE_SCALE_DATAUNITS);
    }

    return (UIE_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    SHORT UieScaleStatus(UCHAR *puchBuffer, SHORT sLength);
*
*     Input:    puchBuffer - point to begining of status data in the message received
*               sLength    - length of receive data
*
** Return:      0 >= successful
*            UIE_SCALE_MOTION           scale status indicates scale is still in motion
*            UIE_SCALE_UNDER_CAPACITY   scale status indicates weight is too small to be measured
*            UIE_SCALE_OVER_CAPACITY    scale status indicates weight is too large to be measured
*            UIE_SCALE_DATAFORMAT       format of the scale message is incorrect
*===========================================================================
*/
static SHORT UieScaleStatus(UCHAR *puchBuffer, SHORT sLength)
{
    UCHAR   uchByte;

	// The scale message may be a status only message if there is a problem with the scale.
	// A status only message is 6 characters with the letter S as the second character.
	// A scale message with a weight will have the status section after the weight section.
    if (sLength != 6              ||
        * puchBuffer      != '\n' ||
        *(puchBuffer + 1) != 'S'  ||
        *(puchBuffer + 4) != '\r' ||
        *(puchBuffer + 5) != UIE_SCALE_ETX) {
        return (UIE_SCALE_DATAFORMAT);               /* exit ... */
    }

    /* --- check status of low byte --- */
    uchByte = *(puchBuffer + 3) - (UCHAR)0x30;

    if (uchByte & UIE_SCALE_UNDER) {
        return (UIE_SCALE_UNDER_CAPACITY);
    } else if (uchByte & UIE_SCALE_OVER) {
        return (UIE_SCALE_OVER_CAPACITY);
    }

    /* --- check status of high byte --- */
    uchByte = *(puchBuffer + 2) - (UCHAR)0x30;
    if (uchByte & UIE_SCALE_IN_MOTION) {
        return (UIE_SCALE_MOTION);
    } else if (uchByte & UIE_SCALE_ZERO) {
        return (0);
    } else {
        return (1);
    }
}


/* ======================================= */
/* ========== End of UieScale.C ========== */
/* ======================================= */
