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
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
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
USHORT  usNbStack[NB_STACK];     /* Stack size            */
static VOID (THREADENTRY *pFunc)(VOID) = NbMain;    /* Add R3.0 */
static VOID (THREADENTRY *pFunc2)(VOID) = NbMainCE; /* V3.3  Notice Board Idle if no comms */

PifSemHandle  husNbBkupSem = PIF_SEM_INVALID_HANDLE;           /* semaphore for backup copy 02/06/2001 */
                                                                                
/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
UCHAR FARCONST  NBTHREDNAME[]="NOTICEBD";         /* NB Thred Name */

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
        PifBeginThread(pFunc2, &usNbStack[NB_STACK], sizeof(usNbStack), PRIO_NOTICEBD, NBTHREDNAME);
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

    PifBeginThread(pFunc, &usNbStack[NB_STACK], sizeof(usNbStack), PRIO_NOTICEBD, NBTHREDNAME );

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
SHORT  NbResetMessage(USHORT usOffset, UCHAR uchRstFlag)
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
SHORT  NbWriteInfFlag(USHORT usType, USHORT fsInfFlag)
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