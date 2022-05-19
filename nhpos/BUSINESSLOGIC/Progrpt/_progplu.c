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
*   Program Name       : _PROGPLU.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : The provided function names are as follows.
*
*   ProgRpt_PluPrint()          - print current/saved daily/PTD plu total.
*   ProgRpt_PluReset()          - reset current daily/PTD to saved total file.
*   ProgRpt_PluIsResettable()   - determine current daily/PTD is resettable.
*   ProgRpt_PluIsBadLoopKey()   - determine loop key is valid or invalid.
*
*   ProgRpt_PluLOOP()           - execute loop operation in plu ttl.
*   ProgRpt_PluPRINT()          - execute print operation in plu ttl.
*   ProgRpt_PluMATH()           - execute mathematics operation in plu ttl.
*
*   ProgRpt_PluFindPlu()        - find plu para by plu/dept/report code.
*   ProgRpt_PluIsLoopTRUE()     - determine loop condition is TRUE.
*   ProgRpt_PluIsPrintTRUE()    - determine print condition is TRUE.
*   ProgRpt_PluQueryDataType()  - get type of specified data.
*   ProgRpt_PluGetInteger()     - get integer from plu total/parameter.
*   ProgRpt_PluGetDateTime()    - get date/time data from plu total.
*   ProgRpt_PluGetString()      - get string data from plu total.
*   ProgRpt_PluGetPrice()       - get price data from plu parameter.
*   ProgRpt_PluGetAdjNo()       - get adjective no. from plu parameter.
*   ProgRpt_PluPrintRootPara()  - print root(out of LOOP) parameter.
*
*  ------------------------------------------------------------------------
*    Update Histories  :
* Date      : Ver.Rev  : NAME       : Description
* Mar-01-96 : 03.01.00 : T.Nakahata : Initial
* Apr-03-96 : 03.01.02 : T.Nakahata : Check TTL_NOTINFILE in IsResettable
*                                   Skip print nested LOOP if no linked item
* 2171 for Win32 Initial
* Sep-17-99 : 01.00.00 : K.Iwata    : Changed type definition of 'D13DIGITS'
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
#include    <ecr.h>
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
#include    <maint.h>
#include    <progrept.h>
#include    <csprgrpt.h>
#include    "_progrpt.h"

extern  MAINTPLU    MaintPlu;

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
**  Synopsis:   SHORT ProgRpt_PluPrint( UCHAR  uchProgRptNo,
*                                       UCHAR  uchMinorClass,
*                                       UCHAR  *puchPluNo,
*                                       USHORT usDeptNo,
*                                       UCHAR  uchReportCode )
*
*   Input   :   UCHAR  uchProgRptNo  - programmable report no.
*               UCHAR  uchMinorClass - minor class for department total.
*               UCHAR  auchPluNo     - target plu no.
*               USHORT usDeptNo      - target department no.
*               UCHAR  uchReportCode - target report code.
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
*    This function prints specified plu report.
*==========================================================================
*/
SHORT ProgRpt_PluPrint( UCHAR  uchProgRptNo,
                        UCHAR  uchMinorClass,
                        TCHAR  *puchPluNo,
                        USHORT usDeptNo,
                        UCHAR  uchReportCode )
{
    SHORT   sRetCode;           /* return code */
    PROGRPT_FHEAD   FileInfo;   /* information about prog report file */
    PROGRPT_RPTHEAD ReportInfo; /* information about report format */
    USHORT  usMaxNoOfRec;       /* maximum no. of record in report format */
    USHORT  usLoopIndex;        /* loop index for format record */
    ULONG   ulReadOffset;       /* read offset for format record */
    TTLPLU  TtlPlu;             /* structure for total module i/f */
    SHORT   sLoopType;          /* type of root loop (plu#, dept#, report) */
    BOOL    f1stAllRead;        /* indicate root loop is all read */
    USHORT  usLoopDeptNo;       /* dept no. for root loop */
    UCHAR   uchLoopReportNo;    /* report code for root loop */
    TCHAR   auchLoopPluNo[STD_PLU_NUMBER_LEN];   /* plu no. for nested loop, Saratoga */
    UCHAR   uchLoopBeginRow;    /* row position before execute root loop */
    UCHAR   uchLoopBeginColumn; /* column position before execute root loop */
    UCHAR   uchLoopEndRow;      /* row position after root loop */
    USHORT  usLoopBeginRecNo;   /* record no. before execute root loop */
    ULONG   ulLoopBeginOffset;  /* file read position before exec root loop */
    BOOL    fContinueLoop;
    DEPTIF          DeptIf;
    SHORT   sReturn;

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));
    ASSERT( ! ProgRpt_PluIsBadLoopKey( &sRetCode, uchProgRptNo, puchPluNo, usDeptNo, uchReportCode ));

    memset(&DeptIf, 0, sizeof(DEPTIF));

    /* --- determine specified report format is correct or incorrect --- */
    if (( sRetCode = ProgRpt_GetProgRptInfo( uchProgRptNo, &ReportInfo )) != PROGRPT_SUCCESS ) {
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    if ( ReportInfo.usFileType != PROGRPT_FILE_PLU ) {
        return ( LDT_PROHBT_ADR );
    }

    if ( ReportInfo.usDevice == PROGRPT_DEVICE_MLD ) {
        if (( uchMinorClass == CLASS_TTLSAVPTD ) ||
            ( uchMinorClass == CLASS_TTLSAVDAY )) {
            return ( LDT_PROHBT_ADR );
        }
    }

    /* --- initialize current row item before print plu report --- */
    memset( auchProgRptBuff, 0x00, sizeof( auchProgRptBuff ));
    uchProgRptColumn = 1;
    uchProgRptRow    = 1;

    /* --- read a record until end of file --- */

    sLoopType = PROGRPT_LOOP_NONE;
    usLoopDeptNo    = usDeptNo;
    uchLoopReportNo = uchReportCode;
    if ( puchPluNo == ( TCHAR * )NULL ) {  /* saratoga */
        memset(auchLoopPluNo, 0, sizeof(auchLoopPluNo));
    } else {
        _tcsncpy(auchLoopPluNo, puchPluNo, STD_PLU_NUMBER_LEN);
    }

    ProgRpt_GetFileHeader( &FileInfo );

    ulReadOffset = FileInfo.aulOffsetReport[ uchProgRptNo - 1 ] + sizeof( PROGRPT_RPTHEAD );

    usLoopIndex  = 0;
    usMaxNoOfRec = ReportInfo.usNoOfLoop  + ReportInfo.usNoOfPrint + ReportInfo.usNoOfMath;

    while ( usLoopIndex < usMaxNoOfRec ) {
		PROGRPT_RECORD   auchRecord;  /* buffer for format record */
		ULONG   ulActualRead;       /* actual length of read record function */

        /* --- retrieve a formatted record --- */
        ProgRpt_ReadFile( ulReadOffset, &auchRecord, PROGRPT_MAX_REC_LEN, &ulActualRead );
        if ( ulActualRead != PROGRPT_MAX_REC_LEN ) {
            return ( LDT_ERR_ADR );
        }

        /* --- analyze operation code of format record, and
            store it to current row item buffer --- */
        switch ( auchRecord.Loop.uchOperation ) {

        case PROGRPT_OP_PRINT:  /* print operation */
            sRetCode = ProgRpt_PluPRINT( &ReportInfo, &auchRecord.Print, uchMinorClass, NULL, NULL, usLoopDeptNo, uchLoopReportNo );
            break;

        case PROGRPT_OP_MATH:   /* mathematics operation */
            sRetCode = ProgRpt_PluMATH( &auchRecord.MathOpe, NULL, NULL );
            break;

        case PROGRPT_OP_LOOP:   /* loop operation */

            ProgRpt_PrintLoopBegin( &ReportInfo, &auchRecord.Loop);

            switch (auchRecord.Loop.LoopItem.uchMajor ) {

            case PROGRPT_PLUKEY_PLUNO:
                if ( sLoopType == PROGRPT_LOOP_NONE ) {
                    sLoopType = PROGRPT_LOOP_PLU;
                }
                sRetCode = ProgRpt_PluLOOP( &ReportInfo, &auchRecord.Loop, uchMinorClass, auchLoopPluNo, usLoopDeptNo, uchLoopReportNo, sLoopType, ulReadOffset );
                usLoopIndex  += auchRecord.Loop.usLoopEnd;
                ulReadOffset +=  PROGRPT_MAX_REC_LEN * auchRecord.Loop.usLoopEnd;

                if ( sLoopType == PROGRPT_LOOP_PLU ) {
                    sLoopType = PROGRPT_LOOP_NONE;
                }
                break;

            case PROGRPT_PLUKEY_DEPTNO:
            case PROGRPT_PLUKEY_REPORT:

                if ((auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_BEGIN_ROOT ) &&
                    ( sLoopType == PROGRPT_LOOP_NONE )) {

                    uchLoopBeginRow    = uchProgRptRow;
                    uchLoopBeginColumn = uchProgRptColumn;
                    usLoopBeginRecNo   = usLoopIndex;
/*                    usLoopPluNo        = PROGRPT_ALLREAD; Saratoga */
                    memset(auchLoopPluNo, 0, sizeof(auchLoopPluNo));
                    ulLoopBeginOffset  = ulReadOffset;

                    if (auchRecord.Loop.LoopItem.uchMajor == PROGRPT_PLUKEY_DEPTNO ) {

                        /* --- loop begin (dept #) --- */

                        sLoopType = PROGRPT_LOOP_DEPT;
                        if ( usDeptNo == PROGRPT_ALLREAD ) {
                            f1stAllRead   = TRUE;
                            /* saratoga */
                            sReturn = SerOpDeptAllRead(&DeptIf, 0);
                            if (sReturn == OP_PERFORM) {
                                usLoopDeptNo = DeptIf.usDeptNo;
                            }
                        } else {
                            f1stAllRead  = FALSE;
                            usLoopDeptNo = usDeptNo;
                        }

                    } else {    /* report code */

                        /* --- loop begin (report code) --- */
                        sLoopType = PROGRPT_LOOP_REPORT;
                        if ( uchReportCode == PROGRPT_ALLREAD ) {
                            f1stAllRead     = TRUE;
                            uchLoopReportNo = 1;
                        } else {
                            f1stAllRead     = FALSE;
                            uchLoopReportNo = uchReportCode;
                        }
                    }

                } else if (auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_END_ROOT ) {

                    /* --- print current row item,
                        if next row item is retrieved --- */

                    ProgRpt_PrintAndFeed( &ReportInfo, &auchRecord);
                    uchLoopEndRow    = auchRecord.Loop.uchRow;

                    if (auchRecord.Loop.LoopItem.uchMajor == PROGRPT_PLUKEY_DEPTNO ) {
                        /* --- loop end (dept #) , saratoga --- */
                        if (f1stAllRead == TRUE) {
                            sReturn = SerOpDeptAllRead(&DeptIf, 0);
                            if (sReturn == OP_PERFORM) {
                                usLoopDeptNo = DeptIf.usDeptNo;
                                fContinueLoop = TRUE;
                            } else {
                                fContinueLoop = FALSE;
                            }
                        } else {
                            fContinueLoop = FALSE;
                        }
                    } else {    /* report code */
                        /* --- loop end (report code #) --- */
                        uchLoopReportNo++;
                        if (( uchLoopReportNo <= PLU_REPORT_MAX ) &&
                            f1stAllRead ) {
                            fContinueLoop = TRUE;
                        } else {
                            fContinueLoop = FALSE;
                        }
                    }

                    if ( fContinueLoop ) {
                        /* --- continue loop next dept/report no. --- */
                        uchProgRptRow    = uchLoopBeginRow;
                        uchProgRptColumn = uchLoopBeginColumn;
                        usLoopIndex      = usLoopBeginRecNo;
                        ulReadOffset     = ulLoopBeginOffset;
                    } else {
                        /* --- loop is now finished --- */
                        sLoopType = PROGRPT_LOOP_NONE;
                        usLoopDeptNo    = usDeptNo;
                        uchLoopReportNo = uchReportCode;
                        if ( puchPluNo == ( TCHAR * )NULL ) {  /* saratoga */
                            memset(auchLoopPluNo, 0, sizeof(auchLoopPluNo));
                        } else {
                            _tcsncpy(auchLoopPluNo, puchPluNo, STD_PLU_NUMBER_LEN);
                        }
                        uchProgRptRow = uchLoopEndRow;
                    }
                } else {
                    return ( LDT_ERR_ADR );
                }
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

    if (( uchMinorClass == CLASS_TTLSAVDAY ) ||
        ( uchMinorClass == CLASS_TTLSAVPTD )) {

        /* --- declare saved dept total is already reported --- */

        TtlPlu.uchMajorClass = CLASS_TTLPLU;
        TtlPlu.uchMinorClass = uchMinorClass;
        sRetCode = SerTtlIssuedReset( &TtlPlu, TTL_NOTRESET );

        if (( sRetCode = TtlConvertError( sRetCode )) != SUCCESS ) {
            /* --- print out error code --- */
            RptPrtError( sRetCode );
        }
    }
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_PluReset( UCHAR uchMinorClass )
*
*   Input   :   UCHAR uchMinorClass - minor class for department total.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 LDT_KEYOVER_ADR
*                                 LDT_ERR_ADR       ( TTL_FAIL )
*                                 LDT_PROHBT_ADR    ( OP_FILE_NOT_FOUND )
*                                   
**  Description:
*    This function prints specified department report.
*==========================================================================
*/
SHORT ProgRpt_PluReset( UCHAR uchMinorClass )
{
	TTLREPORT   TtlReport = { 0 };      /* structure for reset plu total */
    SHORT       sRetCode;       /* return value for user application */
    USHORT      usExecuteFlag;  /* execute flag for EOD/PTD report */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ));

    ASSERT(( RptEODChkFileExist( FLEX_DEPT_ADR ) == RPT_FILE_EXIST ) &&
           ( RptEODChkFileExist( FLEX_PLU_ADR  ) == RPT_FILE_EXIST ));

    TtlReport.uchMajorClass = CLASS_TTLPLU;
    TtlReport.uchMinorClass = uchMinorClass;

    /* --- this function ignores following parameter --- */
    usExecuteFlag = 0;

    /* --- reset specified plu total file --- */
    ASSERT( ProgRpt_PluIsResettable( uchMinorClass ));
    sRetCode = SerTtlTotalReset( &TtlReport, usExecuteFlag );

    /* --- convert return code to lead-thru code --- */
    return ( TtlConvertError( sRetCode ));
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_PluIsResettable( UCHAR uchMinorClass )
*
*   Input   :   UCHAR uchMinorClass - minor class for plu total.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - current total is resettable.
*                       FALSE   - current total is not resettable.
*   
**  Description:
*    This function detemines specified current plu total is able to reset
*   or not.
*    If saved total file is not issued, current total is not resettable.
*==========================================================================
*/
BOOL ProgRpt_PluIsResettable( UCHAR uchMinorClass )
{
	TTLREPORT   TtlReport = { 0 };      /* structure for reset plu total */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ));

    if (( RptEODChkFileExist( FLEX_DEPT_ADR ) != RPT_FILE_EXIST ) ||
        ( RptEODChkFileExist( FLEX_PLU_ADR  ) != RPT_FILE_EXIST )) {
        return ( FALSE );
    }

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

    /* --- determine specified department saved total file is issued ? --- */
    TtlReport.uchMajorClass = CLASS_TTLPLU;
    TtlReport.ulNumber = 0;

    if ( SerTtlIssuedCheck( &TtlReport, TTL_NOTRESET ) == TTL_NOT_ISSUED ) {
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_PluIsBadLoopKey( SHORT  *psLTDCode,
*                                             UCHAR  uchProgRptNo,
*                                             UCHAR  *puchPluNo,
*                                             USHORT usDeptNo,
*                                             UCHAR  uchReportCode )
*
*   Input   :   SHORT  *psLDTCode    - address of lead-thru code.
*               UCHAR  uchProgRptNo  - programmable report no.
*               UCHAR  *puchPluNo    - target plu no.
*               USHORT usDeptNo      - target department no.
*               UCHAR  uchReportCode - target report code.
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
BOOL ProgRpt_PluIsBadLoopKey( SHORT  *psLDTCode,
                              UCHAR  uchProgRptNo,
                              TCHAR  *puchPluNo,
                              USHORT usDeptNo,
                              UCHAR uchReportCode )
{
    PARAFLEXMEM FlexMem;    /* structure for flexible memory */
    PLUIF       PluIf;      /* structure for PLU record retrieval */
    USHORT      usLockHnd;  /* lock handle of Cli*** function */
    PARAPLU     ParaPlu;
    SHORT       sRetCode;
    DEPTIF        DeptIf;    

    ASSERT( psLDTCode != ( SHORT * )NULL );

    *psLDTCode = ( LDT_KEYOVER_ADR );

    /* --- determine specified prog. report no. is valid or invalid --- */
    if (( uchProgRptNo < 1 ) || ( MAX_PROGRPT_FORMAT < uchProgRptNo )) {
        return ( TRUE );
    }

    /* --- determine specified plu no. is valid or invalid, Saratoga --- *

    if ( MAX_PLU_NO < usPluNo ) {
        return ( TRUE );
    }   */

    /* --- determine specified report code is valid or invalid --- */
    if ( PLU_REPORT_MAX < uchReportCode ) {
        return ( TRUE );
    }

    /* --- determine specified department no. is valid or invalid --- */
    /* saratoga */
    FlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
    FlexMem.uchAddress    = FLEX_DEPT_ADR;
    CliParaRead( &FlexMem );
    if ( FlexMem.ulRecordNumber == 0 ) {
        return ( FALSE );
    }
    
    memset(&DeptIf, 0, sizeof(DeptIf));
    if (usDeptNo) {
        /* ind. dept read */
        DeptIf.usDeptNo = usDeptNo;
        if (CliOpDeptIndRead(&DeptIf, 0) != OP_PERFORM) {
            return ( FALSE );
        }
    } else {
        /* all read */
        if (SerOpDeptAllRead(&DeptIf, 0) != OP_PERFORM) {
            return ( FALSE );
        }
    }
    
    /* --- determine specified PLU record exists or not,    Saratoga --- */
    if ( !ProgRpt_PluIsAll( puchPluNo )) {
        _tcsncpy(ParaPlu.auchPLUNumber, puchPluNo, PLU_MAX_DIGIT+1);
        sRetCode = ProgRpt_PluLabelAnalysis(&ParaPlu);
        if ( sRetCode != SUCCESS ) {
            return( TRUE ); /* ..BAD */
        }
        
        _tcsncpy(PluIf.auchPluNo, ParaPlu.auchPLUNumber, OP_PLU_LEN);
        PluIf.uchPluAdj = 1;
        usLockHnd = 0;
        if ( CliOpPluIndRead( &PluIf, usLockHnd ) != OP_PERFORM ) {
            *psLDTCode = LDT_NTINFL_ADR;
            return ( TRUE );
        }
        
        _tcsncpy(puchPluNo, ParaPlu.auchPLUNumber, PLU_MAX_DIGIT);
        puchPluNo[PLU_MAX_DIGIT] = '\0';
    }

    *psLDTCode = SUCCESS;
    return ( FALSE );
}


/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_PluLOOP( PROGRPT_RPTHEAD * pReportInfo,
*                                      PROGRPT_LOOPREC * pLoopRec,
*                                      UCHAR    uchMinorClass,
*                                      UCHAR    *puchPluNo,
*                                      USHORT   usDeptNo,
*                                      UCHAR    uchReportNo,
*                                      SHORT    sLoopType,
*                                      ULONG    ulBeginOffset )
*
*   Input   :   PROGRPT_RPTHEAD * pReportInfo - address of report header.
*               PROGRPT_LOOPREC * pLoopRec    - address of print record.
*               UCHAR   uchMinorClass   - minor class for plu total.
*               UCHAR   *puchPluNo      - target plu no.
*               USHORT  usDeptNo        - target department no.
*               UCHAR   uchReportNo     - target report code.
*               SHORT   sLoopType       - type of loop (plu/dept#/report#)
*               ULONG   ulBeginOffset   - offset for "loop begin" item
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*                                 LDT_ERR_ADR       ( TTL_FAIL )
*                                   
**  Description:
*    This function executes loop operation.
*==========================================================================
*/
SHORT ProgRpt_PluLOOP( PROGRPT_RPTHEAD * pReportInfo,
                       PROGRPT_LOOPREC * pLoopRec,
                       UCHAR   uchMinorClass,
                       TCHAR   *puchPluNo,
                       USHORT  usDeptNo,
                       UCHAR   uchReportCode,
                       SHORT   sLoopType,
                       ULONG   ulBeginOffset )
{
    PLUIF       PluIf;          /* parameter data of retrieved plu */
    TTLPLU      TtlPlu;         /* total data of retrieved plu */
    USHORT      usFindKeyNo;    /* key no. to retrieve plu record */
    ULONG       ulReadOffset;   /* offset for read programmed format */
    BOOL        fLoopEnd;       /* boolean value for loop is end */
    SHORT       sRetCode;       /* return code of this function */
    UCHAR       uchLoopBeginRow;    /* row of "loop begin" item */
    UCHAR       uchLoopBeginColumn; /* column of "loop begin" item */
    BOOL        fAdjPlu;
    BOOL        fLinkedItemFound;
    USHORT      usKey;    	/* for loop condition, 04/03/01 */

    ASSERT( pReportInfo != ( PROGRPT_RPTHEAD * )NULL );
    ASSERT( pLoopRec != ( PROGRPT_LOOPREC * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));
/*    ASSERT( usPluNo <= MAX_PLU_NO );  Saratoga */
    ASSERT( uchReportCode <= PLU_REPORT_MAX );
    ASSERT(( sLoopType == PROGRPT_LOOP_PLU )  ||\
           ( sLoopType == PROGRPT_LOOP_DEPT ) ||\
           ( sLoopType == PROGRPT_LOOP_REPORT ));

    /* --- prepare loop counter & flag before record retrieval --- */
    switch ( sLoopType ) {
    case PROGRPT_LOOP_DEPT:     /* read plu by dept no. */
        usFindKeyNo = usDeptNo;
        usKey = PROGRPT_ALLREAD; /* correct loop condition, 04/03/01 */
        break;

    case PROGRPT_LOOP_REPORT:   /* read plu by report code */
        usFindKeyNo = ( USHORT )uchReportCode;
        usKey = PROGRPT_ALLREAD; /* correct loop condition, 04/03/01 */
        break;

    case PROGRPT_LOOP_PLU:
    default:                    /* read plu by plu no. */
/*        usFindKeyNo = usPluNo;    Saratoga */
        if ( ProgRpt_PluIsAll( puchPluNo )) {
            usFindKeyNo = PROGRPT_ALLREAD;
        } else {
        	usKey = !PROGRPT_ALLREAD; /* correct loop condition, 04/03/01 */
        }
        break;
    }
    PluIf.ulCounter  = 0UL;
    PluIf.usAdjCo    = 0; /* save adjective co., 04/03/01 */

    uchLoopBeginRow    = uchProgRptRow;
    uchLoopBeginColumn = uchProgRptColumn;
    fProgRptEmptyLoop = FALSE;
    fLinkedItemFound = FALSE;

    do {    /* loop until all requested plu record is read */ 
        /* --- retrieve plu record. target plu no. is automatically increment in following function --- */
        PluIf.uchPluAdj  = 1;
        sRetCode = ProgRpt_PluFindPlu( &PluIf, &TtlPlu, uchMinorClass, sLoopType, usFindKeyNo, puchPluNo );

        /* --- determine current key record is in accord with
            specified condition --- */
        if (( sRetCode == PROGRPT_SUCCESS ) && ( ProgRpt_PluIsLoopTRUE( pLoopRec, &PluIf ))) {

            fAdjPlu = (( PluIf.ParaPlu.ContPlu.usDept & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP );

            /* --- restore print position to loop begin --- */
            fLinkedItemFound = TRUE;
            fProgRptEmptyLoop = TRUE;
            uchProgRptRow    = uchLoopBeginRow; 
            uchProgRptColumn = uchLoopBeginColumn;

            /* --- read a record until end of loop --- */
            fLoopEnd = FALSE;
            ulReadOffset = ulBeginOffset + ( ULONG )PROGRPT_MAX_REC_LEN;

            while ( ! fLoopEnd ) {
				ULONG       ulActualRead;   /* actual length of read record function */
				PROGRPT_RECORD   auchRecord;  /* buffer for format record */

				//11-11-03 JHHJ SR 201 Parameter 4 change from USHORT to ULONG
                ProgRpt_ReadFile( ulReadOffset, &auchRecord, PROGRPT_MAX_REC_LEN, &ulActualRead );
                if ( ulActualRead != PROGRPT_MAX_REC_LEN ) {
                    return ( LDT_ERR_ADR );
                }

                /* --- analyze operation code of format record, and
                    store it to current row item buffer --- */
                switch ( auchRecord.Loop.uchOperation ) {
                case PROGRPT_OP_PRINT:
                    ProgRpt_PluPRINT( pReportInfo, &auchRecord.Print, uchMinorClass, &PluIf, &TtlPlu, usDeptNo, uchReportCode );
                    break;

                case PROGRPT_OP_MATH:
                    ProgRpt_PluMATH( &auchRecord.MathOpe, &PluIf, &TtlPlu );
                    break;

                case PROGRPT_OP_LOOP:
                    if ((auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_END_ROOT ) ||
                        (auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_END_NEST )) {
                        if (auchRecord.Loop.LoopItem.uchMajor == PROGRPT_PLUKEY_PLUNO ) {
                            /* --- print current row item, if next row item is retrieved --- */
                            ProgRpt_PrintAndFeed( pReportInfo, &auchRecord );
                            fLoopEnd = TRUE;
                        } else {
                            return ( LDT_ERR_ADR );
                        }
                    } else {
                        return ( LDT_ERR_ADR );
                    }
                    break;

                default:
                    return ( LDT_ERR_ADR );
                }
            
                /* --- report is aborted by user ? --- */
                if ( UieReadAbortKey() == UIE_ENABLE ) {
                    SerOpPluAbort(PluIf.husHandle,0);
                    return ( RPT_ABORTED );
                }
                if ( RptPauseCheck() == RPT_ABORTED ) {
                    SerOpPluAbort(PluIf.husHandle,0);
                    return ( RPT_ABORTED );
                }
                if ( uchRptMldAbortStatus ) {
                    SerOpPluAbort(PluIf.husHandle,0);
                    return ( RPT_ABORTED );
                }

                ulReadOffset += ulActualRead;
            }   /* loop until "loop end" is found */
        }   /* --- execute report if plu item is TRUE condition,    Saratoga --- */
    } while (( usKey == PROGRPT_ALLREAD ) && ( sRetCode == PROGRPT_SUCCESS ));	/* correct loop condition, 04/03/01 */

    if ( ! fLinkedItemFound ) {
		ULONG       ulActualRead;   /* actual length of read record function */
		PROGRPT_RECORD   auchRecord;  /* buffer for format record */

		/* --- there is no liked items in DEPT/REPTCD LOOP, skip print PLU LOOP --- */
        ulReadOffset = ulBeginOffset;

		//11-11-03 JHHJ SR 201 Parameter 4 change from USHORT to ULONG
        ProgRpt_ReadFile( ulReadOffset, &auchRecord, PROGRPT_MAX_REC_LEN, &ulActualRead );
        if ( ulActualRead != PROGRPT_MAX_REC_LEN ) {
            return ( LDT_ERR_ADR );
        }

        /* --- store LOOP-END item's row to global data --- */
        ulReadOffset += ( ULONG )( PROGRPT_MAX_REC_LEN * (auchRecord.Loop.usLoopEnd - 1 ));

		//11-11-03 JHHJ SR 201 Parameter 4 change from USHORT to ULONG
        ProgRpt_ReadFile( ulReadOffset, &auchRecord, PROGRPT_MAX_REC_LEN, &ulActualRead );
        if ( ulActualRead != PROGRPT_MAX_REC_LEN ) {
            return ( LDT_ERR_ADR );
        }
        uchProgRptRow = auchRecord.Loop.uchRow;
    }

    /* --- exit this function, if plu record retrieval is failed --- */
    fProgRptEmptyLoop = FALSE;
    return ( ProgRpt_ConvertError( sRetCode ));
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_PluPRINT( PROGRPT_RPTHEAD  *pReportInfo,
*                                       PROGRPT_PRINTREC *pPrintRec,
*                                       UCHAR   uchMinorClass,
*                                       PLUIF   *pPluIf,
*                                       TTLPLU  *pTtlPlu,
*                                       USHORT  usLoopDeptNo,
*                                       UCHAR   uchLoopReportNo );
*
*   Input   :   PROGRPT_RPTHEAD *pReportInfo - address of report header.
*               PROGRPT_PRINTREC *pPrintRec - address of print record.
*               UCHAR   uchMinorClass - minor class for plu total.
*               PLUIF   *pPluIf     - operational parameter of plu
*               TTLPLU  *pTtlPlu    - total/co. data of plu record.
*               UCHAR uchLoopDeptNo - dept no. of loop key
*               UCHAR uchLoopReportNo - report code of loop key
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
*    This function executes print operation.
*==========================================================================
*/
SHORT ProgRpt_PluPRINT( PROGRPT_RPTHEAD  *pReportInfo,
                        PROGRPT_PRINTREC *pPrintRec,
                        UCHAR   uchMinorClass,
                        PLUIF   *pPluIf,
                        TTLPLU  *pTtlPlu,
                        USHORT  usLoopDeptNo,
                        UCHAR   uchLoopReportNo )
{
    D13DIGITS   D13Digit;       /* value for target print item */
    D13DIGITS   hugeint;        /* 64 bit value for accumulator */
    N_DATE      Date;           /* date for target print item */
    UCHAR       uchDataType;    /* target print item data type */
    UCHAR       uchStrLen;      /* length of mnemonic data without NULL */
    TCHAR       szMnemonic[ PROGRPT_MAX_40CHAR_LEN + 1 ];
                                /* mnemonic work area */

    ASSERT( pReportInfo != ( PROGRPT_RPTHEAD * )NULL );
    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );

    /* --- determine specified condition is TRUE --- */
    if ( ! ProgRpt_PluIsPrintTRUE( pPrintRec, pPluIf, pTtlPlu )) {
        ProgRpt_PrintEOF( pReportInfo, pPrintRec );
        uchProgRptRow    = pPrintRec->uchRow;
        uchProgRptColumn = pPrintRec->uchColumn;
        return ( SUCCESS );
    }

    if ( auchProgRptBuff[ 0 ] != '\0' ) {
        fProgRptEmptyLoop = FALSE;
    }

    /* --- print current row item, if next row item is retrieved --- */
    ProgRpt_PrintAndFeed( pReportInfo, pPrintRec );

    usLoopDeptNo    = ( usLoopDeptNo   == 0 ) ? 1 : usLoopDeptNo;
    uchLoopReportNo = ( UCHAR )(( uchLoopReportNo == 0 ) ? 1 : uchLoopReportNo );

    if (( pTtlPlu == ( TTLPLU * )NULL ) && ( pPluIf == ( PLUIF * )NULL )) {
        /* --- print a record which is out of LOOP operation --- */
        if ( ProgRpt_PluPrintRootPara( pPrintRec, usLoopDeptNo, uchLoopReportNo )) {
            return ( SUCCESS );
        }
    }

    /* --- execute print operation with specified format & style --- */
    memset( szMnemonic, 0x00, sizeof( szMnemonic ));

    uchDataType = ProgRpt_PluQueryDataType( &( pPrintRec->PrintItem ));

    switch ( uchDataType ) {

    case PROGRPT_TYPE_UCHAR:
    case PROGRPT_TYPE_USHORT:
    case PROGRPT_TYPE_LTOTAL:
        if ( ! ProgRpt_PluGetInteger( &( pPrintRec->PrintItem ), pPluIf, pTtlPlu, &hugeint ) ) {
            return ( LDT_ERR_ADR );
        }
        D13Digit = hugeint;

        /* --- convert long value to string with specified form --- */
        ProgRpt_IntegerToString( pPrintRec, &D13Digit, szMnemonic );

        /* --- format and set to print buffer --- */
        uchStrLen = ( UCHAR )_tcslen( szMnemonic );
        ProgRpt_FormatPrintBuff( pPrintRec, szMnemonic, uchStrLen, uchDataType );
        break;

    case PROGRPT_TYPE_ADJNO:
        if ( ! ProgRpt_PluGetAdjNo( pPrintRec, pPluIf, szMnemonic )) {
            return ( LDT_ERR_ADR );
        }

        /* --- format and set to print buffer --- */
        uchStrLen = ( UCHAR )_tcslen( szMnemonic );
        ProgRpt_FormatPrintBuff( pPrintRec, szMnemonic, uchStrLen, uchDataType );
        break;

    case PROGRPT_TYPE_PRICE:
        switch ( pPluIf->ParaPlu.ContPlu.usDept & PLU_TYPE_MASK_REG ) {
        case PLU_OPEN_TYP:
            memset( szMnemonic,
#ifdef DEBUG
                    '',
#else
                    PROGRPT_PRTCHAR_SPACE,
#endif
                    PLU_PRICE_DIGITMAX );
            break;

        case PLU_NON_ADJ_TYP:
        case PLU_ADJ_TYP:
        default:
            D13Digit = 0; /* ### ADD,MOD (2171 for Win32) V1.0 */
            if ( ! ProgRpt_PluGetPrice( pPluIf, &D13Digit ) ) {
                return ( LDT_ERR_ADR );
            }
            /* D13Digit.sBillions   = 0; *//* ### DEL (2171 for Win32) V1.0 */

            /* --- convert long value to string with specified form --- */
            ProgRpt_IntegerToString( pPrintRec, &D13Digit, szMnemonic );
            break;
        }

        /* --- format and set to print buffer --- */
        uchStrLen = ( UCHAR )_tcslen( szMnemonic );
        ProgRpt_FormatPrintBuff( pPrintRec, szMnemonic, uchStrLen, uchDataType );
        break;

    case PROGRPT_TYPE_DATE:
        if ( ProgRpt_PluGetDateTime( &( pPrintRec->PrintItem ), uchMinorClass, pTtlPlu, &Date )) {
            /* --- format and set to print buffer --- */
            ProgRpt_SetDateTimeBuff( pPrintRec, &Date );
        }
        break;

    case PROGRPT_TYPE_STRING:
        if ( ! ProgRpt_PluGetString( &( pPrintRec->PrintItem ), pPluIf, usLoopDeptNo, szMnemonic )) {
            return ( LDT_ERR_ADR );
        }

        /* --- format and set to print buffer --- */
        uchStrLen = ( UCHAR )_tcslen( szMnemonic );
        ProgRpt_FormatPrintBuff( pPrintRec, szMnemonic, uchStrLen, uchDataType );
        break;

    default:
        return ( LDT_ERR_ADR );
    }
    return ( SUCCESS );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_PluMATH( PROGRPT_MATHREC * pMathRec,
*                                      PLUIF  *pPluIf,
*                                      TTLPLU *pTtlPlu )
*
*   Input   :   PROGRPT_MATHREC *pMathRec - address of mathematical record.
*               PLUIF  *pPluIf            - parameter data of plu record.
*               TTLPLU *pTtlPlu           - total/co. data of plu record.
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
*    This function executes mathematical operation.
*==========================================================================
*/
SHORT ProgRpt_PluMATH( PROGRPT_MATHOPE *pMathRec,
                       PLUIF  *pPluIf,
                       TTLPLU *pTtlPlu )
{
    D13DIGITS hugeint;        /* 64 bit value for calculation */
    D13DIGITS Operand1;       /* value for operand 1 */
    D13DIGITS Operand2;       /* value for operand 2 */
    PROGRPT_MATHTTL *pMathTtl;  /* address of math record for total */
    PROGRPT_MATHCO  *pMathCo;   /* address of math record for counter */

    ASSERT( pMathRec != ( PROGRPT_MATHOPE * )NULL );

    /* --- retrieve operand 1 value --- */
    switch ( pMathRec->auchFormat[ 0 ] ) {

    case PROGRPT_MATH_ONEVALUE:

        switch ( pMathRec->Accumulator.uchMajor ) {

        case PROGRPT_ACCUM_TOTAL:
            pMathTtl = ( PROGRPT_MATHTTL * )pMathRec;
            Operand1 = pMathTtl->lOperand1;
            break;

        case PROGRPT_ACCUM_COUNTER:
            pMathCo = ( PROGRPT_MATHCO * )pMathRec;
            Operand1 = pMathCo->sOperand1;
            break;

        default:
            return ( LDT_ERR_ADR );
        }
        break;

    case PROGRPT_MATH_OPERAND:
        if ( ! ProgRpt_PluGetInteger( &( pMathRec->Operand1 ), NULL, pTtlPlu, &Operand1 )) {
            return ( LDT_ERR_ADR );
        }
        break;

    default:
        return ( LDT_ERR_ADR );
    }

    /* --- retrieve operand 2 value --- */
    switch ( pMathRec->uchCondition ) {

    case PROGRPT_MATH_ADD:
    case PROGRPT_MATH_SUB:
    case PROGRPT_MATH_MUL:
    case PROGRPT_MATH_DIV:
        if ( ! ProgRpt_PluGetInteger( &( pMathRec->Operand2 ), pPluIf, pTtlPlu, &Operand2 )) {
            return ( LDT_ERR_ADR );
        }
        break;

    case PROGRPT_MATH_SET:
        /* --- operand2 is not used --- */
        break;

    default:
        return ( LDT_ERR_ADR );
    }

    /* --- calculate operand1 and operand2, and store calculated data to
        specified accumulator --- */
    ProgRpt_CalcAccumulator( pMathRec->uchCondition, &hugeint, &Operand1, &Operand2 );

    /* --- retrieve accumulator address to store calculated data --- */
    ProgRpt_SetAccumulator( &( pMathRec->Accumulator ), &hugeint );

    return ( SUCCESS );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_PluFindPlu( PLUIF   *pPluIf,
*                                         TTLPLU  *pTtlPlu,
*                                         UCHAR   uchMinorClass,
*                                         SHORT   sLoopType,
*                                         USHORT  usFindKeyNo,
*                                         UCHAR   *puchPluNo )
*
*   Input   :   UCHAR uchMinorClass - minor class for plu total.
*               SHORT sLoopType     - loop type (plu#/dept#/report#)
*   Output  :   PLUIF  *pPluIf  - address of retrieved plu parameter.
*               TTLPLU *pTtlPlu - address of retrieved plu total.
*   InOut   :   Nothing
*
**  Return  :   SHORT   sRetCode    - PROGRPT_SUCCESS
*                                     PROGRPT_END_OF_FILE
*                                   
**  Description:
*    This function retrieves a plu parameter which is specified at
*   pPluIf parameter.
*==========================================================================
*/
SHORT ProgRpt_PluFindPlu( PLUIF     *pPluIf,
                          TTLPLU    *pTtlPlu,
                          UCHAR     uchMinorClass,
                          SHORT     sLoopType,
                          USHORT    usFindKeyNo,
                          TCHAR     *puchPluNo)
{
    SHORT       sRetCode;       /* return code for user function */
    SHORT       sReturn;
    DEPTIF      DeptIf;         /* parameter data of department */
    PLUIF_DEP   Plu_Dept;
    PLUIF_REP   Plu_Rept;

    ASSERT( pPluIf  != ( PLUIF  * )NULL );
    ASSERT( pTtlPlu != ( TTLPLU * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));
    ASSERT(( sLoopType == PROGRPT_LOOP_PLU )  ||\
           ( sLoopType == PROGRPT_LOOP_DEPT ) ||\
           ( sLoopType == PROGRPT_LOOP_REPORT ));

    switch ( sLoopType ) {

    case PROGRPT_LOOP_DEPT:
        Plu_Dept.usDept     = usFindKeyNo;
        Plu_Dept.culCounter = pPluIf->ulCounter;
        Plu_Dept.husHandle  = pPluIf->husHandle; /* save handle for sequential read, 04/01/01 */ 
        Plu_Dept.usAdjCo    = pPluIf->usAdjCo;	/* save adjective no for sequential read, 04/01/01 */ 

        sRetCode = SerOpDeptPluRead( &Plu_Dept, 0 );

        switch ( sRetCode ) {

        case OP_PERFORM:
            sRetCode = PROGRPT_SUCCESS;
            _tcsncpy(pPluIf->auchPluNo, Plu_Dept.auchPluNo, OP_PLU_LEN);
            pPluIf->uchPluAdj = Plu_Dept.uchPluAdj;
            pPluIf->ulCounter = Plu_Dept.culCounter;
            pPluIf->husHandle = Plu_Dept.husHandle; /* save handle for sequential read, 04/01/01 */ 
            pPluIf->usAdjCo   = Plu_Dept.usAdjCo;	/* save adjective no for sequential read, 04/01/01 */ 
            pPluIf->ParaPlu = Plu_Dept.ParaPlu;
            break;

        case OP_EOF:
        case OP_NOT_IN_FILE:
        case OP_ABNORMAL_DEPT:
            sRetCode = PROGRPT_END_OF_FILE;
            break;

        case OP_LOCK:
            sRetCode = PROGRPT_LOCK;
            break;

        case OP_FILE_NOT_FOUND:
        default:
            sRetCode = PROGRPT_FILE_NOT_FOUND;
            break;
        }
        break;

    case PROGRPT_LOOP_REPORT:

        ASSERT(( 0 < usFindKeyNo ) && ( usFindKeyNo <= PLU_REPORT_MAX ));
        Plu_Rept.uchReport  = ( UCHAR )usFindKeyNo;
        Plu_Rept.culCounter = pPluIf->ulCounter;
        Plu_Rept.husHandle  = pPluIf->husHandle; /* save handle for sequential read, 04/03/01 */ 
        Plu_Rept.usAdjCo    = pPluIf->usAdjCo;	/* save adjective no for sequential read, 04/03/01 */ 

        sRetCode = SerOpRprtPluRead( &Plu_Rept, 0 );

        switch ( sRetCode ) {

        case OP_PERFORM:
            sRetCode = PROGRPT_SUCCESS;
            _tcsncpy(pPluIf->auchPluNo, Plu_Rept.auchPluNo, OP_PLU_LEN);
            pPluIf->uchPluAdj = Plu_Rept.uchPluAdj;
            pPluIf->ulCounter = Plu_Rept.culCounter;
            pPluIf->husHandle = Plu_Rept.husHandle; /* save handle for sequential read, 04/03/01 */ 
            pPluIf->usAdjCo   = Plu_Rept.usAdjCo;	/* save adjective no for sequential read, 04/03/01 */ 
            pPluIf->ParaPlu = Plu_Rept.ParaPlu;
            break;

        case OP_EOF:
        case OP_NOT_IN_FILE:
            sRetCode = PROGRPT_END_OF_FILE;
            break;

        case OP_LOCK:
            sRetCode = PROGRPT_LOCK;
            break;

        case OP_FILE_NOT_FOUND:
        default:
            sRetCode = PROGRPT_FILE_NOT_FOUND;
            break;
        }
        break;

    case PROGRPT_LOOP_PLU:
    default:

        if ( usFindKeyNo == PROGRPT_ALLREAD ) {
            sRetCode = SerOpPluAllRead( pPluIf, 0 );
        } else {
            /* --- retrieve plu record by plu no.   Saratoga --- */
/*            ASSERT( usFindKeyNo <= MAX_PLU_NO );  */
            _tcsncpy(pPluIf->auchPluNo, puchPluNo, OP_PLU_LEN);
            sRetCode = CliOpPluIndRead( pPluIf, 0 );
        }
        switch ( sRetCode ) {

        case OP_PERFORM:
            sRetCode = PROGRPT_SUCCESS;
            break;

        case OP_EOF:
            sRetCode = PROGRPT_END_OF_FILE;
            break;

        case OP_LOCK:
            sRetCode = PROGRPT_LOCK;
            break;

        case OP_ABNORMAL_DEPT:
        case OP_FILE_NOT_FOUND:
        default:
            sRetCode = PROGRPT_FILE_NOT_FOUND;
            break;
        }
        break;
    }

    if ( sRetCode == PROGRPT_SUCCESS ) {

        DeptIf.usDeptNo = pPluIf->ParaPlu.ContPlu.usDept;
        sReturn = CliOpDeptIndRead( &DeptIf, 0 );

        if (sReturn == OP_PERFORM) {
            if ( pPluIf->ParaPlu.ContPlu.auchContOther[ 2 ] & PLU_USE_DEPTCTL ) {
                pPluIf->ParaPlu.ContPlu.auchContOther[0] = DeptIf.ParaDept.auchControlCode[0];
                pPluIf->ParaPlu.ContPlu.auchContOther[1] = DeptIf.ParaDept.auchControlCode[1];
                pPluIf->ParaPlu.ContPlu.auchContOther[2] = DeptIf.ParaDept.auchControlCode[2];
                pPluIf->ParaPlu.ContPlu.auchContOther[3] = DeptIf.ParaDept.auchControlCode[3];
                pPluIf->ParaPlu.ContPlu.auchContOther[5] = DeptIf.ParaDept.auchControlCode[4];
            } else {
                if ( DeptIf.ParaDept.auchControlCode[2] & PLU_HASH ) {
                    pPluIf->ParaPlu.ContPlu.auchContOther[2] |= PLU_HASH;
                } else {
                    pPluIf->ParaPlu.ContPlu.auchContOther[2] &= ~PLU_HASH;
                }
            }
        }
    }

    if ( sRetCode == PROGRPT_SUCCESS ) {

        /* --- retrieve plu total data --- */
        pTtlPlu->uchMajorClass = CLASS_TTLPLU;
        pTtlPlu->uchMinorClass = uchMinorClass;
        _tcsncpy(pTtlPlu->auchPLUNumber, pPluIf->auchPluNo, OP_PLU_LEN);
        //memcpy(pTtlPlu->auchPLUNumber, pPluIf->auchPluNo, sizeof(pTtlPlu->auchPLUNumber));
        pTtlPlu->uchAdjectNo   = pPluIf->uchPluAdj;
        memset( &pTtlPlu->PLUTotal, 0x00, sizeof( pTtlPlu->PLUTotal ));
        memset( &pTtlPlu->PLUAllTotal, 0x00, sizeof( pTtlPlu->PLUAllTotal ));

        sRetCode = SerTtlTotalRead( pTtlPlu );
        switch ( sRetCode ) {
        case TTL_SUCCESS:
        case TTL_NOTINFILE:
            sRetCode = PROGRPT_SUCCESS;
            break;
        default:
            sRetCode = PROGRPT_FILE_NOT_FOUND;
            break;
        }
    }
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_PluIsLoopTRUE( PROGRPT_LOOPREC *pLoopRec,
*                                           PLUIF           *pPluIf )
*
*   Input   :   PROGRPT_LOOPREC *pLoopRec   - address of loop record.
*               PLUIF           *pPluIf     - address of plu parameter.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - loop condition is TRUE.
*                       FALSE   - loop confition is FALSE.
*
**  Description:
*    This function determines that specified loop condition is TRUE.
*==========================================================================
*/
BOOL ProgRpt_PluIsLoopTRUE( PROGRPT_LOOPREC *pLoopRec,
                            PLUIF   *pPluIf )
{
    BOOL    fRetValue;      /* return value for user application */
    UCHAR   uchLoopCond;    /* condition of loop item */

    ASSERT( pLoopRec != ( PROGRPT_LOOPREC * )NULL );
    ASSERT( pPluIf   != ( PLUIF * )NULL );

    if ( pLoopRec->uchCondition == PROGRPT_ALLREAD ) {
        return ( TRUE );
    }

    uchLoopCond = pLoopRec->uchCondition;

    if ( pPluIf->ParaPlu.ContPlu.auchContOther[ 2 ] & PLU_HASH ) {
        fRetValue = ( uchLoopCond & PROGRPT_COND_HASH ) ? TRUE : FALSE;
    } else if ( pPluIf->ParaPlu.ContPlu.auchContOther[ 0 ] & PLU_MINUS ) {
        fRetValue = ( uchLoopCond & PROGRPT_COND_MINUS ) ? TRUE : FALSE;
    } else if ( pPluIf->ParaPlu.ContPlu.auchContOther[ 2 ] & PLU_SCALABLE ) {
        fRetValue = ( uchLoopCond & PROGRPT_COND_SCALE ) ? TRUE : FALSE;
    } else {
        fRetValue = ( uchLoopCond & PROGRPT_COND_PLUS ) ? TRUE : FALSE;
    }
    return ( fRetValue );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_PluIsPrintTRUE( PROGRPT_PRINTREC * pPrintRec,
*                                            PLUIF  *pPluIF,
*                                            TTLPLU *pTtlPlu )
*
*   Input   :   PROGRPT_PRINTREC *pPrintRec - address of print record.
*               PLUIF            *pPluIF    - address of plu parameter.
*               TTLPLU           *pTtlPlu   - address of plu total.
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
BOOL ProgRpt_PluIsPrintTRUE( PROGRPT_PRINTREC * pPrintRec,
                             PLUIF  *pPluIF,
                             TTLPLU *pTtlPlu )
{
    D13DIGITS   hugeint;    /* 64 bit value */
    BOOL        fRetValue;  /* return value for user application */
    D13DIGITS   D13Value1;  /* 13 digits operand1 value */
    D13DIGITS   D13Value2;  /* 13 digits operand2 value */

    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );

    /* --- determine this item is EOF item --- */

    if (( pPrintRec->PrintItem.uchGroup == ( UCHAR )NULL ) &&
        ( pPrintRec->PrintItem.uchMajor == ( UCHAR )NULL ) &&
        ( pPrintRec->PrintItem.uchMinor == ( UCHAR )NULL ) &&
        ( pPrintRec->PrintItem.uchItem  == ( UCHAR )NULL )) {
        /* --- determine EOF item --- */
        return ( FALSE );
    }

    /* --- retrieve operand1 and operand2 data --- */

    switch ( pPrintRec->uchCondition ) {

    case PROGRPT_PRTCOND_COMPUL:    /* compulsory print */
        fRetValue = TRUE;
        break;

    case PROGRPT_PRTCOND_LESS:      /* ope1 <  ope2 */
    case PROGRPT_PRTCOND_LESSEQ:    /* ope1 <= ope2 */
    case PROGRPT_PRTCOND_GREAT:     /* ope1 >  ope2 */
    case PROGRPT_PRTCOND_GREATEQ:   /* ope1 >= ope2 */
    case PROGRPT_PRTCOND_EQUAL:     /* ope1 == ope2 */
    case PROGRPT_PRTCOND_NOEQUAL:   /* ope1 != ope2 */
    case PROGRPT_PRTCOND_ANDZERO:   /* ope1 != 0 && ope2 != 0 */
    case PROGRPT_PRTCOND_ORZERO:    /* ope1 != 0 || ope2 != 0 */
        if ( ! ProgRpt_PluGetInteger( &( pPrintRec->Operand2 ), pPluIF, pTtlPlu, &hugeint )) {
            return ( FALSE );
        }
        D13Value2 = hugeint;

        /* --- break;... not used --- */

    case PROGRPT_PRTCOND_NOTZERO:   /* ope1 != 0 */
        if ( ! ProgRpt_PluGetInteger( &( pPrintRec->Operand1 ), pPluIF, pTtlPlu, &hugeint )) {
            return ( FALSE );
        }
        D13Value1 = hugeint;

        fRetValue = ProgRpt_IsPrintCondTRUE( pPrintRec->uchCondition, &D13Value1, &D13Value2 );
        break;

    default:
        fRetValue = FALSE;
        break;
    }
    return ( fRetValue );
}

/*
*==========================================================================
**  Synopsis:   UCHAR ProgRpt_PluQueryDataType( PROGRPT_ITEM *pTargetItem )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   UCHAR uchType - data type of specified item.
*                                   PROGRPT_TYPE_UCHAR
*                                   PROGRPT_TYPE_USHORT
*                                   PROGRPT_TYPE_LTOTAL
*                                   PROGRPT_TYPE_DATE
*                                   PROGRPT_TYPE_STRING
*                                   PROGRPT_TYPE_PRICE
*
**  Description:
*    This function determines that which data type is the data pointed by
*   pTargetItem. It returns data type, if function successful.
*==========================================================================
*/
UCHAR ProgRpt_PluQueryDataType( PROGRPT_ITEM *pTargetItem )
{
    UCHAR uchType;  /* data type of target item */

    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );

    uchType = PROGRPT_TYPE_INVALID;

    /* --- what kind of groups ? --- */

    switch ( pTargetItem->uchGroup ) {

    case PROGRPT_GROUP_KEY:         /* loop key group */
        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_PLUKEY_DEPTNO:
            uchType = PROGRPT_TYPE_USHORT;      /* Saratoga */
            break;
            
        case PROGRPT_PLUKEY_REPORT:
            uchType = PROGRPT_TYPE_UCHAR;
            break;

        case PROGRPT_PLUKEY_PLUNO:
            uchType = PROGRPT_TYPE_STRING;      /* Saratoga */
            break;

        default:
            break;
        }
        break;

    case PROGRPT_GROUP_TOTALCO:     /* total-counter group */
        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_TTL_FROM:
        case PROGRPT_TTL_TO:
            uchType = PROGRPT_TYPE_DATE;
            break;

        case PROGRPT_TTL_PLUTOTAL:
        case PROGRPT_TTL_ALLTOTAL:
            uchType = PROGRPT_TYPE_LTOTAL;
            break;

        default:
            break;
        }
        break;

    case PROGRPT_GROUP_PARAM:       /* parameter group */
        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_PLUPARA_ADJNO:
            uchType = PROGRPT_TYPE_ADJNO;
            break;
            
        case PROGRPT_PLUPARA_PRICE:
            uchType = PROGRPT_TYPE_PRICE;
            break;

        case PROGRPT_PLUPARA_DEPTMNEMO:
        case PROGRPT_PLUPARA_MNEMO:
            uchType = PROGRPT_TYPE_STRING;
            break;

        default:
            break;
        }
        break;

    case PROGRPT_GROUP_MNEMO:       /* common mnemonic group */
        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_MNEMO_TRANS:
        case PROGRPT_MNEMO_REPORT:
        case PROGRPT_MNEMO_SPECIAL:
        case PROGRPT_MNEMO_ADJ:
        case PROGRPT_MNEMO_MDEPT:
        case PROGRPT_MNEMO_16CHAR:
        case PROGRPT_MNEMO_40CHAR:
            uchType = PROGRPT_TYPE_STRING;
            break;

        default:
            break;
        }
        break;

    case PROGRPT_GROUP_ACCUM:       /* common accumulator group */
        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_ACCUM_TOTAL:
        case PROGRPT_ACCUM_COUNTER:
            uchType = PROGRPT_TYPE_LTOTAL;
            break;

        default:
            break;
        }
        break;

    default:                        /* illegal group */
        break;
    }
    return ( uchType );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_PluGetInteger( PROGRPT_ITEM *pTargetItem,
*                                           PLUIF        *pPluIf,
*                                           TTLPLU       *pTtlPlu,
*                                           D13DIGITS    *phugeOutput );
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               PLUIF        *pPluIf      - address of plu parameter.
*               TTLPLU       *pTtlPlu     - address of plu total.
*   Output  :   D13DIGITS    *phugeOutput - address of retrieved value.
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    -   long value retrieval is successful.
*                       FALSE   -   long value retrieval is failed.
*                                   
**  Description:
*    This function retrieves a long value which specified at pTargetItem
*   parameter.
*==========================================================================
*/
BOOL ProgRpt_PluGetInteger( PROGRPT_ITEM *pTargetItem,
                            PLUIF        *pPluIf,
                            TTLPLU       *pTtlPlu,
                            D13DIGITS    *phugeOutput )
{
    USHORT  usDeptNo;

    ASSERT(( pTargetItem->uchGroup == PROGRPT_GROUP_KEY ) ||\
           ( pTargetItem->uchGroup == PROGRPT_GROUP_TOTALCO ) ||\
           ( pTargetItem->uchGroup == PROGRPT_GROUP_ACCUM ));

    switch ( pTargetItem->uchGroup ) {
    case PROGRPT_GROUP_KEY:
        ASSERT(( pTargetItem->uchMajor == PROGRPT_PLUKEY_PLUNO ) ||\
               ( pTargetItem->uchMajor == PROGRPT_PLUKEY_DEPTNO ) ||\
               ( pTargetItem->uchMajor == PROGRPT_PLUKEY_REPORT ));

        if (( pPluIf == ( PLUIF * )NULL ) || ( pTtlPlu == ( TTLPLU * )NULL )) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {
        /* Saratoga 
        case PROGRPT_PLUKEY_PLUNO:
            *phugeOutput = pTtlPlu->auchPLUNumber;
            break;  */

        case PROGRPT_PLUKEY_DEPTNO:
            usDeptNo = pPluIf->ParaPlu.ContPlu.usDept & PLU_DEPTNO_MASK;
            *phugeOutput = usDeptNo;
            break;

        case PROGRPT_PLUKEY_REPORT:
            *phugeOutput = pPluIf->ParaPlu.ContPlu.uchRept;
            break;

        default:  
            return ( FALSE );
        }
        break;

    case PROGRPT_GROUP_TOTALCO:
        ASSERT(( pTargetItem->uchMajor == PROGRPT_TTL_PLUTOTAL ) ||\
               ( pTargetItem->uchMajor == PROGRPT_TTL_ALLTOTAL ));
        ASSERT(( pTargetItem->uchItem == PROGRPT_TTLITEM_TOTAL ) ||\
               ( pTargetItem->uchItem == PROGRPT_TTLITEM_CO ));

        if (( pPluIf == ( PLUIF * )NULL ) || ( pTtlPlu == ( TTLPLU * )NULL )) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_TTL_PLUTOTAL:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlPlu->PLUTotal.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlPlu->PLUTotal.lCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_ALLTOTAL:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlPlu->PLUAllTotal.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlPlu->PLUAllTotal.lCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        default:  
            return ( FALSE );
        }
        break;

    case PROGRPT_GROUP_ACCUM:
        ProgRpt_GetAccumulator( pTargetItem, phugeOutput );
        break;

    default:
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_PluGetDateTime( PROGRPT_ITEM *pTargetItem,
*                                            UCHAR  uchMinorClass,
*                                            TTLPLU       *pTtlPlu,
*                                            DATE         *pDate )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               UCHAR uchMinorClass - minor class for department total.
*               TTLPLU       *pTtlPlu     - address of plu total.
*   Output  :   DATE         *pDateTime   - address of date / time data.
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    -   date/time data retrieval is successful.
*                       FALSE   -   date/time data retrieval is failed.
*                                   
**  Description:
*    This function retrieves a date data which specified at pTargetItem
*   parameter.
*==========================================================================
*/
BOOL ProgRpt_PluGetDateTime( PROGRPT_ITEM *pTargetItem,
                             UCHAR  uchMinorClass,
                             TTLPLU       *pTtlPlu,
                             N_DATE         *pDateTime )
{
    TTLPLU  TtlPlu;
    SHORT   sTtlStatus;

    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );
    ASSERT( pDateTime   != ( N_DATE * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));

    if (( uchMinorClass == CLASS_TTLCURPTD ) ||
        ( uchMinorClass == CLASS_TTLCURDAY )) {
        if ( pTargetItem->uchMajor == PROGRPT_TTL_TO ) {
            return ( ProgRpt_GetCurDateTime( pDateTime ));
        }
    }

    if ( pTtlPlu == ( TTLPLU * )NULL ) {

        /* --- print date/time at out of loop --- */

        TtlPlu.uchMajorClass = CLASS_TTLPLU;
        TtlPlu.uchMinorClass = uchMinorClass;
        /* correct PLU No, 04/03/01 */
        tcharnset(TtlPlu.auchPLUNumber, _T('0'), OP_PLU_LEN);
        TtlPlu.auchPLUNumber[11] = _T('1');                                  /* with CD */
        TtlPlu.auchPLUNumber[12] = _T('7');                                  /* with CD */

        sTtlStatus = SerTtlTotalRead( &TtlPlu );
        if (( sTtlStatus != TTL_SUCCESS ) && ( sTtlStatus != TTL_NOTINFILE )) {
            return ( FALSE );
        }
        pTtlPlu = &TtlPlu;
    }

    switch ( pTargetItem->uchMajor ) {

    case PROGRPT_TTL_FROM:
        pDateTime->usMin   = pTtlPlu->FromDate.usMin;
        pDateTime->usHour  = pTtlPlu->FromDate.usHour;
        pDateTime->usMDay  = pTtlPlu->FromDate.usMDay;
        pDateTime->usMonth = pTtlPlu->FromDate.usMonth;
        break;

    case PROGRPT_TTL_TO:
        pDateTime->usMin   = pTtlPlu->ToDate.usMin;
        pDateTime->usHour  = pTtlPlu->ToDate.usHour;
        pDateTime->usMDay  = pTtlPlu->ToDate.usMDay;
        pDateTime->usMonth = pTtlPlu->ToDate.usMonth;
        break;

    default:
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_PluGetString( PROGRPT_ITEM *pTargetItem,
*                                          PLUIF        *pPluIf,
*                                          USHORT       usDeptNo,
*                                          UCHAR        *pszString )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               PLUIF        *pPluIf      - address of plu total.
*               USHORT       usDeptNo     - address of target dept.
*   Output  :   UCHAR        *pszString   - address of buffer for string.
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    -   string data retrieval is success
*                       FALSE   -   string data retrieval is failed
*
**  Description:
*    This function retrieves a string data which specified at pTargetItem
*   parameter.
*==========================================================================
*/
BOOL ProgRpt_PluGetString( PROGRPT_ITEM *pTargetItem,
                           PLUIF        *pPluIf,
                           USHORT       usDeptNo,
                           TCHAR        *pszString )
{
    UCHAR   uchStrLen;
    UCHAR   uchAdjective;
    TCHAR   uchAdjGroup;
    USHORT  usPluType;
    SHORT   sRetCode;
    DEPTIF  DeptIf;

    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );
    ASSERT( pszString   != ( TCHAR * )NULL );

    switch ( pTargetItem->uchGroup ) {

    case PROGRPT_GROUP_KEY:         /* loop key group */
        switch ( pTargetItem->uchMajor ) {

        case PROGRPT_PLUKEY_PLUNO:  /* Saratoga */
            RflConvertPLU(pszString, pPluIf->auchPluNo);
            break;
        }
        break;

    case PROGRPT_GROUP_PARAM:
        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_PLUPARA_DEPTMNEMO:
            if ( usDeptNo == PROGRPT_ALLREAD ) {
                return ( FALSE );
            }

            DeptIf.usDeptNo = usDeptNo;
            if (( sRetCode = CliOpDeptIndRead( &DeptIf, 0 )) != OP_PERFORM ) {
                return ( FALSE );
            }
            memset( pszString, 0x00,(PARA_DEPT_LEN + 1 ) * sizeof(TCHAR));
            _tcsncpy( pszString, DeptIf.ParaDept.auchMnemonic, PARA_DEPT_LEN );

            /* --- fill space character, if NULL character found --- */

            uchStrLen = 0;
            while (( uchStrLen < PARA_DEPT_LEN )  &&
                   ( pszString[ uchStrLen ] == '\0' )) {
#ifdef DEBUG
                pszString[ uchStrLen ] = _T('.');
#else
                pszString[ uchStrLen ] = PROGRPT_PRTCHAR_SPACE;
#endif
                uchStrLen++;
            }
            break;

        case PROGRPT_PLUPARA_MNEMO:
            if ( pPluIf == ( PLUIF * )NULL ) {
                return ( FALSE );
            }

            memset( pszString, 0x00,(PARA_PLU_LEN + 1 ) * sizeof(TCHAR));
            _tcsncpy( pszString, pPluIf->ParaPlu.auchPluName, PARA_PLU_LEN );

            /* --- fill space character, if NULL character found --- */

            uchStrLen = 0;
            while (( uchStrLen < PARA_PLU_LEN )  &&
                   ( pszString[ uchStrLen ] == '\0' )) {
#ifdef DEBUG
                pszString[ uchStrLen ] = _T('.');
#else
                pszString[ uchStrLen ] = PROGRPT_PRTCHAR_SPACE;
#endif
                uchStrLen++;
            }
            break;

        default:
            return ( FALSE );
        }
        break;

    case PROGRPT_GROUP_MNEMO:
        if ( pPluIf == ( PLUIF * )NULL ) {
            uchAdjective = 0;
        } else {
            usPluType = pPluIf->ParaPlu.ContPlu.usDept & PLU_TYPE_MASK_REG;
            if ( usPluType == PLU_ADJ_TYP ) {
                /*----- Classify 4 Adjective groups -----*/
                uchAdjGroup = pPluIf->ParaPlu.ContPlu.auchContOther[ 3 ];
                if ( uchAdjGroup & PLU_USE_ADJG1 ) {
                    uchAdjective = ADJ_GP1VAR1_ADR;
                } else if ( uchAdjGroup & PLU_USE_ADJG2 ) {
                    uchAdjective = ADJ_GP2VAR1_ADR;
                } else if ( uchAdjGroup & PLU_USE_ADJG3 ) {
                    uchAdjective = ADJ_GP3VAR1_ADR;
                } else {                                                                     /* Group 4 */
                    uchAdjective = ADJ_GP4VAR1_ADR;
                }
                uchAdjective += ( pPluIf->uchPluAdj - 1 );
            } else {
                uchAdjective = 0;
            }
        }
        ProgRpt_GetMnemonic( pTargetItem, pszString, uchAdjective );
        break;

    default:
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_PluGetPrice( PLUIF        *pPluIf
*                                         D13DIGITS    *pd13Price )
*
*   Input   :   PLUIF     *pPluIf    -   address of PLU parameter
*   Output  :   D13DIGITS *pd13Price -   address of buffer for price.
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    -   price data retrieval is success
*                       FALSE   -   price data retrieval is failed
*
**  Description:
*    This function retrieves a price data which specified at pTargetItem
*   parameter.
*==========================================================================
*/
BOOL ProgRpt_PluGetPrice( PLUIF *pPluIf, D13DIGITS *pd13Price )
{
    ASSERT( pd13Price != NULL );

    if ( pPluIf == NULL || pd13Price == NULL ) {
        return ( FALSE );
    }

    *pd13Price = RflFunc3bto4b(pPluIf->ParaPlu.auchPrice );
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_PluGetAdjNo( PROGRPT_PRINTREC *pPrintRec,
*                                         PLUIF        *pPluIf
*                                         UCHAR        *pszString )
*
*   Input   :   PROGRPT_PRINTREC *pPrintRec - address of print record.
*               PLUIF        *pPluIf      - address of plu parameter.
*   Output  :   UCHAR        *pszString   - address of buffer for string.
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    -   price data retrieval is success
*                       FALSE   -   price data retrieval is failed
*
**  Description:
*    This function retrieves a adjective code which specified at pTargetItem
*   parameter.
*==========================================================================
*/
BOOL ProgRpt_PluGetAdjNo( PROGRPT_PRINTREC *pPrintRec,
                          PLUIF *pPluIf,
                          TCHAR *pszString )
{
    D13DIGITS   D13Digit;       /* value for target print item */

    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );
    ASSERT( pszString != ( UCHAR * )NULL );

    if ( pPluIf == ( PLUIF * )NULL ) {
        return ( FALSE );
    }

    switch ( pPluIf->ParaPlu.ContPlu.usDept & PLU_TYPE_MASK_REG ) {
    case PLU_ADJ_TYP:
        D13Digit = (D13DIGITS)pPluIf->uchPluAdj; /* ### MOD (2171 for Win32) V1.0 */
        /* D13Digit.sBillions   = 0; */
        ProgRpt_IntegerToString( pPrintRec, &D13Digit, pszString );
        break;

    case PLU_OPEN_TYP:
    case PLU_NON_ADJ_TYP:
    default:
#ifdef DEBUG
        pszString[ 0 ] = '';
#else
        pszString[ 0 ] = PROGRPT_PRTCHAR_SPACE;
#endif
        break;
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_PluPrintRootPara( PROGRPT_PRINTREC *pPrintRec,
*                                              SHORT   usLoopDeptNo,
*                                              UCHAR   uchLoopReportNo );
*
*   Input   :   PROGRPT_PRINTREC *pPrintRec - address of print record.
*               UCHAR uchLoopDeptNo     - dept no. of loop key
*               UCHAR uchLoopReportNo   - report code of loop key
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    -   item is printed.
*                       FALSE   -   item is not printed.
*                                   
**  Description:
*    This function prints a item which is out of LOOP(PLU#) operation.
*==========================================================================
*/
BOOL ProgRpt_PluPrintRootPara( PROGRPT_PRINTREC *pPrintRec,
                               USHORT  usLoopDeptNo,
                               UCHAR   uchLoopReportNo )
{
    D13DIGITS   D13Digit;       /* value for target print item */
    DEPTIF      DeptIf;         /* structure for department parameter */
    SHORT       sRetCode;       /* return code for CliOp*** function */
    UCHAR       uchStrLen;      /* length of mnemonic data without NULL */
    TCHAR       szMnemonic[ PROGRPT_MAX_40CHAR_LEN + 1 ];
                                /* mnemonic work area */

    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );
    ASSERT( uchLoopReportNo <= PLU_REPORT_MAX );

    switch ( pPrintRec->PrintItem.uchGroup ) {
    case PROGRPT_GROUP_KEY:
        switch ( pPrintRec->PrintItem.uchMajor ) {
        case PROGRPT_PLUKEY_DEPTNO:
            D13Digit = ( D13DIGITS )usLoopDeptNo;   /* ### MOD (2171 for Win32) V1.0 */
            break;
        case PROGRPT_PLUKEY_REPORT:
            D13Digit = ( D13DIGITS )uchLoopReportNo;/* ### MOD (2171 for Win32) V1.0 */
            break;
        case PROGRPT_PLUKEY_PLUNO:
        default:
            return ( FALSE );
        }
        /* D13Digit.sBillions = 0; *//* ### DEL (2171 for Win32) V1.0 */
        ProgRpt_IntegerToString( pPrintRec, &D13Digit, szMnemonic );

        uchStrLen = ( UCHAR )_tcslen( szMnemonic );
        ProgRpt_FormatPrintBuff( pPrintRec, szMnemonic, uchStrLen, PROGRPT_TYPE_UCHAR );
        break;

    case PROGRPT_GROUP_PARAM:
        switch ( pPrintRec->PrintItem.uchMajor ) {
        case PROGRPT_PLUPARA_DEPTMNEMO:
            if ( usLoopDeptNo == PROGRPT_ALLREAD ) {
                return ( FALSE );
            }

            DeptIf.usDeptNo = usLoopDeptNo;
            if (( sRetCode = CliOpDeptIndRead( &DeptIf, 0 )) != OP_PERFORM ) {
                return ( FALSE );
            }
            memset( szMnemonic, 0x00,(PARA_DEPT_LEN + 1 ) * sizeof(TCHAR));
            _tcsncpy( szMnemonic, DeptIf.ParaDept.auchMnemonic, PARA_DEPT_LEN );

            /* --- fill space character, if NULL character found --- */

            uchStrLen = 0;
            while (( uchStrLen < PARA_DEPT_LEN )  &&
                   ( szMnemonic[ uchStrLen ] == '\0' )) {
#ifdef DEBUG
                szMnemonic[ uchStrLen ] = _T('.');
#else
                szMnemonic[ uchStrLen ] = PROGRPT_PRTCHAR_SPACE;
#endif
                uchStrLen++;
            }
            uchStrLen = ( UCHAR )_tcslen( szMnemonic );
            ProgRpt_FormatPrintBuff( pPrintRec, szMnemonic, uchStrLen, PROGRPT_TYPE_STRING );
            break;

        default:
            return ( FALSE );
        }
        break;

    case 0:         /* end-of-file record */

        if (( uchProgRptRow < pPrintRec->uchRow ) &&
            ( auchProgRptBuff[ 0 ] == '\0' )) {

            /* --- print last row item --- */
#ifdef DEBUG
            auchProgRptBuff[ 0 ] = _T('}');
#else
            auchProgRptBuff[ 0 ] = PROGRPT_PRTCHAR_SPACE;
#endif
            uchProgRptRow = pPrintRec->uchRow;
        }
        break;

    default:
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*===========================================================================
** Synopsis:    SHORT ProgRpt_PluLabelAnalysis(PARAPLU *ParaPLU)
*
*     Input:   *ParaPLU
*               uchEversion
*    Output:    ParaPLU->aucPLUNumber : converted PLU number (BCD format)
*     InOut:    Nothing
*
** Return:      SUCCESS : successful
*               sReturn : error
*
** Description: This function convert PLU number (BCD format).
*===========================================================================
*/
SHORT   ProgRpt_PluLabelAnalysis (PARAPLU *ParaPlu)
{
    SHORT   sRetCode;

    /* Analyze PLU Label & Get converted PLU Number : ParaPLU->aucPLUNumber */
    sRetCode = MaintPluLabelAnalysis(ParaPlu, MaintPlu.uchEVersion);
    if ( sRetCode != SUCCESS ) {
        return (sRetCode);
    }
    return (SUCCESS);
}

/* ===== End of File (_PROGPLU.C) ===== */