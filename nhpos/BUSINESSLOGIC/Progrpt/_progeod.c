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
*   Program Name       : _PROGEOD.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Midium Model
*   Options            : /AM /W4 /G1s /Za /Zp /Os /Zi
*  ------------------------------------------------------------------------
*   Abstract           :The provided function names are as follows.
*
*   ProgRpt_EODPrint()          - print current/saved daily total files.
*   ProgRpt_EODReset()          - reset current daily to saved total file.
*   ProgRpt_EODIsResettable()   - determine EOD is resettable or not.
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
**  Synopsis:   VOID ProgRpt_EODPrint( UCHAR uchReportType,
*                                      PROGRPT_EODPTDIF *paEODRec,
*                                      USHORT    usNoOfRpt )
*
*   Input   :   UCHAR            uchReportType - report type of EOD report.
*               PROGRPT_EODPTDIF *paEODRec  - points to array of EOD info.
*               USHORT           usNoOfRpt  - number of EOD report
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   SHORT sRetCode  - SUCCESS
*                                 RPT_ABORTED
*   
**  Description:
*    This function resets current daily totals at EOD reset report.
*    An application must call ProgRpt_EODIsRessetable() function before
*   calling this function. If the specified total is resettable, it may
*   call this function.
*    This function deletes saved daily total, and then copies current
*   total to saved total.
*==========================================================================
*/
SHORT ProgRpt_EODPrint( UCHAR     uchReportType,
                        PROGRPT_EODPTDIF *paEODRec,
                        USHORT    usNoOfRpt )
{
    SHORT   sRetCode;       /* return code for internal function */
    USHORT  usLoopCo;       /* index of EOD report information for loop */
    UCHAR   uchProgRptNo;   /* target programmable report no. */
    BOOL    fExecuteEOD;    /* indicate execute at EOD report */
    USHORT  usPrintStsSave; /* print status save area */
	TCHAR   auchPLUNumber[STD_PLU_NUMBER_LEN+1] = {0};/* PLU Number,  Saratoga */

    ASSERT(( uchReportType == RPT_ONLY_PRT_RESET ) ||\
           ( uchReportType == RPT_PRT_RESET ));
    ASSERT( paEODRec != ( PROGRPT_EODPTDIF * )NULL );
    ASSERT(( 0 < usNoOfRpt ) && ( usNoOfRpt <= MAX_PROGRPT_EODPTD ));

    RptInitialize();
    usPrintStsSave = usRptPrintStatus;
    usLoopCo = 0;
    sRetCode = SUCCESS; /* ...bug fixed (1996-4-1) */

    while (( usLoopCo < usNoOfRpt ) && ( sRetCode == SUCCESS )) {

        uchProgRptNo = ( UCHAR )(( paEODRec + usLoopCo )->usReportNo );
        if ( MAX_PROGRPT_FORMAT < uchProgRptNo ) {
            fExecuteEOD   = FALSE;
        } else {
            fExecuteEOD   = TRUE;
        }

        switch (( paEODRec + usLoopCo )->usFileType ) {

        case PROGRPT_FILE_DEPT:
            if ( fExecuteEOD ) {
                if ( ! RptEODChkFileExist( FLEX_DEPT_ADR )) {
                    break;
                }
                RptInitialize();
                if ( uchProgRptNo == 0 ) {
					ProgRptLog(35);
                    sRetCode = RptDEPTRead( CLASS_TTLSAVDAY, RPT_EOD_ALLREAD, 0 );
					ProgRptLog(36);
                } else {
					ProgRptLog(37);
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_DEPT_ADR, uchReportType );

                    /* --- if EJ system, not report on EJ --- */

                    if ( fProgRptWithEJ ) {
                        usRptPrintStatus &= ~( PRT_JOURNAL );
                    }
                    sRetCode = ProgRpt_DeptPrint( uchProgRptNo, CLASS_TTLSAVDAY, PROGRPT_ALLREAD, PROGRPT_ALLREAD );
                    usRptPrintStatus = usPrintStsSave;
					ProgRptLog(38);
                }
            }
            break;

        case PROGRPT_FILE_PLU:
            if ( fExecuteEOD ) {
                if (( ! RptEODChkFileExist( FLEX_DEPT_ADR )) || ( ! RptEODChkFileExist( FLEX_PLU_ADR ))) {
                    break;
                }
                RptInitialize();
                if ( uchProgRptNo == 0 ) {
					ProgRptLog(39);
                    sRetCode = RptPLUDayRead( CLASS_TTLSAVDAY, RPT_ALL_READ, 0, 0, auchPLUNumber);
					ProgRptLog(40);
                } else {
					ProgRptLog(41);
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_PLU_ADR, uchReportType );

                    /* --- if EJ system, not report on EJ --- */
                    if ( fProgRptWithEJ ) {
                        usRptPrintStatus &= ~( PRT_JOURNAL );
                    }
                    sRetCode = ProgRpt_PluPrint( uchProgRptNo, CLASS_TTLSAVDAY, ( USHORT )PROGRPT_ALLREAD, PROGRPT_ALLREAD, PROGRPT_ALLREAD );
                    usRptPrintStatus = usPrintStsSave;
					ProgRptLog(42);
                }
            }
            break;

        case PROGRPT_FILE_CPN:
            if ( fExecuteEOD ) {
                if ( ! RptEODChkFileExist( FLEX_CPN_ADR )) {
                    break;
                }
                RptInitialize();
                if ( uchProgRptNo == 0 ) {
                    sRetCode = RptCpnRead( CLASS_TTLSAVDAY, RPT_EOD_ALLREAD, 0 );
                } else {
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_CPN_ADR, uchReportType );

                    /* --- if EJ system, not report on EJ --- */

                    if ( fProgRptWithEJ ) {
                        usRptPrintStatus &= ~( PRT_JOURNAL );
                    }
                    sRetCode = ProgRpt_CpnPrint( uchProgRptNo, CLASS_TTLSAVDAY, PROGRPT_ALLREAD );
                    usRptPrintStatus = usPrintStsSave;
                }
            }
            break;

        case PROGRPT_FILE_CAS:
            if ( fExecuteEOD ) {
                if ( ! RptEODChkFileExist( FLEX_CAS_ADR )) {
                    break;
                }
                RptInitialize();
                if ( uchProgRptNo == 0 ) {
					ProgRptLog(43);
                    sRetCode = RptCashierRead( CLASS_TTLSAVDAY, RPT_EOD_ALLREAD, 0, 0 );
					ProgRptLog(44);
                } else {
					ProgRptLog(45);
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_CAS_ADR, uchReportType );

                    /* --- if EJ system, not report on EJ --- */
                    if ( fProgRptWithEJ ) {
                        usRptPrintStatus &= ~( PRT_JOURNAL );
                    }
                    sRetCode = ProgRpt_CasPrint( uchProgRptNo, CLASS_TTLSAVDAY, ( USHORT )PROGRPT_ALLREAD );
                    usRptPrintStatus = usPrintStsSave;
					ProgRptLog(46);
                }
            }
            break;

        case PROGRPT_FILE_FIN:
            if ( fExecuteEOD ) {
                RptInitialize();
                if ( uchProgRptNo == 0 ) {
					ProgRptLog(47);
                    sRetCode = RptRegFinDayRead( CLASS_TTLSAVDAY, RPT_EOD_ALLREAD );
					ProgRptLog(48);
                } else {
					ProgRptLog(49);
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_FINANC_ADR, uchReportType );

                    /* --- if EJ system, not report on EJ --- */

                    if ( fProgRptWithEJ ) {
                        usRptPrintStatus &= ~( PRT_JOURNAL );
                    }
                    sRetCode = ProgRpt_FinPrint( uchProgRptNo, CLASS_TTLSAVDAY );
                    usRptPrintStatus = usPrintStsSave;
					ProgRptLog(50);
                }
            }
            break;

        case PROGRPT_FILE_INDFIN:
            if ( fExecuteEOD ) {
                RptInitialize();
                if ( uchProgRptNo == 0 ) {
					ProgRptLog(51);
                    sRetCode = RptIndFinDayRead( CLASS_TTLSAVDAY, RPT_EOD_ALLREAD, 0, 0 );
					ProgRptLog(52);
                } else {
					ProgRptLog(53);
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_FINANC_ADR, uchReportType );

                    /* --- if EJ system, not report on EJ --- */

                    if ( fProgRptWithEJ ) {
                        usRptPrintStatus &= ~( PRT_JOURNAL );
                    }
                    sRetCode = ProgRpt_IndPrint( uchProgRptNo, CLASS_TTLSAVDAY, ( USHORT )PROGRPT_ALLREAD );
                    usRptPrintStatus = usPrintStsSave;
					ProgRptLog(54);
                }
            }
            break;

        case PROGRPT_FILE_HOUR:
            if ( fExecuteEOD ) {
                RptInitialize();
				ProgRptLog(55);
                sRetCode = RptHourlyDayRead(CLASS_TTLSAVDAY, RPT_EOD_ALLREAD );
				ProgRptLog(56);
            }
            break;

        case PROGRPT_FILE_SERTIME:
            if ( fExecuteEOD ) {
                if ( ! RptPTDChkFileExist( FLEX_GC_ADR )) {
                    break;
                }
                RptInitialize();
                sRetCode = RptServiceDayRead( CLASS_TTLSAVDAY, RPT_EOD_ALLREAD );
            }
            break;

        case PROGRPT_FILE_EJALL:
            if ( fExecuteEOD ) {
                RptInitialize();
                if ( uchReportType == RPT_PRT_RESET ) {
                    if (( sRetCode = RptEJClusterReset()) == LDT_PROHBT_ADR ) {
                        /* --- this configulation is not Thermal Printer --- */
                        sRetCode = SUCCESS;
                    }
                }
            }
            break;

        case PROGRPT_FILE_GCF:
        default:
            /* --- guest check total is executed at ProgRpt_EODReset() --- */
            break;
        }
        usLoopCo++;
    }
    return ( sRetCode );
}

/*
*==========================================================================
**  Synopsis:   VOID ProgRpt_EODReset( UCHAR    uchReportType,
*                                      PROGRPT_EODPTDIF *paEODRec,
*                                      USHORT   usNoOfRpt )
*
*   Input   :   UCHAR            uchReportType - report type of EOD report.
*               PROGRPT_EODPTDIF *paEODRec  - points to array of EOD info.
*               USHORT           usNoOfRpt  - number of EOD report
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   Nothing
*   
**  Description:
*    This function resets current daily totals at EOD reset report.
*    An application must call ProgRpt_EODIsRessetable() function before
*   calling this function. If the specified total is resettable, it may
*   call this function.
*    This function deletes saved daily total, and then copies current
*   total to saved total.
*==========================================================================
*/
VOID ProgRpt_EODReset( UCHAR    uchReportType,
                       PROGRPT_EODPTDIF *paEODRec,
                       USHORT   usNoOfRpt )
{
    BOOL    fTtlResetted;   /* boolean value for total is resetted or not */
    BOOL    fExecuteEOD;    /* indicate execute at EOD report */
    UCHAR   uchProgRptNo;   /* target programmable report no. */
    USHORT  usLoopCo;       /* index of EOD report information for loop */
	SHORT   sError;

    ASSERT(( uchReportType == RPT_ONLY_RESET ) ||\
           ( uchReportType == RPT_PRT_RESET ));
    ASSERT( paEODRec != ( PROGRPT_EODPTDIF * )NULL );
    ASSERT(( 0 < usNoOfRpt ) && ( usNoOfRpt <= MAX_PROGRPT_EODPTD ));

    /* --- reset current daily total to saved daily total --- */

    fTtlResetted = FALSE;
    usLoopCo = 0;

    while ( usLoopCo < usNoOfRpt ) {

        uchProgRptNo = ( UCHAR )(( paEODRec + usLoopCo )->usReportNo );
        if ( MAX_PROGRPT_FORMAT < uchProgRptNo ) {
            fExecuteEOD   = FALSE;
        } else {
            fExecuteEOD   = TRUE;
        }
        switch (( paEODRec + usLoopCo )->usFileType ) {

        case PROGRPT_FILE_DEPT:
            if ( fExecuteEOD ) {
                if ( ! RptEODChkFileExist( FLEX_DEPT_ADR )) {
                    break;
                }
				ProgRptLog(19);
                if ( ProgRpt_DeptReset( CLASS_TTLCURDAY ) == SUCCESS ) {
					ProgRptLog(20);
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_DEPT_ADR, RPT_ONLY_RESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_PLU:
            if ( fExecuteEOD ) {
                if (( ! RptEODChkFileExist( FLEX_DEPT_ADR )) || ( ! RptEODChkFileExist( FLEX_PLU_ADR ))) {
                    break;
                }
				ProgRptLog(21);
                if ( ProgRpt_PluReset( CLASS_TTLCURDAY ) == SUCCESS ) {
					ProgRptLog(22);
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_PLU_ADR, RPT_ONLY_RESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
            else
    		if ((CliParaMDCCheck(MDC_EOD4_ADR, EVEN_MDC_BIT1)) &&            /* Execute PLU Optimize, V1.0.15 */
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
		    
			/* execute PTD PLU optimize at EOD, 10/11/01 */
		    if ((CliParaMDCCheck(MDC_EOD4_ADR, EVEN_MDC_BIT1)) &&            /* Execute PLU Optimize V1.0.15 */
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
            
            break;

        case PROGRPT_FILE_CPN:
            if ( fExecuteEOD ) {
                if ( ! RptEODChkFileExist( FLEX_CPN_ADR )) {
                    break;
                }
                if ( ProgRpt_CpnReset( CLASS_TTLCURDAY ) == SUCCESS ) {
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_CPN_ADR, RPT_ONLY_RESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_CAS:
            if ( fExecuteEOD ) {
                if ( ! RptEODChkFileExist( FLEX_CAS_ADR )) {
                    break;
                }
				ProgRptLog(23);
                if ( ProgRpt_CasReset( CLASS_TTLCURDAY, ( USHORT )PROGRPT_ALLRESET ) == SUCCESS ) {
					ProgRptLog(24);
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_CAS_ADR, RPT_ONLY_RESET );
                        ProgRpt_CasPrintResetCasID( uchProgRptNo, PROGRPT_ALLRESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_WAI:
            break;

        case PROGRPT_FILE_FIN:
            if ( fExecuteEOD ) {
				ProgRptLog(25);
                if ( ProgRpt_FinReset( CLASS_TTLCURDAY ) == SUCCESS ) {
					ProgRptLog(26);
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_FINANC_ADR, RPT_ONLY_RESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_INDFIN:
            if ( fExecuteEOD ) {
				ProgRptLog(27);
                if ( ProgRpt_IndReset( RPT_EOD_ALLRESET ,
                                       ( USHORT )PROGRPT_ALLRESET ) == SUCCESS ) {
					ProgRptLog(28);
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_FINANC_ADR, RPT_ONLY_RESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_HOUR:
            if ( fExecuteEOD ) {
				ProgRptLog(29);
                if ( ProgRpt_HourReset( CLASS_TTLCURDAY ) == SUCCESS ) {
					ProgRptLog(30);
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_HOUR_ADR, RPT_ONLY_RESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_SERTIME:
            if ( fExecuteEOD ) {
                if ( ! RptPTDChkFileExist( FLEX_GC_ADR )) {
                    break;
                }
                if ( ProgRpt_TimeReset( CLASS_TTLCURDAY ) == SUCCESS ) {
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_SERVICE_ADR, RPT_ONLY_RESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_GCF:
            if ( fExecuteEOD ) {
				ProgRptLog(31);
                if (( RptEODChkFileExist( FLEX_GC_ADR )) &&
                    ( RptGuestResetByGuestCheck ( RPT_EOD_ALLRESET, ( USHORT )PROGRPT_ALLRESET ) == SUCCESS )) {
					ProgRptLog(32);
                    if ( uchReportType == RPT_ONLY_RESET ) {
                        ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_GCF_ADR, RPT_ONLY_RESET );
                    }
                    fTtlResetted = TRUE;
                }
            }
            break;

        case PROGRPT_FILE_EJALL:
        default:
            /* --- EJ cluster reset is executed at ProgRpt_EODPrint() with
                RPT_PRT_RESET(reset & print) type --- */
            break;
        }
        usLoopCo++;
    }
}

/*
*==========================================================================
**  Synopsis:   BOOL ProgRpt_EODIsResettable( PROGRPT_EODPTDIF *paEODRec,
*                                             USHORT    usNoOfRpt )
*
*   Input   :   PROGRPT_EODPTDIF *paEODRec  - points to array of EOD info.
*               USHORT           usNoOfRpt  - number of EOD report
*   Output  :   Nothing
*   InOut   :   Nothing
*
**  Return  :   BOOL    TRUE    - current total is resettable.
*                       FALSE   - current total is not resettable.
*   
**  Description:
*    This function detemines EOD reset report is resettable or not.
*   If saved total file is not issued, current total is not resettable.
*==========================================================================
*/
BOOL ProgRpt_EODIsResettable( PROGRPT_EODPTDIF *paEODRec,
                              USHORT    usNoOfRpt )
{
    USHORT  usLoopCo;       /* index of EOD report information for loop */
    BOOL    fTtlResettable; /* boolean value for total is resettable or not */
    BOOL    fExecuteEOD;    /* indicate execute at EOD report */
    UCHAR   uchProgRptNo;   /* target programmable report no. */

    ASSERT( paEODRec != ( PROGRPT_EODPTDIF * )NULL );
    ASSERT(( 0 < usNoOfRpt ) && ( usNoOfRpt <= MAX_PROGRPT_EODPTD ));

    fTtlResettable = TRUE;
    usLoopCo = 0;

    while (( usLoopCo < usNoOfRpt ) && fTtlResettable ) {

        uchProgRptNo = ( UCHAR )(( paEODRec + usLoopCo )->usReportNo );
        if ( MAX_PROGRPT_FORMAT < uchProgRptNo ) {
            fExecuteEOD   = FALSE;
        } else {
            fExecuteEOD   = TRUE;
        }

        switch (( paEODRec + usLoopCo )->usFileType ) {

        case PROGRPT_FILE_DEPT:
            if ( fExecuteEOD ) {
                if ( ! RptEODChkFileExist( FLEX_DEPT_ADR )) {
                    break;
                }
				ProgRptLog(6);
                if ( ! ProgRpt_DeptIsResettable( CLASS_TTLCURDAY )) {
					ProgRptLog(7);
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_DEPT_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_PLU:
            if ( fExecuteEOD ) {
                if (( ! RptEODChkFileExist( FLEX_DEPT_ADR )) ||
                    ( ! RptEODChkFileExist( FLEX_PLU_ADR ))) {
                    break;
                }
				ProgRptLog(8);
                if ( ! ProgRpt_PluIsResettable( CLASS_TTLCURDAY )) {
					ProgRptLog(9);
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_PLU_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_CPN:
            if ( fExecuteEOD ) {
                if ( ! RptEODChkFileExist( FLEX_CPN_ADR )) {
                    break;
                }
                if ( ! ProgRpt_CpnIsResettable( CLASS_TTLCURDAY )) {
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_CPN_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_CAS:
            if ( fExecuteEOD ) {
                if ( ! RptEODChkFileExist( FLEX_CAS_ADR )) {
                    break;
                }
				ProgRptLog(10);
                if ( ! ProgRpt_CasIsResettable( CLASS_TTLCURDAY, ( USHORT )PROGRPT_ALLRESET )) {
					ProgRptLog(11);
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_CAS_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_WAI:
            break;

        case PROGRPT_FILE_FIN:
            if ( fExecuteEOD ) {
				ProgRptLog(12);
                if ( ! ProgRpt_FinIsResettable( CLASS_TTLCURDAY )) {
					ProgRptLog(13);
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_FINANC_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_INDFIN:
            if ( fExecuteEOD ) {
				ProgRptLog(14);
                if ( ! ProgRpt_IndIsResettable(( USHORT )PROGRPT_ALLRESET )) {
					ProgRptLog(15);
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_FINANC_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_HOUR:
            if ( fExecuteEOD ) {
				ProgRptLog(16);
                if ( ! ProgRpt_HourIsResettable( CLASS_TTLCURDAY )) {
					ProgRptLog(17);
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_HOUR_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

        case PROGRPT_FILE_SERTIME:
            if ( fExecuteEOD ) {
                if ( ! RptPTDChkFileExist( FLEX_GC_ADR )) {
                    break;
                }
                if ( ! ProgRpt_TimeIsResettable( CLASS_TTLCURDAY )) {
                    ProgRpt_PrintHeader( CLASS_TTLSAVDAY, uchProgRptNo, RPT_SERVICE_ADR, RPT_READ_REPORT );
                    fTtlResettable = FALSE;
                }
            }
            break;

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

/* ===== End of File (_PROGEOD.C) ===== */