/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Local Header File of Echo Back Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieEcho.H
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
* May-25-93  C1        O.Nakada     Removed UieInitEchoBack() from UIEDEV.H.
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
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


/*
*===========================================================================
*   Echo Back Status
*===========================================================================
*/
/* 
===== Caution!! =====
UIEECHO.H and UIEMISC.H should be the same as this definition.
===== Caution!! =====
*/
#define UIE_CTRL_BACK_GROUND    0x01        /* on - enable echo back ground */
#define UIE_BACK_GROUND         0x02        /* Echo Back Ground Flag */
#define UIE_INIT_ECHO_BACK      0x04        /* on - request display         */
#define UIE_DOUBLE_WIDE         0x08        /* on - enable echo back wide   */
#define UIE_KEEP_MSG            0x10        /* on - enable keep message     */


typedef struct _UIEECHO {
    struct {                                /* double wide character        */
        USHORT  usRow;                      /* position of row              */
        USHORT  usColumn;                   /* position of column           */
    } WIDE;
    USHORT      usFigure;                       /* number figure                */
    UCHAR       uchType;                        /* echo back type of current    */
    UCHAR       uchMax;                         /* length of max.               */
    DCURRENCY   lAmount;                        /* preset cash amount           */
    UCHAR       fchStatus;                      /* control status               */
    ULONG       flDescriptor;                   /* descriptor of back ground    */
    TCHAR       auchBack[UIE_MAX_DISP_BUF];     /* display of back ground       */
} UIEECHO;


/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
extern UIEECHO  UieEcho;                    /* echo back status          */
extern UCHAR    uchUieEcho;                 /* echo back type of current */


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
VOID    UieEchoBackInit(VOID);
VOID    UiePreEchoBack(KEYMSG *pMsg);
VOID    UieDisplayEchoBack(KEYMSG *pMsg);
VOID    UieDisplayEchoBackDesc(ULONG flDescriptor);
VOID    UieDisplayEchoBackWide(CONST CHAR *pchType);
VOID    UieEchoBackMacro(USHORT usLead, CONST UCHAR *puchAddr, USHORT usSize);
SHORT   UieEchoBackPrintf(USHORT usRemain, USHORT usDisplay, USHORT usRow, USHORT usColumn, UCHAR uchAttr, CONST TCHAR *pszFmt, ...);
USHORT  UieFillSuppress(TCHAR *puchStr, TCHAR uchFill, USHORT usRemain, TCHAR *puchMax);
VOID    UieRefreshEchoBack(VOID);
USHORT  UieGetOperWidth(VOID);


/* ====================================== */
/* ========== End of UieEcho.H ========== */
/* ====================================== */
