/*************************************************************************
 *
 * NCRSPKER.h
 *
 * $Workfile:: NCRSPKER.h                                                $
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
 * $History:: NCRSPKER.h                                                 $
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

#if !defined(_INC_NCRSPKER)
#define _INC_NCRSPKER

/////////////////////////////////////////////////////////////////////////////
// Calling conventions for C++ programs
/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"{
#endif

/////////////////////////////////////////////////////////////////////////////
// Device IO Control Code definition
/////////////////////////////////////////////////////////////////////////////

#ifndef CTL_CODE
# error Please include WINIOCTL.H or DEVIOCTL.H beforehand
#endif

/////////////////////////////////////////////////////////////////////////////
//  Speaker Driver interface
/////////////////////////////////////////////////////////////////////////////

#define	FILENAME_SPEAKER_DEVICE		_T("SPK1:")

/////////////////////////////////////////////////////////////////////////////
//  IoControl Function Interface
/////////////////////////////////////////////////////////////////////////////

// I/O Control function code

#define	IOCTL_NCRSPEAKER_PLAY_SOUND		CTL_CODE(FILE_DEVICE_UNKNOWN,   \
												 0x800,                 \
												 METHOD_BUFFERED,       \
												 FILE_ANY_ACCESS)

#define	IOCTL_NCRSPEAKER_CANCEL_SOUND	CTL_CODE(FILE_DEVICE_UNKNOWN,   \
												 0x802,                 \
												 METHOD_BUFFERED,       \
												 FILE_ANY_ACCESS)

#define	IOCTL_NCRSPEAKER_ADJUST_VOLUME	CTL_CODE(FILE_DEVICE_UNKNOWN,   \
												 0x804,                 \
												 METHOD_BUFFERED,       \
												 FILE_ANY_ACCESS)

#define	IOCTL_NCRSPEAKER_QUERY_VOLUME	CTL_CODE(FILE_DEVICE_UNKNOWN,   \
												 0x806,                 \
												 METHOD_BUFFERED,       \
												 FILE_ANY_ACCESS)

// parameter for IOCTL_NCRSPEAKER_SOUND_NOTE

typedef struct _SOUND_NOTE				// sound note
{
	DWORD		dwDuration;					// duration in msec.
	DWORD		dwFrequency;				// frequency
} SOUND_NOTE, *PSOUND_NOTE;

// paramter for IOCTL_NCRSPEAKER_ADJUST_VOLUME & IOCTL_NCRSPEAKER_SOUND_NOTE

typedef struct _SPEAKER_VOLUME			// speaker volume
{
	DWORD		dwVolume;					// volume
} SPEAKER_VOLUME, *PSPEAKER_VOLUME;

/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_NCRSPKER) */
///////////////////////// End Of Header /////////////////////////////////////
