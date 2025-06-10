/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : User interface Module                         
* Category    : Notice Board, NCR 2170 US Hospitality Application         
* Program Name: NBUIF.C
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               NbInit()          : Initialize NB Thread
*               NbWriteMessage()  : Write a user's para requests
*               NbResetMessage()  : Reset a user's para requests
*               NbReadAllMessage(): Read all control flag 
*               NbWriteInfFlag()  : Write Information flag 
*               NbStartAsMaster() : Start As Master 
*               NbStartOnline()   : Start online monitor 
*               NbStartInquiry()  : Set INQUIRY flag
*               NbWaitForPowerUp(): Wait, when power up
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-26-92:00.00.01:H.Yamaguchi: initial                                   
* Sep-29-92:00.00.01:H.Yamaguchi: Adds waits for power up until finishing NB.                                   
* May-11-93:00.00.01:H.Yamaguchi: Adds clear FMT/FBM at NbStartAsMaster.                                   
* Sep-07-93:00.00.01:H.Yamaguchi: Same As HPINT.
* Arr-24-95:02.05.04:M.Suzuki   : Add Request FDT R3.0                                    
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
#include <string.h>
#include <stdio.h>

#include <ecr.h>
#include <pif.h>
#include <log.h>
#include <nb.h>
#include "nbcom.h"
#include <appllog.h>
#include <regstrct.h>           /* Add R3.0 */
#include <transact.h>           /* Add R3.0 */
#include <fdt.h>                /* Add R3.0 */

/*
*===========================================================================
*   Internal Work Flag Declarations
*===========================================================================
*/
NBMESSAGE   NbSysFlag;          /* Retains sytem control information */

/*
*===========================================================================
*   Internal Work Area Declarations
*===========================================================================
*/
NBSENDMES   NbSndBuf;           /* Sends    buffer */
NBSENDMES   NbRcvBuf;           /* Receives buffer */

USHORT  usNbRcvFMT = 0;              /* Received a message from Master */
USHORT  usNbRcvFBM = 0;              /* Received a message from B-Master */
SHORT   sNbSavRes = 0;               /* Saves a previous error */
USHORT  usNbSndCnt = 0;              /* Controls a request same message counter */
USHORT  usNbPreDesc = 0;             /* Saves previous descriptor data */

PifSemHandle  husNbSemHand = PIF_SEM_INVALID_HANDLE;            /* Controls to read/write flags  */

PifSemHandle  husNbBkupSem = PIF_SEM_INVALID_HANDLE;           /* semaphore for backup copy 02/06/2001 */
                                                                                
/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
UCHAR CONST  NBTHREDNAME[]="NOTICEBD";         /* NB Thred Name */

/*
*===========================================================================
** Synopsis:    VOID NbInit(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Create Main Notice Board Thread
*===========================================================================
*/
VOID NbInit(VOID)
{
    VOID(THREADENTRY * pFunc)(VOID) = NbMain;    /* Add R3.0 */
    VOID(THREADENTRY * pFunc2)(VOID) = NbMainCE; /* V3.3  Notice Board Idle if no comms */
    SHORT     sRet, sStatus;
    USHORT  usTemp;

	memset(&NbSysFlag, 0, sizeof(NbSysFlag));
	memset(&NbSndBuf, 0, sizeof(NbSndBuf));           /* Sends    buffer */
	memset(&NbRcvBuf, 0, sizeof(NbRcvBuf));           /* Receives buffer */

    husNbSemHand = PifCreateSem(1);        /* Create Flag Semaphore */

    husNbBkupSem = PifCreateSem(1);        /* Add Semaphore 02/06/2001 */

    sRet = NbNetChkBoard(NB_MASTERUADD);             /* Check Comm. Board */
    sStatus = NbCheckCpmEpt();                       /* Chech CPM/EPT,  V3.3 */

    PifRequestSem(husNbSemHand);

    NbSysFlag.fsSystemInf |=  NB_MTUPTODATE ;        /* Set only Master up to date */
    NbSysFlag.fsSystemInf |=  NB_SETMTONLINE ;       /* Set only Master as on line */

    if (0 == (NbSysFlag.fsSystemInf & NB_WITHOUTBM)) { 
        NbSysFlag.fsSystemInf |=  NB_BMUPTODATE ;    /* Set both M & BM up to date */
        NbSysFlag.fsSystemInf |=  NB_SETBMONLINE ;   /* Set both M & BM as on line */
    }

    usTemp = NbSysFlag.fsSystemInf ;                 /* Copy to display descriptor */

    PifReleaseSem(husNbSemHand);

    /* --- No Com Board and PC I/F System, V3.3 --- */
    if (sRet == PIF_ERROR_NET_NOT_PROVIDED && sStatus == NB_SUCCESS) {
        PifBeginThread(pFunc2, NULL, 0, PRIO_NOTICEBD, NBTHREDNAME);
        return;
    }

    if ( sRet == PIF_ERROR_NET_NOT_PROVIDED ) { 
        sRet = NbCheckUAddr();

        if ( sRet != NB_MASTERUADD ) {               /* If Satellite, then execute */
            PifAbort(MODULE_NB_ABORT, FAULT_BAD_ENVIRONMENT);         /* Invalid Evviroment */
        }

        usTemp |= NB_DESCRIPTOR_OFF ;                /* Set MT online flag       */
        NbDescriptor(usTemp);                        /* Display off M descriptor */
        return;
    }    

    PifBeginThread(pFunc, NULL, 0, PRIO_NOTICEBD, NBTHREDNAME );

}

/*
*===========================================================================
** Synopsis:    SHORT NbResetMessage(USHORT usOffset, UCHAR uchRstFlag)
*     Input:    usOffset
*               uchRstFlag
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS
*
** Description: Resets a user's request message.
*===========================================================================
*/
#if defined(NbResetMessage)
SHORT   NbResetMessage_Special(USHORT usOffset, NBMESSAGE_T uchRstFlag);
SHORT   NbResetMessage_Debug(char* aszFilePath, int nLineNo, USHORT usOffset, NBMESSAGE_T uchRstFlag)
{
    NBMESSAGE_T  NbReqMesSave = NbSysFlag.auchMessage[usOffset];
    char  xBuffer[256];

    sprintf(xBuffer, "==NOTE NbResetMessage_Debug(): File %s  lineno=%d usOffset=%d uchRstFlag=0x%x NbReqMesSave=0x%x", RflPathChop(aszFilePath), nLineNo, usOffset, uchRstFlag, NbReqMesSave);
    NHPOS_NONASSERT_NOTE("==NOTE", xBuffer);
    return NbResetMessage_Special(usOffset, uchRstFlag);
}

SHORT   NbResetMessage_Special(USHORT usOffset, NBMESSAGE_T uchRstFlag)
#else
SHORT  NbResetMessage(USHORT usOffset, NBMESSAGE_T uchRstFlag)
#endif
{
    PifRequestSem(husNbSemHand);
 
    NbSysFlag.auchMessage[usOffset] &= (~ uchRstFlag);  /* Resets request parameter */

    PifReleaseSem(husNbSemHand);

    return (NB_SUCCESS);
}
 
/*
*===========================================================================
** Synopsis:    SHORT NbReadAllMessage(NBMESSAGE *pMes)
*     Input:    nothing
*     Output:   pMes
*     InOut:    nothing
*
** Return:      NB_SUCCESS
*
** Description: Read all information flag.
*===========================================================================
*/
SHORT  NbReadAllMessage(NBMESSAGE *pMes)      
{
    PifRequestSem(husNbSemHand);
    
    *pMes = NbSysFlag;       /* Copy System flag  */

    PifReleaseSem(husNbSemHand);

    return (NB_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    SHORT NbWriteInfFlag(USHORT usType, USHORT fsInfFlag)
*     Input:    usType
*               fsInfFlag
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS   Success
*               NB_ILLEGAL : Invalid parameter 
*
** Description: Write a designate Information Flag.
*===========================================================================
*/
#if defined(NbWriteInfFlag)
SHORT   NbWriteInfFlag_Special(USHORT usType, USHORT fsInfFlag);
SHORT   NbWriteInfFlag_Debug(char* aszFilePath, int nLineNo, USHORT usType, USHORT fsInfFlag)
{

    if (NB_SYSTEMFLAG == usType) {
        static USHORT fsInfFlagSave = -1;

        if (fsInfFlag != fsInfFlagSave) {
            char  xBuffer[256];

            sprintf(xBuffer, "==NOTE NbWriteInfFlag_Debug(): File %s  lineno=%d  usType=%d fsInfFlag=0x%x", RflPathChop(aszFilePath), nLineNo, usType, fsInfFlag);
            NHPOS_NONASSERT_NOTE("==NOTE", xBuffer);
            sprintf(xBuffer, "==NOTE                         NbSysFlag.fsSystemInf=0x%x ", NbSysFlag.fsSystemInf);
            NHPOS_NONASSERT_NOTE("==NOTE", xBuffer);
        }
    }

    return NbWriteInfFlag_Special(usType, fsInfFlag);
}

SHORT   NbWriteInfFlag_Special(USHORT usType, USHORT fsInfFlag)
#else
SHORT  NbWriteInfFlag(USHORT usType, USHORT fsInfFlag)
#endif
{
    SHORT   sRet = NB_SUCCESS;
    USHORT  usTemp;

    PifRequestSem(husNbSemHand);

    switch (usType) {

    case NB_SYSTEMFLAG :
        usTemp = fsInfFlag ;                     /* Copy to usTemp          */
        usTemp &= NB_RSTSYSFLAG ;                /* Reset other flag        */
        NbSysFlag.fsSystemInf &= ~NB_RSTSYSFLAG; /* Reset M & BM up to date flag  */
        NbSysFlag.fsSystemInf |= usTemp;         /* Write request parameter */
        usTemp = NbSysFlag.fsSystemInf;          /* Copy to display descriptor */
        PifReleaseSem(husNbSemHand);             /* Release semaphore          */
        NbDispWithoutBoard(usTemp);              /* Display desc. without a board */
        return (sRet);                           /* Return */

    case NB_MTBAKCOPY_FLAG :
        NbSysFlag.fsMBackupInf = fsInfFlag ; /* Write request parameter */
        break;

    case NB_BMBAKCOPY_FLAG :
        NbSysFlag.fsBMBackupInf = fsInfFlag ; /* Write request parameter */
        break;

    default:
        sRet = NB_ILLEGAL;
    }

    PifReleaseSem(husNbSemHand);
    return (sRet);
}


/*
*===========================================================================
** Synopsis:    SHORT NbStartAsMaster(USHORT fsFlag)
*     Input:    fsFlag
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS
*
** Description: Start as master.
*===========================================================================
*/
SHORT  NbStartAsMaster(USHORT fsFlag)
{
    USHORT    usTemp;

    PifRequestSem(husNbSemHand);

    NbSysFlag.fsSystemInf &= NB_RSTUPANDON ;   /* Reset system information flag */

    usTemp = fsFlag & NB_RSTSYSFLAG ;          /* Set M and BM up to date flag */

    usTemp |= (NB_STARTASMASTER | NB_SETMTONLINE | NB_SETBMONLINE) ; /* Set Startasmaster flag */

    NbSysFlag.fsSystemInf |= usTemp;           /* Save system information flag */

    usTemp = NbSysFlag.fsSystemInf ;           /* Save system information flag */

    usNbRcvFMT = 0;                            /* Clear FMT */
    usNbRcvFBM = 0;                            /* Clear FBM */

    /* --- clear network buffer (02/09/2001) --- */

    NbNetClear();

    PifReleaseSem(husNbSemHand);               /* Release semaphore            */

    NbDispWithoutBoard(usTemp);                /* Display desc. without a board */

    {
        char  xBuff[128];
        sprintf(xBuff, "==NOTE: NbStartAsMaster() fsFlag=0x%x  usTemp=0x%x", fsFlag, usTemp);
        NHPOS_NONASSERT_TEXT(xBuff);
    }

    return (NB_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT NbStartOnline(USHORT fsFlag)
*     Input:    fsFlag
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS
*
** Description: Start monitor.
*===========================================================================
*/
SHORT  NbStartOnline(USHORT fsFlag)
{
    USHORT  usTemp;

    PifRequestSem(husNbSemHand);

    NbSysFlag.fsSystemInf &= NB_RSTUPANDON ;   /* Reset system information flag */

    usTemp = fsFlag & NB_RSTSYSFLAG ;       /* Set system information flag */

    usTemp |= (NB_STARTONLINE | NB_SETMTONLINE | NB_SETBMONLINE); /* set Start monitor */ 

    NbSysFlag.fsSystemInf |= usTemp;         /* Save system information flag */

    usTemp = NbSysFlag.fsSystemInf ;           /* Save system information flag */

    /* --- clear network buffer (02/09/2001) --- */

    NbNetClear();

    PifReleaseSem(husNbSemHand);               /* Release semaphore            */

    NbDispWithoutBoard(usTemp);                /* Display desc. without a board */

    {
        char  xBuff[128];
        sprintf(xBuff, "==NOTE: NbStartOnline() fsFlag=0x%x  usTemp=0x%x", fsFlag, usTemp);
        NHPOS_NONASSERT_TEXT(xBuff);
    }

    return (NB_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT NbStartInquiry(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS
*
** Description: Set NB_INQUIRY flag.
*===========================================================================
*/
SHORT  NbStartInquiry(VOID)
{

    PifRequestSem(husNbSemHand);

    NbSysFlag.fsSystemInf |= NB_INQUIRY;       /* Set NB_INQUIRY flag */

    PifReleaseSem(husNbSemHand);               /* Release semaphore   */

    return (NB_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID NbWaitForPowerUp(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Wait for delay, when 2170 power up.
*===========================================================================
*/
VOID  NbWaitForPowerUp(VOID)
{
    SHORT  sRet;
    LONG   lTime;
    
    sRet = NbNetChkBoard(NB_BMASTERUADD);     /* Check Comm. Board */

    if ( sRet == PIF_ERROR_NET_NOT_PROVIDED ) { 
        return;
    }

    lTime = NbGetRcvTime();    /* get timer */

    PifSleep((USHORT)lTime);   /* Wait for Nb's Rcv Time */

    return;
}  

/*
*===========================================================================
** Synopsis:    VOID NbEnterBackupCopy(VOID)    Add 02/06/2001
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Enter Backup Copy Mode (Block normal NB thread)
*===========================================================================
*/
VOID  NbEnterBackupCopy(VOID)
{
    PifRequestSem(husNbBkupSem);
}

/*
*===========================================================================
** Synopsis:    VOID NbExitBackupCopy(VOID)     Add 02/06/2001
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Exit Backup Copy Mode (Allow normal NB thread)
*===========================================================================
*/
VOID  NbExitBackupCopy(VOID)
{
    PifReleaseSem(husNbBkupSem);
}
/*====== End of Source File ======*/