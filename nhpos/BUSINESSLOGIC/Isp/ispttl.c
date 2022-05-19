/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ISP Server Module, Total Functions Source File
* Category    : ISP Server Module, US Hospitality Model
* Program Name: ISPTTL.C                                            
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
* Abstract: The provided function names are as follows:  
*
*       SerRecvTtl();           Entry point of Total function handling  R3.0
*       
*       IspRecvTtlReset();      Total reset EOD, PTD function
*       IspRecvTtlResetCas();   Total reset of a cashier function
*       IspRecvTtlResetWai();   Total reset of a waiter function
*       IspRecvTtlRead();       Read various totals
*       IspRecvTtlReadPlu();    Read plural PLU totals                  R3.0
*       IspRecvTtlGetLen();     Read Total structure length
*       IspRecvTtlResetIndFin() Total reset of a individual financial   R3.1
*
* --------------------------------------------------------------------------
* Update Histories                                                         
* Date     :Ver.Rev.:   Name    : Description
* Jun-26-92:00.00.01:H.Yamaguchi: initial                                   
* May-17-94:02.05.00:Yoshiko.Jim: add CLI_FCTTLWAITTLCHK, CLI_FCTTLCASTTLCHK
* Jun-01-95:02.05.04:M.Suzuki   : add Coupon R3.0
* Jun-15-95:03.00.00:hkato      : R3.0
* Oct-31-95:03.00.10:hkato      : R3.0, Clear Consecutive Co. from PC i/F.
* Nov-14-95:03.01.00:T.Nakahata : R3.1 Initial
*       Add Service Time Total (Daily/PTD) and Individual Financial (Daily)
* Aug-11-99:03.05.00:M.Teraki   : Remove WAITER_MODEL
* Aug-13-99:03.05.00:M.Teraki   : Merge STORE_RECALL_MODEL/GUEST_CHECK_MODEL
* Dec-07-99:01.00.00:hrkato     : Saratoga
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
#include    <math.h>
#include    <stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <appllog.h>
#include    <rfl.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <csttl.h>
#include    <csstbttl.h>
#include    <cscas.h>
#include    <csstbcas.h>
#include    <csgcs.h>
#include    <csstbgcf.h>
#include    <csstbstb.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <maint.h>
#include    <isp.h>
#include    "ispcom.h"
#include    "ispext.h"
#include    "isppick.h"
#include    <csstbbak.h>
#include "ttl.h"
#include "PluTtlD.h"

SHORT   IspRecvTtlPLUOptimize(CLIREQRESETTL *pReqMsgH);

#if defined(DCURRENCY_LONGLONG)
//#define DCURRENCY_LONGLONG_REMOTE
SHORT  IspRecvTtlConvertTo_32 (void *pTotal, void *pTotal_32);
#endif

/*
*===========================================================================
** Synopsis:    SHORT    IspMakeUpdateFormat(UCHAR  *pReqMsg, ITEMAFFECTION *pAffect)
*
*     Input:    pReqMsg    - request from Isp
*               pResTtl    - makeup message pointer
*
** Return:      ISP_SUCCESS:             All data received
*               ISP_ERR_INVALID_MESSAGE: Data error 
*
** Description:
*   This function executes for Total update function request.   Saratoga
*===========================================================================
*/
static SHORT   IspMakeUpdateFormat(UCHAR *pReqMsg, ITEMAFFECTION *pAffect)
{
	DCURRENCY           lNative = 0L;
	LONG                lSign = 1L;
	ISPPICKUP           *pPickData = (VOID *)pReqMsg;

	pAffect->uchMajorClass = CLASS_ITEMAFFECTION;

    if (0 == usIspPickupFunCode) {
        usIspPickupFunCode = pPickData->usFunc;
    } else {
        if (usIspPickupFunCode != pPickData->usFunc) {
            return(ISP_ERR_INVALID_MESSAGE);
        }
    }

    pAffect->uchTotalID = pPickData->uchMedia;
    if (pPickData->uchOpeMode == ISP_PICKUP_VOID_OP) {
        lSign = -1L;
    }

    if (pPickData->usFunc == ISP_FUNC_LOAN) {
        pAffect->uchMinorClass = CLASS_LOANAFFECT;
        pAffect->lAmount       = pPickData->ulTotal * lSign;
    } else {
        pAffect->uchMinorClass = CLASS_PICKAFFECT;
        pAffect->lAmount       = pPickData->ulTotal * -1L * lSign;
    }

    switch (pAffect->uchTotalID) {
    case    CLASS_MAINTFOREIGN1:
    case    CLASS_MAINTFOREIGN2:
    case    CLASS_MAINTFOREIGN3:
    case    CLASS_MAINTFOREIGN4:
    case    CLASS_MAINTFOREIGN5:
    case    CLASS_MAINTFOREIGN6:
    case    CLASS_MAINTFOREIGN7:
    case    CLASS_MAINTFOREIGN8:
        IspLoanPickFCAmount(&lNative, pAffect->lAmount, pAffect->uchTotalID);
        pAffect->lService[0] = lNative;
        break;
    }

    return (ISP_SUCCESS);
}

/*
*============================================================================
**Synopsis:     SHORT  IspRecvTtlSetLog(VOID *pTotal, USHORT usFunCode)
*
*    Input:     VOID    *pTOTAL
*               USHORT  usFunCode
*
*   Output:     Nothing
*    InOut:     Nothing
*
** Return:      Nothing
*
** Description  This function sets up the reset log data for the given
*               type of reset being done by isp, V3.3
*
*============================================================================
*/                                     
static VOID  IspRecvTtlSetLog(VOID *pTotal, USHORT usFunCode)
{
    TTLREGFIN *pRegTotal;
    TTLHOURLY *pHourTotal;
    TTLDEPT *pDeptTotal;
    TTLPLU  *pPLUTotal;
    TTLCPN  *pCpnTotal;
    TTLCASHIER *pCasTotal;
    TTLSERTIME *pTimeTotal;

    pRegTotal = (TTLREGFIN *)pTotal;
    switch(pRegTotal->uchMajorClass) {
    case CLASS_TTLREGFIN:          /* Read Financial Total File */
        IspResetLog.uchAction = 23;
        if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLREADTL) {
            IspResetLog.uchReset = ISP_RESET_READ;
        } else if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLRESETTL) {
            IspResetLog.uchReset = ISP_RESET_RESET;
        } else {
            IspResetLog.uchReset = ISP_RESET_ISSUE;
        }
        pRegTotal = (TTLREGFIN *)pTotal;
        if (pRegTotal->uchMinorClass == CLASS_TTLCURDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else if (pRegTotal->uchMinorClass == CLASS_TTLSAVDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        } else if (pRegTotal->uchMinorClass == CLASS_TTLCURPTD) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
		} else if (pRegTotal->uchMinorClass == CLASS_TTLSAVPTD) {
			IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
			IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
		} else {
			// a default just in case. This should not happen.
			IspResetLog.uchDayPtd = 0;
			IspResetLog.uchCurSave = 0;
		}
		IspResetLog.ulNumber = 0;
        break;

    case CLASS_TTLHOURLY:          /* Read Hourly Total File */
        IspResetLog.uchAction = 24;
        if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLREADTL) {
            IspResetLog.uchReset = ISP_RESET_READ;
        } else if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLRESETTL) {
            IspResetLog.uchReset = ISP_RESET_RESET;
        } else {
            IspResetLog.uchReset = ISP_RESET_ISSUE;
        }
        pHourTotal = (TTLHOURLY *)pTotal;
        if (pHourTotal->uchMinorClass == CLASS_TTLCURDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else if (pHourTotal->uchMinorClass == CLASS_TTLSAVDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        } else if (pHourTotal->uchMinorClass == CLASS_TTLCURPTD) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        }
        IspResetLog.ulNumber = 0;
        break;

   case CLASS_TTLDEPT:                /* Read Dept Total File */
        IspResetLog.uchAction = 26;
        if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLREADTL) {
            IspResetLog.uchReset = ISP_RESET_READ;
        } else if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLRESETTL) {
            IspResetLog.uchReset = ISP_RESET_RESET;
        } else {
            IspResetLog.uchReset = ISP_RESET_ISSUE;
        }
        pDeptTotal = (TTLDEPT *)pTotal;
        if (pDeptTotal->uchMinorClass == CLASS_TTLCURDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else if (pDeptTotal->uchMinorClass == CLASS_TTLSAVDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        } else if (pDeptTotal->uchMinorClass == CLASS_TTLCURPTD) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        }
        IspResetLog.ulNumber = 0;
        break;
   
    case CLASS_TTLPLU:              /* Reset PLU Total File */
        if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLREADTL) {
            IspResetLog.uchAction = 29;
            IspResetLog.uchReset = ISP_RESET_READ;
        } else if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLRESETTL) {
            IspResetLog.uchAction = 39;
            IspResetLog.uchReset = ISP_RESET_RESET;
        } else {
            IspResetLog.uchAction = 39;
            IspResetLog.uchReset = ISP_RESET_ISSUE;
        }
        pPLUTotal = (TTLPLU *)pTotal;
        if (pPLUTotal->uchMinorClass == CLASS_TTLCURDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else if (pPLUTotal->uchMinorClass == CLASS_TTLSAVDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        } else if (pPLUTotal->uchMinorClass == CLASS_TTLCURPTD) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        }
        IspResetLog.ulNumber = 0;
        break;
    
    case CLASS_TTLCPN:               /* Reset Coupon Total File, R3.0 */
        if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLREADTL) {
            IspResetLog.uchAction = 30;
            IspResetLog.uchReset = ISP_RESET_READ;
        } else if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLRESETTL) {
            IspResetLog.uchAction = 40;
            IspResetLog.uchReset = ISP_RESET_RESET;
        } else {
            IspResetLog.uchAction = 40;
            IspResetLog.uchReset = ISP_RESET_ISSUE;
        }
        pCpnTotal = (TTLCPN *)pTotal;
        if (pCpnTotal->uchMinorClass == CLASS_TTLCURDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else if (pCpnTotal->uchMinorClass == CLASS_TTLSAVDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        } else if (pCpnTotal->uchMinorClass == CLASS_TTLCURPTD) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        }
        IspResetLog.ulNumber = 0;
        break;
    
    case CLASS_TTLCASHIER:          /* Reset Cashier Total File */
        if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLREADTL) {
            IspResetLog.uchAction = 21;
            IspResetLog.uchReset = ISP_RESET_READ;
        } else if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLRESETTL) {
            IspResetLog.uchAction = 19;
            IspResetLog.uchReset = ISP_RESET_RESET;
        } else {
            IspResetLog.uchAction = 19;
            IspResetLog.uchReset = ISP_RESET_ISSUE;
        }
        pCasTotal = (TTLCASHIER *)pTotal;
        if (pCasTotal->uchMinorClass == CLASS_TTLCURDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else if (pCasTotal->uchMinorClass == CLASS_TTLSAVDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        } else if (pCasTotal->uchMinorClass == CLASS_TTLCURPTD) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        }
        IspResetLog.ulNumber = pCasTotal->ulCashierNumber;
        break;

    case CLASS_TTLSERVICE:          /* Reset Service Time File */
        if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLREADTL) {
            IspResetLog.uchAction = 130;
            IspResetLog.uchReset = ISP_RESET_READ;
        } else if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLRESETTL) {
            IspResetLog.uchAction = 131;
            IspResetLog.uchReset = ISP_RESET_RESET;
        } else {
            IspResetLog.uchAction = 131;
            IspResetLog.uchReset = ISP_RESET_ISSUE;
        }
        pTimeTotal = (TTLSERTIME *)pTotal;
        if (pTimeTotal->uchMinorClass == CLASS_TTLCURDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else if (pTimeTotal->uchMinorClass == CLASS_TTLSAVDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        } else if (pTimeTotal->uchMinorClass == CLASS_TTLCURPTD) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        }
        break;

    case CLASS_TTLINDFIN:           /* Reset Individual Financial File */
        if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLREADTL) {
            IspResetLog.uchAction = 223;
            IspResetLog.uchReset = ISP_RESET_READ;
        } else if ((usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLRESETTL) {
            IspResetLog.uchAction = 233;
            IspResetLog.uchReset = ISP_RESET_RESET;
        } else {
            IspResetLog.uchAction = 233;
            IspResetLog.uchReset = ISP_RESET_ISSUE;
        }
        pRegTotal = (TTLREGFIN *)pTotal;
        if (pRegTotal->uchMinorClass == CLASS_TTLCURDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else if (pRegTotal->uchMinorClass == CLASS_TTLSAVDAY) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        } else if (pRegTotal->uchMinorClass == CLASS_TTLCURPTD) {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        } else {
            IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
            IspResetLog.uchCurSave = ISP_CURSAVE_SAVE;
        }
        IspResetLog.ulNumber = (ULONG)pRegTotal->usTerminalNumber;
        break;
        
    case CLASS_TTLEODRESET:         /* Reset EOD Total File */
        IspResetLog.uchAction = 99;
        IspResetLog.uchDayPtd = ISP_DAYPTD_DAY;
        IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        IspResetLog.uchReset = ISP_RESET_RESET;
        IspResetLog.ulNumber = 0;
        break;

    case CLASS_TTLPTDRESET:         /* Reset PTD Total File */
        IspResetLog.uchAction = 18;
        IspResetLog.uchDayPtd = ISP_DAYPTD_PTD;
        IspResetLog.uchCurSave = ISP_CURSAVE_CUR;
        IspResetLog.uchReset = ISP_RESET_RESET;
        IspResetLog.ulNumber = 0;
        break;

    default:
        IspResetLog.uchAction = 0;
        IspResetLog.uchDayPtd = 0;
        IspResetLog.uchCurSave = 0;
        IspResetLog.uchReset = 0;
        IspResetLog.ulNumber = 0;
        break;
    }
}

/*
*===========================================================================
** Synopsis:    SHORT   IspChekTtlUpdate(CLIREQTOTAL *pReqMsgH, CLIRESTOTAL *pResMsgH)
*
*     Input:    pReqMsgH    - request from satellite
*
** Return:      STUB_SUCCESS:       All data received
*               STUB_BUSY:          Busy
*
** Description:
*   This function executes for Total update function request.   Saratoga
*===========================================================================
*/
static SHORT   IspCheckTtlUpdate(CLIREQTOTAL *pReqMsgH, CLIRESTOTAL  *pResMsgH)
{
    USHORT          usTransNoHeHas;
    SHORT           sError;
	ITEMAFFECTION   Affect = {0};

    usTransNoHeHas = pReqMsgH->usTransNo;
    if (usTransNoHeHas == usIspTransNoIHav) {
        return(STUB_SUCCESS);                   /* unmatched transaction # */
    }

    sError = IspMakeUpdateFormat (IspResp.pSavBuff->auchData, &Affect);
    if (ISP_SUCCESS == sError) {
        sError = TrnItem (&Affect);	/* check storage full, V1.0.12 */
        if (sError == TRN_SUCCESS ) {
            usIspTransNoIHav = usTransNoHeHas;
            pResMsgH->usTransNo = usTransNoHeHas;
        	sIspCounter++;
        } else {
			sError = TTL_FAIL;
		}
    }

    return(sError);
}


/*
*===========================================================================
** Synopsis:    VOID    IspRecvTtl(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function executes for Total function request.
*===========================================================================
*/
VOID    IspRecvTtl(VOID)
{
    CLIREQRESETTL   *pReqRstH = (CLIREQRESETTL *)IspRcvBuf.auchData;
	CLIRESTOTAL     ResMsgH = {0};
    SHORT           sError = ISP_SUCCESS;

    ResMsgH.usFunCode = pReqRstH->usFunCode;
    ResMsgH.sResCode  = STUB_SUCCESS;
    ResMsgH.usSeqNo   = pReqRstH->usSeqNo & CLI_SEQ_CONT;

    switch(pReqRstH->usFunCode & CLI_RSTCONTCODE) {
    case CLI_FCTTLUPDATE:
        if (fsIspLockedFC & ISP_LOCK_PICKUPCASHIER) {
			CLIREQTOTAL     *pReqTtl = (CLIREQTOTAL *)IspRcvBuf.auchData;
            sError = IspCheckTtlUpdate(pReqTtl, &ResMsgH);
        } else {
            sError = CAS_NO_SIGNIN;  /* if not signin, then error */
        }
        break;

    case    CLI_FCTTLRESETTL:  /* Reset EOD or Store reset */
        sError = IspRecvTtlReset(pReqRstH);

        /*----- Reset Consecutive Number,  R3.0 -----*/
        if (CliParaMDCCheck(MDC_SUPER_ADR, ODD_MDC_BIT1)) {
			MaintResetSpcCo(SPCO_CONSEC_ADR);
			PifLog(MODULE_ISP_LOG, LOG_EVENT_UIC_RESET_CONSECUTIVE);
		}
        break;

    case    CLI_FCTTLRESETCAS: /* Reset a cashier */
        sError = IspRecvTtlResetCas(pReqRstH);
        break;

    case    CLI_FCTTLISSUERST:   /* Set issued flag */
        sError = SerTtlIssuedReset(&pReqRstH->uchMajorClass, pReqRstH->fIssued);
        break;

    case CLI_FCTTLREADTL :   /* TTL read */
        sError = IspRecvTtlRead(pReqRstH);
        if ( 0 <= sError ) {
            ResMsgH.sResCode = STUB_MULTI_SEND;
        }
        break;

    case CLI_FCTTLREADTLPLU :   /* TTL plural PLU read,  R3.0 */
        sError = IspRecvTtlReadPlu(pReqRstH);
        if ( 0 <= sError ) {
            ResMsgH.sResCode = STUB_MULTI_SEND;
        }
        break;

    case CLI_FCTTLREADTLPLUEX :   /* TTL plural PLU read,  Saratoga*/
        sError = IspRecvTtlReadPluEx(pReqRstH);
        if ( 0 <= sError ) {
            ResMsgH.sResCode = STUB_MULTI_SEND;
        }
        break;

    case CLI_FCTTLISSUECHK : /* TTL total issue check */
        sError = SerTtlIssuedCheck(&pReqRstH->uchMajorClass, pReqRstH->fIssued);
        break;

    case CLI_FCTTLCASTTLCHK :                   /* TTL cashier total check  */
        if (SerTtlCasTotalCheck() == TTL_DELCASWAI) {
            SerChangeInqStat();                 /* Execute AC 85            */
        }
        break;

    /* ===== New Functions (Release 3.1) BEGIN ===== */

    case CLI_FCTTLRESETINDFIN:                  /*  TTL Individual Fin. Reset */
        sError = IspRecvTtlResetIndFin(pReqRstH);
        break;

    /* ===== New Functions (Release 3.1) END ===== */

    case    CLI_FCTTLDELETETTL:  /* delete database */
        sError = IspRecvTtlDelete(pReqRstH);
        break;

    case    CLI_FCTTLPLUOPTIMIZE:  /* optimize database */
        sError = IspRecvTtlPLUOptimize(pReqRstH);
        break;

	case CLI_FCTTLDELAYBALUPD:
		sError = SerTtlTumUpdateDelayedBalance();
		break;

	case CLI_FCTTLCLOSESAVTTL:
		TtlCloseSavFile();
		break;

    default:                                  /* not used */
		PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_RECTTL_INV_FUN);
        return; 
        break;
    }

    ResMsgH.sReturn  = sError;     /* Copy error code */

	if (ResMsgH.sReturn < 0)
	{
		char  xBuff[128];

		// issue non assert logs to indicate the error since some errors can be ignored.
		// lets not add to the debugging programmers burden unnecessarily.
		sprintf (xBuff, "==NOTE: IspRecvTtl() usFuncCode 0x%x  uchMajorClass %d  uchMinorClass %d  fIssued 0x%x",
			(pReqRstH->usFunCode & CLI_RSTCONTCODE), pReqRstH->uchMajorClass, pReqRstH->uchMinorClass, pReqRstH->fIssued);
		NHPOS_NONASSERT_TEXT(xBuff);
		sprintf (xBuff, "                      sReturn %d  sResCode %d  usTransNo %d",
			ResMsgH.sReturn, ResMsgH.sResCode, ResMsgH.usTransNo);
		NHPOS_NONASSERT_TEXT(xBuff);

		PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_TOTAL_RQST_FAIL);
		PifLog(MODULE_DATA_VALUE(MODULE_ISP_LOG), (USHORT)(pReqRstH->usFunCode & CLI_RSTCONTCODE));
		PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(ResMsgH.sReturn));
	}

    if (STUB_MULTI_SEND == ResMsgH.sResCode) {
        IspSendMultiple((CLIRESCOM *)&ResMsgH, sizeof(CLIRESTOTAL));
    } else {
        IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESTOTAL), NULL, 0);
    }
}


/*
*=====================================================================================
**   Synopsis:  VOID    IspLoanPickFCAmount(LONG *lNative, LONG lAmount, UCHAR uchTotalID)
*
*       Input:  *pData
*      Output:  Nothing
*
**  Return:     SUCCESS
*
**  Description: Loan/Pickup Foreign Amount Caluculation.               Saratoga
======================================================================================
*/
VOID    IspLoanPickFCAmount(DCURRENCY *lNative, DCURRENCY lAmount, UCHAR uchTotalID)
{
	PARAMDC         ParaMDC = {0};
    PARACURRENCYTBL ParaFCRate = {0};
    UCHAR           uchFCType;

    ParaFCRate.uchMajorClass = CLASS_PARACURRENCYTBL;
    ParaMDC.uchMajorClass = CLASS_PARAMDC;

    switch (uchTotalID) {
    case CLASS_MAINTFOREIGN1:
        ParaFCRate.uchAddress = CNV_NO1_ADR;
        uchFCType = RND_FOREIGN1_1_ADR;
        ParaMDC.usAddress = MDC_FC1_ADR;
        break;

    case CLASS_MAINTFOREIGN2:
        ParaFCRate.uchAddress = CNV_NO2_ADR;
        uchFCType = RND_FOREIGN2_1_ADR;
        ParaMDC.usAddress = MDC_FC2_ADR;
        break;

    case CLASS_MAINTFOREIGN3:
        ParaFCRate.uchAddress = CNV_NO3_ADR;
        uchFCType = RND_FOREIGN3_1_ADR;
        ParaMDC.usAddress = MDC_FC3_ADR;
        break;

    case CLASS_MAINTFOREIGN4:
        ParaFCRate.uchAddress = CNV_NO4_ADR;
        uchFCType = RND_FOREIGN4_1_ADR;
        ParaMDC.usAddress = MDC_FC4_ADR;
        break;

    case CLASS_MAINTFOREIGN5:
        ParaFCRate.uchAddress = CNV_NO5_ADR;
        uchFCType = RND_FOREIGN5_1_ADR;
        ParaMDC.usAddress = MDC_FC5_ADR;
        break;

    case CLASS_MAINTFOREIGN6:
        ParaFCRate.uchAddress = CNV_NO6_ADR;
        uchFCType = RND_FOREIGN6_1_ADR;
        ParaMDC.usAddress = MDC_FC6_ADR;
        break;

    case CLASS_MAINTFOREIGN7:
        ParaFCRate.uchAddress = CNV_NO7_ADR;
        uchFCType = RND_FOREIGN7_1_ADR;
        ParaMDC.usAddress = MDC_FC7_ADR;
        break;

    case CLASS_MAINTFOREIGN8:
        ParaFCRate.uchAddress = CNV_NO8_ADR;
        uchFCType = RND_FOREIGN8_1_ADR;
        ParaMDC.usAddress = MDC_FC8_ADR;
        break;

    default:
        break;
    }

    CliParaRead(&ParaMDC);
    CliParaRead(&ParaFCRate);

    if (ParaFCRate.ulForeignCurrency == 0L) {             /* check fc rate */
        return;
    }

    RflRateCalc2(lNative, lAmount, ParaFCRate.ulForeignCurrency, uchFCType);
}


/*
*===========================================================================
** Synopsis:    SHORT    IspRecvTtlReset(CLIREQRESETTL *pReqMsgH)
*     Input:    pReqMsgH    - request from ISP
*     Output:   nothing
*     InOut:    nothing
*
** Return:      STUB_SUCCESS:           Success
*               CAS_???:                Success or Error code
*               WAI_???:                Success or Error code
*               ISP_ERR_REQUEST_REST    Need to reset first
*
** Description:
*   This function executes for Total reset EOD, PTD function request.
*===========================================================================
*/
SHORT   IspRecvTtlReset(CLIREQRESETTL *pReqMsgH)
{
	USHORT   fsTmpLock = 0;
	SHORT    sError = STUB_SUCCESS;

    do { // do loop once with possible error breaks
        if ( 0 == ( fsIspLockedFC & ISP_LOCK_ALLCASHIER )) {
            if (ISP_SUCCESS == (sError = IspCheckFlexFileSize(FLEX_CAS_ADR))) {
                if (CAS_PERFORM == ( sError = SerCasAllLock() ) ) {  /* cashier all lock */
                    fsTmpLock |= ISP_LOCK_ALLCASHIER; /* Set Locked cashier */
                } else {
					char  xBuff[128];

					sprintf (xBuff, "==NOTE: IspRecvTtlReset() SerCasAllLock() sError = %d", sError);
					NHPOS_NONASSERT_TEXT(xBuff)

					PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_RSET_CASLOCK);
					PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sError));
                    break;      /* Error  break */
                }
            }
        } 

        if (((CLASS_TTLEODRESET == pReqMsgH->uchMajorClass) && (CliParaMDCCheck(MDC_EOD1_ADR, EVEN_MDC_BIT2))) ||
              ((CLASS_TTLPTDRESET == pReqMsgH->uchMajorClass) && (CliParaMDCCheck(MDC_PTD1_ADR, EVEN_MDC_BIT2)))) {

            if (ISP_SUCCESS == (sError = IspCheckFlexFileSize(FLEX_GC_ADR))) {
                if ( CliParaMDCCheck(MDC_EODPTD_ADR, EVEN_MDC_BIT0) )  {
                    /* No Check Exist GCF or not */
                } else {
                    if (GCF_SUCCESS != (sError = SerGusCheckExist() ) ) {
						char  xBuff[128];

						sprintf (xBuff, "==NOTE: IspRecvTtlReset() MDC 96 Bit D ISP_ERR_REQUEST_RESET SerGusCheckExist() sError = %d", sError);
						NHPOS_NONASSERT_TEXT(xBuff)

                        sError = ISP_ERR_REQUEST_RESET;
                        break;
                    }
                }
            }
        }
        /* set pc/if log, V3.3 */
        IspRecvTtlSetLog(&pReqMsgH->uchMajorClass, pReqMsgH->usFunCode);

        sError = SerTtlTotalReset(&pReqMsgH->uchMajorClass, pReqMsgH->usExeMDC);

        /* V3.3 */
        IspResetLog.uchError = (UCHAR)TtlConvertError(sError);
		if (sError < 0) {
			char  xBuff[128];

			sprintf (xBuff, "==NOTE: IspRecvTtlReset() SerTtlTotalReset() sError %d %d uchMajorClass %d  usExeMDC 0x%x", sError, IspResetLog.uchError, pReqMsgH->uchMajorClass, pReqMsgH->usExeMDC);
			NHPOS_ASSERT_TEXT((sError >= 0), xBuff)

			PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_RSET_FAIL);
			PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sError));
			PifLog(MODULE_LINE_NO(MODULE_ISP_LOG), (USHORT)__LINE__);
		}

        IspWriteResetLog();

		/* execute PLU Total Optimize at ISP function, 10/11/01 */
        if (CLASS_TTLEODRESET == pReqMsgH->uchMajorClass) {
			if (pReqMsgH->usExeMDC & TTL_BIT_PLU_EXE) {
	        	sError = SerTtlPLUOptimize(CLASS_TTLCURDAY);
				if (sError < 0) {
					char  xBuff[128];

					sprintf (xBuff, "==NOTE: IspRecvTtlReset() CLASS_TTLEODRESET - SerTtlPLUOptimize(CLASS_TTLCURDAY) sError = %d", sError);
					NHPOS_ASSERT_TEXT((sError >= 0), xBuff)
				}
	        }
	        sError = SerTtlPLUOptimize(CLASS_TTLCURPTD);
			if (sError < 0) {
				char  xBuff[128];

				sprintf (xBuff, "==NOTE: IspRecvTtlReset() CLASS_TTLEODRESET - SSerTtlPLUOptimize(CLASS_TTLCURPTD) sError = %d", sError);
				NHPOS_ASSERT_TEXT((sError >= 0), xBuff)
				PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_RSET_FAIL);
				PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sError));
				PifLog(MODULE_LINE_NO(MODULE_ISP_LOG), (USHORT)__LINE__);
			}
		}
        if (CLASS_TTLPTDRESET == pReqMsgH->uchMajorClass) {
			if (pReqMsgH->usExeMDC & TTL_BIT_PLU_EXE) {
	        	sError = SerTtlPLUOptimize(CLASS_TTLCURPTD);
				if (sError < 0) {
					char  xBuff[128];

					sprintf (xBuff, "==NOTE: IspRecvTtlReset() CLASS_TTLPTDRESET - SSerTtlPLUOptimize(CLASS_TTLCURPTD) sError = %d", sError);
					NHPOS_ASSERT_TEXT((sError >= 0), xBuff)
				}
	        }
	        sError = SerTtlPLUOptimize(CLASS_TTLCURDAY);
			if (sError < 0) {
				char  xBuff[128];

				sprintf (xBuff, "==NOTE: IspRecvTtlReset() CLASS_TTLPTDRESET - SSerTtlPLUOptimize(CLASS_TTLCURDAY) sError = %d", sError);
				NHPOS_ASSERT_TEXT((sError >= 0), xBuff)
				PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_RSET_FAIL);
				PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sError));
				PifLog(MODULE_LINE_NO(MODULE_ISP_LOG), (USHORT)__LINE__);
			}
		}
        if (CLASS_TTLPLU == pReqMsgH->uchMajorClass) {
        	if ((pReqMsgH->uchMinorClass == CLASS_TTLCURDAY) || (pReqMsgH->uchMinorClass == CLASS_TTLSAVDAY)) {
		        sError = SerTtlPLUOptimize(CLASS_TTLCURPTD);	/* optimize PTD at daily reset */
				if (sError < 0) {
					char  xBuff[128];

					sprintf (xBuff, "==NOTE: IspRecvTtlReset() CLASS_TTLPLU - SSerTtlPLUOptimize(CLASS_TTLCURPTD) sError = %d", sError);
					NHPOS_ASSERT_TEXT((sError >= 0), xBuff)
				}
		    }
        	if ((pReqMsgH->uchMinorClass == CLASS_TTLCURPTD) || (pReqMsgH->uchMinorClass == CLASS_TTLSAVPTD)) {
		        sError = SerTtlPLUOptimize(CLASS_TTLCURDAY);	/* optimize Daily at PTD reset */
				if (sError < 0) {
					char  xBuff[128];

					sprintf (xBuff, "==NOTE: IspRecvTtlReset() CLASS_TTLPLU - SSerTtlPLUOptimize(CLASS_TTLCURDAY) sError = %d", sError);
					NHPOS_ASSERT_TEXT((sError >= 0), xBuff)
					PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_RSET_FAIL);
					PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sError));
					PifLog(MODULE_LINE_NO(MODULE_ISP_LOG), (USHORT)__LINE__);
				}
		    }
		}
        break;
    } while (0);  // do loop once only

    if ( fsTmpLock & ISP_LOCK_ALLCASHIER ) {
        SerCasAllUnLock();                         /* Cashier unlock */
    }
    return (sError);
}


/*
*===========================================================================
** Synopsis:    SHORT    IspRecvTtlResetCas(CLIREQRESETTL *pReqMsgH)
*     Input:    pReqMsgH    - request from ISP
*     Output:   nothing
*     InOut:    nothing
*
** Return:      CAS_PERFORM:            Success
*               CAS_???:                Error 
*
** Description:
*   This function executes for Total reset, cashier function request.
*===========================================================================
*/
SHORT   IspRecvTtlResetCas(CLIREQRESETTL *pReqMsgH)
{
    SHORT    sError;

    if (0 != ( fsIspLockedFC & ISP_LOCK_INDCASHIER) || ( fsIspLockedFC & ISP_LOCK_ALLCASHIER) ) {

        /* set pc/if log, V3.3 */
        IspRecvTtlSetLog(&pReqMsgH->uchMajorClass, pReqMsgH->usFunCode);

        sError = SerTtlTotalReset(&pReqMsgH->uchMajorClass, pReqMsgH->usExeMDC);

		if (sError < 0) {
			PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_RSET_FAIL);
			PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sError));
			PifLog(MODULE_LINE_NO(MODULE_ISP_LOG), (USHORT)__LINE__);
		}

        /* V3.3 */
        IspResetLog.uchError = (UCHAR)TtlConvertError(sError);
        IspWriteResetLog();

    } else {
        sError = ISP_ERR_REQUEST_LOCK_CS; 
    }
    return (sError);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspRecvTtlRead(CLIREQRESETTL *pReqMsgH)
*     Input:    pReqMsgH    - request from ISP
*     Output:   nothing
*     InOut:    nothing
*
** Return:      STUB_SUCCESS:           Success
*               CAS_???:                Success or Error code
*               WAI_???:                Success or Error code
*
** Description:
*   This function executes for Total read all totals. 
*===========================================================================
*/
SHORT   IspRecvTtlRead(CLIREQRESETTL *pReqMsgH)
{
    CLIREQDATA      *pReqBuff;
    CLITTLCASWAI    *pTtl;
    SHORT           sError;
	SHORT           sRetLen = 0;

    IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESTOTAL)];
    pReqBuff = (CLIREQDATA *)((UCHAR *)pReqMsgH + sizeof(CLIREQRESETTL));

    IspResp.pSavBuff->usDataLen = 0;

    memcpy(IspResp.pSavBuff->auchData, pReqBuff->auchData, pReqBuff->usDataLen);
    
    pTtl = (CLITTLCASWAI *)IspResp.pSavBuff->auchData;

    /* set pc/if log, V3.3 */
    IspRecvTtlSetLog(pTtl, pReqMsgH->usFunCode);

#if defined(DCURRENCY_LONGLONG_REMOTE)
	{
		union {
			TTLREGFIN  x1;
			TTLHOURLY  x2;
		} u;

		memcpy (&u, pTtl, sizeof(u));
		if (u.x2.uchMajorClass == CLASS_TTLHOURLY) {
			TTLHOURLY_32  *pHourly = (TTLHOURLY_32 *)pTtl;
			u.x2.usBlockRead = pHourly->usBlockRead;
		}
		sError = SerTtlTotalIncrRead(&u); 
		if ( TTL_SUCCESS == sError ) {
			sRetLen = IspRecvTtlConvertTo_32 (&u, pTtl);
		}
	}
#else
		sError = SerTtlTotalIncrRead(pTtl); 
#endif
    
    /* V3.3 */
    IspResetLog.uchError = (UCHAR)TtlConvertError(sError);
    IspWriteResetLog();

    if ( TTL_SUCCESS == sError ) {
        sError = IspRecvTtlGetLen(pTtl->uchMajorClass);    /* Get length */
#if defined(DCURRENCY_LONGLONG_REMOTE)
		sError = sRetLen;
#endif
    
        if ( 0 < sError ) {
            IspResp.pSavBuff->usDataLen = (USHORT)sError;  /* Save length */
            sError = ISP_SUCCESS;
        }
    } 

    return (sError);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspRecvTtlReadPlu(CLIREQRESETTL *pReqMsgH)
*     Input:    pReqMsgH    - request from ISP
*     Output:   nothing
*     InOut:    nothing
*
** Return:      STUB_SUCCESS:           Success
*
** Description:
*   This function executes plural PLU Total read.       R3.0    Saratoga
*===========================================================================
*/
#if defined(DCURRENCY_LONGLONG_REMOTE)
#define X_TTLPLU       TTLPLU_32
#define X_TTLPLUEX     TTLPLUEX_32
#define X_TTLPLUTOTAL  TTLPLUTOTAL_32
#else
#define X_TTLPLU       TTLPLU
#define X_TTLPLUEX     TTLPLUEX
#define X_TTLPLUTOTAL  TTLPLUTOTAL
#endif
SHORT   IspRecvTtlReadPlu(CLIREQRESETTL *pReqMsgH)
{
    CLIREQDATA  *pReqBuff;
    X_TTLPLU    *pTtl;
    SHORT       sError, i;

    IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESTOTAL)];
    pReqBuff = (CLIREQDATA *)((UCHAR *)pReqMsgH + sizeof(CLIREQRESETTL));

    IspResp.pSavBuff->usDataLen = 0;

    memcpy(IspResp.pSavBuff->auchData, pReqBuff->auchData, pReqBuff->usDataLen);
    
    pTtl = (X_TTLPLU *)IspResp.pSavBuff->auchData;

    for (i = 0; i < CLI_PLU_MAX_NO; i++) {
#if defined(DCURRENCY_LONGLONG_REMOTE)
		TTLPLU pTtl64 = *(TTLPLU *)pTtl;
        if (FALSE == IspRecvTtlCheckPluNo(pTtl64.auchPLUNumber)) {   /* Saratoga */
            sError = ISP_SUCCESS;
            break;
        }
        if (((sError = SerTtlTotalRead(&pTtl64)) == TTL_SUCCESS)
             || (sError == TTL_NOTINFILE)) {
			IspRecvTtlConvertTo_32 (&pTtl64, pTtl);
			pTtl++;
            IspResp.pSavBuff->usDataLen += sizeof(X_TTLPLU);
            sError = ISP_SUCCESS;
        } 
#else
        if (FALSE == IspRecvTtlCheckPluNo(pTtl->auchPLUNumber)) {   /* Saratoga */
            sError = ISP_SUCCESS;
            break;
        }
        if (((sError = SerTtlTotalRead(pTtl++)) == TTL_SUCCESS) || (sError == TTL_NOTINFILE)) {
            IspResp.pSavBuff->usDataLen += sizeof(TTLPLU);
            sError = ISP_SUCCESS;
        } 
#endif
		else if (sError == TTL_NOTCLASS) {
			PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_DATA_ERR_01);
		}
    }
 
    return(sError);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspRecvTtlReadPluEx(CLIREQRESETTL *pReqMsgH)
*     Input:    pReqMsgH    - request from ISP
*     Output:   nothing
*     InOut:    nothing
*
** Return:      STUB_SUCCESS:           Success
*
** Description:
*   This function executes plural PLU Total read.       R3.0    Saratoga
*
*   See also SerRecvTtlReadPluEx() which is practically the same function that
*   is just using a different response buffer.
*===========================================================================
*/
SHORT   IspRecvTtlReadPluEx(CLIREQRESETTL *pReqMsgH)
{
    CLIREQDATA  *pReqBuff;
    X_TTLPLUEX  *pTtlEx;
    SHORT       sError, i, sTblId;

    IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESTOTAL)];
    pReqBuff = (CLIREQDATA *)((UCHAR *)pReqMsgH + sizeof(CLIREQRESETTL));

    IspResp.pSavBuff->usDataLen = 0;

    memcpy(IspResp.pSavBuff->auchData, pReqBuff->auchData, pReqBuff->usDataLen);
    
    pTtlEx = (X_TTLPLUEX *)IspResp.pSavBuff->auchData;
    
    switch(pTtlEx->uchMinorClass)
    {
    case CLASS_TTLCURDAY:
        sTblId = PLUTOTAL_ID_DAILY_CUR;
        break;
    case CLASS_TTLSAVDAY:
        sTblId = PLUTOTAL_ID_DAILY_SAV;
        break;
    case CLASS_TTLCURPTD:
        sTblId = PLUTOTAL_ID_PTD_CUR;
        break;
    case CLASS_TTLSAVPTD:
        sTblId = PLUTOTAL_ID_PTD_SAV;
        break;
    default:
		sTblId = 0;    // invalid table id to cause TtlPLUOpenRec() to fail.
		PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_RDTTLPLU_INV_DB);
        break;
    }

    if ((fsIspLockedFC & ISP_LOCK_PLUTTL) == 0) {;   /* check PLUTTL Lock */
		ULONG       ulPluRecNum;

        sError = TtlPLUOpenRec(sTblId, PLUTOTAL_SORT_PLUNO, &ulPluRecNum);
        if(sError == TTL_SUCCESS)
        {
            if(ulPluRecNum == 0) /* TOTAL RESET */
            {
                TtlPLUCloseRec();
                IspResp.pSavBuff->usDataLen = sizeof(X_TTLPLUEX);
                fsIspLockedFC &= ~ISP_LOCK_PLUTTL;   /* Reset PLUTTL Lock */
                return(ISP_SUCCESS);
            }
        }
		else {
			PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_DATA_ERR_05);
			PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sError));
		}

        if(sError == TTL_FAIL)
        {
            TtlPLUCloseRec();
            fsIspLockedFC &= ~ISP_LOCK_PLUTTL;   /* Reset PLUTTL Lock */
            return (LDT_ERR_ADR);
        }
    }
    
    memset(&pTtlEx->Plu[0], 0, sizeof(X_TTLPLUTOTAL) * CLI_PLU_EX_MAX_NO);

    for (i = 0; i < CLI_PLU_EX_MAX_NO; i++) {
		TTLPLU    Ttl = {0};

        sError = TtlPLUGetRec(&Ttl);
        if ((sError == TTL_SUCCESS) || (sError == TTL_EOF)) {

            _tcsncpy(pTtlEx->Plu[i].auchPLUNumber, Ttl.auchPLUNumber, NUM_PLU_LEN);
            pTtlEx->Plu[i].uchAdjectNo = Ttl.uchAdjectNo;
            pTtlEx->Plu[i].PLUTotal.lAmount = Ttl.PLUTotal.lAmount;
			pTtlEx->Plu[i].PLUTotal.lCounter = Ttl.PLUTotal.lCounter;
            
            if (i==0) {
                Ttl.uchMajorClass = pTtlEx->uchMajorClass;
                Ttl.uchMinorClass = pTtlEx->uchMinorClass;
                if (SerTtlTotalRead(&Ttl) == TTL_SUCCESS) { /* 09/28/00 */
                /* if ((sError = SerTtlTotalRead(&Ttl)) == TTL_SUCCESS) { */
                    pTtlEx->uchResetStatus = Ttl.uchResetStatus;
#if defined(DCURRENCY_LONGLONG_REMOTE)
                    pTtlEx->FromDate = *(N_DATE_32 *)&(Ttl.FromDate);
                    pTtlEx->ToDate = *(N_DATE_32 *)&(Ttl.ToDate);         //SR275
#else
                    pTtlEx->FromDate = Ttl.FromDate;
                    pTtlEx->ToDate = Ttl.ToDate;         //SR275
#endif
					pTtlEx->PLUAllTotal.lAmount = Ttl.PLUAllTotal.lAmount;
					pTtlEx->PLUAllTotal.lCounter = Ttl.PLUAllTotal.lCounter;
                }
				else if (sError == TTL_NOTCLASS) {
					PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_DATA_ERR_02);
				}
            }
            if (sError == TTL_EOF) {
                sError = ISP_SUCCESS;
                if (i != (CLI_PLU_EX_MAX_NO-1)) {   /* 09/28/00 */
                    TtlPLUCloseRec();
                    fsIspLockedFC &= ~ISP_LOCK_PLUTTL;   /* Set PLUTTL Lock */
                }
                break;
            } else {
                sError = ISP_SUCCESS;
                fsIspLockedFC |= ISP_LOCK_PLUTTL;   /* Reset PLUTTL Lock */
            }
        } else {
            if (i==0) { /* 09/28/00 */
                sError = ISP_SUCCESS; /* no more record */
            }
            TtlPLUCloseRec();
            fsIspLockedFC &= ~ISP_LOCK_PLUTTL;   /* Reset PLUTTL Lock */
            break;
        }
    }
    
    if (sError == ISP_SUCCESS) {
        IspResp.pSavBuff->usDataLen = sizeof(TTLPLUEX);
    }
    return(sError);
}

#undef  X_TTLPLU
#undef  X_TTLPLUEX

/*
*===========================================================================
** Synopsis:    SHORT     IspRecvTtlGetLen(UCHAR uchMajorClass);
*     Input:    uchMajorClass  - Major Total Class No
*     Output:   nothing          
*     InOut:    nothing
*
** Return:      nothing
*
** Description:  Read a Total structure length.
*===========================================================================
*/
SHORT  IspRecvTtlGetLen(UCHAR uchMajorClass)
{
    SHORT       sRet;

    switch ( uchMajorClass ) {

    case CLASS_TTLREGFIN :                      /* Register financial */
    case CLASS_TTLINDFIN :                      /* individual fin R3.1 */
        sRet = sizeof(TTLREGFIN);
        break;

    case CLASS_TTLHOURLY :                      /* Hourly activity    */   
		//When we are reading a total hourly length we are now
		//only reading one hour at a time, and it is always placed in the
		///0 index of the array.  there are 48 elements in the array, so 
		//we only need to read the first one and not the other 47. JHHJ 
        sRet = (sizeof(TTLHOURLY) - (sizeof(HOURLY) * 47));
        break;
    
    case CLASS_TTLDEPT :                        /* Department */
        sRet = sizeof(TTLDEPT);
        break;

    case CLASS_TTLPLU :                         /* PLU */ 
        sRet = sizeof(TTLPLU);
        break;
    
    case CLASS_TTLCASHIER :                     /* Cashier */ 
        sRet = sizeof(TTLCASHIER);
        break;

    case CLASS_TTLCPN :                         /* Coupon R3.0 */
        sRet = sizeof(TTLCPN);
        break;

    case CLASS_TTLSERVICE:                      /* service time R3.1 */
        sRet = sizeof(TTLSERTIME);
        break;
    
    default :                                   /* Error  */
        sRet = ISP_ERR_INVALID_MESSAGE;
        break;
    }

    return(sRet);
}

#if defined(DCURRENCY_LONGLONG_REMOTE)
// Following source is to perform conversion from 64 bit totals data to a 32 bit totals data
// to allow GenPOS to maintain data and process transactions as 64 bit totals while
// working with remote applications which are still expecting 32 bit totals data.
//
// The following functions provide the translation from internal 64 bit totals to external 32 bit totals.
//    Richard Chambers, May 1, 2016

#define Member64_Member32(t64,t32,x) t32->x = t64->x;
#define Long64_Long32(t64,t32,x) t32->x = (LONG)t64->x;
#define Total64_Total32x(t64,t32)  t32.lAmount = (LONG)t64.lAmount; t32.sCounter = (SHORT)t64.sCounter;
#define Ltotal64_Ltotal32x(t64,t32)  t32.lAmount = (LONG)t64.lAmount; t32.lCounter = t64.lCounter;
#define Total64_Total32(t64,t32,x) Total64_Total32x((t64)->x,(t32)->x)
#define Ltotal64_Ltotal32(t64,t32,x) Ltotal64_Ltotal32x((t64)->x,(t32)->x)
#define Tend64_Tend32(t64,t32,x) Total64_Total32x(t64->OnlineTotal,t32->Online)
#define NDate64_NDate32(t64,t32,x) (t32)->x = *(N_DATE_32 *)&( ( t64 )->x );

void Taxable64_Taxable32(TAXABLE *t64, TAXABLE_32 *t32)
{
	t32->mlTaxableAmount = t64->mlTaxableAmount;    /* Taxable Amount */
    t32->lTaxAmount = (LONG)t64->lTaxAmount;        /* Tax Amount */
    t32->lTaxExempt = (LONG)t64->lTaxExempt;        /* Tax Exempt */
    t32->lFSTaxExempt = (LONG)t64->lFSTaxExempt;    /* FoodStamp Tax Exempt */
}

SHORT  IspRecvTtlConvert_TTLREGFIN (TTLREGFIN *pTotal, TTLREGFIN_32 *pTotal_32)
{
	int i;

	if (pTotal->uchMajorClass != CLASS_TTLREGFIN &&
		pTotal->uchMajorClass != CLASS_TTLINDFIN)
		return ISP_ERR_INVALID_MESSAGE;

	memset (pTotal_32, 0, sizeof(TTLREGFIN_32));

	Member64_Member32(pTotal,pTotal_32,uchMajorClass);      /* Major Class Data definition */
    Member64_Member32(pTotal,pTotal_32,uchMinorClass);      /* Minor Class Data definition */
    Member64_Member32(pTotal,pTotal_32,usTerminalNumber);   /* terminal unique number. R3.1 */
    Member64_Member32(pTotal,pTotal_32,uchResetStatus);     /* Reset Report Status */
    NDate64_NDate32(pTotal,pTotal_32,FromDate);             /* Preiod From */
    NDate64_NDate32(pTotal,pTotal_32,ToDate);               /* Preiod To */
    Member64_Member32(pTotal,pTotal_32,CGGTotal);           /* Current Gross Group Total */
    Long64_Long32(pTotal,pTotal_32,lNetTotal);              /* Net Total, Release 3.1 */
	Taxable64_Taxable32(pTotal->Taxable + 0, pTotal_32->Taxable + 0);         /* Taxable Total 1 to 4, R3.0 */
	Taxable64_Taxable32(pTotal->Taxable + 1, pTotal_32->Taxable + 1);         /* Taxable Total 1 to 4, R3.0 */
	Taxable64_Taxable32(pTotal->Taxable + 2, pTotal_32->Taxable + 2);         /* Taxable Total 1 to 4, R3.0 */
	Taxable64_Taxable32(pTotal->Taxable + 3, pTotal_32->Taxable + 3);         /* Taxable Total 1 to 4, R3.0 */
    Member64_Member32(pTotal,pTotal_32,NonTaxable);          /* Non taxable Total */
    Long64_Long32(pTotal,pTotal_32,lTGGTotal);               /* Training Gross Group Total */
    Total64_Total32(pTotal,pTotal_32,DeclaredTips);          /* Declared Tips */
    Long64_Long32(pTotal,pTotal_32,lDGGtotal);               /* Daily Gross Group Total */
	Total64_Total32(pTotal,pTotal_32,PlusVoid);              /* Plus Void */
    Total64_Total32(pTotal,pTotal_32,PreselectVoid);         /* Preselect Void */
    Total64_Total32(pTotal,pTotal_32,TransactionReturn);     /* transaction return, CURQUAL_TRETURN, MDC_RETURNS_RTN1_ADR, MDC_RPTFIN46_ADR */
    Total64_Total32(pTotal,pTotal_32,TransactionExchng);     /* transaction return, CURQUAL_TRETURN, MDC_RETURNS_RTN2_ADR, MDC_RPTFIN46_ADR */
    Total64_Total32(pTotal,pTotal_32,TransactionRefund);     /* transaction return, CURQUAL_TRETURN, MDC_RETURNS_RTN3_ADR, MDC_RPTFIN46_ADR */
    Total64_Total32(pTotal,pTotal_32,ConsCoupon);            /* Consolidated Coupon */
    Total64_Total32(pTotal,pTotal_32,ItemDisc);              /* Item Discount */
    Total64_Total32(pTotal,pTotal_32,ModiDisc);              /* Modified Item Discount */

	Total64_Total32(pTotal,pTotal_32,ItemDiscExtra.TtlIAmount[0]);      /* Item discounts 3 through 6  */
	Total64_Total32(pTotal,pTotal_32,ItemDiscExtra.TtlIAmount[1]);      /* Item discounts 3 through 6  */
	Total64_Total32(pTotal,pTotal_32,ItemDiscExtra.TtlIAmount[2]);      /* Item discounts 3 through 6  */
	Total64_Total32(pTotal,pTotal_32,ItemDiscExtra.TtlIAmount[3]);      /* Item discounts 3 through 6  */
    Total64_Total32(pTotal,pTotal_32,RegDisc[0]);            /* Regular Discount 1 to 6, Release 3.1 */
    Total64_Total32(pTotal,pTotal_32,RegDisc[1]);            /* Regular Discount 1 to 6, Release 3.1 */
    Total64_Total32(pTotal,pTotal_32,RegDisc[2]);            /* Regular Discount 1 to 6, Release 3.1 */
    Total64_Total32(pTotal,pTotal_32,RegDisc[3]);            /* Regular Discount 1 to 6, Release 3.1 */
    Total64_Total32(pTotal,pTotal_32,RegDisc[4]);            /* Regular Discount 1 to 6, Release 3.1 */
    Total64_Total32(pTotal,pTotal_32,RegDisc[5]);            /* Regular Discount 1 to 6, Release 3.1 */
    Total64_Total32(pTotal,pTotal_32,Coupon);                /* Combination Coupon, R3.0 */
    Total64_Total32(pTotal,pTotal_32,PaidOut);               /* Paid Out */
    Total64_Total32(pTotal,pTotal_32,RecvAcount);            /* Received on Acount */
    Total64_Total32(pTotal,pTotal_32,TipsPaid);              /* Tips Paid Out */
    Total64_Total32(pTotal,pTotal_32,FoodStampCredit);       /* Food Stamp Credit */

    Total64_Total32(pTotal,pTotal_32,CashTender.OnlineTotal);       /* Cash Tender          change EPT*/
    Total64_Total32(pTotal,pTotal_32,CashTender.OfflineTotal);      /* Cash Tender          change EPT*/
    Total64_Total32(pTotal,pTotal_32,CheckTender.OnlineTotal);      /* Check Tender         change EPT*/
    Total64_Total32(pTotal,pTotal_32,CheckTender.OfflineTotal);     /* Check Tender         change EPT*/
    Total64_Total32(pTotal,pTotal_32,ChargeTender.OnlineTotal);     /* Charge Tender        change EPT*/
    Total64_Total32(pTotal,pTotal_32,ChargeTender.OfflineTotal);    /* Charge Tender        change EPT*/
	for (i = 0; i < STD_TENDER_MAX - 3; i++) {
		Total64_Total32(pTotal,pTotal_32,MiscTender[i].OnlineTotal);      /* Misc Tender 1 to 8   change EPT*/
		Total64_Total32(pTotal,pTotal_32,MiscTender[i].OfflineTotal);     /* Misc Tender 1 to 8   change EPT*/
	}

	for (i = 0; i < STD_NO_FOREIGN_TTL; i++) {
		Total64_Total32(pTotal,pTotal_32,ForeignTotal[i]);    /* Foreign Total 1 to 8,    Saratoga */
	}

	for (i = 0; i < STD_TENDER_MAX - 3; i++) {
		Total64_Total32(pTotal,pTotal_32,ServiceTotal[i]);    /* Service Total 3 to 8, 10-20 JHHJ */
	}

	for (i = 0; i < 3; i++) {
		Total64_Total32(pTotal,pTotal_32,AddCheckTotal[i]);    /* Add Check Total 1 to 3 */
		Total64_Total32(pTotal,pTotal_32,ChargeTips[i]);       /* Charge Tips, V3.3 */
	}

	Total64_Total32(pTotal,pTotal_32,Loan);               /* Loan,    Saratoga */
	Total64_Total32(pTotal,pTotal_32,Pickup);             /* Pickup,  Saratoga */
	Total64_Total32(pTotal,pTotal_32,TransCancel);        /* Transaction Cancel */
	Total64_Total32(pTotal,pTotal_32,MiscVoid);           /* Misc Void */
	Total64_Total32(pTotal,pTotal_32,Audaction);          /* Audaction total or number and amount of tenders of zero or negative amount */

	Member64_Member32(pTotal,pTotal_32,sNoSaleCount);            /* No Sale Counter */
	Member64_Member32(pTotal,pTotal_32,lItemProductivityCount);  /* Item Productivity Counter */
	Member64_Member32(pTotal,pTotal_32,sNoOfPerson);             /* Number of Person Counter */
	Member64_Member32(pTotal,pTotal_32,sNoOfChkOpen);            /* Number of Checks Opened */
	Member64_Member32(pTotal,pTotal_32,sNoOfChkClose);           /* Number of Checks Closed */
	Member64_Member32(pTotal,pTotal_32,sCustomerCount);          /* Customer Counter */
	Total64_Total32(pTotal,pTotal_32,HashDepartment);            /* Hash Department */
	for (i = 0; i < STD_NO_BONUS_TTL; i++) {
		Total64_Total32(pTotal,pTotal_32,Bonus[i]);              /* Bonus Total 1 to 8 JHHJ 3-15-04*/
	}

    Long64_Long32(pTotal,pTotal_32,lConsTax);                    /* Consolidation tax */
	for (i = 0; i < 3; i++) {
		Total64_Total32(pTotal,pTotal_32,aUPCCoupon[i]);         /* Coupon (Single/Double/Triple), saratoga */
	}
	for (i = 0; i < STD_NUM_FUTURE_TTLS; i++) {
		Total64_Total32(pTotal,pTotal_32,ttlFutureTotals[i]);    /* Future totals for Release 2.1 JHHJ*/
	}
	Total64_Total32(pTotal,pTotal_32,ttlCoinLoan);			     //New loan totals
	Total64_Total32(pTotal,pTotal_32,ttlCoinAmountDispenced);	 //New loan totals
	Member64_Member32(pTotal,pTotal_32,usCoinLowCounter);		 //New loan counter
	Member64_Member32(pTotal,pTotal_32,usCoinAmountDispancedCounter);  //New loan counter
	Member64_Member32(pTotal,pTotal_32,usCoinLoanCounter);		 //New loan counter

	return 0;
}

SHORT  IspRecvTtlConvert_TTLCASHIER (TTLCASHIER *pTotal, TTLCASHIER_32 *pTotal_32)
{
	int i;

	if (pTotal->uchMajorClass != CLASS_TTLCASHIER)
		return ISP_ERR_INVALID_MESSAGE;

	memset (pTotal_32, 0, sizeof(TTLCASHIER_32));

	Member64_Member32(pTotal,pTotal_32,uchMajorClass);      /* Major Class Data definition */
    Member64_Member32(pTotal,pTotal_32,uchMinorClass);      /* Minor Class Data definition */
    Member64_Member32(pTotal,pTotal_32,ulCashierNumber);    /* Cashier Number */
    Member64_Member32(pTotal,pTotal_32,uchResetStatus);     /* Reset Report Status */
    NDate64_NDate32(pTotal,pTotal_32,FromDate);             /* Preiod From */
    NDate64_NDate32(pTotal,pTotal_32,ToDate);               /* Preiod To */
    Member64_Member32(pTotal,pTotal_32,lDGGtotal);           /* Current Gross Group Total */
	Total64_Total32(pTotal,pTotal_32,PlusVoid);              /* Plus Void */
    Total64_Total32(pTotal,pTotal_32,PreselectVoid);         /* Preselect Void */
    Total64_Total32(pTotal,pTotal_32,TransactionReturn);     /* transaction return, CURQUAL_TRETURN, MDC_RETURNS_RTN1_ADR, MDC_RPTFIN46_ADR */
    Total64_Total32(pTotal,pTotal_32,TransactionExchng);     /* transaction return, CURQUAL_TRETURN, MDC_RETURNS_RTN2_ADR, MDC_RPTFIN46_ADR */
    Total64_Total32(pTotal,pTotal_32,TransactionRefund);     /* transaction return, CURQUAL_TRETURN, MDC_RETURNS_RTN3_ADR, MDC_RPTFIN46_ADR */
    Total64_Total32(pTotal,pTotal_32,ConsCoupon);            /* Consolidated Coupon */
    Total64_Total32(pTotal,pTotal_32,ItemDisc);              /* Item Discount */
    Total64_Total32(pTotal,pTotal_32,ModiDisc);              /* Modified Item Discount */

	Total64_Total32(pTotal,pTotal_32,ItemDiscExtra.TtlIAmount[0]);      /* Item discounts 3 through 6  */
	Total64_Total32(pTotal,pTotal_32,ItemDiscExtra.TtlIAmount[1]);      /* Item discounts 3 through 6  */
	Total64_Total32(pTotal,pTotal_32,ItemDiscExtra.TtlIAmount[2]);      /* Item discounts 3 through 6  */
	Total64_Total32(pTotal,pTotal_32,ItemDiscExtra.TtlIAmount[3]);      /* Item discounts 3 through 6  */
    Total64_Total32(pTotal,pTotal_32,RegDisc[0]);            /* Regular Discount 1 to 6, Release 3.1 */
    Total64_Total32(pTotal,pTotal_32,RegDisc[1]);            /* Regular Discount 1 to 6, Release 3.1 */
    Total64_Total32(pTotal,pTotal_32,RegDisc[2]);            /* Regular Discount 1 to 6, Release 3.1 */
    Total64_Total32(pTotal,pTotal_32,RegDisc[3]);            /* Regular Discount 1 to 6, Release 3.1 */
    Total64_Total32(pTotal,pTotal_32,RegDisc[4]);            /* Regular Discount 1 to 6, Release 3.1 */
    Total64_Total32(pTotal,pTotal_32,RegDisc[5]);            /* Regular Discount 1 to 6, Release 3.1 */
    Total64_Total32(pTotal,pTotal_32,Coupon);                /* Combination Coupon, R3.0 */
    Total64_Total32(pTotal,pTotal_32,PaidOut);               /* Paid Out */
    Total64_Total32(pTotal,pTotal_32,RecvAcount);            /* Received on Acount */
    Total64_Total32(pTotal,pTotal_32,TipsPaid);              /* Tips Paid Out */
    Total64_Total32(pTotal,pTotal_32,FoodStampCredit);       /* Food Stamp Credit */

    Total64_Total32(pTotal,pTotal_32,CashTender.Total);              /* Cash Tender          change EPT*/
    Long64_Long32(pTotal,pTotal_32,CashTender.lHandTotal);           /* Cash Tender          change EPT*/
    Total64_Total32(pTotal,pTotal_32,CheckTender.Total);             /* Check Tender         change EPT*/
    Long64_Long32(pTotal,pTotal_32,CheckTender.lHandTotal);          /* Check Tender         change EPT*/
    Total64_Total32(pTotal,pTotal_32,ChargeTender.Total);            /* Charge Tender        change EPT*/
    Long64_Long32(pTotal,pTotal_32,ChargeTender.lHandTotal);         /* Charge Tender        change EPT*/
	for (i = 0; i < STD_TENDER_MAX - 3; i++) {
		Total64_Total32(pTotal,pTotal_32,MiscTender[i].Total);       /* Misc Tender 1 to 8   change EPT*/
		Long64_Long32(pTotal,pTotal_32,MiscTender[i].lHandTotal);    /* Misc Tender 1 to 8   change EPT*/
	}

	for (i = 0; i < STD_NO_FOREIGN_TTL; i++) {
		Total64_Total32(pTotal,pTotal_32,ForeignTotal[i].Total);    /* Foreign Total 1 to 8,    Saratoga */
		Long64_Long32(pTotal,pTotal_32,ForeignTotal[i].lHandTotal);    /* Foreign Total 1 to 8,    Saratoga */
	}

	for (i = 0; i < STD_TENDER_MAX - 3; i++) {
		Total64_Total32(pTotal,pTotal_32,ServiceTotal[i]);    /* Service Total 3 to 8, 10-20 JHHJ */
	}

	for (i = 0; i < 3; i++) {
		Total64_Total32(pTotal,pTotal_32,AddCheckTotal[i]);    /* Add Check Total 1 to 3 */
		Total64_Total32(pTotal,pTotal_32,ChargeTips[i]);       /* Charge Tips, V3.3 */
	}

	Total64_Total32(pTotal,pTotal_32,Loan);               /* Loan,    Saratoga */
	Total64_Total32(pTotal,pTotal_32,Pickup);             /* Pickup,  Saratoga */
	Total64_Total32(pTotal,pTotal_32,TransCancel);        /* Transaction Cancel */
	Total64_Total32(pTotal,pTotal_32,MiscVoid);           /* Misc Void */
	Total64_Total32(pTotal,pTotal_32,Audaction);          /* Audaction total or number and amount of tenders of zero or negative amount */

	Member64_Member32(pTotal,pTotal_32,sNoSaleCount);            /* No Sale Counter */
	Member64_Member32(pTotal,pTotal_32,lItemProductivityCount);  /* Item Productivity Counter */
	Member64_Member32(pTotal,pTotal_32,sNoOfPerson);             /* Number of Person Counter */
	Member64_Member32(pTotal,pTotal_32,sNoOfChkOpen);            /* Number of Checks Opened */
	Member64_Member32(pTotal,pTotal_32,sNoOfChkClose);           /* Number of Checks Closed */
	Member64_Member32(pTotal,pTotal_32,sCustomerCount);          /* Customer Counter */
	Total64_Total32(pTotal,pTotal_32,HashDepartment);            /* Hash Department */
	for (i = 0; i < STD_NO_BONUS_TTL; i++) {
		Total64_Total32(pTotal,pTotal_32,Bonus[i]);              /* Bonus Total 1 to 8 JHHJ 3-15-04*/
	}

    Long64_Long32(pTotal,pTotal_32,lConsTax);                    /* Consolidation tax */
    Member64_Member32(pTotal,pTotal_32,usPostRecCo);             /* post receipt counter, R3,1 */
    Long64_Long32(pTotal,pTotal_32,lVATServiceTotal);            /* Service Total of VAT, V3.3*/
    Total64_Total32(pTotal,pTotal_32,CheckTransTo);              /* Checks Transferred to, V3.3 */
    Total64_Total32(pTotal,pTotal_32,CheckTransFrom);            /* Checks Transferred from, V3.3 */
    Total64_Total32(pTotal,pTotal_32,DeclaredTips);              /* Declared Tips, V3.3 */
	for (i = 0; i < STD_NUM_FUTURE_TTLS; i++) {
		Total64_Total32(pTotal,pTotal_32,aUPCCoupon[i]);      /* Coupon (Single/Double/Triple), saratoga */
	}
	Taxable64_Taxable32 (pTotal->Taxable + 0, pTotal_32->Taxable + 0); /* Taxable Total 1 to 4, R3.0 */
	Taxable64_Taxable32 (pTotal->Taxable + 1, pTotal_32->Taxable + 1); /* Taxable Total 1 to 4, R3.0 */
	Taxable64_Taxable32 (pTotal->Taxable + 2, pTotal_32->Taxable + 2); /* Taxable Total 1 to 4, R3.0 */
	Taxable64_Taxable32 (pTotal->Taxable + 3, pTotal_32->Taxable + 3); /* Taxable Total 1 to 4, R3.0 */

	Member64_Member32(pTotal,pTotal_32,NonTaxable);         /* Non taxable Total */
	for (i = 0; i < STD_NUM_FUTURE_TTLS; i++) {
		Total64_Total32(pTotal,pTotal_32,ttlFutureTotals[i]);    /* Future totals for Release 2.1 JHHJ*/
	}
	Total64_Total32(pTotal,pTotal_32,ttlCoinLoan);			     //New loan totals
	Total64_Total32(pTotal,pTotal_32,ttlCoinAmountDispenced);	 //New loan totals
	Member64_Member32(pTotal,pTotal_32,usCoinLowCounter);		 //New loan counter
	Member64_Member32(pTotal,pTotal_32,usCoinAmountDispancedCounter);  //New loan counter
	Member64_Member32(pTotal,pTotal_32,usCoinLoanCounter);		 //New loan counter

	return 0;
}

// Convert the total struct pointed to by pTotal to its corresponding 32 bit legacy form.
// This function is used to convert from the internal DCURRENCY and 64 bit version of a
// total struct to the extern LONG and 32 bit version of the total struct for those
// remote applications which have not yet converted to the larger total size.
SHORT  IspRecvTtlConvertTo_32 (void *pTotal, void *pTotal_32)
{
    SHORT       sRet;
	UCHAR       uchMajorClass = ((TTLREGFIN *)pTotal)->uchMajorClass;

    switch ( uchMajorClass ) {

    case CLASS_TTLREGFIN :                      /* Register financial */
    case CLASS_TTLINDFIN :                      /* individual fin R3.1 */
		IspRecvTtlConvert_TTLREGFIN (pTotal, pTotal_32);
		sRet = sizeof(TTLREGFIN_32);
        break;

    case CLASS_TTLHOURLY :                      /* Hourly activity    */   
		//When we are reading a total hourly length we are now
		//only reading one hour at a time, and it is always placed in the
		///0 index of the array.  there are 48 elements in the array, so 
		//we only need to read the first one and not the other 47. JHHJ 
        sRet = sizeof(TTLHOURLY_32_1);
		{
			TTLHOURLY_32_1  *t32 = pTotal_32;
			TTLHOURLY_1     *t64 = pTotal;
			int             i;

			// copy the totals header information over.
			t32->uchMajorClass = t64->uchMajorClass;
			t32->uchMinorClass = t64->uchMinorClass;
			t32->uchResetStatus = t64->uchResetStatus;
			NDate64_NDate32(t64,t32,FromDate);
			t32->StartTime = t64->StartTime;
			t32->EndTime = t64->EndTime;
			t32->usBlockRead = t64->usBlockRead;

			// copy the totals information over.
			Long64_Long32(t64,t32,Total[0].lHourlyTotal);                /* Hourly Total */
			Member64_Member32(t64,t32,Total[0].lItemproductivityCount);  /* IP Counter */
			Member64_Member32(t64,t32,Total[0].sDayNoOfPerson);          /* Number of Person Counter */
			for (i = 0; i < STD_NO_BONUS_TTL; i++) {
				Long64_Long32(t64,t32,Total[0].lBonus[i]);              /* Bonus Total 1 to 8 JHHJ 3-15-04*/
			}
		}
        break;
    
    case CLASS_TTLDEPT :                        /* Department */
		{
			union {
				TTLDEPT_32  t32;
				TTLDEPT     t64;
			} u;
			u.t64 = *(TTLDEPT *)pTotal;
			Ltotal64_Ltotal32((TTLDEPT *)pTotal,&u.t32,DEPTTotal);           /* Department Total */
			Ltotal64_Ltotal32((TTLDEPT *)pTotal,&u.t32,DEPTAllTotal);        /* Department All Total */
			*(TTLDEPT_32 *)pTotal_32 = u.t32;
		}
		sRet = sizeof(TTLDEPT_32);
        break;

    case CLASS_TTLPLU :                         /* PLU */ 
		{
			union {
				TTLPLU_32  t32;
				TTLPLU     t64;
			} u;
			u.t64 = *(TTLPLU *)pTotal;
			Ltotal64_Ltotal32((TTLPLU *)pTotal,&u.t32,PLUTotal);           /* PLU Total */
			Ltotal64_Ltotal32((TTLPLU *)pTotal,&u.t32,PLUAllTotal);        /* PLU All Total */
			*(TTLPLU_32 *)pTotal_32 = u.t32;
		}
		sRet = sizeof(TTLPLU_32);
        break;
    
    case CLASS_TTLCASHIER :                     /* Cashier */ 
		IspRecvTtlConvert_TTLCASHIER (pTotal, pTotal_32);
        sRet = sizeof(TTLCASHIER_32);
        break;

    case CLASS_TTLCPN :                         /* Coupon R3.0 */
		{
			union {
				TTLCPN_32  t32;
				TTLCPN     t64;
			} u;
			u.t64 = *(TTLCPN *)pTotal;
			Total64_Total32((TTLCPN *)pTotal,&u.t32,CpnTotal);          /* Department Total */
			*(TTLCPN_32 *)pTotal_32 = u.t32;
		}
		sRet = sizeof(TTLCPN_32);
        break;

    case CLASS_TTLSERVICE:                      /* service time R3.1 */
		{
			TTLSERTIME     *t64 = pTotal;
			TTLSERTIME_32  *t32 = pTotal_32;
			int            i,j;

			Member64_Member32(t64,t32,uchMajorClass);      /* Major Class Data definition */
			Member64_Member32(t64,t32,uchMinorClass);      /* Minor Class Data definition */
			Member64_Member32(t64,t32,uchResetStatus);     /* Reset Report Status */
			NDate64_NDate32(t64,t32,FromDate);             /* Preiod From */
			NDate64_NDate32(t64,t32,ToDate);               /* Preiod To */

			for (i = 0; i < 3; i++) {
				for (j = 0; j < STD_MAX_HOURBLK; j++) {
					Total64_Total32(t64,t32,ServiceTime[j][i]);
				}
			}
		}
		sRet = sizeof(TTLSERTIME_32);
        break;
    
    default :                                   /* Error  */
        sRet = ISP_ERR_INVALID_MESSAGE;
        break;
    }

    return(sRet);
}

#endif


/******************** New Functions (Release 3.1) BEGIN ******************/
/*
*===========================================================================
** Synopsis:    SHORT    IspRecvTtlResetIndFin(CLIREQRESETTL *pReqMsgH)
*     Input:    pReqMsgH    - request from ISP
*     Output:   nothing
*     InOut:    nothing
*
** Return:      CAS_PERFORM:            Success
*               CAS_???:                Error 
*
** Description:
*   This function executes for Individual Financial Total reset
*===========================================================================
*/
SHORT   IspRecvTtlResetIndFin(CLIREQRESETTL *pReqMsgH)
{
    SHORT    sError;

    /* set pc/if log, V3.3 */
    IspRecvTtlSetLog(&pReqMsgH->uchMajorClass, pReqMsgH->usFunCode);

    sError = SerTtlTotalReset(&pReqMsgH->uchMajorClass, pReqMsgH->usExeMDC);

	if (sError != TTL_SUCCESS) {
		PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_DATA_ERR_04);
		PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_RSET_FAIL);
		PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sError));
		PifLog(MODULE_LINE_NO(MODULE_ISP_LOG), (USHORT)__LINE__);
	}

    /* V3.3 */
    IspResetLog.uchError = (UCHAR)TtlConvertError(sError);
    IspWriteResetLog();

    return (sError);
}
/******************** New Functions (Release 3.1) END ******************/


/*
*===========================================================================
** Synopsis:    SHORT    IspMakeClosePickFormat(VOID)
*
*     Input:    pReqMsg      - request from Isp
*               usCashierNO  - cashier number
*
** Return:      ISP_SUCCESS:             All data received
*               ISP_ERR_INVALID_MESSAGE: Data error 
*
** Description:
*   This function executes for pickup/loan closing operation.
*===========================================================================
*/
SHORT   IspMakeClosePickFormat(VOID)
{
    ITEMTRANSCLOSE  TransClose;

    TransClose.uchMajorClass = CLASS_ITEMTRANSCLOSE;
    if ( usIspPickupFunCode == ISP_FUNC_LOAN ) {
        TransClose.uchMinorClass = CLASS_CLS_LOAN;
    } else {
        TransClose.uchMinorClass = CLASS_CLS_PICK;
    }

    TrnClose(&TransClose);

    return (ISP_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspRecvTtlDelete (CLIREQRESETTL *pReqMsgH)
*     Input:    pReqMsgH    - request from ISP
*     Output:   nothing
*     InOut:    nothing
*
** Return:      STUB_SUCCESS:           Success
*               CAS_???:                Success or Error code
*               WAI_???:                Success or Error code
*               ISP_ERR_REQUEST_REST    Need to reset first
*
** Description:
*   This function executes for Total reset EOD, PTD function request.
*===========================================================================
*/
SHORT   IspRecvTtlDelete(CLIREQRESETTL *pReqMsgH)
{
    USHORT   fsTmpLock;
    SHORT    sError;

    fsTmpLock = 0;
    sError    = STUB_SUCCESS;

    for (;;) {

        if ( 0 == ( fsIspLockedFC & ISP_LOCK_ALLCASHIER )) {
            if (ISP_SUCCESS == (sError = IspCheckFlexFileSize(FLEX_CAS_ADR))) {
                if (CAS_PERFORM == ( sError = SerCasAllLock() ) ) {  /* cashier all lock */
                    fsTmpLock |= ISP_LOCK_ALLCASHIER; /* Set Locked cashier */
                } else {
                    break;      /* Error  break */
                }
            }
        } 

        /* set pc/if log, V3.3 */
        IspRecvTtlSetLog(&pReqMsgH->uchMajorClass, pReqMsgH->usFunCode);

        sError = SerTtlTotalDelete(&pReqMsgH->uchMajorClass);

		if (sError != TTL_SUCCESS) {
			PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_DATA_ERR_03);
			PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sError));
		}

        /* V3.3 */
        IspResetLog.uchError = (UCHAR)TtlConvertError(sError);
        IspWriteResetLog();

        break;
    }

    if ( fsTmpLock & ISP_LOCK_ALLCASHIER ) {
        SerCasAllUnLock();                         /* Cashier unlock */
    }
    return (sError);
}


/*
*===========================================================================
** Synopsis:    SHORT    IspRecvTtlReset(CLIREQRESETTL *pReqMsgH)
*     Input:    pReqMsgH    - request from ISP
*     Output:   nothing
*     InOut:    nothing
*
** Return:      STUB_SUCCESS:           Success
*               CAS_???:                Success or Error code
*               WAI_???:                Success or Error code
*               ISP_ERR_REQUEST_REST    Need to reset first
*
** Description:
*   This function executes for Total reset EOD, PTD function request.
*===========================================================================
*/
SHORT   IspRecvTtlPLUOptimize(CLIREQRESETTL *pReqMsgH)
{
    USHORT   fsTmpLock;
    SHORT    sError;

    fsTmpLock = 0;
    sError    = STUB_SUCCESS;

    for (;;) {
        if ( 0 == ( fsIspLockedFC & ISP_LOCK_ALLCASHIER )) {
            if (ISP_SUCCESS == (sError = IspCheckFlexFileSize(FLEX_CAS_ADR))) {
                if (CAS_PERFORM == ( sError = SerCasAllLock() ) ) {  /* cashier all lock */
                    fsTmpLock |= ISP_LOCK_ALLCASHIER; /* Set Locked cashier */
                } else {
                    break;      /* Error  break */
                }
            }
        } 

        sError = SerTtlPLUOptimize(pReqMsgH->uchMinorClass);
        break;
    }

    if ( fsTmpLock & ISP_LOCK_ALLCASHIER ) {
        SerCasAllUnLock();                         /* Cashier unlock */
    }
    return (sError);
}


/*===== END OF SOURCE =====*/