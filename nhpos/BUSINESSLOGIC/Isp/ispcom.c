/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ISP Server Common Functions Source File
* Category    : ISP Server module, US Hospitality Model
* Program Name: ISPCOM.C                                            
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
*       2020  -> OpenGenPOS Rel 2.4.0 (Windows 10, Datacap removed) Open source
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2019 with Rel 2.4.0
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:
*
*           IspIamMaster()          Check am I Master Terminal ?
*           IspIamBMaster()         Check am I Backup Master Terminal ?
*           IspIamSatellite()    A  Check am I Satellite Terminal ?
*           
*           IspSendResponse()       Send Response
*           IspSendError()          Send Error Response
*
*           IspChangeStatus()       Change server status
*           IspComReadStatus()      Read Communication Status
*
*           IspNetOpen()            Open Net work
*           IspNetClose()           Close Net work
*           IspNetSend()            Send to Net work
*           IspNetReceive()         Receive from Net work
*           
*           IspTimerStart()         Start Timer
*           IspTimerStop()          Stop Timer
*           IspTimerRead()          Read Timer
*           
*           IspAmISatellite()    A  Get current terminal condition  
* --------------------------------------------------------------------------
* Update Histories                                                         
* Date     :Ver.Rev.:   Name    : Description
* Jun-22-92:00.00.01:H.Yamaguchi: initial                                   
* Jun-05-93:00.00.01:H.Yamaguchi: Modified "IspNetSend" & Enhance.
*          :        :           :                                    
*
** NCR2171 **
* Aug-26-99:01.00.00:M.Teraki   :initial (for 2171)
* Dec-24-99:01.00.00:hrkato     :Saratoga
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
#include    <memory.h>
#include    <stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <rfl.h>
#include    <log.h>
#include    <uie.h>
#include    <fsc.h>
#include    <nb.h>
#include    <paraequ.h>
#include	<pifmain.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <storage.h>
#include    <isp.h>
#include    "ispcom.h"
#include    <csstbstb.h>
#include    <appllog.h>

VOID    IspLogNetSend (char *aszPrintLineFirst)
{
	CLIREQCOM   *pSend = (CLIREQCOM *)IspSndBuf.auchData;
	int         iLength = 0;
	PIFUSRTBLINFO  usrStats;
	char        aszPrintLine[255];

	PifNetControl(IspNetConfig.usHandle, PIF_NET_GET_USER_INFO, &usrStats);

	iLength += sprintf (aszPrintLine, " %-30.75s usrStats.usLastSenduchNo %d CLIREQCOM:usFunCode 0x%4.4x usSeqNo 0x%4.4x ",
		aszPrintLineFirst, usrStats.usLastSenduchNo, pSend->usFunCode, pSend->usSeqNo);

	CstLogLine (&aszPrintLine[0]);

	{
		int i, j;
		j = 0;
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		for (i = 0; i < 64; i++) {
			UCHAR tempChar = ((IspSndBuf.auchData[i] >> 4) & 0x0f);
			if ((i % 4) == 0) aszPrintLine[j++] = ' ';
			aszPrintLine[j++] = (tempChar < 10) ? (tempChar + '0') : ((tempChar - 10) + 'a');
			tempChar = (IspSndBuf.auchData[i] & 0x0f);
			aszPrintLine[j++] = (tempChar < 10) ? (tempChar + '0') : ((tempChar - 10) + 'a');
		}
		aszPrintLine[j++] = 0;
	}
	CstLogLine (&aszPrintLine[0]);
}

VOID    IspLogNetRecv (char *aszPrintLineFirst)
{
	CLIRESCOM   *pResp = (CLIRESCOM *)IspRcvBuf.auchData;
	int         iLength = 0;
	PIFUSRTBLINFO  usrStats;
	char        aszPrintLine[255];

	PifNetControl(IspNetConfig.usHandle, PIF_NET_GET_USER_INFO, &usrStats);

	iLength += sprintf (aszPrintLine, " %-30.75s usrStats.usLastRecvuchNo %d CLIRESCOM: usFunCode 0x%4.4x usSeqNo 0x%4.4x sResCode %4.4d sReturn %4.4d",
		aszPrintLineFirst, usrStats.usLastRecvuchNo, pResp->usFunCode, pResp->usSeqNo, pResp->sResCode, pResp->sReturn);

	CstLogLine (&aszPrintLine[0]);

	{
		int i, j;
		j = 0;
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		aszPrintLine[j++] = ' ';
		for (i = 0; i < 64; i++) {
			UCHAR tempChar = ((IspRcvBuf.auchData[i] >> 4) & 0x0f);
			if ((i % 4) == 0) aszPrintLine[j++] = ' ';
			aszPrintLine[j++] = (tempChar < 10) ? (tempChar + '0') : ((tempChar - 10) + 'a');
			tempChar = (IspRcvBuf.auchData[i] & 0x0f);
			aszPrintLine[j++] = (tempChar < 10) ? (tempChar + '0') : ((tempChar - 10) + 'a');
		}
		aszPrintLine[j++] = 0;
	}
	CstLogLine (&aszPrintLine[0]);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspIamMaster(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:       I am     Master Terminal
*               ISP_ERR_ILLEGAL:   I am not Master Terminal
*
** Description: This function checks that I am Master or not.
*===========================================================================
*/
SHORT   IspIamMaster(VOID)
{
    if (IspNetConfig.auchFaddr[CLI_POS_UA] == PIFNET_TGT_MASTER) {
        return (ISP_SUCCESS);           /* if address = "1", then Master */
    } 
    return (ISP_ERR_ILLEGAL);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspIamBMaster(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:       I am Back Up Master Terminal
*               ISP_ERR_ILLEGAL:   I am not Back Up Mater Terminal
*
** Description: Thsi function checks that am I a Backup Mater or not.
*===========================================================================
*/
SHORT   IspIamBMaster(VOID)
{
    if ((IspNetConfig.auchFaddr[CLI_POS_UA] == PIFNET_TGT_BMASTER) && (IspNetConfig.fchStatus & PIFNET_NET_BACKUP)) {
        return (ISP_SUCCESS);        /* if address "2" with backup system , then B-Master */
    }
    return (ISP_ERR_ILLEGAL);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspIamSatellite(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:       I am     Master Terminal
*               ISP_ERR_ILLEGAL:   I am not Master Terminal
*
** Description: This function checks that I am Master or not.
*===========================================================================
*/
SHORT   IspIamSatellite(VOID)
{
    if (IspNetConfig.auchFaddr[CLI_POS_UA] == PIFNET_TGT_MASTER) {
        return (ISP_ERR_ILLEGAL);
    }
    if ((IspNetConfig.auchFaddr[CLI_POS_UA] == PIFNET_TGT_BMASTER) && (IspNetConfig.fchStatus & PIFNET_NET_BACKUP)) {
        return (ISP_ERR_ILLEGAL);
    }
    return (ISP_SUCCESS);       
}

/*
*===========================================================================
** Synopsis:    VOID    IspSendResponse(CLIRESCOM *pResMsgH,
*                                       USHORT usResMsgHLen,
*                                       UCHAR *puchReqData,
*                                       USHORT usReqLen);
*
*     InPut:    pResMsgH     - Response message header (now responded)
*               usResMsgHLen - Response message header length
*               puchReqData  - Send data pointer
*               usReqLen     - Send data length
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.      
*
** Description: This function sends response.
*===========================================================================
*/
VOID    IspSendResponse(CLIRESCOM   *pResMsgH,
                        ULONG       ulResMsgHLen,
                        UCHAR       *puchReqData,
                        ULONG       ulReqLen)
{
    ULONG  ulSendLen;

    // fill in the XGRAM header indicating receiver of the message we are building.
    IspSndBuf.xgHeader = IspRcvBuf.xgHeader;
    IspSndBuf.usLport = PIFNET_PORT_ISPSERVER;          // ensure local port is set correctly
    ulSendLen = sizeof(XGHEADER);
                                                /*=== EDIT MESSAGE ===*/
    memcpy(IspSndBuf.auchData, pResMsgH, ulResMsgHLen);
    ulSendLen += ulResMsgHLen;

    if (NULL != puchReqData ) {                 /*=== EDIT DATA ===*/
        CLIREQDATA  * const pSend = (CLIREQDATA*)(IspSndBuf.auchData + ulResMsgHLen);

        pSend->usDataLen = ulReqLen;                           /* insert the number of bytes to send */
        memcpy(pSend->auchData, puchReqData, ulReqLen);        /* Copy data to send */
        ulSendLen += ulReqLen + sizeof(pSend->usDataLen);      /* Add data length size */
    }
                                               /*=== SEND RESPONSE ===*/
    IspNetSend(ulSendLen);
}


/*
*===========================================================================
** Synopsis:    VOID    IspSendError(SHORT sError);
*     InPut:    sError  - Error Code
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.      
*
** Description: This function sends error response.
*===========================================================================
*/
VOID    IspSendError(SHORT sError)
{
    CLIREQCOM   *pReqMsgH = (CLIREQCOM *)IspRcvBuf.auchData;
    CLIRESCOM   *pResMsgH = (CLIRESCOM *)IspSndBuf.auchData;
     
    // fill in the XGRAM header indicating receiver of the message we are building.
    IspSndBuf.xgHeader = IspRcvBuf.xgHeader;
    IspSndBuf.usLport = PIFNET_PORT_ISPSERVER;          // ensure local port is set correctly
                                                 /*=== EDIT MESSAGE ===*/

    pResMsgH->usFunCode = pReqMsgH->usFunCode;   /* Set function code */
    pResMsgH->sResCode  = sError;                /* Set error code */
    pResMsgH->usSeqNo   = pReqMsgH->usSeqNo;     /* Set sequence number */
    pResMsgH->sReturn   = 0;                     /* Set success */

                                                 /*=== SEND RESPONSE ===*/
    IspNetSend(sizeof(XGHEADER) + sizeof(CLIRESCOM));
}

/*
*===========================================================================
** Synopsis:    VOID    IspChangeStatus(USHORT usState);
*     InPut:    usStatus    - New Status
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function changes server status.
*===========================================================================
*/
VOID    IspChangeStatus(USHORT usState)
{
    switch (usState) {
    case    ISP_ST_NORMAL   :               /* into NORMAL state */
        IspResp.usTimeOutCo = 0;            /* clear time out counter */
        usIspState = usState;
        break;

    case    ISP_ST_MULTI_SEND :             /* into MULTI SEND state */
    case    ISP_ST_MULTI_RECV :             /* into MULTI RECV state */
        usIspState = usState;
        break;

    case    ISP_ST_PASSWORD :               /* into PASSWORD state */
		if (usIspState != ISP_ST_PASSWORD) {
			NHPOS_NONASSERT_TEXT("==NOTE: IspChangeStatus change to ISP_ST_PASSWORD");
		}
        CliDispDescriptor(CLI_MODULE_ISP, CLI_DESC_STOP_BLINK) ; /* Stop blink " COMM " */
        usIspState = usState;
        IspTimerStart() ;                   /* Start timer */
        break;

    default:                                /* not used */
		PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_STATUS_INVALID_STATE);
		PifLog(MODULE_DATA_VALUE(MODULE_ISP_LOG), usState);
		PifLog(MODULE_LINE_NO(MODULE_ISP_LOG), (USHORT)__LINE__);
        break;
    }
}


/*
*===========================================================================
** Synopsis:    USHORT  IspComReadStatus(VOID);
*     InPut:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Communication Status.
*
** Description: This function reads comminication (condition) status.
*===========================================================================
*/
USHORT  IspComReadStatus(VOID)
{
    USHORT      fsComStatus, fsServerFlag;
    NBMESSAGE   NbInf;

    fsComStatus = 0;

    NbReadAllMessage(&NbInf);           /* Read Up to date and online flag */

    if (NbInf.fsSystemInf & NB_MTUPTODATE) {
        fsComStatus |= ISP_COMSTS_M_UPDATE;          /* Set M up to date */
    }
    if (NbInf.fsSystemInf & NB_BMUPTODATE) {
        fsComStatus |= ISP_COMSTS_BM_UPDATE;         /* Set BM up to date */
    }
    if (0 == (NbInf.fsSystemInf & NB_SETMTONLINE)) {
        fsComStatus |= ISP_COMSTS_M_OFFLINE;         /* Set M offline flag */
    }
    if (0 == (NbInf.fsSystemInf & NB_SETBMONLINE)) {
        fsComStatus |= ISP_COMSTS_BM_OFFLINE;        /* Set BM offline flag */
    }

    if ( (NbInf.fsMBackupInf & NB_STOPALLFUN) ||
         (NbInf.fsBMBackupInf & NB_STOPALLFUN) ) {
        fsComStatus |= ISP_COMSTS_STOP;             /* Set STOP func now */
    }

    SstReadFlag(&fsServerFlag);      /* Get server execution flag */

    if (fsServerFlag & SER_SPESTS_INQUIRY) {
        fsComStatus |= ISP_COMSTS_INQUIRY;          /* Set during INQUIRY state */
    }
    if (fsServerFlag & SER_SPESTS_INQTIMEOUT) {
        fsComStatus |= ISP_COMSTS_INQTIMEOUT;       /* Now INQUIRY syate */
    }
    if (fsServerFlag & SER_SPESTS_AC85) {
        fsComStatus |= ISP_COMSTS_AC85;             /* Now will go to out of service */
    }
    if (fsServerFlag & SER_SPESTS_AC42) {
        fsComStatus |= ISP_COMSTS_AC42;             /* Now during backup copy */
    }
    return (fsComStatus);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspNetOpen(VOID);
*     InPut:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:   Successful openning
*
** Description: This function opens NET.
*===========================================================================
*/
SHORT    IspNetOpen(VOID)
{
    SYSCONFIG CONST *SysCon = PifSysConfig();

    memset(&IspNetConfig, 0, sizeof(IspNetConfig));

    IspNetConfig.xgHeader.auchAddr = *(AUCHADDR *)SysCon->auchLaddr;
    IspNetConfig.xgHeader.usLport = PIFNET_PORT_ISPSERVER;
    IspNetConfig.xgHeader.usFport = PIFNET_PORT_ANYPORT ;

    if (1 == usBMOption) {    /* saratoga */
        IspNetConfig.fchStatus |= PIFNET_NET_BACKUP;    /* Set with backup system */
    }

    IspNetConfig.usHandle = PifNetOpen(&IspNetConfig.xgHeader);   /* Open network */
    if (0 <= IspNetConfig.usHandle) {
        IspNetConfig.fchStatus |= PIFNET_NET_OPEN;      /* Set open flag */

		/* Set normal & timeout mode */
        PifNetControl(IspNetConfig.usHandle, PIF_NET_SET_MODE, PIF_NET_NMODE | PIF_NET_TMODE);

        CliDispDescriptor(CLI_MODULE_ISP, CLI_DESC_STOP_ON ) ; /* Display off " COMM " */
        
        return (ISP_SUCCESS);
    }
    else {
        CliDispDescriptor(CLI_MODULE_ISP, CLI_DESC_START_ON ) ; /* Display  " COMM " */

        char  xBuff[128];
        sprintf(xBuff, "**ERROR: IspNetOpen() PifNetOpen failed error %d", IspNetConfig.usHandle);
        NHPOS_ASSERT_TEXT((0 <= IspNetConfig.usHandle), xBuff);

        PifLog(MODULE_ISP_ABORT, FAULT_BAD_ENVIRONMENT);
        PifLog(MODULE_ERROR_NO(MODULE_ISP_ABORT), (USHORT)abs(IspNetConfig.usHandle));

        return (ISP_ERR_ILLEGAL);
    }
}

/*
*===========================================================================
** Synopsis:    VOID    IspNetClose(VOID);
*     InPut:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function closes NET.
*===========================================================================
*/
VOID    IspNetClose(VOID)
{
    PifNetClose(IspNetConfig.usHandle);         /* Close for power down recovry */
    IspNetConfig.fchStatus &= ~PIFNET_NET_OPEN;    /* Reset NET_OPEN flag */
    IspNetConfig.usHandle = PIF_NET_INVALID_HANDLE;
}

VOID	IspNetSynchSeqNo (VOID)
{
    PifNetControl(IspNetConfig.usHandle, PIF_NET_RESET_SEQNO);
}

/*
*===========================================================================
** Synopsis:    VOID    IspNetSend(USHORT usSendData);
*     Input:    usSendData  - Data length to send now
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function sends thru my port.
*===========================================================================
*/
SHORT   IspNetSend(ULONG ulSendData)
{
    SHORT   sError = PIF_ERROR_NET_OFFLINE, sRetry=5;

    if (IspNetConfig.fchStatus & PIFNET_NET_OPEN) {    /* if NET_OPEN, the send data */

        /* IspNetConfig.fchStatus |= ISP_NET_SEND;   */
		// test requiring an ACK/NAK
		if (IspNetConfig.fchStatus & PIFNET_NET_USING_ACKNAK) {    /* if using Ack/Nak then use PIF_NET_SET_STIME */
			PifNetControl(IspNetConfig.usHandle, PIF_NET_SET_STIME, 5000);
		} else {
			PifNetControl(IspNetConfig.usHandle, PIF_NET_SET_STIME, 0);
		}
        do {
			CHAR   xBuff[256];

            if (5 > sRetry) {
                usIspTest ++;               /* ISP Test counter        */
                PifSleep(100);
            }

            sError = PifNetSend(IspNetConfig.usHandle, (VOID *)&IspSndBuf, ulSendData);
			sRetry--;
			sprintf (xBuff, "  IspNetSend: sError %d sRetry %d ", sError, sRetry);
			IspLogNetSend (xBuff);
        } while ( ((PIF_ERROR_NET_WRITE == sError)   && (sRetry > 0)) ||
                  ((PIF_ERROR_NET_OFFLINE == sError) && (sRetry > 0)) ||
				  ((PIF_ERROR_NET_BUSY == sError)    && (sRetry > 0)) ||
				  ((PIF_ERROR_NET_TIMEOUT == sError) && (sRetry > 0)) ||
                  ((PIF_ERROR_NET_UNREACHABLE == sError) && (sRetry > 0)) );

        if ( sError < 0 ) {
            sIspNetError = sError;       
        }

        /* IspNetConfig.fchStatus &= ~ISP_NET_SEND;       */
    }

    return sError;
}

/*
*===========================================================================
** Synopsis:    VOID    IspNetReceive(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function receive thru. my port.
*===========================================================================
*/
VOID    IspNetReceive(VOID)
{
    SHORT       sError, sPrevError;
    USHORT      usPrevErrorCo;

    if (0 == (IspNetConfig.fchStatus & PIFNET_NET_OPEN)) {
        IspResp.sError = ISP_ERR_TIME_OUT;               /* if not open, then return */
        return;
    }

	IspNetConfig.fchStatus |= PIFNET_NET_RECEIVE;
	// There are a number of functions within the Isp subsystem that use the
	// time out as way of counting off time such as function IspERHTimeOut().
	// Do not change this time out lightly!
    PifNetControl(IspNetConfig.usHandle, PIF_NET_SET_TIME, ISP_NET_RECEIVE_TIME);

    sPrevError    = 0;
    usPrevErrorCo = 0;

    do {
		CHAR   xBuff[256];

        sError = PifNetReceive(IspNetConfig.usHandle, &IspRcvBuf, sizeof(XGRAM));

        if (0 > sError) {                             /* Check receive condition */
            if (PIF_ERROR_NET_POWER_FAILURE == sError) {
                IspResp.sError = ISP_ERR_POWER_DOWN;  /* Set POWER_DOWN flag */
            } else if ((PIF_ERROR_NET_TIMEOUT == sError) || (PIF_ERROR_NET_DISCOVERY == sError || PIF_ERROR_NET_CLEAR == sError)) {
#if 1
                IspResp.sError = ISP_ERR_TIME_OUT;    /* Set TIME_OUT flag */
#else
                usPrevErrorCo ++;
                if (usPrevErrorCo > 3) {  /* Check 3 times error detection */
                    IspResp.sError = ISP_ERR_TIME_OUT;    /* Set TIME_OUT flag */
                } else {
                    IspResp.sError = ISP_ERR_ILLEGAL;     /* Set ILLEGAL flag */
                }
#endif
            } else  {
                usPrevErrorCo ++;
                if (usPrevErrorCo > 3) {  /* check 3 times error detection */
                    PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_NET_RECEIVE_RETRY); /* Write Log */
                    PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sError)); /* Write Log */
                    IspResp.sError = ISP_ERR_NETWORK;      /* Set NETWORK flag */
                } else {
                    IspResp.sError = ISP_ERR_ILLEGAL;      /* Set ILLEGAL flag */
                }
            }
        } else {
            IspResp.usRcvdLen = (USHORT)sError;  /* Save received length */
            IspResp.sError    = ISP_SUCCESS;
        }
		if (sError != PIF_ERROR_NET_DISCOVERY) {
			sprintf (xBuff, "  IspNetReceive: sError %d sError %d ", sError, IspResp.sError);
			IspLogNetRecv (xBuff);
		}
    } while (ISP_ERR_ILLEGAL == IspResp.sError);  /* if ILLEGAL, then retry 3 times */

    IspNetConfig.fchStatus &= ~PIFNET_NET_RECEIVE;
}


/*
*===========================================================================
** Synopsis:    VOID    IspTimerStart(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function starts Timer.
*===========================================================================
*/
VOID    IspTimerStart(VOID)
{
    PifGetDateTimeTimer(&IspTimer);      /* Get current TOD */
 
    IspResp.usTimeOutCo = 0 ;                /* Clear counter */
    sIspExeError = 0;                        /* Clear flag */ 
}

/*
*===========================================================================
** Synopsis:    VOID    IspTimerStop(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:  This function stops Timer.
*===========================================================================
*/
VOID    IspTimerStop(VOID)
{
    /* Current is Blank Routine */
}

/*
*===========================================================================
** Synopsis:    LONG    IspTimerRead(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Elapsed Timer (?? seconds ).
*
** Description: This function reads and calculate time.
*===========================================================================
*/
LONG    IspTimerRead(VOID)
{
    PIFTIMER    CurTime = { 0 };
    LONG        lOLD, lNEW;

    PifGetDateTimeTimer(&CurTime);        /* Get current TOD */

    lOLD = (IspTimer.uchHour * 3600L)         /* Convert start time to seconds */
           + (IspTimer.uchMin * 60L)            
           + IspTimer.uchSec;
    lNEW = (CurTime.uchHour * 3600L)          /* Convert current time to seconds */
           + (CurTime.uchMin * 60L) 
           + CurTime.uchSec;
    
    if (lOLD > lNEW) {                 /* Check if OLD > NEW, then goes to new day */
        lNEW += 24 * 3600L;            /* then adds 24*3600(1 day seconds) */
    }

    return (lNEW - lOLD);              /* Return to elapsed time */
}

/*
*===========================================================================
** Synopsis:    SHORT    IspAmISatellite(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:       I work as Satellite 
*               ISP_ERR_ILLEGAL:   I work Master Terminal
*
** Description: This function checks that I am Master or not.
*===========================================================================
*/
SHORT   IspAmISatellite(VOID)
{
    USHORT      fsComStatus;
    
    fsComStatus = IspComReadStatus();   /* Read current status */

    if (ISP_IAM_MASTER) {   /* if I am Master, then checks next */
        if (0 == (fsComStatus & ISP_COMSTS_M_UPDATE)) {
            return (ISP_SUCCESS);
        }
        return (ISP_ERR_ILLEGAL);         /* I work as Master  */
    }

    if (ISP_IAM_BMASTER) {  /* if I am B-Master, then checks next */
        if (0 == (fsComStatus & ISP_COMSTS_BM_UPDATE)) {
            return (ISP_SUCCESS);
        }
        if ((fsComStatus & ISP_COMSTS_M_UPDATE)) {
            return (ISP_SUCCESS);
        }
        return (ISP_ERR_ILLEGAL);         /* I work as Master  */
    }

    return (ISP_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID     IspResetLog(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:     Print Reset Log on Journal. V3.3
*===========================================================================
*/
VOID    IspWriteResetLog(VOID)
{
    FSCTBL      Ring;
    DATE_TIME   WorkDate;
    UCHAR       uchDummy = 0;

    if (IspResetLog.uchReset == ISP_RESET_READ) {
        /* not write log by current read only case */
        return;
    }

	/* print out pcif reset log on EJ */
    if (IspResetLog.uchAction) {
	    uchResetLog = IspResetLog.uchAction;
	} else {
	    uchResetLog = UIF_UCHRESETLOG_NOACTION;
	}
	
    PifGetDateTime(&WorkDate);
    IspResetLog.uchYear = (UCHAR)(WorkDate.usYear%100);
    IspResetLog.uchMonth= (UCHAR)WorkDate.usMonth;
    IspResetLog.uchDay  = (UCHAR)WorkDate.usMDay;
    IspResetLog.uchHour = (UCHAR)WorkDate.usHour;
    IspResetLog.uchMin  = (UCHAR)WorkDate.usMin;

    memset(&IspResetLog.aszMnemonic, 0, sizeof(IspResetLog.aszMnemonic));

    Ring.uchMajor = FSC_RESET_LOG;
    Ring.uchMinor = 0xFF;
    UieWriteRingBuf(Ring, &uchDummy, sizeof(uchDummy)); /* Saratoga */
}

VOID   IspWriteResetLogAction (UCHAR uchAction, UCHAR uchReset, WCHAR *aszMnemonic)
{
	memset (&IspResetLog, 0, sizeof(IspResetLog));
	IspResetLog.uchAction = uchAction;
	IspResetLog.uchReset = uchReset;
	_tcsncpy (IspResetLog.aszMnemonic, aszMnemonic, TCHARSIZEOF(IspResetLog.aszMnemonic) - 1);
	IspWriteResetLog();
}

/*
*============================================================================
**Synopsis:     BOOL    IspRecvTtlCheckPluNo(UCHAR *puchPluNo)
*
*    Input:     UCHAR   *pPluNo
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing
*
** Description  This function checks PLU No.                    Saratoga
*============================================================================
*/
BOOL    IspRecvTtlCheckPluNo(TCHAR *puchPluNo)
{
    USHORT  i;

    for (i = 0; i < PLU_MAX_DIGIT; i++) {
        if (puchPluNo[i] != 0) {
            return(TRUE);
        }
    }

    return(FALSE);
}

/*===== END OF SOURCE =====*/