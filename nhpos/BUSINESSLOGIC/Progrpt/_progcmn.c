/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*=======================================================================*\
*   Title              : Programmable Report module, Program List
*   Category           : Prog. Report, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : _PROGCMN.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           :The provided function names are as follows.
*
*   ProgRpt_GetProgRptInfo() - Retrieve information about prog. report.
*   ProgRpt_GetEODPTDInfo()  - Retrieve information about EOD/PTD.
*
*   ProgRpt_PrintAndFeed()   - Print formatted item and feed device.
*   ProgRpt_PrintHeader()    - Print header of prog. read/reset report.
*   ProgRpt_Feed()           - Feed specified output device.
*   ProgRpt_Print()          - Print formatted item on printer/display
*   ProgRpt_PrintLoopBegin() - Print loop-begin item.
*   ProgRpt_PrintEOF()       - Print End Of File item.
*
*   ProgRpt_InitAccumulator()- Initialize accumulator.
*   ProgRpt_GetAccumulator() - Retrieve accumulator group item.
*   ProgRpt_SetAccumulator() - Set a value to specified accumulator.
*   ProgRpt_CalcAccumulator()- Calculate accumulator.
*   ProgRpt_GetMnemonic()    - Retrieve mnemonic group item.
*   ProgRpt_IntegerToString()- Convert integer value to string data.
*   ProgRpt_SetDateTimeBuff()- Set Date/Time data to print buffer.
*   ProgRpt_FormatPrintBuff()- Format and set string data to print buffer.
*   ProgRpt_IsPrintCondTRUE()- determine print condition is TRUE.
*
*   ProgRpt_ConvertError()   - Convert return code to lead through code.
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
* Date      : Ver.Rev  : NAME       : Description
* Feb-29-96 : 03.01.00 : T.Nakahata : Initial
* Mar-02-96 : 03.01.02 : T.Nakahata : Print ProgramReport# on Header
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Sep-21-99 :          : K.Iwata    : Changed type definition of 'D13DIGITS'
* Dec-13-99 : 01.00.00 : hrkato     : Saratoga
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
#include    <stdlib.h>

#include    <ecr.h>
#include    <regstrct.h>
#include    <pif.h>
#include    <log.h>
#include    <appllog.h>
#include    <paraequ.h>
#include    <para.h>
#include    <maint.h>
#include    <pmg.h>
#include    <mld.h>
#include    <csttl.h>
#include    <cscas.h>
#include    <cswai.h>
#include    <csop.h>
#include    <csstbpar.h>
#include    <transact.h>
#include    <prt.h>
#include    <report.h>
#include    <rfl.h>
#include    <progrept.h>
#include    <csprgrpt.h>
#include    "_progrpt.h"

/*
===========================================================================
    GLOBAL VARIABLES DECLARATIONs
===========================================================================

/*
===========================================================================
    MODULE VARIABLES DECLARATIONs
===========================================================================
*/
    /* --- acucumulator for calculate total/counter(s) --- */
static D13DIGITS hugeAccumulator[ PROGRPT_ACCUM_COUNTER ][ PROGRPT_MAX_ACCUMULATOR ];

    /* 06/02/99 */
TCHAR auchPROGRPT_AM_SYMBOL[] = STD_TIME_AM_SYMBOL;
TCHAR auchPROGRPT_PM_SYMBOL[] = STD_TIME_PM_SYMBOL;

/*
===========================================================================
    PROGRAMMABLE REPORT FILE I/F FUNCTIONs
===========================================================================
*/
/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_GetProgRptInfo( UCHAR uchProgRptNo,
*                                               PROGRPT_RPTHEAD *pHeader )
*
*   Input   :   UCHAR  uchProgRptNo - programmable report no.
*   Output  :   PROGRPT_RPTHEAD *pHeader - points to programmable report header
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - PROGRPT_SUCCESS
*                                 PROGRPT_NOT_IN_FILE
*
**  Description:
*    This function retrieves programmable report header that associated
*   with specified programmable report number.
*==========================================================================
*/
SHORT ProgRpt_GetProgRptInfo( UCHAR uchProgRptNo, PROGRPT_RPTHEAD *pHeader )
{
    ULONG   ulActualRead;       /* actual read size in bytes */
	//11-11-03 JHHJ SR 201
	PROGRPT_FHEAD FileInfo = { 0 };     /* information about prog report file */
    ULONG   ulProgRptOffset;    /* program report format offset */

    ASSERT(( 1 <= uchProgRptNo ) && ( uchProgRptNo <= MAX_PROGRPT_FORMAT ));
    ASSERT( pHeader != ( PROGRPT_RPTHEAD * )NULL );

    /* --- retrieve PROGRPT file header to check file is valid or not --- */

	//11-11-03 JHHJ SR 201 Parameter 4 change from USHORT to ULONG
    ProgRpt_ReadFile( PROGRPT_OFFSET_FILEHEADER, &FileInfo, sizeof( PROGRPT_FHEAD ), &ulActualRead );
    if ( ulActualRead != sizeof( PROGRPT_FHEAD )) {
        return ( PROGRPT_NO_READ_SIZE );
    }

    if ( _tcsncmp( FileInfo.auchMark, PROGRPT_MARK_FHEAD, PROGRPT_MAX_MARK_FHEAD )) {
        return ( PROGRPT_INVALID_FILE );
    }

    if ( FileInfo.usVersion != PROGRPT_VERSION ) {
        return ( PROGRPT_INVALID_VERSION );
    }

    /* --- determine specified report format is stored in file --- */

    ulProgRptOffset = FileInfo.ulOffset40Char + sizeof( PROGRPT_40CHAR );
    if ( FileInfo.aulOffsetReport[ uchProgRptNo - 1 ] < ulProgRptOffset ) {
        return ( PROGRPT_NOT_IN_FILE );
    }

    /* --- retrieve specified programmable report header --- */

	//11-11-03 JHHJ SR 201 Parameter 4 change from USHORT to ULONG
    ProgRpt_ReadFile( FileInfo.aulOffsetReport[ uchProgRptNo - 1 ], pHeader, sizeof( PROGRPT_RPTHEAD ), &ulActualRead );
    if ( ulActualRead != sizeof( PROGRPT_RPTHEAD )) {
        return ( PROGRPT_NO_READ_SIZE );
    }
    if ( _tcsncmp( pHeader->auchMark, PROGRPT_MARK_REPORT, PROGRPT_MAX_MARK_LEN )) {
        return ( PROGRPT_INVALID_FILE );
    }
    return ( PROGRPT_SUCCESS );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_GetEODPTDInfo( USHORT           usReportType,
*                                            USHORT           *pusNoOfRpt,
*                                            PROGRPT_EODPTDIF *paEODPTDRec )
*
*   Input   :   USHORT           usReportType - type of information.
*   Output  :   USHORT           *pusNoOfRpt  - number of EOD/PTD report
*               PROGRPT_EODPTDIF *paEODPTDRec - points to array of EOD/PTD info.
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - PROGRPT_SUCCESS
*                                 PROGRPT_NOT_IN_FILE
*
**  Description:
*    This function retrieves programmable report header that associated
*   with specified programmable report number.
*==========================================================================
*/
SHORT ProgRpt_GetEODPTDInfo( USHORT           usReportType,
                             USHORT           *pusNoOfRpt,
                             PROGRPT_EODPTDIF *paEODPTDRec )
{
    ULONG   ulActualRead;       /* actual read size in bytes */
	PROGRPT_FHEAD   FileInfo = { 0 };   /* information about prog report file */
    ULONG   ulReadOffset;       /* target item read offset */
	PROGRPT_EODPTD  EODPTD = { 0 };     /* information about EOD/PTD report */
    TCHAR    *puchMarkStr;       /* address of EOD/PTD header mark string */

    ASSERT(( usReportType == PROGRPT_EOD ) || ( usReportType == PROGRPT_PTD ));
    ASSERT( pusNoOfRpt != ( USHORT * )NULL );
    ASSERT( paEODPTDRec != ( PROGRPT_EODPTDIF * )NULL );

    /* --- retrieve PROGRPT file header to check file is valid or not --- */

    ProgRpt_ReadFile( PROGRPT_OFFSET_FILEHEADER, &FileInfo, sizeof( PROGRPT_FHEAD ), &ulActualRead );
    if ( ulActualRead != sizeof( PROGRPT_FHEAD )) {
        return ( PROGRPT_NO_READ_SIZE );
    }

    if ( _tcsncmp( FileInfo.auchMark, PROGRPT_MARK_FHEAD, PROGRPT_MAX_MARK_FHEAD )) {
        return ( PROGRPT_INVALID_FILE );
    }

    if ( FileInfo.usVersion != PROGRPT_VERSION ) {
        return ( PROGRPT_INVALID_VERSION );
    }

    /* --- retrieve specified programmable report header --- */

    switch ( usReportType ) {
    case PROGRPT_EOD:
        ulReadOffset = FileInfo.ulOffsetEOD;
        puchMarkStr  = PROGRPT_MARK_EOD;
        break;
    case PROGRPT_PTD:
        ulReadOffset = FileInfo.ulOffsetPTD;
        puchMarkStr  = PROGRPT_MARK_PTD;
        break;
    default:
        return ( PROGRPT_INVALID_PARAM );
    }

    /* --- retrieve information about EOD/PTD report --- */

    ProgRpt_ReadFile( ulReadOffset, &EODPTD, sizeof( PROGRPT_EODPTD ), &ulActualRead );
    if ( ulActualRead != sizeof( PROGRPT_EODPTD )) {
        return ( PROGRPT_NO_READ_SIZE );
    }
    if ( _tcsncmp( EODPTD.auchMark, puchMarkStr, PROGRPT_MAX_MARK_LEN )) {
        return ( PROGRPT_INVALID_FILE );
    }
    if ( MAX_PROGRPT_EODPTD < EODPTD.usNoOfRecord ) {
        return ( PROGRPT_INVALID_FILE );
    }
    if ( ! EODPTD.usUseProgRpt ) {
        return ( PROGRPT_NOT_IN_FILE );
    }

    /* --- copy retrieved information to user's buffer --- */

    *pusNoOfRpt = EODPTD.usNoOfRecord;
    memcpy( paEODPTDRec, &EODPTD.aEODPTD[ 0 ], sizeof( PROGRPT_EODPTDIF ) * EODPTD.usNoOfRecord );

    return ( PROGRPT_SUCCESS );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_PrintAndFeed( PROGRPT_RPTHEAD * pRptHead,
*                                           UCHAR   * puchBuffer )
*
*   Input   :   PROGRPT_RPTHEAD * pRptHead - Information about prog. report
*               UCHAR   * puchBuffer - address of format record.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sLDTCode - SUCCESS
*                                RPT_ABORTED
*                                   
**  Description:
*    This function prints the programmable report and feed it.
*==========================================================================
*/
SHORT ProgRpt_PrintAndFeed( PROGRPT_RPTHEAD *pRptHead, VOID *pBuffer )
{
    PROGRPT_RECORD *pRecord = pBuffer;
    UCHAR   uchItemRow;
    UCHAR   uchItemColumn;
    UCHAR   uchFeedLine;
    BOOL    fLoopEndItem;

    ASSERT( pRptHead != ( PROGRPT_RPTHEAD * )NULL );
    ASSERT( pBuffer != ( UCHAR * )NULL );

    uchItemRow    = pRecord->Print.uchRow;
    uchItemColumn = pRecord->Print.uchColumn;
    ASSERT( 0 < uchItemRow );
    ASSERT( 0 < uchItemColumn );

    fLoopEndItem = (( pRecord->Loop.uchOperation == PROGRPT_OP_LOOP ) &&
                    (( pRecord->Loop.uchLoopBrace == PROGRPT_LOOP_END_ROOT ) ||
                     ( pRecord->Loop.uchLoopBrace == PROGRPT_LOOP_END_NEST )));

    if ( uchItemRow < uchProgRptRow ) {

        return ( RPT_ABORTED );

    } else if ( uchItemRow == uchProgRptRow ) {

        /* --- if specified record is loop end item, compulsory print
            current formatting string --- */

        if  ( fLoopEndItem ) {
            ProgRpt_Print( pRptHead );
        }

    } else {    /* uchProgRptRow < uchItemRow */

        switch ( pRecord->Print.uchOperation ) {
        case PROGRPT_OP_PRINT:
        case PROGRPT_OP_LOOP:

            /* --- if specified record is greater than current saving item,
                print programmable report compulsory --- */

            ProgRpt_Print( pRptHead );

            uchFeedLine = uchItemRow - uchProgRptRow;
            if ( fLoopEndItem ) {
                uchFeedLine = ( fProgRptEmptyLoop ) ? ( UCHAR) 0 : uchFeedLine;
            } else {
                uchFeedLine -= ( UCHAR )1;
            }

            ProgRpt_Feed( pRptHead->usDevice, uchFeedLine );

            break;

        case PROGRPT_OP_MATH:
        default:
            break;
        }
    }

    uchProgRptRow    = uchItemRow;
    uchProgRptColumn = uchItemColumn;

    return ( SUCCESS );
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_PrintHeader( UCHAR uchMinorClass,
*                                         UCHAR uchProgRptNo,
*                                         UCHAR uchReportName,
*                                         UCHAR uchResetType )
*
*   Input   :   UCHAR uchMinorClass - minor class of total file.
*               UCHAR uchProgRptNo  - programmable report no.
*               UCHAR uchReportName - report name which is printed on header.
*               UCHAR uchResetType  - reset type (reset, reset & print, print)
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sLDTCode - SUCCESS
*                                   
**  Description:
*    This function prints the programmable report header.
*==========================================================================
*/
VOID ProgRpt_PrintHeader( UCHAR uchMinorClass,
                          UCHAR uchProgRptNo,
                          UCHAR uchReportName,
                          UCHAR uchResetType )
{
    UCHAR   uchDaily_PTD;   /* indicate total file is daily or PTD */
    UCHAR   uchRead_Reset;  /* indicate report is read(AC135) or reset(AC136) */
    UCHAR   uchACNo;        /* action code no. */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));
    ASSERT( uchProgRptNo <= MAX_PROGRPT_FORMAT );
    ASSERT(( uchReportName == RPT_FINANC_ADR ) ||\
           ( uchReportName == RPT_DEPT_ADR   ) ||\
           ( uchReportName == RPT_PLU_ADR    ) ||\
           ( uchReportName == RPT_HOUR_ADR   ) ||\
           ( uchReportName == RPT_CAS_ADR    ) ||\
           ( uchReportName == RPT_WAT_ADR    ) ||\
           ( uchReportName == RPT_CPN_ADR    ) ||\
           ( uchReportName == RPT_SERVICE_ADR ) ||\
           ( uchReportName == RPT_EOD_ADR    ) ||\
           ( uchReportName == RPT_PTD_ADR    ));
    ASSERT(( uchResetType == RPT_READ_REPORT ) ||\
           ( uchResetType == RPT_ONLY_PRT_RESET ) ||\
           ( uchResetType == RPT_PRT_RESET      ) ||\
           ( uchResetType == RPT_ONLY_RESET     ));

    /* --- Has the header already been printed ? --- */

    if ( ! ( uchRptOpeCount & RPT_HEADER_PRINT )) {

        /* --- initialize print status for report --- */

        RptPrtStatusSet( uchMinorClass );

        switch ( uchMinorClass ) {

        case CLASS_TTLCURDAY:
            uchDaily_PTD  = SPC_DAIRST_ADR;
            uchRead_Reset = RPT_READ_ADR;
            uchACNo       = AC_PROG_READ_RPT;
            break;
   
        case CLASS_TTLSAVDAY:
            uchDaily_PTD  = SPC_DAIRST_ADR;
            uchRead_Reset = RPT_RESET_ADR;
            uchACNo = ( UCHAR )(( uchReportName == RPT_EOD_ADR ) ?
                                    AC_EOD_RPT : AC_PROG_RESET_RPT );
            break;

        case CLASS_TTLCURPTD:
            uchDaily_PTD  = SPC_PTDRST_ADR;
            uchRead_Reset = RPT_READ_ADR;
            uchACNo       = AC_PROG_READ_RPT;
            break;

        case CLASS_TTLSAVPTD:
        default:
            uchDaily_PTD  = SPC_PTDRST_ADR;
            uchRead_Reset = RPT_RESET_ADR;
            uchACNo = ( UCHAR )(( uchReportName == RPT_PTD_ADR ) ?
                                    AC_PTD_RPT : AC_PROG_RESET_RPT );
            break;
        }

        /* --- set up & print header --- */

        MaintMakeHeader( CLASS_MAINTHEADER_RPT,
                         RPT_ACT_ADR,
                         uchReportName,
                         uchDaily_PTD,
                         uchRead_Reset,
                         uchProgRptNo,
                         uchACNo,
                         uchResetType,
                         PRT_RECEIPT | PRT_JOURNAL );
    }
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_Feed( USHORT usDevice, UCHAR uchNoOfFeed )
*
*   Input   :   USHORT usDevice     - type of output device
*               UCHAR  uchNoOfFeed  - no. of feed line
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*                                   
**  Description:
*    This function feeds the specified device.
*==========================================================================
*/
VOID ProgRpt_Feed( USHORT usDevice, UCHAR uchNoOfFeed )
{
    UCHAR   auchFeedLine[ PROGRPT_MAX_COLUMN_MLD ];
    UCHAR   uchCurFeed;
	RPTPROGRAM  RptMldDisp = { 0 };

    ASSERT(( usDevice == PROGRPT_DEVICE_RJ21    ) ||\
           ( usDevice == PROGRPT_DEVICE_RJ24    ) ||\
           ( usDevice == PROGRPT_DEVICE_THERM42 ) ||\
           ( usDevice == PROGRPT_DEVICE_THERM56 ) ||\
           ( usDevice == PROGRPT_DEVICE_MLD     ));
    ASSERT(( usRptPrintStatus == PRT_JOURNAL ) ||\
           ( usRptPrintStatus == PRT_RECEIPT ) ||\
           ( usRptPrintStatus == ( PRT_RECEIPT | PRT_JOURNAL )));

    if ( uchNoOfFeed == 0 ) {
        return;
    }

    switch ( usDevice ) {

    case PROGRPT_DEVICE_RJ21:
    case PROGRPT_DEVICE_RJ24:
    case PROGRPT_DEVICE_THERM42:
    case PROGRPT_DEVICE_THERM56:

        switch ( usRptPrintStatus ) {

        case PRT_JOURNAL:

            PrtFeed( PMG_PRT_JOURNAL, uchNoOfFeed );
            break;

        case PRT_RECEIPT:

            PrtFeed( PMG_PRT_RECEIPT, uchNoOfFeed );
            break;

        case ( PRT_RECEIPT | PRT_JOURNAL ):
        default:

            PrtFeed( PMG_PRT_RECEIPT, uchNoOfFeed );
            PrtFeed( PMG_PRT_JOURNAL, uchNoOfFeed );
            break;
        }
        break;

    case PROGRPT_DEVICE_MLD:
    default:

        memset( auchFeedLine, 0x00, PROGRPT_MAX_COLUMN_MLD );
        memset( &RptMldDisp, 0x00, sizeof( RptMldDisp ));
        RptMldDisp.uchMajorClass  = CLASS_RPTPROGRAM;
        RptMldDisp.uchMinorClass  = 0;
        RptMldDisp.usPrintControl = 0;
        memset( RptMldDisp.aszLineData,
#ifdef DEBUG
                '_',
#else
                PROGRPT_PRTCHAR_SPACE,
#endif
                PROGRPT_MAX_COLUMN_MLD );

        uchCurFeed = 0;

        while ( uchCurFeed < uchNoOfFeed ) {

            uchRptMldAbortStatus = ( UCHAR )MldDispItem( &RptMldDisp, 0 );
            uchCurFeed++;
        }
        break;
    }
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_Print( PROGRPT_RPTHEAD * pRptHead )
*
*   Input   :   PROGRPT_RPTHEAD * pRptHead - Information about prog. report
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    -   print string data to output device
*                       FALSE   -   string data is nothing
*                                   
**  Description:
*    This function prints a formatted item on printer/display.
*==========================================================================
*/
BOOL ProgRpt_Print( PROGRPT_RPTHEAD *pRptHead )
{
    USHORT      usMaxColumn;
	RPTPROGRAM  RptMldDisp = { 0 };

    ASSERT( pRptHead != ( PROGRPT_RPTHEAD * )NULL );
    ASSERT(( pRptHead->usDevice == PROGRPT_DEVICE_RJ21    ) ||\
           ( pRptHead->usDevice == PROGRPT_DEVICE_RJ24    ) ||\
           ( pRptHead->usDevice == PROGRPT_DEVICE_THERM42 ) ||\
           ( pRptHead->usDevice == PROGRPT_DEVICE_THERM56 ) ||\
           ( pRptHead->usDevice == PROGRPT_DEVICE_MLD     ));
    ASSERT(( usRptPrintStatus == PRT_JOURNAL ) ||\
           ( usRptPrintStatus == PRT_RECEIPT ) ||\
           ( usRptPrintStatus == ( PRT_RECEIPT | PRT_JOURNAL )));

    if ( auchProgRptBuff[ 0 ] == '\0' ) {
        return ( FALSE );
    }

    switch ( pRptHead->usDevice ) {

    case PROGRPT_DEVICE_MLD:

        memset( &RptMldDisp, 0x00, sizeof( RptMldDisp ));
        RptMldDisp.uchMajorClass  = CLASS_RPTPROGRAM;
        RptMldDisp.uchMinorClass  = 0;
        RptMldDisp.usPrintControl = 0;
        _tcsncpy( RptMldDisp.aszLineData, auchProgRptBuff, PROGRPT_MAX_COLUMN_MLD );
        //memcpy( RptMldDisp.aszLineData, auchProgRptBuff, PROGRPT_MAX_COLUMN_MLD );

        uchRptMldAbortStatus = ( UCHAR )MldDispItem( &RptMldDisp, 0 );
        memset( auchProgRptBuff, 0x00, sizeof( auchProgRptBuff ));
        return ( TRUE );

    case PROGRPT_DEVICE_RJ21:
        usMaxColumn = PROGRPT_MAX_COLUMN_21;
        break;

    case PROGRPT_DEVICE_RJ24:
        usMaxColumn = PROGRPT_MAX_COLUMN_24;
        break;

    case PROGRPT_DEVICE_THERM42:
        usMaxColumn = PROGRPT_MAX_COLUMN_42;
        break;

    case PROGRPT_DEVICE_THERM56:
        usMaxColumn = PROGRPT_MAX_COLUMN_56;
        break;

    default:
        return ( FALSE );
    }

    switch ( usRptPrintStatus ) {

    case PRT_JOURNAL:

        PrtPrint( PMG_PRT_JOURNAL, auchProgRptBuff, usMaxColumn );
        break;

    case PRT_RECEIPT:

        PrtPrint( PMG_PRT_RECEIPT, auchProgRptBuff, usMaxColumn );
        break;

    case ( PRT_RECEIPT | PRT_JOURNAL ):

        PrtPrint( PMG_PRT_RCT_JNL, auchProgRptBuff, usMaxColumn );
        break;

    default:
        return ( FALSE );
    }

    /* --- reset format buffer of current saving item --- */

    memset( auchProgRptBuff, 0x00, sizeof( auchProgRptBuff ));

    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_PrintLoopBegin( PROGRPT_RPTHEAD * pRptHead,
*                                            PROGRPT_LOOPREC * pLoopRec )
*
*   Input   :   PROGRPT_RPTHEAD * pRptHead - Information about prog. report
*               PROGRPT_LOOPREC * pLoopRec - Address of loop-begin record
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*                                   
**  Description:
*    This function prints a loop-begin record.
*==========================================================================
*/
VOID ProgRpt_PrintLoopBegin( PROGRPT_RPTHEAD *pRptHead,
                             PROGRPT_LOOPREC *pLoopRec )
{
    ASSERT( pRptHead != ( PROGRPT_RPTHEAD * )NULL );
    ASSERT( pLoopRec != ( PROGRPT_LOOPREC * )NULL );

    switch ( pLoopRec->uchLoopBrace )  {

    case PROGRPT_LOOP_BEGIN_ROOT:
    case PROGRPT_LOOP_BEGIN_NEST:

        if ( auchProgRptBuff[ 0 ] == '\0' ) {

            ProgRpt_PrintAndFeed( pRptHead, pLoopRec );

            /* --- set up print buffer to print empty line --- */
#ifdef DEBUG
            auchProgRptBuff[ 0 ] = '{';
#else
            auchProgRptBuff[ 0 ] = PROGRPT_PRTCHAR_SPACE;
#endif
            uchProgRptRow = pLoopRec->uchRow;
        }
        break;

    default:
        break;
    }
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_PrintEOF( PROGRPT_RPTHEAD * pRptHead,
*                                      PROGRPT_PRINTREC * pPrintRec )
*
*   Input   :   PROGRPT_RPTHEAD * pRptHead - Information about prog. report
*               PROGRPT_PRINTREC *pPrintRec - Address of EOF item
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*                                   
**  Description:
*    This function prints an End Of File Item.
*==========================================================================
*/
VOID ProgRpt_PrintEOF( PROGRPT_RPTHEAD *pRptHead,
                       PROGRPT_PRINTREC *pPrintRec )
{
    TCHAR   *pszWork;
    TCHAR   *pszDestBuff;

    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );

    if (( pPrintRec->PrintItem.uchGroup == ( UCHAR )NULL ) &&
        ( pPrintRec->PrintItem.uchMajor == ( UCHAR )NULL ) &&
        ( pPrintRec->PrintItem.uchMinor == ( UCHAR )NULL ) &&
        ( pPrintRec->PrintItem.uchItem  == ( UCHAR )NULL )) {

        if ( uchProgRptRow == pPrintRec->uchRow ) {
            return;
        }
        ProgRpt_PrintAndFeed( pRptHead, pPrintRec );

        /* --- fill space print buffer, if NULL character found --- */

        pszDestBuff = &auchProgRptBuff[ pPrintRec->uchColumn - 1 ];
        pszWork     = &auchProgRptBuff[ 0 ];

        while ( pszWork <= pszDestBuff ) {
            if ( *pszWork == '\0' ) {
#ifdef DEBUG
                *pszWork = '_';
#else
                *pszWork = PROGRPT_PRTCHAR_SPACE;
#endif
            }
            pszWork++;
        }
#ifdef DEBUG
        if ( *pszDestBuff == '_' ) {
            /* --- print EOF item for debug --- */
            *pszDestBuff = '';
        }
#endif
    }
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_InitAccumulator( VOID )
*
*   Input   :   Nothing
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*                                   
**  Description:
*    This function initializes the accumulators.
*==========================================================================
*/
VOID ProgRpt_InitAccumulator( VOID )
{
    memset( hugeAccumulator, 0x00, sizeof( hugeAccumulator ));
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_GetAccumulator( PROGRPT_ITEM *pTargetItem,
*                                            D13DIGITS *phugeOutput );
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*   Output  :   D13DIGITS    *phugeOutput - address of output 64 bit value.
*   InOut   :   Nothing
*
**  Return  :   D13DIGITS *phugeValue - accumulator value for specified item.
*                                   
**  Description:
*    This function retrieves an accumulator value which specified at
*   pPrintRec parameter.
*==========================================================================
*/
VOID ProgRpt_GetAccumulator( PROGRPT_ITEM *pTargetItem, D13DIGITS *phugeOutput )
{
    UCHAR   uchMinorCode = 0;   /* minor code for accumulator */

    ASSERT( phugeOutput != NULL );
    ASSERT( pTargetItem != NULL );
    ASSERT( pTargetItem->uchGroup == PROGRPT_GROUP_ACCUM );
    ASSERT(( pTargetItem->uchMajor == PROGRPT_ACCUM_TOTAL ) ||
           ( pTargetItem->uchMajor == PROGRPT_ACCUM_COUNTER ));

    if (pTargetItem->uchMinor > 0) uchMinorCode = pTargetItem->uchMinor - ( UCHAR )1;
    ASSERT( uchMinorCode < PROGRPT_MAX_ACCUMULATOR );

    *phugeOutput = 0;

    switch ( pTargetItem->uchMajor ) {
    case PROGRPT_ACCUM_TOTAL:
        *phugeOutput = hugeAccumulator[ 0 ][ uchMinorCode ];
        break;

    case PROGRPT_ACCUM_COUNTER:
    default:
        *phugeOutput = hugeAccumulator[ 1 ][ uchMinorCode ];
        break;
    }
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_SetAccumulator( PROGRPT_ITEM *pTargetItem,
*                                            D13DIGITS *phugeNewValue )
*
*   Input   :   PROGRPT_ITEM *pTargetItem   - address of target record.
*               D13DIGITS    *phugeNewValue - address of new value
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*                                   
**  Description:
*    This function stores a value to the specified accumulator.
*==========================================================================
*/
VOID ProgRpt_SetAccumulator( PROGRPT_ITEM *pTargetItem, D13DIGITS *phugeNewValue )
{
    UCHAR   uchMinorCode;   /* minor code for accumulator */

    ASSERT( phugeNewValue != NULL );
    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );
    ASSERT( pTargetItem->uchGroup == PROGRPT_GROUP_ACCUM );
    ASSERT(( pTargetItem->uchMajor == PROGRPT_ACCUM_TOTAL ) ||
           ( pTargetItem->uchMajor == PROGRPT_ACCUM_COUNTER ));

    uchMinorCode = pTargetItem->uchMinor - ( UCHAR )1;
    ASSERT( uchMinorCode < PROGRPT_MAX_ACCUMULATOR );

    switch ( pTargetItem->uchMajor ) {

    case PROGRPT_ACCUM_TOTAL:
        hugeAccumulator[ 0 ][ uchMinorCode ] = *phugeNewValue;
        break;

    case PROGRPT_ACCUM_COUNTER:
    default:
        hugeAccumulator[ 1 ][ uchMinorCode ] = *phugeNewValue;
        break;
    }
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_CalcAccumulator( UCHAR uchCondition,
*                                             D13DIGITS *phugeAnswer,
*                                             D13DIGITS *phugeValue1,
*                                             D13DIGITS *phugeValue2 );
*
*   Input   :   UCHAR     uchCondition    - condition of mathematic operation
*               D13DIGITS *phugeValue1    - address of operand1 value
*               D13DIGITS *phugeValue2    - address of operand2 value
*   Output  :   D13DIGITS *phugeAnswer    - address of calculated value
*   InOut   :   Nothing
*
**  Return  :   Nothing
*                                   
**  Description:
*    This function calculates operand1 value and operand2 value.
*==========================================================================
*/
VOID ProgRpt_CalcAccumulator( UCHAR uchCondition,
                              D13DIGITS *phugeAnswer,
                              D13DIGITS *phugeValue1,
                              D13DIGITS *phugeValue2 )
{
    D13DIGITS D13Value1 = 0;    /* 13 digits operand1 value for calculate */
    D13DIGITS D13Value2 = 0;    /* 13 digits operand2 value for calculate */
    LONG    lValue1 = 0;        /* value for calculate (add, subtract) */
    LONG    lValue2 = 0;        /* value for calculate (add, subtract) */

    ASSERT( phugeAnswer != NULL );
    ASSERT( phugeValue1 != NULL );
    ASSERT( phugeValue2 != NULL );
    ASSERT(( uchCondition == PROGRPT_MATH_SET ) ||\
           ( uchCondition == PROGRPT_MATH_ADD ) ||\
           ( uchCondition == PROGRPT_MATH_SUB ) ||\
           ( uchCondition == PROGRPT_MATH_MUL ) ||\
           ( uchCondition == PROGRPT_MATH_DIV ));

    switch ( uchCondition ) {

    case PROGRPT_MATH_SET:
        *phugeAnswer = *phugeValue1;
        break;

    case PROGRPT_MATH_ADD:
        D13Value1 = *phugeValue1;
        D13Value2 = *phugeValue2;
        D13Value1 += D13Value2;
        /* ### MOD (2171 for Win32) V1.0 */
        *phugeAnswer = D13Value1;
        break;

    case PROGRPT_MATH_SUB:
        D13Value1 = *phugeValue1;
        D13Value2 = *phugeValue2;
        D13Value1 -= D13Value2;

		/* ### MOD (2171 for Win32) V1.0 */
        *phugeAnswer = D13Value1;
        break;

    case PROGRPT_MATH_MUL:
        lValue1 = (LONG) *phugeValue1;
        lValue2 = (LONG) *phugeValue2;
        *phugeAnswer = lValue1 * lValue2;
        break;

    case PROGRPT_MATH_DIV:
    default:
        lValue2 = (LONG)*phugeValue2;
        if ( lValue2 != 0L ) {
            * phugeAnswer = *phugeValue1 / lValue2;
        } else {
            /* --- check '0' devide... set 0 to accumulator --- */
            *phugeAnswer = 0;
        }
        break;
    }
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_GetMnemonic( PROGRPT_ITEM *pTargetItem,
*                                         UCHAR        *pszString,
*                                         UCHAR        uchItemKeyNo )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               UCHAR        uchItemKeyNo - adjective no./major dept no.
*   Output  :   UCHAR        *pszString   - address of buffer for string.
*   InOut   :   Nothing
*
**  Return  :   Nothing
*                                   
**  Description:
*    This function retrieves a string data which specified at pTargetItem
*   parameter.
*==========================================================================
*/
VOID ProgRpt_GetMnemonic( PROGRPT_ITEM *pTargetItem,
                          TCHAR *pszString,
                          UCHAR uchItemKeyNo )
{
	PROGRPT_FHEAD   FileInfo = { 0 };       /* information about prog report file */
    UCHAR           uchMnemoAddr;   /* zero based address for mnemonic */
    ULONG           ulReadOffset;   /* read offset for 16/40 char mnemonic */
    ULONG           ulActualRead;   /* actual read size */
    UCHAR           uchMaxStrLen;
    UCHAR           uchI;

    ASSERT( pszString != ( UCHAR * )NULL );
    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );
    ASSERT( pTargetItem->uchGroup == PROGRPT_GROUP_MNEMO );
    ASSERT(( pTargetItem->uchMajor == PROGRPT_MNEMO_TRANS  ) ||\
           ( pTargetItem->uchMajor == PROGRPT_MNEMO_REPORT ) ||\
           ( pTargetItem->uchMajor == PROGRPT_MNEMO_SPECIAL ) ||\
           ( pTargetItem->uchMajor == PROGRPT_MNEMO_ADJ    ) ||\
           ( pTargetItem->uchMajor == PROGRPT_MNEMO_MDEPT  ) ||\
           ( pTargetItem->uchMajor == PROGRPT_MNEMO_16CHAR ) ||\
           ( pTargetItem->uchMajor == PROGRPT_MNEMO_40CHAR ));

    uchMnemoAddr = pTargetItem->uchMinor;

    switch ( pTargetItem->uchMajor ) {

    case PROGRPT_MNEMO_TRANS:
        ASSERT(( 0 < uchMnemoAddr ) && ( uchMnemoAddr <= TRN_ADR_MAX ));
		RflGetTranMnem( pszString, uchMnemoAddr);
        uchMaxStrLen = PARA_TRANSMNEMO_LEN;
        break;        

    case PROGRPT_MNEMO_REPORT:
        ASSERT(( 0 < uchMnemoAddr ) && ( uchMnemoAddr <= RPT_ADR_MAX ));
		RflGetReportMnem( pszString, uchMnemoAddr);
        uchMaxStrLen = PARA_REPORTNAME_LEN;
        break;        

    case PROGRPT_MNEMO_SPECIAL:
        ASSERT(( 0 < uchMnemoAddr ) && ( uchMnemoAddr <= SPC_ADR_MAX ));
		RflGetSpecMnem( pszString, uchMnemoAddr);
        uchMaxStrLen = PARA_SPEMNEMO_LEN;
        break;        

    case PROGRPT_MNEMO_ADJ:
        ASSERT( uchMnemoAddr <= ADJ_ADR_MAX );
        if (( uchMnemoAddr == 0 ) && ( uchItemKeyNo == 0 )) {
            memset( pszString, 0x00, (PARA_ADJMNEMO_LEN + 1)*sizeof(TCHAR) );
        } else {
            if ( uchMnemoAddr == 0 ) {
				RflGetAdj(pszString, uchItemKeyNo);
			} else {
				RflGetAdj( pszString, uchMnemoAddr);
            }
        }
        uchMaxStrLen = PARA_ADJMNEMO_LEN;
        break;

    case PROGRPT_MNEMO_MDEPT:
        ASSERT( uchMnemoAddr <= MDPT_ADR_MAX );
        if ( uchMnemoAddr == 0 ) {
			RflGetMajorDeptMnem(pszString, uchItemKeyNo);
		} else {
			RflGetMajorDeptMnem( pszString, uchMnemoAddr);
        }
        uchMaxStrLen = PARA_MAJORDEPT_LEN;
        break;

    case PROGRPT_MNEMO_16CHAR:

        /* --- calculate actual offset of target mnemonic --- */

        uchMnemoAddr -= 1;
        ASSERT( uchMnemoAddr < PROGRPT_MAX_16CHAR_ADR );

        ProgRpt_GetFileHeader( &FileInfo );
        ulReadOffset = FileInfo.ulOffset16Char + (PROGRPT_MAX_MARK_LEN * sizeof(TCHAR)) + sizeof( USHORT );
        ulReadOffset += ( (PROGRPT_MAX_16CHAR_LEN * sizeof(TCHAR)) * uchMnemoAddr );

        /* --- retrieve target mnemonic from program report file --- */

        ProgRpt_ReadFile( ulReadOffset, pszString, PROGRPT_MAX_16CHAR_LEN * sizeof(TCHAR), &ulActualRead );
        if ( ulActualRead != PROGRPT_MAX_16CHAR_LEN * sizeof(TCHAR)) {
            ASSERT( ulActualRead == PROGRPT_MAX_16CHAR_LEN * sizeof(TCHAR));
            *pszString = '\0';
            return;
        }

        *( pszString + PROGRPT_MAX_16CHAR_LEN ) = '\0';
        uchMaxStrLen = PROGRPT_MAX_16CHAR_LEN;

        break;        

    case PROGRPT_MNEMO_40CHAR:

        uchMnemoAddr -= 1;
        ASSERT( uchMnemoAddr < PROGRPT_MAX_40CHAR_ADR );

        /* --- calculate actual offset of target mnemonic --- */

        ProgRpt_GetFileHeader( &FileInfo );
        ulReadOffset = FileInfo.ulOffset40Char + (PROGRPT_MAX_MARK_LEN * sizeof(TCHAR)) + sizeof( USHORT );
        ulReadOffset += ( (PROGRPT_MAX_40CHAR_LEN * sizeof(TCHAR)) * uchMnemoAddr );

        /* --- retrieve target mnemonic from program report file --- */

		//11-11-03 JHHJ SR 201 Parameter 4 change from USHORT to ULONG
        ProgRpt_ReadFile( ulReadOffset, pszString, PROGRPT_MAX_40CHAR_LEN * sizeof(TCHAR), &ulActualRead );
        if ( ulActualRead != PROGRPT_MAX_40CHAR_LEN * sizeof(TCHAR)) {
            ASSERT( ulActualRead == PROGRPT_MAX_40CHAR_LEN * sizeof(TCHAR));
            *pszString = '\0';
            return;
        }

        *( pszString + PROGRPT_MAX_40CHAR_LEN ) = '\0';
        uchMaxStrLen = PROGRPT_MAX_16CHAR_LEN;

        break;        

    default:
        *pszString = '\0';
        break;
    }

    /* --- fill space, if NULL character found --- */

    uchI = 0;
    while (( uchI < uchMaxStrLen ) && ( pszString[ uchI ] == '\0' )) {
#ifdef DEBUG
        pszString[ uchI ] = _T('.');
#else
        pszString[ uchI ] = PROGRPT_PRTCHAR_SPACE;
#endif
        uchI++;
    }
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_IntegerToString( PROGRPT_PRINTREC *pPrintRec,
*                                             D13DIGITS        *pD13Digit,
*                                             UCHAR            *pszDestBuff )
*
*   Input   :   PROGRPT_PRINTREC *pPrintRec - address of target record.
*               D13DIGITS    *pD13Digit   - address of source integer data.
*               UCHAR        *pszDestBuff - address of destination buffer.
*                                               inquire 20 char buffer size
*   -$X,XXX,XXX,XXX,XXX     = 13 digit integer (2 + 13 + 4 + NULL char)
*     -$XXX,XXX,XXX,XXX.X   = 12 digit/1 digit (2 + 13 + 4 + NULL char)
*      -$XX,XXX,XXX,XXX.XX  = 11 digit/2 digit (2 + 13 + 4 + NULL char)
*       -$X,XXX,XXX,XXX.XXX = 10 digit/3 digit (2 + 13 + 4 + NULL char)
*
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*                                   
**  Description:
*    This function converts long value to string data with specified format
*   style.
*==========================================================================
*/
VOID ProgRpt_IntegerToString( PROGRPT_PRINTREC *pPrintRec,
                              D13DIGITS        *pD13Digit,
                              TCHAR            *pszDestBuff )
{
    TCHAR   uchSign    = '\0';  /* minus sign character code */
    TCHAR   uchModChar = '\0';  /* modifier character code */
    TCHAR   uchPercent = '\0';  /* percent character code */
    BOOL    fWithComma;         /* determine form with comma */
    UCHAR   uchAllValLen;       /* length of all 13 digit */
    /* UCHAR   uchHighLen;*/    /* length of high 4 digit */
    /* UCHAR   uchLowLen; */    /* length of low 9 digit */
    UCHAR   uchIntegerLen;      /* length of integer value */
    UCHAR   uchDecimalLen;      /* length of decimal value */
    /* UCHAR   szLowDigit[ PROGRPT_NUMFORM_WORK_SIZE ]; */
                                /* digit string data work buffer */
    TCHAR   szHighDigit[ PROGRPT_NUMFORM_WORK_SIZE ];
                                /* digit string data work buffer */
    TCHAR   *pszStart;
    UCHAR   uchLoopCo;

    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );
    ASSERT( pD13Digit != ( D13DIGITS * )NULL );
    ASSERT( pszDestBuff != ( TCHAR )NULL );

    switch ( pPrintRec->auchFormat[ 0 ] ) {

    case PROGRPT_PRT_0:
    case PROGRPT_PRT_0_0:
    case PROGRPT_PRT_0_00:
    case PROGRPT_PRT_0_000:
        switch ( pPrintRec->auchFormat[ 0 ] ) {
        case PROGRPT_PRT_0:
            uchDecimalLen = 0;
            break;
        case PROGRPT_PRT_0_0:
            uchDecimalLen = 1;
            break;
        case PROGRPT_PRT_0_00:
            uchDecimalLen = 2;
            break;
        case PROGRPT_PRT_0_000:
        default:
            uchDecimalLen = 3;
            break;
        }
        fWithComma = FALSE;
        break;

    case PROGRPT_PRT_CMA_0:
    case PROGRPT_PRT_CMA_0_0:
    case PROGRPT_PRT_CMA_0_00:
    case PROGRPT_PRT_CMA_0_000:
        switch ( pPrintRec->auchFormat[ 0 ] ) {
        case PROGRPT_PRT_CMA_0:
            uchDecimalLen = 0;
            break;
        case PROGRPT_PRT_CMA_0_0:
            uchDecimalLen = 1;
            break;
        case PROGRPT_PRT_CMA_0_00:
            uchDecimalLen = 2;
            break;
        case PROGRPT_PRT_CMA_0_000:
        default:
            uchDecimalLen = 3;
            break;
        }
        fWithComma = TRUE;
        break;

    case PROGRPT_PRT_D_0:
    case PROGRPT_PRT_D_0_0:
    case PROGRPT_PRT_D_0_00:
    case PROGRPT_PRT_D_0_000:
        switch ( pPrintRec->auchFormat[ 0 ] ) {
        case PROGRPT_PRT_D_0:
            uchDecimalLen = 0;
            break;
        case PROGRPT_PRT_D_0_0:
            uchDecimalLen = 1;
            break;
        case PROGRPT_PRT_D_0_00:
            uchDecimalLen = 2;
            break;
        case PROGRPT_PRT_D_0_000:
        default:
            uchDecimalLen = 3;
            break;
        }
        uchModChar = PROGRPT_PRTCHAR_DOLLER;
        fWithComma = FALSE;
        break;

    case PROGRPT_PRT_D_CMA_0:
    case PROGRPT_PRT_D_CMA_0_0:
    case PROGRPT_PRT_D_CMA_0_00:
    case PROGRPT_PRT_D_CMA_0_000:
        switch ( pPrintRec->auchFormat[ 0 ] ) {
        case PROGRPT_PRT_D_CMA_0:
            uchDecimalLen = 0;
            break;
        case PROGRPT_PRT_D_CMA_0_0:
            uchDecimalLen = 1;
            break;
        case PROGRPT_PRT_D_CMA_0_00:
            uchDecimalLen = 2;
            break;
        case PROGRPT_PRT_D_CMA_0_000:
        default:
            uchDecimalLen = 3;
            break;
        }
        uchModChar = PROGRPT_PRTCHAR_DOLLER;
        fWithComma = TRUE;
        break;

    case PROGRPT_PRT_A_0:
    case PROGRPT_PRT_A_0_0:
    case PROGRPT_PRT_A_0_00:
    case PROGRPT_PRT_A_0_000:
        switch ( pPrintRec->auchFormat[ 0 ] ) {
        case PROGRPT_PRT_A_0:
            uchDecimalLen = 0;
            break;
        case PROGRPT_PRT_A_0_0:
            uchDecimalLen = 1;
            break;
        case PROGRPT_PRT_A_0_00:
            uchDecimalLen = 2;
            break;
        case PROGRPT_PRT_A_0_000:
        default:
            uchDecimalLen = 3;
            break;
        }
        uchModChar = PROGRPT_PRTCHAR_AT;
        fWithComma = FALSE;
        break;

    case PROGRPT_PRT_A_CMA_0:
    case PROGRPT_PRT_A_CMA_0_0:
    case PROGRPT_PRT_A_CMA_0_00:
    case PROGRPT_PRT_A_CMA_0_000:
        switch ( pPrintRec->auchFormat[ 0 ] ) {
        case PROGRPT_PRT_A_CMA_0:
            uchDecimalLen = 0;
            break;
        case PROGRPT_PRT_A_CMA_0_0:
            uchDecimalLen = 1;
            break;
        case PROGRPT_PRT_A_CMA_0_00:
            uchDecimalLen = 2;
            break;
        case PROGRPT_PRT_A_CMA_0_000:
        default:
            uchDecimalLen = 3;
            break;
        }
        uchModChar = PROGRPT_PRTCHAR_AT;
        fWithComma = TRUE;
        break;

    case PROGRPT_PRT_N_0:
    case PROGRPT_PRT_N_0_0:
    case PROGRPT_PRT_N_0_00:
    case PROGRPT_PRT_N_0_000:
        switch ( pPrintRec->auchFormat[ 0 ] ) {
        case PROGRPT_PRT_N_0:
            uchDecimalLen = 0;
            break;
        case PROGRPT_PRT_N_0_0:
            uchDecimalLen = 1;
            break;
        case PROGRPT_PRT_N_0_00:
            uchDecimalLen = 2;
            break;
        case PROGRPT_PRT_N_0_000:
        default:
            uchDecimalLen = 3;
            break;
        }
        uchModChar = PROGRPT_PRTCHAR_NUM;
        fWithComma = FALSE;
        break;

    case PROGRPT_PRT_N_CMA_0:
    case PROGRPT_PRT_N_CMA_0_0:
    case PROGRPT_PRT_N_CMA_0_00:
    case PROGRPT_PRT_N_CMA_0_000:
        switch ( pPrintRec->auchFormat[ 0 ] ) {
        case PROGRPT_PRT_N_CMA_0:
            uchDecimalLen = 0;
            break;
        case PROGRPT_PRT_N_CMA_0_0:
            uchDecimalLen = 1;
            break;
        case PROGRPT_PRT_N_CMA_0_00:
            uchDecimalLen = 2;
            break;
        case PROGRPT_PRT_N_CMA_0_000:
        default:
            uchDecimalLen = 3;
            break;
        }
        uchModChar = PROGRPT_PRTCHAR_NUM;
        fWithComma = TRUE;
        break;

    case PROGRPT_PRT_0_PER:
    case PROGRPT_PRT_0_0_PER:
    case PROGRPT_PRT_0_00_PER:
    case PROGRPT_PRT_0_000_PER:
        switch ( pPrintRec->auchFormat[ 0 ] ) {
        case PROGRPT_PRT_0_PER:
            uchDecimalLen = 0;
            break;
        case PROGRPT_PRT_0_0_PER:
            uchDecimalLen = 1;
            break;
        case PROGRPT_PRT_0_00_PER:
            uchDecimalLen = 2;
            break;
        case PROGRPT_PRT_0_000_PER:
        default:
            uchDecimalLen = 3;
            break;
        }
        uchPercent = PROGRPT_PRTCHAR_PER;
        fWithComma = FALSE;
        break;

    case PROGRPT_PRT_CMA_0_PER:
    case PROGRPT_PRT_CMA_0_0_PER:
    case PROGRPT_PRT_CMA_0_00_PER:
    case PROGRPT_PRT_CMA_0_000_PER:
        switch ( pPrintRec->auchFormat[ 0 ] ) {
        case PROGRPT_PRT_CMA_0_PER:
            uchDecimalLen = 0;
            break;
        case PROGRPT_PRT_CMA_0_0_PER:
            uchDecimalLen = 1;
            break;
        case PROGRPT_PRT_CMA_0_00_PER:
            uchDecimalLen = 2;
            break;
        case PROGRPT_PRT_CMA_0_000_PER:
        default:
            uchDecimalLen = 3;
            break;
        }
        uchPercent = PROGRPT_PRTCHAR_PER;
        fWithComma = TRUE;
        break;

    case PROGRPT_PRT_STRING:
    default:
        *pszDestBuff = '\0';
        return;
    }

    /* ### MOD (2171 for Win32) V1.0 */
    if( *pD13Digit < 0 ){
        uchSign = PROGRPT_PRTCHAR_MINUS;
        (*pD13Digit) *= -1;
    }
    RflD13ToDecimalStr(pD13Digit,szHighDigit); /* convert D13DIGITS number to decimal string */
    uchAllValLen = (UCHAR)_tcslen(szHighDigit);


    if ( 13 < uchAllValLen ) {
        uchLoopCo = 1;
        while ( szHighDigit[ uchLoopCo ] == PROGRPT_PRTCHAR_ZERO ) {
            uchLoopCo++;
        }
        uchAllValLen -= uchLoopCo;
        memmove( szHighDigit,
                 &szHighDigit[ uchLoopCo ],
                 uchAllValLen * sizeof(TCHAR));
        szHighDigit[ uchAllValLen ] = ( UCHAR )NULL;
    }

    if ( uchSign != '\0' ) {

        /* --- copy minus char (-) to destination buffer --- */

        *pszDestBuff = uchSign;
        pszDestBuff++;
    }

    if ( uchModChar != '\0' ) {

        /* --- copy modifier (#,$,@) to destination buffer --- */

        *pszDestBuff = uchModChar;
        pszDestBuff++;
    }

    pszStart = &szHighDigit[ 0 ];

    if ( uchDecimalLen < uchAllValLen ) {

        /* --- set integer value to destination buffer
            with/without comma --- */

        uchIntegerLen = uchAllValLen - uchDecimalLen;

        for ( uchLoopCo = uchIntegerLen; 0 < uchLoopCo; uchLoopCo-- ) {

            *pszDestBuff = *pszStart;
            pszDestBuff++;
            pszStart++;

            /* --- set comma (',') to destination buffer --- */

            if ( fWithComma &&
                (( uchLoopCo != 1 ) && (( uchLoopCo % 3 ) == 1 )) ) {
                *pszDestBuff = PROGRPT_PRTCHAR_COMMA;
                pszDestBuff++;
            }
        }

        /* --- set decimal point and decimal value,
            if decimal form is specified --- */

        if ( 0 < uchDecimalLen ) {
            *pszDestBuff = PROGRPT_PRTCHAR_DECIMAL;
            pszDestBuff++;
        }

        for ( uchLoopCo = 0; uchLoopCo < uchDecimalLen; uchLoopCo++ ) {
            *pszDestBuff = *pszStart;
            pszStart++;
            pszDestBuff++;
        }

    } else {

        if ( 0 < uchDecimalLen ) {  /* set "0." to buffer */
            *pszDestBuff = PROGRPT_PRTCHAR_ZERO;
            pszDestBuff++;
            *pszDestBuff = PROGRPT_PRTCHAR_DECIMAL;
            pszDestBuff++;
        }

        for ( uchLoopCo = uchDecimalLen; 0 < uchLoopCo; uchLoopCo-- ) {
            if ( uchAllValLen < uchLoopCo ) {
                *pszDestBuff = PROGRPT_PRTCHAR_ZERO;
                pszDestBuff++;
            } else {
                *pszDestBuff = *pszStart;
                pszDestBuff++;
                pszStart++;
            }
        }
    }

    if ( uchPercent != '\0' ) {

        /* --- copy percent (%) char to destination buffer --- */

        *pszDestBuff = uchPercent;
        pszDestBuff++;
    }

    /* --- make NULL terminated string --- */

    *pszDestBuff = '\0';
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_SetDateTimeBuff( PROGRPT_PRINTREC *pPrintRec,
*                                             DATE *pDateTime )
*
*   Input   :   PROGRPT_PRINTREC *pPrintRec - address of target record.
*               DATE         *pDateTime   - address of date/time data.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*                                   
**  Description:
*    This function sets date/time data to global print buffer.
*==========================================================================
*/
VOID ProgRpt_SetDateTimeBuff( PROGRPT_PRINTREC *pPrintRec, N_DATE *pDateTime )
{
    TCHAR   szDateForm[]   = _T("%02u/%02u");     /* print format for date data */
    TCHAR   szTimeForm[]   = _T(" %2u:%02u%s"); /* print format for time data */
    TCHAR   szTimeForm24[] = _T(" %2u:%02u");   /* print format for time data */
    TCHAR   szDateTime[ PROGRPT_DATETIME_WORK_SIZE ];
                                            /* date/time formatted buffer */
    USHORT  usAMPMHour;                     /* hour data in AM/PM format */
    TCHAR   *pszAMPMSymbol;                 /* address for AM/PM string */
    UCHAR   uchStrLen;                      /* string length without NULL */

    ASSERT( pDateTime != ( N_DATE * )NULL );
    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );
    ASSERT( pPrintRec->PrintItem.uchGroup == PROGRPT_GROUP_TOTALCO );

    memset( szDateTime, 0x00, sizeof( szDateTime ));

    /* --- check print format (MDC addr 14, bit 2) --- */

    if ( CliParaMDCCheck( MDC_DRAWER_ADR, EVEN_MDC_BIT2 )) {

        /* --- print format is DD/MM --- */

        RflSPrintf( szDateTime,
                    PROGRPT_DATETIME_WORK_SIZE,
                    szDateForm,
                    pDateTime->usMDay,
                    pDateTime->usMonth );

    } else {

        /* --- print format is MM/DD --- */

        RflSPrintf( szDateTime,
                    PROGRPT_DATETIME_WORK_SIZE,
                    szDateForm,
                    pDateTime->usMonth,
                    pDateTime->usMDay );
    }

    /* --- check print format (MDC addr 14, bit 3) --- */

    if ( CliParaMDCCheck( MDC_DRAWER_ADR, EVEN_MDC_BIT3 )) {

        /* --- print format is militaly (0...23) --- */

        RflSPrintf( &szDateTime[ _tcslen( szDateTime ) ],
                    PROGRPT_DATETIME_WORK_SIZE,
                    szTimeForm24,
                    pDateTime->usHour,
                    pDateTime->usMin );

    } else {

        /* --- print format is AM/PM (1...12) --- */

        if ( pDateTime->usHour == 0 ) {         /* 0:00 */

            usAMPMHour = 12;
            pszAMPMSymbol = auchPROGRPT_AM_SYMBOL; /* 06/02/99 */

        } else if ( pDateTime->usHour < 12 ) {  /* 1:00 ... 11:00 */

            usAMPMHour    = pDateTime->usHour;
            pszAMPMSymbol = auchPROGRPT_AM_SYMBOL; /* 06/02/99 */

        } else if ( pDateTime->usHour == 12 ) { /* 12:00 */

            usAMPMHour = 12;
            pszAMPMSymbol = auchPROGRPT_PM_SYMBOL; /* 06/02/99 */

        } else if ( 12 < pDateTime->usHour ) {  /* 13:00...23:00 */

            usAMPMHour    = pDateTime->usHour - 12;
            pszAMPMSymbol = auchPROGRPT_PM_SYMBOL; /* 06/02/99 */
        }

        RflSPrintf( &szDateTime[ _tcslen( szDateTime ) ],
                    PROGRPT_DATETIME_WORK_SIZE,
                    szTimeForm,
                    usAMPMHour,
                    pDateTime->usMin,
                    pszAMPMSymbol );
    }

    /* --- format date/time data and store print buffer --- */

    uchStrLen = ( UCHAR )_tcslen( szDateTime );
    ProgRpt_FormatPrintBuff( pPrintRec,
                             szDateTime,
                             uchStrLen,
                             PROGRPT_TYPE_DATE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_GetCurDateTime( DATE *pDateTime )
*
*   Input   :   DATE    *pDateTime   - address of date/time data.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*                                   
**  Description:
*    This function gets current date/time data.
*==========================================================================
*/
BOOL ProgRpt_GetCurDateTime( N_DATE *pDateTime )
{
    DATE_TIME  DateTime;
    
    PifGetDateTime( &DateTime );

    pDateTime->usMin   = DateTime.usMin;
    pDateTime->usHour  = DateTime.usHour;
    pDateTime->usMDay  = DateTime.usMDay;
    pDateTime->usMonth = DateTime.usMonth;

    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_FormatPrintBuff( PROGRPT_PRINTREC *pPrintRec,
*                                             UCHAR  *pszSource,
*                                             UCHAR  uchSrcStrLen,
*                                             UCHAR  uchDataType )
*
*   Input   :   PROGRPT_PRINTREC *pPrintRec - address of target record.
*               UCHAR   *pszSource   - address of source string.
*               UCHAR   uchSrcStrLen - length of source string.
*               UCHAR   uchDataType  - original data type of string.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*                                   
**  Description:
*    This function formats source string with specified format and sets
*   to print buffer.
*==========================================================================
*/
VOID ProgRpt_FormatPrintBuff( PROGRPT_PRINTREC *pPrintRec,
                              TCHAR *pszSource,
                              UCHAR uchSrcStrLen,
                              UCHAR uchDataType )
{
    TCHAR   szWorkBuff[ PROGRPT_MAX_40CHAR_LEN * 2 + 1 ];
                                        /* single/double wide buffer */
    TCHAR   *pszDestBuff;               /* address for print buffer */
    TCHAR   *pszWork;                   /* address for print buffer */
    UCHAR   uchCopyLen;                 /* size of source string to copy */
    UCHAR   uchStrLen;                  /* string length without NULL */
    BOOL    fDoubleWide = FALSE;

    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );
    ASSERT( pszSource != ( UCHAR * )NULL );

    /* --- is date/time data printed in double wide ? --- */

    if (( pPrintRec->auchFormat[ 1 ] & PROGRPT_PRINT_DOUBLE ) &&
        ( uchDataType != PROGRPT_TYPE_STRING )) {

        /* --- if original data is mnemonic data with double wide char,
            prohibit convert double wide char twice --- */

        PrtDouble( szWorkBuff, ( PROGRPT_MAX_40CHAR_LEN * 2 + 1 ), pszSource );
        fDoubleWide = TRUE;

    } else {

        _tcsncpy( szWorkBuff, pszSource, uchSrcStrLen );
        szWorkBuff[ uchSrcStrLen ] = '\0';
    }

    /* --- copy formatted date/time data to print buffer --- */

    pszDestBuff = auchProgRptBuff + pPrintRec->uchColumn - 1;

    if ( PROGRPT_MAX_COLUMN <
        ( pPrintRec->uchColumn - ( UCHAR )1 + pPrintRec->uchMaxColumn )) {
        uchCopyLen = ( UCHAR )( PROGRPT_MAX_COLUMN - pPrintRec->uchColumn - 1 );
    } else {
        uchCopyLen = pPrintRec->uchMaxColumn;
    }

    if ( pPrintRec->auchFormat[ 1 ] & PROGRPT_PRINT_LEFT ) {

        /* --- set date/time data to print buffer with left adjust --- */

        _tcsncpy( pszDestBuff, szWorkBuff, uchCopyLen );

        if ( fDoubleWide && ( uchCopyLen % 2 )) {

            /* --- reset double wide character --- */
#ifdef DEBUG
            pszDestBuff[ uchCopyLen - 1 ] = '';
#else
            pszDestBuff[ uchCopyLen - 1 ] = PROGRPT_PRTCHAR_SPACE;
#endif
        }
    }

    if ( pPrintRec->auchFormat[ 1 ] & PROGRPT_PRINT_RIGHT ) {

        /* --- set date/time data to print buffer with right adjust --- */

        uchStrLen = ( UCHAR )_tcslen( szWorkBuff );

        if ( uchStrLen < uchCopyLen ) {

            pszDestBuff += ( uchCopyLen - uchStrLen );
            uchCopyLen  = uchStrLen;

        } else if ( uchStrLen == uchCopyLen ) {

            /* do nothing */

        } else {    /* uchCopyLen < uchStrLen */

            memmove( szWorkBuff,
                     szWorkBuff + ( uchStrLen - uchCopyLen ),
                     uchCopyLen * sizeof(TCHAR) );
            szWorkBuff[ uchCopyLen ] = '\0';
        }
        _tcsncpy( pszDestBuff, szWorkBuff, uchCopyLen );

        if ( fDoubleWide && ( uchCopyLen % 2 )) {

            /* --- reset double wide character --- */
#ifdef DEBUG
            pszDestBuff[ 0 ] = '';
#else
            pszDestBuff[ 0 ] = PROGRPT_PRTCHAR_SPACE;
#endif
        }
    }

    /* --- fill space print buffer, if NULL character found --- */

    pszWork = auchProgRptBuff;
    while ( pszWork != pszDestBuff ) {
        if ( *pszWork == '\0' ) {
#ifdef DEBUG
            *pszWork = '_';
#else
            *pszWork = PROGRPT_PRTCHAR_SPACE;
#endif
        }
        pszWork++;
    }
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_IsPrintCondTRUE( UCHAR uchCondition,
*                                             D13DIGITS *pD13Value1,
*                                             D13DIGITS *pD13Value2 )
*
*   Input   :   UCHAR     uchCondition  - type of print condition
*               D13DIGITS *pD13Value1   - address of operand 1 value
*               D13DIGITS *pD13Value2   - address of operand 2 value
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - print condition is TRUE.
*                       FALSE   - print confition is FALSE.
*                                   
**  Description:
*    This function determines that specified print condition is TRUE.
*==========================================================================
*/
BOOL ProgRpt_IsPrintCondTRUE( UCHAR uchCondition,
                              D13DIGITS *pD13Value1,
                              D13DIGITS *pD13Value2 )
{
    BOOL    fRetValue;  /* return value for user application */

    switch ( uchCondition ) {

    case PROGRPT_PRTCOND_NOTZERO:   /* ope1 != 0 */
        /* ### MOD (2171 for Win32) V1.0 */
	if (*pD13Value1 != 0) {
            fRetValue = TRUE;
        } else {
            fRetValue = FALSE;
        }
        break;

    case PROGRPT_PRTCOND_LESS:      /* ope1 <  ope2 */
        /* ### MOD (2171 for Win32) V1.0 */
	if (pD13Value1 < pD13Value2)
		fRetValue = TRUE;
	else
		fRetValue = FALSE;
        break;

    case PROGRPT_PRTCOND_LESSEQ:    /* ope1 <= ope2 */
        /* ### MOD (2171 for Win32) V1.0 */
	if ( *pD13Value1 <= *pD13Value2 )
		fRetValue = TRUE;
	else
		fRetValue = FALSE;
        break;

    case PROGRPT_PRTCOND_GREAT:     /* ope1 >  ope2 */
        /* ### MOD (2171 for Win32) V1.0 */
	if ( *pD13Value1 > *pD13Value2 )
		fRetValue = TRUE;
	else
		fRetValue = FALSE;
        break;

    case PROGRPT_PRTCOND_GREATEQ:   /* ope1 >= ope2 */
        /* ### MOD (2171 for Win32) V1.0 */
	if ( *pD13Value1 >= *pD13Value2 )
		fRetValue = TRUE;
	else
		fRetValue = FALSE;
        break;

    case PROGRPT_PRTCOND_EQUAL:     /* ope1 == ope2 */
        /* ### MOD (2171 for Win32) V1.0 */
	if ( *pD13Value1 == *pD13Value2 )
		fRetValue = TRUE;
	else
		fRetValue = FALSE;
        break;

    case PROGRPT_PRTCOND_NOEQUAL:   /* ope1 != ope2 */
        /* ### MOD (2171 for Win32) V1.0 */
	if ( *pD13Value1 != *pD13Value2 )
		fRetValue = TRUE;
	else
		fRetValue = FALSE;
        break;

    case PROGRPT_PRTCOND_ANDZERO:   /* ope1 != 0 && ope2 != 0 */
        /* ### MOD (2171 for Win32) V1.0 */
	if ((*pD13Value1 != 0) && (*pD13Value2 != 0))
		fRetValue = TRUE;
	else
		fRetValue = FALSE;
        break;

    case PROGRPT_PRTCOND_ORZERO:    /* ope1 != 0 || ope2 != 0 */
        /* ### MOD (2171 for Win32) V1.0 */
	if ((*pD13Value1 != 0) || (*pD13Value2 != 0))
		fRetValue = TRUE;
	else
		fRetValue = FALSE;
        break;

    default:
        fRetValue = FALSE;
        break;
    }
    return ( fRetValue );
}


/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_ConvertError( SHORT sRetCode )
*
*   Input   :   SHORT sRetCode - Return code for user application.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sLDTCode - SUCCESS
*                                LDT_NTINFL_ADR
*                                LDT_KEYOVER_ADR
*                                LDT_ERR_ADR
*                                LDT_PROHBT_ADR
*                                   
**  Description:
*    This function converts return code to lead through code.
*==========================================================================
*/
SHORT ProgRpt_ConvertError( SHORT sRetCode )
{
    SHORT sLDTCode;     /* lead through code */

    switch ( sRetCode ) {

    case PROGRPT_SUCCESS:           /* success */
    case PROGRPT_END_OF_FILE:       /* end of file */
        sLDTCode = SUCCESS;
        break;
        
    case PROGRPT_FILE_NOT_FOUND:
        sLDTCode = LDT_PROHBT_ADR;  /* 10 * Prohibit Operation */
        break;

    case PROGRPT_NOT_IN_FILE:
        sLDTCode = LDT_NTINFL_ADR;  /*  1 * Not in file */
        break;

    case PROGRPT_LOCK:
        sLDTCode = LDT_LOCK_ADR;    /* 22 * During Lock */
        break;

    case PROGRPT_NO_MAKE_FILE:
        sLDTCode = LDT_FLFUL_ADR;   /*  2 * File Full */
        break;

    default:
        sLDTCode = LDT_ERR_ADR;     /* 21 * Error(Global) */
        break;
    }

    return ( sLDTCode );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_PluIsAll( UCHAR *puchPluNo )
*
*   Input   :   UCHAR *puchPluNo    - PLU Number
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - All PLU Read.
*                       FALSE   - Specified PLU read
*
**  Description:
*    This function determines that specified loop condition is TRUE.
*==========================================================================
*/
BOOL ProgRpt_PluIsAll( TCHAR *puchPluNo )
{
    if ( puchPluNo == ( TCHAR * )NULL ) {  /* saratoga */
        return ( TRUE );
    }

    if ( puchPluNo[0] == 0 && puchPluNo[1] == 0 && puchPluNo[2] == 0 && puchPluNo[3] == 0 &&
        puchPluNo[4] == 0 && puchPluNo[5] == 0 && puchPluNo[6] == 0 && puchPluNo[7] == 0 &&
        puchPluNo[8] == 0 && puchPluNo[9] == 0 && puchPluNo[10] == 0 && puchPluNo[11] == 0 &&
        puchPluNo[12] == 0 ) {
        return ( TRUE );
    } else {
        return ( FALSE );
    }
}


/* ===== End of File (_PROGCMN.C) ===== */
