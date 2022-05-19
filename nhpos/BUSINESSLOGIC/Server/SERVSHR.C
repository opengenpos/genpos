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
* Title       : SERVER module, Shared Printer Source File
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVSHR.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           SerRecvSps();          Shared printer function handling
*           SerClearShrTermLock()  Unlock Shared printer
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jun-07-93:H.Yamaguchi: initial                                   
* Apr-13-95:M.Suzuki   : Chg R3.0 shared K/P                                   
*          :           :                                    
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
#include    <shr.h>
#include    <csstbshr.h>
#include    "servmin.h"

/*
*===========================================================================
** Synopsis:    VOID    SerRecvSps(VOID);
*
** Return:      none.
*
** Description: This function executes for shared printer functuion request.
*===========================================================================
*/
VOID    SerRecvSps(VOID)
{
    CLIREQSHR  *pReqMsgH;
    CLIRESSHR  ResMsgH;

    pReqMsgH = (CLIREQSHR *)SerRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESSHR));
    ResMsgH.usFunCode = pReqMsgH->usFunCode;
    ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sResCode  = STUB_SUCCESS;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {

    case    CLI_FCSPSPRINT:
    case    CLI_FCSPSPRINTKP:       /* Add R3.0 */

        ResMsgH.sReturn = ShrPrint(SerResp.pSavBuff->auchData);
// Following lines commented out so that proper printer error message
// will be sent rather than a network layer message of STUB_BUSY is sent.
//        if (SHR_BUSY == ResMsgH.sReturn) {
//            SerSendError(STUB_BUSY);
//            return;
//        }
		// If we have started a print, then we will turn on our kitchen
		// printer timer.  The purpose of this timer is to check that
		// a submitted print request completes and the printer is unlocked.
		// if the timer expires and the print request is not yet complete.
        if ( (SHR_TIMERON == ResMsgH.sReturn) || 
             (SHR_BUFFER_FULL == ResMsgH.sReturn) ) {
            SerSpsTimerStart(((SHRHEADERINF *)SerResp.pSavBuff->auchData)->uchTermAdr);
                                                            /* Chg R3.0 */
        } else if (SHR_BUSY != ResMsgH.sReturn){
            SerSpsTimerStop();
        }
        break;

	case    CLI_FCSPSPRINTSTAT:
		ResMsgH.sReturn = ShrPolling();
		break;

    case    CLI_FCSPSTERMLOCK:     /* SPS Lock   */

        ResMsgH.sReturn = ShrTermLock(SerRcvBuf.auchFaddr[CLI_POS_UA]);
        if (SHR_TIMERON == ResMsgH.sReturn) {
            fsSerShrStatus = ResMsgH.sReturn ; /* Set shred printer status */
            SerSpsTimerStart(SerRcvBuf.auchFaddr[CLI_POS_UA]);
        } else if (SHR_BUSY != ResMsgH.sReturn) {
            SerSpsTimerStop();
        }
        break;

    case    CLI_FCSPSTERMUNLOCK:   /* SPS Unlock */

        ResMsgH.sReturn = ShrTermUnLock();
        SerSpsTimerStop();
        break;

    /************************** Add R3.0 *******************************/
    case    CLI_FCSPSPOLLING:   /* SPS Polling */

        ResMsgH.sReturn = ShrPolling();
        break;
    /************************** End Add R3.0 ****************************/

    default:
        return;                            
        break;
    }

    SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESSHR) , NULL, 0);
}

/*
*===========================================================================
** Synopsis:    VOID    SerClearShrTermLock(VOID);
*
** Return:      none.
*
** Description:
*   This function executes to unlock shared printer functuion request.
*===========================================================================
*/
VOID    SerClearShrTermLock(VOID)
{
	if (SHR_PERFORM == ShrCancel(SerSpsTimerTermNo())) {
        SerSpsTimerStop();
	}
	else {
		NHPOS_ASSERT_TEXT(0, "SerClearShrTermLock(): ShrCancel () did not return SHR_PERFORM.");
	}
}

/*===== END OF SOURCE =====*/


