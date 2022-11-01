/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlPlu.cpp
//
//  PURPOSE:    Provides PLU total class.
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

#define PLUMINNO	"0"
#define PLUMAXNO	"9999999999999"

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CTtlPlu::CTtlPlu()
//
//  PURPOSE :   Constructor of PLU total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlPlu::CTtlPlu() : m_First(TRUE)
{
    memset( m_ttlPlu, 0, sizeof( m_ttlPlu ));
	memset( &m_ttlPluEx,0,sizeof( m_ttlPluEx));
    //::ZeroMemory( m_ttlPlu,  sizeof( m_ttlPlu ));
}

//===========================================================================
//
//  FUNCTION :  CTtlPlu::~CTtlPlu()
//
//  PURPOSE :   Destructor of PLU total class.
//
//  RETURN :    No return value.
//
//===========================================================================

CTtlPlu::~CTtlPlu()
{
}

//===========================================================================
//
//  FUNCTION :  CTtlPlu::Read()
//
//  PURPOSE :   Read PLU total in 2170.
//
//  RETURN :    TRUE    - Specified total is read.
//              FALSE   - Function is failed.
//
//===========================================================================
// ###DEL Saratoga
//BOOL CTtlPlu::Read(
//    const UCHAR  uchClassToRead,// indicates current or saved daily/PTD total
//    const USHORT usPluNo,       // PLU# to read PLU total
//    const UCHAR  uchAdjectiveNo // Adjective# to read PLU total
//    )
//
BOOL CTtlPlu::Read(
    const UCHAR  uchClassToRead,// indicates current or saved daily/PTD total
    const TCHAR* uchPluNo,      // PLU# to read PLU total
    const UCHAR  uchAdjectiveNo // Adjective# to read PLU total
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) ||
           ( uchClassToRead == CLASS_TTLSAVDAY ) ||
           ( uchClassToRead == CLASS_TTLCURPTD ) ||
           ( uchClassToRead == CLASS_TTLSAVPTD ));

	TCHAR	uchPluTemp[OP_PLU_LEN + 1] = { 0 };

	USHORT usCnt = 0;
	for (USHORT usI = 0; usI < OP_PLU_LEN; usI++)
	{
		if ((uchPluNo[usI] != 0x00) && (uchPluNo[usI] != 0x20))
		{
			uchPluTemp[usCnt] = uchPluNo[usI];
			usCnt++;
		}
	}
	if(usCnt == 0)
		uchPluTemp[0] = '0';

	ASSERT((memcmp(uchPluTemp, PLUMINNO, OP_PLU_LEN) >  0) && 
		   (memcmp(uchPluTemp, PLUMAXNO, OP_PLU_LEN) <= 0));

// ###DEL Saratoga
//    ASSERT(( 0 < usPluNo ) && ( usPluNo <= MAX_PLU_NO ));
//
    ASSERT(( 0 < uchAdjectiveNo ) && ( uchAdjectiveNo <= MAX_ADJECTIVE_NO ));

    CString str;
    str.Format( _T("BEGIN >> CTtlPlu::Read()") );
    str += CString( "...Minor#%d, PLU#%d, Adjective#%d" );
// ###DEL Saratoga
//    pcsample::TraceFunction( str, uchClassToRead, usPluNo, uchAdjectiveNo );
//
	pcsample::TraceFunction( str, uchClassToRead, uchPluNo, uchAdjectiveNo );

    BOOL    fSuccess;

    // --- read specified total via base class function ---

    m_ttlPlu[ 0 ].uchMajorClass  = CLASS_TTLPLU;
    m_ttlPlu[ 0 ].uchMinorClass  = uchClassToRead;
	memcpy(m_ttlPlu[0].auchPLUNumber, uchPluNo, sizeof(m_ttlPlu[0].auchPLUNumber));

    m_ttlPlu[ 0 ].uchAdjectNo    = uchAdjectiveNo;

    fSuccess = CTotal::TotalRead( &m_ttlPlu[ 0 ] );
    if ( fSuccess )
    {
        switch ( uchClassToRead )
        {
        case CLASS_TTLCURDAY:
        case CLASS_TTLCURPTD:
            // --- set current time, if read report is specified ---
            CTotal::TotalGetCurrentDateTime( m_ttlPlu[ 0 ].ToDate );
            break;

        default:
            break;
        }
    }

    pcsample::TraceFunction( _T("END   >> CTtlPlu::Read() = %d"), fSuccess );
    return ( fSuccess );
}

BOOL CTtlPlu::Read(
    const UCHAR  uchClassToRead,// indicates current or saved daily/PTD total
    const USHORT usPluNo,       // PLU# to read PLU total
    const UCHAR  uchAdjectiveNo // Adjective# to read PLU total
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) ||
           ( uchClassToRead == CLASS_TTLSAVDAY ) ||
           ( uchClassToRead == CLASS_TTLCURPTD ) ||
           ( uchClassToRead == CLASS_TTLSAVPTD ));

    ASSERT(( 0 < usPluNo ) && ( usPluNo <= MAX_PLU_NO ));
    ASSERT(( 0 < uchAdjectiveNo ) && ( uchAdjectiveNo <= MAX_ADJECTIVE_NO ));

    CString str;
    str.Format( _T("BEGIN >> CTtlPlu::Read()") );
    str += CString( "...Minor#%d, PLU#%d, Adjective#%d" );
    pcsample::TraceFunction( str, uchClassToRead, usPluNo, uchAdjectiveNo );
	pcsample::TraceFunction( str, uchClassToRead, usPluNo, uchAdjectiveNo );

    // --- read specified total via base class function ---
    m_ttlPlu[ 0 ].uchMajorClass  = CLASS_TTLPLU;
    m_ttlPlu[ 0 ].uchMinorClass  = uchClassToRead;

	TCHAR	uchPluTemp[OP_PLU_LEN + 1] = { 0 };

	_itot (usPluNo, uchPluTemp, 10);
	memcpy(m_ttlPlu[0].auchPLUNumber, uchPluTemp, sizeof(m_ttlPlu[0].auchPLUNumber));

    m_ttlPlu[ 0 ].uchAdjectNo    = uchAdjectiveNo;

    BOOL    fSuccess = CTotal::TotalRead( &m_ttlPlu[ 0 ] );

    if ( fSuccess )
    {
        switch ( uchClassToRead )
        {
        case CLASS_TTLCURDAY:
        case CLASS_TTLCURPTD:
            // --- set current time, if read report is specified ---
            CTotal::TotalGetCurrentDateTime( m_ttlPlu[ 0 ].ToDate );
            break;

        default:
            break;
        }
    }

    pcsample::TraceFunction( _T("END   >> CTtlPlu::Read() = %d"), fSuccess );

    return ( fSuccess );
	
}

//===========================================================================
//
//  FUNCTION :  CTtlPlu::Reset()
//
//  PURPOSE :   Reset PLU total in 2170.
//
//  RETURN :    TRUE    - Specified total is resetted.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlPlu::Reset(
    const BOOL fIsEODReset,      // indicate EOD reset is selected
	const UCHAR uchMajorClass
    )
{
    CString str;
    str.Format( _T("BEGIN >> CTtlPlu::Reset()") );
    str += CString( "...At EOD:%d" );
    pcsample::TraceFunction( str, fIsEODReset );

    USHORT  usResetType;

    // --- reset specified total via base class function ---

    if ( fIsEODReset )
    {
        m_ttlPlu[ 0 ].uchMajorClass = CLASS_TTLEODRESET;
        m_ttlPlu[ 0 ].uchMinorClass = CLASS_TTLCURDAY;
        usResetType = pcttl::EOD_PLU;
    }
    else
    {
        m_ttlPlu[ 0 ].uchMajorClass = CLASS_TTLPLU;
        m_ttlPlu[ 0 ].uchMinorClass = CLASS_TTLCURDAY;
        usResetType = pcttl::NOT_EOD;
    }

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReset( &m_ttlPlu[ 0 ], usResetType );

    pcsample::TraceFunction( _T("END   >> CTtlPlu::Reset() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlPlu::IsResetOK()
//
//  PURPOSE :   Determine whether this total is able to reset or not.
//
//  RETURN :    TRUE    - This total is able to reset.
//              FALSE   - This total is not able to reset.
//
//===========================================================================

BOOL CTtlPlu::IsResetOK()
{
    CString str;
    str.Format( _T("BEGIN >> CTtlPlu::IsResetOK()") );
    pcsample::TraceFunction( str );

    // --- determine specified total is able to reset via base class ---

    m_ttlPlu[ 0 ].uchMajorClass  = CLASS_TTLPLU;
    m_ttlPlu[ 0 ].uchMinorClass  = CLASS_TTLSAVDAY;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalIsReportIssued( &m_ttlPlu[ 0 ] );

    pcsample::TraceFunction( _T("END   >> CTtlPlu::IsResetOK() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlPlu::ResetAsOK()
//
//  PURPOSE :   Set this total as able to reset.
//
//  RETURN :    TRUE    - Set specified total as able to reset.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlPlu::ResetAsOK()
{
    CString str;
    str.Format( _T("BEGIN >> CTtlPlu::ResetAsOK()") );
    pcsample::TraceFunction( str );

    // --- set specified total as able to reset via base class ---

    m_ttlPlu[ 0 ].uchMajorClass    = CLASS_TTLPLU;
    m_ttlPlu[ 0 ].uchMinorClass    = CLASS_TTLSAVDAY;

    BOOL    fSuccess;
    fSuccess = CTotal::TotalReportAsIssued( &m_ttlPlu[ 0 ] );

    pcsample::TraceFunction( _T("END   >> CTtlDept::IsResetOK() = %d"),
                             fSuccess );

    return ( fSuccess );
}
//===========================================================================
//
//  FUNCTION :  CTtlPlu::ReadEx()
//
//  PURPOSE :   Read the plural totals in one function call.
//
//  RETURN :    TRUE    - Plural totals are read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlPlu::ReadEx(
    const UCHAR  uchClassToRead// indicates current or saved daily/PTD total
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) ||
           ( uchClassToRead == CLASS_TTLSAVDAY ) ||
           ( uchClassToRead == CLASS_TTLCURPTD ) ||
           ( uchClassToRead == CLASS_TTLSAVPTD ) ||
		   ( uchClassToRead == CLASS_SAVED_TOTAL));

    CString str;
	BOOL fSuccess = FALSE;
    str.Format( _T("BEGIN >> CTtlPlu::ReadPlural()") );
    str += CString( "...Minor#%d" );
    pcsample::TraceFunction( str, uchClassToRead );

	memset( &m_ttlPluEx, 0, sizeof( m_ttlPluEx ));

	m_ttlPluEx.uchMajorClass = CLASS_TTLPLU;
	m_ttlPluEx.uchMinorClass = uchClassToRead;

    // --- read the plural totals in one function call ---
	fSuccess = CTotal::TotalReadPluEx( &m_ttlPluEx );

    pcsample::TraceFunction( _T("END   >> CTtlPlu::ReadEx() = %d"),
                             fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CTtlPlu::ReadPlural()
//
//  PURPOSE :   Read the plural totals in one function call.
//
//  RETURN :    TRUE    - Plural totals are read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CTtlPlu::ReadPlural(
    const UCHAR  uchClassToRead,// indicates current or saved daily/PTD total
    const TCHAR  uchPluNo[][OP_PLU_LEN],     // array of PLU# to read total
    const UCHAR  uchAdjectiveNo[]   // array of ajective# to read total
    )
{
    ASSERT(( uchClassToRead == CLASS_TTLCURDAY ) ||
           ( uchClassToRead == CLASS_TTLSAVDAY ) ||
           ( uchClassToRead == CLASS_TTLCURPTD ) ||
           ( uchClassToRead == CLASS_TTLSAVPTD ));

    CString str;
    str.Format( _T("BEGIN >> CTtlPlu::ReadPlural()") );
    str += CString( "...Minor#%d" );
    pcsample::TraceFunction( str, uchClassToRead );

    // --- prepare PLU# and adjective# to read total ---

    UINT unI, z;

	memset( &m_ttlPlu, 0, sizeof( m_ttlPlu ));
    //::ZeroMemory( &m_ttlPlu, sizeof( m_ttlPlu ));

    for ( unI = 0; unI < CLI_PLU_MAX_NO; unI++ )
    {
// ###DEL Saratoga
//        ASSERT( usPluNo[ unI ] <= MAX_PLU_NO );

		ASSERT( memcmp(&uchPluNo[unI], PLUMAXNO, OP_PLU_LEN) <= 0);
        ASSERT( uchAdjectiveNo[ unI ] <= MAX_ADJECTIVE_NO );

        m_ttlPlu[ unI ].uchMajorClass = CLASS_TTLPLU;
        m_ttlPlu[ unI ].uchMinorClass = uchClassToRead;
// ###DEL Saratoga
//        m_ttlPlu[ unI ].usPLUNumber   = usPluNo[ unI ];
//
		for(z = 0;z< STD_PLU_NUMBER_LEN;z++){
			m_ttlPlu[unI].auchPLUNumber[z] = uchPluNo[unI][z];
		}
		/*_tcsncpy(m_ttlPlu[unI].auchPLUNumber, 
			   uchPluNo[unI], sizeof(uchPluNo[unI]));*/
        m_ttlPlu[ unI ].uchAdjectNo   = uchAdjectiveNo[ unI ];
    }

    BOOL    fSuccess;

    // --- read the plural totals in one function call ---

    fSuccess = CTotal::TotalReadPlural( &m_ttlPlu[ 0 ] );

    if ( fSuccess )
    {
        switch ( uchClassToRead )
        {
        case CLASS_TTLCURDAY:
        case CLASS_TTLCURPTD:

            // --- set current time, if read report is specified ---

            for ( unI = 0; unI < CLI_PLU_MAX_NO; unI++ )
            {
                CTotal::TotalGetCurrentDateTime( m_ttlPlu[ unI ].ToDate );
            }
            break;

        default:
            break;
        }
    }

    pcsample::TraceFunction( _T("END   >> CTtlDept::ReadPlural() = %d"),
                             fSuccess );

    return ( fSuccess );
}

///////////////// END OF FILE ( TtlPlu.cpp ) /////////////////////////////