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
* Title       : STUB Error Convert Routine Source File
* Category    : STUB Module, US Hospitality Model
* Program Name: CSLEADST.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           CstConvertError()       Convert Error Code to Leadthru Message
*           
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Sep-19-92:H.Yamaguchi: initial                                   
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
#include	<math.h>
#include	<stdio.h>

#include    <ecr.h>
#include    <rfl.h>
#include    <pif.h>
#include    <log.h>
#include    <paraequ.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csstbbak.h>
#include    <appllog.h>


/*
*===========================================================================
** Synopsis:    USHORT     CstConvertError(SHORT sError);
*     Input:    sError 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Leadthru No 
*
** Description: This function converts error code to leadthru number.
				The lead thru mnemonics reported by this function should
				correspond to the lead thru mnemonics reported by the function
				CstDisplayError () for the identical error condition.  If it
				doesn't, please add a comment as to why it doesn't.
*===========================================================================
*/
#if defined(CstConvertError)
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   CstConvertError_Debug() is ENABLED     ====++++====  \z")
USHORT   CstConvertError_Special(SHORT sError);
USHORT   CstConvertError_Debug(SHORT sError, char *aszFilePath, int nLineNo)
{
	char  xBuffer[256];

	sprintf (xBuffer, "CstConvertError_Debug(): sError = %d  File %s  lineno = %d", sError, RflPathChop(aszFilePath), nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);
	return CstConvertError_Special(sError);
}

USHORT   CstConvertError_Special(SHORT sError)
#else
USHORT   CstConvertError(SHORT sError)
#endif
{
    USHORT  usLeadthruNo = LDT_ERR_ADR;     /*  21 * Error(Global) */

    switch ( sError ) {

		case  STUB_SUCCESS :        /* Success */
			usLeadthruNo = 0   ;   
			break;

		case  STUB_RETCODE_NOT_MASTER_ERROR :    /* Prohibit operation */
		case  STUB_NOT_MASTER :    /* Prohibit operation */
			usLeadthruNo = LDT_I_AM_NOT_MASTER;
			break;

		case  STUB_NOT_UPTODATE :         
		case  STUB_ILLEGAL :         
			usLeadthruNo = LDT_PROHBT_ADR ;    /*  10   Prohibit Operation */
			break;

		case  STUB_RETCODE_DUR_INQUIRY:
		case  STUB_DUR_INQUIRY:
			usLeadthruNo = LDT_LNKDWN_ADR;
			break;

		case STUB_KPS_BUSY :
			usLeadthruNo = LDT_KPS_BUSY;
			break;

		case    STUB_DISCOVERY:
			// fall through and treat same as STUB_TIME_OUT.  See also PIF_ERROR_NET_DISCOVERY
		case  STUB_TIME_OUT :
		case  STUB_RETCODE_TIME_OUT_ERROR :
			usLeadthruNo = LDT_REQST_TIMED_OUT;
			break;

		case  STUB_BUSY_MULTI:
		case  STUB_BUSY :
		case  STUB_RETCODE_BUSY_ERROR :
			usLeadthruNo = LDT_SYSBSY_ADR;
 			break;

		case  STUB_RETCODE_UNMATCH_TRNO :
			usLeadthruNo = LDT_COMUNI_ADR;   // Transaction number did not match expected number
			break;

		case    STUB_INQ_M_DOWN:
		case    STUB_RETCODE_M_DOWN_ERROR:
		case    STUB_M_DOWN:
			usLeadthruNo = LDT_LNKDWN_ADR;
			break;

		case    STUB_INQ_BM_DOWN:
		case    STUB_RETCODE_BM_DOWN_ERROR:
		case    STUB_BM_DOWN:
			usLeadthruNo = LDT_SYSBSY_ADR;
			break;

		case    STUB_IAM_BUSY:
			usLeadthruNo = LDT_LNKDWN_ADR;
			if (CLI_IAM_BMASTER) {
				usLeadthruNo = LDT_SYSBSY_ADR;
			}
			break;

		case	STUB_COMERROR :
		case	STUB_NO_BACKUP :
			usLeadthruNo = LDT_EQUIPPROVERROR_ADR;
			break;

		case	STUB_NOT_REGISTERED :
			usLeadthruNo = LDT_UNREGISTERED_ADR;
			break;
    }

	if (sError != STUB_SUCCESS) {
		PifLog (MODULE_STB_LOG, LOG_EVENT_STB_CONVERTERROR);
		PifLog (MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)(abs(sError)));
		PifLog (MODULE_ERROR_NO(MODULE_STB_LOG), usLeadthruNo);
	}

    return (usLeadthruNo);           
}

/*======== End of Source =============*/
