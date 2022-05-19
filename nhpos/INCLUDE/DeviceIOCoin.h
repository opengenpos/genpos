/*************************************************************************
 *
 * DeviceIOCoin.h
 *
 * $Workfile:: DeviceIOCoin.h                                            $
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
 * $History:: DeviceIOCoin.h                                             $
 *
 ************************************************************************/

#if !defined(_INC_DEVICEIO_COIN)
#define _INC_DEVICEIO_COIN

#include "DeviceIo.h"

#ifdef __cplusplus
extern "C"{
#endif

#pragma pack(push, 8)


/*
/////////////////////////////////////////////////////////////////////////////
//  Write Function Interface
/////////////////////////////////////////////////////////////////////////////
*/

typedef struct _DEVICEIO_COIN_DISPENSE
{
	LONG	lAmount;						/* dispnese amount */
} DEVICEIO_COIN_DISPENSE, *PDEVICEIO_COIN_DISPENSE;


/*
/////////////////////////////////////////////////////////////////////////////
//  Device Information
/////////////////////////////////////////////////////////////////////////////
*/

typedef struct _DEVICEIO_COIN_INFO
{
	DWORD	dwSize;							/* length of structure     */
	LONG	nMin;							/* minimum dispense amount */
	LONG	nMax;							/* maximum dispense amount */
} DEVICEIO_COIN_INFO, *PDEVICEIO_COIN_INFO;


/*
/////////////////////////////////////////////////////////////////////////////
*/

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_DEVICEIO_COIN) */
