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
*   Program Name       : _PROGCAS.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : The provided function names are as follows.
*
*   ProgRpt_CasPrint()          - print current/saved daily cashier total.
*   ProgRpt_CasReset()          - reset current daily to saved total file.
*   ProgRpt_CasIsResettable()   - determine cashier is resettable or not.
*   ProgRpt_CasIsBadLoopKey()   - determine loop key is valid or invalid.
*   ProgRpt_CasPrintResetCasID()- print resetted cashier ID.
*
*   ProgRpt_CasLOOP()           - execute loop operation in cashier ttl.
*   ProgRpt_CasPRINT()          - execute print operation in cashier ttl.
*   ProgRpt_CasMATH()           - execute mathematics operation in cas ttl.
*
*   ProgRpt_CasFindCashier()    - find cashier para/total by cashier no.
*   ProgRpt_CasIsLoopTRUE()     - determine loop condition is TRUE.
*   ProgRpt_CasIsPrintTRUE()    - determine print condition is TRUE.
*   ProgRpt_CasQueryDataType()  - get type of specified data.
*   ProgRpt_CasGetInteger()     - get long value from cashier total/para.
*   ProgRpt_CasGetDateTime()    - get date data from cashier total.
*   ProgRpt_CasGetString()      - get string data from cashier parameter.
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
*    Date   : Ver.Rev  : NAME       : Description
* Mar-21-96 : 00.00.01 : T.Nakahata :Initial
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-11-99 : 01.00.00 : hrkato     : Saratoga
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
#include    <csstbcas.h>
#include    <csstbpar.h>
#include    <csstbttl.h>
#include    <maint.h>
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
**  Synopsis:   SHORT ProgRpt_CasPrint( UCHAR  uchProgRptNo,
*                                       UCHAR  uchMinorClass,
*                                       USHORT usCashierNo )
*
*   Input   :   UCHAR  uchProgRptNo  - programmable report no.
*               UCHAR  uchMinorClass - minor class for cashier total.
*               USHORT usCashierNo   - target cashier no.
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
*    This function prints specified cashier report.
*==========================================================================
*/
SHORT ProgRpt_CasPrint( UCHAR  uchProgRptNo,
                        UCHAR  uchMinorClass,
                        ULONG  ulCashierNo )
{
    SHORT   sRetCode;           /* return code */
    PROGRPT_FHEAD   FileInfo;   /* information about prog report file */
	PROGRPT_RPTHEAD ReportInfo = { 0 }; /* information about report format */
    USHORT  usMaxNoOfRec;       /* maximum no. of record in report format */
    USHORT  usLoopIndex;        /* loop index for format record */
    ULONG   ulReadOffset;       /* read offset for format record */
    SHORT       sNoOfCashier;   /* no. of resident cashiers */
    SHORT       sLoopIndex;     /* loop index for reset cashier total */
    ULONG       ausResidentCasID[ MAX_NO_CASH ];
                                /* array for resident cashier ID */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));
    ASSERT( ! ProgRpt_CasIsBadLoopKey( &sRetCode, uchProgRptNo, ulCashierNo ));

    /* --- determine specified report format is correct or incorrect --- */
    if (( sRetCode = ProgRpt_GetProgRptInfo( uchProgRptNo, &ReportInfo )) != PROGRPT_SUCCESS ) {
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    if ( ReportInfo.usFileType != PROGRPT_FILE_CAS ) {
        return ( LDT_PROHBT_ADR );
    }

    if ( ReportInfo.usDevice == PROGRPT_DEVICE_MLD ) {
        if (( uchMinorClass == CLASS_TTLSAVDAY ) || ( uchMinorClass == CLASS_TTLSAVPTD )) {
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

    usLoopIndex  = 0;
    usMaxNoOfRec = ReportInfo.usNoOfLoop  + ReportInfo.usNoOfPrint + ReportInfo.usNoOfMath;

    while ( usLoopIndex < usMaxNoOfRec ) {
		ULONG            ulActualRead = 0;       /* actual length of read record function */
		PROGRPT_RECORD   auchRecord;

        /* --- retrieve a formatted record --- */
        ProgRpt_ReadFile( ulReadOffset, &auchRecord, PROGRPT_MAX_REC_LEN, &ulActualRead );
        if ( ulActualRead != PROGRPT_MAX_REC_LEN ) {
            return ( LDT_ERR_ADR );
        }

        /* --- analyze operation code of format record, and
            store it to current row item buffer --- */
        switch (auchRecord.Loop.uchOperation) {
        case PROGRPT_OP_PRINT:  /* print operation */
            sRetCode = ProgRpt_CasPRINT( &ReportInfo, &auchRecord.Print, uchMinorClass, NULL, NULL );
            break;

        case PROGRPT_OP_MATH:   /* mathematics operation */
            sRetCode = ProgRpt_CasMATH(&auchRecord.MathOpe, NULL );
            break;

        case PROGRPT_OP_LOOP:   /* loop (begin) operation */
            ProgRpt_PrintLoopBegin( &ReportInfo, &auchRecord.Loop);

            switch (auchRecord.Loop.LoopItem.uchMajor ) {
            case PROGRPT_CASKEY_CASNO:
                sRetCode = ProgRpt_CasLOOP( &ReportInfo, &auchRecord.Loop, uchMinorClass, ulCashierNo, ulReadOffset );
                usLoopIndex  += auchRecord.Loop.usLoopEnd;
                ulReadOffset += ( ULONG )( PROGRPT_MAX_REC_LEN *  auchRecord.Loop.usLoopEnd );
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
		TTLCASHIER  TtlCas = { 0 };         /* structure for total module i/f */

		if ( 0 < ulCashierNo ) {    /* individual reset (Cas# = 1...999) */
            ausResidentCasID[ 0 ] = ulCashierNo;
            sNoOfCashier = 1;
        } else {                    /* all reset (Cas# = 0) */
            /* --- retrieve all of current assigned cashier ID --- */
            sNoOfCashier = SerCasAllIdRead( sizeof( ausResidentCasID ), ausResidentCasID );
            if ( sNoOfCashier < 0 ) {
                return ( CasConvertError( sNoOfCashier ));
            }
        }

        /* --- declare saved cashier total is already reported --- */
        TtlCas.uchMajorClass   = CLASS_TTLCASHIER;
        TtlCas.uchMinorClass   = uchMinorClass;

        sLoopIndex = 0;
        while ( sLoopIndex < sNoOfCashier ) {
            TtlCas.ulCashierNumber = ausResidentCasID[ sLoopIndex ];
            sRetCode = SerTtlIssuedReset( &TtlCas, TTL_NOTRESET );
            if (( sRetCode = TtlConvertError( sRetCode )) != SUCCESS ) {
                /* --- print out error code --- */
                RptPrtError( sRetCode );
                break;
            }
            sLoopIndex++;
        }
    }
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_CasReset( USHORT usCashierNo )
*
*   Input   :   USHORT usCashierNo  - target cashier no. to reset.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 LDT_KEYOVER_ADR
*                                 LDT_ERR_ADR       ( TTL_FAIL )
*                                 LDT_PROHBT_ADR    ( OP_FILE_NOT_FOUND )
*                                   
**  Description:
*    This function resets specified cashier total.
*==========================================================================
*/
SHORT ProgRpt_CasReset( UCHAR  uchMinorClass, ULONG ulCashierNo )    /* 0(ALL), 1...999 */
{
    TTLCASHIER  TtlCas;         /* structure for reset cashier total */
    SHORT       sRetCode;       /* return value for user application */
    SHORT       sNoOfCashier;   /* no. of resident cashiers */
    SHORT       sLoopIndex;     /* loop index for reset cashier total */
    USHORT      usExecuteFlag;  /* execute flag for EOD/PTD report */
    ULONG       ausResidentCasID[ MAX_NO_CASH ];
                                /* array for resident cashier ID */

    ASSERT( ulCashierNo <= MAINT_CASH_MAX );
    ASSERT( RptEODChkFileExist( FLEX_CAS_ADR ) == RPT_FILE_EXIST );

    if ( 0 < ulCashierNo ) {    /* individual reset (Cas# = 1...999) */
        ausResidentCasID[ 0 ] = ulCashierNo;
        sNoOfCashier = 1;
    } else {                    /* all reset (Cas# = 0) */
        /* --- retrieve all of current assigned cashier ID --- */
        sNoOfCashier = SerCasAllIdRead( sizeof( ausResidentCasID ), ausResidentCasID );
        if ( sNoOfCashier < 0 ) {
            return ( CasConvertError( sNoOfCashier ));
        }
    }

    /* --- this function ignores following parameter --- */
    usExecuteFlag = 0;

    /* --- reset specified department total file --- */
    TtlCas.uchMajorClass = CLASS_TTLCASHIER;
    TtlCas.uchMinorClass = uchMinorClass;
    sRetCode   = TTL_SUCCESS;
    sLoopIndex = 0;

    while (( sLoopIndex < sNoOfCashier ) && ( sRetCode == TTL_SUCCESS )) {

        TtlCas.ulCashierNumber = ausResidentCasID[ sLoopIndex ];

        ASSERT( ProgRpt_CasIsResettable( uchMinorClass, TtlCas.ulCashierNumber ));
        if (( sRetCode = SerTtlTotalReset( &TtlCas, usExecuteFlag )) == TTL_SUCCESS ) {
            if ( ! ( CliParaMDCCheck( MDC_HOUR_ADR, EVEN_MDC_BIT2 ))) {
                SerCasSecretClr( TtlCas.ulCashierNumber );
            }
            sLoopIndex++;
        }     
    }

    /* --- convert return code to lead-thru code --- */
    return ( TtlConvertError( sRetCode ));
}


/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_CasIsResettable( USHORT usCashierNo )
*
*   Input   :   USHORT usCashierNo  - cashier no. to determine resettable
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - current total is resettable.
*                       FALSE   - current total is not resettable.
*   
**  Description:
*    This function detemines specified cashier total is resettable or not.
*   If saved total file is not issued, current total is not resettable.
*==========================================================================
*/
BOOL ProgRpt_CasIsResettable( UCHAR  uchMinorClass, ULONG ulCashierNo )  /* 0(All), 1...999 */
{
	TTLREPORT   TtlReport = { 0 };      /* structure for reset dept total */

    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) || ( uchMinorClass == CLASS_TTLCURPTD ));
    ASSERT( ulCashierNo <= MAINT_CASH_MAX );

    /* --- determine specified saved total file is issued ? --- */
    if ( RptEODChkFileExist( FLEX_CAS_ADR ) != RPT_FILE_EXIST ) {
        return ( FALSE );
    }

    TtlReport.uchMajorClass = CLASS_TTLCASHIER;
    if ( uchMinorClass == CLASS_TTLCURDAY ) {
        TtlReport.uchMinorClass = CLASS_TTLSAVDAY;
    } else {
        TtlReport.uchMinorClass = CLASS_TTLSAVPTD;
    }
    TtlReport.ulNumber = ulCashierNo;

    if ( SerTtlIssuedCheck( &TtlReport, TTL_NOTRESET ) == TTL_NOT_ISSUED ) {
        return ( FALSE );
    }
    return ( TRUE );
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_CasIsBadLoopKey( SHORT *psLTDCode,
*                                             UCHAR uchProgRptNo,
*                                             USHORT usCashierNo )
*
*   Input   :   SHORT *psLDTCode    - address of lead-thru code.
*               UCHAR uchProgRptNo  - programmable report no.
*               USHORT usCashierNo  - target cashier no.
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
BOOL ProgRpt_CasIsBadLoopKey( SHORT *psLDTCode, UCHAR uchProgRptNo, ULONG ulCashierNo )
{
    *psLDTCode = ( LDT_KEYOVER_ADR );

    /* --- determine specified prog. report no. is valid or invalid --- */
    if (( uchProgRptNo < 1 ) || ( MAX_PROGRPT_FORMAT < uchProgRptNo )) {
        return ( TRUE );
    }

    /* --- determine specified plu no. is valid or invalid --- */
    if ( MAINT_CASH_MAX < ulCashierNo ) {
        return ( TRUE );
    }

    /* --- determine specified cashier record exists or not --- */
    if ( ulCashierNo != PROGRPT_ALLREAD ) {
		CASIF       CasIf = { 0 };      /* structure for cashier record retrieval */

        CasIf.ulCashierNo = ulCashierNo;
        if ( SerCasIndRead( &CasIf ) != CAS_PERFORM ) {
            *psLDTCode = LDT_NTINFL_ADR;
            return ( TRUE );
        }
    }

    *psLDTCode = SUCCESS;
    return ( FALSE );
}

/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_CasPrintResetCasID( UCHAR  uchProgRptNo,
*                                                 USHORT usCashierNo )
*
*   Input   :   UCHAR   uchProgRptNo - programmable report no.
*               USHORT  usCashierNo  - target cashier no.
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
SHORT ProgRpt_CasPrintResetCasID( UCHAR uchProgRptNo, ULONG ulCashierNo )
{
    SHORT   sRetCode;       /* return value for user application */
    SHORT   sNoOfCashier;   /* no. of resident cashiers */
    SHORT   sLoopIndex;     /* loop index for retrieve cashier */
    ULONG   ausResidentCasID[ MAX_NO_CASH ];   /* array for resident cashier ID */
	RPTCASHIER      RptCashier = { 0 };        /* structure for print cashier */
	PROGRPT_RPTHEAD ReportInfo = { 0 };        /* information about report format */
	MAINTTRANS      MaintTrans = { 0 };

    ASSERT( ! ProgRpt_CasIsBadLoopKey( &sRetCode, uchProgRptNo, ulCashierNo ));

    /* --- determine specified report format is correct or incorrect --- */
    if (( sRetCode = ProgRpt_GetProgRptInfo( uchProgRptNo, &ReportInfo )) != PROGRPT_SUCCESS ) {
        return ( ProgRpt_ConvertError( sRetCode ));
    }

    if ( ReportInfo.usFileType != PROGRPT_FILE_CAS ) {
        return ( LDT_PROHBT_ADR );
    }

    if ( ReportInfo.usDevice == PROGRPT_DEVICE_MLD ) {
        return ( LDT_PROHBT_ADR );
    }

    if ( 0 < ulCashierNo ) {    /* individual reset (Cas# = 1...999) */
        ausResidentCasID[ 0 ] = ulCashierNo;
        sNoOfCashier = 1;
    } else {                    /* all reset (Cas# = 0) */
        /* --- retrieve all of current assigned cashier ID --- */
        sNoOfCashier = SerCasAllIdRead( sizeof( ausResidentCasID ), ausResidentCasID );
        if ( sNoOfCashier < 0 ) {
            return ( CasConvertError( sNoOfCashier ));
        }
    }

    /* --- retrieve trainning id mnemonic --- */
    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
    MaintTrans.usPrtControl  = ( PRT_JOURNAL | PRT_RECEIPT );
	RflGetSpecMnem( MaintTrans.aszTransMnemo, SPC_TRNGID_ADR);

    /* --- print resetted cashier id --- */
    RptCashier.uchMajorClass  = CLASS_RPTCASHIER;
    RptCashier.uchMinorClass  = CLASS_RPTCASHIER_RESET;
    RptCashier.usPrintControl = ( PRT_RECEIPT | PRT_JOURNAL );

    sRetCode = PROGRPT_SUCCESS;

	for (sLoopIndex = 0; sLoopIndex < sNoOfCashier; sLoopIndex++) {
		CASIF   CasIF = { 0 };          /* structure for cashier module */

        CasIF.ulCashierNo = ausResidentCasID[ sLoopIndex ];
        if (( sRetCode = SerCasIndRead( &CasIF )) != CAS_PERFORM ) {
            return ( CasConvertError( sRetCode ));
        }
        if ( CasIF.fbCashierStatus[CAS_CASHIERSTATUS_0] & CAS_TRAINING_CASHIER ) {
            PrtPrintItem( NULL, &MaintTrans );
        }
        RptCashier.ulCashierNumber = CasIF.ulCashierNo;
        _tcsncpy( RptCashier.aszCashMnemo, CasIF.auchCashierName, PARA_CASHIER_LEN );
        PrtPrintItem( NULL, &RptCashier );
    }

    return SUCCESS; /* ### Add (2171 for Win32) V1.0 */
}
/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_CasLOOP( PROGRPT_RPTHEAD *pReportInfo,
*                                      PROGRPT_LOOPREC *pLoopRec,
*                                      UCHAR  uchMinorClass,
*                                      USHORT usCashierNo,
*                                      ULONG  ulBeginOffset )
*
*   Input   :   PROGRPT_RPTHEAD *pReportInfo - address of report header.
*               PROGRPT_LOOPREC *pLoopRec    - address of print record.
*               UCHAR  uchMinorClass - minor class for cashier total.
*               USHORT usCashierNo   - target cashier no.
*               ULONG  ulBeginOffset - offset for loop begin item
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
SHORT ProgRpt_CasLOOP( PROGRPT_RPTHEAD * pReportInfo,
                       PROGRPT_LOOPREC * pLoopRec,
                       UCHAR  uchMinorClass,
                       ULONG  ulCashierNo,
                       ULONG  ulBeginOffset )
{
    CASIF       CasIF;          /* parameter of cashier record */
    TTLCASHIER  TtlCas;         /* total data of retrieved cashier */
    ULONG       ulReadOffset;   /* offset for read programmed report */
    SHORT       sLoopIndex;     /* loop index */
    BOOL        fLoopEnd;       /* boolean value for loop is end */
    SHORT       sRetCode;       /* return code of this function */
    UCHAR       uchLoopBeginRow;    /* row of "loop begin" item */
    UCHAR       uchLoopBeginColumn; /* column of "loop begin" item */
    SHORT       sNoOfCashier;   /* no. of resident cashiers */
	ULONG       ausResidentCasID[MAX_NO_CASH] = { 0 };
                                /* array for resident cashier ID */

    ASSERT( pReportInfo != ( PROGRPT_RPTHEAD * )NULL );
    ASSERT( pLoopRec    != ( PROGRPT_LOOPREC * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) || ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) || ( uchMinorClass == CLASS_TTLSAVPTD ));
    ASSERT( ulCashierNo <= MAINT_CASH_MAX );

    /* --- define maximum loop count & target item no. --- */
    switch ( pLoopRec->LoopItem.uchMajor ) {
    case PROGRPT_CASKEY_CASNO:      /* read cashier by cashier no. */
        /* --- prepare loop counter & flag before record retrieval --- */
        if ( ulCashierNo == PROGRPT_ALLREAD ) {
            sNoOfCashier = SerCasAllIdRead( sizeof( ausResidentCasID ), ausResidentCasID );
            if ( sNoOfCashier < 0 ) {
                return ( CasConvertError( sNoOfCashier ));
            }
        } else {
            sNoOfCashier = 1;
            ausResidentCasID[ 0 ] = ulCashierNo;
        }
        break;

    default:
        return ( LDT_ERR_ADR );
    }

    uchLoopBeginRow    = uchProgRptRow;
    uchLoopBeginColumn = uchProgRptColumn;
    sLoopIndex = 0;
    sRetCode = PROGRPT_SUCCESS;

    while (( sLoopIndex < sNoOfCashier ) && ( sRetCode == PROGRPT_SUCCESS )) {
        CasIF.ulCashierNo = ausResidentCasID[ sLoopIndex ];

        /* --- retrieve cashier record. target cashier no. is automatically
            increment in following function --- */
        sRetCode = ProgRpt_CasFindCashier( &CasIF, &TtlCas, uchMinorClass );

        /* --- determine current key record is in accord with specified condition --- */
        if (( sRetCode == PROGRPT_SUCCESS ) && ( ProgRpt_CasIsLoopTRUE( pLoopRec, &CasIF ))) {
            /* --- restore print position to loop begin --- */
            fProgRptEmptyLoop = TRUE;
            uchProgRptRow    = uchLoopBeginRow; 
            uchProgRptColumn = uchLoopBeginColumn; 

            /* --- read a record until end of loop --- */
            fLoopEnd = FALSE;
            ulReadOffset = ulBeginOffset + ( ULONG )PROGRPT_MAX_REC_LEN;

            while ( ! fLoopEnd ) {
				ULONG       ulActualRead;   /* actual length of read record function */
				PROGRPT_RECORD   auchRecord;  /* buffer for format record */

                ProgRpt_ReadFile( ulReadOffset, &auchRecord, PROGRPT_MAX_REC_LEN, &ulActualRead );
                if ( ulActualRead != PROGRPT_MAX_REC_LEN ) {
                    return ( LDT_ERR_ADR );
                }

                /* --- analyze operation code of format record, and store it to current row item buffer --- */
                switch ( auchRecord.Loop.uchOperation) {
                case PROGRPT_OP_PRINT:
                    ProgRpt_CasPRINT( pReportInfo, &auchRecord.Print, uchMinorClass, &CasIF, &TtlCas );
                    break;

                case PROGRPT_OP_MATH:
                    ProgRpt_CasMATH(&auchRecord.MathOpe, &TtlCas );
                    break;

                case PROGRPT_OP_LOOP:
                    if (( auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_END_ROOT ) || (auchRecord.Loop.uchLoopBrace == PROGRPT_LOOP_END_NEST )) {
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

        sLoopIndex++;
    }   /* loop until all requested department record is read */

    fProgRptEmptyLoop = FALSE;
    return ( ProgRpt_ConvertError( sRetCode ));
}


/*
*==========================================================================
**  Synopsis:   SHORT ProgRpt_CasPRINT( PROGRPT_RPTHEAD *pReportInfo,
*                                       PROGRPT_PRINTREC *pPrintRec,
*                                       UCHAR   uchMinorClass,
*                                       CASIF *pCasPara,
*                                       TTLCASHIER *pTtlCas );
*
*   Input   :   PROGRPT_RPTHEAD *pReportInfo - address of report header.
*               PROGRPT_LOOPREC *pPrintRec  - address of print record.
*               UCHAR  uchMinorClass - minor class for cashier total.
*               CASIF       *pCasPara   -   address of cashier parameter.
*               TTLCASHIER  *pTtlCas    -   address of cashier total 
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
SHORT ProgRpt_CasPRINT( PROGRPT_RPTHEAD *pReportInfo,
                        PROGRPT_PRINTREC *pPrintRec,
                        UCHAR uchMinorClass,
                        CASIF *pCasPara,
                        TTLCASHIER *pTtlCas )
{
    UCHAR   uchDataType;        /* target print item data type */
    N_DATE    Date;               /* date for target print item */
    TCHAR   szMnemonic[ PROGRPT_MAX_40CHAR_LEN + 1 ];
                                /* mnemonic work area */
    UCHAR       uchStrLen;          /* length of mnemonic data without NULL */
    D13DIGITS   hugeint;
    D13DIGITS   D13Digit;       /* value for target print item */

    ASSERT( pReportInfo != ( PROGRPT_RPTHEAD * )NULL );
    ASSERT( pPrintRec != ( PROGRPT_PRINTREC * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) || ( uchMinorClass == CLASS_TTLSAVDAY ));

    /* --- determine specified condition is TRUE --- */
    if ( ! ProgRpt_CasIsPrintTRUE( pPrintRec, pTtlCas )) {
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
    uchDataType = ProgRpt_CasQueryDataType( &( pPrintRec->PrintItem ));

    switch ( uchDataType ) {

    case PROGRPT_TYPE_SHORT:
    case PROGRPT_TYPE_USHORT:
    case PROGRPT_TYPE_LONG:
    case PROGRPT_TYPE_TOTAL:
    case PROGRPT_TYPE_LTOTAL:
        if ( ! ProgRpt_CasGetInteger( &( pPrintRec->PrintItem ), pTtlCas, &hugeint ) ) {
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
        if ( ProgRpt_CasGetDateTime( &( pPrintRec->PrintItem ), uchMinorClass, pTtlCas,  &Date )) {
            /* --- format and set to print buffer --- */
            ProgRpt_SetDateTimeBuff( pPrintRec, &Date );
        }
        break;

    case PROGRPT_TYPE_STRING:
        ProgRpt_CasGetString( &( pPrintRec->PrintItem ), pCasPara, szMnemonic );
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
**  Synopsis:   SHORT ProgRpt_CasMATH( PROGRPT_MATHREC *pMathRec,
*                                      TTLCASHIER *pTtlCas )
*
*   Input   :   PROGRPT_MATHREC *pMathRec   - address of mathematical record.
*               TTLCASHIER *pTtlCas - address of cashier total.
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
SHORT ProgRpt_CasMATH( PROGRPT_MATHOPE *pMathRec, TTLCASHIER *pTtlCas )
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
        if ( ! ProgRpt_CasGetInteger( &( pMathRec->Operand1 ), pTtlCas, &Operand1 )) {
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
        if ( ! ProgRpt_CasGetInteger( &( pMathRec->Operand2 ), pTtlCas,  &Operand2 )) {
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
**  Synopsis:   SHORT ProgRpt_CasFindCashier( CASIF      *pCasIF,
*                                             TTLCASHIER *pTtlCas,
*                                             UCHAR      uchMinorClass )
*
*   Input   :   CASIF *pCasIF       - address of cashier parameter.
*               UCHAR uchMinorClass - minor class for cashier total.
*   Output  :   CASIF *pCasIF       - address of cashier parameter.
*               TTLCASHIER *pTtlCas - address of cashier total.
*   InOut   :   Nothing
*
**  Return  :   SHORT   sRetCode    - PROGRPT_SUCCESS
*                                     PROGRPT_END_OF_FILE
*                                   
**  Description:
*    This function retrieves a cashier parameter which is specified at
*   pCasIF parameter.
*==========================================================================
*/
SHORT ProgRpt_CasFindCashier( CASIF      *pCasIF,
                              TTLCASHIER *pTtlCas,
                              UCHAR      uchMinorClass )
{
    SHORT   sRetCode;
    SHORT   sTtlStatus;

    ASSERT( pCasIF != ( CASIF * )NULL );
    ASSERT( pTtlCas != ( TTLCASHIER * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));

    sRetCode = SerCasIndRead( pCasIF );

    switch ( sRetCode ) {

    case CAS_PERFORM:
        memset( pTtlCas, 0x00, sizeof( TTLCASHIER ));
        pTtlCas->uchMajorClass = CLASS_TTLCASHIER;
        pTtlCas->uchMinorClass = uchMinorClass;
        pTtlCas->ulCashierNumber = pCasIF->ulCashierNo;

        sTtlStatus = SerTtlTotalRead( pTtlCas );
        switch ( sTtlStatus ) {
        case TTL_SUCCESS:
        case TTL_NOTINFILE:
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

    default:
        sRetCode = PROGRPT_FILE_NOT_FOUND;
        break;
    }
    return ( sRetCode );
}


/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_CasIsLoopTRUE( PROGRPT_LOOPREC *pLoopRec,
*                                           CASIF   *pCasIF )
*
*   Input   :   PROGRPT_LOOPREC *pLoopRec  - address of loop record.
*               CASIF   *pCasIF - address of cashier parameter.
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
BOOL ProgRpt_CasIsLoopTRUE( PROGRPT_LOOPREC *pLoopRec, CASIF *pCasIF )
{
    BOOL    fRetValue;      /* return value for user application */
    UCHAR   uchLoopCond;    /* condition of loop item */

    ASSERT( pLoopRec != ( PROGRPT_LOOPREC * )NULL );
    ASSERT( pCasIF != ( CASIF * )NULL );

    if ( pLoopRec->uchCondition == PROGRPT_ALLREAD ) {
        return ( TRUE );
    }
    uchLoopCond = pLoopRec->uchCondition;

    if ( pCasIF->fbCashierStatus[0] & CAS_TRAINING_CASHIER ) {
        fRetValue = ( uchLoopCond & PROGRPT_COND_TRAINING ) ? TRUE : FALSE;
    } else {
        fRetValue = ( uchLoopCond & PROGRPT_COND_REGULAR ) ? TRUE : FALSE;
    }
    return ( fRetValue );
}


/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_CasIsPrintTRUE( PROGRPT_PRINTREC * pPrintRec,
*                                            TTLCASHIER *pTtlCas )
*
*   Input   :   PROGRPT_PRINTREC *pPrintRec - address of print record.
*               TTLCASHIER  *pTtlCas    - address of cashier total.
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
BOOL ProgRpt_CasIsPrintTRUE( PROGRPT_PRINTREC *pPrintRec,
                             TTLCASHIER *pTtlCas )
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
        if ( ! ProgRpt_CasGetInteger( &( pPrintRec->Operand2 ), pTtlCas, &hugeint )) {
            return ( FALSE );
        }
        D13Value2 = hugeint;

        /* --- break;... not used --- */
    case PROGRPT_PRTCOND_NOTZERO:   /* ope1 != 0 */
        if ( ! ProgRpt_CasGetInteger( &( pPrintRec->Operand1 ), pTtlCas, &hugeint )) {
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
**  Synopsis:   UCHAR ProgRpt_CasQueryDataType( PROGRPT_ITEM *pTargetItem )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   UCHAR uchType - data type of specified item.
*                                   PROGRPT_TYPE_SHORT
*                                   PROGRPT_TYPE_USHORT
*                                   PROGRPT_TYPE_LONG
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
UCHAR ProgRpt_CasQueryDataType( PROGRPT_ITEM *pTargetItem )
{
    UCHAR uchType;  /* data type of target item */

    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );

    uchType = PROGRPT_TYPE_INVALID;

    /* --- what kind of groups ? --- */

    switch ( pTargetItem->uchGroup ) {

    case PROGRPT_GROUP_KEY:         /* loop key group */
        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_CASKEY_CASNO:
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

        case PROGRPT_TTL_DGGTTL:
        case PROGRPT_TTL_ITEMPROCO:
        case PROGRPT_TTL_CONSTAX:
        case PROGRPT_TTL_VATTOTAL:  /* V3.3 */
        case PROGRPT_TTL_TENDONHAND:/* Saratoga */
        case PROGRPT_TTL_FCONHAND:  /* Saratoga */
            uchType = PROGRPT_TYPE_LONG;
            break;

        case PROGRPT_TTL_NOSALE:
        case PROGRPT_TTL_NOPERSON:
        case PROGRPT_TTL_CHKOPEN:
        case PROGRPT_TTL_CHKCLOSE:
        case PROGRPT_TTL_CUSTOMER:
            uchType = PROGRPT_TYPE_SHORT;
            break;

        case PROGRPT_TTL_POSTRECCO:
            uchType = PROGRPT_TYPE_USHORT;
            break;

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
        case PROGRPT_TTL_FOREIGN:
        case PROGRPT_TTL_SERVICE:
        case PROGRPT_TTL_ADDCHK:
        case PROGRPT_TTL_CHARGE:
        case PROGRPT_TTL_CANCEL:
        case PROGRPT_TTL_MISC:
        case PROGRPT_TTL_AUDACT:
        case PROGRPT_TTL_HASHDEPT:
        case PROGRPT_TTL_BONUS:
        case PROGRPT_TTL_DECTIP:    /* V3.3 */
        case PROGRPT_TTL_TRANTO:
        case PROGRPT_TTL_TRANFROM:
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

    case PROGRPT_GROUP_PARAM:       /* parameter group */
        switch ( pTargetItem->uchMajor ){
        case PROGRPT_CASPARA_NAME:
            uchType = PROGRPT_TYPE_STRING;
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
**  Synopsis:   BOOL ProgRpt_CasGetInteger( PROGRPT_ITEM *pTargetItem,
*                                           TTLCASHIER   *pTtlCas,
*                                           D13DIGITS    *phugeOutput )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               TTLCAS       *pTtlCas     - address of cashier total.
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
BOOL ProgRpt_CasGetInteger( PROGRPT_ITEM *pTargetItem,
                            TTLCASHIER   *pTtlCas,
                            D13DIGITS    *phugeOutput )
{
    ASSERT( pTargetItem != NULL );
    ASSERT( phugeOutput != NULL );

    switch ( pTargetItem->uchGroup ) {

    case PROGRPT_GROUP_KEY:
        if ( pTtlCas == NULL ) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_CASKEY_CASNO:
            *phugeOutput = pTtlCas->ulCashierNumber;
            break;

        default:  
            return ( FALSE );
        }
        break;

    case PROGRPT_GROUP_TOTALCO:
        if ( pTtlCas == ( TTLCASHIER * )NULL ) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {
        case PROGRPT_TTL_DGGTTL:
            *phugeOutput = pTtlCas->lDGGtotal;
            break;

        case PROGRPT_TTL_PLUSVOID:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->PlusVoid.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->PlusVoid.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_PREVOID:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->PreselectVoid.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->PreselectVoid.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_CONSCPN:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->ConsCoupon.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->ConsCoupon.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_ITEMDISC:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->ItemDisc.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->ItemDisc.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_MODDISC:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->ModiDisc.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->ModiDisc.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_REGDISC:
            if (( pTargetItem->uchMinor < 1 ) || ( 6 < pTargetItem->uchMinor )){
                return ( FALSE );
            }
            
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->RegDisc[ pTargetItem->uchMinor - 1 ].lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->RegDisc[ pTargetItem->uchMinor - 1 ].sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_COMBCPN:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->Coupon.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->Coupon.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_PO:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->PaidOut.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->PaidOut.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_ROA:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->RecvAcount.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->RecvAcount.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_DECTIP:    /* V3.3 */
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->DeclaredTips.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->DeclaredTips.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_TPO:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->TipsPaid.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->TipsPaid.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_TENDER:
            if (( pTargetItem->uchMinor < 1 ) || ( 11 < pTargetItem->uchMinor )) {
                return ( FALSE );    
            }

            switch ( pTargetItem->uchMinor ) {
            case 1:
                switch ( pTargetItem->uchItem ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTtlCas->CashTender.Total.lAmount; /* Saratoga */
                    break;
                case PROGRPT_TTLITEM_CO:
                    *phugeOutput = pTtlCas->CashTender.Total.sCounter;/* Saratoga */
                    break;
                default:
                    return ( FALSE );
                }
                break;

            case 2:
                switch ( pTargetItem->uchItem ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTtlCas->CheckTender.Total.lAmount;
                    break;
                case PROGRPT_TTLITEM_CO:
                    *phugeOutput = pTtlCas->CheckTender.Total.sCounter;
                    break;
                default:
                    return ( FALSE );
                }
                break;

            case 3:
                switch ( pTargetItem->uchItem ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTtlCas->ChargeTender.Total.lAmount;
                    break;
                case PROGRPT_TTLITEM_CO:
                    *phugeOutput = pTtlCas->ChargeTender.Total.sCounter;
                    break;
                default:
                    return ( FALSE );
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
                switch ( pTargetItem->uchItem ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTtlCas->MiscTender[ pTargetItem->uchMinor - 4 ].Total.lAmount;
                    break;
                case PROGRPT_TTLITEM_CO:
                    *phugeOutput = pTtlCas->MiscTender[ pTargetItem->uchMinor - 4 ].Total.sCounter;
                    break;
                default:
                    return ( FALSE );
                }
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_TENDONHAND:                /* Saratoga */
            if (( pTargetItem->uchMinor < 1 ) ||
                ( 11 < pTargetItem->uchMinor )) {
                return ( FALSE );    
            }

            switch ( pTargetItem->uchMinor ) {
            case 1:
                switch ( pTargetItem->uchItem ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTtlCas->CashTender.lHandTotal;
                    break;
                case PROGRPT_TTLITEM_CO:
                    *phugeOutput = pTtlCas->CashTender.lHandTotal;
                    break;
                default:
                    return ( FALSE );
                }
                break;

            case 2:
                switch ( pTargetItem->uchItem ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTtlCas->CheckTender.lHandTotal;
                    break;
                case PROGRPT_TTLITEM_CO:
                    *phugeOutput = pTtlCas->CheckTender.lHandTotal;
                    break;
                default:
                    return ( FALSE );
                }
                break;

            case 3:
                switch ( pTargetItem->uchItem ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTtlCas->ChargeTender.lHandTotal;
                    break;
                case PROGRPT_TTLITEM_CO:
                    *phugeOutput = pTtlCas->ChargeTender.lHandTotal;
                    break;
                default:
                    return ( FALSE );
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
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
                switch ( pTargetItem->uchItem ) {
                case PROGRPT_TTLITEM_TOTAL:
                    *phugeOutput = pTtlCas->MiscTender[ pTargetItem->uchMinor - 4 ].lHandTotal;
                    break;
                case PROGRPT_TTLITEM_CO:
                    *phugeOutput = pTtlCas->MiscTender[ pTargetItem->uchMinor - 4 ].lHandTotal;
                    break;
                default:
                    return ( FALSE );
                }
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_FOREIGN:
            if (( pTargetItem->uchMinor < 1 ) || ( 8 < pTargetItem->uchMinor )){         /* Saratoga */
                return ( FALSE );
            }

            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->ForeignTotal[ pTargetItem->uchMinor - 1 ].Total.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->ForeignTotal[ pTargetItem->uchMinor - 1 ].Total.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_FCONHAND:                  /* Saratoga */
            if (( pTargetItem->uchMinor < 1 ) || ( 8 < pTargetItem->uchMinor )){
                return ( FALSE );
            }

            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->ForeignTotal[ pTargetItem->uchMinor - 1 ].lHandTotal;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->ForeignTotal[ pTargetItem->uchMinor - 1 ].lHandTotal;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_SERVICE:
            if (( pTargetItem->uchMinor < 3 ) || ( 8 < pTargetItem->uchMinor )){
                return ( FALSE );
            }

            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->ServiceTotal[ pTargetItem->uchMinor - 3 ].lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->ServiceTotal[ pTargetItem->uchMinor - 3 ].sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_ADDCHK:
            if (( pTargetItem->uchMinor < 1 ) || ( 3 < pTargetItem->uchMinor )){
                return ( FALSE );
            }

            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->AddCheckTotal[ pTargetItem->uchMinor - 1 ].lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->AddCheckTotal[ pTargetItem->uchMinor - 1 ].sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_CHARGE:
            /* V3.3 */
            if ( 3 < pTargetItem->uchMinor ){
                return ( FALSE );
            }

            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->ChargeTips[pTargetItem->uchMinor == 0 ? 0 : pTargetItem->uchMinor-1].lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->ChargeTips[pTargetItem->uchMinor == 0 ? 0 : pTargetItem->uchMinor-1].sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_CANCEL:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->TransCancel.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->TransCancel.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_MISC:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->MiscVoid.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->MiscVoid.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_AUDACT:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->Audaction.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->Audaction.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_LOAN:                      /* Saratoga */
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->Loan.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->Loan.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_PICKUP:                    /* Saratoga */
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->Pickup.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->Pickup.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_NOSALE:
            *phugeOutput = pTtlCas->sNoSaleCount;
            break;

        case PROGRPT_TTL_ITEMPROCO:
            *phugeOutput = pTtlCas->lItemProductivityCount;
            break;

        case PROGRPT_TTL_NOPERSON:
            *phugeOutput = pTtlCas->sNoOfPerson;
            break;

        case PROGRPT_TTL_CHKOPEN:
            *phugeOutput = pTtlCas->sNoOfChkOpen;
            break;

        case PROGRPT_TTL_CHKCLOSE:
            *phugeOutput = pTtlCas->sNoOfChkClose;
            break;

        case PROGRPT_TTL_CUSTOMER:
            *phugeOutput = pTtlCas->sCustomerCount;
            break;

        case PROGRPT_TTL_HASHDEPT:
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->HashDepartment.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->HashDepartment.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_BONUS:
            if (( pTargetItem->uchMinor < 1 ) || ( 8 < pTargetItem->uchMinor )){
                return ( FALSE );
            }

            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->Bonus[ pTargetItem->uchMinor - 1 ].lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->Bonus[ pTargetItem->uchMinor - 1 ].sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_TRANTO:    /* V3.3 */
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->CheckTransTo.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->CheckTransTo.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_TRANFROM:  /* V3.3 */
            switch ( pTargetItem->uchItem ) {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->CheckTransFrom.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->CheckTransFrom.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_CONSTAX:
            *phugeOutput = pTtlCas->lConsTax;
            break;

        case PROGRPT_TTL_POSTRECCO:
            *phugeOutput = pTtlCas->usPostRecCo;
            break;

        case PROGRPT_TTL_VATTOTAL:  /* V3.3 */
            *phugeOutput = pTtlCas->lVATServiceTotal;
            break;

        case PROGRPT_TTL_FSCREDIT:
            switch ( pTargetItem->uchItem )
            {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->FoodStampCredit.lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->FoodStampCredit.sCounter;
                break;

            default:
                return ( FALSE );
            }
            break;

        case PROGRPT_TTL_COUPON:
            switch ( pTargetItem->uchItem )
            {
            case PROGRPT_TTLITEM_TOTAL:
                *phugeOutput = pTtlCas->aUPCCoupon[pTargetItem->uchMinor - 1].lAmount;
                break;

            case PROGRPT_TTLITEM_CO:
                *phugeOutput = pTtlCas->aUPCCoupon[pTargetItem->uchMinor - 1].sCounter;
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
**  Synopsis:   BOOL ProgRpt_CasGetDateTime( PROGRPT_ITEM *pTargetItem,
*                                            UCHAR        uchMinorClass,
*                                            TTLCASHIER   *pTtlCas,
*                                            DATE         *pDateTime )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               UCHAR   uchMinorClass     - minor class for department total.
*               TTLCAS       *pTtlCas     - address of cashier total.
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
BOOL ProgRpt_CasGetDateTime( PROGRPT_ITEM *pTargetItem,
                             UCHAR        uchMinorClass,
                             TTLCASHIER   *pTtlCas,
                             N_DATE         *pDateTime )
{
    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );
    ASSERT( pDateTime != ( N_DATE * )NULL );
    ASSERT(( uchMinorClass == CLASS_TTLCURDAY ) ||\
           ( uchMinorClass == CLASS_TTLSAVDAY ) ||\
           ( uchMinorClass == CLASS_TTLCURPTD ) ||\
           ( uchMinorClass == CLASS_TTLSAVPTD ));

    if ( pTtlCas == ( TTLCASHIER * )NULL ) {
        return ( FALSE );
    }

    if (( uchMinorClass == CLASS_TTLCURDAY ) || ( uchMinorClass == CLASS_TTLCURPTD )) {
        if ( pTargetItem->uchMajor == PROGRPT_TTL_TO ) {
            return ( ProgRpt_GetCurDateTime( pDateTime ));
        }
    }

    switch ( pTargetItem->uchMajor ) {
    case PROGRPT_TTL_FROM:
        pDateTime->usMin   = pTtlCas->FromDate.usMin;
        pDateTime->usHour  = pTtlCas->FromDate.usHour;
        pDateTime->usMDay  = pTtlCas->FromDate.usMDay;
        pDateTime->usMonth = pTtlCas->FromDate.usMonth;
        break;

    case PROGRPT_TTL_TO:
        pDateTime->usMin   = pTtlCas->ToDate.usMin;
        pDateTime->usHour  = pTtlCas->ToDate.usHour;
        pDateTime->usMDay  = pTtlCas->ToDate.usMDay;
        pDateTime->usMonth = pTtlCas->ToDate.usMonth;
        break;

    default:
        return ( FALSE );
    }
    return ( TRUE );
}


/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_CasGetString( PROGRPT_ITEM *pTargetItem,
*                                          CASIF        *pCasPara,
*                                          UCHAR        *pszString )
*
*   Input   :   PROGRPT_ITEM *pTargetItem - address of target record.
*               CASIF        *pCasPara    - address of cashier total.
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
BOOL ProgRpt_CasGetString( PROGRPT_ITEM *pTargetItem,
                           CASIF        *pCasPara,
                           TCHAR        *pszString )
{
    UCHAR   uchStrLen;
    UCHAR   uchDummyKeyNo;

    ASSERT( pszString != ( UCHAR * )NULL );
    ASSERT( pTargetItem != ( PROGRPT_ITEM * )NULL );

    switch ( pTargetItem->uchGroup ) {

    case PROGRPT_GROUP_PARAM:

        if ( pCasPara == ( CASIF * )NULL ) {
            return ( FALSE );
        }

        switch ( pTargetItem->uchMajor ) {

        case PROGRPT_CASPARA_NAME:

            memset( pszString, 0x00, PARA_CASHIER_LEN + 1 );
            _tcsncpy( pszString, pCasPara->auchCashierName, PARA_CASHIER_LEN );

            /* --- fill space character, if NULL character found --- */

            uchStrLen = 0;
            while (( uchStrLen < PARA_CASHIER_LEN )  &&
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

/* ===== End of File (_PROGCAS.C) ===== */