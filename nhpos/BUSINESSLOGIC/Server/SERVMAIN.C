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
* Title       : SERVER module, Main Functions Source File                        
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVMAIN.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*       SerStartUp();               Server start up (main loop)
*       SerInitialize();            Server initialize
*       SerRevRequest();            Receive request
*       SerExeSpeReq();          C  Exec. special request
*       SerRevMesHand();            Response message handling
*       SerErrResHand();            Error handling
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Jun-05-93:H.Yamaguchi: Modified Enhanced function.
* Jul-10-95:M.Suzuki   : R3.0
*          :           :                                    
** GenPOS **
* Sep-08-17 : 02.02.02 : R.Chambers   : SerNetOpen() return status, SerInitialize() decides PifAbort().
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
#include	<math.h>

#include    <ecr.h>
#include    <pif.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    "servmin.h"
#include    <uie.h>
#include    <pifmain.h>
#include    <log.h>
#include    <appllog.h>

/*
*===========================================================================
** Synopsis:    VOID    SerStartUp(VOID);
*
** Return:      none.
*
** Description: Server Module Start Up Function (Main Loop).
*===========================================================================
*/
VOID THREADENTRY SerStartUp(VOID)
{
    USHORT  usForEver = 1;
    CLIREQCOM   *pResp;             /* R3.0 */

    SerInitialize();                            /* Server Initialize */
    while(usForEver) {
        SerRevRequest();                        /* Receive Request */ 
        SerExeSpeReq();                         /* Exec. Special Func. */
        if (SERV_SUCCESS == SerResp.sError) {   /* Receive Success ? */
            PifRequestSem(husSerCreFileSem);    /* Request Semapho */
            SerRevMesHand();                    /* Receive msg. handling */
            PifReleaseSem(husSerCreFileSem);    /* Release Semapho */
            pResp = (CLIREQCOM *)SerRcvBuf.auchData; /* Add R3.0 */
            if ((pResp->usFunCode & CLI_RSTCONTCODE) == CLI_FCSPSPOLLING) {
                SerResp.sError = SERV_TIME_OUT; /* Dummy */
                SerErrResHand();                /* Error Handling */
            }
        } else {                                /* Receive Error ? */
            SerErrResHand();                    /* Error Handling */
        }
    }
    PifEndThread();
}

/*
*===========================================================================
** Synopsis:    VOID    SerInitialize(VOID);
*
** Return:      none.
*
** Description: Server Module Initialize function.
*===========================================================================
*/
VOID    SerInitialize(VOID)
{
	SHORT  sStatus;

    sStatus = SerNetOpen();                     /* Open Net */
	if (sStatus < 0) {
        PifLog(MODULE_SER_LOG, LOG_ERROR_SER_BAD_ENVIROMENT);
        PifLog(MODULE_ERROR_NO(MODULE_SER_LOG), (USHORT)abs(sStatus));
        PifAbort(MODULE_SER_ABORT, FAULT_BAD_ENVIRONMENT);
	}

    if (SER_IAM_MASTER) {                       /* MASTER */
        if (SerNetConfig.fchStatus & SER_NET_BACKUP) {
            SerChangeStatus(SER_STINQUIRY);     /* inquiry state */
            SerSendInquiry();                   /* send inq. request */
        } else {                                /* non back up system ? */
            SerChangeStatus(SER_STNORMAL);      /* normal state */
            SstChangeInqStat(SER_INQ_M_UPDATE); /* decide new status */
            SerSendNtbStart();                  /* start Notice Board */
        }
    }
    if (SER_IAM_BMASTER) {                      /* BACK UP MASTER */
        SerChangeStatus(SER_STINQUIRY);         /* inquiry state */
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerRevRequest(VOID);
*
** Return:      none.
*
** Description: This function request to receive request from CLIENT.
*===========================================================================
*/
VOID    SerRevRequest(VOID)
{
    SerNetReceive();                            /*=== NET RECEIVE ===*/
}

/*
*===========================================================================
** Synopsis:    VOID    SerExeSpeReq(VOID);
*
** Return:      none.
*
** Description: This function checks executive special request.
*===========================================================================
*/
VOID    SerExeSpeReq(VOID)
{
    SerESRTimeOutBackUp();              /* Timer Expire ?, for back up */
    SerESRTimeOutMulti();               /* Timer Expire ?, for multi. */
    SerESRTimeOutKps();                 /* Timer Expire ?, for Kps. */
    SerESRTimeOutSps();                 /* Timer Expire ?, for Sps. */
    SerESRTimeOutBcas();                /* Timer Expire ?, for Bcas */

    SerESRChgBMStat();                  /* BM Out Of Date, from STUB ? */
    SerESRChgMTStat();                  /* MT Out Of Date, from STUB ? */
    SerESRBackUpFinal();                /* Finalize of A/C 42 */
    SerESRNonBKSys();                   /* A/C 85 request ? */
    SerESRBackUp();                     /* A/C 42 request ? */

    SerESRBMDown();                     /* Back Up Master off-line ? */
}

/*
*===========================================================================
** Synopsis:    VOID    SerRevMesHand(VOID);
*
** Return:      none.
*
** Description: This function executes receive message handling.
*===========================================================================
*/
VOID    SerRevMesHand(VOID)
{
    if (SERV_SUCCESS != SerRMHCheckData()) {
        return;                         /* illegal data or data loss */
    }

    switch(usSerStatus) {
    case    SER_STINQUIRY:              /* Inquiry State */    
        SerRMHInquiry();
        break;

    case    SER_STNORMAL:               /* Normal State */
        SerRMHNormal();
        break;

    case    SER_STMULSND:               /* Multi sending State */
        SerRMHMulSnd();
        break;

    case    SER_STMULRCV:               /* Multi Receiving State */
        SerRMHMulRcv();
        break;

    case    SER_STBACKUP:               /* Back Up State */
        SerRMHBackUp();
        break;

    default:                            /* not used */
        break;
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerErrResHand(VOID);
*
** Return:      none.
*
** Description: This function executes error response handling.
*===========================================================================
*/
VOID    SerErrResHand()
{
    switch(SerResp.sError) {
    case    SERV_TIME_OUT:                  /* Time Out Error */
        SerERHTimeOut();                   
        break;

    case    SERV_POWER_DOWN:                /* Power Fail */
        SerERHPowerFail();
        break;

    default:                                /* other Error */
        SerERHOther();
        break;
    }
}

/*===== END OF SOURCE =====*/
