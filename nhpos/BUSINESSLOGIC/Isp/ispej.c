/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1997            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ISP Server Module, EJ Functions Source File
* Category    : ISP Server Module, GP Model
* Program Name: ISPEJ.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*           IspRecvEJ();       Entry point of EJ function handling
*
* --------------------------------------------------------------------------
* Update Histories
* Date     :Ver.Rev.:   Name    : Description
* Jun-22-93:00.00.01:H.Yamaguchi: initial
* Mar-17-97:02.00.04:hrkato     : R2.0(GCA)
*
** NCR2171 **
* Aug-26-99:01.00.00:M.Teraki   :initial (for 2171)
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
#include	<stdio.h>
#include    <memory.h>
#include    <math.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <para.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <ej.h>
#include    <csstbej.h>
#include    <csstbstb.h>
#include    <csserm.h>
#include    <isp.h>
#include    <storage.h>
#include    "ispcom.h"
#include    "ispext.h"
#include    <appllog.h>

/*
*===========================================================================
** Synopsis:    VOID    IspRecvEj(VOID);
*
** Return:      none.
*
** Description:
*   This function executes for EJ function request.
*===========================================================================
*/
VOID    IspRecvEj(VOID)
{
	static EJT_HEADER  LastHeaderRead = { 0 };
	static struct {
		USHORT  usFunCode;
		USHORT  usSeqNo;
		SHORT   sResCode;
		SHORT   sReturn;
		ULONG   ulOffset;   
		ULONG   ulDataLen;
	} LastResMsgHRead = { 0 };

    CLIREQEJ    *pReqMsgH;
    CLIRESEJ    ResMsgH;
    USHORT      usLen;

    pReqMsgH = (CLIREQEJ *)IspRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESEJ));
    ResMsgH.usFunCode     = pReqMsgH->usFunCode;
    ResMsgH.sResCode      = STUB_SUCCESS;
    ResMsgH.sReturn       = ISP_SUCCESS;
    ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.ulDataLen     = 0;
    usLen = sizeof(ResMsgH) - sizeof(ResMsgH.auchEJData);

	// Check to ensure that the struct variable auchEJData is at end of the CLIRESEJ struct
	NHPOS_ASSERT(((UCHAR *)&ResMsgH.auchEJData[0] - (UCHAR *)&ResMsgH) == (sizeof(ResMsgH) - sizeof(ResMsgH.auchEJData)));

    /* Clean-Up E/J Status, at the initial ISP access */

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {
		case CLI_FCEJCLEAR:                         /* EJ clear */
		case CLI_FCEJREADFILE:                      /* EJ read file */
		case CLI_FCEJNOOFTRANS:                     /* EJ Get # of Trans */
		case CLI_FCEJREADRESET_END:                 /* EJ read/reset(not override) */
			IspCleanUpEJ();                         /* Clean-Up E/J Status,    R2.0GCA */
			break;
	        
		case CLI_FCEJREADRESET_OVER:                /* EJ read/reset(override) */
		case CLI_FCEJREADRESET_START:               /* EJ read/reset(not override) */
		case CLI_FCEJREADRESET:                     /* EJ read/reset(not override) */
			if (pReqMsgH->ulOffset == 0L) {
				if (usIspEJState & ISP_ST_EJ) {
					char xBuff[256];
					sprintf (xBuff, "==NOTE: IspRecvEj() IspCleanUpEJ() %d.%d.%d.%d usFunCode 0x%x usIspEJState 0x%x fsIspCleanUP 0x%x",
						IspRcvBuf.auchFaddr[0], IspRcvBuf.auchFaddr[1], IspRcvBuf.auchFaddr[2], IspRcvBuf.auchFaddr[3],
						(pReqMsgH->usFunCode & CLI_RSTCONTCODE), usIspEJState, fsIspCleanUP);
					NHPOS_ASSERT_TEXT(0, xBuff);
					sprintf (xBuff, "         LastHeaderRead usConsecutive %d, usCVLI %d", LastHeaderRead.usConsecutive, LastHeaderRead.usCVLI);
					NHPOS_NONASSERT_TEXT(xBuff);
				}
				IspCleanUpEJ();                     /* Clean-Up E/J Status,    R2.0GCA */
			} else {
				if ((usIspEJState & ISP_ST_EJ) == 0) {  /* after time out */
					char xBuff[128];
					sprintf (xBuff, "**WARNING: IspRecvEj() EJ_PROHIBIT usFunCode 0x%x usIspEJState 0x%x fsIspCleanUP 0x%x", (pReqMsgH->usFunCode & CLI_RSTCONTCODE), usIspEJState, fsIspCleanUP);
					NHPOS_ASSERT_TEXT(0, xBuff);
					sprintf (xBuff, "           pReqMsgH->ulOffset %d, LastHeaderRead usConsecutive %d, usCVLI %d", pReqMsgH->ulOffset, LastHeaderRead.usConsecutive, LastHeaderRead.usCVLI);
					NHPOS_NONASSERT_TEXT(xBuff);
					sprintf (xBuff, "           LastResMsgHRead sResCode %d, sReturn %d, ulOffset %u, ulDataLen %u", LastResMsgHRead.sResCode, LastResMsgHRead.sReturn, LastResMsgHRead.ulOffset, LastResMsgHRead.ulDataLen);
					NHPOS_NONASSERT_TEXT(xBuff);
					ResMsgH.sReturn = EJ_PROHIBIT;
					IspSendResponse((CLIRESCOM *)&ResMsgH, (USHORT)(usLen + ResMsgH.ulDataLen), NULL, 0);
					return;
				}
			}
			break;
    }

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {
    case CLI_FCEJCLEAR:                         /* EJ clear */
        /* check terminal reset report status */
        if (EJLock(EJLOCKFG_ISP) != EJ_PERFORM) {
			char xBuff[128];
			sprintf (xBuff, "**WARNING: IspRecvEj() CLI_FCEJCLEAR. EJLock(EJLOCKFG_ISP)  usIspEJState 0x%x", usIspEJState);
			NHPOS_ASSERT_TEXT(0, xBuff);
            ResMsgH.sReturn = EJ_EJLOCK_FAIL;
            IspSendResponse((CLIRESCOM *)&ResMsgH, (USHORT)(usLen + ResMsgH.ulDataLen), NULL, 0);
            return;
        }

        ResMsgH.sReturn = SerEJClear();

        /* unlock for terminal reset report */
        EJUnLock(EJLOCKFG_ISP);

        /* pc if reset log, V3.3 */
        IspResetLog.uchAction = 100;
        IspResetLog.uchDayPtd = 0;
        IspResetLog.uchCurSave = 0;
        IspResetLog.uchReset = ISP_RESET_RESET;
        IspResetLog.ulNumber = 0;
        IspResetLog.uchError = (UCHAR)(EJConvertError(ResMsgH.sReturn));
        IspWriteResetLog();
        break;

    case CLI_FCEJREADFILE:                      /* EJ read file */
        ResMsgH.sReturn = SerEJReadFile(pReqMsgH->ulOffset, ResMsgH.auchEJData, CLI_MAX_EJDATA, &ResMsgH.ulDataLen);

        /* unlock for terminal reset report */
        EJUnLock(EJLOCKFG_ISP);

        /* pc if reset log, V3.3 */
        IspResetLog.uchAction = 70;
        IspResetLog.uchDayPtd = 0;
        IspResetLog.uchCurSave = 0;
        IspResetLog.uchReset = ISP_RESET_READ;
        IspResetLog.ulNumber = 0;
        IspResetLog.uchError = (UCHAR)(EJConvertError(ResMsgH.sReturn));
        IspWriteResetLog();
        break;

    case CLI_FCEJNOOFTRANS:                     /* EJ Get # of Trans */
        ResMsgH.sReturn = SerEJNoOfTrans();

        /* unlock for terminal reset report */
        EJUnLock(EJLOCKFG_ISP);
        break;

    case CLI_FCEJREADRESET_OVER:                /* EJ read/reset(override) */
        /* check terminal reset report status */
        if (EJLock(EJLOCKFG_ISP) != EJ_PERFORM) {
			char xBuff[128];
			sprintf (xBuff, "**WARNING: IspRecvEj() CLI_FCEJREADRESET_OVER. EJLock(EJLOCKFG_ISP) usIspEJState 0x%x", usIspEJState);
			NHPOS_ASSERT_TEXT(0, xBuff);
            ResMsgH.sReturn = EJ_EJLOCK_FAIL;
            IspSendResponse((CLIRESCOM *)&ResMsgH, (USHORT)(usLen + ResMsgH.ulDataLen), NULL, 0);
            return;
        }

        ResMsgH.ulOffset = pReqMsgH->ulOffset;
        ResMsgH.sReturn = SerEJReadResetTransOver(&ResMsgH.ulOffset, ResMsgH.auchEJData, CLI_MAX_EJDATA, &ResMsgH.ulDataLen);

        if (ResMsgH.sReturn < 0) {
            /* unlock for terminal reset report */
            EJUnLock(EJLOCKFG_ISP);
        }

		if (pReqMsgH->ulOffset == 0) {
			// since offset being read is zero first part is the EJ Header info
			// so we will save this for error messages.
			LastHeaderRead = *((EJT_HEADER *)ResMsgH.auchEJData);
		}

        /* pc if reset log, V3.3 */
        if (ResMsgH.sReturn == EJ_END) {
            IspResetLog.uchAction = 100;
            IspResetLog.uchDayPtd = 0;
            IspResetLog.uchCurSave = 0;
            IspResetLog.uchReset = ISP_RESET_RESET;
            IspResetLog.ulNumber = 0;
            IspResetLog.uchError = (UCHAR)(EJConvertError(ResMsgH.sReturn));
            IspWriteResetLog();
        }
        break;

    case CLI_FCEJREADRESET_START:               /* EJ read/reset(not override) */
        /* check terminal reset report status */
        if (EJLock(EJLOCKFG_ISP) != EJ_PERFORM) {
			char xBuff[128];
			sprintf (xBuff, "**WARNING: IspRecvEj(): CLI_FCEJREADRESET_START. EJLock(EJLOCKFG_ISP) usIspEJState 0x%x", usIspEJState);
			NHPOS_ASSERT_TEXT(0, xBuff);
            ResMsgH.sReturn = EJ_EJLOCK_FAIL;
            IspSendResponse((CLIRESCOM *)&ResMsgH, (USHORT)(usLen + ResMsgH.ulDataLen), NULL, 0);
            return;
        }

        ResMsgH.ulOffset = pReqMsgH->ulOffset;
        ResMsgH.usNumTrans = pReqMsgH->usNumTrans;
        ResMsgH.sReturn = SerEJReadResetTransStart(&ResMsgH.ulOffset, ResMsgH.auchEJData, CLI_MAX_EJDATA, &ResMsgH.ulDataLen, &ResMsgH.usNumTrans);

		if (pReqMsgH->ulOffset == 0) {
			// since offset being read is zero first part is the EJ Header info
			// so we will save this for error messages.
			LastHeaderRead = *((EJT_HEADER *)ResMsgH.auchEJData);
		}

        if ((ResMsgH.sReturn < 0) && (ResMsgH.sReturn != EJ_END)){
            /* unlock for terminal reset report */
            EJUnLock(EJLOCKFG_ISP);

            /* pc if reset log, V3.3 */
            IspResetLog.uchAction = 100;
            IspResetLog.uchDayPtd = 0;
            IspResetLog.uchCurSave = 0;
            IspResetLog.uchReset = ISP_RESET_RESET;
            IspResetLog.ulNumber = 0;
            IspResetLog.uchError = (UCHAR)(EJConvertError(ResMsgH.sReturn));
            IspWriteResetLog();
        }
        break;

    case CLI_FCEJREADRESET:                     /* EJ read/reset(not override) */
        ResMsgH.ulOffset = pReqMsgH->ulOffset;
        ResMsgH.usNumTrans = pReqMsgH->usNumTrans;
        ResMsgH.sReturn = SerEJReadResetTrans(&ResMsgH.ulOffset, ResMsgH.auchEJData, CLI_MAX_EJDATA, &ResMsgH.ulDataLen, &ResMsgH.usNumTrans);

		if (pReqMsgH->ulOffset == 0) {
			// since offset being read is zero first part is the EJ Header info
			// so we will save this for error messages.
			LastHeaderRead = *((EJT_HEADER *)ResMsgH.auchEJData);
		}

        if ((ResMsgH.sReturn < 0) && (ResMsgH.sReturn != EJ_END)){
            /* unlock for terminal reset report */
            EJUnLock(EJLOCKFG_ISP);

            /* pc if reset log, V3.3 */
            IspResetLog.uchAction = 100;
            IspResetLog.uchDayPtd = 0;
            IspResetLog.uchCurSave = 0;
            IspResetLog.uchReset = ISP_RESET_RESET;
            IspResetLog.ulNumber = 0;
            IspResetLog.uchError = (UCHAR)(EJConvertError(ResMsgH.sReturn));
            IspWriteResetLog();
        }
        break;

    case CLI_FCEJREADRESET_END:                 /* EJ read/reset(not override) */
        ResMsgH.sReturn = SerEJReadResetTransEnd(pReqMsgH->usNumTrans);

        /* unlock for terminal reset report */
        EJUnLock(EJLOCKFG_ISP);

        /* pc if reset log, V3.3 */
        IspResetLog.uchAction = 100;
        IspResetLog.uchDayPtd = 0;
        IspResetLog.uchCurSave = 0;
        IspResetLog.uchReset = ISP_RESET_RESET;
        IspResetLog.ulNumber = 0;
        IspResetLog.uchError = (UCHAR)(EJConvertError(ResMsgH.sReturn));
        IspWriteResetLog();
        break;

    default:                                    /* not used */
		PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_RCVEJ_INV_FUN);
        return;
    }

	{
		LastResMsgHRead.usFunCode = ResMsgH.usFunCode;
		LastResMsgHRead.usSeqNo = ResMsgH.usSeqNo;
		LastResMsgHRead.sResCode = ResMsgH.sResCode;
		LastResMsgHRead.sReturn = ResMsgH.sReturn;
		LastResMsgHRead.ulOffset = ResMsgH.ulOffset;
		LastResMsgHRead.ulDataLen = ResMsgH.ulDataLen;
	}
	if (ResMsgH.sReturn < 0 && ResMsgH.sReturn != EJ_EOF && ResMsgH.sReturn != EJ_END) {
		char  xBuff[128];

		sprintf (xBuff, "**WARNING: IspRecvEj() %d.%d.%d.%d usIspEJState 0x%x, usFuncCode 0x%x, sReturn %d, sResCode %d",
			IspRcvBuf.auchFaddr[0], IspRcvBuf.auchFaddr[1], IspRcvBuf.auchFaddr[2], IspRcvBuf.auchFaddr[3],
			usIspEJState, (pReqMsgH->usFunCode & CLI_RSTCONTCODE), ResMsgH.sReturn, ResMsgH.sResCode);
		NHPOS_NONASSERT_TEXT(xBuff);

		PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_EJ_RQST_FAIL);
		PifLog(MODULE_DATA_VALUE(MODULE_ISP_LOG), (USHORT)(pReqMsgH->usFunCode & CLI_RSTCONTCODE));
		PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(ResMsgH.sReturn));
	}

    IspSendResponse((CLIRESCOM *)&ResMsgH, (usLen + ResMsgH.ulDataLen), NULL, 0);
}

/*===== END OF SOURCE =====*/
