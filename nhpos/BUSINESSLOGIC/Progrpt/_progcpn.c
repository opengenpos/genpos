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
*   Program Name       : _PROGCPN.C
*  ------------------------------------------------------------------------
*   Abstract           : The provided function names are as follows.
*
*   ProgRpt_CpnPrint()          - print current/saved daily/PTD coupon total.
*   ProgRpt_CpnReset()          - reset current daily/PTD to saved total file.
*   ProgRpt_CpnIsResettable()   - determine current daily/PTD is resettable.
*   ProgRpt_CpnIsBadLoopKey()   - determine loop key is valid or invalid.
*
*   ProgRpt_CpnLOOP()           - execute loop operation in coupon ttl.
*   ProgRpt_CpnPRINT()          - execute print operation in coupon ttl.
*   ProgRpt_CpnMATH()           - execute mathematics operation in coupon ttl.
*
*   ProgRpt_CpnFindCoupon()     - find coupon paramater & total.
*   ProgRpt_CpnIsPrintTRUE()    - determine print condition is TRUE.
*   ProgRpt_CpnQueryDataType()  - get type of specified data.
*   ProgRpt_CpnGetInteger()     - get integer from coupon total/parameter.
*   ProgRpt_CpnGetDateTime()    - get date/time data from coupon total.
*   ProgRpt_CpnGetString()      - get string data from coupon total.
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
*    Date      Ver.Rev :NAME       :Description
*    Mar-13-96:03.01.00:T.Nakahata :Initial
*    Mar-29-96:03.01.02:T.Nakahata :check TTL_NOTINFILE in IsRessetable
*    Dec-13-99:01.00.00:hrkato     :Saratoga
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
**  Synopsis:   SHORT ProgRpt_CpnPrint( UCHAR uchProgRptNo,
*                                       UCHAR uchMinorClass,
*                                       UCHAR uchCouponNo )
*
*   Input   :   UCHAR uchProgRptNo  - programmable report no.
*               UCHAR uchMinorClass - minor class for department total.
*               UCHAR uchCouponNo   - target coupon no.
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
*    This function prints specified coupon report.
*==========================================================================
*/
SHORT ProgRpt_CpnPrint( UCHAR  uchProgRptNo,
                        UCHAR  uchMinorClass,
                        USHORT usCouponNo )
{
    SHORT   sRetCode;           /* return code */
    PROGRPT_FHEAD   FileInfo;   /* information about prog report file */
    PROGRPT_RPTHEAD ReportInfo; /* information about report format */
    USHORT  usMaxNoOfRec;       /* maximum no. of record in report format */
    USHORT  usLoopIndex;        /* loop index for format record */
    ULONG   ulReadOffset;       /* read offset for format record */
    TTLCPN  TtlCoupon;          /* structure for total module i/f */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));
    ASSERT( ! ProgRpt_CpnIsBadLoopKey( &sRetCode, uchProgRptNo, usCouponNo ));

    /* --- determine specified report format is correct or incorrect --- */

    if (( sRetCode = ProgRpt_GetProgRptInfo( uchProgRptNo, &ReportInfo ))
        != PROGRPT_SUCCESS ) {
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    if ( ReportInfo.usFileType != PROGRPT_FILE_CPN ) {
        return ( LDT_PROHBT_ADR );
    }

    if ( ReportInfo.usDevice == PROGRPT_DEVICE_MLD ) {
        if (( uchMinorClass == CLASS_TTLSAVPTD ) ||
            ( uchMinorClass == CLASS_TTLSAVDAY )) {
            return ( LDT_PROHBT_ADR );
        }
    }

    /* --- initialize current row item before print coupon report --- */
    memset( auchProgRptBuff, 0x00, sizeof( auchProgRptBuff ));
    uchProgRptColumn = 1;
    uchProgRptRow    = 1;

    /* --- read a record until end of file --- */

    ProgRpt_GetFileHeader( &FileInfo );

    ulReadOffset = FileInfo.aulOffsetReport[ uchProgRptNo - 1 ] +
                    sizeof( PROGRPT_RPTHEAD );

    usLoopIndex  = 0;
    usMaxNoOfRec = ReportInfo.usNoOfLoop  +
                   ReportInfo.usNoOfPrint +
                   ReportInfo.usNoOfMath;

    while ( usLoopIndex < usMaxNoOfRec ) {
		PROGRPT_RECORD   auchRecord;   /* buffer for format record */
		ULONG   ulActualRead;       /* actual length of read record function */

        /* --- retrieve a formatted record --- */
		//11-11-03 JHHJ SR 201 Parameter 4 change from USHORT to ULONG
        ProgRpt_ReadFile( ulReadOffset, &auchRecord, PROGRPT_MAX_REC_LEN, &ulActualRead );
        if ( ulActualRead != PROGRPT_MAX_REC_LEN ) {
            return ( LDT_ERR_ADR );
        }

        /* --- analyze operation code of format record, and
            store it to current row item buffer --- */

        switch ( auchRecord.Loop.uchOperation ) {

        case PROGRPT_OP_PRINT:  /* print operation */
            sRetCode = ProgRpt_CpnPRINT( &ReportInfo, &auchRecord.Print, uchMinorClass, NULL, NULL );
            break;

        case PROGRPT_OP_MATH:   /* mathematics operation */
            sRetCode = ProgRpt_CpnMATH(&auchRecord.MathOpe, NULL, NULL );
            break;

        case PROGRPT_OP_LOOP:   /* loop (begin) operation */
            ProgRpt_PrintLoopBegin( &ReportInfo, &auchRecord.Loop);

            switch (auchRecord.Loop.LoopItem.uchMajor ) {
            case PROGRPT_CPNKEY_CPNNO:
                sRetCode = ProgRpt_CpnLOOP( &ReportInfo, &auchRecord.Loop, uchMinorClass, usCouponNo, ulReadOffset );
                usLoopIndex  += auchRecord.Loop.usLoopEnd;
                ulReadOffset += PROGRPT_MAX_REC_LEN * auchRecord.Loop.usLoopEnd;
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

        /* --- declare saved coupon total is already reported --- */

        TtlCoupon.uchMajorClass = CLASS_TTLCPN;
        TtlCoupon.uchMinorClass = uchMinorClass;
        sRetCode = SerTtlIssuedReset( &TtlCoupon, TTL_NOTRESET );

        if (( sRetCode = TtlConvertError( sRetCode )) != SUCCESS ) {
            /* --- print out error code --- */
            RptPrtError( sRetCode );
        }
    }
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_CpnReset( UCHAR uchMinorClass )
*
*   Input   :   UCHAR uchMinorClass - minor class for coupon total.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 LDT_KEYOVER_ADR
*                                 LDT_ERR_ADR       ( TTL_FAIL )
*                                 LDT_PROHBT_ADR    ( OP_FILE_NOT_FOUND )
*                                   
**  Description:
*    This function prints specified coupon report.
*==========================================================================
*/
SHORT ProgRpt_CpnReset( UCHAR uchMinorClass )
{
    TTLREPORT   TtlReport;      /* structure for reset plu total */
    SHORT       sRetCode;       /* return value for user application */
    USHORT      usExecuteFlag;  /* execute flag for EOD/PTD report */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ));

    ASSERT( RptEODChkFileExist( FLEX_CPN_ADR ) == RPT_FILE_EXIST );

    memset( &TtlReport, 0x00, sizeof( TtlReport ));
    TtlReport.uchMajorClass = CLASS_TTLCPN;
    TtlReport.uchMinorClass = uchMinorClass;

    /* --- this function ignores following parameter --- */

    usExecuteFlag = 0;

    /* --- reset specified plu total file --- */

    ASSERT( ProgRpt_CpnIsResettable( uchMinorClass ));
    sRetCode = SerTtlTotalReset( &TtlReport, usExecuteFlag );

    /* --- convert return code to lead-thru code --- */

    return ( TtlConvertError( sRetCode ));
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_CpnIsResettable( UCHAR uchMinorClass )
*
*   Input   :   UCHAR uchMinorClass - minor class for coupon total.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - current total is resettable.
*                       FALSE   - current total is not resettable.
*   
**  Description:
*    This function detemines specified current coupon total is able to reset
*   or not.
*    If saved total file is not issued, current total is not resettable.
*==========================================================================
*/
BOOL ProgRpt_CpnIsResettable( UCHAR uchMinorClass )
{
    TTLREPORT   TtlReport;      /* structure for reset coupon total */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ));

    if ( RptEODChkFileExist( FLEX_CPN_ADR ) != RPT_FILE_EXIST ) {
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

    TtlReport.uchMajorClass = CLASS_TTLCPN;
    TtlReport.ulNumber = 0;

    if ( SerTtlIssuedCheck( &TtlReport, TTL_NOTRESET ) == TTL_NOT_ISSUED ) {
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_CpnIsBadLoopKey( SHORT *psLTDCode,
*                                             UCHAR uchProgRptNo,
*                                             UCHAR uchCouponNo )
*
*   Input   :   SHORT *psLDTCode    - address of lead-thru code.
*               UCHAR uchProgRptNo  - programmable report no.
*               UCHAR uchCouponNo   - target coupon no.
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
BOOL ProgRpt_CpnIsBadLoopKey( SHORT  *psLDTCode,
                              UCHAR  uchProgRptNo,
                              USHORT usCouponNo )
{
    PARAFLEXMEM FlexMem;    /* structure for flexible memory */

    ASSERT( psLDTCode != ( SHORT * )NULL );

    *psLDTCode = ( LDT_KEYOVER_ADR );

    /* --- determine specified prog. report no. is valid or invalid --- */

    if (( uchProgRptNo < 1 ) || ( MAX_PROGRPT_FORMAT < uchProgRptNo )) {
        return ( TRUE );
    }

    /* --- determine specified coupon no. is valid or invalid --- */

    FlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
    FlexMem.uchAddress    = FLEX_CPN_ADR;
    CliParaRead( &FlexMem );
    if ( FlexMem.ulRecordNumber < usCouponNo ) {
        return ( TRUE );
    }

    *psLDTCode = SUCCESS;
    return ( FALSE );
}


/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_CpnLOOP( PROGRPT_RPTHEAD *pReportInfo,
*                                      PROGRPT_LOOPREC *pLoopRec,
*                                      UCHAR  uchMinorClass,
*                                      UCHAR  uchCouponNo,
*                                      ULONG  ulBeginOffset )
*
*   Input   :   PROGRPT_RPTHEAD *pReportInfo - address of report header.
*               PROGRPT_LOOPREC *pLoopRec    - address of print record.
*               UCHAR   uchMinorClass   - minor class for coupon total.
*               UCHAR   uchCouponNo     - target coupon no.
*               ULONG   ulBeginOffset   - offset for loop begin item
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
SHORT ProgRpt_CpnLOOP( PROGRPT_RPTHEAD * pReportInfo,
                       PROGRPT_LOOPREC * pLoopRec,
                       UCHAR    uchMinorClass,
                       USHORT   usCouponNo,
                       ULONG    ulBeginOffset )
{
    CPNIF       CpnIF;          /* parameter of coupon record */
    TTLCPN      TtlCpn;         /* total data of retrieved coupon */
    ULONG       ulReadOffset;   /* offset for read programmed report */
    USHORT      usLoopIndex;    /* loop index */
    USHORT      usMaxNoOfCpn;   /* maximum number of coupons */
    BOOL        fLoopEnd;       /* boolean value for loop is end */
    SHORT       sRetCode;       /* return code of this function */
    UCHAR       uchLoopBeginRow;    /* row of "loop begin" item */
    UCHAR       uchLoopBeginColumn; /* column of "loop begin" item */
    PARAFLEXMEM FlexMem;            /* structure for flexible memory */

    ASSERT( pReportInfo != ( PROGRPT_RPTHEAD * )NULL );
    ASSERT( pLoopRec    != ( PROGRPT_LOOPREC * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));

    FlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
    FlexMem.uchAddress    = FLEX_CPN_ADR;
    CliParaRead( &FlexMem );
    usMaxNoOfCpn = ( USHORT )FlexMem.ulRecordNumber;
    ASSERT( usCouponNo <= usMaxNoOfCpn );

    /* --- define maximum loop count & target item no. --- */
    switch ( pLoopRec->LoopItem.uchMajor ) {
    case PROGRPT_CPNKEY_CPNNO:      /* read coupon by coupon no. */
        if ( usCouponNo == PROGRPT_ALLREAD ) {
            CpnIF.uchCpnNo = 0;
        } else {
            CpnIF.uchCpnNo = usCouponNo - 1;
            usMaxNoOfCpn  = 1;
        }
        break;

    default:
        return ( LDT_ERR_ADR );
    }

    uchLoopBeginRow    = uchProgRptRow;
    uchLoopBeginColumn = uchProgRptColumn;
    usLoopIndex = 0;
    sRetCode = PROGRPT_SUCCESS;

    while (( usLoopIndex < usMaxNoOfCpn ) && ( sRetCode == PROGRPT_SUCCESS )) {
        CpnIF.uchCpnNo++;

        /* --- retrieve coupon record. target coupon no. is automatically
            increment in following function --- */
        sRetCode = ProgRpt_CpnFindCoupon( &CpnIF, &TtlCpn, uchMinorClass );

        if ( sRetCode == PROGRPT_SUCCESS ) {

            /* --- restore print position to loop begin --- */
            fProgRptEmptyLoop = TRUE;
            uchProgRptRow    = uchLoopBeginRow; 
            uchProgRptColumn = uchLoopBeginColumn;

            /* --- read a record until end of loop --- */
            fLoopEnd = FALSE;
            ulReadOffset = ulBeginOffset + ( ULONG )PROGRPT_MAX_REC_LEN;

            while ( ! fLoopEnd ) {
				ULONG       ulActualRead;   /* actual length of read record function */
				PROGRPT_RECORD   auchRecord;    /* buffer for format record */

				//11-11-03 JHHJ SR 201 Parameter 4 change from USHORT to ULONG
                ProgRpt_ReadFile( ulReadOffset, &auchRecord, PROGRPT_MAX_REC_LEN, &ulActualRead );
                if ( ulActualRead != PROGRPT_MAX_REC_LEN ) {
                    return ( LDT_ERR_ADR );
                }

                /* --- analyze operation code of format record, and store it to current row item buffer --- */
                switch ( auchRecord.Loop.uchOperation ) {

                case PROGRPT_OP_PRINT:
                    ProgRpt_CpnPRINT( pReportInfo, &auchRecord.Print, uchMinorClass, &CpnIF, &TtlCpn );
                    break;

                case PROGRPT_OP_MATH:
                    ProgRpt_CpnMATH(&auchRecord.MathOpe, &CpnIF, &TtlCpn );
                    break;

                case PROGRPT_OP_LOOP:
                    if ((auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_END_ROOT ) ||
                        (auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_END_NEST )) {

                        /* --- print current row item, if next row item is retrieved --- */
                        ProgRpt_PrintAndFeed( pReportInfo, &auchRecord );
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

        usLoopIndex++;

    }   /* loop until all requested department record is read */

    fProgRptEmptyLoop = FALSE;
    return ( ProgRpt_ConvertError( sRetCode ));
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_CpnPRINT( PROGRPT_RPTHEAD *pReportInfo,
*                                       PROGRPT_PRINTREC *pPrintRec,
*                                       UCHAR  uchMinorClass,
*                                       CPNIF  *pCpnIF,
*                                       TTLCPN *pTtlCpn );
*
*   Input   :   PROGRPT_RPTHEAD *pReportInfo - address of report header.
*               PROGRPT_LOOPREC *pPrintRec  - address of print record.
*               UCHAR   uchMinorClass   - minor class for coupon total.
*               CPNIF   *pCpnIF     -   address of coupon parameter.
*               TTLCPN  *pTtlCpn    -   address of coupon total 
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
SHORT ProgRpt_CpnPRINT( PROGRPT_RPTHEAD *pReportInfo,
                        PROGRPT_PRINTREC *pPrintRec,
                        UCHAR  uchMinorClass,
                        CPNIF  *pCpnIF,
                        TTLCPN *pTtlCpn )
{
    UCHAR   uchDataType;        /* target print item data type */
    N_DATE    Date;               /* date for target print item */
    TCHAR   szMnemonic[ PROGRPT_MAX_40CHAR_LEN + 1 ];
                                /* mnemonic work area */
    UCHAR   uchStrLen;          /* length of mnemonic data without NULL */
    D13DIGITS   hugeint;
    D13DIGITS   D13Digit;       /* value for target print item */

    ASSERT( pReportInfo != ( PROGRPT_RPTHEAD * )NULL );
    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));


    /* --- determine specified condition is TRUE --- */

    if ( ! ProgRpt_CpnIsPrintTRUE( pPrintRec, pCpnIF, pTtlCpn )) {
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

    uchDataType = ProgRpt_CpnQueryDataType( &( pPrintRec->PrintItem ));

    switch ( uchDataType ) {

    case PROGRPT_TYPE_UCHAR:
    case PROGRPT_TYPE_ULONG:
    case PROGRPT_TYPE_TOTAL:
    case PROGRPT_TYPE_LTOTAL:

        if ( ! ProgRpt_CpnGetInteger( &( pPrintRec->PrintItem ), pCpnIF, pTtlCpn, &hugeint ) ) {
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
        if ( ProgRpt_CpnGetDateTime( &( pPrintRec->PrintItem ), uchMinorClass, pTtlCpn, &Date )) {
            /* --- format and set to print buffer --- */
            ProgRpt_SetDateTimeBuff( pPrintRec, &Date );
        }
        break;

    case PROGRPT_TYPE_STRING:
        ProgRpt_CpnGetString( &( pPrintRec->PrintItem ), pCpnIF, szMnemonic );

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
**  Synopsis:   SHORT ProgRpt_CpnMATH( PROGRPT_MATHREC *pMathRec,
*                                      CPNIF  *pCpnIF,
*                                      TTLCPN *pTtlCpn )
*
*   Input   :   PROGRPT_MATHREC *pMathRec   - address of mathematical record.
*               CPNIF   *pCpnIF     -   address of coupon parameter.
*               TTLCPN  *pTtlCpn    -   address of coupon total.
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
SHORT ProgRpt_CpnMATH( PROGRPT_MATHOPE *pMathRec,
                       CPNIF  *pCpnIF,
                       TTLCPN *pTtlCpn )
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
        if ( ! ProgRpt_CpnGetInteger( &( pMathRec->Operand1 ), pCpnIF, pTtlCpn, &Operand1 )) {
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
        if ( ! ProgRpt_CpnGetInteger( &( pMathRec->Operand2 ), pCpnIF, pTtlCpn, &Operand2 )) {
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
**  Synopsis:   SHORT ProgRpt_CpnFindCoupon( CPNIF  *pCpnIF,
*                                            TTLCPN *pTtlCpn,
*                                            UCHAR  uchMinorClass )
*
*   Input   :   CPNIF  *pCpnIF       - address of coupon parameter.
*               UCHAR  uchMinorClass - minor class for coupon total.
*   Output  :   CPNIF  *pCpnIF       - address of coupon parameter.
*               TTLCPN *pTtlCpn      - address of coupon total.
*   InOut   :   Nothing
*
**  Return  :   SHORT   sRetCode    - PROGRPT_SUCCESS
*                                     PROGRPT_END_OF_FILE
*                                   
**  Description:
*    This function retrieves a coupon parameter which is specified at
*   pCpnIF parameter.
*==========================================================================
*/
SHORT ProgRpt_CpnFindCoupon( CPNIF  *pCpnIF,
                             TTLCPN *pTtlCpn,
                             UCHAR  uchMinorClass )
{
    SHORT   sRetCode;
    SHORT   sTtlStatus;

    ASSERT( pCpnIF  != ( CPNIF * )NULL );
    ASSERT( pTtlCpn != ( TTLCPN * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ));

    sRetCode = CliOpCpnIndRead( pCpnIF, 0 );

    switch ( sRetCode ) {

    case CAS_PERFORM:
        pTtlCpn->uchMajorClass = CLASS_TTLCPN;
        pTtlCpn->uchMinorClass = uchMinorClass;
		NHPOS_ASSERT(pCpnIF->uchCpnNo <= 0xFF);//MAXBYTE
        pTtlCpn->usCpnNumber  = pCpnIF->uchCpnNo;

        sTtlStatus = SerTtlTotalRead( pTtlCpn );
        switch ( sTtlStatus ) {
        case TTL_SUCCESS:
            sRetCode = PROGRPT_SUCCESS;
            break;
        default:
            sRetCode = PROGRPT_FILE_NOT_FOUND;
            break;
        }
        break;

    case CAS_LOCK:
        sRetCode = PROGRPT_LOCK;
        break;

    case OP_FILE_NOT_FOUND:
    default:
        sRetCode = PROGRPT_FILE_NOT_FOUND;
        break;
    }
    return ( sRetCode );
}


/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_CpnIsPrintTRUE( PROGRPT_PRINTREC * pPrintRec,
*                                            CPNIF  *pCpnIF,
*                                            TTLCPN *pTtlCpn )
*
*   Input   :   PROGRPT_PRINTREC *pPrintRec - address of print record.
*               CPNIF  *pCpnIF      - address of coupon parameter.
*               TTLCPN *pTtlCpn     - address of coupon total.
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
BOOL ProgRpt_CpnIsPrintTRUE( PROGRPT_PRINTREC *pPrintRec,
                             CPNIF  *pCpnIF,
                             TTLCPN *pTtlCpn )
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
        if ( ! ProgRpt_CpnGetInteger( &( pPrintRec->Operand2 ), pCpnIF, pTtlCpn, &hugeint )) {
            return ( FALSE );
        }
        D13Value2 = hugeint;

        /* --- break;... not used --- */

    case PROGRPT_PRTCOND_NOTZERO:   /* ope1 != 0 */
        if ( ! ProgRpt_CpnGetInteger( &( pPrintRec->Operand1 ), pCpnIF, pTtlCpn, &hugeint )) {
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
**  Synopsis:   UCHAR ProgRpt_CpnQueryDataType( PROGRPT_ITEM *pTargetItem )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   UCHAR uchType - data type of specified item.
*                                   PROGRPT_TYPE_UCHAR
*                                   PROGRPT_TYPE_ULONG
*                                   PROGRPT_TYPE_TOTAL
*                                   PROGRPT_TYPE_LTOTAL
*                                   PROGRPT_TYPE_DATE
*                                   PROGRPT_TYPE_STRING
*
**  Description:
*    This function determines that which data type is the data pointed by
*   pTargetItem. It returns data type, if function successful.
*==========================================================================
*/
UCHAR ProgRpt_CpnQueryDataType( PROGRPT_ITEM *pTargetItem )
{
    UCHAR uchType;  /* data type of target item */

    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );

    uchType = PROGRPT_TYPE_INVALID;

    /* --- what kind of groups ? --- */
    switch ( pTargetItem->uchGroup ) {

    case PROGRPT_GROUP_KEY:         /* loop key group */
        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_CPNKEY_CPNNO:
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

        case PROGRPT_TTL_CPNTOTAL:
            uchType = PROGRPT_TYPE_TOTAL;
            break;

        default:
            break;
        }
        break;

    case PROGRPT_GROUP_PARAM:       /* parameter group */
        switch ( pTargetItem->uchMajor ){
        case PROGRPT_CPNPARA_MNEMO:
            uchType = PROGRPT_TYPE_STRING;
            break;

        case PROGRPT_CPNPARA_PRICE:
            uchType = PROGRPT_TYPE_ULONG;
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
**  Synopsis:   BOOL ProgRpt_CpnGetInteger( PROGRPT_ITEM *pTargetItem,
*                                           CPNIF     *pCpnIF,
*                                           TTLCPN    *pTtlCpn,
*                                           D13DIGITS *phugeOutput )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               CPNIF     *pCpnIF         - address of coupon parameter.
*               TTLCPN    *pTtlCpn        - address of coupon total.
*   Output  :   D13DIGITS *phugeOutput    - address of retrieved value.
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
BOOL ProgRpt_CpnGetInteger( PROGRPT_ITEM *pTargetItem,
                            CPNIF   *pCpnIF,
                            TTLCPN  *pTtlCpn,
                            D13DIGITS *phugeOutput )
{
    ASSERT( pTargetItem != NULL );
    ASSERT( phugeOutput != NULL );

    switch ( pTargetItem->uchGroup ) {
    case PROGRPT_GROUP_KEY:
        if ( pCpnIF == ( CPNIF * )NULL ) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_CPNKEY_CPNNO:
            *phugeOutput = pTtlCpn->usCpnNumber;
            break;

        default:  
            return ( FALSE );
        }
        break;

    case PROGRPT_GROUP_TOTALCO:
        if ( pTtlCpn == ( TTLCPN * )NULL ) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_TTL_CPNTOTAL:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCpn->CpnTotal.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCpn->CpnTotal.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        default:
            return ( FALSE );
        }
        break;

    case PROGRPT_GROUP_PARAM:
        if ( pCpnIF == ( CPNIF * )NULL ) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_CPNPARA_PRICE:
            *phugeOutput = (LONG)pCpnIF->ParaCpn.ulCouponAmount;
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
**  Synopsis:   BOOL ProgRpt_CpnGetDateTime( PROGRPT_ITEM *pTargetItem,
*                                            UCHAR  uchMinorClass,
*                                            TTLCPN *pTtlCpn,
*                                            DATE   *pDateTime )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               UCHAR   uchMinorClass     - minor class for department total.
*               TTLCPN  *pTtlCpn          - address of coupon total.
*   Output  :   DATE    *pDateTime        - address of date/time data.
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
BOOL ProgRpt_CpnGetDateTime( PROGRPT_ITEM *pTargetItem,
                             UCHAR uchMinorClass,
                             TTLCPN *pTtlCpn,
                             N_DATE   *pDateTime )
{
    SHORT   sTtlStatus;
    TTLCPN  TtlCoupon;

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

    if ( pTtlCpn == ( TTLCPN * )NULL ) {

        /* --- print date/time at out of loop --- */

        TtlCoupon.uchMajorClass = CLASS_TTLCPN;
        TtlCoupon.uchMinorClass = uchMinorClass;
        TtlCoupon.usCpnNumber  = 1;

        if (( sTtlStatus = SerTtlTotalRead( &TtlCoupon )) != TTL_SUCCESS ) {
            return ( FALSE );
        }
        pTtlCpn = &TtlCoupon;
    }

    switch ( pTargetItem->uchMajor ) {

    case PROGRPT_TTL_FROM:

        pDateTime->usMin   = pTtlCpn->FromDate.usMin;
        pDateTime->usHour  = pTtlCpn->FromDate.usHour;
        pDateTime->usMDay  = pTtlCpn->FromDate.usMDay;
        pDateTime->usMonth = pTtlCpn->FromDate.usMonth;
        break;

    case PROGRPT_TTL_TO:

        pDateTime->usMin   = pTtlCpn->ToDate.usMin;
        pDateTime->usHour  = pTtlCpn->ToDate.usHour;
        pDateTime->usMDay  = pTtlCpn->ToDate.usMDay;
        pDateTime->usMonth = pTtlCpn->ToDate.usMonth;
        break;

    default:
        return ( FALSE );
    }
    return ( TRUE );
}


/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_CpnGetString( PROGRPT_ITEM *pTargetItem,
*                                          CPNIF        *pCpnIF,
*                                          UCHAR        *pszString )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               CPNIF        *pCpnIF      - address of coupon paremeter.
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
BOOL ProgRpt_CpnGetString( PROGRPT_ITEM *pTargetItem,
                           CPNIF        *pCpnIF,
                           TCHAR        *pszString )
{
    UCHAR   uchStrLen;
    UCHAR   uchDummyKeyNo;

    ASSERT( pszString != ( UCHAR * )NULL );
    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );

    switch ( pTargetItem->uchGroup ) {

    case PROGRPT_GROUP_PARAM:

        if ( pCpnIF == ( CPNIF * )NULL ) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {

        case PROGRPT_CPNPARA_MNEMO:

            memset( pszString, 0x00, PARA_CPN_LEN + 1 );
            _tcsncpy( pszString, pCpnIF->ParaCpn.aszCouponMnem, PARA_CPN_LEN );

            /* --- fill space character, if NULL character found --- */

            uchStrLen = 0;
            while (( uchStrLen < PARA_CPN_LEN )  &&
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

        uchDummyKeyNo = 1;
        ProgRpt_GetMnemonic( pTargetItem, pszString, uchDummyKeyNo );
        break;

    default:
        return ( FALSE );
    }
    return ( TRUE );
}

/* ===== End of File (_PROGCPN.C) ===== */