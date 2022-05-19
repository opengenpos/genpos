/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Intialization Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosInit.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00 by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:     
*
* --------------------------------------------------------------------------
* Update Histories
*
* Data      : Ver.     : Name       : Description
* Feb-23-95 : G0       : O.Nakada   : Initial
* May-08-96 : G5       : O.Nakada   : Changed cursor size.
** NCR2172 **
* Oct-07-99 : 1.00.00  : K.Iwata    : Initial (for 2172)
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
*   Include File Declarations
*===========================================================================
*/
#include <tchar.h>
#include <tchar.h>
#include <string.h>
#include "ecr.h"
#include "pif.h"
#include "log.h"

#define __EventSubSystem
#include "vos.h"

#include "vosl.h"
#include <pif.h>
#include <rfl.h>

#include "EvsL.h"

/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
USHORT       usVosConfig = 0;                         /* VOS configuration    */
PifSemHandle usVosApiSem = PIF_SEM_INVALID_HANDLE;    /* exclusive semaphore  */
VOSVIDEO     VosVideo;                                /* video information    */
USHORT       usVosPage = 0;                           /* video page           */
USHORT       ausVosStack[VOS_NUM_STACK / 2];          /* back light thread    */


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID VosInitialize(USHORT usMinutes);
*     Input:    usMinutes - time of automatic back light off
*
** Return:      previous minites
*===========================================================================
*/
_declspec(dllexport) VOID VosInitialize(USHORT usMinutes)
{
    static  UCHAR   FARCONST  szName[] = "VOSBL";
/*  static  VOID    (THREADENTRY *pFunc)(VOID) = VosThread; ### DEL (2172 for Win32) V1.0 */

    if (usVosConfig & VOS_CONFIG_INIT) {    /* already initialization      */
        return;                             /* exit ...                    */
    }
    
    usVosApiSem = PifCreateSem(1);          /* create a semaphore          */
#ifdef __EVS_DELETE
    if (PifSysConfig()->uchOperType != DISP_LCD) {  /* not LCD             */
        usVosConfig |= VOS_CONFIG_INIT;     /* initialization completed    */
        return;                             /* exit ...                    */
    }
#else
        usVosConfig |= VOS_CONFIG_INIT;     /* initialization completed    */
#endif
    VosVirtualInit();                       /* initialize a virtual memory */
    VosExecInit(VOS_MODE_TEXT4025, usMinutes);

    /* --- create a back light control thread --- */
#ifdef __EVS_DELETE /* ### DEL (2172 for Win32) V1.0 */
    PifBeginThread(pFunc,                   /* start address               */
                   &ausVosStack[sizeof(ausVosStack) / 2], /* stack address */
                   sizeof(ausVosStack),     /* stack size                  */
                   PRIO_VOS,                /* priority                    */
                   szName,                  /* name                        */
                   NULL);
#endif /* __EVS_DELETE */
    usVosConfig |= (VOS_CONFIG_INIT |       /* initialization completed    */
                    VOS_CONFIG_LCD);        /* LCD provide                 */
}


/*
*===========================================================================
** Synopsis:    VOID VosExecInit(USHORT usMode, USHORT usMinutes);
*     Input:    usMode    - video mode
*               usMinutes - time of automatic back light off
*
** Return:      nothing
*===========================================================================
*/
VOID VosExecInit(USHORT usMode, USHORT usMinutes)
{
    memset(&VosVideo, 0, sizeof(VosVideo)); /* initialization */

    /* --- set video mode --- */

    switch (usMode) {
    case VOS_MODE_TEXT4025:                 /* text 46 x 25     */
        EvsVioMode(PIF_VIO_TEXT4025);
        VosVideo.usMode     = usMode;
        VosVideo.Size.usRow = VOS_NUM_ROW25;
		VosVideo.Size.usCol = VOS_NUM_COL46; //number of columns (room for store forward indicator)
//      VosVideo.Size.usCol = VOS_NUM_COL42; //US Customs SCER cwunn
//        VosVideo.Size.usCol = VOS_NUM_COL40; // Saratoga uses VOS_NUM_COL40
        break;

    case VOS_MODE_TEXT8025:                 /* text 80 x 25     */
        EvsVioMode(PIF_VIO_TEXT8025);
        VosVideo.usMode     = usMode;
        VosVideo.Size.usRow = VOS_NUM_ROW25;
        VosVideo.Size.usCol = VOS_NUM_COL80;
        break;

    default:                                /* unsupported mode */
        PifAbort(MODULE_VOS_INITIALIZE, FAULT_INVALID_ARGUMENTS);
		return;
    }

    /* --- set default cursor type --- */

    EvsVioCurOnOff(PIF_VIO_OFF, NULL);
    EvsVioCurType(1, 14, NULL);

    /* --- set default cursor position --- */

    EvsVioSetCurPos(0, 0, usVosPage, NULL);
    VosVideo.Cursor.usRow = 0;
    VosVideo.Cursor.usCol = 0;

    /* --- update state of back light --- */

    VosVideo.usMinutes = usMinutes;
    VosVideo.usState   = VOS_STATE_BLIGHT | /* back light already on */
                         VOS_STATE_CHANGE;  /* notify to thread      */
}


/* ====================================== */
/* ========== End of VosInit.C ========== */
/* ====================================== */
