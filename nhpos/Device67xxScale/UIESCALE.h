/*************************************************************************
 *
 * UIESCAN.h
 *
 * $Workfile:: UIESCAN.h                                                 $
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
 * $History:: UIESCAN.h                                                  $
 *
 * Oct-27-15  GenPOS 2.2.1 R.Chambers  Get rid of old defines and other cruft,
*                                     move as much as possible into specific device
*                                     interface source files.
************************************************************************/

#ifdef __cplusplus
extern "C"{
#endif

/*
*===========================================================================
*   Public Define Declarations
*===========================================================================
*/

#include <uie.h>

/*
*===========================================================================
*   Scale Interface
*===========================================================================
*/
#define UIE_SCALE_IN_MOTION     0x01        /* scale in motion       */
#define UIE_SCALE_ZERO          0x02        /* zero weight           */
#define UIE_SCALE_UNDER         0x01        /* under capacity        */
#define UIE_SCALE_OVER          0x02        /* over capacity         */


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
BOOL    UieScaleInit(USHORT usPort, USHORT usBaud, UCHAR uchFormat, HANDLE hScale);
BOOL    UieScaleTerm(VOID);
SHORT   UieReadScale(UIE_SCALE *pData);


#ifdef __cplusplus
}
#endif

/* ==================================== */
/* ========== End of UieIO.C ========== */
/* ==================================== */
