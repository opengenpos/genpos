/*************************************************************************
 *
 * NCRKLOCK.h
 *
 * $Workfile:: NCRKLOCK.h                                                $
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
 * $History:: NCRKLOCK.h                                                 $
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

#if !defined(_INC_NCRKLOCK)
#define _INC_NCRKLOCK

/////////////////////////////////////////////////////////////////////////////
// Calling conventions for C++ programs
/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"{
#endif

/////////////////////////////////////////////////////////////////////////////
//  Keylock Driver interface
/////////////////////////////////////////////////////////////////////////////

#define	FILENAME_KEYLOCK_DEVICE		_T("KLK1:")

/////////////////////////////////////////////////////////////////////////////
//  Read Function Interface
/////////////////////////////////////////////////////////////////////////////

typedef struct _NCRKLOCK_POSITION			// keylock position data
{
	DWORD	dwPosition;							// keylock position
} NCRKLOCK_POSITION, *PNCRKLOCK_POSITION;

#define	NCRKLOCK_POSITION_1		 1			// keylock position #1
#define	NCRKLOCK_POSITION_2		 2			// keylock position #2
#define	NCRKLOCK_POSITION_3		 4			// keylock position #3
#define	NCRKLOCK_POSITION_4		 8			// keylock position #4
#define	NCRKLOCK_POSITION_5		16			// keylock position #5
#define	NCRKLOCK_POSITION_6		32			// keylock position #6

/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_NCRKLOCK) */
///////////////////////// End Of Header /////////////////////////////////////
