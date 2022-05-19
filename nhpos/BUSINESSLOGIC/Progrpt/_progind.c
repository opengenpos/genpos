/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*=======================================================================*\
*   Title              : Programmable Report module, Program List
*   Category           : Prog. Report, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : _PROGIND.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : The provided function names are as follows.
*
*   ProgRpt_IndPrint()          - print current/saved daily ind-fin. total.
*   ProgRpt_IndReset()          - reset current daily to saved total file.
*   ProgRpt_IndIsResettable()   - determine current daily is resettable.
*   ProgRpt_IndIsBadLoopKey()   - determine loop key is valid or invalid.
*   ProgRpt_IndPrintResetTermID()- print resetted terminal ID.
*
*   ProgRpt_IndLOOP()           - execute loop operation in ind-fin. ttl.
*
*   ProgRpt_IndFindIndFin()     - find ind-fin total by terminal no.
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
*  Date     :  Ver.Rev : NAME       : Description
* Feb-29-96 : 03.01.00 : T.Nakahata : Initial
* Mar-29-96 : 03.01.02 : T.Nakahata : check TTL_NOTINFILE in IsResettable
* May-24-96 : 03.01.07 : T.Nakahata : Add resetted terminal# print routine
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
#include    <pif.h>
#include    <log.h>
#include    <appllog.h>
#include    <paraequ.h>
#include    <para.h>
#include    <pmg.h>
#include    <rfl.h>
#include    <csttl.h>
#include    <csop.h>
#include    <cscas.h>
#include    <csstbopr.h>
#include    <csstbpar.h>
#include    <csstbttl.h>
#include    <transact.h>
#include    <prt.h>
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
**  Synopsis:   SHORT ProgRpt_IndPrint( UCHAR uchProgRptNo,
*                                       UCHAR uchMinorClass,
*                                       USHORT usTerminalNo )
*
*   Input   :   UCHAR uchProgRptNo  - programmable report no.
*               UCHAR uchMinorClass - minor class for ind-financial total.
*               USHORT usTerminalNo - target terminal no.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*                                 LDT_KEYOVER_ADR
*                                 LDT_ERR_ADR       ( TTL_FAIL )
*                                 LDT_PROHBT_ADR    ( OP_FILE_NOT_FOUND )
*                                   
**  Description:
*    This function prints specified individual financial report.
*==========================================================================
*/
SHORT ProgRpt_IndPrint( UCHAR uchProgRptNo,
                        UCHAR uchMinorClass,
                        USHORT usTerminalNo )
{
    SHORT   sRetCode;           /* return code */
    PROGRPT_FHEAD   FileInfo;   /* information about prog report file */
    PROGRPT_RPTHEAD ReportInfo; /* information about report format */
    USHORT  usMaxNoOfRec;       /* maximum no. of record in report format */
    USHORT  usLoopIndex;        /* loop index for format record */
    ULONG   ulReadOffset;       /* read offset for format record */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) || ( uchMinorClass == CLASS_TTLSAVDAY ));
    ASSERT( ! ProgRpt_IndIsBadLoopKey( &sRetCode, uchProgRptNo, usTerminalNo ));

    /* --- determine specified report format is correct or incorrect --- */
    if (( sRetCode = ProgRpt_GetProgRptInfo( uchProgRptNo, &ReportInfo )) != PROGRPT_SUCCESS ) {
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    if ( ReportInfo.usFileType != PROGRPT_FILE_INDFIN ) {
        return ( LDT_PROHBT_ADR );
    }

    if ( ReportInfo.usDevice == PROGRPT_DEVICE_MLD ) {
        if ( uchMinorClass == CLASS_TTLSAVDAY ) {
            return ( LDT_PROHBT_ADR );
        }
    }

    /* --- initialize current row item before print cashier report --- */
    memset( auchProgRptBuff, 0x00, sizeof( auchProgRptBuff ));
    uchProgRptColumn = 1;
    uchProgRptRow    = 1;

    /* --- read a record until end of file --- */
    ProgRpt_GetFileHeader( &FileInfo );
    ulReadOffset = FileInfo.aulOffsetReport[ uchProgRptNo - 1 ] + sizeof( PROGRPT_RPTHEAD );
    usMaxNoOfRec = ReportInfo.usNoOfLoop + ReportInfo.usNoOfPrint + ReportInfo.usNoOfMath;
    for ( usLoopIndex = 0; usLoopIndex < usMaxNoOfRec; ) {
		ULONG            ulActualRead = 0;       /* actual length of read record function */
		PROGRPT_RECORD   auchRecord;

        /* --- retrieve a formatted record --- */
        ProgRpt_ReadFile( ulReadOffset, &auchRecord, PROGRPT_MAX_REC_LEN, &ulActualRead );
        if ( ulActualRead != PROGRPT_MAX_REC_LEN ) {
            return ( LDT_ERR_ADR );
        }

        /* --- analyze operation code of format record, and
            store it to current row item buffer --- */
		switch ( auchRecord.Loop.uchOperation ) {
        case PROGRPT_OP_PRINT:  /* print operation */
			sRetCode = ProgRpt_FinPRINT( &ReportInfo, &auchRecord.Print, uchMinorClass, NULL );
            break;

        case PROGRPT_OP_MATH:   /* mathematics operation */
			sRetCode = ProgRpt_FinMATH(&auchRecord.MathOpe, NULL );
            break;

        case PROGRPT_OP_LOOP:   /* loop (begin) operation */
			ProgRpt_PrintLoopBegin( &ReportInfo, &auchRecord.Loop );

            switch ( auchRecord.Loop.LoopItem.uchMajor ) {
            case PROGRPT_INDKEY_TERMNO:
                sRetCode = ProgRpt_IndLOOP( &ReportInfo, &auchRecord.Loop, uchMinorClass, usTerminalNo, ulReadOffset );
                usLoopIndex  += auchRecord.Loop.usLoopEnd;
                ulReadOffset += ( PROGRPT_MAX_REC_LEN * auchRecord.Loop.usLoopEnd );
                break;

            default:
                return ( LDT_ERR_ADR );
            }
            break;

        default:
            return ( LDT_ERR_ADR );
        }

        /* --- report is aborted by user ? --- */
        if ( sRetCode != SUCCESS ) {
            return ( sRetCode );
        }
        if ( UieReadAbortKey() == UIE_ENABLE ) {
            return ( RPT_ABORTED );
        }
        if ( RptPauseCheck() == RPT_ABORTED ) {
            return ( RPT_ABORTED );
        }
        if ( uchRptMldAbortStatus ) {
            return ( RPT_ABORTED );
        }

        usLoopIndex++;
        ulReadOffset += PROGRPT_MAX_REC_LEN;
    }

    /* --- print last row item --- */
    ProgRpt_Print( &ReportInfo );

    if ( uchMinorClass == CLASS_TTLSAVDAY ) {
		USHORT      usMaxTermNo;
		TTLREGFIN   TtlFin = {0};         /* structure for total module i/f */

        if ( 0 < usTerminalNo ) {   /* individual reset (Term# = 1...16) */
            usMaxTermNo = usTerminalNo;
        } else {
            usMaxTermNo = RflGetNoTermsInCluster();
            usTerminalNo = 1;
        }

        /* --- declare saved financial total is already reported --- */
        TtlFin.uchMajorClass    = CLASS_TTLINDFIN;
        TtlFin.uchMinorClass    = uchMinorClass;
        for ( ; usTerminalNo <= usMaxTermNo; usTerminalNo++) {
            TtlFin.usTerminalNumber = usTerminalNo;
            sRetCode = SerTtlIssuedReset( &TtlFin, TTL_NOTRESET );
            if (( sRetCode = TtlConvertError( sRetCode )) != SUCCESS ) {
                /* --- print out error code --- */
                RptPrtError( sRetCode );
                break;
            }
        }
    }
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_IndReset( UCHAR  uchResetType,
*                                       USHORT usTerminalNo )
*
*   Input   :   UCHAR  uchResetType  - reset repot type
*               USHORT usTerminalNo  - target terminal no. to reset.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 LDT_KEYOVER_ADR
*                                 LDT_ERR_ADR       ( TTL_FAIL )
*                                 LDT_PROHBT_ADR    ( OP_FILE_NOT_FOUND )
*                                   
**  Description:
*    This function resets current daily individual financial total.
*    An application must call ProgRpt_IndIsRessetable() function before
*   calling this function. If the specified total is resettable, it may
*   call this function.
*    This function deletes saved daily total, and then copies current total
*   to saved total.
*==========================================================================
*/
SHORT ProgRpt_IndReset( UCHAR  uchReportType,
                        USHORT usTerminalNo )   /* 0(ALL), 1...16 */
{
    TTLREPORT   TtlReport;      /* structure for reset total */
    SHORT       sRetCode;       /* return value for user application */
    SHORT       sTermNoMax;     /* max. terminal no. for reset */
    SHORT       sLoopTermNo;    /* terminal no. for loop */
    USHORT      usExecuteFlag;  /* execute flag for EOD report */

    ASSERT(( uchReportType == RPT_EOD_ALLRESET ) ||\
           ( uchReportType == RPT_IND_RESET ) ||\
           ( uchReportType == RPT_ALL_RESET ));

    ASSERT( usTerminalNo <= MAX_TERMINAL_NO );

    if ( 0 < usTerminalNo ) {       /* execute individual reset */
        sTermNoMax  = usTerminalNo;
        sLoopTermNo = usTerminalNo;
    } else {                        /* execute all reset */
        sTermNoMax = RflGetNoTermsInCluster();
        sLoopTermNo = 1;
    }

    /* --- this function ignores following parameter --- */
    if ( uchReportType == RPT_EOD_ALLRESET ) {
        usExecuteFlag = RPT_RESET_INDFIN_BIT;
    } else {
        usExecuteFlag = ~( RPT_RESET_INDFIN_BIT );
    }

    /* --- reset specified individual financial total file --- */
    memset( &TtlReport, 0x00, sizeof( TtlReport ));
    TtlReport.uchMajorClass = CLASS_TTLINDFIN;
    TtlReport.uchMinorClass = CLASS_TTLCURDAY;
    sRetCode = TTL_SUCCESS;

    while (( sLoopTermNo <= sTermNoMax ) && ( sRetCode == TTL_SUCCESS )) {

        TtlReport.ulNumber = sLoopTermNo;

        ASSERT( ProgRpt_IndIsResettable( TtlReport.usNumber ));
        if (( sRetCode = SerTtlTotalReset( &TtlReport, usExecuteFlag )) == TTL_SUCCESS ) {
            sLoopTermNo++;
        }
    }

    /* --- convert return code to lead-thru code --- */
    return ( TtlConvertError( sRetCode ));
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_IndIsResettable( USHORT usTerminalNo )
*
*   Input   :   USHORT usTerminalNo - terminal no. to determine resettable
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - current total is resettable.
*                       FALSE   - current total is not resettable.
*   
**  Description:
*    This function detemines specified current individual financial total is
*   able to reset or not.
*    If saved total file is not issued, current total is not resettable.
*==========================================================================
*/
BOOL ProgRpt_IndIsResettable( USHORT usTerminalNo ) /* 0(All), 1...16 */
{
	TTLREPORT   TtlReport = {0};      /* structure for reset financial total */

    ASSERT( usTerminalNo <= MAX_TERMINAL_NO );

    /* --- determine specified department saved total file is issued ? --- */
    TtlReport.uchMajorClass = CLASS_TTLINDFIN;
    TtlReport.uchMinorClass = CLASS_TTLSAVDAY;
    TtlReport.ulNumber = usTerminalNo;

    if ( SerTtlIssuedCheck( &TtlReport, TTL_NOTRESET ) == TTL_NOT_ISSUED ) {
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_IndIsBadLoopKey( SHORT *psLTDCode,
*                                             UCHAR uchProgRptNo,
*                                             USHORT usTerminalNo )
*
*   Input   :   SHORT *psLDTCode    - address of lead-thru code.
*               UCHAR uchProgRptNo  - programmable report no.
*               USHORT usTerminalNo  - target terminal no.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - specified loop key is bad.
*                       FALSE   - specified loop key is good.
*   
**  Description:
*    This function detemines specified loop key is valid or invalid.
*==========================================================================
*/
BOOL ProgRpt_IndIsBadLoopKey( SHORT *psLDTCode,
                              UCHAR uchProgRptNo,
                              USHORT usTerminalNo )
{
    USHORT usMaxTermNo;

    *psLDTCode = ( LDT_KEYOVER_ADR );

    /* --- determine specified prog. report no. is valid or invalid --- */
    if (( uchProgRptNo < 1 ) || ( MAX_PROGRPT_FORMAT < uchProgRptNo )) {
        return ( TRUE );
    }

    /* --- determine specified plu no. is valid or invalid --- */
    usMaxTermNo = RflGetNoTermsInCluster();
   
    if ( usMaxTermNo < usTerminalNo ) {
        *psLDTCode = LDT_NTINFL_ADR;
        return ( TRUE );
    }

    *psLDTCode = SUCCESS;
    return ( FALSE );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_IndPrintResetTermID( UCHAR  uchProgRptNo,
*                                                  USHORT usTerminalNo )
*
*   Input   :   UCHAR   uchProgRptNo - programmable report no.
*               USHORT  usTerminalNo - target terminal no.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 LDT_ERR_ADR       ( TTL_FAIL )
*                                 LDT_PROHBT_ADR    ( OP_FILE_NOT_FOUND )
*                                   
**  Description:
*    This function prints resetted cashier ID.
*==========================================================================
*/
SHORT ProgRpt_IndPrintResetTermID( UCHAR uchProgRptNo, USHORT usTerminalNo )
{
    PROGRPT_RPTHEAD ReportInfo; /* information about report format */
    SHORT   sRetCode;           /* return value for user application */
    USHORT  usMaxTerminalNo;    /* no. of resident terminals */
    USHORT  usLoopIndex;        /* loop index for retrieve cashier */
    TCHAR   szTermIDForm[] = _T("%6s#%03u");
                                /* print format for terminal ID data */
    TCHAR   aszNull[1] = {'\0'};
    TCHAR   szTerminalID[ PROGRPT_TERMID_WORK_SIZE ];

    ASSERT( ! ProgRpt_IndIsBadLoopKey( &sRetCode, uchProgRptNo, usTerminalNo ));

    /* --- determine specified report format is correct or incorrect --- */
    if (( sRetCode = ProgRpt_GetProgRptInfo( uchProgRptNo, &ReportInfo )) != PROGRPT_SUCCESS ) {
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    if ( ReportInfo.usFileType != PROGRPT_FILE_INDFIN ) {
        return ( LDT_PROHBT_ADR );
    }

    if ( ReportInfo.usDevice == PROGRPT_DEVICE_MLD ) {
        return ( LDT_PROHBT_ADR );
    }

    if ( 0 < usTerminalNo ) {   /* individual reset (Terminal# = 1...16) */
        RflSPrintf( szTerminalID, PROGRPT_TERMID_WORK_SIZE, szTermIDForm, aszNull, usTerminalNo );
        PrtPrint( PMG_PRT_RCT_JNL, szTerminalID, PROGRPT_TERMID_WORK_SIZE );
    } else {                    /* all reset (Terminal# = 0) */
        usMaxTerminalNo = RflGetNoTermsInCluster();

        for (usLoopIndex = 1; usLoopIndex <= usMaxTerminalNo; usLoopIndex++) {
            RflSPrintf( szTerminalID, PROGRPT_TERMID_WORK_SIZE, szTermIDForm, aszNull, usLoopIndex );
            PrtPrint( PMG_PRT_RCT_JNL, szTerminalID, PROGRPT_TERMID_WORK_SIZE );
        }
    }
    return SUCCESS; /* ### Add (2171 for Win32) V1.0 */
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_IndLOOP( PROGRPT_RPTHEAD *pReportInfo,
*                                      PROGRPT_LOOPREC *pLoopRec,
*                                      UCHAR  uchMinorClass,
*                                      USHORT usTerminalNo,
*                                      ULONG  ulBeginOffset )
*
*   Input   :   PROGRPT_RPTHEAD *pReportInfo - address of report header.
*               PROGRPT_LOOPREC *pLoopRec    - address of print record.
*               UCHAR  uchMinorClass    - minor class for cashier total.
*               USHORT usTerminalNo     - target terminal no.
*               ULONG  ulBeginOffset    - offset for loop begin item
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*                                 LDT_KEYOVER_ADR
*                                 LDT_ERR_ADR       ( TTL_FAIL )
*                                 LDT_PROHBT_ADR    ( OP_FILE_NOT_FOUND )
*                                   
**  Description:
*    This function executes loop operation.
*==========================================================================
*/
SHORT ProgRpt_IndLOOP( PROGRPT_RPTHEAD * pReportInfo,
                       PROGRPT_LOOPREC * pLoopRec,
                       UCHAR  uchMinorClass,
                       USHORT usTerminalNo,
                       ULONG  ulBeginOffset )
{
    TTLREGFIN   TtlInd;             /* total data of retrieved ind-financial */
    SHORT       sRetCode;           /* return code of this function */
    UCHAR       uchLoopBeginRow;    /* row of "loop begin" item */
    UCHAR       uchLoopBeginColumn; /* column of "loop begin" item */
    USHORT      usMaxTerminalNo;    /* no. of resident terminals */

    ASSERT( pReportInfo != ( PROGRPT_RPTHEAD * )NULL );
    ASSERT( pLoopRec    != ( PROGRPT_LOOPREC * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) || ( uchMinorClass == CLASS_TTLSAVDAY ));
    ASSERT( usTerminalNo <= MAX_TERMINAL_NO );

    /* --- define maximum loop count & target item no. --- */
    switch ( pLoopRec->LoopItem.uchMajor ) {
    case PROGRPT_INDKEY_TERMNO:      /* read cashier by cashier no. */
        usMaxTerminalNo = RflGetNoTermsInCluster();
        
        /* --- prepare loop counter & flag before record retrieval --- */
        if ( usTerminalNo == PROGRPT_ALLREAD ) {
            usTerminalNo = 1;
        } else {    /* individual read */
            if ( usMaxTerminalNo < usTerminalNo ) {
                return ( LDT_ERR_ADR );
            }
            usMaxTerminalNo = usTerminalNo;
        }
        break;

    default:
        return ( LDT_ERR_ADR );
    }

    uchLoopBeginRow    = uchProgRptRow;
    uchLoopBeginColumn = uchProgRptColumn;
    sRetCode = PROGRPT_SUCCESS;

    while (( usTerminalNo <= usMaxTerminalNo ) && ( sRetCode == PROGRPT_SUCCESS )) {
        /* --- retrieve cashier record. target cashier no. is automatically
            increment in following function --- */
        sRetCode = ProgRpt_IndFindTerminal( usTerminalNo, &TtlInd, uchMinorClass );

        /* --- determine current key record is in accord with
            specified condition --- */
        if ( sRetCode == PROGRPT_SUCCESS ) {
			ULONG       ulReadOffset;       /* offset for read programmed report */
			BOOL        fLoopEnd;           /* boolean value for loop is end */

            /* --- restore print position to loop begin --- */
            fProgRptEmptyLoop = TRUE;
            uchProgRptRow    = uchLoopBeginRow; 
            uchProgRptColumn = uchLoopBeginColumn;

            /* --- read a record until end of loop --- */
            ulReadOffset = ulBeginOffset + ( ULONG )PROGRPT_MAX_REC_LEN;

            fLoopEnd = FALSE;
            while ( ! fLoopEnd ) {
				ULONG            ulActualRead;       /* actual length of read record function */
				PROGRPT_RECORD   auchRecord;

				//11-11-03 JHHJ SR 201 Parameter 4 change from USHORT to ULONG
                ProgRpt_ReadFile( ulReadOffset, &auchRecord, PROGRPT_MAX_REC_LEN, &ulActualRead );
                if ( ulActualRead != PROGRPT_MAX_REC_LEN ) {
                    return ( LDT_ERR_ADR );
                }

                /* --- analyze operation code of format record, and
                    store it to current row item buffer --- */
				switch ( auchRecord.Loop.uchOperation ) {

                case PROGRPT_OP_PRINT:
					ProgRpt_FinPRINT( pReportInfo, &auchRecord.Print, uchMinorClass, &TtlInd );
                    break;

                case PROGRPT_OP_MATH:
					ProgRpt_FinMATH(&auchRecord.MathOpe, &TtlInd );
                    break;

                case PROGRPT_OP_LOOP:
					if (( auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_END_ROOT ) ||
                        ( auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_END_NEST )) {

                        /* --- print current row item,
                            if next row item is retrieved --- */
                        ProgRpt_PrintAndFeed( pReportInfo, &auchRecord.Loop );
                        fLoopEnd = TRUE;
                    } else {
                        return ( LDT_ERR_ADR );
                    }
                    break;

                default:
                    return ( LDT_ERR_ADR );
                }
            
                /* --- report is aborted by user ? --- */
                if ( UieReadAbortKey() == UIE_ENABLE ) {
                    return ( RPT_ABORTED );
                }
                if ( RptPauseCheck() == RPT_ABORTED ) {
                    return ( RPT_ABORTED );
                }
                if ( uchRptMldAbortStatus ) {
                    return ( RPT_ABORTED );
                }

                ulReadOffset += ulActualRead;

            }   /* loop until "loop end" is found */

        }   /* --- execute report if dept item is TRUE condition --- */

        usTerminalNo++;

    }   /* loop until all requested department record is read */

    fProgRptEmptyLoop = FALSE;
    return ( ProgRpt_ConvertError( sRetCode ));
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_IndFindTerminal( USHORT usTerminalNo,
*                                              TTLREGFIN *pTtlInd,
*                                              UCHAR     uchMinorClass )
*
*   Input   :   USHORT usTerminalNo - terminal no. to retrieve total.
*               UCHAR uchMinorClass - minor class for ind-financial total.
*   Output  :   TTLREGFIN *pTtlInd  - address of ind-financial total.
*   InOut   :   Nothing
*
**  Return  :   SHORT   sRetCode    - PROGRPT_SUCCESS
*                                     PROGRPT_END_OF_FILE
*                                   
**  Description:
*    This function retrieves an individual financial total which specified
*   at pTtlInd parameter.
*==========================================================================
*/
SHORT ProgRpt_IndFindTerminal( USHORT usTerminalNo,
                               TTLREGFIN *pTtlInd,
                               UCHAR      uchMinorClass )
{
    SHORT   sRetCode;       /* return code for this function */
    SHORT   sTtlStatus;     /* function status for total module */

    ASSERT( pTtlInd != ( TTLREGFIN * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ));
/*#ifdef DEBUG
{
    USHORT  usMaxTermNo;
    usMaxTermNo = ( PifSysConfig() )->ausOption[ 3 ] + 1;
    ASSERT(( 0 < usTerminalNo ) && ( usTerminalNo <= usMaxTermNo ));
}
#endif*/

    memset( pTtlInd, 0x00, sizeof( TTLREGFIN ));
    pTtlInd->uchMajorClass = CLASS_TTLINDFIN;
    pTtlInd->uchMinorClass = uchMinorClass;
    pTtlInd->usTerminalNumber  = usTerminalNo;

    sTtlStatus = SerTtlTotalRead( pTtlInd );
    switch ( sTtlStatus ) {
    case TTL_SUCCESS:
    case TTL_NOTINFILE:
        sRetCode = PROGRPT_SUCCESS;
        break;
    default:
        sRetCode = PROGRPT_FILE_NOT_FOUND;
        break;
    }
    return ( sRetCode );
}

/* ===== End of File (_PROGIND.C) ===== */
