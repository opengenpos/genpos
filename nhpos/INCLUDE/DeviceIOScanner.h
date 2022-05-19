/*************************************************************************
 *
 * DeviceIOScanner.h
 *
 * $Workfile:: DeviceIOScanner.h                                         $
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
 * $History:: DeviceIOScanner.h                                          $
 *
 ************************************************************************/

#if !defined(_INC_DEVICEIO_SCANNER)
#define _INC_DEVICEIO_SCANNER

#include "DeviceIo.h"

#ifdef __cplusplus
extern "C"{
#endif


/*
/////////////////////////////////////////////////////////////////////////////
//  IoControl Function Interface
/////////////////////////////////////////////////////////////////////////////
*/

#define	DEVICEIO_CTL_SCANNER_ENABLE			DEVICEIO_SCANNER + 1
#define	DEVICEIO_CTL_SCANNER_NOTONFILE		DEVICEIO_SCANNER + 2


/* control scanner */

typedef struct _DEVICEIO_SCANNER_STATE
{
	BOOL	fStatus;						/* scanner status */
} DEVICEIO_SCANNER_STATE, *PDEVICEIO_SCANNER_STATE;


/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_DEVICEIO_SCANNER) */
