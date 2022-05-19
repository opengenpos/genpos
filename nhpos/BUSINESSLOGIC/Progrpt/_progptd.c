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
*   Program Name       : _PROGPTD.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            : /AM /W4 /G1s /Za /Zp /Os /Zi
*  ------------------------------------------------------------------------
*   Abstract           :The provided function names are as follows.
*
*   ProgRpt_PTDPrint()          - print current/saved PTD total files.
*   ProgRpt_PTDReset()          - reset current PTD to saved total files.
*   ProgRpt_PTDIsResettable()   - determine PTD is resettable or not.
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
*    Date      Ver.Rev :NAME       :Description
*    Feb-29-96:03.01.00:T.Nakahata :Initial
*    Apr-01-96:03.01.02:T.Nakahata :fixed (initialize sRetCode at PTDPrint)
*    Apr-12-96:03.01.03:T.Nakahata :check total file exists before print
*    Aug-08-99:03.05.00:M.Teraki   :Remove WAITER_MODEL
*    Aug-13-99:03.05.00:M.Teraki   :Merge GUEST_CHECK_MODEL/STORE_RECALL_MODEL
*    Dec-11-99:01.00.00:hrkato     :Saratoga
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
#include    <maint.h>
#include    <csttl.h>
#include    <cscas.h>
#include    <cswai.h>
#include    <csop.h>
#include    <report.h>
#include    <progrept.h>
#include    <csprgrpt.h>
#include    "_progrpt.h"

#include <csstbpar.h>
#include <csstbttl.h>
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
**  Synopsis:   VOID ProgRpt_PTDPrint( UCHAR    uchReportType,
*                                      PROGRPT_PTDPTDIF *paPTDRec,
*                                      USHORT   usNoOfRpt )
*
*   Input   :   UCHAR            uchReportType - report type of EOD report.
*               PROGRPT_PTDPTDIF *paPTDRec  - points to array of PTD info.
*               USHORT           usNoOfRpt  - number of PTD report
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*   
**  Description:
*    This function resets current PTD totals at PTD reset report.
*    An application must call ProgRpt_PTDIsRessetable() function before
*   calling this function. If the specified total is resettable, it may
*   call this function.
*    This function deletes saved PTD total, and then copies current
*   total to saved total.
*==========================================================================
*/
SHORT ProgRpt_PTDPrint( UCHAR   uchReportType,
                        PROGRPT_EODPTDIF *paPTDRec,
                        USHORT  usNoOfRpt )
{
    TCHAR   auchPLUNumber[STD_PLU_NUMBER_LEN+1];
    SHORT   sRetCode;       /* return code for internal function */
    USHORT  usLoopCo;       /* index of PTD report information for loop */
    UCHAR   uchProgRptNo;   /* target programmable report no. */
    BOOL    fExecutePTD;    /* indicate execute at PTD report */
    USHORT  usPrintStsSave; /* print status save area */

    ASSERT(( uchReportType == RPT_ONLY_PRT_RESET ) ||\
           ( uchReportType == RPT_PRT_RESET ));
    ASSERT( paPTDRec != ( PROGRPT_EODPTDIF * )NULL );
    ASSERT(( 0 < usNoOfRpt ) && ( usNoOfRpt <= MAX_PROGRPT_EODPTD ));

    memset(auchPLUNumber, 0, sizeof(auchPLUNumber));

    RptInitialize();
    usPrintStsSave = usRptPrintStatus;
    usLoopCo = 0;
    sRetCode = SUCCESS; /* ...bug fixed (1996-4-1) */

    while (( usLoopCo < usNoOfRpt ) && ( sRetCode == SUCCESS )) {

        uchProgRptNo = ( UCHAR )(( paPTDRec + usLoopCo )->usReportNo );
        if ( MAX_PROGRPT_FORMAT < uchProgRptNo ) {
            fExecutePTD   = FALSE;
        } else {
            fExecutePTD   = TRUE;
        }

        switch (( paPTDRec + usLoopCo )->usFileType ) {

        case PROGRPT_FILE_DEPT:
            if ( fExecutePTD ) {
                if ( ! RptPTDChkFileExist( FLEX_DEPT_ADR )) {
                    break;
                }
                RptInitialize();
                if ( uchProgRptNo == 0 ) {
                    sRetCode = RptDEPTRead( CLASS_TTLSAVPTD, RPT_PTD_ALLREAD, 0 );
                } else {
                    ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_DEPT_ADR, uchReportType );

                    /* --- if EJ system, not report on EJ --- */

                    if ( fProgRptWithEJ ) {
                        usRptPrintStatus &= ~( PRT_JOURNAL );
                    }
                    sRetCode = ProgRpt_DeptPrint( uchProgRptNo, CLASS_TTLSAVPTD, PROGRPT_ALLREAD, PROGRPT_ALLREAD );
                    usRptPrintStatus = usPrintStsSave;
                }
            }
            break;

        case PROGRPT_FILE_PLU:
            if ( fExecutePTD ) {
                if (( ! RptEODChkFileExist( FLEX_DEPT_ADR )) || ( ! RptPTDChkFileExist( FLEX_PLU_ADR ))) {
                    break;
                }
                RptInitialize();
                if ( uchProgRptNo == 0 ) {
                    sRetCode = RptPLUPTDRead( CLASS_TTLSAVPTD, RPT_ALL_READ, 0, 0, auchPLUNumber);      /* Saratoga */
                } else {
                    ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_PLU_ADR, uchReportType );

                    /* --- if EJ system, not report on EJ --- */
                    if ( fProgRptWithEJ ) {
                        usRptPrintStatus &= ~( PRT_JOURNAL );
                    }
                    sRetCode = ProgRpt_PluPrint( uchProgRptNo, CLASS_TTLSAVPTD, ( USHORT )PROGRPT_ALLREAD, PROGRPT_ALLREAD, PROGRPT_ALLREAD );
                    usRptPrintStatus = usPrintStsSave;
                }
            }
            break;

        case PROGRPT_FILE_CPN:
            if ( fExecutePTD ) {
                if ( ! RptPTDChkFileExist( FLEX_CPN_ADR )) {
                    break;
                }
                RptInitialize();
                if ( uchProgRptNo == 0 ) {
                    sRetCode = RptCpnRead( CLASS_TTLSAVPTD, RPT_PTD_ALLREAD, 0 );
                } else {
                    ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_CPN_ADR, uchReportType );

                    /* --- if EJ system, not report on EJ --- */
                    if ( fProgRptWithEJ ) {
                        usRptPrintStatus &= ~( PRT_JOURNAL );
                    }
                    sRetCode = ProgRpt_CpnPrint( uchProgRptNo, CLASS_TTLSAVPTD, PROGRPT_ALLREAD );
                    usRptPrintStatus = usPrintStsSave;
                }
            }
            break;

        case PROGRPT_FILE_CAS:  /* V3.3 */
            if ( fExecutePTD ) {
                if ( ! RptPTDChkFileExist( FLEX_CAS_ADR )) {
                    break;
                }
                RptInitialize();
                if ( uchProgRptNo == 0 ) {
                    sRetCode = RptCashierRead( CLASS_TTLSAVPTD, RPT_PTD_ALLREAD, 0, 0 );
                } else {
                    ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_CAS_ADR, uchReportType );

                    /* --- if EJ system, not report on EJ --- */
                    if ( fProgRptWithEJ ) {
                        usRptPrintStatus &= ~( PRT_JOURNAL );
                    }
                    sRetCode = ProgRpt_CasPrint( uchProgRptNo, CLASS_TTLSAVPTD, ( USHORT )PROGRPT_ALLREAD );
                    usRptPrintStatus = usPrintStsSave;
                }
            }
            break;

        case PROGRPT_FILE_WAI:
            break;

        case PROGRPT_FILE_FIN:
            if ( fExecutePTD ) {
                RptInitialize();
                if ( uchProgRptNo == 0 ) {
                    sRetCode = RptRegFinPTDRead( CLASS_TTLSAVPTD, RPT_PTD_ALLREAD );
                } else {
                    ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_FINANC_ADR, uchReportType );

                    /* --- if EJ system, not report on EJ --- */
                    if ( fProgRptWithEJ ) {
                        usRptPrintStatus &= ~( PRT_JOURNAL );
                    }
                    sRetCode = ProgRpt_FinPrint( uchProgRptNo, CLASS_TTLSAVPTD );
                    usRptPrintStatus = usPrintStsSave;
                }
            }
            break;

        case PROGRPT_FILE_HOUR:
            if ( fExecutePTD ) {
                RptInitialize();
                sRetCode = RptHourlyPTDRead(CLASS_TTLSAVPTD, RPT_PTD_ALLREAD );
            }
            break;

        case PROGRPT_FILE_SERTIME:
            if ( fExecutePTD ) {
                if ( ! RptPTDChkFileExist( FLEX_GC_ADR )) {
                    break;
                }
                RptInitialize();
                sRetCode = RptServicePTDRead( CLASS_TTLSAVPTD, RPT_PTD_ALLREAD );
            }
            break;

        case PROGRPT_FILE_INDFIN:
        case PROGRPT_FILE_GCF:
        case PROGRPT_FILE_EJALL:
        default:
            /* --- guest check total is executed at ProgRpt_PTDReset() --- */
            break;
        }
        usLoopCo++;
    }
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_PTDReset( UCHAR    uchReportType,
*                                      PROGRPT_EODPTDIF *paPTDRec,
*                                      USHORT   usNoOfRpt )
*
*   Input   :   UCHAR            uchReportType - report type of EOD report.
*               PROGRPT_EODPTDIF *paPTDRec  - points to array of PTD info.
*               USHORT           usNoOfRpt  - number of PTD report
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*   
**  Description:
*    This function resets current PTD totals at PTD reset report.
*    An application must call ProgRpt_PTDIsRessetable() function before
*   calling this function. If the specified total is resettable, it may
*   call this function.
*    This function deletes saved PTD total, and then copies current
*   total to saved total.
*==========================================================================
*/
VOID ProgRpt_PTDReset( UCHAR    uchReportType,
                       PROGRPT_EODPTDIF *paPTDRec,
                       USHORT   usNoOfRpt )
{
    BOOL    fTtlResetted;   /* boolean value for total is resetted or not */
    BOOL    fExecutePTD;    /* indicate execute at PTD report */
    UCHAR   uchProgRptNo;   /* target programmable report no. */
    USHORT  usLoopCo;       /* index of PTD report information for loop */
	SHORT   sError;

    ASSERT(( uchReportType == RPT_ONLY_RESET ) ||\
           ( uchReportType == RPT_PRT_RESET ));
    ASSERT( paPTDRec != ( PROGRPT_EODPTDIF * )NULL );
    ASSERT(( 0 < usNoOfRpt ) && ( usNoOfRpt <= MAX_PROGRPT_EODPTD ));

    /* --- reset current PTD total to saved PTD total --- */

    fTtlResetted = FALSE;
    usLoopCo = 0;

    while ( usLoopCo < usNoOfRpt ) {

        uchProgRptNo = ( UCHAR )(( paPTDRec + usLoopCo )->usReportNo );
        if ( MAX_PROGRPT_FORMAT < uchProgRptNo ) {
            fExecutePTD   = FALSE;
        } else {
            fExecutePTD   = TRUE;
        }
        switch (( paPTDRec + usLoopCo )->usFileType ) {

        case PROGRPT_FILE_DEPT:
            if ( fExecutePTD ) {
                if ( ! RptPTDChkFileExist( FLEX_DEPT_ADR )) {
                    break;
                }
                if ( ProgRpt_DeptReset( CLASS_TTLCURPTD ) == SUCCESS ) {
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_DEPT_ADR, RPT_ONLY_RESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_PLU:
            if ( fExecutePTD ) {
                if (( ! RptEODChkFileExist( FLEX_DEPT_ADR )) || ( ! RptPTDChkFileExist( FLEX_PLU_ADR ))) {
                    break;
                }
                if ( ProgRpt_PluReset( CLASS_TTLCURPTD ) == SUCCESS ) {
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_PLU_ADR, RPT_ONLY_RESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
    		else
		    if ((CliParaMDCCheck(MDC_PTD4_ADR, EVEN_MDC_BIT1)) &&            /* Execute PLU Optimize V1.0.15 */
        		(RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
		        (RptPTDChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {      /* File Exist */

        		/* Optimize PLU Total File */
		        if ((sError = SerTtlPLUOptimize(CLASS_TTLCURPTD)) != TTL_SUCCESS) {
        
        		    /* Print PLU File Header */
				    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
        			                RPT_ACT_ADR,                            /* Report Name Address */
            			            RPT_PLU_ADR,                            /* Report Name Address */
	                    		    SPC_PTDRST_ADR,                         /* Special Mnemonics Address */
		    	                    0,                          			/* Report Name Address */
        			                0,                                      /* Not Print */
            			            AC_PLU_RESET_RPT,                       /* A/C Number */
                        			(UCHAR)RPT_TYPE_OPTIMIZE,               /* Reset Type */
			                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */

        		    RptPrtError(TtlConvertError(sError));
		        } else {
        		    /* Print PLU File Header */
				    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
        			                RPT_ACT_ADR,                            /* Report Name Address */
            			            RPT_PLU_ADR,                            /* Report Name Address */
	                    		    SPC_PTDRST_ADR,                         /* Special Mnemonics Address */
		    	                    0,                          			/* Report Name Address */
        			                0,                                      /* Not Print */
            			            AC_PLU_RESET_RPT,                       /* A/C Number */
                        			(UCHAR)RPT_TYPE_OPTIMIZE,               /* Reset Type */
			                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */

                    fTtlResetted = TRUE;
		        }
		    }

			/* execute Daily PLU optimize at PTD, 10/11/01 */
		    if ((CliParaMDCCheck(MDC_PTD4_ADR, EVEN_MDC_BIT1)) &&            /* Execute PLU Optimize V1.0.15 */
		        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
        		(RptEODChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {      /* File Exist */

		        /* Optimize PLU Total File */
        		if ((sError = SerTtlPLUOptimize(CLASS_TTLCURDAY)) != TTL_SUCCESS) {
        
				    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
        			                RPT_ACT_ADR,                            /* Report Name Address */
            			            RPT_PLU_ADR,                            /* Report Name Address */
	                    		    SPC_DAIRST_ADR,                         /* Special Mnemonics Address */
		    	                    0,                          			/* Report Name Address */
        			                0,                                      /* Not Print */
            			            AC_PLU_RESET_RPT,                       /* A/C Number */
                        			(UCHAR)RPT_TYPE_OPTIMIZE,               /* Reset Type */
			                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */

            		RptPrtError(TtlConvertError(sError));
        		} else {
		            /* Print PLU File Header */
				    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
        	    		            RPT_ACT_ADR,                            /* Report Name Address */
            	        		    RPT_PLU_ADR,                            /* Report Name Address */
			                        SPC_DAIRST_ADR,                         /* Special Mnemonics Address */
    			                    0,                          			/* Report Name Address */
        	    		            0,                                      /* Not Print */
            	        		    AC_PLU_RESET_RPT,                       /* A/C Number */
		                        	(UCHAR)RPT_TYPE_OPTIMIZE,               /* Reset Type */
	    		                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */

                    fTtlResetted = TRUE;
		        }
		    }
		    
            break;

        case PROGRPT_FILE_CPN:
            if ( fExecutePTD ) {
                if ( ! RptPTDChkFileExist( FLEX_CPN_ADR )) {
                    break;
                }
                if ( ProgRpt_CpnReset( CLASS_TTLCURPTD ) == SUCCESS ) {
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_CPN_ADR, RPT_ONLY_RESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_CAS:  /* V3.3 */
            if ( fExecutePTD ) {
                if ( ! RptPTDChkFileExist( FLEX_CAS_ADR )) {
                    break;
                }
                if ( ProgRpt_CasReset( CLASS_TTLCURPTD, ( USHORT )PROGRPT_ALLRESET ) == SUCCESS ) {
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_CAS_ADR, RPT_ONLY_RESET );
                        ProgRpt_CasPrintResetCasID( uchProgRptNo, PROGRPT_ALLRESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_WAI:
            break;

        case PROGRPT_FILE_FIN:
            if ( fExecutePTD ) {
                if ( ProgRpt_FinReset( CLASS_TTLCURPTD ) == SUCCESS ) {
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_FINANC_ADR, RPT_ONLY_RESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_HOUR:
            if ( fExecutePTD ) {
                if ( ProgRpt_HourReset( CLASS_TTLCURPTD ) == SUCCESS ) {
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_SERTIME:
            if ( fExecutePTD ) {
                if ( ! RptPTDChkFileExist( FLEX_GC_ADR )) {
                    break;
                }
                if ( ProgRpt_TimeReset( CLASS_TTLCURPTD ) == SUCCESS ) {
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_INDFIN:
        case PROGRPT_FILE_GCF:
        case PROGRPT_FILE_EJALL:
        default:
            /* --- EJ cluster reset is executed at ProgRpt_PTDPrint() with
                RPT_PRT_RESET(reset & print) type --- */
            break;
        }
        usLoopCo++;
    }
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_PTDIsResettable( PROGRPT_EODPTDIF *paPTDRec,
*                                             USHORT    usNoOfRpt )
*
*   Input   :   PROGRPT_EODPTDIF *paPTDRec  - points to array of PTD info.
*               USHORT           usNoOfRpt  - number of PTD report
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - current total is resettable.
*                       FALSE   - current total is not resettable.
*   
**  Description:
*    This function detemines PTD reset report is resettable or not.
*   If saved total file is not issued, current total is not resettable.
*==========================================================================
*/
BOOL ProgRpt_PTDIsResettable( PROGRPT_EODPTDIF *paPTDRec, USHORT    usNoOfRpt )
{
    USHORT  usLoopCo;       /* index of PTD report information for loop */
    BOOL    fTtlResettable; /* boolean value for total is resettable or not */
    BOOL    fExecutePTD;    /* indicate execute at EOD report */
    UCHAR   uchProgRptNo;   /* target programmable report no. */

    ASSERT( paPTDRec != ( PROGRPT_EODPTDIF * )NULL );
    ASSERT(( 0 < usNoOfRpt ) && ( usNoOfRpt <= MAX_PROGRPT_EODPTD ));

    fTtlResettable = TRUE;
    usLoopCo = 0;

    while (( usLoopCo < usNoOfRpt ) && fTtlResettable ) {

        uchProgRptNo = ( UCHAR )(( paPTDRec + usLoopCo )->usReportNo );
        if ( MAX_PROGRPT_FORMAT < uchProgRptNo ) {
            fExecutePTD   = FALSE;
        } else {
            fExecutePTD   = TRUE;
        }
        switch (( paPTDRec + usLoopCo )->usFileType ) {

        case PROGRPT_FILE_DEPT:
            if ( fExecutePTD ) {
                if ( ! RptPTDChkFileExist( FLEX_DEPT_ADR )) {
                    break;
                }
                if ( ! ProgRpt_DeptIsResettable( CLASS_TTLCURPTD )) {
                    ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_DEPT_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_PLU:
            if ( fExecutePTD ) {
                if (( ! RptEODChkFileExist( FLEX_DEPT_ADR )) || ( ! RptPTDChkFileExist( FLEX_PLU_ADR ))) {
                    break;
                }
                if ( ! ProgRpt_PluIsResettable( CLASS_TTLCURPTD )) {
                    ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_PLU_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_CPN:
            if ( fExecutePTD ) {
                if ( ! RptPTDChkFileExist( FLEX_CPN_ADR )) {
                    break;
                }
                if ( ! ProgRpt_CpnIsResettable( CLASS_TTLCURPTD )) {
                    ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_CPN_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_CAS:  /* V3.3 */
            if ( fExecutePTD ) {
                if ( ! RptPTDChkFileExist( FLEX_CAS_ADR )) {
                    break;
                }
                if ( ! ProgRpt_CasIsResettable( CLASS_TTLCURPTD, ( USHORT )PROGRPT_ALLRESET )) {
                    ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_CAS_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_WAI:
            break;

        case PROGRPT_FILE_FIN:
            if ( fExecutePTD ) {
                if ( ! ProgRpt_FinIsResettable( CLASS_TTLCURPTD )) {
                    ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_FINANC_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_HOUR:
            if ( fExecutePTD ) {
                if ( ! ProgRpt_HourIsResettable( CLASS_TTLCURPTD )) {
                    ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_HOUR_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_SERTIME:
            if ( fExecutePTD ) {
                if ( ! RptPTDChkFileExist( FLEX_GC_ADR )) {
                    break;
                }
                if ( ! ProgRpt_TimeIsResettable( CLASS_TTLCURPTD )) {
                    ProgRpt_PrintHeader( CLASS_TTLSAVPTD, uchProgRptNo, RPT_SERVICE_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_INDFIN:
        case PROGRPT_FILE_GCF:
        case PROGRPT_FILE_EJALL:
        default:
            /* --- these files are always ressetable --- */
            break;
        }
        usLoopCo++;
    }
    return ( fTtlResettable );
}

/* ===== End of File (_PROGPTD.C) ===== */