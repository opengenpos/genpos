/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Local Header File of Error Message
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieError.H
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
* Sep-04-92  00.01.00  O.Nakada     Changed number of feed lines.
* Oct-21-92  00.01.08  O.Nakada     Added UIE_DESCRIPTOR_PAPER.
* Nov-04-92  00.01.09  O.Nakada     Changed number of journal feed.
* Feb-19-93  01.00.10  O.Nakada     Modfiy model dependent functions.
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Mar-15-95  G0        O.Nakada     Modified all declarations, correspond to
*                                   hospitality release 3.0 functions.
*
*** NCR2172 ***
* Oct-05-99  01.00.00  M.Teraki     Added #pragma pack(...)
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
*   Descriptor Control of Validation and Slip Printer
*===========================================================================
*/
#define UIE_PAPER_NOT_SUPPORT   0xFFFF      /* not support              */
#define UIE_DESCRIPTOR_PAPER    6           /* No. 6                    */


/*
*===========================================================================
*   Error Message Interface
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct _UIEERRORTBL {
    CONST   UCHAR FAR *puchAddr;            /* address of error message     */
    UCHAR   uchSize;                        /* record size of error message */
} UIEERRORTBL;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/

VOID    UieRefreshDisplay(VOID);


/* ======================================= */
/* ========== End of UieError.H ========== */
/* ======================================= */
