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
* Title       : PTD Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application          
* Program Name: RPTPTD.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               RptPTDRead()        : Read PTD Report Module
*               RptPTDReset()       : Reset PTD Report Module
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-10-92: 00.00.01   : K.You     : initial                                   
* Dec-07-92: 01.00.00   : K.you     : Del <rfl.h>                                   
* Jul-22-93: 01.00.12   : M.Suzuki  : Move RptPTDChkFileExist() to rptcom.c.
* Dec-06-95: 03.01.00   : M.Ozawa   : Add Service Time Reset
* Aug-11-99: 03.05.00   : M.Teraki  : remove WAITER_MODEL
* Aug-12-99: 03.05.00   : K.Iwata   : marge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
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
#include <plu.h>

#include "rptcom.h"

/*
*=============================================================================
**  Synopsis: SHORT RptPTDRead( UCHAR uchType ) 
*               
*       Input:  uchType
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*
**  Description: Read PTD Report.  
*===============================================================================
*/
SHORT RptPTDRead( UCHAR uchType ) 
{
	TCHAR       auchPLUNumber[PLU_MAX_DIGIT+1] = {0};
   
    /* Print Header Name */
    if (uchType == ( UCHAR)RPT_TYPE_ONLY_PRT ) {                /* Only Print Case */
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* Count Up Consecutive No. */
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class          */              
                        RPT_ACT_ADR,                            /* Report Name Address       */
                        RPT_PTD_ADR,                            /* Report Name Address       */
                        0,                                      /* Special Mnemonics Address */
                        0,                                      /* Report Name Address       */
                        uchType,                                /* Reset Type                */
                        AC_PTD_RPT,                             /* A/C Number                */
                        0,                                      /* Reset Type                */
                        PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */
    }
   
    /* Check to Execute Register Financial Report */
    if (!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT0)) {             /* Execute Register Financial Report */
        if (RptRegFinPTDRead(CLASS_TTLSAVPTD, RPT_PTD_ALLREAD) == RPT_ABORTED) {     /* Aborted by User */
            return(SUCCESS);
        }
    }

    /* Check to Execute PLU Report */
    if ((!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT2)) &&           /* Execute PLU Report */
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
        (RptPTDChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {     /* File Exist */
        RptInitialize();                                            /* Clear Report Work */
        if (RptPLUPTDRead(CLASS_TTLSAVPTD, RPT_PTD_ALLREAD, 0, 0, auchPLUNumber) == RPT_ABORTED) {          /* Aborted by User */
            return(SUCCESS);
        }
    }

	/* Check to Execute Department Report */
    if ((!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT1)) &&           /* Execute Department Report */
        (RptPTDChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST)) {    /* File Exist */
        RptInitialize();                                            /* Clear Report Work */
        if (RptDEPTRead(CLASS_TTLSAVPTD, RPT_PTD_ALLREAD, 0) == RPT_ABORTED) {                     /* Aborted by User */
            return(SUCCESS);
        }
    }


    /* Check to Execute Hourly Activity Report */
    if (!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT3)) {             /* Execute Hourly Activity Report */
        if (RptHourlyPTDRead(CLASS_TTLSAVPTD, RPT_PTD_ALLREAD ) == RPT_ABORTED) {    /* Aborted by User */
            return(SUCCESS);
        }
    }

    /* Check to Execute Service Time Report */
    if (!CliParaMDCCheck(MDC_PTD3_ADR, ODD_MDC_BIT1) &&             /* Execute Hourly Activity Report */
        (RptPTDChkFileExist(FLEX_GC_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
        if (RptServicePTDRead(CLASS_TTLSAVPTD, RPT_PTD_ALLREAD ) == RPT_ABORTED) {    /* Aborted by User */
            return(SUCCESS);
        }
    }


    /* Check to Cashier Report, V3.3 */
    if ((!CliParaMDCCheck(MDC_PTD2_ADR, EVEN_MDC_BIT0)) &&          /* Execute Cashier Report */
        (RptPTDChkFileExist(FLEX_CAS_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
        RptInitialize();                                            /* Clear Report Work */
        if (RptCashierRead(CLASS_TTLSAVPTD, RPT_PTD_ALLREAD, 0, 0) == RPT_ABORTED) {                      /* Aborted by User */
            return(SUCCESS);
        }
    }

    /* Check to Coupon Report */
    if ((!CliParaMDCCheck(MDC_PTD2_ADR, EVEN_MDC_BIT3)) &&          /* Execute Waiter Report */
        (RptPTDChkFileExist(FLEX_CPN_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
        RptInitialize();                                            /* Clear Report Work */
        if (RptCpnRead(CLASS_TTLSAVPTD, RPT_PTD_ALLREAD, 0) == RPT_ABORTED) {           /* Aborted by User */
            return(SUCCESS);
        }
    }

    /* Make Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

    if (uchType == ( UCHAR)RPT_TYPE_RST_PRT) {                  /* Reset and Print Case */
        if (CliParaMDCCheck(MDC_SUPER_ADR, ODD_MDC_BIT1)) {     /* Reset Consecutive Number */
			MaintResetSpcCo(SPCO_CONSEC_ADR);
        }
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptPTDReset( UCHAR uchType ) 
*               
*       Input:  RPT_TYPE_ONLY_RST   : Only Print Type
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*
**  Description: Reset PTD Report.  
*===============================================================================
*/

SHORT RptPTDReset( UCHAR uchType ) 
{
    SHORT   sError;

    /* Distingush Report Type */
    switch (uchType) {
    case ( UCHAR)RPT_TYPE_ONLY_RST:             /* Only Reset Type */
        sError = RptPTDOnlyReset();                      
        break;

    default:                                    /* Print and Reset Type */
        sError = RptPTDPrtReset();
        break;
    }
    return(sError);
}    
    
/*
*=============================================================================
**  Synopsis: SHORT RptPTDOnlyReset( VOID ) 
*               
*       Input:  Nothing   
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*           LDT_PROHBT_ADR      : Prohibit Function
*
**  Description: Only Reset PTD Report.  
*===============================================================================
*/

SHORT RptPTDOnlyReset( VOID ) 
{
    ULONG       ausRcvBuffer[1];
    SHORT       sError;
    UCHAR       uchResetFlag = 0;
	TTLREPORT   TtlReport = {0};

    /* Print Header Name */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class          */              
                    RPT_ACT_ADR,                            /* Report Name Address       */
                    RPT_PTD_ADR,                            /* Report Name Address       */
                    0,                                      /* Special Mnemonics Address */
                    0,                                      /* Report Name Address       */
                    ( UCHAR)RPT_TYPE_ONLY_RST,              /* Reset Type                */
                    AC_PTD_RPT,                             /* A/C Number                */
                    0,                                      /* Reset Type                */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */
 
  
    /* Check Issued Report at Reset Mode */
    if ((sError = RptPTDAllIssuedChk()) != SUCCESS) {               /* Not Issued Reset Report */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
        return(sError);
    }

    TtlReport.uchMajorClass = CLASS_TTLPTDRESET;

    /* Check to Execute Register Financial Report */
    if (!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT0)) {             /* Execute Register Financial Report */

        /* Print Register Financial Header */
        RptEODPTDMakeHeader(AC_REGFIN_READ_RPT, RPT_FINANC_ADR, SPC_PTDRST_ADR, 0);

        /* Reset Register Financial Total File */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_REGFIN_BIT)) != TTL_SUCCESS) {
            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
        }
    }

    /* Check to Execute PLU Report */
    if ((!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT2)) &&           /* Execute PLU Report */
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
        (RptPTDChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {     /* File Exist */
    
        /* Reset PLU Total File */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_PLU_BIT)) != TTL_SUCCESS) {
            /* Print PLU File Header */
            RptEODPTDMakeHeader(AC_PLU_RESET_RPT, RPT_PLU_ADR, SPC_PTDRST_ADR, 0);
            RptPrtError(TtlConvertError(sError));
        } else {
			PLUIF  PluIf = {0};
    
            /* Print PLU File Header */
            RptEODPTDMakeHeader(AC_PLU_RESET_RPT, RPT_PLU_ADR, SPC_PTDRST_ADR, 0);
            if (SerOpPluAllRead(&PluIf, 0) == OP_PERFORM) {             /* PLU Record or File Exists ? */
                uchResetFlag = 1;                                       /* Set Reset Completed Flag */                                
            }
        }
    } else if ((CliParaMDCCheck(MDC_PTD4_ADR, EVEN_MDC_BIT1)) &&            /* Execute PLU Optimize V1.0.15 */
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
			PLUIF  PluIf = {0};
    
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

            if (SerOpPluAllRead(&PluIf, 0) == OP_PERFORM) {             /* PLU Record or File Exists ? */
                uchResetFlag = 1;                                       /* Set Reset Completed Flag */                                
            }
        }
    }
	/* execute Daily PLU optimize at PTD, 10/11/01 */
    if ((CliParaMDCCheck(MDC_PTD4_ADR, EVEN_MDC_BIT1)) &&            /* Execute PLU Optimize V1.0.15 */
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
        (RptEODChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {      /* File Exist */

        /* Optimize PLU Total File */
        if ((sError = SerTtlPLUOptimize(CLASS_TTLCURDAY)) != TTL_SUCCESS) {
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
			PLUIF  PluIf = {0};
    
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

            if (SerOpPluAllRead(&PluIf, 0) == OP_PERFORM) {             /* PLU Record or File Exists ? */
                uchResetFlag = 1;                                       /* Set Reset Completed Flag */                                
            }
        }
    }

    /* Check to Execute Department Report */

    if ((!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT1)) &&           /* Execute Department Report */
        (RptPTDChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST)) {    /* File Exist */
    
        /* Reset Department Total File */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_DEPT_BIT)) != TTL_SUCCESS) {
            /* Print Department File Header */
            RptEODPTDMakeHeader(AC_DEPTSALE_READ_RPT, RPT_DEPT_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);
            RptPrtError(TtlConvertError(sError));
        } else {
            /* Print Department File Header */
            RptEODPTDMakeHeader(AC_DEPTSALE_READ_RPT, RPT_DEPT_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
        }
    }
    
    /* Check to Execute Hourly Activity Report */
    if ((!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT3)) && RptHourGetBlock()) {             /* Execute Hourly Activity Report */
        /* Print Hourly Activity File Header */
        RptEODPTDMakeHeader(AC_HOURLY_READ_RPT, RPT_HOUR_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);

        /* Reset Hourly Activity Total Report */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_HOURLY_BIT)) != TTL_SUCCESS) {
            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
        }
    }

    /* Check to Execute Service Time Report */
    if ((!CliParaMDCCheck(MDC_PTD3_ADR, ODD_MDC_BIT1)) && RptHourGetBlock()) {             /* Execute Service Time Report */
        /* Print Service Time File Header */
        RptEODPTDMakeHeader(AC_SERVICE_READ_RPT, RPT_SERVICE_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);

        /* Reset Service Time Total Report */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_SERVICE_BIT)) != TTL_SUCCESS) {
            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
        }
    }

    /* Execute Guest Check File Report */
    if ((!CliParaMDCCheck(MDC_PTD2_ADR, EVEN_MDC_BIT2)) &&          /* Execute GCF Report */
        (RptEODChkFileExist(FLEX_GC_ADR) == RPT_FILE_EXIST)) {  /* File Exist Use RptEODChkFileExist() */      
        if ((sError = RptGuestResetByGuestCheck (RPT_PTD_ALLRESET, 0)) != SUCCESS && (sError != LDT_NTINFL_ADR)) {
            RptPrtError(sError);
        } else {
            if (sError == SUCCESS) {
                uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
            }
        }
    }

    /* Check to Cashier Report, V3.3 */
    if ((!CliParaMDCCheck(MDC_PTD2_ADR, EVEN_MDC_BIT0)) &&          /* Execute Cashier Report */
        (RptPTDChkFileExist(FLEX_CAS_ADR) == RPT_FILE_EXIST)) {      /* File Exist */

        /* Reset Cashier Total Report */
        if ((sError = RptCashierReset(CLASS_TTLCURPTD, RPT_PTD_ALLRESET, RPT_ONLY_RESET, 0)) != SUCCESS) {

            /* Print Cashier File Header and Error */
            if (sError != RPT_RESET_FAIL) {
                RptEODPTDMakeHeader(AC_CASH_RESET_RPT, RPT_CAS_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);
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

    /* Check to Coupon Report */
    if ((!CliParaMDCCheck(MDC_PTD2_ADR, EVEN_MDC_BIT3)) &&          /* Execute Coupon Report */
        (RptPTDChkFileExist(FLEX_CPN_ADR) == RPT_FILE_EXIST)) {      /* File Exist */

        RptEODPTDMakeHeader(AC_CPN_RESET_RPT, RPT_CPN_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);

        /* Reset Coupon Total Report */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_COUPON_BIT)) != TTL_SUCCESS) {
                RptPrtError(sError);
        } else {
                uchResetFlag = 1;                                   /* Set Reset Completed Flag */
        }
    }

    if (uchResetFlag) {                                     /* Reset Completed Case */                               
        MaintIncreSpcCo(SPCO_PTDRST_ADR);
    }

    /* Make Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);                    

    if (CliParaMDCCheck(MDC_SUPER_ADR, ODD_MDC_BIT1)) { /* Reset Consecutive Number */
		MaintResetSpcCo(SPCO_CONSEC_ADR);
    }

    return(SUCCESS);
}    
    
/*
*=============================================================================
**  Synopsis: SHORT RptPTDPrtReset( VOID ) 
*               
*       Input:  Nothing   
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*           LDT_PROHBT_ADR      : Prohibit Function
*
**  Description: Reset and Print PTD Report.  
*===============================================================================
*/

SHORT RptPTDPrtReset( VOID ) 
{
    ULONG       ausRcvBuffer[1];
    SHORT       sError;
    UCHAR       uchResetFlag = 0;
	TTLREPORT   TtlReport = {0};

    /* Print Header Name */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class          */              
                    RPT_ACT_ADR,                            /* Report Name Address       */
                    RPT_PTD_ADR,                            /* Report Name Address       */
                    0,                                      /* Special Mnemonics Address */
                    0,                                      /* Report Name Address       */
                    ( UCHAR)RPT_TYPE_RST_PRT,               /* Reset Type                */
                    AC_PTD_RPT,                             /* A/C Number                */
                    0,                                      /* Reset Type                */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */
 
    /* Check Issued Report at Reset Mode */
    if ((sError = RptPTDAllIssuedChk()) != SUCCESS) {               /* Not Issued Reset Report */
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
        return(sError);                                                            
    }

    TtlReport.uchMajorClass = CLASS_TTLPTDRESET;

    /* Check to Execute Register Financial Report */
    if (!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT0)) {             /* Execute Register Financial Report */
        /* Reset Register Financial Total File */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_REGFIN_BIT)) != TTL_SUCCESS) {
            /* Print Register Financial Header and Error */
            RptEODPTDMakeHeader(AC_REGFIN_READ_RPT, RPT_FINANC_ADR, SPC_PTDRST_ADR, 0);
            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                           /* Set Reset Completed Flag */                                
        }
    }

    /* Check to Execute Department Report */
    if ((!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT1)) &&           /* Execute Department Report */
        (RptPTDChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST)) {    /* File Exist */
        /* Reset Department Total File */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_DEPT_BIT)) != TTL_SUCCESS) {
            /* Print Department File Header and Error */
            RptEODPTDMakeHeader(AC_DEPTSALE_READ_RPT, RPT_DEPT_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);
            RptPrtError(TtlConvertError(sError));
        } else {
            if (sError == TTL_SUCCESS) {
                uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
            }
        }
    }

    /* Check to Execute PLU Report */
    if ((!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT2)) &&           /* Execute PLU Report */
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
        (RptPTDChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {     /* File Exist */
    
        /* Reset PLU Total File */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_PLU_BIT)) != TTL_SUCCESS) {
            /* Print PLU File Header and Error */
            RptEODPTDMakeHeader(AC_PLU_RESET_RPT, RPT_PLU_ADR, SPC_PTDRST_ADR, 0);
            RptPrtError(TtlConvertError(sError));
        } else {
            if (sError == TTL_SUCCESS) {
				PLUIF  PluIf = {0};

                if (SerOpPluAllRead(&PluIf, 0) == OP_PERFORM) {             /* PLU Record or File Exists ? */
                    uchResetFlag = 1;                                       /* Set Reset Completed Flag */                                
                }
            }
        }
    } else if ((CliParaMDCCheck(MDC_PTD4_ADR, EVEN_MDC_BIT1)) &&            /* Execute PLU Optimize V1.0.15 */
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
			PLUIF  PluIf = {0};
    
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

            if (SerOpPluAllRead(&PluIf, 0) == OP_PERFORM) {             /* PLU Record or File Exists ? */
                uchResetFlag = 1;                                       /* Set Reset Completed Flag */                                
            }
        }
    }
	/* execute Daily PLU optimize at PTD, 10/11/01 */
    if ((CliParaMDCCheck(MDC_PTD4_ADR, EVEN_MDC_BIT1)) &&            /* Execute PLU Optimize V1.0.15 */
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
        (RptEODChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {      /* File Exist */

        /* Optimize PLU Total File */
        if ((sError = SerTtlPLUOptimize(CLASS_TTLCURDAY)) != TTL_SUCCESS) {
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
			PLUIF  PluIf = {0};
    
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

            if (SerOpPluAllRead(&PluIf, 0) == OP_PERFORM) {             /* PLU Record or File Exists ? */
                uchResetFlag = 1;                                       /* Set Reset Completed Flag */                                
            }
        }
    }
    
    /* Check to Execute Hourly Activity Report */
    if ((!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT3)) && RptHourGetBlock()) {             /* Execute Hourly Activity Report */
        /* Reset Hourly Activity Total Report */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_HOURLY_BIT)) != TTL_SUCCESS) {
            /* Print Hourly Activity File Header and Error */
            RptEODPTDMakeHeader(AC_HOURLY_READ_RPT, RPT_HOUR_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);
            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                           /* Set Reset Completed Flag */                                
        }
    }

    /* Check to Execute Service Time Report */
    if ((!CliParaMDCCheck(MDC_PTD3_ADR, ODD_MDC_BIT1))
        && RptHourGetBlock()                /* Execute Service Time Report */
        && (RptPTDChkFileExist(FLEX_GC_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
    
        /* Reset Service Time Total Report */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_SERVICE_BIT)) != TTL_SUCCESS) {
            /* Print Service Time File Header and Error */
            RptEODPTDMakeHeader(AC_SERVICE_READ_RPT, RPT_SERVICE_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);
            RptPrtError(TtlConvertError(sError));
        } else {
            uchResetFlag = 1;                           /* Set Reset Completed Flag */                                
        }
    }

    /* Execute Guest Check File Report */
    if ((!CliParaMDCCheck(MDC_PTD2_ADR, EVEN_MDC_BIT2)) &&          /* Execute GCF Report */
        (RptEODChkFileExist(FLEX_GC_ADR) == RPT_FILE_EXIST)) {  /* File Exist Use RptEODChkFileExist() */      
        if ((sError = RptGuestResetByGuestCheck (RPT_PTD_ALLRESET, 0)) != SUCCESS && (sError != LDT_NTINFL_ADR)) {
            RptPrtError(sError);
        } else {
            if (sError == SUCCESS) {
                uchResetFlag = 1;                                   /* Set Reset Completed Flag */                                
            }
        }
    }

    /* Check to Cashier Report, V3.3 */
    if ((!CliParaMDCCheck(MDC_PTD2_ADR, EVEN_MDC_BIT0)) && (RptPTDChkFileExist(FLEX_CAS_ADR) == RPT_FILE_EXIST)) {     /* Execute Cashier Report and File Exist */
        /* Reset Cashier Total Report */
        RptInitialize();                                            /* Clear Report Work */
        if ((sError = RptCashierReset(CLASS_TTLCURPTD, RPT_PTD_ALLRESET, RPT_PRT_RESET, 0)) != SUCCESS) {
            /* Print Cashier File Header and Error */
            if (sError != RPT_RESET_FAIL) {
                RptEODPTDMakeHeader(AC_CASH_RESET_RPT, RPT_CAS_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);
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

    /* Check to Coupon Report */
    if ((!CliParaMDCCheck(MDC_PTD2_ADR, EVEN_MDC_BIT3)) && (RptPTDChkFileExist(FLEX_CPN_ADR) == RPT_FILE_EXIST)) {  /* Execute Coupon Report if File Exist */
        /* Reset Coupon Total Report */
        if ((sError = SerTtlTotalReset(&TtlReport, RPT_RESET_COUPON_BIT)) != TTL_SUCCESS) {
            RptEODPTDMakeHeader(AC_CPN_RESET_RPT, RPT_CPN_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);
            RptPrtError(sError);
        } else {
            uchResetFlag = 1;                                   /* Set Reset Completed Flag */
        }
    }

    /* Count Up Reset Counter */
    if (uchResetFlag) {                                     /* Reset Completed Case */                               
        MaintIncreSpcCo(SPCO_PTDRST_ADR);
    }
    /* else {
        MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        Make Trailer 
    } */
    
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT RptPTDAllIssuedChk( VOID ) 
*               
*       Input:  Nothing         
*      Output:  Nothing
*
**  Return: SUCCESS             : Successful 
*
**  Description: Check Issued Flag (Financial, DEPT, PLU, Hourly, Cashier, Waiter File).  
*===============================================================================
*/

SHORT RptPTDAllIssuedChk( VOID ) 
{
    SHORT       sError;
    SHORT       sReturn;
	TTLREPORT   TtlReport = {0};

    /* Check Register Financial Issued Report Flag */
    TtlReport.uchMajorClass = CLASS_TTLREGFIN;
    TtlReport.uchMinorClass = CLASS_TTLSAVPTD;
    TtlReport.ulNumber = 0;

    /* Check to Execute Register Financial Report */
    if (!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT0)) {             /* Execute Register Financial Report */
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
    
            /* Print Register Financial Header and Error */
            RptEODPTDMakeHeader(AC_REGFIN_READ_RPT, RPT_FINANC_ADR, SPC_PTDRST_ADR, 0);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }

    /* Check to Execute Department Report */
    if ((!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT1)) &&           /* Execute Department Report */
        (RptPTDChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST)) {    /* File Exist */

        /* Check Department Issued Report Flag */
        TtlReport.uchMajorClass = CLASS_TTLDEPT;
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
    
            /* Print Department Header and Error */
            RptEODPTDMakeHeader(AC_DEPTSALE_READ_RPT, RPT_DEPT_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }    

    /* Check to Execute PLU Report */
    if ((!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT2)) &&           /* Execute PLU Report */
        (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_EXIST) &&    /* File Exist */
        (RptPTDChkFileExist(FLEX_PLU_ADR) == RPT_FILE_EXIST)) {     /* File Exist */

        /* Check PLU Issued Report Flag */
        TtlReport.uchMajorClass = CLASS_TTLPLU;
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
    
            /* Print PLU Header and Error */
            RptEODPTDMakeHeader(AC_PLU_RESET_RPT, RPT_PLU_ADR, SPC_PTDRST_ADR, 0);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }

    /* Check to Execute Hourly Activity Report */
    if ((!CliParaMDCCheck(MDC_PTD1_ADR, ODD_MDC_BIT3)) && RptHourGetBlock()) {             /* Execute Hourly Activity Report */
    
        /* Check Hourly Activity Issued Report Flag */
        TtlReport.uchMajorClass = CLASS_TTLHOURLY;
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
    
            /* Print Hourly Activity and Error */
            RptEODPTDMakeHeader(AC_HOURLY_READ_RPT, RPT_HOUR_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }


    /* Check to Execute Service Time Report */
    if ((!CliParaMDCCheck(MDC_PTD3_ADR, ODD_MDC_BIT1)) && RptHourGetBlock()) {     /* Execute Service Time Report */
    
        /* Check Service Time Issued Report Flag */
        TtlReport.uchMajorClass = CLASS_TTLSERVICE;
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
    
            /* Print Service Time and Error */
            RptEODPTDMakeHeader(AC_SERVICE_READ_RPT, RPT_SERVICE_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }


    /* Check to Cashier Report, V3.3 */
    if ((!CliParaMDCCheck(MDC_PTD2_ADR, EVEN_MDC_BIT0)) &&          /* Execute Waiter Report */
        (RptPTDChkFileExist(FLEX_CAS_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
    
        /* Check Cashier Issued Report Flag */
        TtlReport.uchMajorClass = CLASS_TTLCASHIER;
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
    
            /* Print Cashier Header and Error */
            RptEODPTDMakeHeader(AC_CASH_RESET_RPT, RPT_CAS_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }

    /* Check to Coupon Report */

    if ((!CliParaMDCCheck(MDC_PTD2_ADR, EVEN_MDC_BIT3)) &&          /* Execute Coupon Report */
        (RptPTDChkFileExist(FLEX_CPN_ADR) == RPT_FILE_EXIST)) {      /* File Exist */
    
        /* Check Coupon Issued Report Flag */
        TtlReport.uchMajorClass = CLASS_TTLCPN;
        if ((sError = SerTtlIssuedCheck(&TtlReport, TTL_NOTRESET)) == TTL_NOT_ISSUED) {
    
            /* Print Coupon Header and Error */
            RptEODPTDMakeHeader(AC_CPN_RESET_RPT, RPT_CPN_ADR, SPC_PTDRST_ADR, RPT_ALL_RESET);
            sReturn = TtlConvertError(sError);
            RptPrtError(sReturn);
            return(sReturn);
        }
    }

    return(SUCCESS);
}

/* --- End of Source File --- */