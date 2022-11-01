/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  GuestCheck.cpp
//
//  PURPOSE:    Provides Guest Check class.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//          I N C L U D E    F I L E s
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include "PC2170.h"
#endif
#include "R20_PC2172.h"

#include "Total.h"

/////////////////////////////////////////////////////////////////////////////
//
//          D E B U G    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CGuestCheck::CGuestCheck()
//
//  PURPOSE :   Constructor of Guest Check class.
//
//  RETURN :    No return value.
//
//===========================================================================

CGuestCheck::CGuestCheck() :
    m_sLastError( SUCCESS ),
    m_fLocked( FALSE ),
    m_usLockedGCNo( static_cast< USHORT >( -1 ))
{
	memset( &m_tranGCF, 0, sizeof( m_tranGCF ));
    //::ZeroMemory( &m_tranGCF, sizeof( m_tranGCF ));
}

//===========================================================================
//
//  FUNCTION :  CGuestCheck::~CGuestCheck()
//
//  PURPOSE :   Destructor of Guest Check class.
//
//  RETURN :    No return value.
//
//===========================================================================

CGuestCheck::~CGuestCheck()
{
}

//===========================================================================
//
//  FUNCTION :  CGuestCheck::Read()
//
//  PURPOSE :   Read Guest Check in 2170.
//
//  RETURN :    TRUE    - Specified Guest Check is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CGuestCheck::Read(
    const USHORT usGCNo         // Guest Check# to read
    )
{
    ASSERT(( 0 < usGCNo ) && ( usGCNo <= pcttl::MAX_GC_NO ));

    // --- read guest check in 2170 by guest check# ---

    LPVOID  lpv;
    lpv = static_cast< LPVOID >( &m_tranGCF );

    SHORT   sActualRead;
    sActualRead = ::SerGusIndRead( usGCNo,
                                   static_cast< UCHAR* >( lpv ),
                                   sizeof( m_tranGCF ));

    if ( 0 <= sActualRead )
    {
        if ( 0 == sActualRead )
        {
            // --- no data read because specified record is not setteled ---

			memset( &m_tranGCF, 0,sizeof( m_tranGCF ));
            //::ZeroMemory( &m_tranGCF, sizeof( m_tranGCF ));
            m_tranGCF.usGuestNo = usGCNo;
        }

        // --- function is successful ---

        m_sLastError = GCF_SUCCESS;
    }
    else
    {
        // --- function is failed ---

        m_sLastError = sActualRead;
    }

    return ( m_sLastError == GCF_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CGuestCheck::Reset()
//
//  PURPOSE :   Reset Guest Check in 2170.
//
//  RETURN :    TRUE    - Specified Guest Check is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CGuestCheck::Reset(
    const USHORT usGCNo,         // Guest Check# to reset
	const UCHAR uchMajorClass
    )
{
    ASSERT(( 0 < usGCNo ) && ( usGCNo <= pcttl::MAX_GC_NO ));

    // --- reset (delete) specified guest check in 2170 ---

    m_sLastError = ::SerGusDelete( usGCNo );

    return ( m_sLastError == GCF_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CGuestCheck::GetAllGCNo()
//
//  PURPOSE :   Read all existing Guest Check number in 2170.
//
//  RETURN :    TRUE    - All Guest Check number is successfully read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CGuestCheck::GetAllGCNo(
    USHORT* pausGCNo,           // points to buffer to receive Guest Check#
    const USHORT usBufferSize,  // size of pausGCNo buffer in bytes
    USHORT& usNoOfGC            // no. of Guest Checks in pausGCNo buffer
    )
{
    ASSERT(( sizeof( USHORT ) * GCF_MAX_GCF_NUMBER ) <= usBufferSize );
    ASSERT( ! IsBadWritePtr( pausGCNo, usBufferSize ));

    // --- get all existing Guest Checks in 2170 ---

    SHORT   sActualRead;

	// Get a list of all unpaid guest checks both Counter and Drive Thru.
	// Counter only uses GCF_COUNTER_TYPE type. Counter and Drive Thru uses GCF_DRIVE_THROUGH_STORE.
    sActualRead = ::SerGusAllIdRead(GCF_DRIVE_THROUGH_STORE, pausGCNo, usBufferSize );

    if ( 0 <= sActualRead )
    {
        // --- calculate no. of GC from no. of bytes read value ---
        usNoOfGC = static_cast< USHORT >( sActualRead / sizeof( USHORT ));
        m_sLastError = GCF_SUCCESS;
    }
    else
    {
        // --- function is failed ---
		usNoOfGC = 0;
        m_sLastError = sActualRead;
    }

    return ( m_sLastError == GCF_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CGuestCheck::GetAllGCNo()
//
//  PURPOSE :   Read all Guest Check number opened by specified operator#.
//
//  RETURN :    TRUE    - All Guest Check number is successfully read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CGuestCheck::GetAllGCNoByOperator(
    const ULONG ulOperatorNo,  // operator# to read Guest Checks
    USHORT* pausGCNo,           // points to buffer to receive Guest Check#
    const USHORT usBufferSize,  // size of pausGCNo buffer in bytes
    USHORT& usNoOfGC            // no. of Guest Checks in pausGCNo buffer
    )
{
    ASSERT(( sizeof( USHORT ) * GCF_MAX_GCF_NUMBER ) <= usBufferSize );
    ASSERT( ! IsBadWritePtr( pausGCNo, usBufferSize ));

    // --- get all Guest Checks opened by specified operator ---
    SHORT   sActualRead;

    sActualRead = ::SerGusAllIdReadBW( GCF_COUNTER_TYPE, ulOperatorNo, pausGCNo, usBufferSize );

    if ( 0 <= sActualRead )
    {
        // --- calculate no. of GC from no. of bytes read value ---
        usNoOfGC = static_cast< USHORT >( sActualRead / sizeof( USHORT ));
        m_sLastError = GCF_SUCCESS;
    }
    else
    {
        // --- function is failed ---
		usNoOfGC = 0;
		m_sLastError = sActualRead;
    }

    return ( m_sLastError == GCF_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CGuestCheck::ReadAndLock()
//
//  PURPOSE :   Read Guest Check and lock its record.
//
//  RETURN :    TRUE    - Specified Guest Check is read and locked.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CGuestCheck::ReadAndLock(
    const USHORT usGCNo         // Guest Check# to read and lock
    )
{
    ASSERT(( 0 < usGCNo ) && ( usGCNo <= pcttl::MAX_GC_NO ));

    // --- read and lock guest check in 2170 by specified guest check# ---

    LPVOID  lpv;
    lpv = static_cast< LPVOID >( &m_tranGCF );

    SHORT   sActualRead;
    sActualRead = ::SerGusReadLock( usGCNo,
                                    static_cast< UCHAR* >( lpv ),
                                    sizeof( m_tranGCF ));

    if ( 0 <= sActualRead )
    {
        if ( 0 == sActualRead )
        {
            // --- no data read because specified record is not settled ---

			memset( &m_tranGCF, 0, sizeof( m_tranGCF ));
            //::ZeroMemory( &m_tranGCF, sizeof( m_tranGCF ));
            m_tranGCF.usGuestNo = usGCNo;
        }

        // --- function is successful ---

        m_fLocked      = TRUE;
        m_usLockedGCNo = usGCNo;
        m_sLastError   = GCF_SUCCESS;
    }
    else
    {
        // --- function is failed ---

        m_sLastError = sActualRead;
    }

    return ( m_sLastError == GCF_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CGuestCheck::Lock()
//
//  PURPOSE :   Lock Guest Check in 2170.
//
//  RETURN :    TRUE    - Specified Guest Check is locked.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CGuestCheck::Lock(
    const USHORT usGCNo         // Guest Check# to lock record
    )
{
    ASSERT(( usGCNo == pcttl::LOCK_ALL_GC ) ||
           (( 0 < usGCNo ) && ( usGCNo <= pcttl::MAX_GC_NO )));

    // --- is the Guest Check already locked ? --- 

    if ( m_fLocked )
    {
        // --- yes, it is already locked ---

        if ( usGCNo == m_usLockedGCNo )
        {
            // --- this Guest Check is already locked ---

            m_sLastError = GCF_SUCCESS;
        }
        else
        {
            // --- other Guest Check is locked ---

            m_sLastError = GCF_DUR_LOCKED;
        }
    }
    else
    {
        // --- no, it is not locked ---

        if ( usGCNo == pcttl::LOCK_ALL_GC )
        {
            // --- lock all of Guest Check records in 2170 ---

            m_sLastError = ::SerGusAllLock();
        }
        else
        {
            // --- lock specified Guest Check in 2170 ---

            m_sLastError = ::SerGusReadLock( usGCNo, NULL, 0 );
        }
    }

    if ( m_sLastError == GCF_SUCCESS )
    {
        // --- store locked Guest Check data to member variable ---

        m_fLocked      = TRUE;
        m_usLockedGCNo = usGCNo;
    }

    return ( m_sLastError == GCF_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CGuestCheck::Unlock()
//
//  PURPOSE :   Unlock Guest Check in 2170.
//
//  RETURN :    TRUE    - Specified Guest Check is unlocked.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CGuestCheck::Unlock()
{
    // --- is the Guest Check locked ? ---

    if ( m_fLocked )
    {
        // --- yes, it is locked by User application ---

        if ( m_usLockedGCNo == pcttl::LOCK_ALL_GC )
        {
            // --- unlock all of Guest Check records in 2170 terminal ---

            m_sLastError = ::SerGusAllUnLock();
        }
        else
        {
            // --- unlock the last locked Guest Check record ---

            m_sLastError = ::SerGusResetReadFlag( m_usLockedGCNo );
        }
    }
    else
    {
        // --- no, it is not locked, and nothing to do ---

        m_sLastError = GCF_SUCCESS;
    }

    if ( m_sLastError == GCF_SUCCESS )
    {
        // --- set this class object as no record is locked ---

        m_fLocked      = FALSE;
        m_usLockedGCNo = static_cast< USHORT >( -1 );
    }

    return ( m_sLastError == GCF_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CGuestCheck::CloseOpenedGC()
//
//  PURPOSE :   Close the specified Guest Check in 2170.
//
//  RETURN :    TRUE    - Specified Guest Check is closed
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CGuestCheck::CloseOpenedGC(
    const USHORT usGCNo         // Guest Check# to close
    )
{
    ASSERT(( 0 < usGCNo ) && ( usGCNo <= pcttl::MAX_GC_NO ));

    // --- compulsorily close the specified guest check ---

    m_sLastError = ::SerGusResetReadFlag( usGCNo );

    return ( m_sLastError == GCF_SUCCESS );
}

//===========================================================================
//
//  FUNCTION :  CGuestCheck::IsEmpty()
//
//  PURPOSE :   Determine whether the Guest Check is empty or not.
//
//  RETURN :    TRUE    - Guest Check is empty.
//              FALSE   - Guest Check has some records.
//
//===========================================================================

BOOL CGuestCheck::IsEmpty()
{
    BOOL    fEmpty;

    // --- check the Guest Check has some records ---

    m_sLastError = ::SerGusCheckExist();

    switch ( m_sLastError )
    {
    case GCF_EXIST:

        fEmpty = FALSE;
        break;

    default:

        fEmpty = TRUE;
        break;
    }

    return ( fEmpty );
}

//===========================================================================
//
//  FUNCTION :  CGuestCheck::ResetDeliveryQueue()
//
//  PURPOSE :   Reset the delivery queue of flexible drive through.
//
//  RETURN :    TRUE    - Delivery queue is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CGuestCheck::ResetDeliveryQueue()
{
    // --- reset delivery queue in 2170 ---

    m_sLastError = ::SerGusResetDeliveryQueue();

    return ( m_sLastError == GCF_SUCCESS );
}

////////////////// END OF FILE ( GuesetCheck.cpp ) //////////////////////////
