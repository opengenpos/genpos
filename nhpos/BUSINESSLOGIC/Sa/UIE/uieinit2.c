/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Initialize Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieInit2.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Nov-26-92  01.00.01  O.Nakada     Add Scale I/F function.
* May-20-93  B3        O.Nakada     Add coin despensor function.
* May-25-93  C1        O.Nakada     Removed input device control functions.
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Mar-15-95  G0        O.Nakada     Modified all modules, correspond to
*                                   hospitality release 3.0 functions.
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2 R.Chambers  Source cleanup to remove old, before NCR 7448,
*                                   code that applied to NCR 2170 back in the 1990s.
* May-19-15  GenPOS 2.2.1 R.Chambers   use PifCreateSemNew() with usUieSyncNormal to set max count.
* May-21-15  GenPOS 2.2.1 R.Chambers   use PifCreateSemNew() with usUieSyncError to set max count.
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
#include "ecr.h"
#include "pif.h"
#include "uie.h"
#include "uiel.h"
#include "uielio.h"
#include "uieio.h"
#include "uiering.h"
#include "uiedisp.h"
#include "uieecho.h"
#include "uieerror.h"
#include "uietrack.h"

                                                                                
/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
CONST   SYSCONFIG *pUieSysCfg;                           /* system configuration    */
UCHAR   fchUieActive = (UIE_ACTIVE_GET | UIE_DISPLAY);   /* status of active device */


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieInitialize2(CONST CVTTBL FAR *pCvt,
*                                            CONST FSCTBL FAR *pFsc,
*                                            UCHAR uchMax);
*     Input:    pCvt   - address of conversion table
*               pFsc   - addres of FSC table
*               uchMax - number of mode lock position
*
**  Return:     nothing
*===========================================================================
*/
VOID  UieInitialize2(CONST CVTTBL * *pCvt, CONST FSCTBL * *pFsc, UCHAR uchMax)
{
    pUieSysCfg = PifSysConfig();            /* address of system conf. */

    /* --- create a semaphore --- */
//    usUieSyncNormal = PifCreateSem(0);      /* synchronous - normal    */
	// we are using PifCreateSemNew() for this semaphore since it has the possibility of being
	// released multiple times when processing a control string with the control string pushing
	// characters into the ring buffer.  The max count of 50 is an arbitrary number.
    usUieSyncNormal = PifCreateSemNew(50, 0, __FILE__, __LINE__);      /* synchronous - normal    */
//    usUieSyncError  = PifCreateSem(0);      /* synchronous - error     */
    usUieSyncError = PifCreateSemNew(50, 0, __FILE__, __LINE__);      /* synchronous - error    */
    usUieErrorMsg   = PifCreateSem(1);      /* exclusive - error       */
	
    UieRingBufInit();                       /* initialization          */
    UieDisplayInit();                       /* initialization          */
    UieEchoBackInit();                      /* initialization          */

    UieTracking(UIE_TKEY_POWER, 0, 0);      /* initiale/master reset   */

    /* --- initialization of input device --- */
    UieKeyboardInit(pCvt, pFsc, uchMax);    /* keyboard                */
    UieDrawerInit();                        /* drawer                  */
    UieCoinInit();                          /* coin despensor          */
#if defined(PERIPHERAL_DEVICE_WAITER_LOCK)
#pragma message("Support for waiter lock.")
    UieWaiterInit();                        /* waiter lock             */
#endif
#if defined(PERIPHERAL_DEVICE_SCANNER)
#pragma message("Support for scanner.")
    UieScannerInit();                       /* scanner 1 and 2         */
#endif
#if defined(PERIPHERAL_DEVICE_MSR)
#pragma message("Support for MSR.")
    UieMsrInit();                           /* MSR                     */
#endif
#if defined(PERIPHERAL_DEVICE_SCALE)
#pragma message("Support for scale.")
    UieScaleInit();                         /* scale                   */
#endif
}


/* ======================================= */
/* ========== End of UieInit2.C ========== */
/* ======================================= */
