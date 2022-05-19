/*************************************************************************
 *
 * DeviceIOMSR.h
 *
 * $Workfile:: DeviceIOMSR.h                                             $
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
 * $History:: DeviceIOMSR.h                                              $
 *
 ************************************************************************/

#if !defined(_INC_DEVICEIO_MSR)
#define _INC_DEVICEIO_MSR

#include "DeviceIo.h"

#ifdef __cplusplus
extern "C"{
#endif


/*
/////////////////////////////////////////////////////////////////////////////
//  Read Function Interface
/////////////////////////////////////////////////////////////////////////////
*/

#define	DEVICEIO_MSR_TRACK1			(96)
#define	DEVICEIO_MSR_TRACK2			(40)
#define	DEVICEIO_MSR_TRACK3			(104)

typedef struct _DEVICEIO_MSR_DATA
{
	SHORT	sStatus1;						/* ISO Track 1 status   */
	SHORT	sLength1;						/* ISO Track 1 length   */
	TCHAR	uchTrack1[DEVICEIO_MSR_TRACK1];	/* ISO Track 1 data     */
	SHORT	sStatus2;						/* ISO Track 2 status   */
	SHORT	sLength2;						/* ISO Track 2 length   */
	TCHAR	uchTrack2[DEVICEIO_MSR_TRACK2];	/* ISO Track 2 data     */
	SHORT	sStatus3;						/* JIS-II status        */
	SHORT	sLength3;						/* JIS-II length        */
	TCHAR	uchTrack3[DEVICEIO_MSR_TRACK3];	/* JIS-II data          */
} DEVICEIO_MSR_DATA, *PDEVICEIO_MSR_DATA;

/* for MSR data status */

#define	DEVICEIO_MSR_SUCCESS		0		/* sorrectly converted  */
#define	DEVICEIO_MSR_START_ERROR	1		/* start sentinel error */
#define	DEVICEIO_MSR_PARITY_ERROR	2		/* parity error         */
#define	DEVICEIO_MSR_END_ERROR		3		/* end sentinel error   */
#define	DEVICEIO_MSR_LRC_ERROR		4		/* LRC errror           */
#define	DEVICEIO_MSR_BUFFER_SMALL	5		/* not enough buffer    */
#define	DEVICEIO_MSR_GENERIC_ERROR	6		/* generic error        */
#define	DEVICEIO_MSR_NO_DATA		999		/* no data              */

#define	DEVICEIO_CTL_MSR_ENABLE        (DEVICEIO_MSR + 1)
#define	DEVICEIO_CTL_MSR_READTRACKS    (DEVICEIO_MSR + 2)	/* sync operation */
/*
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_DEVICEIO_MSR) */
