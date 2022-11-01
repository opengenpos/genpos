/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlHourly.cpp
//
//  PURPOSE:    Provides Hourly Activity total class.
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
#include "Global.h"

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
//  FUNCTION :  CTtlHourly::CTtlHourly()
//
//  PURPOSE :   Constructor of Hourly Activity total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlHourly::CTtlHourly()
{
	memset( &m_ttlHourly, 0, sizeof( m_ttlHourly ));
    //::ZeroMemory( &m_ttlHourly, sizeof( m_ttlHourly ));
}

//===========================================================================
//
//  FUNCTION :  CTtlHourly::~CTtlHourly()
//
//  PURPOSE :   Destructor of Hourly Activity total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlHourly::~CTtlHourly()
{
}

//===========================================================================
//
//  FUNCTION :  CTtlHourly::Read()
//
//  PURPOSE :   Read Hourly Activity total in 2170.
//
//  RETURN :    TRUE    - Specified total is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlHourly::Read(
    const UCHAR  uchClassToRead // indicates current or saved daily/PTD total
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) ||
           ( uchClassToRead == CLASS_TTLSAVDAY ) ||
           ( uchClassToRead == CLASS_TTLCURPTD ) ||
           ( uchClassToRead == CLASS_TTLSAVPTD ) ||
		   ( uchClassToRead == CLASS_SAVED_TOTAL));


    CString str;
    str.Format( _T("BEGIN >> CTtlHourly::Read()") );
    str += CString( "...Minor#%d" );
    pcsample::TraceFunction( str, uchClassToRead );

	TTLHOURLY  m_ttlWork;
    BOOL    fSuccess = FALSE;
	USHORT usCurReadPos = 0;
	USHORT usPrevReadPos = 0;
	USHORT usHrlyFilePosition = 0;		//file position in the Hourly File on NHPOS side

	/*the file position read from in NHPOS will vary according to the 
	minor class passed in. We need to know this positon to calcute the 
	index postion to assign data to in the buffer below*/
	switch(uchClassToRead)
	{
       case CLASS_TTLCURDAY:
		   usHrlyFilePosition = TTL_HOUR_DAYCURR;
           break;

       case CLASS_TTLSAVDAY:
		   usHrlyFilePosition = TTL_HOUR_DAYCURR + TTL_HOUR_SIZE;
           break;

       case CLASS_TTLCURPTD:
		   usHrlyFilePosition = TTL_HOUR_PTDCURR;
           break;

       case CLASS_TTLSAVPTD:
		   usHrlyFilePosition = TTL_HOUR_PTDCURR + TTL_HOUR_SIZE;
           break;

	   case CLASS_SAVED_TOTAL:
		   usHrlyFilePosition = 0;

	   default:		//if no valid class is passed in, return FALSE. ASSERT above should prevent this point from being hit
		   return FALSE;
	}

	memset(&m_ttlHourly, 0, sizeof(TTLHOURLY));
	m_ttlHourly.uchMajorClass = CLASS_TTLHOURLY;
    m_ttlHourly.uchMinorClass = uchClassToRead;

	memset(&m_ttlWork, 0, sizeof(TTLHOURLY));
    m_ttlWork.uchMajorClass = CLASS_TTLHOURLY;
    m_ttlWork.uchMinorClass = uchClassToRead;
	m_ttlWork.usBlockRead = 0;

    // --- read specified total via base class function ---

	//continue reading until all 48 HOURLY blocks are read
	while(usPrevReadPos < TTL_MAX_BLOCK ){
		fSuccess = CTotal::TotalRead( &m_ttlWork );

		if ( fSuccess )
		{
			//we now just read ONE record at a time, and set the information to the correct
			//bucket of hourly time.  we have removed the dependency on file size,
			//which causes problems when the hourly files increase due to variable additions
			//and become too large for the current data type of the variable JHHJ.
			usCurReadPos = m_ttlWork.usBlockRead;
			//we set the one record that we have just received to the 
			//correct location in the structure
			m_ttlHourly.Total[usPrevReadPos] = m_ttlWork.Total[0];
			usPrevReadPos = usCurReadPos;
		} else
		{
			return fSuccess;
		}

	}
	if ( fSuccess ){
		switch ( uchClassToRead )
		{
		case CLASS_TTLCURDAY:
		case CLASS_TTLCURPTD:

			// --- set current time, if read report is specified ---
			CTotal::TotalGetCurrentDateTime( m_ttlHourly.ToDate );
			m_ttlHourly.uchResetStatus = m_ttlWork.uchResetStatus;
			break;

		default:
			m_ttlHourly.uchResetStatus = m_ttlWork.uchResetStatus;
			break;
		}
	}


	m_ttlHourly.FromDate = m_ttlWork.FromDate;
    pcsample::TraceFunction( _T("END   >> CTtlHourly::Read() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlHourly::Reset()
//
//  PURPOSE :   Reset Hourly Activity total in 2170.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlHourly::Reset(
    const BOOL fIsEODReset,  // indicate EOD reset is selected
	const UCHAR uchMajorClass
    )
{
    ASSERT( fIsEODReset );

    CString str;
    str.Format( _T("BEGIN >> CTtlHourly::Reset()") );
    str += CString( "...At EOD:%d" );
    pcsample::TraceFunction( str, fIsEODReset );

    // --- reset specified total via base class function ---

    m_ttlHourly.uchMajorClass = CLASS_TTLEODRESET;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReset( &m_ttlHourly, pcttl::EOD_HOURLY );

    pcsample::TraceFunction( _T("END   >> CTtlHourly::Read() = %d"), fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlHourly::IsResetOK()
//
//  PURPOSE :   Determine whether this total is able to reset or not.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlHourly::IsResetOK()
{
    CString str;
    str.Format( _T("BEGIN >> CTtlHourly::IsResetOK()") );
    pcsample::TraceFunction( str );

    // --- determine specified total is able to reset via base class ---

    m_ttlHourly.uchMajorClass = CLASS_TTLHOURLY;
    m_ttlHourly.uchMinorClass = CLASS_TTLSAVDAY;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalIsReportIssued( &m_ttlHourly );

    pcsample::TraceFunction( _T("END   >> CTtlHourly::Read() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlHourly::ResetAsOK()
//
//  PURPOSE :   Set this total as able to reset.
//
//  RETURN :    TRUE    - Set specified total as able to reset.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlHourly::ResetAsOK()
{
    CString str;
    str.Format( _T("BEGIN >> CTtlHourly::ResetAsOK()") );
    pcsample::TraceFunction( str );

    // --- set specified total as able to reset via base class ---

    m_ttlHourly.uchMajorClass = CLASS_TTLHOURLY;
    m_ttlHourly.uchMinorClass = CLASS_TTLSAVDAY;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReportAsIssued( &m_ttlHourly );

    pcsample::TraceFunction( _T("END   >> CTtlHourly::Read() = %d"),
                             fSuccess );

    return ( fSuccess );
}

////////////////// END OF FILE ( TtlHourly.cpp ) ////////////////////////////
