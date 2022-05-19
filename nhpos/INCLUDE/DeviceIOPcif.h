/*************************************************************************
 *
 * DeviceIOPcif.h
 *
 * $Workfile:: DeviceIOPcif.h                                            $
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
#define SCF_TYPENAME_PCIF                 _T("PCIF")

#define SCF_TYPE_PCIF                0x00010000

/* Capability of MSR I/F */
#define SCF_DATANAME_MODEM           _T("ModemString")


/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_DEVICEIO_SERVERLOCK) */
