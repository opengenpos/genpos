/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL  Error Convert Routine Source File
* Category    : TOTAL / TOTALUPDATE Module, US Hospitality Model
* Program Name: CSLEADTL.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           TtlConvertError()       Convert Error Code to Leadthru Message
*           
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jul-04-92:H.Yamaguchi: initial                                   
* Oct-20-92:H.Yamaguchi: Change TTL_NOT_ISSUED ---> LDT_NOT_ISSUED_ERR                                   
*          :           :                                    
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
#include	<tchar.h>
#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <paraequ.h>
#include    <csttl.h>
#include    <appllog.h>


/*
*===========================================================================
** Synopsis:    USHORT     TtlConvertError(SHORT sError);
*     Input:    sError 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Leadthru No 
*
** Description: This function converts error code to leadthru number.
*===========================================================================
*/
#if defined(TtlConvertError)
USHORT   TtlConvertError_Special(SHORT sError);
USHORT   TtlConvertError_Debug(SHORT sError, char *aszFilePath, int nLineNo)
{
	USHORT  usRetStatus = TtlConvertError_Special(sError);

	if (TTL_SUCCESS != sError) {
		int     iLen = 0;
		char    xBuffer[256];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		sprintf (xBuffer, "TtlConvertError_Debug(): sError = %d, %d, File %s, lineno = %d", sError, usRetStatus, aszFilePath + iLen, nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}
	return usRetStatus;
}

USHORT   TtlConvertError_Special(SHORT sError)
#else
USHORT   TtlConvertError(SHORT sError)
#endif
{
    USHORT  usLeadthruNo;

    switch ( sError ) {
//		case  TTL_UNLOCKED:        /* same as TTL_SUCCESS, TtlConvertError() */
//		case  TTL_ISSUED:          /* same as TTL_SUCCESS, TtlConvertError() */
		case  TTL_SUCCESS :        /* Success, TtlConvertError()  */
			usLeadthruNo = 0   ;   
			break;

		case  TTL_FAIL  :         /* Failed */
			usLeadthruNo = LDT_ERR_ADR ;       /*  21 * Error(Global) */
			break;

		case  TTL_BUFFER_FULL :   /* Update Total File Full */
			usLeadthruNo = LDT_FLFUL_ADR ;     /*   2   File Full */
			break;

		case  TTL_NOTINFILE :     /* specified data not in file */
			usLeadthruNo = LDT_NTINFL_ADR ;    /*   1   Not In File */       
			break;

		case  TTL_NOT_ISSUED :    /* reset report disable */
			usLeadthruNo = LDT_NOT_ISSUED_ADR ;       /*  23 * Error(Global) */
			break;

		case  TTL_LOCKED :        /* file status (Locked) */
			usLeadthruNo = LDT_LOCK_ADR ;      /*  22   During Lock */
			break;

		case  TTL_NOTCLASS :      /* Illigal Class # */
			usLeadthruNo = LDT_ERR_ADR   ;     /*  21 * Error(Global) */
			break;

		case  TTL_NOTPTD :        /* Not Exist PTD */
			usLeadthruNo = LDT_PROHBT_ADR ;    /*  10   Prohibit Operation */
			break;

		case  TTL_CHECK_RAM :     /* Check RAM Size */
			usLeadthruNo = LDT_FLFUL_ADR ;     /*   2   File Full */
			break;

		case  TTL_NOT_MASTER :    /* Prohibit operation */
			usLeadthruNo = LDT_PROHBT_ADR ;    /*  10   Prohibit Operation */
			break;

		case TTL_NEAR_FULL:
			usLeadthruNo = LDT_NEARFULL_ADR;
			break;

		case TTL_UPD_DB_BUSY:
			usLeadthruNo = LDT_LNKDWN_ADR;
			break;

		case TTL_UPD_M_DOWN_ERROR:
			usLeadthruNo = LDT_LNKDWN_ADR;
			break;

		case TTL_UPD_BM_DOWN_ERROR:
			usLeadthruNo = LDT_SYSBSY_ADR;
			break;

		case TTL_DUR_INQUIRY :
		case TTL_M_DOWN_ERROR :
			usLeadthruNo = LDT_LNKDWN_ADR;    // Master Terminal busy
			break;

		case TTL_BM_DOWN_ERROR :
			usLeadthruNo = LDT_SYSBSY_ADR;    // Backup Master busy
			break;

		case TTL_TIME_OUT_ERROR:
			usLeadthruNo = LDT_REQST_TIMED_OUT;
			break;

		case TTL_UNMATCH_TRNO :
			usLeadthruNo = LDT_COMUNI_ADR;   // Transaction number did not match expected number
			break;

		case TTL_BUSY_ERROR:
			usLeadthruNo = LDT_REQST_TIMED_OUT;
			break;

		default:  
			usLeadthruNo = LDT_ERR_ADR   ;     /*  21 * Error(Global) */
			break;
    }

	if (sError != TTL_SUCCESS) {
		PifLog (MODULE_TTL_TUM, LOG_EVENT_TTL_CONVERTERROR);
		PifLog (MODULE_ERROR_NO(MODULE_TTL_TUM), (USHORT)(sError * (-1)));
		PifLog (MODULE_DATA_VALUE(MODULE_TTL_TUM), usLeadthruNo);
	}

    return (usLeadthruNo);           
}

