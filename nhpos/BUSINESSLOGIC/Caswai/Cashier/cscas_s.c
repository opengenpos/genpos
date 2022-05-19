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
*   Title              : CASHIER module, for Satellite Terminal Program List                        
*   Category           : Client/Server CASHIER module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSCAS.C                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as floolws.
*
*            CasConvertError(SHORT sError);              * Convert a error to a leadthru *
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
*    Date                Ver.Rev    :NAME         :Description
*    Aug-11-92         : 00.00.01   :H.YAMAGUCHI  :Initial
*
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
#include	<tchar.h>
#include	<math.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <cscas.h>
#include    <appllog.h>

/*
*===========================================================================
** Synopsis:    USHORT     CasConvertError(SHORT sError);
*     Input:    sError 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Leadthru No 
*
** Description: This function converts error code to leadthru number.
*===========================================================================
*/
USHORT   CasConvertErrorWithLineNumber(SHORT sError, ULONG ulCalledFromLineNo)
{
    USHORT  usLeadthruNo;

    switch ( sError ) {

    case  CAS_PERFORM :   /* Success  0  */
        usLeadthruNo = 0               ; 
        break;

    case  CAS_NO_MAKE_FILE :   /* Error   -6  */
        usLeadthruNo = LDT_FLFUL_ADR ;     /*   2 * File Full */
        break;

    case  CAS_LOCK :           /* Error   -5  */  
        usLeadthruNo = LDT_LOCK_ADR  ;     /*  22 * During Lock */
        break;

    case  CAS_FILE_NOT_FOUND:  /* Error   -1  */
        usLeadthruNo = LDT_PROHBT_ADR  ;   /*  10 * Prohibit Operation */
        break;

    case  CAS_NOT_IN_FILE :    /* Error   -3  */
        usLeadthruNo = LDT_NTINFL_ADR ;    /*   1 * Not In File */       
        break;

    case  CAS_SECRET_UNMATCH : /* Error   -14  */
        usLeadthruNo = LDT_KEYOVER_ADR ;   /*   8 * Key Entered Over Limitation */       
        break;

    case  CAS_NO_SIGNIN :      /* Error   -15  */
        usLeadthruNo = LDT_NOTSIGNEDIN_ADR ;   /*  10 * Prohibit Operation, V3.3 */
        break;

    case  CAS_CASHIER_FULL :   /* Error   -2  */
        usLeadthruNo = LDT_FLFUL_ADR ;     /*   2 * File Full */
        break;

    case  CAS_DURING_SIGNIN :  /* Error   -12  */
    case  CAS_ALREADY_OPENED:  /* Error   -16  */
        usLeadthruNo = LDT_OPNWTIN_ADR ; /*  14 Opened Cashier/Waiter */
        break;

    case  CAS_OTHER_LOCK :     /* Error   -11  */
        usLeadthruNo = LDT_LOCK_ADR  ;     /*  22 * During Lock */
        break;

    case  CAS_DIFF_NO :        /* Error  -13  */
        usLeadthruNo = LDT_LOCK_ADR  ;     /*  22 * During Lock */
        break;

    case  CAS_ALREADY_LOCK :   /* Error  -10  */
        usLeadthruNo = LDT_LOCK_ADR  ;     /*  22 * During Lock */
        break;

    case  CAS_NOT_MASTER :     /* Error  -20  */
        usLeadthruNo = LDT_I_AM_NOT_MASTER ;    /*  10 * Prohibit Operation */
        break;

    case  CAS_REQUEST_SECRET_CODE : /* Error  -18  */
        usLeadthruNo = LDT_SECRET_ADR ;    /*  115 * request secret code, V3.3 */
        break;

    case  CAS_NOT_ALLOWED : /* Error  -19  */
        usLeadthruNo = LDT_PROHBT_ADR ;    /*  10 * Prohibit Operation */
        break;

    case  CAS_CAS_DISABLED : /* Error  -22  */
        usLeadthruNo = LDT_NOUNMT_ADR ;    /*  16 * Cashier/Waiter # Unmatch */
        break;

	case CAS_COMERROR: /* Error -37 */
		usLeadthruNo = LDT_EQUIPPROVERROR_ADR;
		break;

	case CAS_NOT_REGISTERED: /* Error -177 */
		usLeadthruNo = LDT_UNREGISTERED_ADR;
		break;

	case CAS_M_DOWN_ERROR:
	case CAS_DUR_INQUIRY:
		usLeadthruNo = LDT_LNKDWN_ADR;         /*  6 * Link Down or Server state is SER_STINQUIRY */
		break;

	case CAS_BUSY_ERROR:
	case CAS_TIME_OUT_ERROR:
		usLeadthruNo = LDT_REQST_TIMED_OUT;    /*  147 * Requested operation timed out before completion */
		break;

    default:  
        usLeadthruNo = LDT_ERR_ADR   ;     /*  21 * Error(Global) */
        break;

    }

	if (sError != CAS_PERFORM) {
		PifLog (MODULE_CASHIER, LOG_EVENT_STB_CONVERTERROR);
		PifLog (MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sError));
		PifLog (MODULE_ERROR_NO(MODULE_CASHIER), usLeadthruNo);
		PifLog (MODULE_LINE_NO(MODULE_CASHIER), (USHORT)ulCalledFromLineNo);
	}

    return (usLeadthruNo);           
}


/*====== End of Source ======*/

