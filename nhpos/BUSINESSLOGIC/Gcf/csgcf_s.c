/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Guest Check module, For Satellite Terminal                        
*   Category           : Client/Server Guest Check module, NCR2170 US HOSPITALITY MODEL.
*   Program Name       : CSGCF.C
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as follows
*
*           GusConvertError         * Converts error code to leadthru number *
*            
*
*  ------------------------------------------------------------------------
*   Update Histories
*   Date     :Ver.Rev.:  NAME     :Description
*   Aug-11-92:00.00.01:H.YAMAGUCHI:Initial
*            :        :           : 
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
#include    <math.h>
#include	<stdio.h>

#include    <ecr.h>
#include    <rfl.h>
#include    <pif.h>
#include    <log.h>
#include    <appllog.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csgcs.h>

/*
*===========================================================================
** Synopsis:    USHORT     GusConvertError(SHORT sError);
*     Input:    sError 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Leadthru No 
*
** Description: This function converts error code to leadthru number.
*===========================================================================
*/
#if defined(GusConvertError)
USHORT   GusConvertError_Special(SHORT sError);
USHORT   GusConvertError_Debug(SHORT sError, char *aszFilePath, int nLineNo)
{
	if (sError < 0) {
		char  xBuffer[256];

		sprintf (xBuffer, "GusConvertError_Debug(): sError = %d  File %s  lineno = %d", sError, RflPathChop(aszFilePath), nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}
	return GusConvertError_Special(sError);
}

USHORT   GusConvertError_Special(SHORT sError)
#else
USHORT   GusConvertError(SHORT sError)
#endif
{
    USHORT  usLeadthruNo;

	if (sError > 0) {
		usLeadthruNo = 0; // errors are negative numbers only
	}
	else {
		switch ( sError ) {
		case  GCF_SUCCESS :         /* Success */
			usLeadthruNo = 0;                   /*  0   SUCCESS  */
			break;

		case  GCF_NOT_CREATE :      /* NOT CREAT GCF FILE */
			usLeadthruNo = LDT_FLFUL_ADR     ; /*   2   File Full */
			break;

		case  GCF_FULL :            /* FILE FULL */
			usLeadthruNo = LDT_FLFUL_ADR     ; /*   2   File Full */
			break;

		case  GCF_NOT_IN :          /* NOT FINED GCN */
			usLeadthruNo = LDT_NTINFL_ADR    ; /*   1   Not In File */       
			break;

		case  GCF_NOT_LOCKED :      /* NOT DURING READING GCN */
			usLeadthruNo = LDT_ERR_ADR   ;     /*  21 * Error(Global) */
			break;

		case  GCF_LOCKED :          /* DURING READING GCN BY OTHER TERMINAL */
			usLeadthruNo = LDT_LOCK_ADR      ; /*  22   During Lock */
			break;

		case  GCF_EXIST :           /* ALREADY EXIST GCN */
			usLeadthruNo = LDT_SAMEGCNO_ADR  ; /*  17   Same Check # Already Exists in GCF */
			break;

		case  GCF_ALL_LOCKED :       /* LOCK FOR DURING RESET */
			usLeadthruNo = LDT_LOCK_ADR      ; /*  22   During Lock */
			break;

		case  GCF_DUR_LOCKED :       /* ALREADY  LOCK */
			usLeadthruNo = LDT_LOCK_ADR      ; /*  22   During Lock */
			break;

		case  GCF_FATAL :           /* BROKEN INDEX TABLE */
			usLeadthruNo = LDT_ERR_ADR       ; /*  21   Error(Global) */
			break;

		case  GCF_FILE_NOT_FOUND:   /* FILE NOT FOUND */
			usLeadthruNo = LDT_PROHBT_ADR    ; /*  10   Prohibit Operation */
			break;

		case  GCF_DATA_ERROR :      /* USER INPUT DATA ERROR */
			usLeadthruNo = LDT_KEYOVER_ADR   ;     /*  8  Key Entered Over Limitation */
			break;

		case  GCF_NOT_MASTER :      /* Not master */
			usLeadthruNo = LDT_I_AM_NOT_MASTER ; /*  10   Prohibit Operation */
			break;

		case GCF_COMERROR: /* Error -37 */
			usLeadthruNo = LDT_EQUIPPROVERROR_ADR;
			break;

		case  GCF_DUR_INQUIRY :
		case  GCF_M_DOWN_ERROR :
			usLeadthruNo = LDT_LNKDWN_ADR;    // Master Terminal busy
			break;

		case  GCF_BM_DOWN_ERROR :
			usLeadthruNo = LDT_SYSBSY_ADR;    // Backup Master busy
			break;

		case GCF_TIME_OUT_ERROR:
			usLeadthruNo = LDT_REQST_TIMED_OUT;
			break;

		case GCF_BUSY_ERROR:
			usLeadthruNo = LDT_REQST_TIMED_OUT;
			break;

		case GCF_UNMATCH_TRNO :
			usLeadthruNo = LDT_COMUNI_ADR;   // Transaction number did not match expected number
			break;

		default:   
            PifLog(MODULE_GCF, LOG_ERROR_GCF_DRIVE_DEFERROR);
			usLeadthruNo = LDT_ERR_ADR   ;     /*  21 * Error(Global) */
			break;
		}

		if (sError != GCF_SUCCESS) {
			PifLog (MODULE_GCF, LOG_ERROR_GCF_CONVERTERROR);
			PifLog (MODULE_ERROR_NO(MODULE_GCF), (USHORT)abs(sError));
			PifLog (MODULE_ERROR_NO(MODULE_GCF), usLeadthruNo);
		}

	}
    return (usLeadthruNo);           
}


/*========= End of File =========*/
