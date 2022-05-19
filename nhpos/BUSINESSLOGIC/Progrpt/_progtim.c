/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*=======================================================================*\
*   Title              : Programmable Report module, Program List
*   Category           : Prog. Report, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : _PROGTIM.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : The provided function names are as follows.
*
*   ProgRpt_TimeReset()         - reset current daily/PTD to saved total file.
*   ProgRpt_TimeIsResettable()  - determine current daily/PTD is resettable.
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
*    Date      Ver.Rev :NAME       :Description
*    Feb-29-96:03.01.00:T.Nakahata :Initial
*    Mar-29-96:03.01.02:T.Nakahata :check TTL_NOTINFILE in IsResettable
*
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include	<tchar.h>
#include    <string.h>

#include    <ecr.h>
#include    <regstrct.h>
#include    <pif.h>
#include    <log.h>
#include    <appllog.h>
#include    <paraequ.h>
#include    <para.h>
#include    <rfl.h>
#include    <csttl.h>
#include    <csop.h>
#include    <cscas.h>
#include    <cswai.h>
#include    <csstbopr.h>
#include    <csstbpar.h>
#include    <csstbttl.h>
#include    <report.h>
#include    <progrept.h>
#include    <csprgrpt.h>
#include    "_progrpt.h"

/*
===========================================================================
    GLOBAL VARIABLES DECLARATIONs
===========================================================================
*/

/*
===========================================================================
    PROGRAMMABLE REPORT FILE I/F FUNCTIONs
===========================================================================
*/
/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_TimeReset( UCHAR uchMinorClass )
*
*   Input   :   UCHAR uchMinorClass - minor class for service time total.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 LDT_KEYOVER_ADR
*                                 LDT_ERR_ADR       ( TTL_FAIL )
*                                 LDT_PROHBT_ADR    ( OP_FILE_NOT_FOUND )
*                                   
**  Description:
*    This function resets current daily/PTD service time total.
*    An application must call ProgRpt_TimeIsRessetable() function before
*   calling this function. If the specified total is resettable, it may
*   call this function.
*    This function deletes saved daily/PTD total, and then copies current
*   total to saved total. Service time total is resetted at only EOD/PTD
*   report.
*==========================================================================
*/
SHORT ProgRpt_TimeReset( UCHAR uchMinorClass )
{
    TTLREPORT   TtlReport;      /* structure for reset service time total */
    SHORT       sRetCode;       /* return value for user application */
    USHORT      usExecuteFlag;  /* execute flag for EOD/PTD report */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ));

    memset( &TtlReport, 0x00, sizeof( TtlReport ));

    /* --- determine whether reset daily total or PTD total --- */
    switch ( uchMinorClass ) {

    case CLASS_TTLCURDAY:   /* reset daily total at EOD reset report */
        TtlReport.uchMajorClass = CLASS_TTLEODRESET;
        break;

    case CLASS_TTLCURPTD:   /* reset PTD total at PTD reset report */
    default:
        TtlReport.uchMajorClass = CLASS_TTLPTDRESET;
        break;
    }

    /* --- reset service time total at EOD/PTD reset report --- */
    usExecuteFlag = RPT_RESET_SERVICE_BIT;

    /* --- reset specified hourly total file --- */
    ASSERT( ProgRpt_TimeIsResettable( uchMinorClass ));
    sRetCode = SerTtlTotalReset( &TtlReport, usExecuteFlag );

    /* --- convert return code to lead-thru code --- */
    return ( TtlConvertError( sRetCode ));
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_TimeIsResettable( UCHAR uchMinorClass )
*
*   Input   :   UCHAR uchMinorClass - minor class for service time total.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - current total is resettable.
*                       FALSE   - current total is not resettable.
*   
**  Description:
*    This function detemines specified current service time total is able to
*   reset or not.
*    If saved total file is not issued, current total is not resettable.
*==========================================================================
*/
BOOL ProgRpt_TimeIsResettable( UCHAR uchMinorClass )
{
    TTLREPORT   TtlReport;      /* structure for reset service time total */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ));

    /* --- determine whether reset daily total or PTD total --- */
    switch ( uchMinorClass ) {

    case CLASS_TTLCURDAY:
        TtlReport.uchMinorClass = CLASS_TTLSAVDAY;
        break;

    case CLASS_TTLCURPTD:
    default:
        TtlReport.uchMinorClass = CLASS_TTLSAVPTD;
        break;
    }

    /* --- determine specified saved total file is issued ? --- */
    TtlReport.uchMajorClass = CLASS_TTLSERVICE;
    TtlReport.ulNumber = 0;

    if ( SerTtlIssuedCheck( &TtlReport, TTL_NOTRESET ) == TTL_NOT_ISSUED ) {
        return ( FALSE );
    }
    return ( TRUE );
}

/* ===== End of File (_PROGTIM.C) ===== */
