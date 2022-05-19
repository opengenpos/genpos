/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : CPM module, Error Conversion Routines                        
*   Category           : Charge Posting Manager, NCR 2170 RESTRANT MODEL
*   Program Name       : CPMERRTL.C                                            
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*                   CpmConvertError();

*-----------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name       : Description
* Nov-11-93 : 00.00.01 : H.YAMAGUCHI  : Initial                                   
* Jun-17-15 : 02.02.01 : R.Chambers   : added CPM_RET_SERVICE_ERROR and comments.
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
#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <cpm.h>
#include    <appllog.h>

/*
*===========================================================================
** Synopsis:    USHORT     CpmConvertError(SHORT sError);
*     Input:    sError 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Leadthru No 
*
** Description: This function converts error code to leadthru number.
*===========================================================================
*/
USHORT   CpmConvertError(SHORT sError)
{
    USHORT  usLeadthruNo;

    switch ( sError ) {
    case CPM_RET_SUCCESS:                       /* Success              */
        usLeadthruNo = 0; 
        break;

    case CPM_RET_LENGTH:                        /* EEPT_RET_LENGTH receive length error */
        usLeadthruNo = LDT_ERR_ADR;             /*  21 * Error(Global)  */
        break;

    case  CPM_RET_OFFLINE:                      /* EEPT_RET_OFFLINE CPM & PMS is offline */
        usLeadthruNo = LDT_CPM_OFFLINE_ADR;     /* 81 * CPM offline     */
        break;

    case  CPM_RET_TIMEOUT:                      /* EEPT_RET_TIMEOUT time out error       */
        usLeadthruNo = LDT_REQST_TIMED_OUT;     /* 81 * CPM offline     */
        break;

    case  CPM_RET_FAIL:                         /* EEPT_RET_FAIL other communication  */
        usLeadthruNo = LDT_ERR_ADR;             /*  21 * Error(Global)  */
        break;

    case  CPM_RET_STOP:                         /* EEPT_RET_STOP CPM is stop process  */
        usLeadthruNo = LDT_CPM_STOP_FUNC_ADR;   /*  80 * CPM Stop       */
        break;

    case  CPM_RET_BUSY:                         /* EEPT_RET_BUSY CPM is busy          */
        usLeadthruNo = LDT_CPM_BUSY_ADR;        /* 79 * CPM busy        */
        break;

    case  CPM_RET_PORT_NONE:                    /* EEPT_RET_PORT_NONE PMS is not provide   */
        usLeadthruNo = LDT_EQUIPPROVERROR_ADR;  /* Equipment not provisioned properly */
        break;

    case  CPM_RET_NOT_MASTER:                   /* EEPT_RET_NOT_MASTER I'm not master (O/S) */
        usLeadthruNo = LDT_PROHBT_ADR;          /* 10 * Prohibit Operation */
        break;

    case  CPM_RET_BADPROV:                      // EEPT_RET_BADPROV
        usLeadthruNo = LDT_EQUIPPROVERROR_ADR;  /* Equipment not provisioned properly */
        break;

    case  CPM_RET_CANCELLED:                    // EEPT_RET_CANCELLED
        usLeadthruNo = LDT_CANCEL_ADR;
		break;

    case  CPM_RET_SERVERCOMMS:                  // EEPT_RET_SERVERCOMMS
        usLeadthruNo = LDT_EQUIPPROVERROR_ADR;
		break;

    case  CPM_RET_INVALIDDATA:                  // EEPT_RET_INVALIDDATA
        usLeadthruNo = LDT_WRONG_ACNO_ADR;
		break;

    case  CPM_DUR_INQUIRY :
    case  CPM_M_DOWN_ERROR:
        usLeadthruNo = LDT_LNKDWN_ADR;
		break;

    case  CPM_BM_DOWN_ERROR:
        usLeadthruNo = LDT_SYSBSY_ADR;
		break;

	case  CPM_TIME_OUT_ERROR:
		usLeadthruNo = LDT_REQST_TIMED_OUT;
		break;

	case  CPM_UNMATCH_TRNO :
		usLeadthruNo = LDT_COMUNI_ADR;   // Transaction number did not match expected number
		break;

	case  CPM_BUSY_ERROR:
		usLeadthruNo = LDT_REQST_TIMED_OUT;
		break;

	case  CPM_RET_SERVICE_ERROR:                  // EEPT_RET_SERVICE_ERROR an error such as encryption error in DSI Client
		usLeadthruNo = LDT_ALTERNATE_TENDER_RQD;
		break;

    default:
		//SR368
		// sError is a negative value as are EEPTDLL_BASE_ERROR (lowest in range) and EEPTDLL_MAX_ERROR (max of range)
		// See the error code table of EptErrorCodeTable[] which contains the sError codes beginning with -100 
		if (sError <= EEPTDLL_BASE_ERROR && sError >= EEPTDLL_MAX_ERROR)
			usLeadthruNo = EEPTDLL_LDTOFFSET + (sError * -1);
		else
			usLeadthruNo = LDT_CPM_UNKNOWNRESPONSE; /*  Unknown error  */
        break;
    }

	if (sError != STUB_SUCCESS) {
		if (sError <= EEPTDLL_BASE_ALT_TENDER && sError >= EEPTDLL_MAX_ALT_TENDER) {
			// if in the range for 
			usLeadthruNo = LDT_ALTERNATE_TENDER_RQD;
		}
		PifLog (MODULE_STB_LOG, LOG_EVENT_STB_CONVERTERROR_CPM);
		PifLog (MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)(sError * (-1)));
		PifLog (MODULE_ERROR_NO(MODULE_STB_LOG), usLeadthruNo);
	}

    return (usLeadthruNo);           
}

/*====== End of Source ======*/