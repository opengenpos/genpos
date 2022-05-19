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
*   Program Name       : _PROGFIN.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : The provided function names are as follows.
*
*   ProgRpt_FinPrint()          - print current/saved daily/PTD financ. total.
*   ProgRpt_FinReset()          - reset current daily/PTD to saved total file.
*   ProgRpt_FinIsResettable()   - determine current daily/PTD is resettable.
*   ProgRpt_FinIsBadLoopKey()   - determine loop key is valid or invalid.
*
*   ProgRpt_FinPRINT()          - execute print operation in financial ttl.
*   ProgRpt_FinMATH()           - execute mathematics operation in fin. ttl.
*
*   ProgRpt_FinIsPrintTRUE()    - determine print condition is TRUE.
*   ProgRpt_FinQueryDataType()  - get type of specified data.
*   ProgRpt_FinGetInteger()     - get integer from financial total/para.
*   ProgRpt_FinGetDateTime()    - get date data from financial total.
*   ProgRpt_FinGetString()      - get string data from financial total.
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
* Date      : Ver.Rev  : NAME       : Description
* Mar-21-96 : 03.01.00 : T.Nakahata : Initial
* Mar-29-96 : 03.01.02 : T.Nakahata : check TTL_NOTINFILE in IsResettable
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
**  Synopsis:   SHORT ProgRpt_FinPrint( UCHAR uchProgRptNo,
*                                       UCHAR uchMinorClass )
*
*   Input   :   UCHAR uchProgRptNo  - programmable report no.
*               UCHAR uchMinorClass - minor class for department total.
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
*    This function prints specified financial report.
*==========================================================================
*/
SHORT ProgRpt_FinPrint( UCHAR uchProgRptNo,
                        UCHAR uchMinorClass )
{
    SHORT       sRetCode;       /* return code */
    SHORT       sTtlStatus;     /* return code for total module */
    PROGRPT_FHEAD   FileInfo;   /* information about prog report file */
    PROGRPT_RPTHEAD ReportInfo; /* information about report format */
    USHORT  usLoopIndex;        /* loop index for format record */
    USHORT  usMaxNoOfRec;       /* maximum no. of record in report format */
    ULONG   ulReadOffset;       /* read offset for format record */
    TTLREGFIN   TtlFin;         /* structure for total module i/f */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));
    ASSERT(( 0 < uchProgRptNo ) && ( uchProgRptNo <= MAX_PROGRPT_FORMAT ));

    /* --- determine specified report format is correct or incorrect --- */
    if (( sRetCode = ProgRpt_GetProgRptInfo( uchProgRptNo, &ReportInfo )) != PROGRPT_SUCCESS ) {
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    if ( ReportInfo.usFileType != PROGRPT_FILE_FIN ) {
        return ( LDT_PROHBT_ADR );
    }

    if ( ReportInfo.usDevice == PROGRPT_DEVICE_MLD ) {
        if (( uchMinorClass == CLASS_TTLSAVPTD ) || ( uchMinorClass == CLASS_TTLSAVDAY )) {
            return ( LDT_PROHBT_ADR );
        }
    }

    /* --- retrieve register financial total --- */
    memset( &TtlFin, 0x00, sizeof( TTLREGFIN ));
    TtlFin.uchMajorClass = CLASS_TTLREGFIN;
    TtlFin.uchMinorClass = uchMinorClass;
    if (( sTtlStatus = SerTtlTotalRead( &TtlFin )) != TTL_SUCCESS ) {
        return ( LDT_NTINFL_ADR );
    }

    /* --- initialize current row item before print financial report --- */
    memset( auchProgRptBuff, 0x00, sizeof( auchProgRptBuff ));
    uchProgRptColumn = 1;
    uchProgRptRow    = 1;

    /* --- read a record until end of file --- */
    ProgRpt_GetFileHeader( &FileInfo );

    ulReadOffset = FileInfo.aulOffsetReport[ uchProgRptNo - 1 ] + sizeof( PROGRPT_RPTHEAD );

    usLoopIndex  = 0;
    usMaxNoOfRec = ReportInfo.usNoOfLoop  + ReportInfo.usNoOfPrint + ReportInfo.usNoOfMath;

    while ( usLoopIndex < usMaxNoOfRec ) {
		PROGRPT_RECORD   auchRecord;   /* buffer for format record */
		ULONG   ulActualRead;       /* actual length of read record function */

        /* --- retrieve a formatted record --- */
		//11-11-03 JHHJ SR 201 Parameter 4 change from USHORT to ULONG
        ProgRpt_ReadFile( ulReadOffset, &auchRecord, PROGRPT_MAX_REC_LEN, &ulActualRead );
        if ( ulActualRead != PROGRPT_MAX_REC_LEN ) {
            return ( LDT_ERR_ADR );
        }

        /* --- analyze operation code of format record, and store it to current row item buffer --- */
        switch ( auchRecord.Loop.uchOperation) {
        case PROGRPT_OP_PRINT:  /* print operation */
            sRetCode = ProgRpt_FinPRINT( &ReportInfo, &auchRecord.Print, uchMinorClass, &TtlFin );
            break;

        case PROGRPT_OP_MATH:   /* mathematics operation */
            sRetCode = ProgRpt_FinMATH(&auchRecord.MathOpe, &TtlFin );
            break;

        case PROGRPT_OP_LOOP:   /* loop operation */
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

        /* --- declare saved financial total is already reported --- */

        TtlFin.uchMajorClass = CLASS_TTLREGFIN;
        TtlFin.uchMinorClass = uchMinorClass;
        sRetCode = SerTtlIssuedReset( &TtlFin, TTL_NOTRESET );

        if (( sRetCode = TtlConvertError( sRetCode )) != SUCCESS ) {
            /* --- print out error code --- */
            RptPrtError( sRetCode );
        }
    }
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_FinReset( UCHAR uchMinorClass )
*
*   Input   :   UCHAR uchMinorClass - minor class for financial total.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 LDT_KEYOVER_ADR
*                                 LDT_ERR_ADR       ( TTL_FAIL )
*                                 LDT_PROHBT_ADR    ( OP_FILE_NOT_FOUND )
*                                   
**  Description:
*    This function resets current daily/PTD financial total.
*    An application must call ProgRpt_FinIsRessetable() function before
*   calling this function. If the specified total is resettable, it may
*   call this function.
*    This function deletes saved daily/PTD total, and then copies current
*   total to saved total. Financial total is resetted at only EOD/PTD
*   report.
*==========================================================================
*/
SHORT ProgRpt_FinReset( UCHAR uchMinorClass )
{
    TTLREPORT   TtlReport;      /* structure for reset dept total */
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

    /* --- reset dept total at EOD/PTD reset report --- */

    usExecuteFlag = RPT_RESET_REGFIN_BIT;

    /* --- reset specified department total file --- */

    ASSERT( ProgRpt_FinIsResettable( uchMinorClass ));
    sRetCode = SerTtlTotalReset( &TtlReport, usExecuteFlag );

    /* --- convert return code to lead-thru code --- */

    return ( TtlConvertError( sRetCode ));
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_FinIsResettable( UCHAR uchMinorClass )
*
*   Input   :   UCHAR uchMinorClass - minor class for financial total.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - current total is resettable.
*                       FALSE   - current total is not resettable.
*   
**  Description:
*    This function detemines specified current financial total is able to
*   reset or not.
*    If saved total file is not issued, current total is not resettable.
*==========================================================================
*/
BOOL ProgRpt_FinIsResettable( UCHAR uchMinorClass )
{
    TTLREPORT   TtlReport;      /* structure for reset financial total */

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

    /* --- determine specified department saved total file is issued ? --- */

    TtlReport.uchMajorClass = CLASS_TTLREGFIN;
    TtlReport.ulNumber = 0;

    if ( SerTtlIssuedCheck( &TtlReport, TTL_NOTRESET ) == TTL_NOT_ISSUED ) {
        return ( FALSE );
    }
    return ( TRUE );
}


/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_FinPRINT( PROGRPT_RPTHEAD *pReportInfo,
*                                       PROGRPT_PRINTREC *pPrintRec,
*                                       UCHAR       uchMinorClass,
*                                       TTLREGFIN   *pTtlFin );
*
*   Input   :   PROGRPT_RPTHEAD *pReportInfo - address of report header.
*               PROGRPT_LOOPREC *pPrintRec  - address of print record.
*               UCHAR uchMinorClass - minor class for department total.
*               TTLREGFIN *pTtlFin  -   address of reg-financial total 
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
SHORT ProgRpt_FinPRINT( PROGRPT_RPTHEAD *pReportInfo,
                        PROGRPT_PRINTREC *pPrintRec,
                        UCHAR       uchMinorClass,
                        TTLREGFIN *pTtlFin )
{
    D13DIGITS   D13Digit;       /* value for target print item */
    UCHAR   uchDataType;        /* target print item data type */
    N_DATE    Date;               /* date for target print item */
    TCHAR   szMnemonic[ PROGRPT_MAX_40CHAR_LEN + 1 ];
                                /* mnemonic work area */
    UCHAR     uchStrLen;          /* length of mnemonic data without NULL */
    D13DIGITS hugeint;

    ASSERT( pReportInfo != ( PROGRPT_RPTHEAD * )NULL );
    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );

    /* --- determine specified condition is TRUE --- */
    if ( ! ProgRpt_FinIsPrintTRUE( pPrintRec, pTtlFin )) {
        ProgRpt_PrintEOF( pReportInfo, pPrintRec );
        uchProgRptRow    = pPrintRec->uchRow;
        uchProgRptColumn = pPrintRec->uchColumn;
        return ( SUCCESS );
    }

    /* --- print current row item, if next row item is retrieved --- */
    ProgRpt_PrintAndFeed( pReportInfo, pPrintRec );

    /* --- execute print operation with specified format & style --- */
    memset( szMnemonic, 0x00, sizeof( szMnemonic ));

    /* --- retrieve print item --- */
    uchDataType = ProgRpt_FinQueryDataType( &( pPrintRec->PrintItem ));

    switch ( uchDataType ) {

    case PROGRPT_TYPE_D13:
    case PROGRPT_TYPE_SHORT:
    case PROGRPT_TYPE_USHORT:
    case PROGRPT_TYPE_LONG:
    case PROGRPT_TYPE_TOTAL:
    case PROGRPT_TYPE_HUGEINT:
        if ( ! ProgRpt_FinGetInteger( &( pPrintRec->PrintItem ), pTtlFin, &hugeint ) ) {
            return ( SUCCESS );
        }
        D13Digit = hugeint;

        /* --- convert long value to string with specified form --- */
        ProgRpt_IntegerToString( pPrintRec, &D13Digit, szMnemonic );

        /* --- format and set to print buffer --- */
        uchStrLen = ( UCHAR )_tcslen( szMnemonic );
        ProgRpt_FormatPrintBuff( pPrintRec, szMnemonic, uchStrLen, uchDataType );
        break;

    case PROGRPT_TYPE_DATE:
        if ( ProgRpt_FinGetDateTime( &( pPrintRec->PrintItem ), uchMinorClass, pTtlFin, &Date )) {
            /* --- format and set to print buffer --- */
            ProgRpt_SetDateTimeBuff( pPrintRec, &Date );
        }
        break;

    case PROGRPT_TYPE_STRING:
        ProgRpt_FinGetString( &( pPrintRec->PrintItem ), szMnemonic );

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
**  Synopsis:   SHORT ProgRpt_FinMATH( PROGRPT_MATHREC *pMathRec,
*                                      TTLREGFIN *pTtlFin )
*
*   Input   :   PROGRPT_MATHREC *pMathRec   - address of mathematical record.
*               TTLREGFIN *pTtlFin  - address of reg-financial total.
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
SHORT ProgRpt_FinMATH( PROGRPT_MATHOPE *pMathRec,
                       TTLREGFIN *pTtlFin )
{
    D13DIGITS hugeint;        /* 64 bit value for calculation */
    D13DIGITS Operand1;       /* value for operand 1 */
    D13DIGITS Operand2;       /* value for operand 2 */
    PROGRPT_MATHTTL *pMathTtl;  /* address of math record for total */
    PROGRPT_MATHCO  *pMathCo;   /* address of math record for counter */

    ASSERT( pMathRec != ( PROGRPT_MATHOPE * )NULL );

    /* --- retrieve operand 1 value --- */
    switch ( pMathRec->auchFormat[ 0 ] ) {
    case PROGRPT_MATH_ONEVALUE: /* calculate with a value */
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

    case PROGRPT_MATH_OPERAND:  /* calculate with an operand */
        if ( ! ProgRpt_FinGetInteger( &( pMathRec->Operand1 ), pTtlFin, &Operand1 )) {
            return ( SUCCESS );
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
        if ( ! ProgRpt_FinGetInteger( &( pMathRec->Operand2 ), pTtlFin, &Operand2 )) {
            return ( SUCCESS );
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
**  Synopsis:   BOOL ProgRpt_FinIsPrintTRUE( PROGRPT_PRINTREC * pPrintRec,
*                                            TTLREGFIN *pTtlFin )
*
*   Input   :   PROGRPT_PRINTREC *pPrintRec - address of print record.
*               TTLREGFIN *pTtlFin  - address of ind-financial total.
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
BOOL ProgRpt_FinIsPrintTRUE( PROGRPT_PRINTREC *pPrintRec,
                             TTLREGFIN *pTtlFin )
{
    D13DIGITS D13Value1;    /* */
    D13DIGITS D13Value2;    /* */
    D13DIGITS hugeint;
    BOOL    fRetValue;  /* return value for user application */

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
        if ( ! ProgRpt_FinGetInteger( &( pPrintRec->Operand2 ), pTtlFin, &hugeint )) {
            return ( FALSE );
        }
        D13Value2 = hugeint;

        /* --- break;... not used --- */

    case PROGRPT_PRTCOND_NOTZERO:   /* ope1 != 0 */
        if ( ! ProgRpt_FinGetInteger( &( pPrintRec->Operand1 ), pTtlFin, &hugeint )) {
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
**  Synopsis:   UCHAR ProgRpt_FinQueryDataType( PROGRPT_ITEM *pTargetItem )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   UCHAR uchType - data type of specified item.
*                                   PROGRPT_TYPE_STRING
*                                   PROGRPT_TYPE_SHORT
*                                   PROGRPT_TYPE_USHORT
*                                   PROGRPT_TYPE_LONG
*                                   PROGRPT_TYPE_TOTAL
*                                   PROGRPT_TYPE_D13
*                                   PROGRPT_TYPE_HUGEINT
*                                   PROGRPT_TYPE_DATE
*
**  Description:
*    This function determines that which data type is the data pointed by
*   pTargetItem. It returns data type, if function successful.
*==========================================================================
*/
UCHAR ProgRpt_FinQueryDataType( PROGRPT_ITEM *pTargetItem )
{
    UCHAR uchType;  /* data type of target item */

    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );

    uchType = PROGRPT_TYPE_INVALID;

    /* --- what kind of groups ? --- */
    switch ( pTargetItem->uchGroup ) {

    case PROGRPT_GROUP_KEY:         /* loop key group */
        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_INDKEY_TERMNO:
            uchType = PROGRPT_TYPE_USHORT;
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

        case PROGRPT_TTL_CGGTOTAL:
        case PROGRPT_TTL_TAXABLE:
        case PROGRPT_TTL_NONTAXABLE:
            uchType = PROGRPT_TYPE_D13;
            break;

        case PROGRPT_TTL_NETTOTAL:
        case PROGRPT_TTL_TAXTOTAL:
        case PROGRPT_TTL_TAXEXMPT:
        case PROGRPT_TTL_TRAINGGT:
        case PROGRPT_TTL_DGGTTL:
        case PROGRPT_TTL_ITEMPROCO:
        case PROGRPT_TTL_CONSTAX:
        case PROGRPT_TTL_FSEXMPT:   /* saratoga */
            uchType = PROGRPT_TYPE_LONG;
            break;

        case PROGRPT_TTL_NOSALE:
        case PROGRPT_TTL_NOPERSON:
        case PROGRPT_TTL_CHKOPEN:
        case PROGRPT_TTL_CHKCLOSE:
        case PROGRPT_TTL_CUSTOMER:
            uchType = PROGRPT_TYPE_SHORT;
            break;

        case PROGRPT_TTL_DECTIP:
        case PROGRPT_TTL_PLUSVOID:
        case PROGRPT_TTL_PREVOID:
        case PROGRPT_TTL_CONSCPN:
        case PROGRPT_TTL_ITEMDISC:
        case PROGRPT_TTL_MODDISC:
        case PROGRPT_TTL_REGDISC:
        case PROGRPT_TTL_COMBCPN:
        case PROGRPT_TTL_PO:
        case PROGRPT_TTL_ROA:
        case PROGRPT_TTL_TPO:
        case PROGRPT_TTL_TENDER:
        case PROGRPT_TTL_OFFTEND:
        case PROGRPT_TTL_FOREIGN:
        case PROGRPT_TTL_SERVICE:
        case PROGRPT_TTL_ADDCHK:
        case PROGRPT_TTL_CHARGE:
        case PROGRPT_TTL_CANCEL:
        case PROGRPT_TTL_MISC:
        case PROGRPT_TTL_AUDACT:
        case PROGRPT_TTL_HASHDEPT:
        case PROGRPT_TTL_BONUS:
        case PROGRPT_TTL_LOAN:      /* Saratoga */
        case PROGRPT_TTL_PICKUP:    /* Saratoga */
        case PROGRPT_TTL_FSCREDIT:  /* saratoga */
        case PROGRPT_TTL_COUPON:    /* saratoga */
            uchType = PROGRPT_TYPE_TOTAL;
            break;

        default:
            break;
        }
        break;

    case PROGRPT_GROUP_MNEMO:       /* common mnemonic group */
        switch ( pTargetItem->uchMajor ){
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
        switch ( pTargetItem->uchMajor ){
        case PROGRPT_ACCUM_TOTAL:
        case PROGRPT_ACCUM_COUNTER:
            uchType = PROGRPT_TYPE_HUGEINT;
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
**  Synopsis:   BOOL ProgRpt_FinGetInteger( PROGRPT_ITEM *pTargetItem,
*                                           TTLREGFIN    *pTtlFin,
*                                           D13DIGITS    *phugeOutput )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               TTLREGFIN    *pTtlFin     - address of ind-financial total.
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
BOOL ProgRpt_FinGetInteger( PROGRPT_ITEM *pTargetItem,
                            TTLREGFIN   *pTtlFin,
                            D13DIGITS     *phugeOutput )
{
    UCHAR   uchMinorCode;
    UCHAR   uchItemCode;
    TOTAL   *pTotal;
    D13DIGITS   *pD13Value;

    ASSERT( pTargetItem != NULL );
    ASSERT( phugeOutput != NULL );

    if ( PROGRPT_TTLITEM_CO < pTargetItem->uchItem ) {
        return ( FALSE );
    }

    uchMinorCode = pTargetItem->uchMinor;
    uchItemCode  = pTargetItem->uchItem;

    switch ( pTargetItem->uchGroup ) {
    case PROGRPT_GROUP_KEY:
        if ( pTtlFin == ( TTLREGFIN * )NULL ) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_INDKEY_TERMNO:
            *phugeOutput = pTtlFin->usTerminalNumber;
            break;

        default:  
            return ( FALSE );
        }
        break;

    case PROGRPT_GROUP_TOTALCO:
        if ( pTtlFin == ( TTLREGFIN * )NULL ) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_TTL_CGGTOTAL:
            pD13Value = &( pTtlFin->CGGTotal );
            *phugeOutput = *pD13Value; /* ### MOD (2171 for Win32) V1.0 */
            break;

        case PROGRPT_TTL_TAXABLE:
            if (( uchMinorCode < 1 ) || ( 4 < uchMinorCode )) {
                return ( FALSE );
            }
            pD13Value = &( pTtlFin->Taxable[ uchMinorCode - 1 ].mlTaxableAmount );
            *phugeOutput = *pD13Value; /* ### MOD (2171 for Win32) V1.0 */
            break;

        case PROGRPT_TTL_NONTAXABLE:
            pD13Value = &( pTtlFin->NonTaxable );
            *phugeOutput = *pD13Value;  /* ### MOD (2171 for Win32) V1.0 */
            break;

        case PROGRPT_TTL_NETTOTAL:
            *phugeOutput = pTtlFin->lNetTotal;
            break;

        case PROGRPT_TTL_TAXTOTAL:
            if (( uchMinorCode < 1 ) || ( 4 < uchMinorCode )) {
                return ( FALSE );
            }
            *phugeOutput = pTtlFin->Taxable[ uchMinorCode - 1 ].lTaxAmount;
            break;

        case PROGRPT_TTL_TAXEXMPT:
            if (( uchMinorCode < 1 ) || ( 4 < uchMinorCode )) {
                return ( FALSE );
            }
            *phugeOutput = pTtlFin->Taxable[ uchMinorCode - 1 ].lTaxExempt;
            break;

        case PROGRPT_TTL_FSEXMPT:   /* saratoga */
            if (( uchMinorCode < 1 ) || ( 3 < uchMinorCode ))
                return ( FALSE );
            *phugeOutput = pTtlFin->Taxable[ uchMinorCode - 1 ].lFSTaxExempt;
            break;

        case PROGRPT_TTL_TRAINGGT:
            *phugeOutput = pTtlFin->lTGGTotal;
            break;

        case PROGRPT_TTL_DGGTTL:
            *phugeOutput = pTtlFin->lDGGtotal;
            break;

        case PROGRPT_TTL_DECTIP:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->DeclaredTips.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->DeclaredTips.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_PLUSVOID:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->PlusVoid.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->PlusVoid.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_PREVOID:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->PreselectVoid.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->PreselectVoid.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_CONSCPN:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->ConsCoupon.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->ConsCoupon.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_ITEMDISC:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->ItemDisc.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->ItemDisc.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_MODDISC:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->ModiDisc.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->ModiDisc.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_REGDISC:
            if (( uchMinorCode < 1 ) || ( 6 < uchMinorCode )){
                return ( FALSE );
            }
            
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->RegDisc[ uchMinorCode - 1 ].lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->RegDisc[ uchMinorCode - 1 ].sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_COMBCPN:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->Coupon.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->Coupon.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_PO:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->PaidOut.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->PaidOut.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_ROA:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->RecvAcount.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->RecvAcount.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_TPO:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->TipsPaid.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->TipsPaid.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_TENDER:
            switch ( uchMinorCode ) {
            case 1:
                pTotal = &( pTtlFin->CashTender.OnlineTotal );
                switch ( uchItemCode ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTotal->lAmount;
                    break;
                case PROGRPT_TTLITEM_CO:
                default:
                    *phugeOutput = pTotal->sCounter;
                    break;
                }
                break;

            case 2:
                pTotal = &( pTtlFin->CheckTender.OnlineTotal );
                switch ( uchItemCode ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTotal->lAmount;
                    break;
                case PROGRPT_TTLITEM_CO:
                default:
                    *phugeOutput = pTotal->sCounter;
                    break;
                }
                break;

            case 3:
                pTotal = &( pTtlFin->ChargeTender.OnlineTotal );
                switch ( uchItemCode ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTotal->lAmount;
                    break;
                case PROGRPT_TTLITEM_CO:
                default:
                    *phugeOutput = pTotal->sCounter;
                    break;
                }
                break;

            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                uchMinorCode -= 4;
                pTotal = &( pTtlFin->MiscTender[ uchMinorCode ].OnlineTotal );
                switch ( uchItemCode ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTotal->lAmount;
                    break;
                case PROGRPT_TTLITEM_CO:
                default:
                    *phugeOutput = pTotal->sCounter;
                    break;
                }
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_OFFTEND:
            switch ( uchMinorCode ) {
            case 1:
                pTotal = &( pTtlFin->CashTender.OfflineTotal );
                switch ( uchItemCode ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTotal->lAmount;
                    break;
                case PROGRPT_TTLITEM_CO:
                default:
                    *phugeOutput = pTotal->sCounter;
                    break;
                }
                break;

            case 2:
                pTotal = &( pTtlFin->CheckTender.OfflineTotal );
                switch ( uchItemCode ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTotal->lAmount;
                    break;
                case PROGRPT_TTLITEM_CO:
                default:
                    *phugeOutput = pTotal->sCounter;
                    break;
                }
                break;

            case 3:
                pTotal = &( pTtlFin->ChargeTender.OfflineTotal );
                switch ( uchItemCode ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTotal->lAmount;
                    break;
                case PROGRPT_TTLITEM_CO:
                default:
                    *phugeOutput = pTotal->sCounter;
                    break;
                }
                break;

            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                uchMinorCode -= 4;
                pTotal = &( pTtlFin->MiscTender[ uchMinorCode ].OfflineTotal );
                switch ( uchItemCode ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTotal->lAmount;
                    break;
                case PROGRPT_TTLITEM_CO:
                default:
                    *phugeOutput = pTotal->sCounter;
                    break;
                }
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_FOREIGN:

            if (( uchMinorCode < 1 ) || ( 8 < uchMinorCode )){  /* Saratoga */
                return ( FALSE );
            }

            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->ForeignTotal[ uchMinorCode - 1 ].lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->ForeignTotal[ uchMinorCode - 1 ].sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_SERVICE:
            if (( uchMinorCode < 3 ) || ( 8 < uchMinorCode )) {
                return ( FALSE );
            }
            uchMinorCode -= 3;

            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->ServiceTotal[ uchMinorCode ].lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->ServiceTotal[ uchMinorCode ].sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_ADDCHK:
            if (( uchMinorCode < 1 ) || ( 3 < uchMinorCode )){
                return ( FALSE );
            }

            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->AddCheckTotal[ uchMinorCode - 1 ].lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->AddCheckTotal[ uchMinorCode - 1 ].sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_CHARGE:
            /* V3.3 */
            if ( 3 < uchMinorCode ){
                return ( FALSE );
            }
            if (uchMinorCode) uchMinorCode--;

            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->ChargeTips[uchMinorCode].lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->ChargeTips[uchMinorCode].sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_CANCEL:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->TransCancel.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->TransCancel.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_MISC:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->MiscVoid.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->MiscVoid.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_AUDACT:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->Audaction.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->Audaction.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_NOSALE:
            *phugeOutput = pTtlFin->sNoSaleCount;
            break;

        case PROGRPT_TTL_ITEMPROCO:
            *phugeOutput = pTtlFin->lItemProductivityCount;
            break;

        case PROGRPT_TTL_NOPERSON:
            *phugeOutput = pTtlFin->sNoOfPerson;
            break;

        case PROGRPT_TTL_CHKOPEN:
            *phugeOutput = pTtlFin->sNoOfChkOpen;
            break;

        case PROGRPT_TTL_CHKCLOSE:
            *phugeOutput = pTtlFin->sNoOfChkClose;
            break;

        case PROGRPT_TTL_CUSTOMER:
            *phugeOutput = pTtlFin->sCustomerCount;
            break;

        case PROGRPT_TTL_HASHDEPT:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->HashDepartment.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->HashDepartment.sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_BONUS:
            if (( uchMinorCode < 1 ) || ( 8 < uchMinorCode )){
                return ( FALSE );
            }

            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->Bonus[ uchMinorCode - 1 ].lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->Bonus[ uchMinorCode - 1 ].sCounter;
                break;
            }
            break;

        case PROGRPT_TTL_CONSTAX:
            *phugeOutput = pTtlFin->lConsTax;
            break;

        case PROGRPT_TTL_LOAN:                      /* Saratoga */
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->Loan.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlFin->Loan.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_PICKUP:                    /* Saratoga */
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->Pickup.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlFin->Pickup.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_FSCREDIT:
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->FoodStampCredit.lAmount;
                break;
            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->FoodStampCredit.sCounter;
                break;
            }
            break;

         case PROGRPT_TTL_COUPON:   /* saratoga */
            if (( uchMinorCode < 1 ) || ( 3 < uchMinorCode ))
                return ( FALSE );
            switch ( uchItemCode ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlFin->aUPCCoupon[uchMinorCode - 1].lAmount;
                break;
            case PROGRPT_TTLITEM_CO:
            default:
                *phugeOutput = pTtlFin->aUPCCoupon[uchMinorCode - 1].sCounter;
                break;
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
**  Synopsis:   BOOL ProgRpt_FinGetDateTime( PROGRPT_ITEM *pTargetItem,
*                                            UCHAR      uchMinorClass,
*                                            TTLREGFIN  *pTtlFin,
*                                            DATE       *pDateTime )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               UCHAR uchMinorClass - minor class for department total.
*               TTLREGFIN    *pTtlFin     - address of ind-financial total.
*   Output  :   DATE         *pDateTime   - address of date/time data.
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    -   string data retrieval is success
*                       FALSE   -   string data retrieval is failed
*                                   
**  Description:
*    This function retrieves a date data which specified at pTargetItem
*   parameter.
*==========================================================================
*/
BOOL ProgRpt_FinGetDateTime( PROGRPT_ITEM *pTargetItem,
                             UCHAR      uchMinorClass,
                             TTLREGFIN  *pTtlFin,
                             N_DATE       *pDateTime )
{
    TTLREGFIN   TtlRegFin;
    SHORT       sTtlStatus;

    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );
    ASSERT( pDateTime != ( N_DATE * )NULL );
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

    if ( pTtlFin == ( TTLREGFIN * )NULL ) {
        /* --- print date/time at out of loop --- */
        TtlRegFin.uchMajorClass = CLASS_TTLINDFIN;
        TtlRegFin.uchMinorClass = uchMinorClass;
        TtlRegFin.usTerminalNumber  = 1;

        if (( sTtlStatus = SerTtlTotalRead( &TtlRegFin )) != TTL_SUCCESS ) {
            return ( FALSE );
        }
        pTtlFin = &TtlRegFin;
    }

    switch ( pTargetItem->uchMajor ) {

    case PROGRPT_TTL_FROM:
        pDateTime->usMin   = pTtlFin->FromDate.usMin;
        pDateTime->usHour  = pTtlFin->FromDate.usHour;
        pDateTime->usMDay  = pTtlFin->FromDate.usMDay;
        pDateTime->usMonth = pTtlFin->FromDate.usMonth;
        break;

    case PROGRPT_TTL_TO:
        pDateTime->usMin   = pTtlFin->ToDate.usMin;
        pDateTime->usHour  = pTtlFin->ToDate.usHour;
        pDateTime->usMDay  = pTtlFin->ToDate.usMDay;
        pDateTime->usMonth = pTtlFin->ToDate.usMonth;
        break;

    default:
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_FinGetString( PROGRPT_ITEM *pTargetItem,
*                                          UCHAR        *pszString )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
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
BOOL ProgRpt_FinGetString( PROGRPT_ITEM *pTargetItem,
                           TCHAR        *pszString )
{
    UCHAR   uchDummyKeyNo;

    ASSERT( pszString != ( UCHAR * )NULL );
    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );

    switch ( pTargetItem->uchGroup ) {

    case PROGRPT_GROUP_MNEMO:
        uchDummyKeyNo = 1;
        ProgRpt_GetMnemonic( pTargetItem, pszString, uchDummyKeyNo );
        break;

    default:
        return ( FALSE );
    }
    return ( TRUE );
}

/* ===== End of File (_PROGFIN.C) ===== */
