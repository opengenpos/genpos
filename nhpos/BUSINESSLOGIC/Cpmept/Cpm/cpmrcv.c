/*

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation

*=============================================================================
* Title       : CHARGE POSTING MANAGER Receive Task
* Category    : Charge Posting Manager, NCR 2170 RESTRANT MODEL
* Program Name: CPMRCV.C
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*               CpmMainReceiveTask()  
*                   CpmOpenSetPort();
*                   CpmCreateMessage();
*                       CpmSetReadTimer();
*                       CpmReceiveCom();
*                           CpmRecoverPort();
*                           Cpm1ByteTimeOut();
*                           Cpm1BytePowerOff();
*                       CpmCheckMessageType();
*                   CpmExeResponse();
*                   CpmExeUnsoli();
*               
*-----------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name       : Description
* Nov-01-93 : 00.00.01 : Yoshiko.Jim  : Initial                                   
* Apr-15-94 : 00.00.05 : Yoshiko.Jim  : E-022 corr. (mod. CpmCheckMessageType())
*=============================================================================
*=============================================================================
* PVCS Entry
* ----------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*=============================================================================
*/

/**
;============================================================================
;                  I N C L U D E     F I L E s                              +
;============================================================================
**/
/**------- MS - C ------**/
#include <tchar.h>

#include <memory.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <cpm.h>
#include "cpmlocal.h"

/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/
CPMSPOOL    CpmSpool;                               /* read data save area  */
//USHORT  usCpmWaitTime;                              /* wait timer for Read  */
/**
;=============================================================================
;+              P R O G R A M    D E C L A R A T I O N s                     +
;=============================================================================
**/
/**
;=============================================================================
;+              E X T E R N A L  R A M s
;=============================================================================
**/

/*
*=============================================================================
*
** Format  : VOID THREADENTRY   CpmMainReceiveTask(VOID);
*               
*    Input :    nothing
*
*   Output :    nothing
*    InOut :    nothing 
*
** Return  :    nothing
*
** Synopsis:    This function is the main loop for the Charge Post message
*				receiving thread.  This loop runs as a background process to accept
*				messages from the Charge Post Host and providing those messages to
*				the NeighborhoodPOS charge post system.
*
*				There are two main kinds of messages from the Charge Post Host:
*					CPM_TYPE_USER_RESP - response to a user request
*						Normally used to respond to a user charge request, accept or decline.
*					CPM_TYPE_UNSOLI - an unsolicited or autonomous message from the Host
*						Normally used to indicate status changes for the Charge Post Host.
*
*=============================================================================
*/
VOID THREADENTRY    CpmMainReceiveTask(VOID)
{
    SHORT       sMsgType;
    UCHAR       uchSaveState;

    for (;;) {

		// obtain access to the Charge Post Manager port handle
		// and check to see our port state.  This is for the RS-232 port.
        PifRequestSem(usCpmSemHandle);              /* for hsCpmSemHandle   */
        if (hsCpmPortHandle == CPM_PORT_NONE) {     /* not open comm. port? */
            PifReleaseSem(usCpmSemHandle);          /* for hsCpmSemHandle   */
            CpmRecoverPort();                       /* port recover process */
            PifRequestSem(usCpmSemHandle);          /* for hsCpmSemHandle   */
        }
        PifReleaseSem(usCpmSemHandle);              /* for hsCpmSemHandle   */

        // Now check to see if we have received a message from the Charge
		// Post Host.  Determine the type of message and handle the message
		// appropriately, either response to user request or an unsolicited
		// status message.
		sMsgType = CpmCreateMessage();
        uchSaveState = uchCpmStatus;                /* save current status  */
        switch (sMsgType) {
        case CPM_TYPE_USER_RESP:
            if (uchCpmStatus != CPM_STATE_SEND_OK) {
                Para_CpmTally.usPMSThrowResp++;     /* count up tally       */
                break;                              /* if during this status*/
            }
            if (CpmCheckSpoolData() != CPM_SAME) {  /* check send add */
                Para_CpmTally.usPMSThrowResp++;     /* count up tally */
                break;                              /* if during this status */
            }                                       /* message is thrown    */
            Para_CpmTally.usPMSRecResp++;           /* count up tally       */    
            CpmExeResponse();                       /* send response to user &
                                                    if err tally count up */
            break;

        case CPM_TYPE_UNSOLI:
            Para_CpmTally.usPMSRecUnsoli++;         /* count up tally       */    
            CpmExeUnsoli(uchSaveState);
            break;

        default:
            if (uchCpmStatus != CPM_STATE_SEND_OK) {
                break;                              /* if during this status*/  
            }                                       /* message is thrown    */
            if (sMsgType == CPM_RET_TIMEOUT) {
                Para_CpmTally.usTimeout++;          /* count up tally       */    
            }
            CpmExeError(sMsgType);                  /* send error to user & if
                                                    err tally count up   */ 
            break;
        }
    }
}

/*
*=============================================================================
** Format  : SHORT  CpmOpenSetPort(USHORT usWaitTimer)
*               
*    Input : ulWaitTimer -Wait timer value (NN sec.)              
*   Output : none
*    InOut : none
*
** Return  : CPM_RET_SUCCESS    - open port success
*            sRet               - return open failure
*
** Synopsis: This function is open port and set status.
*
*=============================================================================
*/
SHORT   CpmOpenSetPort(USHORT usWaitTimer)
{
    SHORT   i, sRet;
    
    if (usWaitTimer > CPM_MAX_SET_TIMER) {
        usWaitTimer = CPM_MAX_SET_TIMER * 1000;     /* set max msec.        */
    } else {
        usWaitTimer *= 1000;                        /* set msec.            */
    }

	// RJC added following if for LAN support of Charge Post
	if (chCpmPortID == CPM_PORT_USENET) {
		hsCpmPortHandle = CPM_PORT_USENET;  /* indicate we are using LAN   */
		return PIF_OK;
	}

    for (i = 0; i < CPM_WAIT_PORT; i++) {
        sRet = hsCpmPortHandle;
        if (hsCpmPortHandle < 0) {                  /* not open comm. port? */
            sRet = PifOpenCom(chCpmPortID, &CpmProtocol);
                                                    /* executes open port   */
        }
        if (sRet >= 0) {                            /* open port success ?  */

            hsCpmPortHandle = sRet;                 /* set handle value     */
            sRet = PifControlCom(hsCpmPortHandle,   /* set receive time     */
                                 PIF_COM_SET_TIME,
                                 (USHORT FAR *)&usWaitTimer);
                                                            
            if (sRet == PIF_ERROR_COM_POWER_FAILURE) {
                                                    /* power down error ?   */
                PifCloseCom(hsCpmPortHandle);       /* port close           */
                hsCpmPortHandle = CPM_PORT_NONE;    /* reset handle value   */

                if (hsCpmNetHandle != CPM_PORT_NONE) {  /* not stand alone? */
                    CpmNetClose();                  /* close old IHC port   */
                    CpmNetOpen();                   /* open new IHC port    */
                }
                continue;                           /* retry port open      */

            } else if (sRet > 0) {
                return(CPM_RET_SUCCESS);            /* return open success  */
            }
        }
        PifSleep(CPM_WAIT_PORT*1000);
    }
    hsCpmPortHandle = CPM_PORT_NONE;                /* reset handle value   */
    return(sRet);                                   /* return open failure  */

}

/*
*=============================================================================
*
** Format  :    SHORT   CpmCreateMessage(VOID);
*               
*    Input :    nothing
*
*   Output :    usMsgType;
*    InOut :    nothing 
*
** Return  :    CPM_TYPE_USER_RESP  - response message for user.
*               CPM_TYPE_UNSOLI     - unsoli. message from PMS.
*               < 0                 - error message for user.
*
** Synopsis:    This function create message by RS-232 port.
*
*=============================================================================
*/
SHORT   CpmCreateMessage(VOID)
{
    UCHAR   i = 1;                                  /* for other task       */
    SHORT   sCurrent = CPM_NOISE;

    for (;;) {
        CpmSetReadTimer(sCurrent);                  /* set for PifReadCom() */
                                                    
        sCurrent = CpmReceiveCom(sCurrent);         

        if (sCurrent < 0) {
            return(sCurrent);                       /* return err message   */
        }
        switch (sCurrent ) {
            case CPM_NOISE:                         /* unnecessary data ?   */
                if (uchCpmStatus == CPM_STATE_STOP) {   /* after STOP unsoli*/
                    return(sCurrent);               /* return CPM_NOISE     */
                }
                if (i > CPM_SLEEP_COUNT) {          /* normal loop          */
                    PifSleep(CPM_SLEEP_TIME*1000);  /* sleep for other task */
                    i = 1;                          /* reset counter        */
                    break;
                } else {
                    i++;                            /* count up             */
                    break;
                }
                                                    /* check message type   */
            case CPM_END:                           /* created message !    */
                return(CpmCheckMessageType(CpmSpool.auchData));
                                                    /* check message type   */
        }
    }
}

/*
*=============================================================================
** Format  : SHORT   CpmSetReadTimer(SHORT sCurrent);
*               
*    Input : sCurrent   current status during reading
*    InOut : none
* 
**   Return : CPM_RET_SUCCESS    -success to recover port
*             CPM_RET_FAIL       -fail to recover port      
*
** Synopsis: This function close RS-232 port and open again.
*
*=============================================================================
*/
SHORT   CpmSetReadTimer(SHORT sCurrent)
{
    USHORT  usTime;
    SHORT   sRet;

//    if (sCurrent == CPM_CONT) {
        usTime = CPM_FIX_TIME;
//    } else {
//        usTime = CpmGetMDCTime(MDC_CPM_WAIT_TIME);
//    }
    
    PifRequestSem(usCpmSemHandle);
    sRet = CpmOpenSetPort(usTime);                  /* open port & set time */
    PifReleaseSem(usCpmSemHandle);
    return(sRet);
}

/*
*=============================================================================
** Format  : SHORT   CpmReceiveCom(SHORT sCurrent)
*               
*    Input : sCurrent   current status
*    InOut : none
* 
** Return  : CPM_CONT        : data is continue on message
*            CPM_END         : data is last on message
*            CPM_NOIZE       : unnecessary data
**           CPM_RET_LENGTH  : received data length error
*            CPM_RET_TIMEOUT : timeout err   
*            CPM_RET_OFFLINE : offline err   
*            CPM_RET_FAIL    : other error
*
** Synopsis: This function receives data from either an RS-232 Port or from a
*				Charge Post Host over ethernet.  The data received is put into
*				the Charge Post spool static area.
*=============================================================================
*/
SHORT   CpmReceiveCom(SHORT sCurrent)
{
    UCHAR   *puchP;
    SHORT   sRet, sRet1Byte;
	UCHAR   uchCpmStatusSave;
	SHORT	sRetry = 5;

    memset(&CpmSpool, 0, sizeof(CpmSpool)); /* clear spool area     */

	// The following are some basic data integrity checks in case people change #defines
	// If any of these fail, PIF_ERROR_NET_TIMEOUT value conflicts with other return values
	// and the receive loop may not function correctly.
#if PIF_ERROR_NET_TIMEOUT == CPM_RET_TIMEOUT
#pragma message(" *** WARNING:  PIF_ERROR_NET_TIMEOUT == CPM_RET_TIMEOUT")
#endif

#if PIF_ERROR_NET_TIMEOUT == CPM_RET_FAIL
#pragma message(" *** WARNING:  PIF_ERROR_NET_TIMEOUT == CPM_RET_FAIL")
#endif

#if PIF_ERROR_NET_TIMEOUT == CPM_END
#pragma message(" *** WARNING:  PIF_ERROR_NET_TIMEOUT == CPM_END")
#endif

	if (hsCpmPortHandle == CPM_PORT_USENET) {
		sRet = PIF_ERROR_NET_TIMEOUT;
		while (sRet == PIF_ERROR_NET_TIMEOUT) {

			// We use this semaphore which gates access to the uchCpmStatus global
			// variable as a means to synchronize sends and receives of requests/responses
			// for users between the sending code in cpmsend.c and the receiving code
			// here.  This will ensure that a receive will be synchroized with a user
			// send or if an unsolicited message arrives, we will hande that.
			// The global variable uchCpmStatus tells us whether we are expecting a
			// response or not.
			PifRequestSem(usCpmSemHandle);                  /* for usCpmStatus      */
			sRet = CpmHostRecv(CpmSpool.auchData, sizeof(CpmSpool.auchData));
			uchCpmStatusSave = uchCpmStatus;
		    PifReleaseSem(usCpmSemHandle);                  /* for usCpmStatus      */
			if (sRet < 0) {
				CpmSpool.usLen = 0;
				if (uchCpmStatusSave == CPM_STATE_SEND_OK) {
					sRetry--;
					if(sRetry < 0){
						switch (sRet) {
							case PIF_ERROR_NET_TIMEOUT:
								sRet = CPM_RET_TIMEOUT;
								break;
							default:
								sRet = CPM_RET_FAIL;
								break;
						}
					}
				}else{
					sRetry = 5;
				}
			}
			else {
				CpmSpool.usLen = sRet;
				sRet = CPM_END;
			}
		}
		return sRet;
	}
	else {
		for (;;) {
			PifRequestSem(usCpmSemState);               /* for uchCpmStatus     */
			if (hsCpmPortHandle == CPM_PORT_NONE) {     /* if RS-232 port open? */
				PifReleaseSem(usCpmSemState);           /* for uchCpmStatus      */
				sRet = CpmRecoverPort();                /* port recover process */
				if ((sRet == CPM_RET_FAIL) ||
					(sRet == CPM_RET_COM_ERRORS)) {             /* RS-232 port not open */
					return(sRet);                       /* return port open err */
				}
			}
			else {
				PifReleaseSem(usCpmSemState);           /* for uchCpmStatus      */
			}
			if (sCurrent == CPM_NOISE) {
				sRet = PifReadCom(hsCpmPortHandle,      /* read 1byte from PMS  */
									(VOID *)&CpmSpool.auchData[0],
									1);               
				sRet = CpmCheckComError(sRet, 1, CPM_RECEIVE);
														/* check PifReadCom err */
			} else {

				CpmSetReadTimer(sCurrent);
				sRet = PifReadCom(hsCpmPortHandle,      /* read next data       */
									(VOID *)&CpmSpool.auchData[1],
									(CPM_SPOOL_SIZE-1));      
				sRet = CpmCheckComError(sRet, 0, CPM_RECEIVE);
														/* check PifReadCom err */
			}
			switch (sRet) {
				case CPM_RET_SUCCESS:
					break;

				case PIF_ERROR_COM_TIMEOUT:             /* time out error ?     */
					sRet1Byte = Cpm1ByteTimeOut();
					if (sRet1Byte == CPM_READ_AGAIN) {
						if (sCurrent == CPM_CONT) {
							sCurrent = CPM_NOISE;
						}
						continue;
					} else {
						return(sRet1Byte);
					}

				case PIF_ERROR_COM_POWER_FAILURE:       /* power down error ?   */
					sRet1Byte = Cpm1BytePowerOff();
					if (hsCpmNetHandle != CPM_PORT_NONE) {  /* not stand alone? */
						CpmNetClose();                  /* close old IHC port   */
						CpmNetOpen();                   /* open new IHC port    */
					}
					if (sRet1Byte == CPM_READ_AGAIN) {
						continue;
					} else {
						return(sRet1Byte);
					}

				default:
					return(sRet);
			}

			if (sCurrent == CPM_NOISE) {
				if (CpmSpool.auchData[0] == CPM_START_CHAR) {
					sCurrent = CPM_CONT;
					PifSleep(200);
				}
				continue;
			} else {
				puchP = memchr((VOID *)&CpmSpool.auchData[1],
							   CPM_END_CHAR,
							   (CPM_SPOOL_SIZE-1));
				if (puchP == 0) {
					return(CPM_RET_LENGTH);
				} else {
					CpmSpool.usLen = ((USHORT)(puchP - &CpmSpool.auchData[0]))+1;
					return(CPM_END);
				}
			}
		}
	}
}

/*
*=============================================================================
** Format  : SHORT   CpmRecoverPort(VOID);
*               
*    Input : none
*    InOut : none
* 
**   Return : CPM_RET_SUCCESS    -success to recover port
*             CPM_RET_FAIL       -fail to recover port      
*
** Synopsis: This function close RS-232 port and open again.
*
*=============================================================================
*/
SHORT   CpmRecoverPort(VOID)
{
    USHORT      usWaitTimer;
    SHORT       sRet;

    PifRequestSem(usCpmSemHandle);
    if (hsCpmPortHandle != CPM_PORT_NONE) {
        PifCloseCom(hsCpmPortHandle);               /* port close           */
        hsCpmPortHandle = CPM_PORT_NONE;            /* reset handle value   */
    } else {
        PifCloseCom(hsCpmSaveHandle);               /* port close           */
    }

    usWaitTimer = CpmGetMDCTime(MDC_CPM_WAIT_TIME);
    sRet=CpmOpenSetPort(usWaitTimer);               /* open port & set time */
    PifReleaseSem(usCpmSemHandle);

    return(sRet);                                   /* port recover fail    */
}

/*
*=============================================================================
** Format  : SHORT   Cpm1ByteTimeOut(VOID)
*               
*    Input : none
*    InOut : none
* 
*            CPM_READ_AGAIN  : wait left time
**           CPM_RET_TIMEOUT : time out error
*
** Synopsis: This function executes timeout process.
*
*=============================================================================
*/
SHORT   Cpm1ByteTimeOut(VOID)
{
    USHORT  usTime = CPM_MAX_SET_TIMER;
    SHORT   sRet;

    if (uchCpmStatus != CPM_STATE_SEND_OK) {        /* after write message  */
        return(CPM_RET_TIMEOUT);
    }
    usTime = CpmCheckLeftTime();
    if (usTime != CPM_PASS) {
        PifRequestSem(usCpmSemHandle);
        sRet = CpmOpenSetPort(usTime);              /* open port & set time */
        PifReleaseSem(usCpmSemHandle);
        return(CPM_READ_AGAIN);                     /* ret wait left time   */
    } else {
        return(CPM_RET_TIMEOUT);                    /* return time out      */
    }
}

/*
*=============================================================================
** Format  : SHORT   Cpm1BytePowerOff(VOID)
*               
*    Input : none
*    InOut : none
* 
** Return  : CPM_READ_AGAIN  : wait left time
**           CPM_RET_OFFLINE : offline error
*
** Synopsis: This function executes power down process.
*
*=============================================================================
*/
SHORT   Cpm1BytePowerOff(VOID)
{
    USHORT  usTime;
    SHORT   sRet;

    if (uchCpmStatus != CPM_STATE_SEND_OK) {        /* after write message  */
        return(CPM_RET_OFFLINE);
    }
    usTime = CpmCheckLeftTime();
    if (usTime != CPM_PASS) {
        PifRequestSem(usCpmSemHandle);
        sRet = CpmOpenSetPort(usTime);              /* open port & set time */
        PifReleaseSem(usCpmSemHandle);
        return(CPM_READ_AGAIN);                     /* ret wait left time   */
    } else {
        return(CPM_RET_OFFLINE);                    /* return offline       */
    }
}

/*
*=============================================================================
** Format  : USHORT   CpmCheckMessageType(UCHAR *pData);
*               
*    Input : pData     address of message
*    InOut : none
* 
**   Return : CPM_TYPE_USER_RESP    -response to user
*             CPM_TYPE_UNSOLI       -unsoli.data from PMS
*             CPM_TYPE_ERR_RESP     -other data  
*
** Synopsis: This function check spool data.
*
*=============================================================================
*/
SHORT   CpmCheckMessageType(UCHAR *pData)
{
    CPMRCRSPDATA   *pRec;

    pRec = (CPMRCRSPDATA *)pData;
    switch (pRec->auchFuncCode2[0]) {               
        case CPM_FC2_ACCEPTED:                      /* user response data   */
        case CPM_FC2_REJECTED:
        case CPM_FC2_OVERRIDE:
        case CPM_FC2_NEEDGLINE:
        case CPM_FC2_PARTTEND:                      /* E-022 corr. '93 4/15 */
             return(CPM_TYPE_USER_RESP);

        case CPM_FC2_STOP_CHARG:                    /* unsoli. data         */
        case CPM_FC2_STOP_POST:  
        case CPM_FC2_START:
             return(CPM_TYPE_UNSOLI);

        default:                                    /* other data           */
             return(CPM_TYPE_ERR_RESP);
    }
}

/*
*=============================================================================
** Format  : VOID   CpmExeResponse(VOID);
*               
*    Input : none
*    InOut : none
* 
**   Return : none;
*
** Synopsis: This function forwards the Charge Post Host response to the requesting
*				user.  If the sending terminal is ourselves, then we just set up
*				out Charge Post Manager machine states and semaphores and return.
*				If the requesting terminal is another terminal, then we need to
*				forward the CPM Host response to that terminal and then 
*=============================================================================
*/
VOID    CpmExeResponse(VOID)
{
    CPMRCRSPDATA   *pRcv;
    DATE_TIME   Timer;

    pRcv = (CPMRCRSPDATA *)&CpmSpool.auchData[0];
    if (CpmSaveSend.uchTerminal == CPM_FROM_SELF) { /* if request from self */

        if (pRcv->auchLastMsgInd[0] == CPM_CONT_MSG) {
            PifGetDateTime(&Timer);                 /* get new time         */
            CpmSendTOD.uchHour = (UCHAR)Timer.usHour;
            CpmSendTOD.uchMin  = (UCHAR)Timer.usMin;
            CpmSendTOD.uchSec  = (UCHAR)Timer.usSec;
        } 
        PifReleaseSem(usCpmSemRcv);                 /* release for SELF term*/
        PifRequestSem(usCpmSemSpool);               /* request for CpmSpool */
        PifSleep(100);
    }
	else {

		CpmReturnMessage((UCHAR *)&CpmSpool.auchData[0], CpmSpool.usLen); 

		if (pRcv->auchLastMsgInd[0] == CPM_CONT_MSG) {
			PifGetDateTime(&Timer);                     /* get new time         */
			CpmSendTOD.uchHour = (UCHAR)Timer.usHour;
			CpmSendTOD.uchMin  = (UCHAR)Timer.usMin;
			CpmSendTOD.uchSec  = (UCHAR)Timer.usSec;
			return;                                     /* continue response    */
		}
		CpmWriteStatus(CPM_STATE_OUTSIDE);              /* change idle status   */
	}

    return;
}

/*
*=============================================================================
** Format  : VOID   CpmExeUnsoli(UCHAR uchState);
*               
*    Input : none
*    InOut : none
* 
**   Return : none;
*
** Synopsis: This function control unsoli.data & status.
*=============================================================================
*/
VOID    CpmExeUnsoli(UCHAR uchState)
{
    CPMRCRSPDATA   *pRcv;
    SHORT           sRet;
    DATE_TIME       Timer;

    pRcv = (CPMRCRSPDATA *)&CpmSpool.auchData[0];
    switch (uchState) {
        case CPM_STATE_STOP:                        /* CPM is stopping      */
            if (pRcv->auchFuncCode2[0] == CPM_FC2_START) {
                CpmWriteStatus(CPM_STATE_OUTSIDE);  /* change idle status   */
            }
            return;

        case CPM_STATE_OUTSIDE:
            if ((pRcv->auchFuncCode2[0] == CPM_FC2_STOP_CHARG) ||               
                (pRcv->auchFuncCode2[0] == CPM_FC2_STOP_POST)) {
                CpmWriteStatus(CPM_STATE_STOP);     /* change stop status   */
            }
            return;

        case CPM_STATE_SEND_OK:                     /* during user request  */
        case CPM_STATE_INSIDE:
            if ((pRcv->auchFuncCode2[0] == CPM_FC2_STOP_CHARG) ||               
                (pRcv->auchFuncCode2[0] == CPM_FC2_STOP_POST)) {

                CpmWriteStatus(CPM_STATE_STOP);     /* change stop status   */
                PifGetDateTime(&Timer);             /* get new time         */
                CpmSendTOD.uchHour = (UCHAR)Timer.usHour;
                CpmSendTOD.uchMin  = (UCHAR)Timer.usMin;
                CpmSendTOD.uchSec  = (UCHAR)Timer.usSec;

                for (;;) {
                    sRet = CpmCreateMessage();      /* get next data(msg)   */
                    if (sRet == CPM_TYPE_USER_RESP) {
                        CpmExeResponse();           /* send response to user*/
                    } else if (sRet == CPM_TYPE_UNSOLI) {
                        if (pRcv->auchFuncCode2[0] == CPM_FC2_START) {
                            CpmWriteStatus(CPM_STATE_OUTSIDE);
                                                    /* change idle status   */
                        }
                    } else {
                        CpmExeError(CPM_RET_FAIL);  /* ret message to user  */
                    }
                }
            }
            return;
    }
}

/*
*=============================================================================
** Format  : VOID   CpmExeError(SHORT sErrCode);
*               
*    Input : sErrCode   - error code
*    InOut : none
* 
**   Return : none;
*
** Synopsis: This function send error response to user.
*=============================================================================
*/
VOID    CpmExeError(SHORT sErrCode)
{

    if (CpmSaveSend.uchTerminal == CPM_FROM_SELF) { /* if request from self */
        CpmSpool.sErrCode = sErrCode;
        PifReleaseSem(usCpmSemRcv);                 /* release for SELF term*/
        PifRequestSem(usCpmSemSpool);               /* request for CpmSpool */
        CpmWriteStatus(CPM_STATE_OUTSIDE);          /* change idle status   */
        return;                                     /* do not send by IHC   */ 
    }
                                                    /* ret err msg by IHC   */
    CpmReturnErrMessage(sErrCode);                  /* ret message to user  */
    CpmWriteStatus(CPM_STATE_OUTSIDE);              /* change idle status   */
    return;
}
/* ---------- End of CPMRCV.C ---------- */


