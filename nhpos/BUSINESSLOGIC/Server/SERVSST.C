/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : SERVER module, User Interface Source File                        
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVSST.C                                            
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen)
*       2012  -> GenPOS Rel 2.2.0
*       2014  -> GenPOS Rel 2.2.1
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*           SstInitialize();            Server Initialize (make thread)
*           SstSetFlag();               Set special request flag
*           SstSetFlagWaitDone();       Set flag and Wait to do request
*           SstResetFlag();             Reset special reauest flag
*           SstReadFlag();              Read special request flag
*           SstIncTransNo();            Increment transaction #
*           SstReadInqStat();           Read inquiry data
*           SstChangeInqCurTransNo();   Change inquiry transaction #    
*           SstChangeInqPreTransNo();   Change inquiry prev. transaction #    
*           SstChangeInqDate();         Change inquiry date
*           SstChangeInqStat();         Change inquiry status
*           SstSetBcasPointer();        Set pointer of broadcast reg# block
*           SstSetBcasRegNo();          Set broadcast register #.
*           SstChkBcasing();            Check broadcasting terminal
*           SstResetCurTransNo();       Reset current transaction No.
*           SstResetIndTransNo();       Reset ind. transaction No.
*           SerCreateTmpFile();         Create server temporary file
*           SerDeleteTmpFile();         Delete server temporary file
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Jun-04-93:H.Yamaguchi: Modified same as HP Int'l
* Apr-12-94:K.You      : Add flex GC file feature(add hsSerTmpFile, SerCreateTmpFile
*          :           : ,SerDeleteTmpFile, husSerCreFileSem)(mod. SstInitialize).
* Aug-04-00:M.Teraki   : change num. of kps timer (1 -> No.of KP)
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
#include    <math.h>
#include    <memory.h>

#include    "ecr.h"
#include    "pif.h"
#include    "plu.h"
#include    "csstbfcc.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "csserm.h"
#include    "paraequ.h"
#include    "para.h"
#include    "pararam.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "appllog.h"
#include    "servmin.h"
#include    "rfl.h"
#include    "nb.h"

/*
------------------------------------------
    Internal Work Area (Uninitialized)
------------------------------------------
*/

/*
------------------------------------------
    Internal Work Area (initialized)
------------------------------------------
*/
XGRAM           SerSndBuf;                  /* Send response buffer */
XGRAM           SerRcvBuf;                  /* Receive request buffer */
SERNETCONFIG    SerNetConfig;               /* NET configration */
USHORT          usSerStatus;                /* Server status */
USHORT          fsSerExeSpe;                /* special request flags */
UCHAR           auchSerTmpBuf[SER_MAX_TMPBUF];  /* temporary buffer */
SERPREVIOUS     SerResp;                    /* Response common structure */
SERTIMER        SerTimer;                   /* Timer Control */
SERTIMER        SerKpsTimer[SER_MAX_KP];    /* Timer Control for KPS */
SERTIMER        SerBcasTimer;               /* Timer Control for B-cast */
SERTIMER        SerSpsTimer;                /* Timer Control for SPS */
USHORT          husSerIFSerCli;             /* Semapho, STUB - SERVER */       
PifSemHandle    usSerCliSem = PIF_SEM_INVALID_HANDLE;                /* Save samapho handle */
PifSemHandle    husSerCreFileSem = PIF_SEM_INVALID_HANDLE;           /* Samapho, create temp. file - used temp. file */
SHORT           sSerExeError;               /* Special request error */
CLIOPBCAS       *pSerBcas;                  /* broadcast inf. pointer */
USHORT          usSerBcasFunCode;           /* Broadcast function code */
USHORT          fsSerShrStatus;             /* Shred printer status  */
SHORT           hsSerTmpFile;               /* save area for file handle of temporary buffer file */

/* static VOID (THREADENTRY *pFunc)(VOID) = SerStartUp;    / Add R3.0 */
static VOID (PIFENTRY *pPifSaveFarData)(VOID) = PifSaveFarData; /* saratoga */

/*
------------------------------------------
    Code Area
------------------------------------------  
*/                                              
/* UCHAR FARCONST  SERTHREADNAME[] = "SERVER_T";   / THREAD NAME */      
TCHAR  FARCONST  aszSerTmpFileName[]   = _T("SERTMP");

/*
*===========================================================================
** Synopsis:    VOID    SstInitialize(USHORT usMode);
*
*     Input:    usMode - power up mode
*
** Return:      none.
*
** Description:
*   This function is called from STUB initialize and cerates SERVER MODULE
*   THREAD.
*===========================================================================
*/
VOID    SstInitialize(USHORT usMode)
{
    SHORT   hsFileHandle;

    if (usMode & POWER_UP_CLEAR_TOTAL) {
        usSerCurStatus = 0;  // clear the inquiry status, will be set later using SstChangeInqStat().
        memset(&Para.SerCurDate, 0, sizeof(CLIINQDATE));
        memset(Para.ausSerCurTransNo, 0, sizeof(Para.ausSerCurTransNo));
        memset(Para.ausSerPreTransNo, 0, sizeof(Para.ausSerPreTransNo));
    } else if (usMode & LOAD_MODE_PARA1) {
        memset(Para.ausSerCurTransNo, 0, sizeof(Para.ausSerCurTransNo));
        memset(Para.ausSerPreTransNo, 0, sizeof(Para.ausSerPreTransNo));
    }

    fsSerCurBcasInf = 0;  // reset broadcast status since we are restarting application (AC42)

    if (pPifSaveFarData != NULL) {              /* Saratoga */
        (*pPifSaveFarData)();                           
    } 

    husSerIFSerCli = PifCreateSem(1);           /* Create Semapho */

    /* open server temporary file */
    husSerCreFileSem = PifCreateSem(1);         /* Create Semapho */
    hsSerTmpFile = -1;
    hsFileHandle = PifOpenFile(aszSerTmpFileName, auchTEMP_OLD_FILE_READ_WRITE);    /* saratoga */
    if (hsFileHandle >= 0) {
        hsSerTmpFile = hsFileHandle;
    }
	else {
		PifLog (MODULE_SER_LOG, LOG_EVENT_SER_TEMP_FILE_ERR_01);
	}
}

/*
*===========================================================================
** Synopsis:    SHORT   SstSetFlag(USHORT fsFlag);
*
*     Input:    fsFlag  - Special Exe. Flag to set
*
** Return:      STUB_SUCCESS:   OK
*
** Description: This function sets special flag.
*===========================================================================
*/
SHORT   SstSetFlag(USHORT fsFlag)
{
    PifRequestSem(husSerIFSerCli);

    fsSerExeSpe |= fsFlag;

    PifReleaseSem(husSerIFSerCli);
    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    SHORT   SstSetFlagWaitDone(USHORT fsFlag, USHORT husCliSem);
*
*     Input:    husCliSem   - semapho handle STUB has.
*               fsFlag      - specified special flag.
*
** Return:      STUB_SUCCESS:   OK
*               other:          Error
*
** Description:
*   This function sets special flag and waits until the special request 
*   is executed by SERVER.  When SERVER completes the request, it will
*   release the semaphore and allow the thread using this function to continue.
*
*   This function is used in several places to start a particular action such
*   as an AC75 or an AC42 from the Supervisor UI thread and then wait for
*   other threads to complete the request and then release the semaphore.
*===========================================================================
*/
SHORT   SstSetFlagWaitDone(USHORT fsFlag, USHORT husCliSem)
{
    SYSCONFIG CONST *SysCon = PifSysConfig();

    if (SER_IAM_SATELLITE) {
		if (fsFlag == SER_SPESTS_MTOD) {
			NbResetMTUpToDate();
		}
		if (fsFlag == SER_SPESTS_BMOD) {
			NbResetBMUpToDate();
		}
        return STUB_SUCCESS;
    }

    if (((0 == SysCon->auchLaddr[0]) &&
         (0 == SysCon->auchLaddr[1]) &&
         (0 == SysCon->auchLaddr[2])) ||    /* comm. board not provide */
        (0 == SysCon->auchLaddr[3])) {      /* U.A is 0 */
        
        return  STUB_NOT_MASTER ;    /* Prohibit operation */
    }

    PifRequestSem(husSerIFSerCli);
    usSerCliSem = husCliSem;
    fsSerExeSpe |= fsFlag;
    PifReleaseSem(husSerIFSerCli);

    PifRequestSem(usSerCliSem);
	usSerCliSem = PIF_SEM_INVALID_HANDLE;
    return sSerExeError;
}

/*
*===========================================================================
** Synopsis:    SHORT   SstResetFlag(USHORT fsFlag);
*
*     Input:    fsFlag  - Special Exe. Flag to clear
*
** Return:      STUB_SUCCESS:   OK
*
** Description: This function resets special flag.
*===========================================================================
*/
SHORT   SstResetFlag(USHORT fsFlag)
{
    PifRequestSem(husSerIFSerCli);

    fsSerExeSpe &= ~fsFlag;

    PifReleaseSem(husSerIFSerCli);
    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    SHORT   SstReadFlag(USHORT *pusFlag);
*
*    Output:    pusFlag  - Pointer of Special Exe. Flag to copy
*
** Return:      STUB_SUCCESS:   OK
*
** Description: This function reads special flag.
*===========================================================================
*/
SHORT   SstReadFlag(USHORT *pfsFlag)
{
    PifRequestSem(husSerIFSerCli);

    *pfsFlag = fsSerExeSpe;

    PifReleaseSem(husSerIFSerCli);
    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    USHORT  SstIncTransNo(UCHAR uchUniqueAddr, USHORT usTranNo);
*
*     Input:    uchUniqueAddr   - unique address of each terminal
*               usTranNo        - transaction # to be written
*
** Return:      New Transaction #
*
** Description: This function increments transaction # of specified terminal.
*===========================================================================
*/
USHORT  SstIncTransNo(UCHAR uchUniqueAddr, USHORT usTranNo)
{
    USHORT usCurTransNo;                /* Add Work, Dec/1/2000 */

	if (uchUniqueAddr < 1 || uchUniqueAddr > PIF_NET_MAX_IP) {
		NHPOS_ASSERT_TEXT(0, "SstIncTransNo(): uchUniqueAddr is out of range.");
		return 0;
	}

    PifRequestSem(husSerIFSerCli);

    if (0 == Para.ausSerCurTransNo[uchUniqueAddr - 1]) {
        Para.ausSerPreTransNo[uchUniqueAddr - 1] = usTranNo;
        Para.ausSerCurTransNo[uchUniqueAddr - 1] = usTranNo;     /* Change 1 -> usTransNoHeHas, Dec/1/2000 */
    } else {
        Para.ausSerPreTransNo[uchUniqueAddr - 1] = Para.ausSerCurTransNo[uchUniqueAddr - 1];
        Para.ausSerCurTransNo[uchUniqueAddr - 1] = usTranNo;
    }

    if (0 == usTranNo) {
        Para.ausSerPreTransNo[uchUniqueAddr - 1] = 0;
        Para.ausSerCurTransNo[uchUniqueAddr - 1] = 1;
    }

    if (pPifSaveFarData != NULL) {              /* Saratoga */
        (*pPifSaveFarData)();                           
    } 

    usCurTransNo = Para.ausSerCurTransNo[uchUniqueAddr - 1]; /* Save Work, Dec/1/2000 */

    PifReleaseSem(husSerIFSerCli);
    return usCurTransNo;
}

/*
*===========================================================================
** Synopsis:    VOID    SstReadInqStat(SERINQSTATUS *pInqData);
*
*    Output:    pInqData    - inquiry data pointer to be copied
*
** Return:      none.
*
** Description:
*   This function reads the Inquiry status, date and transaction # of 
*   this cluster.
*===========================================================================
*/
VOID    SstReadInqStat(SERINQSTATUS *pInqData)
{
    PifRequestSem(husSerIFSerCli);

    pInqData->usStatus = usSerCurStatus;
    pInqData->CurDate = Para.SerCurDate;
    memcpy(pInqData->ausTranNo, Para.ausSerCurTransNo, sizeof(Para.ausSerCurTransNo));
    memcpy(pInqData->ausPreTranNo, Para.ausSerPreTransNo, sizeof(Para.ausSerPreTransNo));
    pInqData->fsBcasInf = fsSerCurBcasInf;

    PifReleaseSem(husSerIFSerCli);
}


/*
*===========================================================================
** Synopsis:    VOID    SstChangeInqCurPreTraNo(USHORT usTerminalNo, USHORT usCurTransNo, USHORT usPreTransNo);
*
*     Input:    usTerminalNo        Terminal number for change, 1 - 16
*               usCurTransNo        Current Transaction number for the terminal
*               usPreTransNo        Previous Transaction number for the terminal
*
** Return:      none.
*
** Description: This function changes (updates) Inquiry Transaction #.
*===========================================================================
*/
VOID    SstChangeInqCurPreTraNo(USHORT usTerminalNo, USHORT usCurTransNo, USHORT usPreTransNo)
{
	if (usTerminalNo < 1 || usTerminalNo > PIF_NET_MAX_IP) {
		NHPOS_ASSERT_TEXT(0, "SstChangeInqCurPreTraNo(): usTerminalNo is out of range.");
		return;
	}

    PifRequestSem(husSerIFSerCli);

	usTerminalNo--;
	Para.ausSerCurTransNo[usTerminalNo] = usCurTransNo;
	Para.ausSerPreTransNo[usTerminalNo] = usPreTransNo;
    if (pPifSaveFarData != NULL) {              /* Saratoga */
        (*pPifSaveFarData)();                           
    } 

    PifReleaseSem(husSerIFSerCli);
}

/*
*===========================================================================
** Synopsis:    VOID    SstChangeInqCurTraNo(USHORT *pausTransNo);
*
*     Input:    pausTransNo - pointer of Inquiry Transaction # to be written
*
** Return:      none.
*
** Description: This function changes (updates) Inquiry Transaction #.
*===========================================================================
*/
VOID    SstChangeInqCurTraNo(USHORT *pausTransNo)
{
    PifRequestSem(husSerIFSerCli);
    
    memcpy (&Para.ausSerCurTransNo, pausTransNo, sizeof(Para.ausSerCurTransNo));
    if (pPifSaveFarData != NULL) {              /* Saratoga */
        (*pPifSaveFarData)();                           
    } 

    PifReleaseSem(husSerIFSerCli);
}

/*
*===========================================================================
** Synopsis:    VOID    SstChangeInqPreTraNo(USHORT *pausTransNo);
*
*     Input:    pausTransNo - pointer of Inquiry Transaction # to be written
*
** Return:      none.
*
** Description: This function changes (updates) Inquiry Transaction #.
*===========================================================================
*/
VOID    SstChangeInqPreTraNo(USHORT *pausTransNo)
{
    PifRequestSem(husSerIFSerCli);
    
    memcpy (&Para.ausSerPreTransNo, pausTransNo, sizeof(Para.ausSerPreTransNo));
    if (pPifSaveFarData != NULL) {              /* Saratoga */
        (*pPifSaveFarData)();                           
    } 

    PifReleaseSem(husSerIFSerCli);
}

/*
*===========================================================================
** Synopsis:    VOID    SstChangeInqBcasInf(USHORT fsBcasInf);
*
*     Input:    fsBcasInf   - broadcast information to be written
*
** Return:      none.
*
** Description: This function changes (updates) Inquiry Broadcast infomation.
*===========================================================================
*/
VOID    SstChangeInqBcasInf(USHORT fsBcasInf)
{
    PifRequestSem(husSerIFSerCli);
    
    fsSerCurBcasInf = fsBcasInf;
    if (pPifSaveFarData != NULL) {              /* Saratoga */
        (*pPifSaveFarData)();                           
    } 

    PifReleaseSem(husSerIFSerCli);
}

/*
*===========================================================================
** Synopsis:    VOID    SstChangeInqDate(CLIINQDATE *pInqDate);
*
*     Input:    pInqDate    - pointer of Inquiry date struc. to be written
*
** Return:      none.
*
** Description: This function updates the Inquiry date and snapshots the current status.
*               The Inquiry date indicates the last known time when the
*               Master and Backup have been synchronized with an AC42 as
*               indicated by function SerESRBackUpFinal() or through a
*               message that has been processed by function SerRecvInqChgDate().
*===========================================================================
*/
VOID    SstChangeInqDate(CLIINQDATE *pInqDate)
{
    PifRequestSem(husSerIFSerCli);
    
    Para.SerCurDate = *pInqDate;
    if (pPifSaveFarData != NULL) {              /* Saratoga */
        (*pPifSaveFarData)();                           
    } 

    PifReleaseSem(husSerIFSerCli);
}

/*
*===========================================================================
** Synopsis:    VOID    SstChangeInqStat(USHORT usStatus);
*
*     Input:    usStatus    - New Status
*
** Return:      none.
*
** Description: This function changes (updates) Inquiry status.
*===========================================================================
*/
VOID    SstChangeInqStat(USHORT usStatus)
{
    PifRequestSem(husSerIFSerCli);

    usSerCurStatus = usStatus;
    if (pPifSaveFarData != NULL) {                  /* Saratoga */
        (*pPifSaveFarData)();
    }

    PifReleaseSem(husSerIFSerCli);
}

/*
*===========================================================================
** Synopsis:    VOID    SstSetBcasPointer(CLIOPBCAS *pBcas,
*                                         USHORT usFunCode);
*
*     Input:    pBcas - pointer to broadcast information structure
*               usFunCode - Broadcast function code
*
** Return:      none.
*
** Description:
*   This function sets pointer (or NULL) which points to broadcast 
*   information.  If this point is not NULL then it indicates to the Server Thread
*   that there is currently a broadcast going on which needs to be monitored.
*   The Server Thread will check to see if any Satellites have started a parameter
*   download and whether all downloads have completed or the broadcast timer has
*   expired.
*===========================================================================
*/
VOID    SstSetBcasPointer(CLIOPBCAS *pBcas, USHORT usFunCode)
{
    PifRequestSem(husSerIFSerCli);
    
    pSerBcas = pBcas;
    if (NULL != pSerBcas) {
        memset(pBcas, 0, sizeof(CLIOPBCAS) * CLI_ALLTRANSNO);
        usSerBcasFunCode = usFunCode;
    }

    PifReleaseSem(husSerIFSerCli);
}

/*
*===========================================================================
** Synopsis:    SHORT    SstSetBcasRegNo(UCHAR uchUA, ULONG usRegNo, 
*                                        USHORT usFunCode, UCHAR uchStatus);
*
*     Input:    uchUA   - terminal number (unique address)
*               ulRegNo - store/ reg number
*               usFunCode - Broadcast function code
*               uchStatus - 1: final, 2: broadcasting
*
** Return:      0:  Broadcast by A/C on master
*              -1:  Broadcast by A/C on satellite
*
** Description:
*   This function sets success status and store/ reg number for the 
*   terminal which is specifed by unique address (uchUA)
*===========================================================================
*/
SHORT    SstSetBcasRegNo(UCHAR uchUA, USHORT usRegNo, 
                         USHORT usFunCode, UCHAR uchStatus)
{
    SHORT   sRet = -1;

	NHPOS_ASSERT(uchUA <= CLI_ALLTRANSNO);

    PifRequestSem(husSerIFSerCli);
    
    if ((NULL != pSerBcas) &&
        ((CLI_FCOPREQALL == usFunCode) || (usSerBcasFunCode == usFunCode)) &&
        ((0 < uchUA) && (CLI_ALLTRANSNO >= uchUA))) {
        -- uchUA;
        if (1 == uchStatus) {
            (pSerBcas + uchUA)->usBcasRegNo = usRegNo;
        }
        (pSerBcas + uchUA)->uchBcasStatus = uchStatus;
        sRet = 0;
    }    

    PifReleaseSem(husSerIFSerCli);

    return sRet;
}

/*
*===========================================================================
** Synopsis:    SHORT    SstChkBcasing(VOID);
*
** Return:      0:  finished all terminal 
*              -1:  processing terminal exist
*              0<:  no. of terminal processing
*
** Description: 
*   This function checks whether any Satellites have started downloads after
*   receiving a parameter change Notice Board message and provides a count of
*   the number of Satellites that are still in the process of downloading.
*===========================================================================
*/
SHORT   SstChkBcasing(VOID)
{
	SHORT   sRetStatus = 0;   // default is no broadcast currently executing

    PifRequestSem(husSerIFSerCli);  // Get exclusive access to pSerBcas array.

    if (NULL != pSerBcas) {
		// If there is a pSerBcas array, set when starting a broadcast, then
		// we will iterate over the array to find out how many terminals are
		// downloading parameters and which ones have finished.
		//    uchBcasStatus == 0 then no download started or in process
		//    uchBcasStatus == 1 then download started and completed
		//    uchBcasStatus == 2 then download started and not yet completed
		USHORT  usStatusCount = 0;
		SHORT   sProc = 0;
		SHORT   sI;
		UCHAR   uchBcasStatus;

		sRetStatus = -1;    /* default is no terminals downloading, check if timer expired */

        for (sI = 0; sI < CLI_ALLTRANSNO; sI++) {
            uchBcasStatus = (pSerBcas + sI)->uchBcasStatus;
            usStatusCount += uchBcasStatus;     // increment indicator by status value (0, 1, 2)
            if (2 == uchBcasStatus) {
                ++ sProc;  // count up number of downloads still in process
            }
        }

		if (usStatusCount != 0)   // if any downloads started, completed or not, then usStatusCount is nonzero
			sRetStatus = sProc;   /* no. terminals still processing if any started downloads */
    }
    
    PifReleaseSem(husSerIFSerCli);

    return sRetStatus;
}

/*
*===========================================================================
** Synopsis:    VOID    SstResetCurTransNo(VOID);
*
** Return:      none.
*
** Description: This function resets all transaction numbers.
*===========================================================================
*/
VOID    SstResetCurTransNo(VOID)
{
    PifRequestSem(husSerIFSerCli);

    memset(Para.ausSerCurTransNo, 0, sizeof(Para.ausSerCurTransNo));       /* clear cur. */
    memcpy(Para.ausSerPreTransNo, Para.ausSerCurTransNo, sizeof(Para.ausSerCurTransNo));   /* save cur. to prev. */
    if (pPifSaveFarData != NULL) {      /* Saratoga */
        (*pPifSaveFarData)();                           
    } 

    PifReleaseSem(husSerIFSerCli);
}

/*
*===========================================================================
** Synopsis:    VOID    SstResetIndTransNo(UCHAR uchUniqueAddr);
*
** Return:      none.
*
** Description: This function resets transaction number of specified terminal.
*===========================================================================
*/
VOID    SstResetIndTransNo(UCHAR uchUniqueAddr)
{
	if (uchUniqueAddr > 0 && uchUniqueAddr <= CLI_ALLTRANSNO) {
		PifRequestSem(husSerIFSerCli);

		Para.ausSerCurTransNo[uchUniqueAddr - 1] = 0;
		Para.ausSerPreTransNo[uchUniqueAddr - 1] = 0;

		if (pPifSaveFarData != NULL) {              /* Saratoga */
			(*pPifSaveFarData)();                           
		} 

		PifReleaseSem(husSerIFSerCli);
	}
}

/*
*==========================================================================
**    Synopsis: SHORT   SerCreateTmpFile( USHORT usSize )
*                                              
*       Input:  USHORT  usSize  number of item
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*           Normal End: SERV_SUCCESS
*           Error End:  SERV_ILLEGAL                                                       
**  Description:
*           Create GCF/Total temporary file.
*==========================================================================
*/      
SHORT   SerCreateTmpFile( USHORT usSize )
{
    SHORT       sReturn;
    SHORT       hsFileHandle;
    ULONG       ulActMove;
    ULONG       ulFileSize;

    /* get semapho */
    PifRequestSem(husSerCreFileSem);

    /* check server status */
    if ((usSerStatus == SER_STMULRCV) || (usSerStatus == SER_STMULSND)) {
        PifReleaseSem(husSerCreFileSem);
		PifLog (MODULE_SER_LOG, LOG_EVENT_SER_TEMP_FILE_ERR_03);
		PifLog (MODULE_DATA_VALUE(MODULE_SER_LOG), usSerStatus);
        return(LDT_PROHBT_ADR);	/* avoid fault when satellite sends data at start up, 08/31/01 */
        /* return(SERV_ERROR); */
    }

    ulFileSize = TrnCalStoSize(usSize, FLEX_CONSSTORAGE_ADR) + 1024L;

    hsFileHandle = PifOpenFile(aszSerTmpFileName, auchTEMP_NEW_FILE_READ_WRITE);    /* saratoga */
    if (hsFileHandle == PIF_ERROR_FILE_EXIST) {
        PifCloseFile(hsSerTmpFile);
        PifDeleteFile(aszSerTmpFileName);
        hsFileHandle = PifOpenFile(aszSerTmpFileName, auchTEMP_NEW_FILE_READ_WRITE);    /* saratoga */
    }
	if (hsFileHandle < 0) {
		hsSerTmpFile = -1;
        PifReleaseSem(husSerCreFileSem);
		PifLog (MODULE_SER_LOG, LOG_EVENT_SER_TEMP_FILE_ERR_02);
		PifLog (MODULE_ERROR_NO(MODULE_SER_LOG), abs(hsFileHandle));
		return (SERV_ERROR);
	}
 
    sReturn = PifSeekFile(hsFileHandle, ulFileSize, &ulActMove);
                                                                
    if ((sReturn < 0) || (ulActMove != ulFileSize)) {       /* can not make file */
        PifCloseFile(hsFileHandle);                         /* Close file then delete it */
        PifDeleteFile(aszSerTmpFileName); 
		PifReleaseSem(husSerCreFileSem);
		PifLog (MODULE_SER_LOG, LOG_EVENT_SER_TEMP_FILE_ERR_04);
		PifLog (MODULE_ERROR_NO(MODULE_SER_LOG), abs(sReturn));
        return(SERV_ERROR);
    }

    /* save file handle and set the max size of the temp file */
    hsSerTmpFile = hsFileHandle;
    PifSeekFile(hsSerTmpFile, 0L, &ulActMove);
    PifWriteFile(hsSerTmpFile, ( USHORT *)&ulFileSize, sizeof(USHORT)); 
    PifReleaseSem(husSerCreFileSem);
    return(SERV_PERFORM);
}

/*
*==========================================================================
**    Synopsis: VOID    SerDeleteTmpFile( VOID )
*                                              
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*
**  Description:
*           Delete server temporary file file in case of loding loader B/D.
*==========================================================================
*/      
VOID    SerDeleteTmpFile( VOID )
{
    /* get semapho */
    PifRequestSem(husSerCreFileSem);

    /* check server status */
    if ((usSerStatus != SER_STMULRCV) && (usSerStatus != SER_STMULSND)) {
        if (hsSerTmpFile != -1) {
            PifCloseFile(hsSerTmpFile);
			hsSerTmpFile = -1;
        }
        PifDeleteFile(aszSerTmpFileName);
    }

    PifReleaseSem(husSerCreFileSem);
}

/*===== END OF SOURCE =====*/
