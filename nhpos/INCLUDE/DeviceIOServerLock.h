/*************************************************************************
 *
 * DeviceIOServerLock.h
 *
 * $Workfile:: DeviceIOServerLock.h                                      $
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

#if !defined(_INC_DEVICEIO_SERVERLOCK)
#define _INC_DEVICEIO_SERVERLOCK

#include "DeviceIo.h"

#ifdef __cplusplus
extern "C"{
#endif


/*
/////////////////////////////////////////////////////////////////////////////
//  Read Function Interface
/////////////////////////////////////////////////////////////////////////////
*/

typedef struct _DEVICEIO_SERVERLOCK_NUMBER
{
	DWORD	dwNumber;						/* server lock number */
} DEVICEIO_SERVERLOCK_NUMBER, *PDEVICEIO_SERVERLOCK_NUMBER;


/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_DEVICEIO_SERVERLOCK) */
