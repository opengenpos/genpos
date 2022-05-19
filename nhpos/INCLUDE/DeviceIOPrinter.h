/*************************************************************************
 *
 * DeviceIOPrinter.h
 *
 * $Workfile:: DeviceIOPrinter.h                                         $
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
 * $History:: DeviceIOPrinter.h                                          $
 *
 ************************************************************************/

#if !defined(_INC_DEVICEIO_PRINTER)
#define _INC_DEVICEIO_PRINTER

#include "DeviceIo.h"
#include "printercaps.h"
#ifdef __cplusplus
extern "C"{
#endif

/*
/////////////////////////////////////////////////////////////////////////////
//  IoControl Function Interface
/////////////////////////////////////////////////////////////////////////////
*/

#define	DEVICEIO_CTL_PRINTER_WAIT				DEVICEIO_PRINTER + 1
#define	DEVICEIO_CTL_PRINTER_VALIDATION			DEVICEIO_PRINTER + 2
#define	DEVICEIO_CTL_PRINTER_END_VALIDATION		DEVICEIO_PRINTER + 3
#define	DEVICEIO_CTL_PRINTER_FEED				DEVICEIO_PRINTER + 4
#define	DEVICEIO_CTL_PRINTER_IMPORTANT			DEVICEIO_PRINTER + 5
#define	DEVICEIO_CTL_PRINTER_END_IMPORTANT		DEVICEIO_PRINTER + 6
#define	DEVICEIO_CTL_PRINTER_GETSTATUS			DEVICEIO_PRINTER + 7
#define	DEVICEIO_CTL_PRINTER_PRTCONFIG			DEVICEIO_PRINTER + 8
#define	DEVICEIO_CTL_PRINTER_SET_CALLBACK		DEVICEIO_PRINTER + 9
#define	DEVICEIO_CTL_PRINTER_FONT				DEVICEIO_PRINTER + 10
#define	DEVICEIO_CTL_PRINTER_VOL_WAIT_COUNT		DEVICEIO_PRINTER + 11

//US Customs slip printer control change for short forms
#define	DEVICEIO_CTL_PRINTER_SMALL_VALIDATION		DEVICEIO_PRINTER + 12
#define	DEVICEIO_CTL_PRINTER_END_SMALL_VALIDATION	DEVICEIO_PRINTER + 13

/* control printer */

/////////////////////////////////////////////////////////////////////////
// Write Function
/////////////////////////////////////////////////////////////////////////
typedef struct _DEVICEIO_PRINTER_WRITE
{
	WORD	wPrintStation;
	LPBYTE	lpData;								/* Write Data */
	WORD	wLength;							/* Write Data Length */
} DEVICEIO_PRINTER_WRITE, *PDEVICEIO_PRINTER_WRITE;




/////////////////////////////////////////////////////////////////////////
// IoControl Function
/////////////////////////////////////////////////////////////////////////
typedef struct _DEVICEIO_PRINTER_ARG_VALIDATION
{
	WORD	wPrintStation;
} DEVICEIO_PRINTER_ARG_VALIDATION, *PDEVICEIO_PRINTER_ARG_VALIDATION;

typedef struct _DEVICEIO_PRINTER_ARG_END_VALIDATION
{
	WORD	wPrintStation;
} DEVICEIO_PRINTER_ARG_END_VALIDATION, *PDEVICEIO_PRINTER_ARG_END_VALIDATION;

typedef struct _DEVICEIO_PRINTER_ARG_FEED
{
	WORD	wPrintStation;
	WORD	wFeedLen;
} DEVICEIO_PRINTER_ARG_FEED, *PDEVICEIO_PRINTER_ARG_FEED;

typedef struct _DEVICEIO_PRINTER_ARG_IMPORTANT
{
	WORD	wPrintStation;
} DEVICEIO_PRINTER_ARG_IMPORTANT, *PDEVICEIO_PRINTER_ARG_IMPORTANT;

typedef struct _DEVICEIO_PRINTER_ARG_END_IMPORTANT
{
	WORD	wPrintStation;
} DEVICEIO_PRINTER_ARG_END_IMPORTANT, *PDEVICEIO_PRINTER_ARG_END_IMPORTANT;

typedef struct _DEVICEIO_PRINTER_ARG_GETSTATUS
{
	WORD	wPrintStation;
	LPBYTE	lpStatus;								/* Status */
} DEVICEIO_PRINTER_ARG_GETSTATUS, *PDEVICEIO_PRINTER_ARG_GETSTATUS;

typedef struct _DEVICEIO_PRINTER_ARG_PRTCONFIG
{
	WORD	wPrintStation;
	LPWORD	lpColumn;								/* Print Column */
	LPBYTE	lpStatus;								/* Status */
} DEVICEIO_PRINTER_ARG_PRTCONFIG, *PDEVICEIO_PRINTER_ARG_PRTCONFIG;

typedef VOID	(WINAPI *PCALL_BACK)(DWORD);		/* CallBack Function */

typedef struct _DEVICEIO_PRINTER_ARG_SET_CALLBACK
{
	PCALL_BACK	lpCallBack;							/* Status */
} DEVICEIO_PRINTER_ARG_SET_CALLBACK, *PDEVICEIO_PRINTER_ARG_SET_CALLBACK;

typedef struct _DEVICEIO_PRINTER_ARG_FONT
{
	WORD	wPrintStation;
	WORD	wFont;									/* Print Font */
} DEVICEIO_PRINTER_ARG_FONT, *PDEVICEIO_PRINTER_ARG_FONT;

typedef struct _DEVICEIO_PRINTER_ARG_VOL_WAIT_COUNT
{
	WORD	wRetryCount;
} DEVICEIO_PRINTER_ARG_VOL_WAIT_COUNT, *PDEVICEIO_PRINTER_ARG_VOL_WAIT_COUNT;

/**
;========================================================================
;+                                                                      +
;+              E R R O R    D E F I N I T I O N s                      +
;+                                                                      +
;========================================================================
**/
#define DEVICEIO_PRINTER_MSG_SUCCESS_ADR	  0			//successful print
#define DEVICEIO_PRINTER_MSG_VSLPINS_ADR      39      /* Request Validation Slip Insertion */
#define DEVICEIO_PRINTER_MSG_VSLPREMV_ADR     40      /* Request Validation Slip Removing */
#define DEVICEIO_PRINTER_MSG_PRTTBL_ADR       5       /* Printer Trouble */
#define DEVICEIO_PRINTER_MSG_PAPERCHG_ADR     50      /* Request Slip Paper Change */                                  
#define DEVICEIO_PRINTER_MSG_JNLLOW_ADR       4       /* Journal Low Condition */
#define DEVICEIO_PRINTER_MSG_PAPEROUT_ADR	  49		// Receipt paper out
#define DEVICEIO_PRINTER_MSG_COVEROPEN_ADR	  48		// Receipt Cover open
#define DEVICEIO_PRINTER_MSG_POWEROFF_ADR	  51	    // Power Off

/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_DEVICEIO_PRINTER) */
