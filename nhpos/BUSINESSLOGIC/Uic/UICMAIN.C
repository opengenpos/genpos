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
* Title       : Initialize and Main Module for Master Terminal                        
* Category    : UIC Main, NCR 2170 US Hospitality Application         
* Program Name: UICMAIN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           UicMain()               : UIC main 
*
*             UicInitialize()       : Initialize
*               UicChkRequstJob()   : Check Request Backup Copy and Parameter
*               UicRBCHand()        : Request Backup Copy Handling
*               UicRPHand()         : Request Parameter   Handling
*               UicResetExecuteFG() : Reset   Execute Flag
*               UicDisplayErrorMsg(): Display leadthrough no
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-26-92:00.00.01:H.Yamaguchi: initial                                   
* Apr-17-93:00.00.09:H.Yamaguchi: Adds reset consecutive no.
* Apr-24-95:02.05.04:M.Suzuki   : Add FDT Control R3.0                                   
*          :        :           :                                    
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
#include	<stdio.h>
#include	<stdlib.h>

#include <ecr.h>
#include <pif.h>
#include <log.h>
#include <uie.h>
#include <fsc.h>
#include <csstbbak.h>
#include <csstbstb.h>
#include <csop.h>
#include <csstbopr.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <nb.h>
#include <uic.h>
#include <storage.h>
#include "uiccom.h"
#include <appllog.h>
#include <regstrct.h>           /* Add R3.0 */
#include <transact.h>           /* Add R3.0 */
#include <fdt.h>            /* Add R3.0 */
#include <maint.h>            /* Add R3.0 */

/*
*===========================================================================
*   Internal Work Area Declarations
*===========================================================================
*/
                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    VOID UicInitialize(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Initialize
*===========================================================================
*/
VOID UicInitialize(VOID)
{
    UicResetFlag(husUicHandle);            /* Reset a handle */
}

/*
*===========================================================================
** Synopsis:    VOID  UicMain(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Main UIC Idle thread.  This thread processes the
*               Notice Board messages that require some time to process
*               such as parameter and file transfer actions.  The
*               purpose of this thread is to offload actions that might
*               interfere with the requirement of the main Notice Board
*               thread to run once a second as part of communicating status
*               information within the terminal cluster.
*===========================================================================
*/
VOID  THREADENTRY UicMain(VOID)
{
	static  UCHAR ucReqOperatorMessage = 0;

    USHORT      usSys;
    UCHAR       uchReq;
    NBMESSAGE   pMes;
    SHORT       sRet;

    UicInitialize();                            /* Initialize */

    for (;;) {                                  /* loop for ever */
		// Wait for a semaphore release from NbRcvMesHand() after
		// it receives an NB_FCSTARTASMASTER message.  The semaphore
		// husUicExecSem is released via function UicStart() which releases
		// the semaphore if global variable fsUicExecuteFG == UIC_IDLESTATE.
		// After doing a loop to process the latest Notice Board message,
		// we will reset global variable fsUicExecuteFG using function UicResetExecuteFG().
        PifRequestSem(husUicExecSem);           /* Wait for start action */

		// Read the latest Notice Board message and execute
		// the request.  If function UicChkRequestJob () returns
		// a value of UIC_EXEC_JOB then there is a pending request.
        for (;;) {                              /*  Loop */
			// Check the latest Notice Board status to see if there is
			// a pending request.  If not, (UicChkRequestJob() == UIC_SUCCESS)
			// then break this loop in order to clear the execute flag and
			// do a semaphore wait until Notice Board notifies us again.
            NbReadAllMessage(&pMes);            /* Read terminal Inform. flag */
            sRet = UicChkRequestJob(&pMes);     /* Check backup and parameter */
  
            if (sRet == UIC_SUCCESS) {
                break;                          /* Exit, if no request */
            }

            usSys = pMes.fsMBackupInf | pMes.fsBMBackupInf  ; /* Save Master Backup Inf. */

            UicRBCHand(usSys);                  /* Execute Request backup handling */

            if ( usSys & NB_STOPALLFUN )  {
                break;                          /* If stop func , then return */
            }

			if (ucReqOperatorMessage == 0) {
				uchReq = pMes.auchMessage[NB_MESOFFSET0] & (UCHAR)( NB_RSTALLPARA | NB_REQRESETCONSEC | NB_REQLAYOUT | NB_REQ_OPERATOR_MSG) ; /* Save Request Para Inf. */
				ucReqOperatorMessage |= NB_REQ_OPERATOR_MSG;
			}
			else {
				uchReq = pMes.auchMessage[NB_MESOFFSET0] & (UCHAR)( NB_RSTALLPARA | NB_REQRESETCONSEC | NB_REQLAYOUT) ; /* Save Request Para Inf. */
			}

			if ((pMes.auchMessage[NB_MESOFFSET0] & (UCHAR)NB_REQ_OPERATOR_MSG) == 0) {
				ucReqOperatorMessage = 0;
			}

            usSys = pMes.fsSystemInf & NB_RSTSYSFLAG ; /* Save Request Para Inf. */

            UicRPHand(uchReq, usSys);           /* Execute Request parameter handling */

			// Check to see if there is a Flexible Drive Thru message.  If there is then
			// we will notify the FDT subsystem of a change in state.
            uchReq = pMes.auchMessage[NB_MESOFFSET1] & (UCHAR)NB_RSTALLFDT ; /* Save Request FDT */
            if (uchReq) {
                FDTNoticeMessage(uchReq);       /* Request to FDT */
                NbResetMessage(NB_MESOFFSET1, uchReq);
            }

            break;                              /* Exits Loop */
        }

        UicResetExecuteFG();                    /* Reset fsUicexecuteFG */
    }
}

/*
*===========================================================================
** Synopsis:    SHORT  UicChkRequestJob(NBMESSAGE *pMes)
*     Input:    pMes
*     Output:   nothing
*     InOut:    nothing
*
** Return:      UIC_SUCCESS or UIC_EXEC_JOB
*
** Description: Check backup copy flag and request parameter
*===========================================================================
*/
SHORT  UicChkRequestJob(NBMESSAGE *pMes)
{
    USHORT    usMBack;
    UCHAR     uchReq;

    usMBack = (pMes->fsMBackupInf) | (pMes->fsBMBackupInf); /* Save Master Backup Inf. */

    if ( usMBack & NB_EXECUTEBKCOPY )  {  /* if request backup, then return */
        return(UIC_EXEC_JOB);
    }

	if(pMes->auchMessage[NB_MESOFFSET3] & NB_REQSETDBFLAG)
	{
		return(UIC_SUCCESS);
	}

    uchReq = (pMes->auchMessage[NB_MESOFFSET0]) & (UCHAR)( NB_RSTALLPARA | NB_REQRESETCONSEC | NB_REQLAYOUT | NB_REQ_OPERATOR_MSG) ; /* Save Request Para Inf. */

    uchReq |= (pMes->auchMessage[NB_MESOFFSET1]) & (UCHAR)NB_RSTALLFDT ; /* Save Request FDT Inf. */

    if ( uchReq )  {
        return(UIC_EXEC_JOB);             /* If request operara, then return */
    }

    return(UIC_SUCCESS);

}


/*
*===========================================================================
** Synopsis:    SHORT  UicRBCHand(USHORT usMBack)
*     Input:    usMBack
*     Output:   nothing
*     InOut:    nothing
*
** Return:      UIC_SUCCESS
*
** Description: Check backup copy flag and request parameter, then execute
*               if necessary
*===========================================================================
*/
SHORT  UicRBCHand(USHORT usMBack)
{
    UCHAR     uchAdd;
    SHORT     sRet, husKeyBoardHand;

    uchAdd = CliReadUAddr();          /* Get unique address */

    if (uchAdd > UIC_UADDBMASTER) {   /* If terminal is satelite, then return */
        return(UIC_SUCCESS);
    }

    sRet = UicCheckAndSet();                      /* Request keyboard lock */

    if ( 0 < sRet )  {
        husKeyBoardHand = sRet ;                  /* Saves Terminal Handle  */
    }  else {
        return(UIC_SUCCESS);
    }

    switch (usMBack & NB_EXECUTEBKCOPY ) {

    case NB_EXECUTEBKCOPY:            /* If execute backup copy, then call CliReqBackUp */
        sRet = CliReqBackUp();        /* Starts backup copy */
        if (sRet) {
            UicDisplayErrorMsg(OP_FILE_NOT_FOUND);  /* Display error */
        }
		break;

    default:
        break;
    }
    
    sRet = UicResetFlag(husKeyBoardHand);   /* Unlock KeyBoard */

    if ( sRet != UIC_SUCCESS)  {
        PifAbort(MODULE_UIC_ABORT, FAULT_INVALID_HANDLE);    /* Abort  */
        return(sRet);
    }
    return(UIC_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    SHORT  UicRPHand(UCHAR uchReq, USHORT usSys)
*     Input:    uchReq 
*               usSys 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      UIC_SUCCESS
*
** Description: Check Request parameter , then execute if necessary
*===========================================================================
*/
SHORT  UicRPHand(UCHAR uchReq, USHORT usSys)
{
    UCHAR     uchAdd;
    SHORT     sRet, husKeyBoardHand;

    uchAdd = CliReadUAddr();    /* Get unique address */

    switch (uchAdd) {
    case (UIC_UADDMASTER):      /* If terminal is Master */
        if (usSys & NB_MTUPTODATE) {
			NHPOS_NONASSERT_NOTE("==STATE", "==STATE: Master up to date.  Resetting NbMessage.");
            NbResetMessage(NB_MESOFFSET0, NB_RSTALLPARA | NB_REQRESETCONSEC | NB_REQLAYOUT);
            return(UIC_SUCCESS);
        }
        break;

    case (UIC_UADDBMASTER):     /* If terminal is B-Master */
		if ((usSys & NB_BMUPTODATE) != 0) {
			if ((uchReq & (NB_REQSUPER | NB_REQALLPARA | NB_REQPLU)) == 0) {
				NbResetMessage(NB_MESOFFSET0, NB_RSTALLPARA | NB_REQRESETCONSEC | NB_REQLAYOUT);
				return(UIC_SUCCESS);
			}
			// It appears that the Master Terminal sends a message to the Backup Master Terminal every time it
			// is updated including totals updates.
			NbResetMessage(NB_MESOFFSET0, ~(NB_REQSUPER | NB_REQALLPARA | NB_REQPLU));   // if Backup is up to date, then do SUP changes only.
        }
        break;

    default:                   /* If terminal is satellite */
        break;

    }

	// If we are a Disconnected Satellite that is Unjoined then we will
	// just ignore this message
	if (CstIamDisconnectedUnjoinedSatellite() == STUB_SUCCESS) {
        NbResetMessage(NB_MESOFFSET0, NB_RSTALLPARA | NB_REQLAYOUT | NB_REQRESETCONSEC);
        return(UIC_SUCCESS);
	}

    sRet = UicCheckAndSet();                      /* Request keyboard lock */

    if ( 0 < sRet )  {
        husKeyBoardHand = sRet ;                  /* Saves Terminal Handle  */
    }  else {
        return(UIC_SUCCESS);
    }

    if ( uchReq & NB_REQALLPARA) {         /* Request All para , then execute. */
        NbResetMessage(NB_MESOFFSET0, NB_RSTALLPARA);
        PifLog(MODULE_UIC_LOG, LOG_EVENT_UIC_CHANGE_PARA);      /* Write log     */
		NHPOS_NONASSERT_TEXT("NB_REQALLPARA: Start CliReqOpPara(CLI_LOAD_ALL)");
        sRet = CliReqOpPara(CLI_LOAD_ALL);
		{
			char xBuff[128];
			sprintf(xBuff, "NB_REQALLPARA: End CliReqOpPara(CLI_LOAD_ALL) sRet = %d", sRet);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
        PifLog(MODULE_UIC_LOG, LOG_EVENT_UIC_CHANGE_PARA);      /* Write log     */
        PifLog(MODULE_ERROR_NO(MODULE_UIC_LOG), (USHORT)abs(sRet));      /* Write log     */
        if (sRet) {
            NbWriteMessage(NB_MESOFFSET0, NB_REQALLPARA);   /* If error, the set req all para flag */
            UicDisplayErrorMsg(sRet);                       /* Display error */
        }

		// If we are a Disconnected Satellite then we need to request the additional information
		// that is needed by a Disconnected Satellite when it is operating Unjoined as a
		// standalone Master Terminal.  This includes PLU files, Cashier files, etc.
		if (CstIamDisconnectedSatellite() == STUB_SUCCESS) {
			SHORT  sStatus = 0;

			sRet = OpReqFile(_T("PARAM$PL"), 0);
			if (sRet != OP_PERFORM) {
				sStatus = OpConvertError(sRet);
			}

			sRet = OpReqFile(_T("PARAMCAS"), 0);
			if (sRet != OP_PERFORM) {
				sStatus = OpConvertError(sRet);
			}

			sRet = OpReqFile(_T("DFPR_DB"), 0);
			if (sRet != OP_PERFORM) {
				sStatus = OpConvertError(sRet);
			}

			sRet = OpReqFile(_T("PARAMDEP"), 0);
			if (sRet != OP_PERFORM) {
				sStatus = OpConvertError(sRet);
			}

			sRet = OpReqFile(_T("PARAMETK"), 0);
			if (sRet != OP_PERFORM) {
//				sStatus = OpConvertError(sRet);
			}

			sRet = OpReqFile(_T("PARAMIDX"), 0);
			if (sRet != OP_PERFORM) {
				sStatus = OpConvertError(sRet);
			}
		}
    } else {
        if ( uchReq & NB_REQPLU) {            /* Request PLU para, then execute */
            NbResetMessage(NB_MESOFFSET0, NB_REQPLU);
            PifLog(MODULE_UIC_LOG, LOG_EVENT_UIC_CHANGE_PLU);      /* Write log     */
			NHPOS_NONASSERT_TEXT("NB_REQPLU: Start CliReqOpPara(CLI_LOAD_PLU)");
            sRet = CliReqOpPara(CLI_LOAD_PLU);
			{
				char xBuff[128];
				sprintf(xBuff, "NB_REQPLU: End CliReqOpPara(NB_REQPLU) sRet = %d", sRet);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
            PifLog(MODULE_UIC_LOG, LOG_EVENT_UIC_CHANGE_PLU);      /* Write log     */
			PifLog(MODULE_ERROR_NO(MODULE_UIC_LOG), (USHORT)abs(sRet));      /* Write log     */
            NbResetMessage(NB_MESOFFSET0, NB_REQPLU);
            if (sRet) {
                NbWriteMessage(NB_MESOFFSET0, NB_REQPLU);       /* If error, then set req PLU flag */
                UicDisplayErrorMsg(sRet);                       /* Display error */
            }
        }

        if ( uchReq & NB_REQSUPER) {           /* Request SUP para, then execute */
            NbResetMessage(NB_MESOFFSET0, NB_REQSUPER);
            PifLog(MODULE_UIC_LOG, LOG_EVENT_UIC_CHANGE_SUP);      /* Write log     */
			NHPOS_NONASSERT_TEXT("NB_REQSUPER: Start CliReqOpPara(CLI_LOAD_SUP)");
            sRet = CliReqOpPara(CLI_LOAD_SUP);
			{
				char xBuff[128];
				sprintf(xBuff, "NB_REQSUPER: End CliReqOpPara(CLI_LOAD_SUP) sRet = %d", sRet);
				NHPOS_NONASSERT_TEXT(xBuff);
			}
            PifLog(MODULE_UIC_LOG, LOG_EVENT_UIC_CHANGE_SUP);      /* Write log     */
			PifLog(MODULE_ERROR_NO(MODULE_UIC_LOG), (USHORT)abs(sRet));      /* Write log     */
            NbResetMessage(NB_MESOFFSET0, NB_REQSUPER);
            if (sRet) {
                NbWriteMessage(NB_MESOFFSET0, NB_REQSUPER);    /* If error, the set req SUP flag */
                UicDisplayErrorMsg(sRet);                      /* Display error */
            }
        }
    }

	if ( uchReq & NB_REQLAYOUT) {           /* Request Layout, then execute */ //ESMITH LAYOUT
		NbResetMessage(NB_MESOFFSET0, NB_REQLAYOUT);
		PifLog(MODULE_UIC_LOG, LOG_EVENT_UIC_CHANGE_LAY);      /* Write log     */
		NHPOS_NONASSERT_TEXT("NB_REQLAYOUT: Start CliReqOpPara(CLI_LOAD_LAY)");
		sRet = CliReqOpPara(CLI_LOAD_LAY);
		{
			char xBuff[128];
			sprintf(xBuff, "NB_REQLAYOUT: End CliReqOpPara(CLI_LOAD_LAY) sRet = %d", sRet);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
		PifLog(MODULE_UIC_LOG, LOG_EVENT_UIC_CHANGE_LAY);      /* Write log     */
		PifLog(MODULE_ERROR_NO(MODULE_UIC_LOG), (USHORT)abs(sRet));      /* Write log     */
		NbResetMessage(NB_MESOFFSET0, NB_REQLAYOUT);
		if (sRet) {
			NbWriteMessage(NB_MESOFFSET0, NB_REQLAYOUT);    /* If error, the set req SUP flag */
			UicDisplayErrorMsg(sRet);                      /* Display error */
		}
	}

    if ( uchReq & NB_REQRESETCONSEC ) {      /* Request reset consecutive no. */
        NbResetMessage(NB_MESOFFSET0, NB_REQRESETCONSEC);
		MaintResetSpcCo(SPCO_CONSEC_ADR);
        PifLog(MODULE_UIC_LOG, LOG_EVENT_UIC_RESET_CONSECUTIVE);
    }

    if ( uchReq & NB_REQ_OPERATOR_MSG ) {      /* display operator message received. */
		USHORT  usMessageBufferEnum = 0xffff;
		OpDisplayOperatorMessage (&usMessageBufferEnum);
		NbResetMessage(NB_MESOFFSET0, NB_REQ_OPERATOR_MSG);
    }

    sRet = UicResetFlag(husKeyBoardHand);   /* Unlock KeyBoard */

    if ( sRet != UIC_SUCCESS)  {
        PifAbort(MODULE_UIC_ABORT, FAULT_INVALID_HANDLE);    /* Abort  */
        PifAbort(MODULE_ERROR_NO(MODULE_UIC_ABORT), (USHORT)abs(sRet));    /* Abort  */
        return(sRet);
    }
    
    return(UIC_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    SHORT UicResetExecuteFG(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing 
*
** Description: Reset fsUicExecuteGF.
*===========================================================================
*/
VOID  UicResetExecuteFG(VOID)
{
    PifRequestSem(husUicFlagSem);
    fsUicExecuteFG = UIC_IDLESTATE;           /* Sets Idle  state */
    PifReleaseSem(husUicFlagSem);
}


/*
*===========================================================================
** Synopsis:    VOID    UicDisplayErrorMsg(SHORT sError);
*     Input:    sError   - Error Code
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Display error message.
*===========================================================================
*/
#if defined(UicDisplayErrorMsg)
#pragma message("UicDisplayErrorMsg defined")
VOID   UicDisplayErrorMsg_Special(SHORT sError);
VOID   UicDisplayErrorMsg_Debug(SHORT sError, char *aszFilePath, int nLineNo)
{
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	sprintf (xBuffer, "UicDisplayErrorMsg_Debug(): sError = %d, File %s, lineno = %d", sError, aszFilePath + iLen, nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);
	UicDisplayErrorMsg_Special(sError);
}

VOID   UicDisplayErrorMsg_Special(SHORT sError)
#else
VOID   UicDisplayErrorMsg(SHORT sError)
#endif
{
    USHORT  usLeadthru;

    usLeadthru = OpConvertError(sError);       /* Convert error cord */

	if (sError != 0) {
		PifLog (MODULE_UIC_LOG, LOG_EVENT_STB_CSTDISPLAYERROR);
		PifLog (MODULE_UIC_LOG, (USHORT)(sError * (-1)));
	}

    UieErrorMsg(usLeadthru, UIE_WITHOUT);      /* Display */
}

/*====== End of Source File ======*/


