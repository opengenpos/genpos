/*************************************************************************
 *
 * DeviceIOCashDrawer.h
 *
 * $Workfile:: DeviceIOCashDrawer.h                                      $
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
 * $History:: DeviceIOServerLock.h                                       $
 *
 ************************************************************************/

#if !defined(_INC_DEVICEIO_CASHDRAWER)
#define _INC_DEVICEIO_CASHDRAWER

#include "DeviceIo.h"

#ifdef __cplusplus
extern "C"{
#endif


/*
/////////////////////////////////////////////////////////////////////////////
//  IoControl Function Interface
/////////////////////////////////////////////////////////////////////////////
*/

#define	DEVICEIO_CTL_DRAWER_OPEN		DEVICEIO_DRAWER + 1
#define	DEVICEIO_CTL_DRAWER_GET_STATUS	DEVICEIO_DRAWER + 2


/* open a drawer */

typedef struct _DEVICEIO_DRAWER_OPEN
{
	DWORD	dwNumber;							/* drawer ID */
} DEVICEIO_DRAWER_OPEN, *PDEVICEIO_DRAWER_OPEN;

#define	DEVICEIO_DRAWER_OPEN_A			0x0001	/* drawer A is open */
#define	DEVICEIO_DRAWER_OPEN_B			0x0002	/* drawer B is open */

/* get status of drawers */

typedef struct _DEVICEIO_DRAWER_STATUS
{
	DWORD	dwDrawerA;							/* status of drawer A */
	DWORD	dwDrawerB;							/* status of drawer B */
} DEVICEIO_DRAWER_STATUS, *PDEVICEIO_DRAWER_STATUS;

#define	DEVICEIO_DRAWER_OPENED			0x0001	/* drawer is opened       */
#define	DEVICEIO_DRAWER_BEING_OPENED	0x0002	/* drawer is being opened */
#define	DEVICEIO_DRAWER_CLOSED			0x0004	/* drawer is closed       */


/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_DEVICEIO_CASHDRAWER) */
