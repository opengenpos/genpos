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
*   Program Name       : _PROGDPT.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : The provided function names are as follows.
*
*   ProgRpt_DeptPrint()         - print current/saved daily/PTD dept total.
*   ProgRpt_DeptReset()         - reset current daily/PTD to saved total file.
*   ProgRpt_DeptIsResettable()  - determine current daily/PTD is resettable.
*   ProgRpt_DeptIsBadLoopKey()  - determine loop key is valid or invalid.
*
*   ProgRpt_DeptLOOP()          - execute loop operation in dept ttl.
*   ProgRpt_DeptPRINT()         - execute print operation in dept ttl.
*   ProgRpt_DeptMATH()          - execute mathematics operation in dept ttl.
*
*   ProgRpt_DeptFindDept()      - find department para by (major) dept no.
*   ProgRpt_DeptIsLoopTRUE()    - determine loop condition is TRUE.
*   ProgRpt_DeptIsPrintTRUE()   - determine print condition is TRUE.
*   ProgRpt_DeptQueryDataType() - get type of specified data.
*   ProgRpt_DeptGetInteger()    - get integer from department total/para.
*   ProgRpt_DeptGetDateTime()   - get date data from department total.
*   ProgRpt_DeptGetString()     - get string data from department total.
*   ProgRpt_DeptPrintRootPara() - print root(out of LOOP) parameter.
*
*  ------------------------------------------------------------------------
*    Update Histories  :
* Date      : Ver.Rev  : NAME       : Description
* Feb-29-96 : 03.01.00 : T.Nakahata : Initial
* Mar-29-96 : 03.01.02 : T.Nakahata : Check TTL_NOTINFILE in IsResettable
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
**  Synopsis:   SHORT ProgRpt_DeptPrint( UCHAR  uchProgRptNo,
*                                        UCHAR  uchMinorClass,
*                                        USHORT usDeptNo,
*                                        UCHAR  uchMjrDeptNo )
*
*   Input   :   UCHAR  uchProgRptNo  - programmable report no.
*               UCHAR  uchMinorClass - minor class for department total.
*               USHORT usDeptNo      - target department no.
*               UCHAR  uchMjrDeptNo  - target major department no.
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
*    This function prints specified department report.
*==========================================================================
*/
SHORT ProgRpt_DeptPrint( UCHAR  uchProgRptNo,
                         UCHAR  uchMinorClass,
                         USHORT usDeptNo,
                         UCHAR  uchMjrDeptNo )
{
    SHORT   sRetCode;           /* return code */
    PROGRPT_FHEAD   FileInfo;   /* information about prog report file */
    PROGRPT_RPTHEAD ReportInfo; /* information about report format */
    USHORT  usMaxNoOfRec;       /* maximum no. of record in report format */
    USHORT  usLoopIndex;        /* loop index for format record */
    ULONG   ulReadOffset;       /* read offset for format record */
    BOOL    fMDeptLoop;
    BOOL    fMjrAllRead;
    USHORT  usLoopDeptNo;
    UCHAR   uchLoopMjrNo;
    UCHAR   uchLoopBeginRow;
    UCHAR   uchLoopBeginColumn;
    UCHAR   uchLoopEndRow;      /* row position after root loop */
    USHORT  usLoopBeginRecNo;
    ULONG   ulLoopBeginOffset;

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));
    ASSERT( ! ProgRpt_DeptIsBadLoopKey( &sRetCode, uchProgRptNo, usDeptNo, uchMjrDeptNo ));

    /* --- determine specified report format is correct or incorrect --- */
    if (( sRetCode = ProgRpt_GetProgRptInfo( uchProgRptNo, &ReportInfo )) != PROGRPT_SUCCESS ) {
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    if ( ReportInfo.usFileType != PROGRPT_FILE_DEPT ) {
        return ( LDT_PROHBT_ADR );
    }

    if ( ReportInfo.usDevice == PROGRPT_DEVICE_MLD ) {
        if (( uchMinorClass == CLASS_TTLSAVPTD ) || ( uchMinorClass == CLASS_TTLSAVDAY )) {
            return ( LDT_PROHBT_ADR );
        }
    }

    /* --- initialize current row item before print dept report --- */
    memset( auchProgRptBuff, 0x00, sizeof( auchProgRptBuff ));
    uchProgRptColumn = 1;
    uchProgRptRow    = 1;

    /* --- read a record until end of file --- */
    fMDeptLoop = FALSE;
    uchLoopMjrNo = uchMjrDeptNo;
    usLoopDeptNo = usDeptNo;

    ProgRpt_GetFileHeader( &FileInfo );

    ulReadOffset = FileInfo.aulOffsetReport[ uchProgRptNo - 1 ] + sizeof( PROGRPT_RPTHEAD );

    usLoopIndex  = 0;
    usMaxNoOfRec = ReportInfo.usNoOfLoop  + ReportInfo.usNoOfPrint + ReportInfo.usNoOfMath;

    while ( usLoopIndex < usMaxNoOfRec ) {
		PROGRPT_RECORD    auchRecord;  /* buffer for format record */
		ULONG   ulActualRead;       /* actual length of read record function */

        /* --- retrieve a formatted record --- */
		//11-11-03 JHHJ SR 201 Parameter 4 change from USHORT to ULONG
        ProgRpt_ReadFile( ulReadOffset, &auchRecord, PROGRPT_MAX_REC_LEN, &ulActualRead );
        if ( ulActualRead != PROGRPT_MAX_REC_LEN ) {
            return ( LDT_ERR_ADR );
        }

        /* --- analyze operation code of format record, and store it to current row item buffer --- */
        switch ( auchRecord.Loop.uchOperation ) {

        case PROGRPT_OP_PRINT:  /* print operation */
            sRetCode = ProgRpt_DeptPRINT( &ReportInfo, &auchRecord.Print, uchMinorClass, NULL, NULL, uchLoopMjrNo);
            break;

        case PROGRPT_OP_MATH:   /* mathematics operation */
            sRetCode = ProgRpt_DeptMATH( &auchRecord.MathOpe, NULL );
            break;

        case PROGRPT_OP_LOOP:   /* loop operation */
            ProgRpt_PrintLoopBegin( &ReportInfo, &auchRecord.Loop);

            switch (auchRecord.Loop.LoopItem.uchMajor ) {

            case PROGRPT_DEPTKEY_DEPTNO:
                ProgRpt_PrintAndFeed( &ReportInfo, &auchRecord );
                sRetCode = ProgRpt_DeptLOOP( &ReportInfo, &auchRecord.Loop, uchMinorClass, usLoopDeptNo, uchLoopMjrNo, fMDeptLoop, ulReadOffset );
                usLoopIndex  += auchRecord.Loop.usLoopEnd;
                ulReadOffset += PROGRPT_MAX_REC_LEN * auchRecord.Loop.usLoopEnd;
                break;

            case PROGRPT_DEPTKEY_MDEPTNO:
                if ((auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_BEGIN_ROOT ) && ( ! fMDeptLoop )) {
                    /* --- loop begin (major dept #) --- */
                    fMDeptLoop = TRUE;

                    uchLoopBeginRow    = uchProgRptRow;
                    uchLoopBeginColumn = uchProgRptColumn;
                    usLoopBeginRecNo   = usLoopIndex;
                    usLoopDeptNo       = PROGRPT_ALLREAD;
                    ulLoopBeginOffset  = ulReadOffset;

                    if ( uchMjrDeptNo == PROGRPT_ALLREAD ) {
                        fMjrAllRead  = TRUE;
                        uchLoopMjrNo = 1;
                    } else {
                        fMjrAllRead  = FALSE;
                        uchLoopMjrNo = uchMjrDeptNo;
                    }
                } else if (auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_END_ROOT ) {
                    /* --- print current row item, if next row item is retrieved --- */
                    ProgRpt_PrintAndFeed( &ReportInfo, &auchRecord );
                    uchLoopEndRow    = auchRecord.Loop.uchRow;
                    uchLoopMjrNo++;

                    if ( fMjrAllRead && ( uchLoopMjrNo <= MAX_MDEPT_NO )) {
                        /* --- continue loop next mjr dept no. --- */
                        uchProgRptRow    = uchLoopBeginRow;
                        uchProgRptColumn = uchLoopBeginColumn;
                        usLoopIndex      = usLoopBeginRecNo;
                        ulReadOffset     = ulLoopBeginOffset;
                    } else {
                        /* --- loop is now finished --- */
                        fMDeptLoop = FALSE;
                        uchLoopMjrNo  = uchMjrDeptNo;
                        usLoopDeptNo  = usDeptNo; 
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

    if (( uchMinorClass == CLASS_TTLSAVDAY ) || ( uchMinorClass == CLASS_TTLSAVPTD )) {
		TTLDEPT TtlDept = { 0 };            /* structure for total module i/f */

        /* --- declare saved dept total is already reported --- */
        TtlDept.uchMajorClass = CLASS_TTLDEPT;
        TtlDept.uchMinorClass = uchMinorClass;
        sRetCode = SerTtlIssuedReset( &TtlDept, TTL_NOTRESET );

        if (( sRetCode = TtlConvertError( sRetCode )) != SUCCESS ) {
            /* --- print out error code --- */
            RptPrtError( sRetCode );
        }
    }
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_DeptReset( UCHAR uchMinorClass )
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
*    This function resets current daily/PTD department total.
*    An application must call ProgRpt_DeptIsRessetable() function before
*   calling this function. If the specified total is resettable, it may
*   call this function.
*    This function deletes saved daily/PTD total, and then copies current
*   total to saved total. Department total is resetted at only EOD/PTD
*   report.
*==========================================================================
*/
SHORT ProgRpt_DeptReset( UCHAR uchMinorClass )
{
    TTLREPORT   TtlReport;      /* structure for reset dept total */
    SHORT       sRetCode;       /* return value for user application */
    USHORT      usExecuteFlag;  /* execute flag for EOD/PTD report */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ));

    ASSERT( RptEODChkFileExist( FLEX_DEPT_ADR ) == RPT_FILE_EXIST );

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

    /* --- reset dept total at EOD/PTD reset report --- */
    usExecuteFlag = RPT_RESET_DEPT_BIT;

    /* --- reset specified department total file --- */
    ASSERT( ProgRpt_DeptIsResettable( uchMinorClass ));
    sRetCode = SerTtlTotalReset( &TtlReport, usExecuteFlag );

    /* --- convert return code to lead-thru code --- */
    return ( TtlConvertError( sRetCode ));
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_DeptIsResettable( UCHAR uchMinorClass )
*
*   Input   :   UCHAR uchMinorClass - minor class for department total.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - current total is resettable.
*                       FALSE   - current total is not resettable.
*   
**  Description:
*    This function detemines specified current department total is able to
*   reset or not.
*    If saved total file is not issued, current total is not resettable.
*==========================================================================
*/
BOOL ProgRpt_DeptIsResettable( UCHAR uchMinorClass )
{
    TTLREPORT   TtlReport;      /* structure for reset dept total */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ));

    if ( RptEODChkFileExist( FLEX_DEPT_ADR ) != RPT_FILE_EXIST ) {
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
    TtlReport.uchMajorClass = CLASS_TTLDEPT;
    TtlReport.ulNumber = 0;

    if ( SerTtlIssuedCheck( &TtlReport, TTL_NOTRESET ) == TTL_NOT_ISSUED ) {
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_DeptIsBadLoopKey( SHORT  *psLDTCode,
*                                              UCHAR  uchProgRptNo,
*                                              USHORT usDeptNo,
*                                              UCHAR  uchMjrDeptNo )
*
*   Input   :   SHORT  *psLDTCode    - address of lead-thru code.
*               UCHAR  uchProgRptNo  - programmable report no.
*               USHORT usDeptNo      - target department no.
*               UCHAR  uchMjrDeptNo  - target major department no.
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
BOOL ProgRpt_DeptIsBadLoopKey( SHORT  *psLDTCode,
                               UCHAR  uchProgRptNo,
                               USHORT usDeptNo,
                               UCHAR  uchMjrDeptNo )
{
    /* PARAFLEXMEM FlexMem;    / structure for flexible memory */
    *psLDTCode = ( LDT_KEYOVER_ADR );

    /* --- determine specified prog. report no. is valid or invalid --- */
    if (( uchProgRptNo < 1 ) || ( MAX_PROGRPT_FORMAT < uchProgRptNo )) {
        return ( TRUE );
    }

    /* --- determine specified department no. is valid or invalid --- */
    if (MAX_DEPT_NO <  usDeptNo) { /* 9999 */	/* fixed for always returns error, 09/08/01 */
    /* if (usDeptNo <  MAX_DEPT_NO) { / 9999 */
        return ( TRUE );
    }
/*    
    FlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
    FlexMem.uchAddress    = FLEX_DEPT_ADR;
    CliParaRead( &FlexMem );
    if ( FlexMem.ulRecordNumber < usDeptNo ) {
        return ( TRUE );
    }
*/
    /* --- determine specified major dept. no. is valid or invalid --- */
    if ( MAX_MDEPT_NO < uchMjrDeptNo ) {
        return ( TRUE );
    }

    *psLDTCode = SUCCESS;
    return ( FALSE );
}


/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_DeptLOOP( PROGRPT_RPTHEAD * pReportInfo,
*                                       PROGRPT_LOOPREC * pLoopRec,
*                                       UCHAR  uchMinorClass,
*                                       USHORT usDeptNo,
*                                       UCHAR  uchMjrDeptNo,
*                                       BOOL    fByMjrDept,
*                                       ULONG   ulBeginOffset )
*
*   Input   :   PROGRPT_RPTHEAD * pReportInfo - address of report header.
*               PROGRPT_LOOPREC * pLoopRec    - address of print record.
*               UCHAR   uchMinorClass   - minor class for department total.
*               USHORT  usDeptNo        - target department no.
*               UCHAR   uchMjrDeptNo    - target major department no.
*               BOOL    fByMjrDept      - boolean value (read by major dept)
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
SHORT ProgRpt_DeptLOOP( PROGRPT_RPTHEAD * pReportInfo,
                        PROGRPT_LOOPREC * pLoopRec,
                        UCHAR   uchMinorClass,
                        USHORT  usDeptNo,
                        UCHAR   uchMjrDeptNo,
                        BOOL    fByMjrDept,
                        ULONG   ulBeginOffset )
{
    MDEPTIF     ParaMjrDept;    /* operational parameter of major department */
    ULONG       ulReadOffset;   /* offset for read programmed format */
    BOOL        fLoopEnd;       /* boolean value for loop is end */
    SHORT       sRetCode;       /* return code of this function */
    UCHAR       uchLoopBeginRow;    /* row of "loop begin" item */
    BOOL        fLinkedItemFound;

    ASSERT( pReportInfo != ( PROGRPT_RPTHEAD * )NULL );
    ASSERT( pLoopRec != ( PROGRPT_LOOPREC * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));
    ASSERT( usDeptNo     <= MAX_DEPT_NO );
    ASSERT( uchMjrDeptNo <= MAX_MDEPT_NO );
    ASSERT(( fByMjrDept == TRUE ) || ( fByMjrDept == FALSE ));
    
    /* --- prepare loop counter & flag before record retrieval --- */
    if ( fByMjrDept ) { /* read dept by major dept no. */
        if ( uchMjrDeptNo == 1 ) {
            /* --- major dept is 1, read default major dept at first --- */
            ParaMjrDept.uchMajorDeptNo = 0;
        } else {
            ParaMjrDept.uchMajorDeptNo = uchMjrDeptNo;
        }
        ParaMjrDept.usDeptNo      = 0;
    } else {            /* read dept by dept no. */
        ParaMjrDept.uchMajorDeptNo = 0;
        if ( usDeptNo == PROGRPT_ALLREAD ) {
            ParaMjrDept.usDeptNo = 0;
        } else {
            ParaMjrDept.usDeptNo = usDeptNo;
        }
    }
    
    ParaMjrDept.ulCounter = 0;  /* saratoga */

    uchLoopBeginRow    = pLoopRec->uchRow;
    fLinkedItemFound = FALSE;
    do {    /* loop until all requested department record is read */ 
		TTLDEPT     TtlDept = { 0 };        /* total data of retrieved department */

        /* --- retrieve department record. target dept no. is automatically increment in following function --- */
        sRetCode = ProgRpt_DeptFindDept( &ParaMjrDept, &TtlDept, uchMinorClass, fByMjrDept );

        /* --- determine current key record is in accord with specified condition --- */
        if (( sRetCode == PROGRPT_SUCCESS ) && ( ProgRpt_DeptIsLoopTRUE( pLoopRec, &ParaMjrDept.ParaDept ))) {

            /* --- restore print position to loop begin --- */
            fLinkedItemFound = TRUE;
            fProgRptEmptyLoop = TRUE;
            uchProgRptRow    = uchLoopBeginRow; 

            /* --- read a record until end of loop --- */
            fLoopEnd = FALSE;
            ulReadOffset = ulBeginOffset + ( ULONG )PROGRPT_MAX_REC_LEN;

            while ( ! fLoopEnd ) {
				ULONG       ulActualRead;   /* actual length of read record function */
				PROGRPT_RECORD    auchRecord; /* buffer for format record */

                ProgRpt_ReadFile( ulReadOffset, &auchRecord, PROGRPT_MAX_REC_LEN, &ulActualRead );
                if ( ulActualRead != PROGRPT_MAX_REC_LEN ) {
                    return ( LDT_ERR_ADR );
                }

                /* --- analyze operation code of format record, and store it to current row item buffer --- */
                switch ( auchRecord.Loop.uchOperation ) {
                case PROGRPT_OP_PRINT:
                    ProgRpt_DeptPRINT( pReportInfo, &auchRecord.Print, uchMinorClass, &ParaMjrDept.ParaDept, &TtlDept, ParaMjrDept.uchMajorDeptNo );
                    break;

                case PROGRPT_OP_MATH:
                    ProgRpt_DeptMATH(&auchRecord.MathOpe, &TtlDept );
                    break;

                case PROGRPT_OP_LOOP:
                    if ((auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_END_ROOT ) ||
                        (auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_END_NEST )) {
                        if (auchRecord.Loop.LoopItem.uchMajor == PROGRPT_DEPTKEY_DEPTNO ) {
                            /* --- print current row item, if next row item is retrieved --- */
                            ProgRpt_PrintAndFeed( pReportInfo, &auchRecord.Loop );
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

        /* --- determine all linked dept has been read --- */
        if ( fByMjrDept && ( sRetCode == PROGRPT_END_OF_FILE )) {
            /* --- default major dept is finished, read major dept #1 --- */
            if ( ParaMjrDept.uchMajorDeptNo == 0 ) {
                ParaMjrDept.uchMajorDeptNo = 1;
                ParaMjrDept.usDeptNo      = 0;
                ParaMjrDept.ulCounter = 0;  /* saratoga */
                sRetCode = PROGRPT_SUCCESS;
            }
        }
    } while (( usDeptNo == PROGRPT_ALLREAD ) && ( sRetCode == PROGRPT_SUCCESS ));

    if ( ! fLinkedItemFound ) {
		ULONG       ulActualRead;   /* actual length of read record function */
		PROGRPT_RECORD    auchRecord; /* buffer for format record */

		/* --- there is no liked items in MDEPT LOOP, skip print DEPT LOOP --- */
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

    /* --- exit this function, if department record retrieval is failed --- */
    fProgRptEmptyLoop = FALSE;
    return ( ProgRpt_ConvertError( sRetCode ));
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_DeptPRINT( PROGRPT_RPTHEAD *pReportInfo,
*                                        PROGRPT_PRINTREC *pPrintRec,
*                                        UCHAR   uchMinorClass,
*                                        OPDEPT_PARAENTRY  *pParaDept,
*                                        TTLDEPT *pTtlDept,
*                                        UCHAR   uchLoopMjrNo );
*
*   Input   :   PROGRPT_RPTHEAD * pReportInfo - address of report header.
*               PROGRPT_PRINTREC *pPrintRec - address of print record.
*               UCHAR  uchMinorClass - minor class for cashier total.
*               OPDEPT_PARAENTRY  *pParaDept  - operational parameter of department
*               TTLDEPT *pTtlDept   - total/co. data of department record.
*               UCHAR   uchLoopMjrNo- major dept no. of loop key
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
SHORT ProgRpt_DeptPRINT( PROGRPT_RPTHEAD *pReportInfo,
                         PROGRPT_PRINTREC *pPrintRec,
                         UCHAR    uchMinorClass,
                         OPDEPT_PARAENTRY *pParaDept,
                         TTLDEPT *pTtlDept,
                         UCHAR uchLoopMjrNo )
{
    D13DIGITS   hugeint;            /* 64 bit value for accumulator */
    UCHAR       uchDataType;        /* target print item data type */
    D13DIGITS   D13Digit;       /* value for target print item */
    N_DATE      Date;               /* date for target print item */
    TCHAR       szMnemonic[ PROGRPT_MAX_40CHAR_LEN + 1 ];
                                /* mnemonic work area */
    UCHAR   uchStrLen;          /* length of mnemonic data without NULL */

    ASSERT( pReportInfo != ( PROGRPT_RPTHEAD * )NULL );
    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );
    ASSERT( uchLoopMjrNo <= MAX_MDEPT_NO );

    /* --- determine specified condition is TRUE --- */
    if ( ! ProgRpt_DeptIsPrintTRUE( pPrintRec, pTtlDept )) {
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

    /* --- execute print operation with specified format & style --- */
    memset( szMnemonic, 0x00, sizeof( szMnemonic ));

    uchLoopMjrNo = ( UCHAR )(( uchLoopMjrNo == 0 ) ? 1 : uchLoopMjrNo );

    if (( pTtlDept == ( TTLDEPT * )NULL ) &&
        ( pParaDept == ( OPDEPT_PARAENTRY * )NULL )) {  /* root record */

        if ( ProgRpt_DeptPrintRootPara( pPrintRec, uchLoopMjrNo )) {
            return ( SUCCESS );
        }
    }

    /* --- retrieve print item --- */
    uchDataType = ProgRpt_DeptQueryDataType( &( pPrintRec->PrintItem ));
    switch ( uchDataType ) {

    case PROGRPT_TYPE_UCHAR:
    case PROGRPT_TYPE_LTOTAL:

        if ( pTtlDept != ( TTLDEPT * )NULL ) {  /* saratoga */
            pTtlDept->uchMajorDEPTNo = uchLoopMjrNo;
        }
        if ( ! ProgRpt_DeptGetInteger( &( pPrintRec->PrintItem ), pTtlDept, &hugeint ) ) {
            return ( LDT_ERR_ADR );
        }
        D13Digit = hugeint;

        /* --- convert long value to string with specified form --- */
        ProgRpt_IntegerToString( pPrintRec, &D13Digit, szMnemonic );

        /* --- format and set to print buffer --- */
        uchStrLen = ( UCHAR )_tcslen( szMnemonic );
        ProgRpt_FormatPrintBuff( pPrintRec, szMnemonic, uchStrLen, uchDataType );
        break;

    case PROGRPT_TYPE_DATE:
        if ( ProgRpt_DeptGetDateTime( &( pPrintRec->PrintItem ), uchMinorClass, pTtlDept, &Date )) {
            /* --- format and set to print buffer --- */
            ProgRpt_SetDateTimeBuff( pPrintRec, &Date );
        }
        break;

    case PROGRPT_TYPE_STRING:
        if ( ! ProgRpt_DeptGetString( &( pPrintRec->PrintItem ), pParaDept, szMnemonic, uchLoopMjrNo )) {
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
**  Synopsis:   SHORT ProgRpt_DeptMATH( PROGRPT_MATHREC * pMathRec,
*                                       TTLDEPT *pTtlDept )
*
*   Input   :   PROGRPT_MATHREC * pMathRec - address of mathematical record.
*               TTLDEPT *pTtlDept   - total/co. data of department record.
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
SHORT ProgRpt_DeptMATH( PROGRPT_MATHOPE *pMathRec,
                        TTLDEPT *pTtlDept )
{
    D13DIGITS hugeint = 0;        /* 64 bit value for calculation */
    D13DIGITS Operand1 = 0;       /* value for operand 1 */
    D13DIGITS Operand2 = 0;       /* value for operand 2 */

    ASSERT( pMathRec != ( PROGRPT_MATHOPE * )NULL );

    /* --- retrieve operand 1 value --- */
    switch ( pMathRec->auchFormat[ 0 ] ) {
    case PROGRPT_MATH_ONEVALUE:
        switch ( pMathRec->Accumulator.uchMajor ) {
        case PROGRPT_ACCUM_TOTAL:
            Operand1 = pMathRec->lOperand1;
            break;

        case PROGRPT_ACCUM_COUNTER:
            Operand1 = pMathRec->sOperand1;
            break;

        default:
            return ( LDT_ERR_ADR );
        }
        break;

    case PROGRPT_MATH_OPERAND:
        if ( ! ProgRpt_DeptGetInteger( &( pMathRec->Operand1 ), pTtlDept, &Operand1 )) {
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
        if ( ! ProgRpt_DeptGetInteger( &( pMathRec->Operand2 ), pTtlDept, &Operand2 )) {
            return ( LDT_ERR_ADR );
        }
        break;

    case PROGRPT_MATH_SET:
        /* --- operand2 is not used --- */
        break;

    default:
        return ( LDT_ERR_ADR );
    }

    /* --- calculate operand1 and operand2, and store calculated data to specified accumulator --- */
    ProgRpt_CalcAccumulator( pMathRec->uchCondition, &hugeint, &Operand1, &Operand2 );

    /* --- retrieve accumulator address to store calculated data --- */
    ProgRpt_SetAccumulator( &( pMathRec->Accumulator ), &hugeint );

    return ( SUCCESS );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_DeptFindDept( MDEPTIF *pMjrDeptIF,
*                                           TTLDEPT *pTtlDept,
*                                           UCHAR   uchMinorClass,
*                                           BOOL    fByMjrDept )
*
*   Input   :   MDEPTIF *pMjrDeptIF - address of target dept information.
*               UCHAR uchMinorClass - minor class for department total.
*               BOOL    fByMjrDept  - boolean value for read by major dept.
*   Output  :   MDEPTIF *pMjrDeptIF - address of retrieved department.
*               TTLDEPT *pTtlDept   - address of total record.
*   InOut   :   Nothing
*
**  Return  :   SHORT   sRetCode    - PROGRPT_SUCCESS
*                                     PROGRPT_END_OF_FILE
*                                   
**  Description:
*    This function retrieves a department parameter which is specified at
*   pMjrDeptIF parameter.
*    It must be 0 at pMjrDeptIF->uchDeptNo parameter at first calling. And
*   then user does not modify at pMjrDeptIF->uchDeptNo parameter at second
*   calling.
*==========================================================================
*/
SHORT ProgRpt_DeptFindDept( MDEPTIF *pMjrDeptIF,
                            TTLDEPT *pTtlDept,
                            UCHAR   uchMinorClass,
                            BOOL    fByMjrDept )
{
    SHORT   sRetCode;   /* return code for user function */
    DEPTIF  DeptIF;     /* structure for i/f to department record */
    SHORT   sReturn;   /* return code for user function */

    ASSERT( pMjrDeptIF != ( MDEPTIF * )NULL );
    ASSERT( pMjrDeptIF->uchMajorDeptNo <= MAX_MDEPT_NO );
    ASSERT( pMjrDeptIF->usDeptNo       <= MAX_DEPT_NO );
    ASSERT( pTtlDept   != ( TTLDEPT * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));
    ASSERT(( fByMjrDept == TRUE ) || ( fByMjrDept == FALSE ));

    if ( fByMjrDept ) {

        /* --- retrieve department record by major dept no. --- */
        /* input: pMjrDeptIF->uchMajorDeptNo : 0...10               */
        /*        pMjrDeptIF->uchDeptNo      : 0(first time)...50   */
        sRetCode = SerOpMajorDeptRead( pMjrDeptIF, 0 );
        switch ( sRetCode ) {
        case OP_PERFORM:    /* pMjrDeptIF->uchDeptNo = 0...49 */
            sRetCode = PROGRPT_SUCCESS;
            break;

        case OP_EOF:        /* pMjrDeptIF->uchDeptNo = 50 */
            sRetCode = PROGRPT_END_OF_FILE;
            break;

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
    } else {
        /* --- retrieve department record by department no. --- */
        /* input : pMjrDeptIF->uchDeptNo : 0...50 */
        DeptIF.usDeptNo = ( UCHAR )( pMjrDeptIF->usDeptNo + 1 );

        sRetCode = CliOpDeptIndRead( &DeptIF, 0 );
        switch ( sRetCode ) {
        case OP_PERFORM:
            pMjrDeptIF->ParaDept = DeptIF.ParaDept;
            pMjrDeptIF->usDeptNo = DeptIF.usDeptNo;
            sRetCode = PROGRPT_SUCCESS;
            break;

        case OP_LOCK:
            sRetCode = PROGRPT_LOCK;
            break;

        case OP_ABNORMAL_DEPT:
            sRetCode = PROGRPT_END_OF_FILE;
            break;

        case OP_FILE_NOT_FOUND:
        default:
            sRetCode = PROGRPT_FILE_NOT_FOUND;
            break;
        }
    }

    if ( sRetCode == PROGRPT_SUCCESS ) {
        /* --- retrieve department total data --- */
        memset(pTtlDept, 0, sizeof(TTLDEPT));

        pTtlDept->uchMajorClass = CLASS_TTLDEPT;
        pTtlDept->uchMinorClass = uchMinorClass;
        pTtlDept->usDEPTNumber  = pMjrDeptIF->usDeptNo;

        sReturn = SerTtlTotalRead(pTtlDept);                            /* Saratoga */
        if ((sReturn != TTL_SUCCESS) && (sReturn != TTL_NOTINFILE)) {
            sRetCode = PROGRPT_FILE_NOT_FOUND;
        } else {
            sRetCode = PROGRPT_SUCCESS;
        }
    }
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_DeptIsLoopTRUE( PROGRPT_LOOPREC *pLoopRec,
*                                            OPDEPT_PARAENTRY *pParaDept )
*
*   Input   :   PROGRPT_LOOPREC *pLoopRec  - address of loop record.
*               DEPTPARA        *pParaDept - address of department parameter.
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
BOOL ProgRpt_DeptIsLoopTRUE( PROGRPT_LOOPREC *pLoopRec,
                             OPDEPT_PARAENTRY *pParaDept )
{
    BOOL    fRetValue;      /* return value for user application */
    UCHAR   uchLoopCond;    /* condition of loop item */

    ASSERT( pLoopRec  != ( PROGRPT_LOOPREC * )NULL );
    ASSERT( pParaDept != ( OPDEPT_PARAENTRY * )NULL );

    if ( pLoopRec->uchCondition == PROGRPT_ALLREAD ) {
        return ( TRUE );
    }

    uchLoopCond = pLoopRec->uchCondition;

    /* Negative Hash Dept, V3.3 */
    if (( pParaDept->auchControlCode[ 2 ] & PLU_HASH ) && ( pParaDept->auchControlCode[ 0 ] & PLU_MINUS )) {
        /* hash affect to gross total */
        if (CliParaMDCCheck(MDC_DEPT1_ADR, ODD_MDC_BIT1)) {
            fRetValue = ( uchLoopCond & PROGRPT_COND_MINUS ) ? TRUE : FALSE;
        } else {
            fRetValue = ( uchLoopCond & PROGRPT_COND_HASH ) ? TRUE : FALSE;
        }
    } else
    if ( pParaDept->auchControlCode[ 2 ] & PLU_HASH ) {
        fRetValue = ( uchLoopCond & PROGRPT_COND_HASH ) ? TRUE : FALSE;
    } else if ( pParaDept->auchControlCode[ 0 ] & PLU_MINUS ) {
        fRetValue = ( uchLoopCond & PROGRPT_COND_MINUS ) ? TRUE : FALSE;
    } else if ( pParaDept->auchControlCode[ 2 ] & PLU_SCALABLE ) {
        fRetValue = ( uchLoopCond & PROGRPT_COND_SCALE ) ? TRUE : FALSE;
    } else {
        fRetValue = ( uchLoopCond & PROGRPT_COND_PLUS ) ? TRUE : FALSE;
    }
    return ( fRetValue );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_DeptIsPrintTRUE( PROGRPT_PRINTREC * pPrintRec,
*                                             TTLDEPT *pTtlDept )
*
*   Input   :   PROGRPT_PRINTREC *pPrintRec - address of print record.
*               TTLDEPT          *pTtlDept  - address of department total.
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
BOOL ProgRpt_DeptIsPrintTRUE( PROGRPT_PRINTREC * pPrintRec,
                              TTLDEPT *pTtlDept )
{
    BOOL        fRetValue;  /* return value for user application */
    D13DIGITS   hugeint;    /* 64 bit value */
    D13DIGITS   D13Value1;  /* 13 digits operand1 value */
    D13DIGITS   D13Value2;  /* 13 digits operand2 value */

    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );

    /* --- determine this item is EOF item --- */

    if (( pPrintRec->PrintItem.uchGroup == ( UCHAR )NULL ) &&
        ( pPrintRec->PrintItem.uchMajor == ( UCHAR )NULL ) &&
        ( pPrintRec->PrintItem.uchMinor == ( UCHAR )NULL ) &&
        ( pPrintRec->PrintItem.uchItem  == ( UCHAR )NULL )) {
        /* --- determine EOF item  --- */
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
        if ( ! ProgRpt_DeptGetInteger( &( pPrintRec->Operand2 ), pTtlDept, &hugeint )) {
            return ( FALSE );
        }
        D13Value2 = hugeint;

        /* --- break;... not used --- */

    case PROGRPT_PRTCOND_NOTZERO:   /* ope1 != 0 */
        if ( ! ProgRpt_DeptGetInteger( &( pPrintRec->Operand1 ), pTtlDept, &hugeint )) {
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
**  Synopsis:   UCHAR ProgRpt_DeptQueryDataType( PROGRPT_ITEM *pTargetItem )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   UCHAR uchType - data type of specified item.
*                                   PROGRPT_TYPE_UCHAR
*                                   PROGRPT_TYPE_LTOTAL
*                                   PROGRPT_TYPE_DATE
*                                   PROGRPT_TYPE_STRING
*
**  Description:
*    This function determines that which data type is the data pointed by
*   pTargetItem. It returns data type, if function successful.
*==========================================================================
*/
UCHAR ProgRpt_DeptQueryDataType( PROGRPT_ITEM *pTargetItem )
{
    UCHAR uchType;  /* data type of target item */

    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );

    uchType = PROGRPT_TYPE_INVALID;

    /* --- what kind of groups ? --- */
    switch ( pTargetItem->uchGroup ) {
    case PROGRPT_GROUP_KEY:         /* loop key group */
        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_DEPTKEY_DEPTNO:
        case PROGRPT_DEPTKEY_MDEPTNO:
            uchType = PROGRPT_TYPE_UCHAR;
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

        case PROGRPT_TTL_DEPTTOTAL:
        case PROGRPT_TTL_ALLTOTAL:
            uchType = PROGRPT_TYPE_LTOTAL;
            break;

        default:
            break;
        }
        break;

    case PROGRPT_GROUP_PARAM:       /* parameter group */
        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_DEPTPARA_MNEMO:
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
**  Synopsis:   BOOL ProgRpt_DeptGetInteger( PROGRPT_ITEM *pTargetItem,
*                                            TTLDEPT      *pTtlDept,
*                                            D13DIGITS    *phugeOutput );
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               TTLDEPT      *pTtlDept    - address of department total.
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
BOOL ProgRpt_DeptGetInteger( PROGRPT_ITEM *pTargetItem,
                             TTLDEPT      *pTtlDept,
                             D13DIGITS    *phugeOutput )
{
    ASSERT( pTargetItem != NULL );
    ASSERT( phugeOutput != NULL );

    switch ( pTargetItem->uchGroup ) {
    case PROGRPT_GROUP_KEY:
        if ( pTtlDept == ( TTLDEPT * )NULL ) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_DEPTKEY_DEPTNO:
            *phugeOutput = pTtlDept->usDEPTNumber;
            break;

        case PROGRPT_DEPTKEY_MDEPTNO:
            *phugeOutput = pTtlDept->uchMajorDEPTNo;
            break;

        default:  
            return ( FALSE );
        }
        break;

    case PROGRPT_GROUP_TOTALCO:
        if ( pTtlDept == ( TTLDEPT * )NULL ) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_TTL_DEPTTOTAL:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlDept->DEPTTotal.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlDept->DEPTTotal.lCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_ALLTOTAL:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlDept->DEPTAllTotal.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlDept->DEPTAllTotal.lCounter;
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
**  Synopsis:   BOOL ProgRpt_DeptGetDateTime( PROGRPT_ITEM *pTargetItem,
*                                             UCHAR     uchMinorClass,
*                                             TTLDEPT   *pTtlDept,
*                                             DATE      *pDate )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               UCHAR   uchMinorClass   - minor class for dept total.
*               TTLDEPT *pTtlDept       - address of department total.
*   Output  :   DATE    *pDateTime      - address of date / time data.
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
BOOL ProgRpt_DeptGetDateTime( PROGRPT_ITEM *pTargetItem,
                              UCHAR     uchMinorClass,
                              TTLDEPT   *pTtlDept,
                              N_DATE      *pDateTime )
{
	TTLDEPT TtlDepartment = { 0 };
    SHORT   sTtlStatus;

    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );
    ASSERT( pDateTime   != ( N_DATE * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));

    if (( uchMinorClass == CLASS_TTLCURPTD ) || ( uchMinorClass == CLASS_TTLCURDAY )) {
        if ( pTargetItem->uchMajor == PROGRPT_TTL_TO ) {
            return ( ProgRpt_GetCurDateTime( pDateTime ));
        }
    }

    if ( pTtlDept == NULL ) {
        /* --- print date/time at out of loop --- */
        TtlDepartment.uchMajorClass = CLASS_TTLDEPT;
        TtlDepartment.uchMinorClass = uchMinorClass;
        TtlDepartment.usDEPTNumber  = 1;

        if (( sTtlStatus = SerTtlTotalRead( &TtlDepartment )) != TTL_SUCCESS ) {
            if (sTtlStatus != TTL_NOTINFILE) {  /* saratoga */
                return ( FALSE );
            }
        }
        pTtlDept = &TtlDepartment;
    }

    switch ( pTargetItem->uchMajor ) {
    case PROGRPT_TTL_FROM:
		*pDateTime = pTtlDept->FromDate;
        break;

    case PROGRPT_TTL_TO:
		*pDateTime = pTtlDept->ToDate;
        break;

    default:
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_DeptGetString( PROGRPT_ITEM *pTargetItem,
*                                           OPDEPT_PARAENTRY *pDeptPara,
*                                           UCHAR        *pszString,
*                                           UCHAR        uchMjrDeptNo )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               DEPTPARA     *pDeptPara   - address of department total.
*               UCHAR        uchMjrDeptNo - major department no.
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
BOOL ProgRpt_DeptGetString( PROGRPT_ITEM     *pTargetItem,
                            OPDEPT_PARAENTRY *pDeptPara,
                            TCHAR            *pszString,
                            UCHAR            uchMjrDeptNo )
{
    UCHAR   uchStrLen;

    ASSERT( pTargetItem  != ( PROGRPT_ITEM * )NULL );
    ASSERT( pszString    != ( UCHAR * )NULL );
    ASSERT( uchMjrDeptNo <= MAX_MDEPT_NO );

    switch ( pTargetItem->uchGroup ) {
    case PROGRPT_GROUP_PARAM:
        if ( pDeptPara == NULL ) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_DEPTPARA_MNEMO:
            memset( pszString, 0x00, PARA_DEPT_LEN + 1 );
            _tcsncpy( pszString, pDeptPara->auchMnemonic, PARA_DEPT_LEN );

            /* --- fill space character, if NULL character found --- */
            uchStrLen = 0;
            while (( uchStrLen < PARA_DEPT_LEN )  && ( pszString[ uchStrLen ] == '\0' )) {
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
        ProgRpt_GetMnemonic( pTargetItem, pszString, uchMjrDeptNo );
        break;

    default:
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_DeptPrintRootPara( PROGRPT_PRINTREC *pPrintRec,
*                                               UCHAR   uchLoopMjrNo )
*
*   Input   :   PROGRPT_PRINTREC *pPrintRec - address of print record.
*               UCHAR   uchLoopMjrNo    - major dept no. of loop key
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    -   item is printed.
*                       FALSE   -   item is not printed.
*                                   
**  Description:
*    This function prints a item which is out of LOOP(DEPT#) operation.
*==========================================================================
*/
BOOL ProgRpt_DeptPrintRootPara( PROGRPT_PRINTREC *pPrintRec,
                                UCHAR   uchLoopMjrNo )
{
    D13DIGITS   D13Digit;       /* value for target print item */
    UCHAR       uchStrLen;      /* length of mnemonic data without NULL */
    TCHAR       szMnemonic[ PROGRPT_MAX_40CHAR_LEN + 1 ];
                                /* mnemonic work area */

    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );
    ASSERT( uchLoopMjrNo <= MAX_MDEPT_NO );

    switch ( pPrintRec->PrintItem.uchGroup ) {
    case PROGRPT_GROUP_KEY:
        switch ( pPrintRec->PrintItem.uchMajor ) {
        case PROGRPT_DEPTKEY_MDEPTNO:
            D13Digit = ( D13DIGITS )uchLoopMjrNo; /* ### MOD (2171 for Win32) V1.0 */
            /* D13Digit.sBillions   = 0;*/

            ProgRpt_IntegerToString( pPrintRec, &D13Digit, szMnemonic );

            uchStrLen = ( UCHAR )_tcslen( szMnemonic );
            ProgRpt_FormatPrintBuff( pPrintRec, szMnemonic, uchStrLen, PROGRPT_TYPE_UCHAR );
            break;

        default:
            return ( FALSE );
        }
        break;

    case 0:
        if (( uchProgRptRow < pPrintRec->uchRow ) && ( auchProgRptBuff[ 0 ] == '\0' )) {
            /* --- print last row item --- */
#ifdef DEBUG
            auchProgRptBuff[ 0 ] = '}';
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

/* ===== End of File (_PROGDPT.C) ===== */
