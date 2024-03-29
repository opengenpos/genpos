/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : EOD Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application          
* Program Name: RPTEOD.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               RptEODRead()        : Read EOD Report Module
*               RptEODReset()       : Reset EOD Report Module
*               RptEODPTDLock()     : Lock for EOD/PTD
*               RptEODPTDUnLock()   : Unlock for EOD/PTD
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-10-92: 00.00.01   : K.You     : initial                                   
* Dec-07-92: 01.00.00   : K.you     : Del <rfl.h>                                   
* Jun-02-93: 01.00.12   : J.Ikeda   : RptEODPTDLock()- SerGusCheckExist()
*                                   : return value maintenance                                   
* Jul-22-93: 01.00.12   : M.Suzuki  : Move RptEODChkFileExist() to rptcom.c.
* Dec-06-95: 03.01.00   : M.Ozawa   : Add Individual Financial/Service Time Reset
* Aug-11-99: 03.05.00   : M.Teraki  : R3.5 remove WAITER_MODEL
* Aug-12-99: 03.05.00   : K.Iwata   : Marge GUEST_CHECK_MODEL,STORE_RECAL_MODEL
* Dec-11-99: 01.00.00   : hrkato    : Saratoga
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/
/*
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/

#include	<tchar.h>
#include <string.h>
#include <stdio.h>

#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <maint.h>
#include <csop.h>
#include <cswai.h>
#include <cscas.h>
#include <csgcs.h>
#include <csttl.h>
#include <csstbopr.h>
#include <csstbstb.h>
#include <csstbwai.h>
#include <csstbcas.h>
#include <csstbgcf.h>
#include <csstbttl.h>
#include <report.h>
#include <mldsup.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <plu.h>
#include <..\Total\TOTALUM\ttltum.h>
#include <nb.h>
#include "rptcom.h"
#include "PluTtlD.H"
#include <ttl.h>


/*
*=============================================================================
**  Synopsis: SHORT RptEODRead( UCHAR uchType ) 
*               
*       Input:  uchType
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*
**  Description: Read EOD Report.  
*===============================================================================
*/

SHORT RptEODRead( UCHAR uchType ) 
{
    SHORT       sReturn;
	FILE        *fpFile = NULL;
    TCHAR       auchPLUNumber[PLU_MAX_DIGIT+1];

    memset(auchPLUNumber, 0, sizeof(auchPLUNumber));

    /* Print Header Name */
    if (uchType == ( UCHAR)RPT_TYPE_ONLY_PRT ) {                /* Only Print Case */
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Count Up Consecutive No. */
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class          */              
                        RPT_ACT_ADR,                            /* Report Name Address       */
                        RPT_EOD_ADR,                            /* Report Name Address       */
                        0,                                      /* Special Mnemonics Address */
                        0,                                      /* Report Name Address       */
                        uchType,                                /* Reset Type Print          */
                        AC_EOD_RPT,                             /* A/C Number                */
                        0,                                      /* Reset Type                */
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */
    }
   

	fpFile = ItemOpenHistorialReportsFolder (RPTREGFIN_FOLDER_ANY, AC_EOD_RPT, CLASS_TTLSAVDAY, RPT_EOD_ADR, 0, 0, 0);
    NHPOS_ASSERT_TEXT(fpFile != NULL, "**ERROR: RptEODRead() ItemOpenHistorialReportsFolder() failed.");
    if (!fpFile) {
        return -1;
    }

    RptDescriptionSet(RptDescriptionCreate(AC_EOD_RPT, CLASS_TTLSAVDAY, RPT_EOD_ADR, fpFile, RPTREGFIN_FOLDER_ANY, RPTREGFIN_OUTPUT_HTML, RptElementStream));
    uchUifACRptOnOffMld = RPT_DISPLAY_STREAM;

    do {     // do once to allow break if an error is seen during processing.

		sReturn = SUCCESS;
		/* Check to Execute Individual Financial Report */
		if (!CliParaMDCCheck(MDC_EOD3_ADR, ODD_MDC_BIT0)) {             /* Execute Individual Financial Report */
			ItemGenerateAc23Report (CLASS_TTLINDFIN, CLASS_TTLSAVDAY, RPT_EOD_ALLREAD, fpFile, 0);
			if ((sReturn = RptIndFinDayRead(CLASS_TTLSAVDAY, RPT_EOD_ALLREAD, 0, 0)) == RPT_ABORTED) {     /* Aborted by User */
				break;
			}
		}

		/* Check to Execute Register Financial Report */
		if (!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT0)) {             /* Execute Register Financial Report */
			ItemGenerateAc23Report (CLASS_TTLREGFIN, CLASS_TTLSAVDAY, RPT_EOD_ALLREAD, fpFile, 0);
			if ((sReturn = RptRegFinDayRead(CLASS_TTLSAVDAY, RPT_EOD_ALLREAD)) == RPT_ABORTED) {     /* Aborted by User */
				break;
			}
		}

		/* Check to Execute Department Report */
		if ((!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT1)) &&           /* Execute Department Report */
			(RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST)) {    /* File Exist */
			RptInitialize();                                            /* Clear Report Work */
			if ((sReturn = RptDEPTRead(CLASS_TTLSAVDAY, RPT_EOD_ALLREAD, 0)) == RPT_ABORTED) {                     /* Aborted by User */
				break;
			}
		}

		/* Check to Execute PLU Report */
		if ((!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT2)) &&           /* Execute PLU Report */
			(RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
			(RptEODChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {     /* File Exist */
			RptInitialize();                                            /* Clear Report Work */
			if ((sReturn = RptPLUDayRead(CLASS_TTLSAVDAY, RPT_EOD_ALLREAD, 0, 0, auchPLUNumber)) == RPT_ABORTED) {          /* Aborted by User */
				break;
			}
		}

		/* Check to Execute Hourly Activity Report */
		if (!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT3)) {             /* Execute Hourly Activity Report */
			if ((sReturn = RptHourlyDayRead(CLASS_TTLSAVDAY, RPT_EOD_ALLREAD )) == RPT_ABORTED) {    /* Aborted by User */
				break;
			}
		}

		/* Check to Execute Service Time Report */
		if (!CliParaMDCCheck(MDC_EOD3_ADR, ODD_MDC_BIT1) &&             /* Execute Service Time Report */
			(RptPTDChkFileExist(FLEX_GC_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
			if ((sReturn = RptServiceDayRead(CLASS_TTLSAVDAY, RPT_EOD_ALLREAD )) == RPT_ABORTED) {    /* Aborted by User */
				break;
			}
		}

		/* Check to Cashier Report */
		if ((!CliParaMDCCheck(MDC_EOD2_ADR, EVEN_MDC_BIT0)) &&          /* Execute Cashier Report */
			(RptEODChkFileExist(FLEX_CAS_ADR) == RPT_FILE_EXIST)) {     /* File Exist */
			RptInitialize();                                            /* Clear Report Work */
			ItemGenerateAc21Report (CLASS_TTLCASHIER, CLASS_TTLSAVDAY, RPT_EOD_ALLREAD, fpFile, 0);
			if ((sReturn = RptCashierRead(CLASS_TTLSAVDAY, RPT_EOD_ALLREAD, 0, 0)) == RPT_ABORTED) {  /* Aborted by User */
				break;
			}
		}

		/* Check to Coupon Report */
		if ((!CliParaMDCCheck(MDC_EOD2_ADR, EVEN_MDC_BIT3)) &&          /* Execute Waiter Report */
			(RptEODChkFileExist(FLEX_CPN_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
			RptInitialize();                                            /* Clear Report Work */
            ItemGenerateAc30Report(CLASS_TTLCPN, CLASS_TTLSAVDAY, RPT_EOD_ALLREAD, fpFile);
            if ((sReturn = RptCpnRead(CLASS_TTLSAVDAY, RPT_EOD_ALLREAD, 0)) == RPT_ABORTED) { /* Aborted by User */
				break;
			}
		}
	} while (0);   // do loop only once to allow break if an error is seens

	ItemCloseHistorialReportsFolder(fpFile);

	if (sReturn == RPT_ABORTED) return(SUCCESS);

    if ((uchType == ( UCHAR)RPT_TYPE_RST_PRT ) &&                 /* Reset & Print Case */
            (!CliParaMDCCheck(MDC_EOD3_ADR, ODD_MDC_BIT2))) {     /* Execute Cluster E/J Reset */
        RptInitialize();                                          /* Clear Report Work */
        sReturn = RptEJClusterReset();
        if (sReturn == PRT_ABORTED) {
			SerTtlTumUpdateDelayedBalance();
			//EOD catch totals up!
            return(SUCCESS);
        } else {
            /* Make Trailer */
            MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
        }
    } else {
        /* Make Trailer */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    }

	if(uchType == RPT_TYPE_RST_PRT)
	{
		SerTtlTumUpdateDelayedBalance();
		//EOD catch totals up!
	}
    
    if (uchType == ( UCHAR)RPT_TYPE_RST_PRT) {                  /* Reset and Print Case */
        if (CliParaMDCCheck(MDC_SUPER_ADR, ODD_MDC_BIT1)) {     /* Reset Consecutive Number */
			MaintResetSpcCo(SPCO_CONSEC_ADR);
        }
    }

    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptEODReset( UCHAR uchType ) 
*               
*       Input:  RPT_TYPE_ONLY_RST   : Only Print Type
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*
**  Description: Reset EOD Report.  
*===============================================================================
*/

SHORT RptEODReset( UCHAR uchType ) 
{
    SHORT   sError;

    /* Distingush Report Type */
    switch (uchType) {
    case ( UCHAR)RPT_TYPE_ONLY_RST:             /* Only Reset Type */
        sError = RptEODOnlyReset();                      
        break;

    default:                                    /* Print and Reset Type */
        sError = RptEODPrtReset();
        break;
    }
    return(sError);
}    
    
/*
*=============================================================================
**  Synopsis: SHORT RptEODOnlyReset( VOID ) 
*               
*       Input:  Nothing   
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*           LDT_PROHBT_ADR      : Prohibit Function
*
**  Description: Only Reset EOD Report.  
*===============================================================================
*/

SHORT RptEODOnlyReset( VOID ) 
{
    UCHAR       uchResetFlag = 0;
    ULONG       ausRcvBuffer[1];
    SHORT       sError;
    TTLREPORT   TtlReport;

    /* Print Header Name */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class          */              
                    RPT_ACT_ADR,                            /* Report Name Address       */
                    RPT_EOD_ADR,                            /* Report Name Address       */
                    0,                                      /* Special Mnemonics Address */
                    0,                                      /* Report Name Address       */
                    ( UCHAR)RPT_TYPE_ONLY_RST,              /* Reset Type                */
                    AC_EOD_RPT,                             /* A/C Number                */
                    0,                                      /* Reset Type                */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */
  
    /* Check Issued Report at Reset Mode */
    if ((sError = RptEODAllIssuedChk()) != SUCCESS) {               /* Not Issued Reset Report */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
        return(sError);                                                 
    }

	memset (&TtlReport, 0, sizeof(TtlReport));
    TtlReport.uchMajorClass = CLASS_TTLEODRESET;

    /* Check to Execute Individual Financial Report */
    if (!CliParaMDCCheck(MDC_EOD3_ADR, ODD_MDC_BIT0)) {             /* Execute Individual Financial Report */
        /* Print Individual Financial Header */
        RptEODPTDMakeHeader(AC_IND_READ_RPT, RPT_FINANC_ADR, SPC_DAIRST_ADR, 0);

        /* Reset Individual Financial Total File */
        if ((sError = RptIndFinDayReset(RPT_EOD_ALLRESET, RPT_ONLY_RESET, 0)) != SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "RptIndFinDayReset(RPT_EOD_ALLRESET): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
        }
    }

    /* Check to Execute Register Financial Report */
    if (!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT0)) {             /* Execute Register Financial Report */
        /* Print Register Financial Header */
        RptEODPTDMakeHeader(AC_REGFIN_READ_RPT, RPT_FINANC_ADR, SPC_DAIRST_ADR, 0);
        /* Reset Register Financial Total File */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_REGFIN_BIT)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlTotalReset(RPT_RESET_REGFIN_BIT): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
        }
    }

    /* Check to Execute Department Report */
    if ((!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT1)) &&            /* Execute Department Report */
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST)) {     /* File Exist */
		DEPTTBL     Table[FLEX_DEPT_MAX + 1] = {0};  // plus one to hold the RPT_EOF record indicating end of list.

		RptDEPTTableAll(CLASS_TTLSAVDAY, &Table[0]);            /* Saratoga */
		TtlWriteSavFile(CLASS_TTLDEPT,&Table[0],sizeof(Table),0);
    
        /* Reset Department Total File */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_DEPT_BIT)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlTotalReset(RPT_RESET_DEPT_BIT): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            /* Print Department File Header */
            RptEODPTDMakeHeader(AC_DEPTSALE_READ_RPT, RPT_DEPT_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
            RptPrtError(TtlConvertError(sError));
        } else {
            /* Print Department File Header */
            RptEODPTDMakeHeader(AC_DEPTSALE_READ_RPT, RPT_DEPT_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
        }
    }

    /* Check to Execute PLU Report */
    if ((!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT2)) &&            /* Execute PLU Report */
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
        (RptEODChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {      /* File Exist */

		TtlWriteSavFilePlu(CLASS_TTLSAVDAY);
    
		/* Reset PLU Total File */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_PLU_BIT)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlTotalReset(RPT_RESET_PLU_BIT): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            /* Print PLU File Header */
            RptEODPTDMakeHeader(AC_PLU_RESET_RPT, RPT_PLU_ADR, SPC_DAIRST_ADR, 0);
            RptPrtError(TtlConvertError(sError));
        } else {
			PLUIF  PluIf;

			/* Print PLU File Header */
            RptEODPTDMakeHeader(AC_PLU_RESET_RPT, RPT_PLU_ADR, SPC_DAIRST_ADR, 0);
            memset (&PluIf, 0, sizeof(PluIf));
            if (SerOpPluAllRead(&PluIf, 0) == OP_PERFORM) {             /* PLU Record or File Exists ? */
                uchResetFlag = 1;                                       /* Set Reset Completed Flag */                                
            }
        }
    } else if ((CliParaMDCCheck(MDC_EOD4_ADR, EVEN_MDC_BIT1)) &&            /* Execute PLU Optimize V1.0.15 */
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
        (RptEODChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
        
        /* Optimize PLU Total File */
        if ((sError = SerTtlPLUOptimize(CLASS_TTLCURDAY)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlPLUOptimize(CLASS_TTLCURDAY): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

        
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

            RptPrtError(TtlConvertError(sError));
        } else {
			PLUIF  PluIf;

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

            memset (&PluIf, 0, sizeof(PluIf));
            if (SerOpPluAllRead(&PluIf, 0) == OP_PERFORM) {             /* PLU Record or File Exists ? */
                uchResetFlag = 1;                                       /* Set Reset Completed Flag */                                
            }
        }
	}
	/* execute PTD PLU optimize at EOD, 10/11/01 */
    if ((CliParaMDCCheck(MDC_EOD4_ADR, EVEN_MDC_BIT1)) &&            /* Execute PLU Optimize V1.0.15 */
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
        (RptPTDChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {      /* File Exist */

        /* Optimize PLU Total File */
        if ((sError = SerTtlPLUOptimize(CLASS_TTLCURPTD)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlPLUOptimize(CLASS_TTLCURPTD): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

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
			PLUIF  PluIf;

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

            memset (&PluIf, 0, sizeof(PluIf));
            if (SerOpPluAllRead(&PluIf, 0) == OP_PERFORM) {             /* PLU Record or File Exists ? */
                uchResetFlag = 1;                                       /* Set Reset Completed Flag */                                
            }
        }
    }
    
    /* Check to Execute Hourly Activity Report */
    if ((!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT3)) && RptHourGetBlock()) {  /* Execute Hourly Activity Report */
        /* Print Hourly Activity File Header */
        RptEODPTDMakeHeader(AC_HOURLY_READ_RPT, RPT_HOUR_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
        /* Reset Hourly Activity Total Report */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_HOURLY_BIT)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlTotalReset(RPT_RESET_HOURLY_BIT): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
        }
    }    

    /* Check to Execute Service Time Report */
    if ((!CliParaMDCCheck(MDC_EOD3_ADR, ODD_MDC_BIT1)) && RptHourGetBlock()) {  /* Execute Service Time Report */
        /* Print Service Time File Header */
        RptEODPTDMakeHeader(AC_SERVICE_READ_RPT, RPT_SERVICE_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
        /* Reset Service Time Total Report */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_SERVICE_BIT)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlTotalReset(RPT_RESET_SERVICE_BIT): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
        }
    }    

    /* Check to Cashier Report */
    if ((!CliParaMDCCheck(MDC_EOD2_ADR, EVEN_MDC_BIT0)) &&          /* Execute Cashier Report */
        (RptEODChkFileExist(FLEX_CAS_ADR) == RPT_FILE_EXIST)) {     /* File Exist */
        /* Reset Cashier Total Report */
        if ((sError = RptCashierReset(CLASS_TTLCURDAY, RPT_EOD_ALLRESET, RPT_ONLY_RESET, 0)) != SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "RptCashierReset(CLASS_TTLCURDAY): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            /* Print Cashier File Header and Error */
            if (sError != RPT_RESET_FAIL) {
                RptEODPTDMakeHeader(AC_CASH_RESET_RPT, RPT_CAS_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
                RptPrtError(sError);
            }
        } else {
            if (sError == SUCCESS) {
                if (SerCasAllIdRead(sizeof(ausRcvBuffer), ausRcvBuffer) > 0) {
                    uchResetFlag = 1;                                   /* Set Reset Completed Flag */
                }
            }
        }
    }

    /* Execute Guest Check File Report */
    if ((!CliParaMDCCheck(MDC_EOD2_ADR, EVEN_MDC_BIT2)) &&          /* Execute GCF Report */
        (RptEODChkFileExist(FLEX_GC_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
        if ((sError = RptGuestResetByGuestCheck (RPT_EOD_ALLRESET, 0)) != SUCCESS && (sError != LDT_NTINFL_ADR)) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "RptGuestResetByGuestCheck(RPT_EOD_ALLRESET): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            RptPrtError(sError);
        } else {
            if (sError == SUCCESS) {
                uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
            }
        }
    }

    /* Check to Coupon Report */
    if ((!CliParaMDCCheck(MDC_EOD2_ADR, EVEN_MDC_BIT3)) &&          /* Execute Coupon Report */
        (RptEODChkFileExist(FLEX_CPN_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
        RptEODPTDMakeHeader(AC_CPN_RESET_RPT, RPT_CPN_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);

        /* Reset Coupon Total Report */
		TtlSavCpnSavFile(0, 0);

        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_COUPON_BIT)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlTotalReset(RPT_RESET_COUPON_BIT): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            RptPrtError(sError);
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */
        }
    }

    /* Count Up Reset Counter and Reset Consecutive Counter */
    if (uchResetFlag) {                                     /* Reset Completed Case */                               
        MaintIncreSpcCo(SPCO_EODRST_ADR);
    }

    /* Make Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

    if (CliParaMDCCheck(MDC_SUPER_ADR, ODD_MDC_BIT1)) { /* Reset Consecutive Number */
		MaintResetSpcCo(SPCO_CONSEC_ADR);
    }

 	TtlCloseSavFile();

	//SR871 Allow AC99 Type 3 to reset Delay Balance JHHJ
	SerTtlTumUpdateDelayedBalance();

    return(SUCCESS);
}    
    
/*
*=============================================================================
**  Synopsis: SHORT RptEODPrtReset( VOID ) 
*               
*       Input:  Nothing   
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*           LDT_PROHBT_ADR      : Prohibit Function
*
**  Description: Reset and Print EOD Report.  
*===============================================================================
*/

SHORT RptEODPrtReset( VOID ) 
{

    UCHAR       uchResetFlag = 0;
    ULONG       ausRcvBuffer[1];
    SHORT       sError;
    TTLCSREPORTHEAD   TtlReport;

    /* Print Header Name */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Increment Consecutive Counter */
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class          */              
                    RPT_ACT_ADR,                            /* Report Name Address       */
                    RPT_EOD_ADR,                            /* Report Name Address       */
                    0,                                      /* Special Mnemonics Address */
                    0,                                      /* Report Name Address       */
                    ( UCHAR)RPT_TYPE_RST_PRT,               /* Reset Type                */
                    AC_EOD_RPT,                             /* A/C Number                */
                    0,                                      /* Reset Type                */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */
  
    /* Check Issued Report at Reset Mode */
    if ((sError = RptEODAllIssuedChk()) != SUCCESS) {               /* Not Issued Reset Report */
		UCHAR  xBuff[128];
		sprintf(xBuff, "RptEODAllIssuedChk(): sError = %d", sError);
		NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
        return(sError);                                                 
    }

	memset (&TtlReport, 0, sizeof(TtlReport));
    TtlReport.Rpt.uchMajorClass = CLASS_TTLEODRESET;

    /* Check to Execute Individual Financial Report */
    if (!CliParaMDCCheck(MDC_EOD3_ADR, ODD_MDC_BIT0)) {             /* Execute Individual Financial Report */
        /* Reset Individual Financial Total File */
        if ((sError = RptIndFinDayReset(RPT_EOD_ALLRESET, RPT_PRT_RESET, 0)) != SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "RptIndFinDayReset(): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            /* Print Individual Financial Header and Error */
            RptEODPTDMakeHeader(AC_IND_READ_RPT, RPT_FINANC_ADR, SPC_DAIRST_ADR, 0);
            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */
        }
    }

    /* Check to Execute Register Financial Report and Execute Register Financial Report */
    if (!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT0)) {
        /* Reset Register Financial Total File */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_REGFIN_BIT)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlTotalReset(RPT_RESET_REGFIN_BIT): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            /* Print Register Financial Header and Error */
            RptEODPTDMakeHeader(AC_REGFIN_READ_RPT, RPT_FINANC_ADR, SPC_DAIRST_ADR, 0);
            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
        }
    }

    /* Check to Execute Department Report and Execute Department Report */
    if ((!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT1)) && (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST)) {     /* File Exist */
        /* Reset Department Total File */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_DEPT_BIT)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlTotalReset(RPT_RESET_DEPT_BIT): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            /* Print Department File Header and Error */
            RptEODPTDMakeHeader(AC_DEPTSALE_READ_RPT, RPT_DEPT_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
            RptPrtError(TtlConvertError(sError));
        } else {
            if (sError == TTL_SUCCESS) {
                uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
            }
        }
    }

    /* Check to Execute PLU Report */
    if ((!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT2)) &&            /* Execute PLU Report */
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
        (RptEODChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {      /* File Exist */

        /* Reset PLU Total File */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_PLU_BIT)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlTotalReset(RPT_RESET_PLU_BIT): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            /* Print PLU File Header and Error */
            RptEODPTDMakeHeader(AC_PLU_RESET_RPT, RPT_PLU_ADR, SPC_DAIRST_ADR, 0);
            RptPrtError(TtlConvertError(sError));
        } else {
            if (sError == TTL_SUCCESS) {
				PLUIF  PluIf;

                memset (&PluIf, 0, sizeof(PluIf));
                if (SerOpPluAllRead(&PluIf, 0) == OP_PERFORM) {             /* PLU Record or File Exists ? */
                    uchResetFlag = 1;                                       /* Set Reset Completed Flag */                                
                }
            }
        }
    } else if ((CliParaMDCCheck(MDC_EOD4_ADR, EVEN_MDC_BIT1)) &&            /* Execute PLU Optimize, V1.0.15 */
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
        (RptEODChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {      /* File Exist */

        /* Optimize PLU Total File */
        if ((sError = SerTtlPLUOptimize(CLASS_TTLCURDAY)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlPLUOptimize(CLASS_TTLCURDAY): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);
        
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

            RptPrtError(TtlConvertError(sError));
        } else {
			PLUIF  PluIf;

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

            memset (&PluIf, 0, sizeof(PluIf));
            if (SerOpPluAllRead(&PluIf, 0) == OP_PERFORM) {             /* PLU Record or File Exists ? */
                uchResetFlag = 1;                                       /* Set Reset Completed Flag */                                
            }
        }
    }
	/* execute PTD PLU optimize at EOD, 10/11/01 */
    if ((CliParaMDCCheck(MDC_EOD4_ADR, EVEN_MDC_BIT1)) &&            /* Execute PLU Optimize V1.0.15 */
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
        (RptPTDChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {      /* File Exist */

        /* Optimize PLU Total File */
        if ((sError = SerTtlPLUOptimize(CLASS_TTLCURPTD)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlPLUOptimize(CLASS_TTLCURPTD): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

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
			PLUIF  PluIf;

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

            memset (&PluIf, 0, sizeof(PluIf));
            if (SerOpPluAllRead(&PluIf, 0) == OP_PERFORM) {             /* PLU Record or File Exists ? */
                uchResetFlag = 1;                                       /* Set Reset Completed Flag */                                
            }
        }
    }
    
    /* Check to Execute Hourly Activity Report */
    if ((!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT3)) && RptHourGetBlock()) {    /* Execute Hourly Activity Report */
        /* Reset Hourly Activity Total Report */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_HOURLY_BIT)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlTotalReset(RPT_RESET_HOURLY_BIT): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            /* Print Hourly Activity Header and Error */
            RptEODPTDMakeHeader(AC_HOURLY_READ_RPT, RPT_HOUR_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
        }
    }    

    /* Check to Execute Service Time Report */
    if ((!CliParaMDCCheck(MDC_EOD3_ADR, ODD_MDC_BIT1))
        && RptHourGetBlock()               /* Execute Service Time Report */
        && (RptPTDChkFileExist(FLEX_GC_ADR) == RPT_FILE_EXIST)) {      /* File Exist */

        /* Reset Service Time Total Report */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_SERVICE_BIT)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlTotalReset(RPT_RESET_SERVICE_BIT): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            /* Print Service Time Header and Error */
            RptEODPTDMakeHeader(AC_SERVICE_READ_RPT, RPT_SERVICE_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
        }
    }    

    /* Check to Cashier Report */
    if ((!CliParaMDCCheck(MDC_EOD2_ADR, EVEN_MDC_BIT0)) &&          /* Execute Cashier Report */
        (RptEODChkFileExist(FLEX_CAS_ADR) == RPT_FILE_EXIST)) {     /* File Exist */
        /* Reset Cashier Total Report */
        RptInitialize();                                            /* Clear Report Work */
        if ((sError = RptCashierReset(CLASS_TTLCURDAY, RPT_EOD_ALLRESET, RPT_PRT_RESET, 0)) != SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "RptCashierReset(CLASS_TTLCURDAY): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            /* Print Cashier File Header and Error */
            if (sError != RPT_RESET_FAIL) {
                RptEODPTDMakeHeader(AC_CASH_RESET_RPT, RPT_CAS_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
                RptPrtError(sError);
            }
        } else {
            if (sError == SUCCESS) {
                if (SerCasAllIdRead(sizeof(ausRcvBuffer), ausRcvBuffer) > 0) {
                    uchResetFlag = 1;                                   /* Set Reset Completed Flag */
                }
            }
        }
    }

    /* Execute Guest Check File Report */
    if ((!CliParaMDCCheck(MDC_EOD2_ADR, EVEN_MDC_BIT2)) &&          /* Execute GCF Report */
        (RptEODChkFileExist(FLEX_GC_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
        if ((sError = RptGuestResetByGuestCheck (RPT_EOD_ALLRESET, 0)) != SUCCESS && (sError != LDT_NTINFL_ADR)) {
            RptPrtError(sError);
        } else {
            if (sError == SUCCESS) {
                uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
            }
        }
    }

	/* Check to Coupon Report */
    if ((!CliParaMDCCheck(MDC_EOD2_ADR, EVEN_MDC_BIT3)) &&          /* Execute Coupon Report */
        (RptEODChkFileExist(FLEX_CPN_ADR) == RPT_FILE_EXIST)) {      /* File Exist */

            /* Reset Coupon Total Report */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_COUPON_BIT)) != TTL_SUCCESS) {
			UCHAR  xBuff[128];
			sprintf(xBuff, "SerTtlTotalReset(RPT_RESET_COUPON_BIT): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);

            RptEODPTDMakeHeader(AC_CPN_RESET_RPT, RPT_CPN_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
            RptPrtError(sError);
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */
        }
    }

 	TtlCloseSavFile();

    /* Count Up Reset Counter */
    if (uchResetFlag) {                                     /* Reset Completed Case */                               
        MaintIncreSpcCo(SPCO_EODRST_ADR);
    }
    /* else {
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        Make Trailer 
    } */

    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: VOID RptEODPTDMakeHeader( UCHAR uchACNoAddr,
*                                    UCHAR uchRptNameAddr,
*                                    UCHAR uchSpeAddr,
*                                    UCHAR uchType ) 
*               
*       Input:  uchACNoAddr     : A/C Number Address   
*               uchRptNameAddr  : Report Name Address
*               uchSpeAddr      : Special Name Address
*               uchType         : Report Type
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*
**  Description: Make Report Header for EOD/PTD Report.  
*===============================================================================
*/

VOID RptEODPTDMakeHeader( UCHAR uchACNoAddr,
                       UCHAR uchRptNameAddr,
                       UCHAR uchSpeAddr,
                       UCHAR uchType )
{
    /* Print Header Name */
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class */              
                    RPT_ACT_ADR,                            /* Report Name Address */
                    uchRptNameAddr,                         /* Report Name Address */
                    uchSpeAddr,                             /* Special Mnemonics Address */
                    RPT_RESET_ADR,                          /* Report Name Address */
                    uchType,                                /* Report Type */
                    uchACNoAddr,                            /* A/C Number */
                    0,                                      /* Reset Type                */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */

}

/*
*=============================================================================
**  Synopsis: SHORT RptEODAllIssuedChk( VOID ) 
*               
*       Input:  Nothing         
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*
**  Description: Check Issued Flag (Financial, DEPT, PLU, Hourly, Cashier, Waiter File).  
*===============================================================================
*/
SHORT RptEODAllIssuedChk( VOID ) 
{
    SHORT             sError;
    SHORT             sReturn;
    TTLCSREPORTHEAD   TtlReport;

	memset (&TtlReport, 0, sizeof(TtlReport));

    /* Check Individual Financial Issued Report Flag */
    TtlReport.Rpt.uchMajorClass = CLASS_TTLINDFIN;
    TtlReport.Rpt.uchMinorClass = CLASS_TTLSAVDAY;
    TtlReport.Rpt.ulNumber = 0;

    /* Check to Individual Register Financial Report and Execute Individual Financial Report*/
    if (!CliParaMDCCheck(MDC_EOD3_ADR, ODD_MDC_BIT0)) {
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
            /* Print Individual Financial Header and Error */
            RptEODPTDMakeHeader(AC_IND_READ_RPT, RPT_FINANC_ADR, SPC_DAIRST_ADR, 0);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }

    /* Check Register Financial Issued Report Flag */
    TtlReport.Rpt.uchMajorClass = CLASS_TTLREGFIN;
    TtlReport.Rpt.uchMinorClass = CLASS_TTLSAVDAY;
    TtlReport.Rpt.ulNumber = 0;

    /* Check to Execute Register Financial Report and Execute Register Financial Report*/
    if (!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT0)) {
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
            /* Print Register Financial Header and Error */
            RptEODPTDMakeHeader(AC_REGFIN_READ_RPT, RPT_FINANC_ADR, SPC_DAIRST_ADR, 0);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }

    /* Check to Execute Department Report and Execute Department Report*/
    if ((!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT1)) && (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST)) {     /* File Exist */
        /* Check Department Issued Report Flag */
        TtlReport.Rpt.uchMajorClass = CLASS_TTLDEPT;
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
            /* Print Department Header and Error */
            RptEODPTDMakeHeader(AC_DEPTSALE_READ_RPT, RPT_DEPT_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }    

    /* Check to Execute PLU Report and Execute PLU Report*/
    if ((!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT2)) &&
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) && (RptEODChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
        /* Check PLU Issued Report Flag */
        TtlReport.Rpt.uchMajorClass = CLASS_TTLPLU;
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
            /* Print PLU Header and Error */
            RptEODPTDMakeHeader(AC_PLU_RESET_RPT, RPT_PLU_ADR, SPC_DAIRST_ADR, 0);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }

    /* Check to Execute Hourly Activity Report and Execute Hourly Activity Report*/
    if ((!CliParaMDCCheck(MDC_EOD1_ADR, ODD_MDC_BIT3)) && RptHourGetBlock()) {
        /* Check Hourly Activity Issued Report Flag */
        TtlReport.Rpt.uchMajorClass = CLASS_TTLHOURLY;
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
            /* Print Hourly Activity and Error */
            RptEODPTDMakeHeader(AC_HOURLY_READ_RPT, RPT_HOUR_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }

    /* Check to Execute Service Time Report and Execute Service Time Report*/
    if ((!CliParaMDCCheck(MDC_EOD3_ADR, ODD_MDC_BIT1)) && RptHourGetBlock()) {
        /* Check Serice Time Issued Report Flag */
        TtlReport.Rpt.uchMajorClass = CLASS_TTLSERVICE;
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
            /* Print Service Time and Error */
            RptEODPTDMakeHeader(AC_SERVICE_READ_RPT, RPT_SERVICE_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }

    /* Check to Cashier Report and Execute Cashier Report*/
    if ((!CliParaMDCCheck(MDC_EOD2_ADR, EVEN_MDC_BIT0)) && (RptEODChkFileExist(FLEX_CAS_ADR) == RPT_FILE_EXIST)) {     /* File Exist */
        /* Check Cashier Issued Report Flag */
        TtlReport.Rpt.uchMajorClass = CLASS_TTLCASHIER;
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
            /* Print Cashier Header and Error */
            RptEODPTDMakeHeader(AC_CASH_RESET_RPT, RPT_CAS_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }

    /* Check to Coupon Report and Execute Coupon Report*/
    if ((!CliParaMDCCheck(MDC_EOD2_ADR, EVEN_MDC_BIT3)) && (RptEODChkFileExist(FLEX_CPN_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
        /* Check Coupon Issued Report Flag */
        TtlReport.Rpt.uchMajorClass = CLASS_TTLCPN;
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
            /* Print Coupon Header and Error */
            RptEODPTDMakeHeader(AC_CPN_RESET_RPT, RPT_CPN_ADR, SPC_DAIRST_ADR, RPT_ALL_RESET);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }

    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptEODPTDLock( UCHAR uchACNumber ) 
*               
*       Input:  uchAddress      : AC_EOD_RPT   
*                               : AC_PTD_RPT   
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*
**  Description: Lock for EOD/PTD Report.  
*===============================================================================
*/
SHORT RptEODPTDLock( UCHAR uchACNumber )
{
    USHORT   usAddress;
    SHORT   sError;
    SHORT   sReturn;

    /* Lock All Cashier File */
    if ((sError = SerCasAllLock()) != CAS_PERFORM && sError != CAS_FILE_NOT_FOUND) {
        return(LDT_PROHBT_ADR);
    }

    /* Check EOD or PTD Report */
    usAddress = MDC_PTD2_ADR;
    if (uchACNumber == AC_EOD_RPT) {
        usAddress = MDC_EOD2_ADR;
    }

    /* Execute Guest Check File Report */
    if (CliParaMDCCheck(usAddress, EVEN_MDC_BIT2) && !CliParaMDCCheck(MDC_EODPTD_ADR, EVEN_MDC_BIT0)) {     /* Do not Execute GCF Report */
        /* Check GC File Exist */
        if ((sReturn = SerGusCheckExist()) != GCF_SUCCESS && sReturn != GCF_FILE_NOT_FOUND) {   /* GCF Exist */
            SerCasAllUnLock();
            return(LDT_GCFNTEMPT_ADR);
        }
    }

    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptEODPTDUnLock( VOID ) 
*               
*       Input:  Nothing   
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*
**  Description: Unlock for EOD/PTD Report.  
*===============================================================================
*/

SHORT RptEODPTDUnLock(VOID)
{
    SerCasAllUnLock();
    return(SUCCESS);
}

/* --- End of Source File --- */