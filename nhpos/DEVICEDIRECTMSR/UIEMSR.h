/*************************************************************************
 *
 * UIESCAN.h
 *
 * $Workfile:: UIEMSR.h                                                 $
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
 * $History:: UIEMSR.h                                                  $
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
*   Function Prototype Declarations
*===========================================================================
*/
BOOL    UieMSRInit(USHORT usPort, USHORT usBaud, UCHAR uchFormat, HANDLE hScale);
BOOL    UieMSRTerm(VOID);
SHORT   UieReadMSR(UIE_MSR *pData);

SHORT   UieMSRClear(VOID);
SHORT   UieMSRAnalysis(UIE_MSR *pData, UCHAR *puchBuffer, SHORT sLength);
SHORT   UieMSRStatus(UCHAR *puchBuffer, SHORT sLength);


#ifdef __cplusplus
}
#endif

/* ==================================== */
/* ========== End of UieIO.C ========== */
/* ==================================== */
