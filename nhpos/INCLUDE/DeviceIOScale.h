/*************************************************************************
 *
 * DeviceIOScale.h
 *
 * $Workfile:: DeviceIOScale.h                                           $
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
 * $History:: DeviceIOScale.h                                            $
 *
 ************************************************************************/

#if !defined(_INC_DEVICEIO_SCALE)
#define _INC_DEVICEIO_SCALE

#include "DeviceIo.h"

#ifdef __cplusplus
extern "C"{
#endif


/*
/////////////////////////////////////////////////////////////////////////////
//  IoControl Function Interface
/////////////////////////////////////////////////////////////////////////////
*/

/* I/O Control function code */

#define	DEVICEIO_CTL_SCALE_ENABLE		DEVICEIO_SCALE + 1
#define	DEVICEIO_CTL_SCALE_READWEIGHT	DEVICEIO_SCALE + 2	/* sync operation */


/* read a scale weight */

typedef struct _DEVICEIO_SCALE_STATE
{
	BOOL	fStatus;						/* scanner status */
} DEVICEIO_SCALE_STATE, *PDEVICEIO_SCALE_STATE;

/* get scale status */

typedef struct _DEVICEIO_SCALE_DATA
{
	SHORT	sStatus;								/* status         */
	UCHAR	uchUnit;								/* unit           */
	UCHAR	uchDec;									/* precision      */
	ULONG	ulData;									/* data           */
} DEVICEIO_SCALE_DATA, *PDEVICEIO_SCALE_DATA;

/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_DEVICEIO_SCALE) */
