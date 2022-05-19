/*
******************************************************************************
**                                                                          **
**        *=*=*=*=*=*=*=*                                                   **
**        *  NCR 2170   *             NCR Corporation, E&M OISO             **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1996-9              **
**    <|\/~             ~\/|>                                               **
**   _/^\_               _/^\_                                              **
**                                                                          **
******************************************************************************
*=============================================================================
* Title       : Enhanced EPT User Interface Routines
* Category    : Enhanced EPT module,NCR2170 GP R2.0
* Program Name: EEPTIF.C
* ----------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*               EEptInit();
*                   EEptInitialArea();
*                   EEptGetComPort();
*               EEptSendMessage();
*               EEptReceiveMessage();
*
*-----------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name       : Description
* Feb-19-97 : 02.00.00 : T.Yatuhasi   : Initial
* Dec-09-99 : 01.00.00 : hrkato       : Saratoga
* Sep-23-16 : 02.02.01 : R.Chambers   : if SCF_DATANAME_STOREFORWARD not found do not disable Store and Forward.
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
;=============================================================================
;+                  I N C L U D E     F I L E s                              +
;=============================================================================
**/
/**------- MS - C ------**/
#include	<tchar.h>

#include <memory.h>

/**------- 2170 local------**/
#include "ecr.h"
#include "pif.h"
#include "plu.h"
#include "termcfg.h"
#include "paraequ.h"
#include "para.h"
#include "pararam.h"
#include "eeptl.h"
#include "scf.h"
#include "uie.h"
#include "csstbpar.h"

/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/
/**     protocol       **/
PROTOCOL    EEptProtocol;                            /* protocol for EEPT     */

/**     semaphore       **/
PifSemHandle  usEEptSemMain = PIF_SEM_INVALID_HANDLE;          /* for access to stub   */
PifSemHandle  usEEptSemSend = PIF_SEM_INVALID_HANDLE;          /* for start send thred */
PifSemHandle  usEEptSemRcv = PIF_SEM_INVALID_HANDLE;           /* for inform user I/F  */
PifSemHandle  usEEptSemSpool = PIF_SEM_INVALID_HANDLE;         /* for EEptSpool         */
PifSemHandle  usEEptSemState = PIF_SEM_INVALID_HANDLE;         /* for uchEEptStatus     */
PifSemHandle  usEEptSemHandle = PIF_SEM_INVALID_HANDLE;        /* for hsEEptPortHandle  */

/**     RS-232 Port     **/
CHAR    chEEptPortID;                                /* RS-232 Port connect# */
SHORT   hsEEptPortHandle;                            /* port handle saved    */
/* SHORT   hsEEptSaveHandle;                         * port handle backup (R2.0)  */

/**     EEPT STATUS      **/
UCHAR  uchEEptStatus;                                /* current state of EEPT */

/**     SEND DATA       **/
EEPTSAVESEND EEptSaveSend;                            /* send data save area  */
USHORT      usEEptSeqNo;                             /* request data seq no. (R2.0) */

/**     send time       **/
EEPTTIMER    EEptSendTOD;                             /* message send time    */


/*
*=============================================================================
*
** Format  : VOID   EEptInit(VOID);
*
*    Input :    nothing
*
*   Output :    nothing
*    InOut :    nothing
*
** Return  :    nothing
*
** Synopsis:    This function initializes EEPT.
*               This Creates semaphore, creates thread of send & receive,
*               and opens port for RS-232 and IHC.
*=============================================================================
*/
VOID    EEptInit(VOID)
{
    TCHAR   tchDeviceName[SCF_USER_BUFFER];
    TCHAR   tchDllName[SCF_USER_BUFFER];
	ULONG	dwCount;

    chEEptPortID = (UCHAR)EEptGetComPort();     /* get RS-232 port ID,  Saratoga */
    if (chEEptPortID == EEPT_PORT_NONE) {
		XEptToggleStoreForward(DSI_STORE_AND_FORWARD_OFF);
		StoreForwardOFF();
        return;
    }

    EEptInitialArea();

    usEEptSemMain = PifCreateSem(1);                 /* for access to stub   */
    usEEptSemSend  = PifCreateSem(0);                /* for start send thred */
    usEEptSemState = PifCreateSem(1);                /* for uchEEptStatus     */
    usEEptSemHandle = PifCreateSem(1);               /* for hsEEptPortHandle  */
    usEEptSemRcv  = PifCreateSem(0);                 /* for start recv thred */
    usEEptSemSpool  = PifCreateSem(0);               /* for EEptSpool         */

    EEptSetProtocol();                                 /* Set protocol data    */
    if (EEptSetReadTimer(0) == EEPT_RET_COM_ERRORS) {  /* open RS-232 port     */ //SR517
		UieErrorMsg(LDT_EPT_OFFLINE_ADR, UIE_WITHOUT);		
	}

    if (chEEptPortID == EEPT_PORT_USENET) {         
		EEptHostOpen();
	}
    else if (chEEptPortID == EEPT_PORT_USEDLL) {         
		ScfGetActiveDevice(SCF_TYPENAME_EPT_EX_DLL, &dwCount, tchDeviceName, tchDllName);
		if(dwCount) {
			ULONG   dwDataType, dwRet;

			XEptDllOpen(tchDeviceName, tchDllName);
			if(CliParaMDCCheck(MDC_STR_FWD_ADR, EVEN_MDC_BIT0)) {
				//Store and Forward functionality is Enabled.
				// Now we will check to see if it is turned on.
				dwRet = ScfGetParameter (tchDeviceName, SCF_DATANAME_STOREFORWARD, &dwDataType, tchDllName, sizeof(tchDllName), &dwCount);
				if(dwRet != SCF_SUCCESS) {
					// if fetching the data from the Registry fails then turn off Store and Forward however we want to
					// leave Store and Forward enabled. What can happen is that the first time GenPOS runs under Windows 7
					// the Registry will not contain the keyword/value for StoreAndForward so we do not have a record of
					// it. In this case it is reasonable to turn Store and Forward off but it is not reasonable
					// to disable Store and Forward.  Amtrak testing with Windows 7 ran into a problem with this
					// requiring use of AC102 after initializing a tablet for a train.
					// Sep-23-2016, R.Chambers
					ParaResetStoreBitFlags(PIF_STORE_AND_FORWARD_ONE_ON | PIF_STORE_AND_FORWARD_TWO_ON); // Store and Forward default is Disabled so ensure that indicator is Off
				}
				else {
//					ParaSetStoreFlag( (UCHAR)_ttoi (tchDllName)); // get Store and Forward indicator status from the registery
//					if (ParaStoreForwardFlag() < PIF_STORE_AND_FORWARD_OFF || ParaStoreForwardFlag() > PIF_STORE_AND_FORWARD_ON)
//						ParaSetStoreFlag(PIF_STORE_AND_FORWARD_OFF); // Store and Forward default is Enabled so ensure that indicator is in range
				}
			}
			else {
				ParaSetStoreFlag(PIF_STORE_AND_FORWARD_DISABLED); // Store and Forward is Disabled so ensure that indicator is Off
			}

			XEptToggleStoreForward(DSI_STORE_AND_FORWARD_OFF);
			XEptToggleStoreForward(ParaStoreForwardFlag());
		}

		ScfGetActiveDevice(SCF_TYPENAME_EPT_EX_DLL1, &dwCount, tchDeviceName, tchDllName);
		if(dwCount){
			XEptDllOpen(tchDeviceName, tchDllName);
		}

		// Since the function XEptDllOpen() only allows for a single instance of the
		// DSI Client control, calling it multiple times may result in previous instances
		// of the DSI Client control being forgotten.  Problem if SCF_TYPENAME_EPT_EX_DLL is
		// used with one of the other types below.
//		ScfGetActiveDevice(SCF_TYPENAME_EPT_EX_DLL2, &dwCount, tchDeviceName, tchDllName);
//		if(dwCount){
//			XEptDllOpen(tchDeviceName, tchDllName);
//		}

//		ScfGetActiveDevice(SCF_TYPENAME_EPT_EX_DLL3, &dwCount, tchDeviceName, tchDllName);
//		if(dwCount){
//			XEptDllOpen(tchDeviceName, tchDllName);
//		}
	}
	/* start Receive thread */
	{
		CHAR  CONST *aszEEptRcvThreadName = "EEPT_RCV";      /* receive name     */
		VOID (THREADENTRY *pFunc1)(VOID) = EEptMainReceiveTask;    /* Add R2.0 */
		PifBeginThread(pFunc1, 0, 0, PRIO_EEPT_RCV, aszEEptRcvThreadName);
	}

	/* start Send thread    */
	{
		CHAR  CONST *aszEEptSendThreadName = "EEPT_SEND";    /* send name        */
		VOID (THREADENTRY *pFunc2)(VOID) = EEptMainSendTask;       /* Add R2.0 */
		PifBeginThread(pFunc2, 0, 0, PRIO_EEPT_SEND, aszEEptSendThreadName);
	}

}

/*
*=============================================================================
** Format  : VOID   EEptInitialArea(VOID);
*
*    Input :    nothing
*   Output :    nothing
*    InOut :    nothing
** Return  :    nothing
*
** Synopsis:    This function initializes static area of EEPT.
*=============================================================================
*/
VOID    EEptInitialArea(VOID)
{
    uchEEptStatus = EEPT_STATE_OUTSIDE;               /* current state of EPT */
    hsEEptPortHandle = EEPT_PORT_NONE;                /* port handle saved    */

#ifdef DEBUG
    memset(&aEEptLog, 0, sizeof(EEPT_232C_LOG));      /* clear log area (R2.0) */
    aEEptLog.uchExecuteFlag = 1;                      /* set logging on */
#endif

    usEEptSeqNo = 1;                                 /* request data sequence #(R2.0) */

}

/*
*=============================================================================
** Format  : SHORT   EEptGetComPort(VOID)
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : 0 >=          ; Port ID
*            EEPT_PORT_NONE ; nothing
*
** Synopsis: This function search port id for PMS.
*=============================================================================
*/
SHORT   EEptGetComPort(VOID)
{
    SYSCONFIG CONST FAR *pSys;
    SHORT               i;

    pSys = PifSysConfig();                      /* get system configration  */

	if (pSys->usInterfaceDetect & SYSCONFIG_IF_EEPT_LAN) {
		return (EEPT_PORT_USENET);
	}
	
	// chgeck to see if we are using a DLL type of EPT interface
	// such as Mercury, DataCap, or FreedomPay.
	if (pSys->usInterfaceDetect & SYSCONFIG_IF_EEPT_DLL) {
		return (EEPT_PORT_USEDLL);
	}
	else if (pSys->usInterfaceDetect & SYSCONFIG_IF_EEPT_DLL1) {
		return (EEPT_PORT_USEDLL);
	}
	else if (pSys->usInterfaceDetect & SYSCONFIG_IF_EEPT_DLL2) {
		return (EEPT_PORT_USEDLL);
	}
	else if (pSys->usInterfaceDetect & SYSCONFIG_IF_EEPT_DLL3) {
		return (EEPT_PORT_USEDLL);
	}

    if (!(pSys->uchCom & COM_PROVIDED)) {       /* exist RS-232 board ?     */
        return(EEPT_PORT_NONE);
    }

    for (i = 1; i < PIF_LEN_PORT; i++) {        /* get port ID              */
                                                /* auchComPort[0] = R/J     */
        if (pSys->auchComPort[i] == DEVICE_EPT_EX) {    /* Saratoga */
            return(i);
        }
    }
    return(EEPT_PORT_NONE);
}

/*
*=============================================================================
** Format  : SHORT   EEptSendMessage(UCHAR *pSendData, USHORT usSendLen)
*
*    Input : pSendData      pointer of send data
*            usSendLen      length of send data
*    InOut : none
*
**   Return : EEPT_RET_SUCCESS    -accept send process
*             EEPT_RET_BUSY       -reject process
*             EEPT_RET_STOP       -stopping PMS
*             EEPT_RET_PORT_NONE  -PMS is not provide
*
** Synopsis: This function sends data to PMS.
*=============================================================================
*/
SHORT   EEptSendMessage(UCHAR *pSendData, USHORT usSendLen)
{
    USHORT  usCompare;

    if (chEEptPortID == EEPT_PORT_NONE) {
        return(EEPT_RET_PORT_NONE);                  /* ret PMS not provide  */
    }
    PifRequestSem(usEEptSemMain);                    /* for access to stub   */
    PifRequestSem(usEEptSemState);                   /* for uchEEptStatus     */

    switch (uchEEptStatus) {                         /* current EEPT status ? */
        case EEPT_STATE_SEND_OK:
        case EEPT_STATE_INSIDE:
            usCompare = EEptComparePrevious(pSendData, usSendLen);
            if (usCompare == EEPT_SAME) {
                PifReleaseSem(usEEptSemState);
                PifReleaseSem(usEEptSemMain);
                return(EEPT_RET_SUCCESS);            /* ret process accepted */

            } else {
                PifReleaseSem(usEEptSemState);       /* for usEEptStatus      */
                PifReleaseSem(usEEptSemMain);
                return(EEPT_RET_BUSY);               /* ret process rejected */
            }

/* R2.0 Start */
        case EEPT_STATE_UNSOLI:
            PifReleaseSem(usEEptSemState);       /* for usEEptStatus      */
            PifReleaseSem(usEEptSemMain);
            return(EEPT_RET_BUSY);               /* ret process rejected */
/* R2.0 End   */

        case EEPT_STATE_OUTSIDE:
            break;                                  /* go to next process   */
    }
                                                    /* accept process start */
    uchEEptStatus = EEPT_STATE_INSIDE;                /* status change        */
	EEptSaveSend.auchReqData.usType = EEPTREQUNIONTYPE_REQDATA;
    memcpy(&EEptSaveSend.auchReqData.u.eeptReqData, pSendData, usSendLen);
                                                    /* copy send data       */
    EEptSaveSend.usLen = usSendLen;                  /* copy length of data  */

    PifReleaseSem(usEEptSemState);                   /* for chEEptStatus      */
    PifReleaseSem(usEEptSemSend);                    /* for start send thred */
    PifReleaseSem(usEEptSemMain);                    /* for access to stub   */
    return(EEPT_RET_SUCCESS);
}

/*
*=============================================================================
** Format  : SHORT   EEptReceiveMessage(UCHAR *pRecData, USHORT usRecSize);
*
*    Input : pRecData       pointer of receive data
*            usRecSize      length of receive data
*    InOut : none
*
**   Return : >= 0               -length of response message
*             EEPT_RET_PORT_NONE  -PMS is not provide
*             EEPT_RET_LENGTH     -length error
*
** Synopsis:    This function sends received data to SELF
*               (called from only SELF terminal).                   R2.0
*=============================================================================
*/
SHORT   EEptReceiveMessage(UCHAR *pRecData, USHORT usRecSize)
{
    USHORT  usCompare;
    SHORT   sRet;

    if (chEEptPortID == EEPT_PORT_NONE) {
        return(EEPT_RET_PORT_NONE);                  /* ret PMS not provide  */
    }

    PifRequestSem(usEEptSemRcv);                     /* for inform rcv data  */

//    if (EEptSpool.sErrCode < 0) {
//        sRet = EEptSpool.sErrCode;                   /* if receive task err  */
//    } else {
        if (EEptSpool.usLen <= usRecSize) {          /* save area size is OK */
            usCompare = EEptCheckSpoolData();        /* check send address   */
            if (usCompare == EEPT_SAME) {
				NHPOS_ASSERT((EEptSpool.usLen <= usRecSize));
				usRecSize = ((EEptSpool.usLen <= usRecSize) ? EEptSpool.usLen : usRecSize);
                memcpy(pRecData, EEptSpool.auchData, usRecSize);
				if (EEptSpool.sErrCode < 0) {
					sRet = EEptSpool.sErrCode;                   /* if receive task err  */
				} else {
					sRet = (SHORT)usRecSize;             /* return read length   */
					if (EEptSpool.sErrCode == EEPT_RET_AUTO_STORED) {
						sRet |= 0x4000;
					}
				}
            } else {
                sRet = EEPT_RET_FAIL;                /* return length error  */
            }
		} else if (EEptSpool.sErrCode < 0) {
	        sRet = EEptSpool.sErrCode;                   /* if receive task err  */
        } else {
            sRet = EEPT_RET_LENGTH;                  /* return length error  */
        }
//    }
    PifReleaseSem(usEEptSemSpool);                   /* for EEptSpool         */
    return(sRet);
}

SHORT  EEptClearSendRecv (VOID)
{
	memset(&EEptSpool, 0, sizeof(EEPTSPOOL));         /* clear spool area     */
	memset(&EEptSaveSend, 0, sizeof(EEptSaveSend));   /* clear spool area     */
	return 0;
}

/*
*=============================================================================
** Format  : SHORT   StoreForwardOFF();
*
*    Input : none
*    InOut : none
*
**   Return : true   Store and Forward was on
*             false  Store and Forward was off
*
** Synopsis:    
*=============================================================================
*/
USHORT	StoreForwardOFF()
{
	USHORT  usStoreForwardRet = 0;
	PARAMDC WorkMDC;

	// Get the current status so that we can return that
	WorkMDC.uchMajorClass = CLASS_PARAMDC;
	WorkMDC.usAddress = MDC_STR_FWD_ADR;
	CliParaRead( &WorkMDC );     
	usStoreForwardRet = ((WorkMDC.uchMDCData & EVEN_MDC_BIT0) != 0);

	//turn off store and forward
	WorkMDC.uchMDCData &= ~EVEN_MDC_BIT0; // MDC_PARAM_BIT_A now not set, so SF now disabled
	CliParaWrite(&WorkMDC);

	return usStoreForwardRet;
}

/* ---------- End of EEPTIF.C ---------- */