/*************************************************************************
 *
 * DeviceIODisplay.h
 *
 * $Workfile:: DeviceIODisplay.h                                         $
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
 * $History:: DeviceIODisplay.h                                          $
 *
 ************************************************************************/

#if !defined(_INC_DEVICEIO_DISPLAY)
#define _INC_DEVICEIO_DISPLAY

#include "DeviceIo.h"

#ifdef __cplusplus
extern "C"{
#endif

#pragma pack(push, 8)


/*
/////////////////////////////////////////////////////////////////////////////
//  IoControl Function Interface
/////////////////////////////////////////////////////////////////////////////
*/

#define DEVICEIO_CTL_DISPLYA_STRING			(DEVICEIO_DISPLAY + 1)
#define DEVICEIO_CTL_DISPLYA_ATTRIBUTE		(DEVICEIO_DISPLAY + 2)
#define DEVICEIO_CTL_DISPLYA_DESCRIPTOR		(DEVICEIO_DISPLAY + 3)
#define DEVICEIO_CTL_DISPLYA_GETINFO		(DEVICEIO_DISPLAY + 99)


/* Dispalay Attribute */

#define DEVICEIO_DISPLAY_AN		0x00		/* normal attribute        */
#define DEVICEIO_DISPLAY_AB		0x80		/* blink attribute         */
#define DEVICEIO_DISPLAY_AC		0x20		/* comma attribute         */
#define DEVICEIO_DISPLAY_AP		0x40		/* decimal point attribute */

/* Display String */

// This struct needs to be synchronized with struct FRAMEWORK_IO_DISPLAY_DATA
// in file FrameworkIO.h so anything added here probably needs to be added there.
typedef struct _DEVICEIO_DISPLAY_DATA
{
	LONG	nRow;							/* start of row        */
	LONG	nColumn;						/* start of column     */
	UCHAR	uchAttribute;					/* character attribute */
	LONG	nChars;							/* number of character */
	TCHAR*	lpuchBuffer;					/* buffer address      */
} DEVICEIO_DISPLAY_DATA, *PDEVICEIO_DISPLAY_DATA;

/* Descriptor Attribute */

#define	DEVICEIO_DISPLAY_DOFF	0			/* off   */
#define	DEVICEIO_DISPLAY_DON	1			/* on    */
#define	DEVICEIO_DISPLAY_DBLINK	2			/* blink */

/* Light Descriptor */

typedef struct _DEVICEIO_DISPLAY_DESCRIPTOR
{
	LONG	nNumber;						/* descriptor number   */
	UCHAR	uchAttribute;					/* character attribute */
} DEVICEIO_DISPLAY_DESCRIPTOR, *PDEVICEIO_DISPLAY_DESCRIPTOR;

/* Display Information */

typedef struct _DEVICEIO_DISPLAY_INFO
{
	DWORD	dwSize;							/* length of structure     */
	LONG	nRow;							/* number of row           */
	LONG	nColumn;						/* number of column        */
	LONG	nDescriptor;					/* number of descriptor    */
	LONG	nDecimal;						/* number of decimal point */
	LONG	nComma;							/* number of comma         */
	UCHAR	szFirmware[128];				/* firmware information    */
} DEVICEIO_DISPLAY_INFO, *PDEVICEIO_DISPLAY_INFO;


/*
/////////////////////////////////////////////////////////////////////////////
*/

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_DEVICEIO_DISPLAY) */
