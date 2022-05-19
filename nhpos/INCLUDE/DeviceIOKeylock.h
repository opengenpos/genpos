/*************************************************************************
 *
 * DeviceIOKeylock.h
 *
 * $Workfile:: DeviceIOKeylock.h                                         $
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
 * $History:: DeviceIOKeylock.h                                          $
 *
 ************************************************************************/

#if !defined(_INC_DEVICEIO_KEYLOCK)
#define _INC_DEVICEIO_KEYLOCK

#include "DeviceIo.h"

#ifdef __cplusplus
extern "C"{
#endif


/*
/////////////////////////////////////////////////////////////////////////////
//  Read Function Interface
/////////////////////////////////////////////////////////////////////////////
*/

#define	NCRKLOCK_POSITION_1		 1			// keylock position #1
#define	NCRKLOCK_POSITION_2		 2			// keylock position #2
#define	NCRKLOCK_POSITION_3		 4			// keylock position #3
#define	NCRKLOCK_POSITION_4		 8			// keylock position #4
#define	NCRKLOCK_POSITION_5		16			// keylock position #5

typedef struct _DEVICEIO_KEYLOCK_POSITION
{
	DWORD	dwPosition;						/* keylock position */
} DEVICEIO_KEYLOCK_POSITION, *PDEVICEIO_KEYLOCK_POSITION;


/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_DEVICEIO_KEYLOCK) */
