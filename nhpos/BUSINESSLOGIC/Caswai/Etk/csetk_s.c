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
*   Title              : EMPLOYEE module, for Satellite Terminal Program List                        
*   Category           : Client/Server EMPLOYEE module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSETK.C                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as floolws.
*
*            EtkConvertError(SHORT sError);              * Convert a error to a leadthru *
*
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*    Date      Ver.Rev :NAME       :Description
*    Jun-15-93:00.00.01:M.YAMAMOTO :Initial
*
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include <tchar.h>
#include <math.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <csetk.h>
#include    <appllog.h>


/*
*=========================================================================== 
**    Synopsis:    USHORT   EtkConvertError(SHORT sError)
*
*     Input:    sError 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Leadthru No 
*
** Description: This function converts error code to leadthru number.
*===========================================================================
*/
USHORT   EtkConvertErrorWithLineNumber(SHORT sError, ULONG ulCalledFromLineNo)
{
    USHORT  usLeadthruNo;
	SHORT   sErrorSave = sError;

    switch ( sError ) {
    case  ETK_SUCCESS :   /* Success  0  */
    case  ETK_CONTINUE :
        usLeadthruNo = 0               ; 
        break;

    case  ETK_FIELD_OVER:
    case  ETK_FULL:
    case  ETK_NO_MAKE_FILE :   /* Error   -6  */
        usLeadthruNo = LDT_FLFUL_ADR ;     /*   2 * File Full */
        break;

    case  ETK_LOCK :           /* Error   -5  */  
        usLeadthruNo = LDT_LOCK_ADR  ;     /*  22 * During Lock */
        break;

    case  ETK_FILE_NOT_FOUND:  /* Error   -1  */
    case  ETK_DATA_EXIST:
        usLeadthruNo = LDT_PROHBT_ADR  ;   /*  10 * Prohibit Operation */
        break;

    case  ETK_NOT_IN_JOB :
    case  ETK_NOT_IN_FIELD:
    case  ETK_NOT_IN_FILE :    /* Error   -3  */
        usLeadthruNo = LDT_NTINFL_ADR ;    /*   1 * Not In File */       
        break;

	case  LDT_PROHBT_ADR:
        usLeadthruNo = LDT_PROHBT_ADR ;    /*  10 * Prohibit Operation */
        break;

    case  ETK_NOT_MASTER :     /* Error  -20  */
        usLeadthruNo = LDT_I_AM_NOT_MASTER ;    /*  148 * I am not Master */
        break;

    case  ETK_NOTISSUED :       /* Error  -18 */
        usLeadthruNo = LDT_NOT_ISSUED_ADR ;    /*  23 * Not Issued Report */
        break;

    case  ETK_NOT_TIME_OUT :
    case  ETK_NOT_TIME_IN :
        usLeadthruNo = LDT_ETKERR_ADR ;    /*  64 * ETK Error */
        break;

    case  ETK_DUR_INQUIRY :
    case  ETK_M_DOWN_ERROR :
		usLeadthruNo = LDT_LNKDWN_ADR;    // Master Terminal busy
        break;

    case  ETK_BM_DOWN_ERROR :
		usLeadthruNo = LDT_SYSBSY_ADR;    // Backup Master busy
        break;

	case ETK_TIME_OUT_ERROR:
		usLeadthruNo = LDT_REQST_TIMED_OUT;
		break;

	case ETK_UNMATCH_TRNO :
		usLeadthruNo = LDT_COMUNI_ADR;   // Transaction number did not match expected number
		break;

	case ETK_BUSY_ERROR:
		usLeadthruNo = LDT_REQST_TIMED_OUT;
		break;

    default:  /* ETK_DELETE_FILE */
        usLeadthruNo = LDT_ERR_ADR   ;     /*  21 * Error(Global) */
        break;

    }
	if (usLeadthruNo != 0) {
		PifLog (MODULE_ETK, LOG_EVENT_STB_CONVERTERROR);
		PifLog (MODULE_ERROR_NO(MODULE_ETK), (USHORT)abs(sErrorSave));
		PifLog (MODULE_ERROR_NO(MODULE_ETK), usLeadthruNo);
		PifLog (MODULE_LINE_NO(MODULE_CASHIER), (USHORT)ulCalledFromLineNo);
	}

    return (usLeadthruNo);
}

/*====== End of Source ======*/

