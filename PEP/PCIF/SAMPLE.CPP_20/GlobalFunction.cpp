/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  GlobalFunction.cpp
//
//  PURPOSE:    Provides global functions called by any other class.
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
#include "Parameter.h"
#include "Terminal.h"
#include "Global.h"

#include "PCSample.h"
#include "PCSampleDoc.h"

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
//          F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  TraceFunction()
//
//  PURPOSE :   Trace the function call to the log file.
//
//  RETURN :    No return value.
//
//===========================================================================

VOID AFX_CDECL pcsample::TraceFunction(
	LPCTSTR lpszFormat,         // Format of trace string
	...                         // Variable arguments to format string
)
{
	// --- get instance of current application ---
	CPCSampleApp*   pApp = dynamic_cast<CPCSampleApp*>(AfxGetApp());

	if (pApp) {
		// --- get variable argument pointer and pass to main application ---
		va_list argList;

		va_start(argList, lpszFormat);
		pApp->TraceFunction(lpszFormat, argList);
		va_end(argList);
	}
}

//===========================================================================
//
//  FUNCTION :  ReadPluEx()
//
//  PURPOSE :   Print recent PLU records.
//
//  RETURN :    TRUE    - All records are printed.
//              FALSE   - Function is failed.
//
//===========================================================================
//SR275
BOOL pcsample::ReadPluEx( class CTtlPlu&	TtlPlu,
						   const UCHAR		uchClassToRead,
						   class CPCSampleDoc* const pDoc )
{
    CParaPlu    paraPlu;
	LONG		lCounter=0;
	BOOL fSuccess = FALSE,
		 fContinue = TRUE,
		 fFirst = TRUE;

	// --- initialize PLU parameter class before API call ---
    paraPlu.Initialize();

	/* 
		Using TTlPlu.ReadEx, we are reading the current daily plu
		totals and values. The read is done in sets of 20 (CLI_PLU_EX_MAX_NO).
		Each read will extract zero to 20 of the current daily plu
		values and print them to the display. This will be done until
		the counter (which contains the total number of Current daily
		plus) is decremented to zero (a full read of twenty) or less
		than zero ( 28 Plu's so two reads are needed which result in 
		lCounters final value to be -12, two reads of lCounter - 20). 

		This functionality is implemented to test NHPOS's ability to 
		read multiple recent PLU values using SerTtlTotalReadPluEx. SR254
	*/
	pDoc->llReportTotalAmount = pDoc->lReportTotalCount = 0;
	do {

		fSuccess = TtlPlu.ReadEx(uchClassToRead);
		if( fFirst && fSuccess ) {
			lCounter = TtlPlu.getTtlCntEx();
		}
		
		if( fSuccess) {
			if(lCounter > 0) {
				if(fFirst) {
					pDoc->ReportExHeader(TtlPlu);
				}
				pDoc->ReportTotalPluEx(TtlPlu, paraPlu);
				lCounter -= CLI_PLU_EX_MAX_NO;
				fFirst = FALSE;
			} else {
				fContinue = FALSE;
				if(fFirst) {
					pDoc->ReportError( pcsample::CLASS_PARAMETER, pcsample::ERR_ALLREAD, TtlPlu.GetLastError());
				}
			}
		} else {
			pDoc->ReportError(  CLASS_TTLPLU, pcsample::ERR_TTLREAD, TtlPlu.GetLastError());
		}
	} while ( (lCounter >=0)  && (fContinue == TRUE) );
	
	if (pDoc->lReportTotalCount > 0) {
		CString str;
		str.Format(_T("  TOTALS  \t\t\t\t\t\t %d\t%16s"), pDoc->lReportTotalCount, pDoc->MakeCurrencyString(pDoc->llReportTotalAmount));
		pDoc->SetReportString(str);
		str.Format(_T("*** END OF PLU REPORT ***"));
		pDoc->SetReportString(str);
	}
	return fSuccess;
}

//===========================================================================
//
//  FUNCTION :  ReadAllRecords()
//
//  PURPOSE :   Print all PLU records belong to the specified department.
//
//  RETURN :    TRUE    - All records are printed.
//              FALSE   - Function is failed.
//
//===========================================================================
// ###DEL Saratoga
//BOOL pcsample::ReadAllRecords(
//    const UCHAR uchClassToRead, // indicates current or saved daily total
//    CTtlPlu&    ttlPlu,         // PLU total class for interface to 2170
//    const UCHAR uchDeptNo,      // department# to read PLU record
//    BOOL&       fHeaderPrinted, // indicates header is already printed
//    CPCSampleDoc* const pDoc    // document class to print PLU record
//    )
BOOL pcsample::ReadAllRecords(
    const UCHAR uchClassToRead, // indicates current or saved daily total
    CTtlPlu&    ttlPlu,         // PLU total class for interface to 2170
    const USHORT usDeptNo,      // department# to read PLU record
    BOOL&       fHeaderPrinted, // indicates header is already printed
    CPCSampleDoc* const pDoc    // document class to print PLU record
    )
{
    BOOL        fSuccess;
    USHORT      usNoOfRead;
    CParaPlu    paraPlu;
 
    // --- initialize PLU parameter class before API call ---
    paraPlu.Initialize();

    // --- read PLU record until error occurs or all records are read ---
	pDoc->llReportTotalAmount = pDoc->lReportTotalCount = 0;
    do
    {
        // --- read plural PLU parameter by department# ---
        fSuccess = paraPlu.ReadPluralByDept( usDeptNo, usNoOfRead );
		
        if ( fSuccess && ( 0 < usNoOfRead ))
        {
			TCHAR       uchPluNo[CLI_PLU_MAX_NO][OP_PLU_LEN] = { 0 };
			UCHAR       uchAdjectiveNo[CLI_PLU_MAX_NO] = { 0 };

            // --- prepare PLU# and adjective# to read PLU total ---
            for ( USHORT usI = 0; usI < usNoOfRead; usI++ )
            {
                paraPlu.getPluNo((LPTSTR)&uchPluNo[usI], usI);
                uchAdjectiveNo[ usI ] = paraPlu.getAdjectiveNo( usI );
            }

            // --- read plural PLU total by PLU# and adjective# ---
            fSuccess = ttlPlu.ReadPlural( uchClassToRead, uchPluNo,  uchAdjectiveNo );
            if ( fSuccess )
            {
                if ( ! fHeaderPrinted )
                {
                    // --- print header of PLU report ---
                    pDoc->ReportHeader( ttlPlu );
                    fHeaderPrinted = TRUE;
                }
				if (pDoc->pSavedTotalFile) {
					for (USHORT usI = 0; usI < usNoOfRead; usI++)
					{
						pDoc->pSavedTotalFile->Write(ttlPlu.m_ttlPlu[usI]);
					}
				}
                // --- print retrieved PLU parameter and total ---
                pDoc->ReportTotal( ttlPlu, paraPlu, usNoOfRead );
            }
            else
            {
                // --- output error message ---
                pDoc->ReportError( CLASS_TTLPLU, pcsample::ERR_TTLREAD, ttlPlu.GetLastError());
            }
        }
        else
        {
            // --- output error message ---
            if ( ! fSuccess )
            {
                pDoc->ReportError( pcsample::CLASS_PARAMETER, pcsample::ERR_ALLREAD, paraPlu.GetLastError());
            }
        }
    }
    while ( fSuccess && ( usNoOfRead == CLI_PLU_MAX_NO ));

	pDoc->pSavedTotalFile && pDoc->pSavedTotalFile->WriteIndex();

	if (pDoc->lReportTotalCount > 0) {
		CString str;
		str.Format(_T("  TOTALS  \t\t\t\t\t\t\t\t %d\t%16s"), pDoc->lReportTotalCount, pDoc->MakeCurrencyString(pDoc->llReportTotalAmount));
		pDoc->SetReportString(str);
		str.Format(_T("*** END OF PLU REPORT ***"));
		pDoc->SetReportString(str);
	}
    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  ReadAllRecords()
//
//  PURPOSE :   Print all PLU records by PLU number order.
//
//  RETURN :    TRUE    - All records are printed.
//              FALSE   - Function is failed.
//
//===========================================================================
// ###DEL Saratoga
//BOOL pcsample::ReadAllRecords(
//    const UCHAR uchClassToRead, // indicates current or saved daily total
//    CTtlPlu&    ttlPlu,         // PLU total class for interface to 2170
//    BOOL&       fHeaderPrinted, // indicates header is already printed
//    CPCSampleDoc* const pDoc    // document class to print PLU record
//    )
BOOL pcsample::ReadAllRecords(
    const UCHAR uchClassToRead, // indicates current or saved daily total
    CTtlPlu&    ttlPlu,         // PLU total class for interface to 2170
    BOOL&       fHeaderPrinted, // indicates header is already printed
    CPCSampleDoc* const pDoc    // document class to print PLU record
    )
{
    BOOL        fSuccess;
    USHORT      usNoOfRead;
    CParaPlu    paraPlu;
 
    // --- initialize PLU parameter class before API call ---
    paraPlu.Initialize();

    // --- read PLU record until error occurs or all records are read ---
    do
    {
        // --- read plural PLU parameter by PLU # ---
        fSuccess = paraPlu.ReadPlural ( usNoOfRead );
		
        if ( fSuccess && ( 0 < usNoOfRead ))
        {
			TCHAR       uchPluNo[CLI_PLU_MAX_NO][OP_PLU_LEN] = { 0 };
			UCHAR       uchAdjectiveNo[CLI_PLU_MAX_NO] = { 0 };

            // --- prepare PLU# and adjective# to read PLU total ---
            for ( USHORT usI = 0; usI < usNoOfRead; usI++ )
            {
                paraPlu.getPluNo((LPTSTR)&uchPluNo[usI], usI);
                uchAdjectiveNo[ usI ] = paraPlu.getAdjectiveNo( usI );
            }

            // --- read plural PLU total by PLU# and adjective# ---
            fSuccess = ttlPlu.ReadPlural( uchClassToRead, uchPluNo,  uchAdjectiveNo );
            if ( fSuccess )
            {
                if ( ! fHeaderPrinted )
                {
                    // --- print header of PLU report ---
                    pDoc->ReportHeader( ttlPlu );
                    fHeaderPrinted = TRUE;
                }

                // --- print retrieved PLU parameter and total ---
                pDoc->ReportTotal( ttlPlu, paraPlu, usNoOfRead );
            }
            else
            {
                // --- output error message ---
                pDoc->ReportError( CLASS_TTLPLU, pcsample::ERR_TTLREAD, ttlPlu.GetLastError());
            }
        }
        else
        {
            // --- output error message ---
            if ( ! fSuccess )
            {
                pDoc->ReportError( pcsample::CLASS_PARAMETER, pcsample::ERR_ALLREAD, paraPlu.GetLastError());
            }
        }
    }
    while ( fSuccess && ( usNoOfRead == CLI_PLU_MAX_NO ));

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  ReadAllRecords()
//
//  PURPOSE :   Print all coupon records.
//
//  RETURN :    TRUE    - All records are printed.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL pcsample::ReadAllRecords(
    const UCHAR uchClassToRead,     // indicates current or saved daily total
    CTtlCoupon&   ttlCoupon,  // coupon total class for i/f to 2170
    CPCSampleDoc* const pDoc        // document class to print PLU record
    )
{
    BOOL            fSuccess;
    CParaFlexMem    paraFlexMem;

    // --- get size of flexible memory in 2170 ---
    fSuccess = paraFlexMem.Read();
    if ( fSuccess )
    {
        BOOL    fHeaderPrinted;
        fHeaderPrinted = FALSE;

		USHORT     usCouponNo;
		ULONG      ulMaxCouponNo;

        fSuccess      = TRUE;
        ulMaxCouponNo = paraFlexMem.getMemorySize( FLEX_CPN_ADR );

        // --- read all coupon record until error occurs ---
		usCouponNo = 1;
        while ( fSuccess && (usCouponNo <= ulMaxCouponNo ))
        {
			CParaCoupon paraCoupon;

            // --- read coupon parameter by coupon# ---
            fSuccess = paraCoupon.Read(usCouponNo);
            if ( fSuccess )
            {

                // --- read coupon total by coupon# ---
                fSuccess = ttlCoupon.Read( uchClassToRead, usCouponNo );
				if (fSuccess)
				{
					if (!fHeaderPrinted)
					{
						// --- print header of coupon report ---
						pDoc->ReportHeader(ttlCoupon);
						fHeaderPrinted = TRUE;
					}

					if (pDoc->pSavedTotalFile) {
						CPNTBL  ttlCouponCpn = { 0 };

						ttlCouponCpn.usCpnNo = usCouponNo;
						_tcsncpy (ttlCouponCpn.auchMnemo, paraCoupon.getName(), OP_CPN_NAME_SIZE);
						ttlCouponCpn.CPNTotal = ttlCoupon.m_ttlCoupon.CpnTotal;
						pDoc->pSavedTotalFile->Write(ttlCouponCpn, usCouponNo);
					}

					// --- print retrieved coupon parameter and total ---
                    pDoc->ReportTotal( ttlCoupon, paraCoupon );
                }
                else
                {
                    // --- output error message ---
                    pDoc->ReportError( CLASS_TTLCPN, pcsample::ERR_TTLREAD, ttlCoupon.GetLastError());
                }
            }
            else
            {
                // --- output error message ---
                pDoc->ReportError( pcsample::CLASS_PARAMETER, pcsample::ERR_INDREAD, paraCoupon.GetLastError());
            }

            // --- increment coupon# to get next record ---
			if(usCouponNo >= pcttl::MAX_COUPON_NO)
			{
				break;
			}
			usCouponNo++;
        }
    }
    else
    {
        // --- output error message ---
        pDoc->ReportError( pcsample::CLASS_PARAMETER, pcsample::ERR_ALLREAD, paraFlexMem.GetLastError());
    }

    return ( fSuccess );
}

///////////////// END OF FILE ( GlobalFunction.cpp ) ////////////////////////