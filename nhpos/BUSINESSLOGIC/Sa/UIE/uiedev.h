/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Local Header File of Input Device Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieDev.H
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
*                   2. Data type Definitions
*                   3. External Data Definitions
*                   4. Function Prototypes
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* May-25-93  C1        O.Nakada     Removed input device control functions.
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Mar-15-95  G0        O.Nakada     Modified all declarations, correspond to
*                                   hospitality release 3.0 functions.
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
*   Function Prototype Declarations
*===========================================================================
*/
SHORT   UieReadMacro(KEYMSG *pMsg);
VOID    UieEmptyRingBuf(VOID);
SHORT   UieReadData(KEYMSG *pMsg);
SHORT   UieReadTester(KEYMSG *pMsg);
SHORT   UieModeData(UCHAR uchMode, KEYMSG *pMsg);
SHORT   UieMiscData(FSCTBL *Ring, UCHAR *puchData, USHORT usLength,
                    KEYMSG *pMsg);
SHORT   UieKeyData(USHORT usType, FSCTBL Fsc, KEYMSG *pMsg);
SHORT   UieCheckKeyData(USHORT usType, FSCTBL Fsc, KEYMSG *pMsg);
SHORT   UieWriteANKey(UCHAR uchChar, KEYMSG *pMsg);
SHORT   UieMakeGeneralMsg(FSCTBL Fsc, KEYMSG *pMsg);
SHORT   UieMakeClearMsg(KEYMSG *pMsg);
SHORT   UieMakeErrorMsg(USHORT usErrorCode, KEYMSG *pMsg);

SHORT	UieReadKeyboardData(KEYMSG *pMsg);  // TOUCHSCREEN support
SHORT	UieCopyKeyMsg();		// Makes a copy of UieMsg
SHORT	UieGetKeyMsg(KEYMSG *pMsg, USHORT i);

/* ===================================== */
/* ========== End of UieDev.H ========== */
/* ===================================== */
