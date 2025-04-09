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
* Title       : SERVER module, Cashier Functions Source File                        
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVCAS.C                                            
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
*       2020  -> OpenGenPOS Rel 2.4.0 (Windows 10, Datacap removed) Open source
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2019 with Rel 2.4.0
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:
*
*           SerRecvCas();       Cashier function handling
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jun-04-92:H.Nakashima: initial                                   
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
#include    <stdio.h>

#include    <ecr.h>
#include    <appllog.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <csttl.h>
#include    <cscas.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    "servmin.h"
#include	"display.h"
#include	"csstbcas.h"
#include    <rfl.h>

/*
*===========================================================================
** Synopsis:    VOID    SerRecvCas(VOID);
*
** Return:      none.
*
** Description: This function executes for cashier function request.
*===========================================================================
*/
VOID    SerRecvCas(VOID)
{
    CLIREQCASHIER   * const pReqMsgH = (CLIREQCASHIER *)SerRcvBuf.auchData;
    CLIRESCASHIER   ResMsgH = { 0 };
    CASIF           Casif = { 0 };
	USHORT          usTermNo;
	SHORT           sSerSendStatus;

	usTermNo = PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);

    ResMsgH.usFunCode     = pReqMsgH->usFunCode;
    ResMsgH.sResCode      = STUB_SUCCESS;
    ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.ulCashierNo   = pReqMsgH->ulCashierNo;
    ResMsgH.ulSecretNo    = pReqMsgH->ulSecretNo;
    ResMsgH.usStartGCN    = pReqMsgH->usStartGCN;   /* V3.3 */
    ResMsgH.usEndGCN      = pReqMsgH->usEndGCN;     /* V3.3 */
    ResMsgH.uchChgTipRate = pReqMsgH->uchChgTipRate;/* V3.3 */
    ResMsgH.uchTeamNo     = pReqMsgH->uchTeamNo;    /* V3.3 */
    ResMsgH.uchTerminal   = pReqMsgH->uchTerminal;  /* V3.3 */
	ResMsgH.usSupervisorID    = pReqMsgH->usSupervisorID;
	ResMsgH.usCtrlStrKickoff  = pReqMsgH->usCtrlStrKickoff;
	ResMsgH.usStartupWindow   = pReqMsgH->usStartupWindow;
	ResMsgH.ulGroupAssociations   = pReqMsgH->ulGroupAssociations;
    memcpy(ResMsgH.fbStatus, pReqMsgH->fbStatus, CLI_CASHIERSTATUS);    /* V3.3 */
    _tcsncpy(ResMsgH.auchCasName, pReqMsgH->auchCasName, CLI_CASHIERNAME);

    Casif.ulCashierNo     = pReqMsgH->ulCashierNo;
    Casif.ulCashierSecret = pReqMsgH->ulSecretNo;
    Casif.usUniqueAddress = pReqMsgH->usTerminalNo;
    Casif.ulCashierOption = pReqMsgH->ulOption;     /* V3.3 */

    memcpy(Casif.fbCashierStatus, pReqMsgH->fbStatus, CLI_CASHIERSTATUS);   /* V3.3 */
    _tcsncpy(Casif.auchCashierName, pReqMsgH->auchCasName, CLI_CASHIERNAME);
    Casif.usGstCheckStartNo = pReqMsgH->usStartGCN; /* V3.3 */
    Casif.usGstCheckEndNo = pReqMsgH->usEndGCN;     /* V3.3 */
    Casif.uchChgTipRate   = pReqMsgH->uchChgTipRate;/* V3.3 */
    Casif.uchTeamNo       = pReqMsgH->uchTeamNo;    /* V3.3 */
    Casif.uchTerminal     = pReqMsgH->uchTerminal;  /* V3.3 */
	Casif.usSupervisorID    = pReqMsgH->usSupervisorID;
	Casif.usCtrlStrKickoff  = pReqMsgH->usCtrlStrKickoff;
	Casif.usStartupWindow   = pReqMsgH->usStartupWindow;
	Casif.ulGroupAssociations   = pReqMsgH->ulGroupAssociations;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {

    case CLI_FCCASSIGNIN:
        ResMsgH.sReturn  = CasSignIn(&Casif);
		if (0 <= ResMsgH.sReturn) {
			memcpy(ResMsgH.fbStatus, Casif.fbCashierStatus, CLI_CASHIERSTATUS); /* V3.3 */
			_tcsncpy(ResMsgH.auchCasName, Casif.auchCashierName, CLI_CASHIERNAME);
			ResMsgH.usStartGCN  = Casif.usGstCheckStartNo;  /* V3.3 */
			ResMsgH.usEndGCN    = Casif.usGstCheckEndNo;    /* V3.3 */
			ResMsgH.uchChgTipRate = Casif.uchChgTipRate;    /* V3.3 */
			ResMsgH.uchTeamNo   = Casif.uchTeamNo;          /* V3.3 */
			ResMsgH.uchTerminal = Casif.uchTerminal;        /* V3.3 */
			ResMsgH.usSupervisorID    = Casif.usSupervisorID;
			ResMsgH.usCtrlStrKickoff  = Casif.usCtrlStrKickoff;
			ResMsgH.usStartupWindow   = Casif.usStartupWindow;
			ResMsgH.ulGroupAssociations   = Casif.ulGroupAssociations;
		}
        if (CAS_DURING_SIGNIN == ResMsgH.sReturn) {
            SerSendError(STUB_BUSY);
            return;
        }
#if defined(POSSIBLE_DEAD_CODE)
        if (CAS_PERFORM == ResMsgH.sReturn) {
            SstCpmResetReqNo(Casif.usUniqueAddress);  /* Reset Cpm No. */
        }
#endif
		break;

    case CLI_FCCASALLIDREAD : /* Cashier All Id Read */
        SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESCASHIER)];
        SerResp.ulSavBuffSize = sizeof(auchSerTmpBuf) - sizeof(CLIRESCASHIER);
        ResMsgH.sReturn = SerCasAllIdRead(sizeof(ULONG)*CAS_NUMBER_OF_MAX_CASHIER, (ULONG *)SerResp.pSavBuff->auchData );
        if (0 <= ResMsgH.sReturn) {
            SerResp.pSavBuff->usDataLen = sizeof(ULONG)*ResMsgH.sReturn;
            ResMsgH.sResCode = STUB_MULTI_SEND;
        } else {
            SerResp.pSavBuff->usDataLen = 0;
        }

		if(ResMsgH.sResCode == STUB_MULTI_SEND)
		{
			sSerSendStatus = SerSendMultiple((CLIRESCOM *)&ResMsgH, sizeof(CLIRESCASHIER));
			if (sSerSendStatus < 0) {
				char xBuff [128];
				sprintf (xBuff, "usFunCode = 0x%x, usSeqNo = 0x%x, sSerSendStatus = %d", ResMsgH.usFunCode, ResMsgH.usSeqNo, sSerSendStatus);
				NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
			}
			return;
		}
        break;

    case CLI_FCCASINDREAD :   /* Cashier individual Read */
        ResMsgH.sReturn = SerCasIndRead(&Casif);
        
        if (0 <= ResMsgH.sReturn) {
            ResMsgH.ulSecretNo    = Casif.ulCashierSecret ;
            _tcsncpy(ResMsgH.auchCasName, Casif.auchCashierName, CLI_CASHIERNAME);
            memcpy(ResMsgH.fbStatus, Casif.fbCashierStatus, CLI_CASHIERSTATUS);
            ResMsgH.usStartGCN    = Casif.usGstCheckStartNo;
            ResMsgH.usEndGCN      = Casif.usGstCheckEndNo;
            ResMsgH.uchChgTipRate = Casif.uchChgTipRate;
            ResMsgH.uchTeamNo     = Casif.uchTeamNo;
            ResMsgH.uchTerminal   = Casif.uchTerminal;
			ResMsgH.usSupervisorID    = Casif.usSupervisorID;
			ResMsgH.usCtrlStrKickoff  = Casif.usCtrlStrKickoff;
			ResMsgH.usStartupWindow   = Casif.usStartupWindow;
			ResMsgH.ulGroupAssociations   = Casif.ulGroupAssociations;
        }
        break;
 
    case CLI_FCCASSIGNOUT:
        ResMsgH.sReturn = CasSignOut(&Casif);
        break;

    case CLI_FCCASPRESIGNOUT:
        ResMsgH.sReturn = CasPreSignOut(&Casif);
        break;

    case CLI_FCCASASSIGN:
        ResMsgH.sReturn = CasAssign(&Casif);
        break;

    case CLI_FCCASDELETE:
        ResMsgH.sReturn = CasDelete(Casif.ulCashierNo);
        if (0 <= ResMsgH.sReturn) {
            CLITTLCASWAI    TtlCas = { 0 };

            TtlCas.uchMajorClass = CLASS_TTLCASHIER;
			TtlCas.ulCashierNo   = Casif.ulCashierNo;
			TtlCas.uchMinorClass = CLASS_TTLSAVDAY;
			TtlTotalDelete(&TtlCas);
			TtlCas.uchMinorClass = CLASS_TTLSAVPTD;
			TtlTotalDelete(&TtlCas);
		}
        break;

    case CLI_FCCASINDCLOSE:
        ResMsgH.sReturn = CasClose(Casif.ulCashierNo);
        break;

    case CLI_FCCASCLRSCODE:
        ResMsgH.sReturn = CasSecretClr(Casif.ulCashierNo);
        break;

    case CLI_FCCASOPENPICLOA:                   /* Saratoga */
        ResMsgH.sReturn = CasOpenPickupLoan(&Casif);
        memcpy(ResMsgH.fbStatus, Casif.fbCashierStatus, CLI_CASHIERSTATUS);
        _tcsncpy(ResMsgH.auchCasName, Casif.auchCashierName, CLI_CASHIERNAME);
        ResMsgH.ulSecretNo = Casif.ulCashierSecret;
        break;

    case CLI_FCCASCLOSEPICLOA:                  /* Saratoga */
        ResMsgH.sReturn = CasClosePickupLoan(&Casif);
        _tcsncpy(ResMsgH.auchCasName, Casif.auchCashierName, CLI_CASHIERNAME);
        memcpy(ResMsgH.fbStatus, Casif.fbCashierStatus, CLI_CASHIERSTATUS);
        ResMsgH.ulSecretNo = Casif.ulCashierSecret;
        break;

	case CLI_FCCASDELAYBALON:
		ResMsgH.sReturn = SerCasDelayedBalance(Casif.uchTerminal, Casif.ulCashierOption);
		if(ResMsgH.sReturn == SUCCESS)
		{			
			if(usTermNo == Casif.uchTerminal)
			{
				/*The following PM_CNTRL was not being used, so we use this one
				to display the D-B message on the display*/
				flDispRegDescrControl |= PM_CNTRL;
				flDispRegKeepControl |= PM_CNTRL;

				//We set the class so that we can send the dummy data,
				//and trigger the discriptor to show D-B to inform the user that
				//the terminal is in delay balance mode
				RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0); 	//Delay Balance JHHJ
			}
		}
		break;

    default:                                    /* not used */
		// Issue a PifLog() to record that we have an error condition then
		// return an error to the sender.  This is a bad function code.
		PifLog (MODULE_SER_LOG, LOG_EVENT_SER_CAS_BAD_FUNCODE);
		ResMsgH.sReturn = CAS_COMERROR;
        //return;
        break;
    }
    sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESCASHIER), NULL, 0);
	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "usFunCode = 0x%x, usSeqNo = 0x%x, sSerSendStatus = %d", pReqMsgH->usFunCode, pReqMsgH->usSeqNo, sSerSendStatus);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}
}

/*===== END OF SOURCE =====*/

