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
* Title       : SERVER module, Kitchin Printer Source File
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVKPS.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           SerRecvKps();       Kitchin printer function handling
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
*          :           :                                    
*          :           : Saratoga
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
#include    <memory.h>

#include    <ecr.h>
#include    <pif.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <cskp.h>
#include    <cscas.h>
#include    "servmin.h"

/*
*===========================================================================
** Synopsis:    VOID    SerRecvKps(VOID);
*
** Return:      none.
*
** Description:
*   This function executes for kitchin printer functuion request.
*===========================================================================
*/
VOID    SerRecvKps(VOID)
{
    CLIREQKITPRINT  *pReqMsgH;
    CLIRESKITPRINT  ResMsgH;
    USHORT          usSendLen, usDataLen, usWriteLen;

    pReqMsgH = (CLIREQKITPRINT *)SerRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESKITPRINT));
    ResMsgH.usFunCode = pReqMsgH->usFunCode;
    ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sResCode  = STUB_SUCCESS;

    usDataLen = 0;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {

    case    CLI_FCKPSPRINT:
        ResMsgH.sReturn = KpsPrint(SerResp.pSavBuff->auchData,
                                   SerResp.pSavBuff->usDataLen,
                                   SerRcvBuf.auchFaddr[CLI_POS_UA],
                                   &ResMsgH.usKptStatus,
                                    pReqMsgH->usOutPrinterInfo ); /* ### ADD 2172 Rel1.0 (Saratoga) */
// Following lines commented out so that proper printer error message
// will be sent rather than a network layer message of STUB_BUSY is sent.
//        if (KPS_PRINTER_BUSY == ResMsgH.sReturn) {
//            SerSendError(STUB_BUSY);
//            return;
//        }
		// If we have started a print, then we will turn on our kitchen
		// printer timer.  The purpose of this timer is to check that
		// a submitted print request completes and the printer is unlocked.
		// if the timer expires and the print request is not yet complete.
        if (KPS_TIMER_ON == ResMsgH.sReturn) {
            SerKpsTimerStart((UCHAR)(pReqMsgH->usOutPrinterInfo & 0x00ff));
        } else if (KPS_PRINTER_BUSY != ResMsgH.sReturn) {
            SerKpsTimerStop((UCHAR)(pReqMsgH->usOutPrinterInfo & 0x00ff));
        }
        usSendLen = sizeof(CLIRESKITPRINT);
        break;

    case    CLI_FCKPSALTMANKP:
        ResMsgH.sReturn = CliParaAllWrite(CLASS_PARAMANUALTKITCH,
                                          SerResp.pSavBuff->auchData,
                                          SerResp.pSavBuff->usDataLen,
                                          0,
                                          &usWriteLen);
        usSendLen = sizeof(CLIRESALTKP);
        break;

    default:                                    /* not used */
        return;
        break;
    }
    SerSendResponse((CLIRESCOM *)&ResMsgH, usSendLen, NULL, 0);
}

/*
*===========================================================================
** Synopsis:    VOID    SerChekKpsUnLock(VOID);
*
** Return:      none.
*
** Description: This function executes for kitchin printer functuion request.
*===========================================================================
*/
VOID    SerChekKpsUnLock(VOID)
{
    KpsUnlock();
}

/*===== END OF SOURCE =====*/

