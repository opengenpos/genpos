/*************************************************************************
 *
 * NCRSERAL.h
 *
 * $Workfile:: NCRSERAL.h                                                $
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
 * $History:: NCRSERAL.h                                                 $
 *
 ************************************************************************/

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//---------------------------------------------------------------------------
//  Ver. |   Date   |         Notes                                 | By
//-------+----------+-----------------------------------------------+--------
//       |          |
/////////////////////////////////////////////////////////////////////////////

#if !defined(_INC_NCRSERAL)
#define _INC_NCRSERAL

/////////////////////////////////////////////////////////////////////////////
// Calling conventions for C++ programs
/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"{
#endif

/////////////////////////////////////////////////////////////////////////////
// Global Definitions
/////////////////////////////////////////////////////////////////////////////

// I/O Control function code

#define     IOCTL_SERIAL_SET_POWER		CTL_CODE(FILE_DEVICE_SERIAL_PORT,   \
												 25,                        \
												 METHOD_BUFFERED,           \
												 FILE_ANY_ACCESS)

// DeviceIoControl(IOCTL_SERIAL_SET_POWER) parameters

#define	SERIAL_POWER_DISABLE	1		// always disabled
#define	SERIAL_POWER_ENABLE		2		// always enabled
#define	SERIAL_POWER_AUTO		3		// judgement is performed in driver

typedef struct _SERIAL_SET_POWER
{
	DWORD	dwMode;						// +12V power supply control mode
} SERIAL_SET_POWER, *PSERIAL_SET_POWER;

/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_NCRSERAL) */
///////////////////////// End Of Header /////////////////////////////////////
