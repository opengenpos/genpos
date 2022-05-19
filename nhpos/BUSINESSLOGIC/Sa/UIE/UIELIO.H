/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Local Common Header File of I/O module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieLIO.H
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
*   Active Device Status
*===========================================================================
*/
#define UIE_ACTIVE_GET      0x01            /* on - active for normal */
#define UIE_DISPLAY         0x02            /* on - visible display   */
#define UIE_EVENT_WAIT      0x04            /* on - processing FSC_CNTRL_STRING_EVENT, waiting for signal   */


/*
*===========================================================================
*   Descriptor Control
*===========================================================================
*/
#define UIE_DESC_MASK           0x00000003L
#define UIE_DESC_SHIFT          2


/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/


/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
/* ----- UieInit2.C ----- */
extern  CONST   SYSCONFIG FAR *pUieSysCfg;  /* system configuration    */
extern  UCHAR   fchUieActive;               /* status of active device */


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/


/* ===================================== */
/* ========== End of UieLIO.H ========== */
/* ===================================== */
