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
* Title       : Client/Server STUB, Total Function Source File                        
* Category    : Client/Server STUB, US Hospitality Model
* Program Name: CSSTBTTL.C                                            
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*#              CliTtlUpdateFile()          Update Total file
*                   CstTtlMasterReset()     Master reset information
*#                  CstTtlUpdate()          Update Total
*#                      CstTtlUpdateFile()  Update Total on self
*
*               SerTtlTotalRead()           Read Specified total file
*               SerTtlTotalIncrRead()       Read Specified total file incrementally
*               SerTtlTotalReset()          Reset Specified total file
*               SerTtlIssuedSet()           Set issued report flag
*               SerTtlIssuedCheck()         Check issued report flag
*                  CstTtlResetConsecCo()    Reset consecutive counter
**
*               CliTtlUpdateFileFH()        Update Total file(file handle I/f version)
*                   CstTtlUpdateFH()        Update Total(file handle I/f version)
*                       CstTtlUpdateFileFH()Update Total on self(file handle I/f version)
*
*               SerTtlIndFinTotalCheck()    Check individual financial total
*
*               SerTtlReadWriteChkAndWait A Check update total
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-08-92:H.Nakashima: initial                                   
* Apr-17-93:H.Yamaguchi: Adds reset consec. no at AC 18 or 99.
* Jun-04-93:H.Yamaguchi: Modified same as HP Int'l
* Jul-27-93:H.Yamaguchi: Adds CstInitTtlUpdMsgH/CstInitTtlRstMsgH
* Dec-09-93:H.Yamaguchi: Adds SerTtlReadWriteChk                                   
* Mar-15-94:K.You      : Adds flex GC file feature.(add CliTtlUpdateFileFH,
*          :           : CstTtlUpdateFH, CstTtlUpdateFileFH)
* May-17-94:Yoshiko.Jim: add SerTtlWaiTotalCheck(), SerTtlCasTotalCheck()
* Nov-14-95:T.Nakahata : R3.1 Initial
*       Add Service Time Total (Daily/PTD) and Individual Financial (Daily)
* Aug-11-99:M.Teraki   : R3.5 remove WAITER_MODEL
* Dec-03-99:hrkato     : Saratoga
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
#include    <memory.h>
#include    <math.h>
#include    <stdio.h>

#include    "ecr.h"
#include    "pif.h"
#include	<rfl.h>
#include    "plu.h"
#include    "csttl.h"
#include    "csstbttl.h"
#include    "paraequ.h"
#include    "para.h"
#include	"pararam.h"
#include    "nb.h"
#include    "csstbpar.h"
#include    "csstbfcc.h"
#include    "csserm.h"
#include    "csstubin.h"
#include    "ttl.h"
#include	"display.h"
#include	"appllog.h"
#include	"ej.h"
#include	"csstbej.h"
#include	"maint.h"

static USHORT      usCliTranNo = 0;            /* transaction # of my terminal */

extern XGRAM       CliSndBuf;          /* Send Buffer */    
extern XGRAM       CliRcvBuf;          /* Receive Buffer */
extern  CLICOMIF   CliMsg;


/*
*===========================================================================
** Synopsis:    SHORT   CstTtlMasterReset(VOID);
*
** Return:      TTL_SUCCESS:    success to read file
*
** Description: This function supports to inform master reset.
*===========================================================================
*/
SHORT   CstTtlMasterReset(VOID)
{
    CLIREQTOTAL     ReqMsgH = {0};
    CLIRESTOTAL     ResMsgH = {0};
    SHORT           sError;            
    SHORT           sCliRetCode;
    SHORT			sErrorBM = STUB_SUCCESS, sErrorM = STUB_SUCCESS;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCRESETTRANSNO | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQTOTAL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);

    if (STUB_SELF == (sErrorM = CstSendMaster())) {
        SstResetIndTransNo(CLI_TGT_MASTER);
		usCliTranNo =  0;
        CliMsg.sRetCode = TTL_SUCCESS;
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	sCliRetCode = CliMsg.sRetCode;
    if (STUB_SELF == (sErrorBM = CstSendBMaster())) {
        SstResetIndTransNo(CLI_TGT_BMASTER);
		usCliTranNo =  0;
		sErrorBM = STUB_SUCCESS;
		CliMsg.sRetCode = TTL_SUCCESS;
    } else if (STUB_SUCCESS == sErrorM || STUB_SUCCESS != sErrorBM) {
		// If there was an error talking to the Backup Master Terminal
		// then we will return the status of the Master Terminal request.
		CliMsg.sRetCode = sCliRetCode;
	}

	sError = CliMsg.sRetCode;

	if (sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CstTtlMasterReset() sError %d, sErrorM %d, sErrorBM %d", sError, sErrorM, sErrorBM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return sError;
}

/*
*===========================================================================
** Synopsis:    VOID  CstTtlSetCliTranNo (USHORT usTerminalPosition)
*
*     In:       usTerminalPosition - terminal number (1 - 16)
*
** Return:      Nothing
*
** Description: This function forces the static variable usCliTranNo, which
*               is used with total update, to be reset to what ever the
*               current value is in the Inquiry state data for this terminal.
*               This is used with the Join command of the new Mobile Register
*               functionality so as to synchronize the transaction sequence
*               number expected by the Master Terminal.
*===========================================================================
*/
VOID  CstTtlSetCliTranNo (USHORT usTerminalPosition)
{
    SERINQSTATUS    InqData = {0};

    SstReadInqStat(&InqData);

	usCliTranNo = InqData.ausTranNo[usTerminalPosition - 1];
}


/*
*===========================================================================
** Synopsis:    SHORT   SerTtlTotalRead(VOID *pTotal);
*     InOut:    *pTotal     - store specified total file
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file     
*
** Description: This function supports to read Total file.
*===========================================================================
*/
SHORT   SerTtlTotalRead(VOID *pTotal)
{
	//US Customs for Rel 2.x JHHJ 7-25-05
	//We can now send a request to the master for the information requested
	return CliTtlTotalRead(pTotal);
}

VOID  SerTtlStartTransferredTotalsUpdate (USHORT  usTerminalPosition)
{
	static USHORT  usTerminalNumbers[32];

	usTerminalNumbers[usTerminalPosition] = usTerminalPosition;
	PifWriteQueue(usTtlQueueJoinUpdate, usTerminalNumbers + usTerminalPosition);  // Release Semaphore for TtlTUMJoinDisconnectedSatellite() to start
}

VOID  SerTtlStartUpdateTotalsMsg (USHORT  usTerminalPosition)
{
	static USHORT  usTerminalNumbers[32];

	usTerminalNumbers[usTerminalPosition] = usTerminalPosition;
	PifWriteQueue(usTtlQueueUpdateTotalsMsg, usTerminalNumbers + usTerminalPosition);  // Release Semaphore for TtlTUMUpdateTotalsMsg() to start
}


/*
*===========================================================================
** Synopsis:    SHORT   SerTtlTotalIncrRead(VOID *pTotal);
*     InOut:    *pTotal     - store specified total file
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file     
*
** Description: This function supports to read Total file incrementally.
*===========================================================================
*/
SHORT   SerTtlTotalIncrRead(VOID *pTotal)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return TtlTotalIncrRead(pTotal);
    }else
	{
		//US Customs for Rel 2.x JHHJ 7-25-05
		//We can now send a request to the master for the infromation requested
		return CliTtlTotalReadIncr(pTotal);
	}
    return TTL_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerTtlTotalReset(VOID *pTotal,
*                                        USHORT usMDCBit);
*     InOut:    *pTotal     - store specified total file
*               usMDCBit   - A/C 99, 18 execution bit
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file     
*               TTL_NOTINFILE:  not exist total file
*               TTL_NOT_ISSUED: report has not beed issued
*               TTL_LOCKED:     total file locked
*
** Description:  This function supports to reset Total file.
*===========================================================================
*/
SHORT   SerTtlTotalReset(VOID *pTotal, USHORT usMDCBit)
{
    TTLCASHIER      *pTtl = pTotal;
    CLIREQRESETTL   ReqMsgH = {0};
    CLIRESTOTAL     ResMsgH = {0};
    SERINQSTATUS    InqData = {0};
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.uchMajorClass = pTtl->uchMajorClass;
    ReqMsgH.uchMinorClass = pTtl->uchMinorClass;
    ReqMsgH.ulIDNO        = pTtl->ulCashierNumber;
    ReqMsgH.usExeMDC      = usMDCBit;
    SstReadInqStat(&InqData);
    memcpy(ReqMsgH.ausTranNo,    InqData.ausTranNo,    2*CLI_ALLTRANSNO);
    memcpy(ReqMsgH.ausPreTranNo, InqData.ausPreTranNo, 2*CLI_ALLTRANSNO);
       
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    switch(pTtl->uchMajorClass) {
    case    CLASS_TTLCASHIER:
        CliMsg.usFunCode = CLI_FCTTLRESETCAS;
        break;
    case    CLASS_TTLINDFIN:
        CliMsg.usFunCode = CLI_FCTTLRESETINDFIN;
        break;
        
    default:
        CliMsg.usFunCode = CLI_FCTTLRESETTL;
        break;
    }
    CliMsg.usFunCode   |= CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);
    CliMsg.sRetCode     = TTL_SUCCESS;

    if (STUB_SELF == SstUpdateAsMaster()) {
		// save our CliMsg data and release the semaphore to allow other threads
		// access to client side functionality until such time as we need to
		// continue our process.
		CLICOMIF   CliMsgSave = CliMsg;
		PifReleaseSem(husCliExeNet);

        if ((CLASS_TTLEODRESET == pTtl->uchMajorClass) || (CLASS_TTLPTDRESET == pTtl->uchMajorClass)) {
			ProgRptLog(100);
            while(TTL_SUCCESS != TtlReadWrtChk()) { /* total still exist */
                PifSleep(100);
            }
			ProgRptLog(101);
        }
	    /* 09/03/01 */
    	sRetCode = TtlPLUGetBackupStatus();
	    while (sRetCode == TTL_BK_STAT_BUSY) {
    	    PifSleep(100);
        	sRetCode = TtlPLUGetBackupStatus();
	    }

		//  Get the semaphore back and restore our CliMsg data in case it was changed
		//  by some other thread.
		PifRequestSem(husCliExeNet);
		CliMsg = CliMsgSave;

		ProgRptLog(103);
        sRetCode = TtlTotalReset(pTotal, usMDCBit);
		ProgRptLog(104);
        if (TTL_SUCCESS == sRetCode) {
            if ((CLASS_TTLEODRESET == pTtl->uchMajorClass) || (CLASS_TTLPTDRESET == pTtl->uchMajorClass)) {
                SstResetCurTransNo();
                /*--- Reset Consec Co  for all satellite ----*/
				ProgRptLog(105);
                CstTtlResetConsecCo(pTtl->uchMajorClass, (UCHAR)usMDCBit) ;
				ProgRptLog(106);
            }
        }
		if (TTL_SUCCESS == sRetCode) {
			SstUpdateBackUp();
		}
    } else {
        sRetCode = TTL_NOT_MASTER;
    }

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerTtlIssuedSet(VOID *pTotal,
*                                       UCHAR fIssued);
*     InPut:    *pTotal     - store specified total file
*               fIssued     - status of report flag 
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file     
*               TTL_LOCKED:     total file locked
*
** Description:  This function supports to set report issued flag.
*===========================================================================
*/
SHORT   SerTtlIssuedSet(VOID *pTotal, UCHAR fIssued)
{
    TTLCASHIER      *pTtl = pTotal;
    CLIREQRESETTL   ReqMsgH = {0};
    CLIRESTOTAL     ResMsgH = {0};
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.uchMajorClass = pTtl->uchMajorClass;
    ReqMsgH.uchMinorClass = pTtl->uchMinorClass;
    ReqMsgH.ulIDNO      = pTtl->ulCashierNumber;
    ReqMsgH.fIssued     = fIssued;
       
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLISSUE | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = TtlIssuedSet(pTotal, fIssued);
    } else {
        sRetCode = TTL_NOT_MASTER;
    }
    if (TTL_SUCCESS == sRetCode) {
        SstUpdateBackUp();
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerTtlIssuedCheck(VOID *pTotal, UCHAR fIssued);
*     InPut:    *pTotal     - specify total file
*               fIssued     - status of report flag 
*
** Return:      TTL_ISSUED:     success to read file
*               TTL_FAIL:       failed to read file     
*               TTL_LOCKED:     total file locked
*
** Description:
*   This function supports to check whether total is printed or not.
*===========================================================================
*/
SHORT   SerTtlIssuedCheck(VOID *pTotal, UCHAR fIssued)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return TtlIssuedCheck(pTotal, fIssued);
    } 
    return TTL_NOT_MASTER;
}

// Description:  This function performs a check on the specified Totals bucket
//               to determine if the various classes of Totals are indicating that
//               they are being reset or not. The purpose of this function is to
//               perform an audit on the various totals to determine if the last
//               End of Day or other total reset was completed successfully or not.
SHORT   SerTtlIssuedCheckAllReset(VOID)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return TtlIssuedCheckAllReset(CLASS_TTLSAVDAY);
    } 
    return TTL_NOT_MASTER;
}
/*
*===========================================================================
** Synopsis:    SHORT   SerTtlIssuedCheck(VOID *pTotal, UCHAR fIssued);
*     InPut:    *pTotal     - specify total file
*               fIssued     - status of report flag 
*
** Return:      TTL_ISSUED:     success to read file
*               TTL_FAIL:       failed to read file     
*               TTL_LOCKED:     total file locked
*
** Description:
*   This function supports to check whether total is printed or not.
*===========================================================================
*/
SHORT   SerTtlTumUpdateDelayedBalance(VOID)
{
	SHORT	    sRet = TTL_NOT_MASTER;

    if (STUB_SELF == SstReadAsMaster()) {
        sRet = TtlTumUpdateDelayedBalance();
		if((uchMaintDelayBalanceFlag) && (sRet == TTL_SUCCESS))
		{

			//set the flag to turn off after we update the totals
			//and clear the flag that signals that we are in delay balance mode
			uchMaintDelayBalanceFlag = 0;

			PifLog(MODULE_STB_LOG, LOG_EVENT_DB_FLAG_OFF_1);

			/*The following PM_CNTRL was not being used, so we use this one
			to display the D-B message on the display*/
			flDispRegDescrControl &= ~PM_CNTRL;
			flDispRegKeepControl &= ~PM_CNTRL;

			//We set the class so that we can send the dummy data,
			//and trigger the discriptor to show D-B to inform the user that
			//the terminal is no longer in delay balance mode
			RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0);

			//Insert EJCall to Reconcile EJ Files.
			EJDelayBalanceUpdateFile();

			EJWriteDelayBalance(uchMaintDelayBalanceFlag);
		}

		if(sRet != TTL_SUCCESS)
		{
			PifLog(MODULE_STB_LOG, LOG_EVENT_DB_TTLUPDATE_FAIL);
			PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)abs(sRet));
		}

		PifSaveFarData();
    }

	return sRet;
}
/*
*===========================================================================
** Synopsis:    SHORT   SerTtlIssuedReset(VOID *pTotal,
*                                         UCHAR fIssued);
*     InPut:    *pTotal     - store specified total file
*               fIssued     - status of report flag 
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file     
*               TTL_LOCKED:     total file locked
*
** Description:  This function supports to set report issued flag.
*===========================================================================
*/
SHORT   SerTtlIssuedReset(VOID *pTotal, UCHAR fIssued)
{
    TTLCASHIER      *pTtl = pTotal;
    CLIREQRESETTL   ReqMsgH = {0};
    CLIRESTOTAL     ResMsgH = {0};
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);

    ReqMsgH.uchMajorClass = pTtl->uchMajorClass;
    ReqMsgH.uchMinorClass = pTtl->uchMinorClass;
    ReqMsgH.ulIDNO      = pTtl->ulCashierNumber;
    ReqMsgH.fIssued     = fIssued;
       
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLISSUERST | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = TtlIssuedReset(pTotal, fIssued);
    } else {
        sRetCode = TTL_NOT_MASTER;
    }
    if (TTL_SUCCESS == sRetCode) {
        SstUpdateBackUp();
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerTtlTotalCheck(VOID *pTotal);
*     InPut:    *pTotal     - specify total file
*
** Return:      TTL_SUCCESS
*
** Description:  This function supports to check whether total is 0 or not.
*===========================================================================
*/
SHORT   SerTtlTotalCheck(VOID *pTotal)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return TtlTotalCheck(pTotal);
    } 
    return TTL_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   CstTtlResetConsecCo(UCHAR  uchMajorClass
*                                           UCHAR  uchMDCBit);
*     InPut:    uchMajorClass - specify EOD or PTD
*               uchMDCBit     - specify EOD/RESET MDC Bit
*
** Return:      STUB_SUCCESS
*
** Description:
*   This function supports to reset consecutive counter for all satellite.
*===========================================================================
*/
SHORT   CstTtlResetConsecCo(UCHAR uchMajorClass, UCHAR  uchMDCBit)
{
    UCHAR   uchExecMDC;

    /*--- Check Option MDC Bit of Reset Consecutive counter ---*/ 

    if ( CliParaMDCCheck(MDC_SUPER_ADR , ODD_MDC_BIT1) ) {

        if ( CLASS_TTLEODRESET == uchMajorClass ) {
            uchExecMDC = CliParaMDCCheck(MDC_EOD1_ADR , /* Addr 199 EOD */
                                         ODD_MDC_BIT0 | /* Financial */
                                         ODD_MDC_BIT1 | /* Dept      */ 
                                         ODD_MDC_BIT3); /* Hourly    */
        } else {
            uchExecMDC = CliParaMDCCheck(MDC_PTD1_ADR , /* Addr 201 PTD */
                                         ODD_MDC_BIT0 | /* Financial */
                                         ODD_MDC_BIT1 | /* Dept      */ 
                                         ODD_MDC_BIT3); /* Hourly    */
        }

        if ( ( 0 == ( ODD_MDC_BIT0 & uchExecMDC )) && ( 0 == ( ODD_MDC_BIT0 & uchMDCBit  )) ) {  
            /* Exec Fin and request reset Fin */
            NbWriteMessage(NB_MESOFFSET0, NB_REQRESETCONSEC); 
        } else if ( ( 0 == ( ODD_MDC_BIT3 & uchExecMDC )) && 
                    ( 0 == ( ODD_MDC_BIT3 & uchMDCBit  )) &&  
                    (      ( ODD_MDC_BIT0 & uchExecMDC )) ){ 

            /* Exec Hou and req reset Hou and Not exec Fin */
            NbWriteMessage(NB_MESOFFSET0, NB_REQRESETCONSEC); 
        } else if ( ( 0 == ( ODD_MDC_BIT1 & uchExecMDC )) &&
                    ( 0 == ( ODD_MDC_BIT1 & uchMDCBit  )) &&
                    (( ODD_MDC_BIT0 | ODD_MDC_BIT3 ) == uchExecMDC ) ) {

            /* Exec Dept and request Dept and not exec Fin and not exec Hou */
            NbWriteMessage(NB_MESOFFSET0, NB_REQRESETCONSEC); 
        }
    }
    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerTtlReadWriteChkAndWait(VOID);
*     InPut:    usTime   Delay Timer Value
*
** Return:     STUB_SUCESS   
*
** Description:
*   If update total is not 0, then wait for designate time.
*===========================================================================
*/
SHORT   SerTtlReadWriteChkAndWait(USHORT usTime)
{
	if (STUB_SELF == SstReadAsMaster())
	{
		if (TTL_SUCCESS == TtlReadWrtChk()) {
			return STUB_SUCCESS;
		}
		if ( usTime ) {
			PifSleep(usTime);  /* Delay usTime Ms */
		}
	}
    return STUB_SUCCESS;
}


/*
*===========================================================================
** Synopsis:    SHORT   CstTtlUpdateFileFH(USHORT usOffset,
*                                           SHORT hsFileHandle,
*                                           USHORT usSize);
*     InPut:    usOffset    - read start pointer
*               hsFileHandle    - file handle of writing data file
*               usSize      - size of file  
*
** Return:      TTL_SUCCESS:    successful updating
*               TTL_FAIL:       fail to update
*               TTL_LOCKED:     total file is still loked
*               TTL_BUFFER_FULL:    update bufer full (BUSY)
*
** Description: This function is a sub function of the total update function
*               (file handle I/F version).
*===========================================================================
*/
static SHORT   CstTtlUpdateFileFH(ULONG ulItemOffset, SHORT hsFileHandle, USHORT usSize, UCHAR *puchDelayBalance)
{
    SHORT   sTtlTryError = TTL_SUCCESS;
	SHORT	sRetry = 5;

//    sRetry = CstReadMDCPara(CLI_MDC_RETRY);
    do {
        if (TTL_BUFFER_FULL == sTtlTryError) {
            PifSleep (250);  // If the total buffer is full of pending transactions, give it a bit of time and then retry
        }
        sTtlTryError = TtlUpdateFileFH(ulItemOffset, hsFileHandle, usSize, puchDelayBalance);
    } while ((TTL_BUFFER_FULL == sTtlTryError) && (0 <= --sRetry));

	NHPOS_ASSERT(TTL_SUCCESS == sTtlTryError);
    return sTtlTryError;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliTtlUpdateFileFH(USHORT usOffset,
*                                          SHORT hsFileHandle,
*                                          USHORT usLength);
*     InPut:    usOffset        - read start pointer
*               hsFileHandle    - file handle of writing data file
*               usLength        - size of file  
*
** Return:      TTL_SUCCESS:    successful updating
*               TTL_FAIL:       fail to update
*               TTL_LOCKED:     total file is still loked
*               TTL_BUFFER_FULL:    update bufer full (BUSY)
*
** Description: This function supports to update Total(file handle I/F version).
*===========================================================================
*/
SHORT   CliTtlUpdateFileFH(ULONG ulOffset, SHORT hsFileHandle, USHORT usSize)
{
	static SHORT    sErrorMsave = 0;
	static SHORT    sErrorBMsave = 0;
    SHORT           sError;
    CLIREQTOTAL     ReqMsgH = {0};
    CLIRESTOTAL     ResMsgH = {0};
    SHORT           sBRetCode = 0;            
    SHORT           sCliRetCode;
	SHORT			sErrorM = 0, sErrorBM = 0;
	USHORT          usCliTranNoTemp = 0;
	UCHAR			uchDelayBalance;

    PifRequestSem(husCliExeNet);

    if (0 == usCliTranNo) {
        CstTtlMasterReset();
    }

    ReqMsgH.usTransNo   = usCliTranNo + 1;

    if (0 == ReqMsgH.usTransNo) {           /* Add "0" Check, Dec/1/2000 */
        ReqMsgH.usTransNo = 1;
    }
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLUPDATE | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQTOTAL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);
    CliMsg.pauchReqData = ( UCHAR *)&hsFileHandle;
    CliMsg.usReqLen     = usSize;

    if (STUB_SELF == (sErrorM = CstSendMasterFH(!CLI_FCBAKGCF))) {
        CliMsg.sRetCode = CstTtlUpdateFileFH(ulOffset, hsFileHandle, usSize, &uchDelayBalance);
        if (TTL_SUCCESS == CliMsg.sRetCode) {
            usCliTranNo = SstIncTransNo(CLI_TGT_MASTER, ReqMsgH.usTransNo);
                                /* Change ++usCliTranNo to ReqMsgH.usTransNo, Dec/1/2000 */
        }
		sErrorM = STUB_SUCCESS;
    }
	else if (sErrorM >= 0 && TTL_SUCCESS == CliMsg.sRetCode) {
        usCliTranNo = ResMsgH.usTransNo;
	}

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

	sCliRetCode = CliMsg.sRetCode;
    if (STUB_SELF == (sErrorBM = CstSendBMasterFH(!CLI_FCBAKGCF))) {
        CliMsg.sRetCode = CstTtlUpdateFileFH(ulOffset, hsFileHandle, usSize,&uchDelayBalance);
		sErrorBM = STUB_SUCCESS;
		if (TTL_SUCCESS == CliMsg.sRetCode) {
			/* Change ++usCliTranNo to ReqMsgH.usTransNo, Dec/1/2000 */
			usCliTranNoTemp = SstIncTransNo(CLI_TGT_BMASTER, ReqMsgH.usTransNo);
		}
        if (STUB_SUCCESS != sErrorM) {
			if (TTL_SUCCESS == CliMsg.sRetCode) {
				usCliTranNo = usCliTranNoTemp;
			}
			else {
				CliMsg.sRetCode = sCliRetCode;
			}
		}
    }
	else  if (sErrorBM != STUB_SUCCESS) {
		// If there was an error in sending to the Backup Master
		// then lets report it but we want to return
		// a success so that execution will continue regardless.
		CliMsg.sRetCode = sCliRetCode;
	}

	sError = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);

	CstSetMasterBackupStatus (sErrorM, sErrorBM);
	if (sError || sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		if (sError || sErrorMsave != sErrorM || sErrorBMsave != sErrorBM) {
			USHORT  usConsecutive = MaintCurrentSpcCo(SPCO_CONSEC_ADR);
			char    xBuff[128];

			PifLog(MODULE_STB_LOG, LOG_ERROR_TTL_BM_UPDATE);
			PifLog(MODULE_DATA_VALUE(MODULE_STB_LOG), usConsecutive);
			PifLog(MODULE_DATA_VALUE(MODULE_STB_LOG), ReqMsgH.usTransNo);
			PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)abs(sError));
			PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)abs(sErrorM));
			PifLog(MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)abs(sErrorBM));
			sprintf (xBuff, "==NOTE: CliTtlUpdateFileFH() consec %d usTransNo %d sError %d, sErrorM %d, sErrorBM %d", usConsecutive, ReqMsgH.usTransNo, sError, sErrorM, sErrorBM);
			NHPOS_NONASSERT_TEXT(xBuff);
			sErrorMsave = sErrorM;
			sErrorBMsave = sErrorBM;
		}
	}
    return sError;
}


/*
*===========================================================================
** Synopsis:    SHORT   SerTtlCasTotalCheck(VOID);
*     InPut:    none
*
** Return:      TTL_SUCCES      - Successed the non Deletting Record 
*               TTL_DELCASWAI   - Successed the Deletting Record
*               TTL_LOCKED      - File Locked
*               TTL_FAIL        - fail error
*               TTL_FILE_HANDLE_ERR - Occured File Handle Error
*
** Description:
*   This function supports to check cashier total.
*===========================================================================
*/
SHORT   SerTtlCasTotalCheck(VOID)
{
    if (STUB_SELF == SstReadAsMaster()) {
        return TtlCasTotalCheck();
    } 
    return TTL_NOT_MASTER;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliTtlTotalRead(VOID *pTotal);
*     InOut:    *pTotal     - store specified total file
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file
*
** Description:
*   This function supports to read Total file.          Saratoga
*===========================================================================
*/
SHORT   CliTtlTotalRead(VOID *pTotal)
{
	TTLDEPT		    *pDept = pTotal;
	USHORT          usDataLength = 0;
    SHORT           sRetCode, sErrorM = STUB_SUCCESS, sErrorBM = STUB_SUCCESS;
    CLIREQRESETTL   ReqMsgH = {0};
    CLIRESTOTAL     ResMsgH = {0};

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));

    CliMsg.usFunCode    = CLI_FCTTLREADTL;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);

    CliMsg.pauchReqData = (UCHAR *)pTotal;
    CliMsg.usReqLen     = sizeof(CLITTLCASWAI);
    CliMsg.pauchResData = (UCHAR *)pTotal;

	CliMsg.usResLen = CliRecvTtlGetLen(pDept->uchMajorClass);

#if 0
    if (STUB_SELF == (sErrorM = CstSendMaster())) {     /* --- Send Master Terminal --- */
        CliMsg.sRetCode = TtlTotalRead(pTotal, &usDataLength);
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

    if (sErrorM != STUB_SUCCESS) {
        if (STUB_SELF == (sErrorBM = CstSendBMaster())) {     /* --- Send BMaster Terminal --- */
            CliMsg.sRetCode = TtlTotalRead(pTotal, &usDataLength);
			sErrorBM = STUB_SUCCESS;
        }
    }
#else
    if (STUB_SELF == (sErrorM = SstReadFAsMaster())) {     /* --- Send Master Terminal --- */
        CliMsg.sRetCode = TtlTotalRead(pTotal, &usDataLength);
		sErrorM = STUB_SUCCESS;
    }
#endif

    sRetCode = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	if (sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		TTLREPORT  *pTtlReport = (TTLREPORT  *)pTotal;
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliTtlTotalRead() class %d, %d - sRetCode %d, sErrorM %d, sErrorBM %d", pTtlReport->uchMajorClass, pTtlReport->uchMinorClass, sRetCode, sErrorM, sErrorBM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return sRetCode;
 
}

/*
*===========================================================================
** Synopsis:    SHORT   CliTtlTotalRead(VOID *pTotal);
*     InOut:    *pTotal     - store specified total file
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file
*
** Description:
*   This function supports to read Total file.          Saratoga
*===========================================================================
*/
SHORT   CliTtlTotalReadIncr(VOID *pTotal)
{
	USHORT          usDataLength = 0;
    SHORT           sRetCode;
    SHORT			sErrorBM = STUB_SUCCESS, sErrorM = STUB_SUCCESS;
	CLIREQRESETTL   ReqMsgH = {0};
    CLIRESTOTAL     ResMsgH = {0};

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));

    CliMsg.usFunCode    = CLI_FCTTLREADTLINCR;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);

    CliMsg.pauchReqData = (UCHAR *)pTotal;
    CliMsg.usReqLen     = sizeof(CLITTLCASWAI);
    CliMsg.pauchResData = (UCHAR *)pTotal;
    CliMsg.usResLen     = sizeof(TTLHOURLY);

#if 0
    if (STUB_SELF == (sErrorM = CstSendMaster())) {     /* --- Send Master Terminal --- */
        CliMsg.sRetCode = TtlTotalRead(pTotal, &usDataLength);
		sErrorM = STUB_SUCCESS;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sError = STUB_M_DOWN;
		CliMsg.sRetCode = STUB_RETCODE_M_DOWN_ERROR;
	}

    if (sErrorM != STUB_SUCCESS) {
        if (STUB_SELF == (sErrorBM = CstSendBMaster())) {     /* --- Send BMaster Terminal --- */
            CliMsg.sRetCode = TtlTotalRead(pTotal, &usDataLength);
			sErrorBM = STUB_SUCCESS;
        }
    }
#else
    if (STUB_SELF == (sErrorM = SstReadFAsMaster())) {     /* --- Send Master Terminal --- */
        CliMsg.sRetCode = TtlTotalRead(pTotal, &usDataLength);
		sErrorM = STUB_SUCCESS;
    }
#endif

    sRetCode = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);

	if (sRetCode || sErrorM || (sErrorBM && sErrorBM != STUB_DISCOVERY)) {
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CliTtlTotalReadIncr() sRetCode %d, sErrorM %d, sErrorBM %d", sRetCode, sErrorM, sErrorBM);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
    return sRetCode;
 
}


/*
*===========================================================================
** Synopsis:    SHORT   CliCasTtlTenderAmountRead(USHORT usCashierNumber, VOID *pTtlCas)
*     InOut:    USHORT usCashierNumber
*    Output:    
*
** Return:      TTL_SUCCESS:    Success to read file
*               TTL_FAIL:       Failed  to read file
*
** Description:
*   Cashier cash amount Read from Cashier total File            Saratoga
*===========================================================================
*/
SHORT   CliCasTtlTenderAmountRead(ULONG ulCashierNumber, VOID *pTtlCas)
{
    TTLCASTENDER    *pTender = pTtlCas;
    CLIREQCASTEND   ReqMsgH = {0};
    CLIRESCASTEND   ResMsgH = {0};
    SHORT           sErrorM = STUB_SUCCESS, sErrorBM = STUB_SUCCESS;
    SHORT           sRetCode, sMoved = 0;

    PifRequestSem(husCliExeNet);

    ReqMsgH.ulCashierNumber = ulCashierNumber;

    memset(&CliMsg,  0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLREADTLCASTEND | CLI_FCWBACKUP | CLI_FCCLIENTWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQCASTEND);

    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESCASTEND);
    CliMsg.sRetCode     = TTL_SUCCESS;

#if 0
    if (STUB_SELF == (sErrorM = CstSendMaster())) {     /* --- Send Master Terminal --- */
        pTender->ulCashierNumber = ulCashierNumber;
        CliMsg.sRetCode = TtlTenderAmountRead(pTender);
		sErrorM = STUB_SUCCESS;
        sMoved = 1;
    }

	if (STUB_TIME_OUT == sErrorM) {
		// if we have a time out when talking to the Master Terminal then
		// lets assume that it is down.
		sErrorM = STUB_M_DOWN;
        CliMsg.sRetCode = TTL_FAIL;
	}

    if (sErrorM != STUB_SUCCESS) {
        if (STUB_SELF == (sErrorBM = CstSendBMaster())) {     /* --- Send BMaster Terminal --- */
            pTender->ulCashierNumber = ulCashierNumber;
            CliMsg.sRetCode = TtlTenderAmountRead(pTender);
			sErrorBM = STUB_SUCCESS;
            sMoved = 1;
        }
    }
#else
    if (STUB_SELF == (sErrorM = SstReadFAsMaster())) {     /* --- Send Master Terminal --- */
        pTender->ulCashierNumber = ulCashierNumber;
        CliMsg.sRetCode = TtlTenderAmountRead(pTender);
		sErrorM = STUB_SUCCESS;
        sMoved = 1;
    }
#endif
    if (sMoved == 0) {
		int  i;

        pTender->ulCashierNumber = ulCashierNumber;
        pTender->lCashTender    = ResMsgH.lCashTender;
        pTender->lCheckTender   = ResMsgH.lCheckTender;
        pTender->lChargeTender  = ResMsgH.lChargeTender;
		// copy tender and totals from received input into user's buffer
		// area.  Tenders are usually sized to STD_TENDER_MAX but we will
		// calculate the number of elements to ensure everything works properly.
        for (i = 0; i < sizeof(ResMsgH.lMiscTender)/sizeof(ResMsgH.lMiscTender[0]); i++) {
            pTender->lMiscTender[i] = ResMsgH.lMiscTender[i];
        }
        for (i = 0; i < sizeof(ResMsgH.lForeignTotal)/sizeof(ResMsgH.lForeignTotal[0]); i++) {
            pTender->lForeignTotal[i] = ResMsgH.lForeignTotal[i];
        }
    }

	sRetCode = CliMsg.sRetCode;
    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*===========================================================================
** Synopsis:    SHORT   SerTtlIssuedReset(VOID *pTotal,
*                                         UCHAR fIssued);
*     InPut:    *pTotal     - store specified total file
*               fIssued     - status of report flag 
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file     
*               TTL_LOCKED:     total file locked
*
** Description:  This function supports to set report issued flag.
*===========================================================================
*/
SHORT   SerTtlPLUOptimize(UCHAR uchMinorClass)
{
    CLIREQRESETTL   ReqMsgH = {0};
    CLIRESTOTAL     ResMsgH = {0};
    SHORT           sRetCode = TTL_NOT_MASTER;

    PifRequestSem(husCliExeNet);

    ReqMsgH.uchMajorClass = CLASS_TTLPLU;
    ReqMsgH.uchMinorClass = uchMinorClass;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLPLUOPTIMIZE | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);

    sRetCode = TTL_NOT_MASTER;
    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = TtlPLUOptimize(uchMinorClass);
		if (TTL_SUCCESS == sRetCode) {
			SstUpdateBackUp();
		}
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}
/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerTtlTotalReadPluEx(VOID FAR *pTotal)
*     InOut:    *pTotal 	- store specified total file
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file     
*
** Description:
*   This function supports to reads 20 PLU Total file.	Saratoga
*===========================================================================
*/
SHORT SerTtlTotalReadPluEx(VOID *pTotal)
{
    CLIREQRESETTL  	ReqMsgH = {0};
    CLIRESTOTAL     ResMsgH = {0};
    SHORT           sRetCode, sErrorM;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLREADTLPLUEX;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);
    CliMsg.pauchReqData = (UCHAR *)pTotal;
    CliMsg.usReqLen     = sizeof(TTLPLU);
    CliMsg.pauchResData = (UCHAR *)pTotal;
    CliMsg.usResLen     = sizeof(TTLPLUEX);

    sErrorM = SstReadFAsMaster();
    sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

SHORT   SerTtlTotalDelete(VOID *pTotal)
{
    TTLCASHIER      *pTtl = pTotal;
    CLIREQRESETTL   ReqMsgH = {0};
    CLIRESTOTAL     ResMsgH = {0};
    SERINQSTATUS    InqData;
    SHORT           sRetCode = TTL_NOT_MASTER;

    PifRequestSem(husCliExeNet);

    ReqMsgH.uchMajorClass = pTtl->uchMajorClass;
    ReqMsgH.uchMinorClass = pTtl->uchMinorClass;
    ReqMsgH.ulIDNO        = pTtl->ulCashierNumber;
    SstReadInqStat(&InqData);
    memcpy(ReqMsgH.ausTranNo,    InqData.ausTranNo,    2*CLI_ALLTRANSNO);
    memcpy(ReqMsgH.ausPreTranNo, InqData.ausPreTranNo, 2*CLI_ALLTRANSNO);
       
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode = CLI_FCTTLDELETETTL;

    CliMsg.usFunCode   |= CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);
    CliMsg.sRetCode     = TTL_SUCCESS;

    sRetCode = TTL_NOT_MASTER;
    if (STUB_SELF == SstUpdateAsMaster()) {         
        if (TTL_SUCCESS != TtlReadWrtChk()) { /* total still exist */
			CLICOMIF CliMsgSave = CliMsg;

			PifReleaseSem(husCliExeNet);
			NHPOS_NONASSERT_TEXT("==NOTE: SerTtlTotalDelete (TTL_SUCCESS != TtlReadWrtChk())");
            CstSleep();
			while(TTL_SUCCESS != TtlReadWrtChk()) { /* total still exist */
				CstSleep();
			}
			PifRequestSem(husCliExeNet);
			NHPOS_NONASSERT_TEXT("==NOTE: SerTtlTotalDelete (TTL_SUCCESS == TtlReadWrtChk()) complete.");
			CliMsg = CliMsgSave;
		}
	    /* 09/03/01 */
    	sRetCode = TtlPLUGetBackupStatus();
		NHPOS_ASSERT_TEXT((sRetCode != TTL_BK_STAT_BUSY), "==WARNING: SerTtlTotalDelete TtlPLUGetBackupStatus()");
	    while (sRetCode == TTL_BK_STAT_BUSY) {
    	    PifSleep(1000);
        	sRetCode = TtlPLUGetBackupStatus();
	    }

        sRetCode = TtlTotalDelete(pTotal);

		if (TTL_SUCCESS == sRetCode) {
			SstUpdateBackUp();
		}
    }

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT     CliRecvTtlGetLen(UCHAR uchMajorClass);
*     Input:    uchMajorClass  - Major Total Class No
*     Output:   nothing          
*     InOut:    nothing
*
** Return:      nothing
*
** Description:  Read a Total structure length.
*===========================================================================
*/
SHORT  CliRecvTtlGetLen(UCHAR uchMajorClass)
{
    SHORT       sRet;

    switch ( uchMajorClass ) {
    case CLASS_TTLREGFIN :                      /* Register financial */
    case CLASS_TTLINDFIN :                      /* individual fin R3.1 */
        sRet = sizeof(TTLREGFIN);
        break;

    case CLASS_TTLHOURLY :                      /* Hourly activity    */   
        sRet = sizeof(TTLHOURLY);
        break;
 
    case CLASS_TTLDEPT :                        /* Department */
        sRet = sizeof(TTLDEPT);
        break;

    case CLASS_TTLPLU :                         /* PLU */ 
        sRet = sizeof(TTLPLU);
        break;
    
    case CLASS_TTLCASHIER :                     /* Cashier */ 
        sRet = sizeof(TTLCASHIER);
        break;

    case CLASS_TTLCPN :                         /* Coupon R3.0 */
        sRet = sizeof(TTLCPN);
        break;

    case CLASS_TTLSERVICE:                      /* service time R3.1 */
        sRet = sizeof(TTLSERTIME);
        break;
    
    default :                                   /* Error  */
        sRet = 0;
        break;
    }

    return(sRet);
}
/*===== END OF SOURCE =====*/