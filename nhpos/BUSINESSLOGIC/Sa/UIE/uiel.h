/*
*===========================================================================
*
*   Copyright (c) 1994, NCR Corporation, E&M-OISO. All rights reserved.
*
*===========================================================================
* Title       : Local Common Header File
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieL.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: 
* Options     : 
* --------------------------------------------------------------------------
* Abstract:     This header file defines User Interface Engine the 2170
*               system application.
*
*               This file contains:
*                   1. Symbol Definitions
*                   2. Function Prototypes
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* May-09-94  F1        O.Nakada     Initial
* Mar-15-95  G0        O.Nakada     Modified all declarations, correspond to
*                                   hospitality release 3.0 functions.
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2 R.Chambers  Source cleanup to remove old, before NCR 7448,
*                                   code that applied to NCR 2170 back in the 1990s.
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
#ifndef _INC_UIEL
#define _INC_UIEL


/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/
#undef  STATIC
#define STATIC

#define UIE_DEC_INIT            0xFF        /* initialize data      */


/*
*===========================================================================
*   Receipt/Journal Feed
*===========================================================================
*/
/*
===== Quation =====
UIESEQ.H and UIEERROR.H should be the same as this definition.
===== Quation =====
*/
#if !defined(UIE_RECEIPT_FEED_ROW)
#define UIE_RECEIPT_FEED_ROW    8           /* Number of Receipt Feed Lines */
#define UIE_JOURNAL_FEED_ROW    4           /* Number of Journal Feed Line */
#endif


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/* ----- UIEINIT.C ----- */
VOID    UieInitialize2(CONST CVTTBL  * *pCvt, CONST FSCTBL  * *pFsc, UCHAR uchMax);

/* ----- UIEKEY.C ----- */
VOID    UieSetStatusKeyboard(VOID);
VOID    UieSetModeS2A(VOID);
VOID    UieSetModeS2D(VOID);
VOID    UieSetModeD2S(VOID);

//  added to support TOUCHSCREEN
LONG	UieSendKeyboardInput(ULONG ulLength, CONST TCHAR* pData);
VOID	UieClearKeyboardBuf();

/* ----- UIEDRAW.C ----- */
VOID    DLLENTRY UieSetStatusDrawer(VOID);
LONG             UiePutDrawerData(ULONG, CONST USHORT *, VOID *);

/* ----- UIEWAIT.C ----- */
#if defined(PERIPHERAL_DEVICE_WAITER_LOCK)
VOID    DLLENTRY UieSetStatusWaiter(VOID);
LONG             UiePutServerLockData(ULONG, CONST SHORT *, VOID *);
#endif

/* ----- UIEMSR.C ----- */
#if defined(PERIPHERAL_DEVICE_MSR)
LONG             UiePutMsrData(ULONG, CONST MSR_BUFFER *, VOID *);
LONG             UiePutPinPadData(ULONG, CONST PINPAD_BUFFER *, VOID *);
LONG             UiePutSignatureCaptureData(ULONG, CONST UIE_SIGCAP *, VOID *);
#endif

/* ----- UIESCAN.C ----- */
LONG             UiePutScannerData(ULONG, CONST UCHAR *, VOID *);

/* ----- UIEECHO.C ----- */
VOID    DLLENTRY UieEchoBackGround2(ULONG flDescriptor, UCHAR uchAttr, UCHAR *pszBuffer, USHORT usLen);
VOID    DLLENTRY UieResetKeepMessage(VOID);
UCHAR   DLLENTRY UieGetEchoStatus(VOID);
VOID    DLLENTRY UieRedisplay(VOID);

/* ----- UIEDISP.C ----- */
SHORT   DLLENTRY UiePrint(USHORT usDisplay, USHORT usRow, USHORT usColumn, UCHAR *pszBuffer, USHORT usLen);
SHORT   DLLENTRY UieAttrPrint(USHORT usDisplay, USHORT usRow, USHORT usColumn, UCHAR uchAttr, UCHAR *pszBuffer, USHORT usLen);
VOID    DLLENTRY UieSetStatusDisplay(USHORT usControl);
VOID    DLLENTRY UieDisplayAttrEx(USHORT usDispId, USHORT usRow, USHORT usColumn, CONST TCHAR FAR *puchString, USHORT usChars);
VOID    DLLENTRY UieLightDescrEx(USHORT usDispId, USHORT usDescrId, UCHAR uchAttr);

#define PifDisplayAttr  UieDisplayAttrEx
#define PifLightDescr   UieLightDescrEx


#endif                                      /* _INC_UIEL */
/* =================================== */
/* ========== End of UieL.H ========== */
/* =================================== */
