/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ISP Server Module, Cashier Functions Source File                        
* Category    : ISP Server Module, US Hospitality Model
* Program Name: ISPCAS.C                                            
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
*           IspRecvCas();       Entry point of cashier function handling
*
*
* --------------------------------------------------------------------------
* Update Histories                                                         
* Date     :Ver.Rev.:   Name    : Description
* Jun-26-92:00.00.01:H.Yamaguchi: initial                                   
* May-07-96:03.01.05:T.Nakahata : Add Cashier Ind/All Terminal (Un)Lock
* Aug-06-98:03.03.00:M.Ozawa    : Enhanced to V3.3 Cashier Status
* Dec-14-99:01.00.00:hrkato     : Saratoga
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
#include	<regstrct.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <para.h>
#include	<pararam.h>

#include    <csttl.h>
#include    <csstbttl.h>
#include    <cscas.h>
#include    <csstbcas.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include	<transact.h>
#include    <isp.h>
#include    "ispcom.h"
#include    "ispext.h"
#include    <appllog.h>
#include    <csstbstb.h>
#include	<ej.h>
#include	<csstbej.h>

/*
*===========================================================================
** Synopsis:    VOID    IspRecvCas(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function executes for cashier function request.
*===========================================================================
*/
VOID    IspRecvCas(VOID)
{
    CLIREQCASHIER   *pReqMsgH = (VOID *)IspRcvBuf.auchData;
	CLIRESCASHIER   ResMsgH = {0};
    CASIF           Casif = {0};

	//----------------------------------------------------------------------------------
	// Asserts for invariant conditions needed for successful use of this function

		NHPOS_ASSERT(sizeof(IspRcvBuf.auchData) >= sizeof(CLIREQCASHIER));

        // test for IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESCASHIER)];
		NHPOS_ASSERT(sizeof(auchIspTmpBuf) >= sizeof(CLIREQDATA)+sizeof(CLIRESCASHIER)+2);
		NHPOS_ASSERT(sizeof(auchIspTmpBuf) >= sizeof(CLIRESCASHIER)+2*CAS_NUMBER_OF_MAX_CASHIER+2);

		// Asserts for array data types are the same for memcpy, memset, _tcsncpy, _tcsnset, etc
		NHPOS_ASSERT(sizeof(ResMsgH.auchCasName[0]) == sizeof(pReqMsgH->auchCasName[0]));
		NHPOS_ASSERT(sizeof(Casif.fbCashierStatus[CAS_CASHIERSTATUS_0]) == sizeof(pReqMsgH->fbStatus[0]));
	//-----------------------------------------------------------------------------------

    ResMsgH.usFunCode     = pReqMsgH->usFunCode;
    ResMsgH.sResCode      = STUB_SUCCESS;
    ResMsgH.sReturn       = ISP_SUCCESS;
    ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.ulCashierNo   = pReqMsgH->ulCashierNo;
    ResMsgH.ulSecretNo    = pReqMsgH->ulSecretNo;

    _tcsncpy(ResMsgH.auchCasName, pReqMsgH->auchCasName, CLI_CASHIERNAME);
    memcpy(ResMsgH.fbStatus, pReqMsgH->fbStatus, CLI_CASHIERSTATUS);

    ResMsgH.usStartGCN    = pReqMsgH->usStartGCN;
    ResMsgH.usEndGCN      = pReqMsgH->usEndGCN;
    ResMsgH.uchChgTipRate = pReqMsgH->uchChgTipRate;
    ResMsgH.uchTeamNo     = pReqMsgH->uchTeamNo;
    ResMsgH.uchTerminal   = pReqMsgH->uchTerminal;
	ResMsgH.usSupervisorID   = pReqMsgH->usSupervisorID;
	ResMsgH.usCtrlStrKickoff = pReqMsgH->usCtrlStrKickoff;
	ResMsgH.usStartupWindow  = pReqMsgH->usStartupWindow;
	ResMsgH.ulGroupAssociations = pReqMsgH->ulGroupAssociations;

    Casif.ulCashierNo      = pReqMsgH->ulCashierNo;
    Casif.ulCashierSecret  = pReqMsgH->ulSecretNo;
    Casif.usUniqueAddress  = (USHORT)IspRcvBuf.auchFaddr[CLI_POS_UA];

    _tcsncpy(Casif.auchCashierName, pReqMsgH->auchCasName, CLI_CASHIERNAME);
    memcpy(Casif.fbCashierStatus, pReqMsgH->fbStatus, CLI_CASHIERSTATUS);

    Casif.usGstCheckStartNo = pReqMsgH->usStartGCN;
    Casif.usGstCheckEndNo   = pReqMsgH->usEndGCN;
    Casif.uchChgTipRate     = pReqMsgH->uchChgTipRate;
    Casif.uchTeamNo         = pReqMsgH->uchTeamNo;
    Casif.uchTerminal       = pReqMsgH->uchTerminal;
	Casif.usSupervisorID   = pReqMsgH->usSupervisorID;
	Casif.usCtrlStrKickoff = pReqMsgH->usCtrlStrKickoff;
	Casif.usStartupWindow  = pReqMsgH->usStartupWindow;
	Casif.ulGroupAssociations = pReqMsgH->ulGroupAssociations;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {
    case CLI_FCCASASSIGN:    /*  Add or replace cashier para file */
        ResMsgH.sReturn  = SerCasAssign(&Casif);
        break;

    case CLI_FCCASDELETE:   /* Delete a cashier */
        ResMsgH.sReturn = IspCheckTtlCas(Casif.ulCashierNo);    /* Check total */
        if ( ISP_SUCCESS == ResMsgH.sReturn ) {
            ResMsgH.sReturn = SerCasDelete(Casif.ulCashierNo);   /* Check issued report */
        }
        break;

    case CLI_FCCASINDCLOSE:  /*  Force to close a designate cashier */
        ResMsgH.sReturn = SerCasClose(Casif.ulCashierNo);
        break;

    case CLI_FCCASCLRSCODE:  /*  Clear secret code */
        ResMsgH.sReturn = SerCasSecretClr(Casif.ulCashierNo);
        break;

    case CLI_FCCASALLIDREAD : /* Cashier All Id Read */
        IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESCASHIER)];
        ResMsgH.sReturn = SerCasAllIdRead(sizeof(ULONG)*CAS_NUMBER_OF_MAX_CASHIER, (ULONG *)IspResp.pSavBuff->auchData );
        if (0 <= ResMsgH.sReturn) {
            IspResp.pSavBuff->usDataLen = sizeof(ULONG)*ResMsgH.sReturn;
            ResMsgH.sResCode = STUB_MULTI_SEND;
        } else {
            IspResp.pSavBuff->usDataLen = 0;
        }
        break;

    case CLI_FCCASINDREAD :   /* Cashier individual Read */
        ResMsgH.sReturn = SerCasIndRead(&Casif);
        
        if (0 <= ResMsgH.sReturn) {
            ResMsgH.ulSecretNo   = Casif.ulCashierSecret ;
            _tcsncpy(ResMsgH.auchCasName, Casif.auchCashierName, CLI_CASHIERNAME);
            memcpy(ResMsgH.fbStatus, Casif.fbCashierStatus, CLI_CASHIERSTATUS);
            ResMsgH.usStartGCN    = Casif.usGstCheckStartNo;
            ResMsgH.usEndGCN      = Casif.usGstCheckEndNo;
            ResMsgH.uchChgTipRate = Casif.uchChgTipRate;
            ResMsgH.uchTeamNo     = Casif.uchTeamNo;
            ResMsgH.uchTerminal   = Casif.uchTerminal;
			ResMsgH.usSupervisorID = Casif.usSupervisorID;
			ResMsgH.usCtrlStrKickoff = Casif.usCtrlStrKickoff;
			ResMsgH.usStartupWindow = Casif.usStartupWindow;
			ResMsgH.ulGroupAssociations = Casif.ulGroupAssociations;
        }
        break;
 
    case CLI_FCCASINDLOCK :   /* Cashier individual Lock */
        if (0 == ( fsIspLockedFC & ISP_LOCK_INDCASHIER) ) {
            ResMsgH.sReturn = SerCasIndLock(Casif.ulCashierNo);
            if ( CAS_PERFORM  == ResMsgH.sReturn ) {
                fsIspLockedFC |= ISP_LOCK_INDCASHIER;   /* Set INDCASHIER */
                ulIspLockedCasNO  = Casif.ulCashierNo;  /* Save Cashier ID */
            }
        } else {
            ResMsgH.sReturn = CAS_DIFF_NO;  /* if alredy locked, then error */
        }
        break;
   
    case CLI_FCCASINDUNLOCK : /* Cashier individual UnLock */
        if ( fsIspLockedFC & ISP_LOCK_INDCASHIER ) {
            ResMsgH.sReturn = IspCleanUpLockCas(ISP_LOCK_INDCASHIER);
        } else {
            ResMsgH.sReturn = CAS_DIFF_NO;  /* if unlocked, then error */
        }
        break;
    
    case CLI_FCCASALLLOCK :   /* Cashier All Lock */
        if ( 0 == ( fsIspLockedFC & ISP_LOCK_ALLCASHIER) ) {

            if (CAS_PERFORM == ( ResMsgH.sReturn = SerCasAllLock() ) ) {  /* cashier all lock */
                fsIspLockedFC |= ISP_LOCK_ALLCASHIER;  /* Set ALLCASHIER flag */
            }
        
        } else {
            ResMsgH.sReturn = CAS_ALREADY_LOCK;  /* if already locked, then error */
        }
        break;
    
    case CLI_FCCASALLUNLOCK : /* Cashier All UnLock */
        if ( fsIspLockedFC & ISP_LOCK_ALLCASHIER ) {
            ResMsgH.sReturn = IspCleanUpLockCas(ISP_LOCK_ALLCASHIER);
        }
        break;
    
    case CLI_FCCASCHKSIGNIN : /* Cashier Check exist sign-in */
        IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESCASHIER)];
		memset (IspResp.pSavBuff->auchData, 0, sizeof(ULONG) * STD_NUM_OF_TERMINALS * 2);
        ResMsgH.sReturn = SerCasChkSignIn( (ULONG *) IspResp.pSavBuff->auchData );

        if (CAS_DURING_SIGNIN == ResMsgH.sReturn) {
            IspResp.pSavBuff->usDataLen = sizeof(ULONG) * STD_NUM_OF_TERMINALS * 2; // CAS_TERMINAL_NO_EXTENDED ;
            ResMsgH.sResCode = STUB_MULTI_SEND;
        } else {
            IspResp.pSavBuff->usDataLen = 0;
        }
        break;

    case CLI_FCCASINDTMLOCK:    /* terminal lock R3.1 */
        ResMsgH.sReturn  = SerCasIndTermLock( pReqMsgH->usTerminalNo );
        break;

    case CLI_FCCASINDTMUNLOCK:  /* terminal unlock R3.1 */
        ResMsgH.sReturn  = SerCasIndTermUnLock( pReqMsgH->usTerminalNo );
        break;

    case CLI_FCCASALLTMLOCK:    /* terminal all lock R3.1 */
        ResMsgH.sReturn  = SerCasAllTermLock();
        break;

    case CLI_FCCASALLTMUNLOCK:  /* terminal all unlock R3.1 */
        SerCasAllTermUnLock();
        ResMsgH.sReturn  = CAS_PERFORM;
        break;

	case CLI_FCCASDELAYBALON:
		//This function tells the terminal that they are in delay balance
		//mode
		uchMaintDelayBalanceFlag |= MAINT_DBON;
		ResMsgH.sReturn = SerCasDelayedBalance(Casif.uchTerminal, MAINT_DBON);

		if(ResMsgH.sReturn == SUCCESS)
		{
			//set the flag in Para to signify that we are
			//in delay balance mode, and then save the data. JHHJ
			PifSaveFarData();
			PifLog(MODULE_ISP_LOG, LOG_EVENT_DB_FLAG_ON_1);
			EJWriteDelayBalance(uchMaintDelayBalanceFlag);
		}
		break;

    case CLI_FCCASOPENPICLOA:   /*  Sign-in for pickup loan,    Saratoga */
        if (0 == (fsIspLockedFC & ISP_LOCK_PICKUPCASHIER)) {
            if (CAS_PERFORM ==  SerCasIndTermLock(CliReadUAddr())) {
                
                ResMsgH.sReturn = CliCasOpenPickupLoan(&Casif);
                if (CAS_PERFORM == ResMsgH.sReturn) {
                    if (Casif.fbCashierStatus[CAS_CASHIERSTATUS_0] & CAS_TRAINING_CASHIER) {
                        CliCasClosePickupLoan(&Casif);
                        ResMsgH.sReturn = CAS_DIFF_NO;
                        SerCasIndTermUnLock(CliReadUAddr());    /* 09/28/00 */
                    } else {
						ITEMREGMODEIN   ItemModeIn = {0};
						ITEMTRANSOPEN   ItemTransOpen = {0};

                        /*- build structure to make interface with transaction module -----*/
                        ItemModeIn.uchMajorClass = CLASS_ITEMMODEIN;
                        ItemModeIn.uchMinorClass = CLASS_SUPMODEIN;
                        TrnModeIn(&ItemModeIn);

                        /* --- open item, cons. post rec. file --- */
                        TrnICPOpenFile();

                        /* ----- send transaction open data ----- */
                        ItemTransOpen.uchMajorClass = CLASS_ITEMTRANSOPEN;
                        ItemTransOpen.uchMinorClass = CLASS_OPEN_LOAN;
                        ItemTransOpen.ulCashierID   = Casif.ulCashierNo;

                        /* ----- slip condition set ----- */
                        TrnOpen(&ItemTransOpen);  /* open transaction */

                        memcpy(ResMsgH.fbStatus, Casif.fbCashierStatus, CLI_CASHIERSTATUS);
                        ResMsgH.ulSecretNo = Casif.ulCashierSecret;
                        _tcsncpy(ResMsgH.auchCasName, Casif.auchCashierName, CLI_CASHIERNAME);
                        fsIspLockedFC |= ISP_LOCK_PICKUPCASHIER;
                        ulIspPickupCasNO   = Casif.ulCashierNo;
                        usIspTransNoIHav   = 0;
                        usIspPickupFunCode = 0;
                        lIspTotal   = 0L;
                        sIspCounter = 0;
                    }
                } else {
                    SerCasIndTermUnLock(CliReadUAddr());    /* 09/28/00 */
                }
            } else {
                ResMsgH.sReturn = CAS_DURING_SIGNIN;
            }

        } else {
            ResMsgH.sReturn = CAS_DURING_SIGNIN;
        }
        break;

    case CLI_FCCASCLOSEPICLOA:    /*  Sign-out for pickup loan */
        if (fsIspLockedFC & ISP_LOCK_PICKUPCASHIER) {
            ResMsgH.sReturn = IspCleanUpPickupCas(Casif.ulCashierNo);
        } else {
            ResMsgH.sReturn = CAS_NO_SIGNIN;  /* if not signin, then error */
        }
        break;



    default:                 /* not used */
		PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_RECV_INVALID_FUN);
        return;
        break;
    }

    if (STUB_MULTI_SEND == ResMsgH.sResCode) {
        IspSendMultiple((CLIRESCOM *)&ResMsgH, sizeof(CLIRESCASHIER));
    } else {
        IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESCASHIER), NULL, 0);
    }
}

/*===== END OF SOURCE =====*/