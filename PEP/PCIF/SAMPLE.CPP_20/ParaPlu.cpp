/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  ParaPlu.cpp
//
//  PURPOSE:    Plu Parameter class definitions and declarations.
//
//  AUTHOR:
//
//      Ver 1.00.01 : 1999-07-12 : Correct PLU Polling problem on ReadPlural()
//                                 and Read PluralbyDept()
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

#include "Parameter.h"
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
//  FUNCTION :  CParaPlu::CParaPlu()
//
//  PURPOSE :   Constructor of Plu Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaPlu::CParaPlu() :
    m_sLastError( OP_PERFORM ),
    m_ulPluralCounter( 0 )
{
	memset( m_paraPlu, 0, sizeof( m_paraPlu       ));
    memset( m_paraPluByDept, 0, sizeof( m_paraPluByDept ));
	m_usPluralAdjCo   = 0;	// ADD Saratoga
	m_husPluralHandle = 0;	// ADD Saratoga
}

//===========================================================================
//
//  FUNCTION :  CParaPlu::~CParaPlu()
//
//  PURPOSE :   Destructor of Plu Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaPlu::~CParaPlu()
{
}

//===========================================================================
//
//  FUNCTION :  CParaPlu::Initialize()
//
//  PURPOSE :   Initialize all of member variables in PLU parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

VOID CParaPlu::Initialize()
{
    pcsample::TraceFunction( _T("BEGIN >> CParaPlu::Initialize()") );

    m_sLastError      = OP_PERFORM;
    m_ulPluralCounter = 0;
	m_usPluralAdjCo = 0;		// ADD Saratoga
	m_husPluralHandle = 0;		// ADD Saratoga
	memset(m_paraPlu, 0, sizeof(m_paraPlu));
    memset(m_paraPluByDept, 0, sizeof(m_paraPluByDept));

    pcsample::TraceFunction( _T("END   >> CParaPlu::Initialize() = VOID") );
}

//===========================================================================
//
//  FUNCTION :  CParaPlu::Read()
//
//  PURPOSE :   Read the PLU parameter by PLU# and adjectiv#.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

//BOOL CParaPlu::Read(             // ### DEL Saratoga
//    const USHORT usPluNo,       // PLU# to read parameter
//    const UCHAR  uchAdjectiveNo // adjective# to read parameter
//    )
BOOL CParaPlu::Read(
    const TCHAR  uchPluNo,       // PLU# to read parameter
    const UCHAR  uchAdjectiveNo  // adjective# to read parameter
	)
{
    pcsample::TraceFunction( _T("BEGIN >> CParaPlu::Read()") );

    // --- get PLU parameter by the specified PLU# and adjective# ---
    _tcsncpy(m_paraPlu[0].auchPluNo, &uchPluNo, OP_PLU_LEN);
    m_paraPlu[ 0 ].uchPluAdj = uchAdjectiveNo;

    m_sLastError = ::CliOpPluIndRead( &m_paraPlu[ 0 ], 0 );

    CString str;
    str.Format( _T("\t::CliOpPluIndRead() - Called (%d)"), m_sLastError );
    str += CString( "...PLU#%d, Adjctive#%d" );
    pcsample::TraceFunction( str, uchPluNo, uchAdjectiveNo );

    pcsample::TraceFunction( _T("END   >> CParaPlu::Read() = %d"), ( m_sLastError == OP_PERFORM ));

    return ( m_sLastError == OP_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CParaPlu::ReadPlural()
//
//  PURPOSE :   Read the plural PLU parameters.
//
//  RETURN :    TRUE    - PLU parameters are read.
//              FALSE   - Function is failed.
//
//===========================================================================
BOOL CParaPlu::ReadPlural(
    USHORT& usNoOfRead          // variable to store no. of read records
    )
{
	TCHAR	uchPluTemp[OP_PLU_LEN];  // ###ADD Saratoga

    pcsample::TraceFunction( _T("BEGIN >> CParaPlu::ReadPlural()") );

	memset( m_paraPlu, 0, sizeof( m_paraPlu ));

    // --- get read counter updated by previous read function call ---
    m_paraPlu[ 0 ].ulCounter = m_ulPluralCounter;
	m_paraPlu[ 0 ].usAdjCo   = m_usPluralAdjCo;		// ADD Saratoga
	m_paraPlu[ 0 ].husHandle = m_husPluralHandle;	// ADD Saratoga

    // --- get plural parameters in one PC Interface API call ---
    m_sLastError = ::SerOpPluAllRead( &m_paraPlu[ 0 ], 0 );

    CString str;
    str.Format( _T("\t::SerOpPluAllRead() - Called (%d)"), m_sLastError );
    str += CString( "...Counter#%ld" );
    pcsample::TraceFunction( str, m_ulPluralCounter );
    pcsample::TraceFunction( str, (ULONG)m_usPluralAdjCo );
    pcsample::TraceFunction( str, (ULONG)m_husPluralHandle );

    if ( m_sLastError == OP_PERFORM )
    {
        // --- calculate no. of read records in this function call ---
        usNoOfRead = 0;

		while( usNoOfRead < CLI_PLU_MAX_NO )
		{
			USHORT usCnt = 0;

			memset(uchPluTemp, 0x00, sizeof(uchPluTemp));
			for (USHORT usI = 0; usI< OP_PLU_LEN; usI++)
			{
				if ((m_paraPlu[usNoOfRead].auchPluNo[usI] != 0x00) &&
					(m_paraPlu[usNoOfRead].auchPluNo[usI] != 0x20))
				{
					uchPluTemp[usCnt] = m_paraPlu[usNoOfRead].auchPluNo[usI];
					usCnt++;
				}
			}

			if(usCnt == 0)
				break;
			else
				usNoOfRead++;
		}

        // --- update read counter for next read function call ---
        m_ulPluralCounter = m_paraPlu[ CLI_PLU_MAX_NO -1 ].ulCounter;
		m_usPluralAdjCo   = m_paraPlu[CLI_PLU_MAX_NO - 1].usAdjCo;		// ADD Saratoga
		m_husPluralHandle = m_paraPlu[CLI_PLU_MAX_NO - 1].husHandle;	// ADD Saratoga

        pcsample::TraceFunction( _T("\t::SerOpPluAllRead() - No Of Read (%d)"), usNoOfRead );
    }

    pcsample::TraceFunction( _T("END   >> CParaPlu::ReadPlural() = %d"), ( m_sLastError == OP_PERFORM ));
    return ( m_sLastError == OP_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CParaPlu::ReadPluralByDept()
//
//  PURPOSE :   Read the plural PLU parameters by department#.
//
//  RETURN :    TRUE    - PLU parameters are read.
//              FALSE   - Function is failed.
//
//===========================================================================
// ###DEL Saratoga
//BOOL CParaPlu::ReadPluralByDept(
//    const UCHAR uchDeptNo,      // department# to read PLU parameter
//    USHORT& usNoOfRead          // variable to store no. of read records
//    )
BOOL CParaPlu::ReadPluralByDept(
    const USHORT usDeptNo,      // department# to read PLU parameter
    USHORT& usNoOfRead          // variable to store no. of read records
    )
{
	TCHAR	uchPluTemp[OP_PLU_LEN];
    ASSERT(( 0 < usDeptNo ) && ( usDeptNo <= OP_MAX_DEPT_NO ));

    pcsample::TraceFunction( _T("BEGIN >> CParaPlu::ReadPluralByDept()") );

    memset( m_paraPlu, 0, sizeof( m_paraPlu       ));
    memset( m_paraPluByDept, 0, sizeof( m_paraPluByDept ));

    // --- store target department# ---
    m_paraPluByDept[ 0 ].usDept    = usDeptNo;
	
    // --- get read counter updated by previous read function call ---
    m_paraPluByDept[ 0 ].culCounter = m_ulPluralCounter;
	m_paraPluByDept[ 0 ].usAdjCo    = m_usPluralAdjCo;		// ADD Saratoga
	m_paraPluByDept[ 0 ].husHandle  = m_husPluralHandle;	// ADD Saratoga

    // --- get plural parameters in one PC Interface API call ---
    m_sLastError = ::SerOpDeptPluRead( &m_paraPluByDept[ 0 ], 0 ); 

    CString str;
    str.Format( _T("\t::SerOpPluAllReadByDept() - Called (%d)"), m_sLastError );
    str += CString( "...Dept#%d, Counter#%ld" );

	pcsample::TraceFunction( str, usDeptNo, m_ulPluralCounter );
    pcsample::TraceFunction( str, usDeptNo, (ULONG)m_usPluralAdjCo );
    pcsample::TraceFunction( str, usDeptNo, (ULONG)m_husPluralHandle );

    if ( m_sLastError == OP_PERFORM )
    {
        // --- calculate no. of read records in this function call ---
        usNoOfRead = 0;
		while( usNoOfRead < CLI_PLU_MAX_NO )
		{
			USHORT usCnt = 0;

			memset(uchPluTemp, 0x00, sizeof(uchPluTemp));

			for (USHORT usI = 0; usI< OP_PLU_LEN; usI++)
			{
				if ((m_paraPluByDept[usNoOfRead].auchPluNo[usI] != 0x00) &&
					(m_paraPluByDept[usNoOfRead].auchPluNo[usI] != 0x20))
				{
					uchPluTemp[usCnt] = m_paraPlu[usNoOfRead].auchPluNo[usI];
					usCnt++;
				}
			}

			if(usCnt == 0)
				break;
			else
			{
				_tcsncpy(m_paraPlu[usNoOfRead].auchPluNo, m_paraPluByDept[usNoOfRead].auchPluNo, sizeof(m_paraPluByDept[usNoOfRead].auchPluNo));
	            m_paraPlu[ usNoOfRead ].uchPluAdj = m_paraPluByDept[ usNoOfRead ].uchPluAdj;

				memcpy(&m_paraPlu[ usNoOfRead ].ParaPlu, &m_paraPluByDept[ usNoOfRead ].ParaPlu, sizeof( OPPLU_PARAENTRY ));
				usNoOfRead++;
			}
		}

		// --- update read counter for next read function call ---
        m_ulPluralCounter = m_paraPluByDept[ CLI_PLU_MAX_NO - 1 ].culCounter;
		m_usPluralAdjCo   = m_paraPluByDept[ CLI_PLU_MAX_NO - 1 ].usAdjCo;		// ADD Saratoga
		m_husPluralHandle = m_paraPluByDept[ CLI_PLU_MAX_NO - 1 ].husHandle;	// ADD Saratoga

        pcsample::TraceFunction( _T("\t::SerOpPluAllReadByDept() - No Of Read (%d)"), usNoOfRead );
    }

    pcsample::TraceFunction( _T("END   >> CParaPlu::ReadPluralByDept() = %d"), ( m_sLastError == OP_PERFORM ));
    return ( m_sLastError == OP_PERFORM );
}

//===========================================================================
//
//  FUNCTION :  CParaPlu::getName()
//
//  PURPOSE :   Get PLU name in read PLU parameter.
//
//  RETURN :    LPTSTR  - Address of buffer to PLU name
//
//===========================================================================

LPTSTR CParaPlu::getName(
    LPTSTR lpszPluName,         // points to buffer to retrieve PLU name
    const USHORT usIndex        // index of member variable of plu parameter 
    ) const
{
    ASSERT( usIndex < CLI_PLU_MAX_NO );
    ASSERT( ! IsBadWritePtr( lpszPluName, OP_PLU_NAME_SIZE + 1 ));

    // --- copy PLU name of specified index to user buffer ---
    memset( lpszPluName, 0, (OP_PLU_NAME_SIZE + 1) * sizeof(TCHAR) );
    _tcsncpy( lpszPluName, m_paraPlu[ usIndex ].ParaPlu.auchPluName, OP_PLU_NAME_SIZE );

    return ( lpszPluName );
}

void CParaPlu::convertPLU(TCHAR *pszDestPLU, const TCHAR *pszSourcePLU)
{
    USHORT  i;

    memset(pszDestPLU, 0, PLU_MAX_DIGIT+1);
    for ( i = 0; (*(pszSourcePLU+i) == '0') && (i<PLU_MAX_DIGIT) ; i++) {
    }
    if(i>=PLU_MAX_DIGIT)
        return;

    switch(i) {
	case 0:				//RSS 14
    case 1:             /* EAN-13 */
    case 6:             /* EAN-8 */
    case 2:             /* UPC-A, UPC-E */
    case 3:
        memcpy(pszDestPLU, (pszSourcePLU), (PLU_MAX_DIGIT) * sizeof(WCHAR));
        break;

    default:            /* Velocity */
        memcpy(pszDestPLU, (pszSourcePLU + i), (PLU_MAX_DIGIT - i - 1) * sizeof(WCHAR));
        /* not print out C/D */
        break;                                              /* R2.0 End */
    }

}


/////////////////// END OF FILE ( ParaPlu.cpp ) /////////////////////////////