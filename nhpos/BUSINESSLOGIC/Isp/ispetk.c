/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ISP Server Module, ETK Functions Source File
* Category    : ISP Server Module, US Hospitality Model
* Program Name: ISPETK.C
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
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:
*
*           IspRecvEtk();       Entry point of cashier function handling
*
* --------------------------------------------------------------------------
* Update Histories
* Date     :Ver.Rev.:   Name    : Description
* Jun-22-93:00.00.01:H.Yamaguchi: initial
* Oct-05-93:00.00.01:H.Yamaguchi: Adds EtkStatRead/SerEtkCurIndWrite
* Nov-09-95:T.Nakahata : R3.1 Initial
*                           Increase No. of Employees (200 => 250)
*                           Increase Employee No. ( 4 => 9digits)
*                           Add Employee Name (Max. 16 Char.)
* Dec-14-99:hrkato     : Saratoga
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

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <para.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <csserm.h>
#include    <isp.h>
#include    <storage.h>
#include    <csttl.h>
#include    "ispcom.h"
#include    "ispext.h"
#include    <appllog.h>

/*
*===========================================================================
** Synopsis:    VOID    IspRecvEtk(VOID);
*
** Return:      none.
*
** Description:
*   This function executes for ETK function request.
*===========================================================================
*/
VOID    IspRecvEtk(VOID)
{
    CLIREQETK   *pReqMsgH;
    CLIRESETK   ResMsgH;

    pReqMsgH = (CLIREQETK *)IspRcvBuf.auchData;
    memset(&ResMsgH , 0, sizeof(CLIRESETK));
    ResMsgH.usFunCode     = pReqMsgH->usFunCode;
    ResMsgH.sResCode      = STUB_SUCCESS;
    ResMsgH.sReturn       = ISP_SUCCESS;
    ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.usDataLen     = 0;
    ResMsgH.usFieldNo     = pReqMsgH->usFieldNo;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {

    case CLI_FCETKTIMEIN:                   /* ETK time in */
        ResMsgH.sReturn = CliEtkTimeIn(pReqMsgH->ulEmployeeNo, pReqMsgH->uchJobCode, (ETK_FIELD *)ResMsgH.auchETKData);
        ResMsgH.usDataLen = sizeof(ETK_FIELD);
        break;             

    case CLI_FCETKASSIGN:                   /* ETK assign   */
        ResMsgH.sReturn = SerEtkAssign(pReqMsgH->ulEmployeeNo, (ETK_JOB *)pReqMsgH->auchNewJob, pReqMsgH->auchEmployeeName);
        break;             

    case CLI_FCETKINDJOBREAD:               /* ETK ind job read  */
        ResMsgH.sReturn = SerEtkIndJobRead(pReqMsgH->ulEmployeeNo, (ETK_JOB *)ResMsgH.auchETKData, &ResMsgH.auchEmployeeName[0]);
        ResMsgH.usDataLen = sizeof(ETK_JOB);
        break;

    case CLI_FCETKDELETE:                   /* ETK delete   */
        ResMsgH.sReturn = SerEtkDelete(pReqMsgH->ulEmployeeNo);
        break;

    case CLI_FCETKALLRESET:                 /* ETK all reset  */
        ResMsgH.sReturn = SerEtkAllReset();
        /* pc if reset log, V3.3 */
        IspResetLog.uchAction = 151;
        IspResetLog.uchDayPtd = 0;
        IspResetLog.uchCurSave = 0;
        IspResetLog.uchReset = ISP_RESET_RESET;
        IspResetLog.ulNumber = 0;
        IspResetLog.uchError = (UCHAR)(EtkConvertError(ResMsgH.sReturn));
        IspWriteResetLog();
        break;

    case CLI_FCETKISSUEDSET:                /* ETK issued set */
        ResMsgH.sReturn = SerEtkIssuedSet();
        /* pc if reset log, V3.3 */
        IspResetLog.uchAction = 151;
        IspResetLog.uchDayPtd = 0;
        IspResetLog.uchCurSave = 0;
        IspResetLog.uchReset = ISP_RESET_ISSUE;
        IspResetLog.ulNumber = 0;
        IspResetLog.uchError = (UCHAR)(EtkConvertError(ResMsgH.sReturn));
        break;

    case CLI_FCETKCURALLIDRD:            /* ETK all cur id read   */
        IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESETK)];
        ResMsgH.sReturn = SerEtkCurAllIdRead(4*FLEX_ETK_MAX, (ULONG *)IspResp.pSavBuff->auchData);
        if (0 <= ResMsgH.sReturn) {
            IspResp.pSavBuff->usDataLen = 4*ResMsgH.sReturn;
            ResMsgH.sResCode = STUB_MULTI_SEND;
        } else {
            ResMsgH.usDataLen = 0;
        }
        break;

    case CLI_FCETKSAVALLIDRD:            /* ETK all sav id read   */
        IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESETK)];
        ResMsgH.sReturn = SerEtkSavAllIdRead(4*FLEX_ETK_MAX, (ULONG *)IspResp.pSavBuff->auchData);
        if (0 <= ResMsgH.sReturn) {
            IspResp.pSavBuff->usDataLen = 4*ResMsgH.sReturn;
            ResMsgH.sResCode = STUB_MULTI_SEND;
        } else {
            ResMsgH.usDataLen = 0;
        }
        break;

    case CLI_FCETKCURINDREAD:            /* ETK cur ind id read   */
        ResMsgH.sReturn = SerEtkCurIndRead(pReqMsgH->ulEmployeeNo, &ResMsgH.usFieldNo, (ETK_FIELD  *)ResMsgH.auchETKData,
                                           (ETK_TIME   *)(ResMsgH.auchETKData + sizeof(ETK_FIELD)));
        ResMsgH.usDataLen = sizeof(ETK_FIELD) + sizeof(ETK_TIME);

        /* pc if reset log, V3.3 */
        IspResetLog.uchAction = 150;
        IspResetLog.uchDayPtd = 0;
        IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        IspResetLog.uchReset = ISP_RESET_READ;
        IspResetLog.ulNumber = pReqMsgH->ulEmployeeNo,
        IspResetLog.uchError = (UCHAR)(EtkConvertError(ResMsgH.sReturn));
        break;

    case CLI_FCETKSAVINDREAD:            /* ETK sav ind id read   */
        ResMsgH.sReturn = SerEtkSavIndRead(pReqMsgH->ulEmployeeNo, &ResMsgH.usFieldNo, (ETK_FIELD  *)ResMsgH.auchETKData,
                                           (ETK_TIME   *)(ResMsgH.auchETKData + sizeof(ETK_FIELD)));
        ResMsgH.usDataLen = sizeof(ETK_FIELD) + sizeof(ETK_TIME);

        /* pc if reset log, V3.3 */
        IspResetLog.uchAction = 150;
        IspResetLog.uchDayPtd = 0;
        IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        IspResetLog.uchReset = ISP_RESET_READ;
        IspResetLog.ulNumber = pReqMsgH->ulEmployeeNo,
        IspResetLog.uchError = (UCHAR)(EtkConvertError(ResMsgH.sReturn));
        break;

    case CLI_FCETKALLLOCK:               /* ETK all lock          */
        if ( fsIspLockedFC & ISP_LOCK_ALLETK) {
            ResMsgH.sReturn = ETK_LOCK;
        } else {
            if ( ETK_SUCCESS == ( ResMsgH.sReturn = SerEtkAllLock() ) ) {
                fsIspLockedFC  |= ISP_LOCK_ALLETK;
            }
        }
        break;

    case CLI_FCETKALLUNLOCK:             /* ETK all unlock        */
        IspCleanUpLockETK();
        break;
    
    case CLI_FCETKISSUEDCHK:             /* ETK issued check      */
        ResMsgH.sReturn = SerEtkIssuedCheck();
        break;

    case CLI_FCETKSTATREAD:               /* ETK status read */
        ResMsgH.sReturn = SerEtkStatRead(pReqMsgH->ulEmployeeNo, &ResMsgH.sTMStatus, &ResMsgH.usFieldNo, (ETK_JOB *)ResMsgH.auchETKData);
        ResMsgH.usDataLen = sizeof(ETK_JOB);
        break;

    case CLI_FCETKCURINDWRITE:            /* ETK cur ind write field */
        ResMsgH.sReturn = SerEtkCurIndWrite(pReqMsgH->ulEmployeeNo, &ResMsgH.usFieldNo, (ETK_FIELD  *)pReqMsgH->auchNewJob);
        break;

    case CLI_FCETKCREATTLIDX:               /* ETK create total index  (ISP unique)*/
        /* This function executes only re-writing PARAMETK file by PEP. */
        ResMsgH.sReturn = EtkCreateTotalIndex();
        break;

    default:                                   /* not used */ 
		PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_RCVETK_INV_FUN);
        return;
        break;
    }

    if (STUB_MULTI_SEND == ResMsgH.sResCode) {
        IspSendMultiple((CLIRESCOM *)&ResMsgH, sizeof(CLIRESETK));
    } else {
        IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESETK), NULL, 0);
    }
}

/*===== END OF SOURCE =====*/
