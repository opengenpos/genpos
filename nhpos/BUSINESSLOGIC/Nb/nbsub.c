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
* Title       : Notice Board Sub routine Module                         
* Category    : Notice Board, NCR 2170 US Hospitality Application         
* Program Name: NBSUB.C
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*          NbCheckOffline()      : Check offline
*          NbPreCheckOffline()   : Precheck offline
*          NbSetMTOnline()       : Set   Master online flag  
*          NbResetMTOnline()     : Reset Master online flag  
*          NbSetBMOnline()       : Set   B-Master online flag  
*          NbResetBMOnline()     : Reset B-Master online flag  
*          NbMakeSndMessage()    : Make up send a message  
*          NbCheckUAddr()        : Check terminal address 
*          NbExecRcvAsMMes()     : Execute a received as master message  
*          NbExecRcvMonMes()     : Execute a received online message  
*          NbSetFCtoSndBuf()     : Set function code to send buffer 
*          NbEndThread()         : Go to EndThread or Abort 
*          NbExecRcvSetOnline()  : Set online flag 
*          NbMakeConfirmMessage(): Make-up confirm or response message
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-26-92:00.00.01:H.Yamaguchi: initial                                   
* Apr-17-93:00.00.09:H.Yamaguchi: Adds retry Co. and change Timer control.
* May-11-93:00.00.01:H.Yamaguchi: Adds retry confirm BMaster.                                   
* Sep-07-93:00.00.01:H.Yamaguchi: Same As HPINT.
* Apr-24-95:02.05.04:M.Suzuki   : Add Message[1] for Flex. Drive Thru R3.0
* Jul-08-15:02.02.01:R.Chambers : reduce number ASSRTLOG logs at Amtrak due to PIF_ERROR_NET_UNREACHABLE
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
#include <csstbstb.h>
#include <plu.h>
#include <csstbfcc.h>
#include "paraequ.h"
#include "para.h"
#include "pararam.h"
#include <csserm.h>
#include <nb.h>
#include "nbcom.h"
#include <appllog.h>
#include <regstrct.h>           /* Add R3.0 */
#include <transact.h>           /* Add R3.0 */
#include <fdt.h>                /* Add R3.0 */
#include "display.h"
#include "cscas.h"
#include "csstbcas.h"
#include "ej.h"
#include <rfl.h>

/*
*===========================================================================
*   Internal Work Flag Declarations
*===========================================================================
*/

/*
*===========================================================================
*   Internal Work Area Declarations
*===========================================================================
*/

static UCHAR   auchNbReqMes[NB_MAX_MESSAGE] = { 0 };   /* Saves a user's request */
static USHORT  usNbSeqNO = 0;                          /* Uses Sequence Number control */

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    SHORT  NbCheckOffline(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS 
*
** Description: Check offline or not.  At Amtrak we have seen multiple network
*               errors of PIF_ERROR_NET_UNREACHABLE and PIF_ERROR_NET_DISCOVERY
*               probably due to their use of tablet with cellular LAN.  The
*               result is a cluttered ASSRTLOG log file with 20 minutes to 40 minutes
*               of these logs being generated once per second.
*===========================================================================
*/
#if defined NbCheckOffline
SHORT  NbCheckOffline_Special(SHORT sRet);
SHORT  NbCheckOffline_Debug(SHORT sRet, char *aszFilePath, int iLineNo)
{
	static  USHORT  usUnreachableCount = 0;
	static  ULONG   usDiscoveryCount = 0;
	static  ULONG   usTimeOutCount = 0;
	SHORT   sRetStatus;
	USHORT  usNbRcvFMTTemp = usNbRcvFMT;
	USHORT  usNbRcvFBMTemp = usNbRcvFBM;

	sRetStatus = NbCheckOffline_Special(sRet);
	if (sRet < 0 && (NbSysFlag.fsSystemInf  & NB_SETMTONLINE)) {
		int     bPrintLog = 0;     // 0 - no print, non-zero print with value indicating the trigger.
		int     iLen = 0;
		char    xBuffer[256];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		// if not a run of PIF_ERROR_NET_UNREACHABLE or PIF_ERROR_NET_DISCOVERY then reset otherwise
		// we will emit an PIF_ERROR_NET_UNREACHABLE or PIF_ERROR_NET_DISCOVERY log only every so many minutes.
		// this would normally be hit about once a second if continual PIF_ERROR_NET_UNREACHABLE or PIF_ERROR_NET_DISCOVERY.
		switch (sRet) {
			case PIF_ERROR_NET_UNREACHABLE:
				usUnreachableCount++;
				bPrintLog = (usUnreachableCount %= (20 * 60)) == 1 ? 1 : 0;  // usUnreachableCount rolls back to zero when incremented to 20 * 60.
				break;
			case PIF_ERROR_NET_DISCOVERY:
				usDiscoveryCount++;
				bPrintLog = (usDiscoveryCount %= (20 * 60)) == 1 ? 2 : 0;  // usDiscoveryCount rolls back to zero when incremented to 20 * 60.
				break;
			case PIF_ERROR_NET_TIMEOUT:
				usTimeOutCount++;
				bPrintLog = (usTimeOutCount %= (5 * 60)) == 1 ? 4 : 0;  // usTimeOutCount rolls back to zero when incremented to 5 * 60.
				break;
			default:
				bPrintLog = 7;    // indicate this is an error other than one of the special ones we are watching for.
				break;
		}

		if (bPrintLog) {
			sprintf(xBuffer, "NbCheckOffline(): sRet %d  bPrintLog %d usNbRcvFMT %d  usNbRcvFBM %d  counts = %u  %u  %u File %s  lineno = %d", sRet, bPrintLog, usNbRcvFMTTemp, usNbRcvFBMTemp, usUnreachableCount, usDiscoveryCount, usTimeOutCount, aszFilePath + iLen, iLineNo);
			NHPOS_NONASSERT_TEXT(xBuffer);
			if (bPrintLog == 7) {
				// if error other than PIF_ERROR_NET_UNREACHABLE, PIF_ERROR_NET_DISCOVERY, PIF_ERROR_NET_TIMEOUT
				// reset those special counts so that if the next error is one of those we will log it.
				// since this is a different kind of error reset the counts we watch so that if there is
				// one of these types of errors we will get a new log for that error type.
				usUnreachableCount = 0;
				usDiscoveryCount = 0;
				usTimeOutCount = 0;
			}
		}
	} else {
		usUnreachableCount = 0;   // if not a run of PIF_ERROR_NET_UNREACHABLE then reset
		usDiscoveryCount = 0;     // if not a run of PIF_ERROR_NET_DISCOVERY then reset
		usTimeOutCount = 0;       // if not a run of PIF_ERROR_NET_TIMEOUT then reset
	}
	return sRetStatus;
}
SHORT   NbCheckOffline_Special(SHORT sRet)
#else
SHORT  NbCheckOffline(SHORT sRet)
#endif
{
    LONG    lTime;
    USHORT  usCo, usMDC, usTmp;
    UCHAR   uchUadd;

    lTime = NbGetRcvTime();                  /* get timer */
    usMDC = NbGetMDCTime();                  /* Read MDC timer value */

    usCo = NB_MAXLIMIT + usMDC/16;           /* Calculate retry counter */

    if ( lTime < NB_TMDEFAULT ) {
        /* Calculate retry count */
        usTmp = (USHORT)( (( (usMDC & NB_MASK_MDC_TIME) + NB_TMDEFAULT_SEC)*usCo )/( NB_TMPOWERUP ) ) ; 
        usCo = usTmp + NB_DEFAULT_TIMES ;      /* Adds Default retry counter */
    } 

    uchUadd = CliReadUAddr();           /* Get unique address */

    if ( usNbRcvFMT > usCo ) {
        usNbRcvFMT = 0;                     /* Clear count  */
		if (uchUadd ==  NB_BMASTERUADD) {
			NbStartAsMaster(NB_BMUPTODATE | NB_MTUPTODATE);     /* Start As Master */
		}

		if (uchUadd != NB_MASTERUADD) {
			NbResetMTOnline();                  /* Reset Master online flag */
		}
    }

    if ( usNbRcvFBM > usCo ) {
        usNbRcvFBM = 0;                     /* Clear count  */
        if (uchUadd ==  NB_MASTERUADD) {
			NbStartAsMaster(NB_MTUPTODATE | NB_BMUPTODATE);     /* Start As Master */
		}

		if (uchUadd != NB_BMASTERUADD) {
			NbResetBMOnline();                  /* Reset B-Master online flag */
		}
    }

    return(NB_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT  NbPreCheckOffline(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS 
*
** Description: Check offline or not  
*===========================================================================
*/
#if defined NbPreCheckOffline
SHORT  NbPreCheckOffline_Special(VOID);
SHORT  NbPreCheckOffline_Debug(char *aszFilePath, int iLineNo)
{
	SHORT   sRetStatus;
	USHORT  usNbRcvFMTTemp = usNbRcvFMT;
	USHORT  usNbRcvFBMTemp = usNbRcvFBM;

	sRetStatus = NbPreCheckOffline_Special();
	if (usNbRcvFMT > 3 && (NbSysFlag.fsSystemInf  & NB_SETMTONLINE)) {
		int     iLen = 0;
		char    xBuffer[256];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		sprintf(xBuffer, "NbPreCheckOffline(): usNbRcvFMT %d  usNbRcvFBM %d  File %s  lineno = %d", usNbRcvFMTTemp, usNbRcvFBMTemp, aszFilePath + iLen, iLineNo);
		NHPOS_NONASSERT_TEXT(xBuffer);
	}
	return sRetStatus;
}
SHORT   NbPreCheckOffline_Special()
#else
SHORT  NbPreCheckOffline(VOID)
#endif
{
    LONG    lTime;
    USHORT  usCo, usMDC, usTmp;

    lTime = NbGetRcvTime();                  /* get timer */
    usMDC = NbGetMDCTime();                  /* Read MDC timer value */

    usCo = NB_MAXLIMIT + usMDC/16;

    if ( lTime < NB_TMDEFAULT ) {
        /* Calculate retry count */
        usTmp = (USHORT)((((usMDC & NB_MASK_MDC_TIME) + NB_TMDEFAULT_SEC)*usCo )/(NB_TMPOWERUP)) ; 
        usCo = usTmp + NB_DEFAULT_TIMES ;      /* Adds Default retry counter */
    } 

    if ( usNbRcvFMT >= usCo ) {
        NbResetMTOnline();                  /* Reset Master online flag */
        usNbRcvFMT = 0;                     /* Clear count  */
    }

    if ( usNbRcvFBM >= usCo ) {
        NbResetBMOnline();                  /* Reset B-Master online flag */
        usNbRcvFBM = 0;                     /* Clear count  */
    }

    return(NB_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID NbSetMTOnline()
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Set Master Online flag.
*===========================================================================
*/
#if defined NbSetMTOnline
USHORT  NbSetMTOnline_Special(VOID);
USHORT  NbSetMTOnline_Debug(char *aszFilePath, int iLineNo)
{
	USHORT  usOldMask;
	usOldMask = NbSetMTOnline_Special();
	if ((usOldMask & NB_SETMTONLINE) == 0) {
		int     iLen = 0;
		char    xBuffer[256];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		sprintf(xBuffer, "NbSetMTOnline(): usOldMask 0x%x  File %s  lineno = %d", usOldMask, aszFilePath + iLen, iLineNo);
		NHPOS_NONASSERT_TEXT(xBuffer);
	}
	return usOldMask;
}
USHORT   NbSetMTOnline_Special(VOID)
#else
USHORT  NbSetMTOnline(VOID)
#endif
{
	USHORT  fsSystemInf;

    PifRequestSem(husNbSemHand);
	fsSystemInf = NbSysFlag.fsSystemInf;
    NbSysFlag.fsSystemInf |= NB_SETMTONLINE;  /* Set M online */
    PifReleaseSem(husNbSemHand);

	if ((fsSystemInf & NB_SETMTONLINE) == 0) {
		char  xBuff[128];
		sprintf (xBuff, "==STATUS: NbSetMTOnline() NB_SETMTONLINE set  fsSystemInf 0x%x", fsSystemInf);
		NHPOS_NONASSERT_TEXT(xBuff);
		// if it was Not On Line and now being set then log the status change
		PifLog(MODULE_NB_LOG, LOG_EVENT_NB_MTONLINE_SET);
		PifLog(MODULE_DATA_VALUE(MODULE_NB_LOG), fsSystemInf);
	}

	return fsSystemInf;
}

#if defined NbSetMTUpToDate
USHORT  NbSetMTUpToDate_Special(VOID);
USHORT  NbSetMTUpToDate_Debug(char *aszFilePath, int iLineNo)
{
	USHORT  usOldMask;
	usOldMask = NbSetMTUpToDate_Special();
	if ((usOldMask & NB_MTUPTODATE) == 0) {
		int     iLen = 0;
		char    xBuffer[256];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		sprintf(xBuffer, "NbSetMTUpToDate(): usOldMask 0x%x  File %s  lineno = %d", usOldMask, aszFilePath + iLen, iLineNo);
		NHPOS_NONASSERT_TEXT(xBuffer);
	}
	return usOldMask;
}
USHORT   NbSetMTUpToDate_Special(VOID)
#else
USHORT  NbSetMTUpToDate (VOID)
#endif
{
	USHORT  fsSystemInf;

    PifRequestSem(husNbSemHand);
	fsSystemInf = NbSysFlag.fsSystemInf;
    NbSysFlag.fsSystemInf |= NB_MTUPTODATE;  /* Set M up to date */
    PifReleaseSem(husNbSemHand);

	if ((fsSystemInf & NB_MTUPTODATE) == 0) {
		char  xBuff[128];
		sprintf (xBuff, "==STATUS: NbSetMTUpToDate() NB_MTUPTODATE set  fsSystemInf 0x%x", fsSystemInf);
		NHPOS_NONASSERT_TEXT(xBuff);
		// if it was Not Up To Date and now being set then log the status change
		PifLog(MODULE_NB_LOG, LOG_EVENT_NB_MTUPTODATE_SET);
		PifLog(MODULE_DATA_VALUE(MODULE_NB_LOG), fsSystemInf);
	}

	return fsSystemInf;
}
/*
*===========================================================================
** Synopsis:    VOID NbResetMTOnline()
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Reset Master Online flag.
*===========================================================================
*/
#if defined NbResetMTOnline
USHORT  NbResetMTOnline_Special(VOID);
USHORT  NbResetMTOnline_Debug(char *aszFilePath, int iLineNo)
{
	USHORT  usOldMask;
	UCHAR   uchUadd = CliReadUAddr();

	usOldMask = NbResetMTOnline_Special();
	if (uchUadd != NB_MASTERUADD && (usOldMask & NB_SETMTONLINE)) {
		int     iLen = 0;
		char    xBuffer[256];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		sprintf(xBuffer, "NbResetMTOnline(): usOldMask 0x%x  File %s  lineno = %d", usOldMask, aszFilePath + iLen, iLineNo);
		NHPOS_NONASSERT_TEXT(xBuffer);
	}
	return usOldMask;
}
USHORT   NbResetMTOnline_Special(VOID)
#else
USHORT  NbResetMTOnline(VOID)
#endif
{
	USHORT  fsSystemInf;
	UCHAR   uchUadd = CliReadUAddr();

    PifRequestSem(husNbSemHand);
		fsSystemInf = NbSysFlag.fsSystemInf;
		if ( uchUadd != NB_MASTERUADD ) {               /* If not Master, then execute */
			NbSysFlag.fsSystemInf &= ~NB_SETMTONLINE ;  /* Reset M online */
		}
    PifReleaseSem(husNbSemHand);

	if (uchUadd != NB_MASTERUADD && (fsSystemInf & NB_SETMTONLINE)) {
		char  xBuff[128];
		sprintf (xBuff, "==WARNING: NbResetMTOnline() NB_SETMTONLINE cleared  fsSystemInf 0x%x", fsSystemInf);
		NHPOS_NONASSERT_TEXT(xBuff);
		// if it was On Line and now being reset then log the status change
		PifLog(MODULE_NB_LOG, LOG_EVENT_NB_MTONLINE_RESET);
		PifLog(MODULE_DATA_VALUE(MODULE_NB_LOG), fsSystemInf);
	}

	return fsSystemInf;
}

#if defined NbResetMTUpToDate
USHORT  NbResetMTUpToDate_Special(VOID);
USHORT  NbResetMTUpToDate_Debug(char *aszFilePath, int iLineNo)
{
	USHORT  usOldMask;
	usOldMask = NbResetMTUpToDate_Special();
	if (usOldMask & NB_MTUPTODATE) {
		int     iLen = 0;
		char    xBuffer[256];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		sprintf(xBuffer, "NbResetMTUpToDate(): usOldMask 0x%x  File %s  lineno = %d", usOldMask, aszFilePath + iLen, iLineNo);
		NHPOS_NONASSERT_TEXT(xBuffer);
	}
	return usOldMask;
}
USHORT   NbResetMTUpToDate_Special(VOID)
#else
USHORT  NbResetMTUpToDate(VOID)
#endif
{
	USHORT  fsSystemInf;

    PifRequestSem(husNbSemHand);
	fsSystemInf = NbSysFlag.fsSystemInf;
    NbSysFlag.fsSystemInf &= ~NB_MTUPTODATE ;  /* Reset M up to date */
    PifReleaseSem(husNbSemHand);

	if (fsSystemInf & NB_MTUPTODATE) {
		char  xBuff[128];
		sprintf (xBuff, "==WARNING: NbResetMTUpToDate() NB_MTUPTODATE cleared  fsSystemInf 0x%x", fsSystemInf);
		NHPOS_NONASSERT_TEXT(xBuff);
		// if it was Up To Date and now being reset then log the status change
		PifLog(MODULE_NB_LOG, LOG_EVENT_NB_MTUPTODATE_RESET);
		PifLog(MODULE_DATA_VALUE(MODULE_NB_LOG), fsSystemInf);
	}

	return fsSystemInf;
}

/*
*===========================================================================
** Synopsis:    VOID NbSetBMOnline()
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Set B-Master Online flag.
*===========================================================================
*/
#if defined NbSetBMOnline
USHORT  NbSetBMOnline_Special(VOID);
USHORT  NbSetBMOnline_Debug(char *aszFilePath, int iLineNo)
{
	USHORT  usOldMask;
	usOldMask = NbSetBMOnline_Special();
	if ((usOldMask & NB_SETBMONLINE) == 0) {
		int     iLen = 0;
		char    xBuffer[256];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		sprintf(xBuffer, "NbSetBMOnline(): usOldMask 0x%x  File %s  lineno = %d", usOldMask, aszFilePath + iLen, iLineNo);
		NHPOS_NONASSERT_TEXT(xBuffer);
	}
	return usOldMask;
}
USHORT   NbSetBMOnline_Special(VOID)
#else
USHORT  NbSetBMOnline(VOID)
#endif
{
	USHORT  fsSystemInf;

    PifRequestSem(husNbSemHand);
	fsSystemInf = NbSysFlag.fsSystemInf;
    NbSysFlag.fsSystemInf |= NB_SETBMONLINE;  /* Set M online */
    PifReleaseSem(husNbSemHand);

	if ((fsSystemInf & NB_SETBMONLINE) == 0) {
		char  xBuff[128];
		sprintf (xBuff, "==STATUS: NbSetBMOnline() NB_SETBMONLINE set  fsSystemInf 0x%x", fsSystemInf);
		NHPOS_NONASSERT_TEXT(xBuff);
		// if it was Not On Line and now being set then log the status change
		PifLog(MODULE_NB_LOG, LOG_EVENT_NB_BMONLINE_SET);
		PifLog(MODULE_DATA_VALUE(MODULE_NB_LOG), fsSystemInf);
	}

	return fsSystemInf;
}

#if defined NbSetBMUpToDate
USHORT  NbSetBMUpToDate_Special(VOID);
USHORT  NbSetBMUpToDate_Debug(char *aszFilePath, int iLineNo)
{
	USHORT  usOldMask;
	usOldMask = NbSetBMUpToDate_Special();
	if ((usOldMask & NB_BMUPTODATE) == 0) {
		int     iLen = 0;
		char    xBuffer[256];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		sprintf(xBuffer, "NbSetBMUpToDate(): usOldMask 0x%x  File %s  lineno = %d", usOldMask, aszFilePath + iLen, iLineNo);
		NHPOS_NONASSERT_TEXT(xBuffer);
	}
	return usOldMask;
}
USHORT   NbSetBMUpToDate_Special(VOID)
#else
USHORT  NbSetBMUpToDate(VOID)
#endif
{
	USHORT  fsSystemInf;

    PifRequestSem(husNbSemHand);
	fsSystemInf = NbSysFlag.fsSystemInf;
    NbSysFlag.fsSystemInf |= NB_BMUPTODATE;  /* Reset B-M up to date */
    PifReleaseSem(husNbSemHand);

	if ((fsSystemInf & NB_BMUPTODATE) == 0) {
		char  xBuff[128];
		sprintf (xBuff, "==STATUS: NbSetBMUpToDate() NB_BMUPTODATE set  fsSystemInf 0x%x", fsSystemInf);
		NHPOS_NONASSERT_TEXT(xBuff);
		// if it was Not Up To Date and now being set then log the status change
		PifLog(MODULE_NB_LOG, LOG_EVENT_NB_BMUPTODATE_SET);
		PifLog(MODULE_DATA_VALUE(MODULE_NB_LOG), fsSystemInf);
	}

	return fsSystemInf;
}

/*
*===========================================================================
** Synopsis:    VOID NbResetBMOnline()
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Reset B-Master Online flag.
*===========================================================================
*/
#if defined NbResetBMOnline
USHORT  NbResetBMOnline_Special(VOID);
USHORT  NbResetBMOnline_Debug(char *aszFilePath, int iLineNo)
{
	USHORT  usOldMask;
	usOldMask = NbResetBMOnline_Special();
	if (usOldMask & NB_SETBMONLINE) {
		int     iLen = 0;
		char    xBuffer[256];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		sprintf(xBuffer, "NbResetBMOnline(): usOldMask 0x%x  File %s  lineno = %d", usOldMask, aszFilePath + iLen, iLineNo);
		NHPOS_NONASSERT_TEXT(xBuffer);
	}
	return usOldMask;
}
USHORT   NbResetBMOnline_Special(VOID)
#else
USHORT  NbResetBMOnline(VOID)
#endif
{
	USHORT  fsSystemInf;
	UCHAR   uchUadd = CliReadUAddr();

    PifRequestSem(husNbSemHand);
		fsSystemInf = NbSysFlag.fsSystemInf;
		if ( uchUadd != NB_BMASTERUADD ) {               /* If not Master, then execute */
			NbSysFlag.fsSystemInf &= ~NB_SETBMONLINE ;  /* Reset B-M online */
		}
    PifReleaseSem(husNbSemHand);

	if (fsSystemInf & NB_SETBMONLINE) {
		NHPOS_NONASSERT_TEXT("==STATUS: NbResetBMOnline() NB_SETBMONLINE cleared.");
		// if it was On Line and now being reset then log the status change
		PifLog(MODULE_NB_LOG, LOG_EVENT_NB_BMONLINE_RESET);
		PifLog(MODULE_DATA_VALUE(MODULE_NB_LOG), fsSystemInf);
	}

	return fsSystemInf;
}

#if defined NbResetBMUpToDate
USHORT  NbResetBMUpToDate_Special(VOID);
USHORT  NbResetBMUpToDate_Debug(char *aszFilePath, int iLineNo)
{
	USHORT  usOldMask;
	usOldMask = NbResetBMUpToDate_Special();
	if (usOldMask & NB_BMUPTODATE) {
		int     iLen = 0;
		char    xBuffer[256];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		sprintf(xBuffer, "NbResetBMUpToDate(): 	usOldMask 0x%x  File %s  lineno = %d",usOldMask, aszFilePath + iLen, iLineNo);
		NHPOS_NONASSERT_TEXT(xBuffer);
	}
	return usOldMask;
}
USHORT   NbResetBMUpToDate_Special(VOID)
#else
USHORT  NbResetBMUpToDate(VOID)
#endif
{
	USHORT  fsSystemInf;

    PifRequestSem(husNbSemHand);
	fsSystemInf = NbSysFlag.fsSystemInf;
    NbSysFlag.fsSystemInf &= ~NB_BMUPTODATE;  /* Reset B-M up to date */
    PifReleaseSem(husNbSemHand);

	if (fsSystemInf & NB_BMUPTODATE) {
		NHPOS_NONASSERT_TEXT("==STATUS: NbResetBMUpToDate() NB_BMUPTODATE cleared.");
		// if it was Up To Date and now being reset then log the status change
		PifLog(MODULE_NB_LOG, LOG_EVENT_NB_BMUPTODATE_RESET);
		PifLog(MODULE_DATA_VALUE(MODULE_NB_LOG), fsSystemInf);
	}

	return fsSystemInf;
}

USHORT  NbSetMTBM_Online (UCHAR uchFadr)
{
	USHORT  fsSystemInf;

    PifRequestSem(husNbSemHand);
		fsSystemInf = NbSysFlag.fsSystemInf;
		if (uchFadr == NB_MASTERUADD)
			NbSysFlag.fsSystemInf |= NB_SETMTONLINE;  /* Set M on line */
		else if (uchFadr == NB_BMASTERUADD)
			NbSysFlag.fsSystemInf |= NB_SETBMONLINE;  /* Set BM on line */
    PifReleaseSem(husNbSemHand);

	if ((fsSystemInf & NB_SETMTONLINE) == 0 && uchFadr == NB_MASTERUADD) {
		NHPOS_NONASSERT_TEXT("==STATUS: NbSetMTBM_Online(): NB_MASTERUADD");
		// if it was Not Online and now being set then log the status change
		PifLog(MODULE_NB_LOG, LOG_EVENT_NB_MTONLINE_SET);
		PifLog(MODULE_DATA_VALUE(MODULE_NB_LOG), fsSystemInf);
	}
	if ((fsSystemInf & NB_SETBMONLINE) == 0 && uchFadr == NB_BMASTERUADD) {
		NHPOS_NONASSERT_TEXT("==STATUS: NbSetMTBM_Online(): NB_BMASTERUADD");
		// if it was Not Online and now being set then log the status change
		PifLog(MODULE_NB_LOG, LOG_EVENT_NB_BMONLINE_SET);
		PifLog(MODULE_DATA_VALUE(MODULE_NB_LOG), fsSystemInf);
	}

	return fsSystemInf;
}
/*
*===========================================================================
** Synopsis:    SHORT NbWriteMessage(USHORT usOffset, UCHAR uchReqFlag)
*     Input:    usOffset
*               uchReqFlag
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS
*
** Description: Writs a user's request message.
*===========================================================================
*/
SHORT  NbWriteMessage(USHORT usOffset, UCHAR uchReqFlag)
{
    PifRequestSem(husNbSemHand);
 
    auchNbReqMes[usOffset] |= uchReqFlag ; /* Write request parameter */

    PifReleaseSem(husNbSemHand);

    /********************** Add R3.0 ***********************************/
    if (usOffset == NB_MESOFFSET1) { 
		SYSCONFIG CONST *pSys = PifSysConfig();

        if (((0 == pSys->auchLaddr[0]) &&
             (0 == pSys->auchLaddr[1]) &&
             (0 == pSys->auchLaddr[2])) ||    /* comm. board not provide */
             (0 == pSys->auchLaddr[3])) {      /* U.A is 0 */

            FDTNoticeMessage((UCHAR)(auchNbReqMes[NB_MESOFFSET1] & NB_RSTALLFDT));
        }
    }
    /********************** End Add R3.0 *******************************/

    return (NB_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    VOID NbMakeSndMessage()
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Make up a send message.
*===========================================================================
*/
VOID  NbMakeSndMessage(VOID)                 /* Make a message to send */
{
    UCHAR uchUadd, uchFdtFlag = 0;
	static  USHORT  fsSystemInfSnapshot = 0;

	if (fsSystemInfSnapshot != NbSysFlag.fsSystemInf) {
		char  xBuff[128];

		sprintf (xBuff, "NbMakeSndMessage(): snap 0x%x  current 0x%x", fsSystemInfSnapshot, NbSysFlag.fsSystemInf);
		NHPOS_NONASSERT_TEXT(xBuff);
		fsSystemInfSnapshot = NbSysFlag.fsSystemInf;
	}

	// Obtain our semaphore used to synchronize access to the
	// Notice Board global variables.  We will be making changes
	// to NbSysFlag to communicate some status info, specifically
	// that the Master or BMaster is online.
	// We also increment the send count and the sequence number
	// and a couple of other globals that are gated with the husNbSemHand
	// semaphore.
    PifRequestSem(husNbSemHand);

    uchUadd = CliReadUAddr();

    if ( uchUadd == NB_MASTERUADD ) {               /* If Master, then execute */
        NbSndBuf.NbConMes.fsBackupInf = NbSysFlag.fsMBackupInf;
        NbSysFlag.fsSystemInf |= NB_SETMTONLINE ;  /* Set M online */
        usNbRcvFMT = 0;
    } else {                                        /* If B-Master, then execute */
        NbSndBuf.NbConMes.fsBackupInf = NbSysFlag.fsBMBackupInf;
        NbSysFlag.fsSystemInf |= NB_SETBMONLINE;  /* Set BM online */
        usNbRcvFBM = 0;
    }

    if (NbSndBuf.NbConMes.usNbFunCode != NB_FCSTARTASMASTER ) {  /* Starts as master ? */
        PifReleaseSem(husNbSemHand);                            /* if not, then return */
        return;
    }

    NbSndBuf.NbConMes.fsSystemInf = NbSysFlag.fsSystemInf;      /* set SystemInf */

    if ( NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET0] & NB_REQRESETCONSEC ) {
        if ( usNbSndCnt >= NB_MAXLIMIT ) {
            NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET0] &= ~NB_REQRESETCONSEC;
        } else {
            usNbSndCnt ++;                              /* Increment request same message counter */
            PifReleaseSem(husNbSemHand);
            NbSndBuf.NbConMes.usSeqNO = usNbSeqNO;   /* Set Sequence No. */
            return;
        }
    } 

    if ( auchNbReqMes[NB_MESOFFSET0] || auchNbReqMes[NB_MESOFFSET1] ||
			auchNbReqMes[NB_MESOFFSET2] || auchNbReqMes[NB_MESOFFSET3]) {
        NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET0] |= auchNbReqMes[NB_MESOFFSET0];
        NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET1] |= auchNbReqMes[NB_MESOFFSET1];    /* Add R3.0 */
		NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET2] |= auchNbReqMes[NB_MESOFFSET2];    /* Add R3.0 */
		NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET3] |= auchNbReqMes[NB_MESOFFSET3];    /* Add R3.0 */
        auchNbReqMes[NB_MESOFFSET0] = 0;
        uchFdtFlag = (UCHAR)(auchNbReqMes[NB_MESOFFSET1] & NB_RSTALLFDT); /* Add R3.0 */
        auchNbReqMes[NB_MESOFFSET1] = 0;                        /* Add R3.0 */
		auchNbReqMes[NB_MESOFFSET2] = 0;
		auchNbReqMes[NB_MESOFFSET3] = 0;
        usNbSndCnt = 1;
        usNbSeqNO ++;
    } else {
        usNbSndCnt ++;                              /* Increment request same message counter */
		//We do not want to send the reset DB flag as often as we send other messages
		//so we only send it half the time
		if((usNbSndCnt > (NB_SENDMAXTIMES / 2)) &&
			(NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET3] & NB_REQSETDBFLAG))
		{
			NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET3] &= ~NB_REQSETDBFLAG;
		}

		if ( usNbSndCnt > 3 )  {
			// following clears specific flags which should only be sent a few times.
			// we want to make sure that these indicators are cleared in a timely manner to prevent Satellite
			// Terminals from pulling a single instance of database changes multiple times.
			// see thread entry function UicMain() for the processing of Notice Board messages.
            NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET0] &= ~NB_REQ_OPERATOR_MSG;   /* Clear flag */
            NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET0] &= ~(NB_REQPLU | NB_REQSUPER | NB_REQALLPARA | NB_REQLAYOUT);    /* Clear flag */
		}

        if ( usNbSndCnt > NB_SENDMAXTIMES )  {
            NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET0] = 0;   /* Clear current data */
            NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET1] = 0;   /* Add R3.0 */
			NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET2] = 0;   /* Add R3.0 */
			NbSndBuf.NbConMes.auchMessage[NB_MESOFFSET3] = 0;   /* Add R3.0 */
            usNbSndCnt = 1;        
        }
    }

	// All done, so lets release the semaphore for our globals.
    PifReleaseSem(husNbSemHand);

	// Now we notify the Flexible Drive Thru that there is something
	// for it to do if there is indeed something to do.  FDTNoticeMessage()
	// triggers FDTMain () to start through its loop body by releasing a semaphore.

    if (uchFdtFlag) {   /* Add R3.0 */
        /* Request to FDT in self */
        FDTNoticeMessage(uchFdtFlag);
    }
    NbSndBuf.NbConMes.usSeqNO = usNbSeqNO;          /* Set Sequence No. */
}

/*
*===========================================================================
** Synopsis:    USHORT NbCheckUAddr()
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Terminal address  or NB_SUCCESS
*
** Description: Check terminal is Master or B-Master.
*===========================================================================
*/
USHORT  NbCheckUAddr(VOID)                 /* Check terminal address */
{
    //SYSCONFIG CONST FAR * pSys;
    UCHAR   uchUadd;
    USHORT  usTadd;

    uchUadd = CliReadUAddr();           /* Get unique address */

    usTadd = (USHORT)uchUadd;

	if (CstIamDisconnectedSatellite() == STUB_SUCCESS) {
		return NB_SUCCESS;
	}

    if ( usTadd > NB_BMASTERUADD ) {
        return(NB_SUCCESS);             /* Return without a unique address */
    }
    
    if ( usTadd == NB_BMASTERUADD ) {
        if ((NbSysFlag.fsSystemInf & NB_WITHOUTBM) != 0) {    /* saratoga */
            return(NB_SUCCESS);        /* if not b-master , then this terinal is satellite */      
        }
    } 
    return(usTadd);                    /* Rerurn with a unique address */
}

/*
*===========================================================================
** Synopsis:    VOID NbExecRcvAsMMes()
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Execute a received as master message.
*===========================================================================
*/
VOID  NbExecRcvAsMMes(VOID)            /* Execute a received message */
{
    SHORT   sRet;
    USHORT  fsSelfInf;

    sRet = NbCheckUAddr();
    
	// Obtain the semaphore for the Notice Board globals.
	// We get this here since NbExecRcvSetOnline modifies NbSysFlag.
    PifRequestSem(husNbSemHand);

    if ( ( sRet != NB_SUCCESS ) && ( 0 == NbSndBuf.NbConMes.usNbFunCode ) ) {  
        NbExecRcvSetOnline();         /* Set online flag */
        PifReleaseSem(husNbSemHand);
        return;
    }
    
    NbRcvBuf.NbConMes.fsSystemInf &= NB_RSTSYSFLAG ;         /* Reset system flag */
    fsSelfInf = NbSysFlag.fsSystemInf;                       /* Save previous flag */

    NbSysFlag.fsSystemInf &= ~(NB_MTUPTODATE | NB_BMUPTODATE );  /* Reset M and B-M up to date flag */

    NbSysFlag.fsSystemInf |= NbRcvBuf.NbConMes.fsSystemInf;  /* Set M/B-M up to date */

	if (fsSelfInf != NbSysFlag.fsSystemInf) {
		char xBuff[128];
		sprintf (xBuff, "==STATUS: NbExecRcvAsMMes() fsSelfInf 0x%x NbSysFlag.fsSystemInf 0x%x", fsSelfInf, NbSysFlag.fsSystemInf);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	if ( NbRcvBuf.NbConMes.usSeqNO != usNbSeqNO ) {
        NbSysFlag.auchMessage[NB_MESOFFSET0] |= NbRcvBuf.NbConMes.auchMessage[NB_MESOFFSET0];  /* Set req. para bit */
        NbSysFlag.auchMessage[NB_MESOFFSET1] |= NbRcvBuf.NbConMes.auchMessage[NB_MESOFFSET1];  /* Set req. FDT bit */
		NbSysFlag.auchMessage[NB_MESOFFSET2] |= NbRcvBuf.NbConMes.auchMessage[NB_MESOFFSET2];  /* Set req. FDT bit */
		NbSysFlag.auchMessage[NB_MESOFFSET3] |= NbRcvBuf.NbConMes.auchMessage[NB_MESOFFSET3];  /* Set req. FDT bit */

        usNbSeqNO = NbRcvBuf.NbConMes.usSeqNO;
    }

    if ( NbRcvBuf.NbMesHed.auchFaddr[3] == NB_MASTERUADD) { /* If Master, then execute */
		// If we have received a Notice Board message from the Master Terminal then lets compare check sums.
		// If there is a difference then emit an ASSRTLOG log indicating that there is a difference in the
		// check sum we have for the Master Terminal and what the Master Terminal tells us it calculates.
		if (NbSysFlag.ulMT_ParaHashCurrent != NbRcvBuf.NbConMes.ulParaHashCurrent) {
			CHAR  xBuff[128];
			sprintf (xBuff, "==STATUS:  NbSysFlag.ulMT_ParaHashCurrent (%u) != NbRcvBuf.NbConMes.ulParaHashCurrent (%u)", NbSysFlag.ulMT_ParaHashCurrent, NbRcvBuf.NbConMes.ulParaHashCurrent);
			NHPOS_NONASSERT_TEXT(xBuff);
			NbSysFlag.ulMT_ParaHashCurrent = NbRcvBuf.NbConMes.ulParaHashCurrent;
		}
	} else if ( NbRcvBuf.NbMesHed.auchFaddr[3] == NB_BMASTERUADD ) {  /* If Backup Master, then execute */
		// Perform the same checks on the check sums for a message from the Backup Master Terminal as we do for
		// a Master Terminal.
		if (NbSysFlag.ulBU_ParaHashCurrent != NbRcvBuf.NbConMes.ulParaHashCurrent) {
			CHAR  xBuff[128];
			sprintf (xBuff, "==STATUS:  NbSysFlag.ulBU_ParaHashCurrent (%u) != NbRcvBuf.NbConMes.ulParaHashCurrent (%u)", NbSysFlag.ulBU_ParaHashCurrent, NbRcvBuf.NbConMes.ulParaHashCurrent);
			NHPOS_NONASSERT_TEXT(xBuff);
			NbSysFlag.ulBU_ParaHashCurrent = NbRcvBuf.NbConMes.ulParaHashCurrent;
		}
	}

    if ( NbRcvBuf.NbMesHed.auchFaddr[3] == NB_MASTERUADD ) {         /* If Master, then execute */
        if (sRet != NB_MASTERUADD) {                                /* Do not overwrite self status */
            NbSysFlag.fsMBackupInf = NbRcvBuf.NbConMes.fsBackupInf ;     /* Copy Master backup information flag */
        }

        if ( sRet == NB_BMASTERUADD ) {
            if ( (NB_MTUPTODATE | NB_BMUPTODATE) == ( fsSelfInf & NB_RSTSYSFLAG ) )  {   
                if ( NbRcvBuf.NbConMes.fsSystemInf == NB_MTUPTODATE ) {
                    SstChangeInqStat(SER_INQ_M_UPDATE);
                }
            }
        }
		if(NbSysFlag.auchMessage[NB_MESOFFSET3] != 0)
		{
			if(NbSysFlag.auchMessage[NB_MESOFFSET3] & NB_REQSETDBFLAG)
			{
				if(uchMaintDelayBalanceFlag)
				{
					uchMaintDelayBalanceFlag = 0;
					NbSysFlag.auchMessage[NB_MESOFFSET3] &= ~NB_REQSETDBFLAG;
					PifSaveFarData();
					PifLog(MODULE_NB_LOG, LOG_EVENT_DB_FLAG_OFF_2);

					/*The following PM_CNTRL was not being used, so we use this one
					to display the D-B message on the display*/
					flDispRegDescrControl &= ~PM_CNTRL;
					flDispRegKeepControl &= ~PM_CNTRL;

					//We set the class so that we can send the dummy data,
					//and trigger the discriptor to show D-B to inform the user that
					//the terminal is in delay balance mode
					RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0);

					//Reconcile the Delayed Balance EJ information
					//into the current EJ file. JHHJ
					EJDelayBalanceUpdateFile();
					EJWriteDelayBalance(uchMaintDelayBalanceFlag);
				}else
				{
					//we are not in delay balance mode, so we can turn this off JHHJ
					NbSysFlag.auchMessage[NB_MESOFFSET3] &= ~NB_REQSETDBFLAG;

					/*The following PM_CNTRL was not being used, so we use this one
					to display the D-B message on the display*/
					flDispRegDescrControl &= ~PM_CNTRL;
					flDispRegKeepControl &= ~PM_CNTRL;

					//We set the class so that we can send the dummy data,
					//and trigger the discriptor to make sure that 
					//the terminal is not in delay balance mode
					RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0);
				}
			}
		}
    } else {                                                         /* If B-Master, then execute */
        if ( sRet != NB_BMASTERUADD ) {                             /* Do not overwrite self status */
            NbSysFlag.fsBMBackupInf = NbRcvBuf.NbConMes.fsBackupInf ;    /* Copy BMaster backup information flag */
        }
    }

    NbExecRcvSetOnline();            /* Set online flag */

    PifReleaseSem(husNbSemHand);
}

/*
*===========================================================================
** Synopsis:    VOID NbExecRcvMonMes()
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Execute a received online message.
*===========================================================================
*/
VOID  NbExecRcvMonMes(VOID)            /* Execute a received message */
{
    SHORT   sRet;

    sRet = NbCheckUAddr();
    
	// Obtain the semaphore for the Notice Board globals.
	// We get this here since NbExecRcvSetOnline modifies NbSysFlag.
    PifRequestSem(husNbSemHand);

	if (0 != NbSndBuf.NbConMes.usNbFunCode) {
		// if there is a function code in the message then save the current status
		// check that the message is not from ourself since we see both messages from the
		// other terminal (Master and Backup Master) as well as ourselves (Backup Master and Master).
		if ( NbRcvBuf.NbMesHed.auchFaddr[3] == NB_MASTERUADD ) {        /* If Master, then execute */
			if (sRet == NB_BMASTERUADD) {                               /* Do not overwrite self status */
				NbSysFlag.fsMBackupInf = NbRcvBuf.NbConMes.fsBackupInf ;
			}
		} else if ( NbRcvBuf.NbMesHed.auchFaddr[3] == NB_BMASTERUADD ) {                                                        /* If B-Master, then execute */
			if ( sRet == NB_MASTERUADD ) {                              /* Do not overwrite self status */
				NbSysFlag.fsBMBackupInf = NbRcvBuf.NbConMes.fsBackupInf ;
			}
		}
	}

    NbExecRcvSetOnline();            /* Set online flag */

    PifReleaseSem(husNbSemHand);
}

/*
*===========================================================================
** Synopsis:    VOID NbSetFCtoSndBuf(USHORT usFunCode)
*     Input:    usFunCode
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Set a function code and reset start flag.
*===========================================================================
*/
VOID  NbSetFCtoSndBuf(USHORT usNbFunCode)
{

    PifRequestSem(husNbSemHand);

    NbSndBuf.NbConMes.usNbFunCode = usNbFunCode;                    /* Set function code */

    NbSysFlag.fsSystemInf &= ~(NB_STARTASMASTER | NB_STARTONLINE) ; /*  Reset start flag */

    PifReleaseSem(husNbSemHand);

}

/*
*===========================================================================
** Synopsis:    VOID  NbEndThread(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Goes to EndThread or Abort. 
*===========================================================================
*/
VOID  NbEndThread(VOID)
{
    NBMESSAGE pMes;
    SHORT     sRet;

    sRet = NbCheckUAddr();

    PifLog (MODULE_NB_LOG, LOG_ERROR_NB_THREADSHUTDOWN);    // Log the fact we are shutting down NoticeBoard

    if ( sRet != NB_MASTERUADD ) {          /* If Satellite, then execute */
        PifAbort(MODULE_NB_ABORT, FAULT_BAD_ENVIRONMENT);    /* Invalid Enviroment */
    }

    NbReadAllMessage(&pMes);                /* Read all flag      */        

    if (( pMes.fsSystemInf & NB_WITHOUTBM ) == 0) { 
        PifAbort(MODULE_NB_ABORT, FAULT_BAD_ENVIRONMENT);    /* Invalid Enviroment */
    }
    NbDescriptor(pMes.fsSystemInf);         /* Display desc.    */
    PifEndThread();                         /* Stop this thread */
}

/*
*===========================================================================
** Synopsis:    VOID NbExecRcvSetOnline(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Set online flag when receive message.
*
** WARNING:  The caller must have done a PifRequestSem(husNbSemHand);
*            before calling this function.
*            This function modifes Notice Board globals.
*===========================================================================
*/
VOID  NbExecRcvSetOnline(VOID)            /* Set online flag */
{
        
    if ( NbRcvBuf.NbMesHed.auchFaddr[3] == NB_MASTERUADD ) {   /* If Master, then execute */
        NbSysFlag.fsSystemInf |= NB_SETMTONLINE;               /* Set M online */
        usNbRcvFMT = 0;
        usNbRcvFBM ++;
    } else if ( NbRcvBuf.NbMesHed.auchFaddr[3] == NB_BMASTERUADD ) {     /* If B-Master, then execute */
        NbSysFlag.fsSystemInf |= NB_SETBMONLINE;               /* Set M online */
        usNbRcvFBM = 0;
        usNbRcvFMT ++;
    }
}

/*
*===========================================================================
** Synopsis:    SHORT  NbMakeConfirmMessage(SHORT sTermNo, USHORT fsSystemInf)
*     Input:    sTermNo     -Terminal No
*               fsSytemInf  -System Information Flag
*     Output:   nothing
*     InOut:    nothing
*
** Return:      NB_SUCCESS 
*
** Description: Makeup to confirm or response message.  
*===========================================================================
*/
SHORT  NbMakeConfirmMessage(SHORT sTermNo, USHORT fsSystemInf)
{
    USHORT  usTmp = (fsSystemInf & NB_RSTSYSFLAG);

    /*------- Start of Request or Response direct communication to B-M ---*/
    /* If detected timeout from B-Master then retry individual inquiry */

	NbSndBuf.NbConMes.ulParaHashCurrent = ParaGenerateHash();

    if ( sTermNo == NB_MASTERUADD ) {
        if ( ( usTmp == (NB_MTUPTODATE | NB_BMUPTODATE)) && 
             ( NbSndBuf.NbMesHed.auchFaddr[3] == (UCHAR)NB_BMASTERUADD) ) { 
            NbSndBuf.NbConMes.usNbFunCode &= ~(NB_MASTERUADD | NB_BMASTERUADD);
            NbSndBuf.NbConMes.usNbFunCode |= NB_MASTERUADD;  /* Set function code */
            sNbSavRes ++;                                  /* Saves a previous error */
            return(NB_SUCCESS);
        }
    } else {
        if ( NbSndBuf.NbMesHed.auchFaddr[3] == (UCHAR)NB_MASTERUADD ) { 
            NbSndBuf.NbConMes.usNbFunCode &= ~(NB_MASTERUADD | NB_BMASTERUADD);
            NbSndBuf.NbConMes.usNbFunCode |= NB_BMASTERUADD;  /* Set function code */
            sNbSavRes ++;                                   /* Saves a previous error */
            return(NB_SUCCESS);
        }
    }

    NbSndBuf.NbMesHed.auchFaddr[3] = (UCHAR)0;  /* Set all address */
    NbSndBuf.NbConMes.usNbFunCode &= NB_MASKFUNC; /* Reset function code */

    /*-------- End of check and request or response  ----------*/
   
    return(NB_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    SHORT   NbCheckCpmEpt(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
** Return:      NB_SUCCESS  ; Provide EPT/CPM
*               NB_ILLEGAL  ; Not Provide EPT/CPM
** Description: Check CPM/EPT, PC/IF System,                     V3.3
*===========================================================================
*/
SHORT   NbCheckCpmEpt(VOID)
{
    SHORT   i;
    SYSCONFIG CONST  *pSys = PifSysConfig();

    /* --- Check RS-232 Board --- */
    if (!(pSys->uchCom & COM_PROVIDED)) {
        return(NB_ILLEGAL);
    }

	/* --- Check PC I/F System --- */
    for (i = 1; i < PIF_LEN_PORT; i++) {
        if (pSys->auchComPort[i] == DEVICE_PC) {
            return(NB_SUCCESS);
        }
    }

    return(NB_ILLEGAL);
}

/*====== End of Source File ======*/

