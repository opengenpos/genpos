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
* Title       : ISP Server Module, Operational Para. Functions Source File
* Category    : ISP Server Module, US Hospitality Model
* Program Name: ISPOPR.C
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
*      IspRecvOpr();           Entry point of Ope Para function handling
*
*      IspRecvCheckPara();  C  Check   flex  parameter
*      IspAfterProcess();      Execute after process
*
* --------------------------------------------------------------------------
* Update Histories
* Date     :Ver.Rev.:   Name    : Description
* Jun-26-92:00.00.01:H.Yamaguchi: initial
* Jun-23-93:00.00.01:H.Yamaguchi: Adds Enhace
* Jun-15-95:03.00.00:hkato      : R3.0
* Mar-01-96:03.01.00:T.Nakahata : R3.1 Initial (Add PPI file)
* Aug-04-98:03.03.00:hrkato     : V3.3
*
** NCR2171 **
* Aug-26-99:01.00.00:M.Teraki   :initial (for 2171)
* Dec-14-99:01.00.00:hrkato     :Saratoga
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
#include    <string.h>
#include	<stdio.h>
#include	<math.h>
#include    <memory.h>

#include    <ecr.h>
#include    <pif.h>
#include    <rfl.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <csop.h>
#include    <csstbopr.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csttl.h>
#include    <csstbttl.h>
#include    <storage.h>
#include    <isp.h>
#include    "ispcom.h"
#include    "ispext.h"
#include    <appllog.h>
#include    <applini.h>
#include "display.h"
#include <pararam.h>
#include <nb.h>

#include "scf.h"
#include <uie.h>
#include <BlFWif.h>

CONST TCHAR FARCONST aszParaProg[]      = _T("PARAM PRG");
CONST TCHAR FARCONST aszParaSPV[]       = _T("PARAM SPV");
CONST TCHAR FARCONST aszParaStoReg[]    = _T("STR REG");

static VOID (PIFENTRY *pPifSaveFarData)(VOID) = PifSaveFarData; /* saratoga */

/*
*===========================================================================
** Synopsis:    SHORT    IspCheckDataFlexMem(VOID * pFlex);
*     Input:    pFlex    - Start address of Flex memory data
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:                 Data is Ok.
*               ISP_ERR_INVALID_MESSAGE:     Data is over than max size.
*
** Description: This function check parameter.                  Saratoga
*===========================================================================
*/
static SHORT   IspCheckDataFlexMem (ISPFLEXMEM  *pFM)
{
	SHORT    sRetStatus = ISP_SUCCESS;
    UCHAR    uchAdd;

    for ( uchAdd = FLEX_DEPT_ADR ; uchAdd <= MAX_FLXMEM_SIZE; uchAdd++ ) {
        switch ( uchAdd ) {
        case FLEX_DEPT_ADR :   
            if ( pFM->Fxmem[uchAdd-1].ulRecordNumber >  FLEX_DEPT_MAX ) {
                sRetStatus = (ISP_ERR_INVALID_MESSAGE);
            }
            break;
        
        case FLEX_PLU_ADR  :
            if ( pFM->Fxmem[uchAdd-1].ulRecordNumber >  FLEX_PLU_MAX ) {
                sRetStatus = (ISP_ERR_INVALID_MESSAGE);
            }
            break;

        case FLEX_WT_ADR   :    
            if ( pFM->Fxmem[uchAdd-1].ulRecordNumber >  FLEX_WT_MAX ) {
                sRetStatus = (ISP_ERR_INVALID_MESSAGE);
            }
            break;

        case FLEX_CAS_ADR  :
            if ( pFM->Fxmem[uchAdd-1].ulRecordNumber >  FLEX_CAS_MAX ) {
                sRetStatus = (ISP_ERR_INVALID_MESSAGE);
            }
            break;
    
        case FLEX_ETK_ADR  :
            if ( pFM->Fxmem[uchAdd-1].ulRecordNumber >  FLEX_ETK_MAX ) {
                sRetStatus = (ISP_ERR_INVALID_MESSAGE);
            }
            break;

        case FLEX_EJ_ADR  :
            if ( pFM->Fxmem[uchAdd-1].ulRecordNumber >  FLEX_EJ_MAX ) {
                sRetStatus = (ISP_ERR_INVALID_MESSAGE);
            }
            break;

        case FLEX_ITEMSTORAGE_ADR:
        case FLEX_CONSSTORAGE_ADR:
            if ((pFM->Fxmem[uchAdd-1].ulRecordNumber < FLEX_ITEMSTORAGE_MIN) ||   
                (pFM->Fxmem[uchAdd-1].ulRecordNumber > FLEX_ITEMSTORAGE_MAX)) {   
                sRetStatus = (ISP_ERR_INVALID_MESSAGE);
            }
            break;

        /************************ Add R3.0 ****************************/
        case FLEX_CPN_ADR  :
            if ( pFM->Fxmem[uchAdd-1].ulRecordNumber >  FLEX_CPN_MAX ) {
                sRetStatus = (ISP_ERR_INVALID_MESSAGE);
            }
            break;

        case FLEX_CSTRING_ADR  :
            if ( pFM->Fxmem[uchAdd-1].ulRecordNumber >  FLEX_CSTRING_MAX ) {
                sRetStatus = (ISP_ERR_INVALID_MESSAGE);
            }
            break;
        /************************ End Add R3.0 ************************/

		case FLEX_PROGRPT_ADR:						/* Programable Report Size */
            if ( pFM->Fxmem[uchAdd-1].ulRecordNumber >  FLEX_PROGRPT_MAX ) {
                sRetStatus = (ISP_ERR_INVALID_MESSAGE);
            }
            break;
            
		case FLEX_PPI_ADR:
            if ( pFM->Fxmem[uchAdd-1].ulRecordNumber >  FLEX_PPI_MAX ) {
                sRetStatus = (ISP_ERR_INVALID_MESSAGE);
            }
            break;

 		case FLEX_GC_ADR:
           if ( pFM->Fxmem[uchAdd-1].ulRecordNumber >  FLEX_GC_MAX ) {
                sRetStatus = (ISP_ERR_INVALID_MESSAGE);
            }
            break;

        default :
            // if out of range then just ignore this file number for now.
			// sRetStatus = (ISP_ERR_INVALID_MESSAGE);
            break;
        }
    }

	if (sRetStatus != ISP_SUCCESS) {
		char xBuff[128];
		sprintf (xBuff, "==ERROR: IspCheckDataFlexMem() ulRecordNumber %d check failed for file %d.", uchAdd, pFM->Fxmem[uchAdd - 1].ulRecordNumber);
		NHPOS_ASSERT_TEXT((sRetStatus == ISP_SUCCESS), xBuff);
	}
 
    return (sRetStatus);
}


/*
*===========================================================================
** Synopsis:    VOID    IspRecvOpr(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function executes for request operational parameter.
*===========================================================================
*/
VOID    IspRecvOpr(VOID)
{
    CLIREQOPPARA    *pReqMsgH = 0;
    CLIREQDATA      *pReqBuff = 0;
    PLUIF           *pPlu = 0;
    CLIRESOPPARA    ResMsgH;
    SHORT           i, sFlag = 0;
	SHORT           sRetStatus = 0;

    pReqMsgH = (CLIREQOPPARA *)IspRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ResMsgH.usFunCode     = pReqMsgH->usFunCode;
    ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sResCode      = STUB_MULTI_SEND;
    ResMsgH.sReturn       = ISP_SUCCESS;
    ResMsgH.uchMajorClass = pReqMsgH->uchMajorClass;
    ResMsgH.usHndorOffset = pReqMsgH->usHndorOffset;
    ResMsgH.usRWLen       = pReqMsgH->usRWLen;

	if ( CLI_FCOSYSTEMSTATUS == (pReqMsgH->usFunCode & CLI_RSTCONTCODE)) {
        IspSendSystemStatusResponse();
        return;
	}

    switch( pReqMsgH->usFunCode & CLI_RSTCONTCODE ) {
    case CLI_FCOPLOCK :         /* OP lock    */
    case CLI_FCOPUNLOCK :       /* OP unlock  */
    case CLI_FCOPPLUFILEUPD :   /* OP PLU  file update sub index  */
    case CLI_FCPARAALLREAD :    /* PARA all read  rameter */
        IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESOPPARA)];
        pReqBuff = (CLIREQDATA *)((UCHAR *)pReqMsgH + sizeof(CLIREQOPPARA));
        IspResp.pSavBuff->usDataLen = pReqBuff->usDataLen;
        pPlu = (PLUIF *)IspResp.pSavBuff->auchData;
        break;

    case CLI_FCPARAALLWRITE :   /* PARA all write rameter */
        break;

    case CLI_FCOPOEPPLUREAD :      /* OP OPE PLU read , saratoga */
        IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESOPPARA)];
        pReqBuff = (CLIREQDATA *)((UCHAR *)pReqMsgH + sizeof(CLIREQOPPARA));
        IspResp.pSavBuff->usDataLen = sizeof(PLUIF_OEP);
        pPlu = (PLUIF *)IspResp.pSavBuff->auchData;
        memcpy(IspResp.pSavBuff->auchData, pReqBuff->auchData, pReqBuff->usDataLen);
        break;

    case CLI_FCOPREQDISPMSG :  /* OP display a message to the operator  */
        break;

    default:                    /* not used */
        pReqBuff = (CLIREQDATA *)((UCHAR *)pReqMsgH + sizeof(CLIREQOPPARA));
        IspResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESOPPARA)];
        IspResp.pSavBuff->usDataLen = pReqBuff->usDataLen;
        memcpy(IspResp.pSavBuff->auchData, pReqBuff->auchData, pReqBuff->usDataLen);
        pPlu = (PLUIF *)IspResp.pSavBuff->auchData;
        break;
    }

    switch( pReqMsgH->usFunCode & CLI_RSTCONTCODE ) {
    case CLI_FCOPPLUINDREAD :   /*  OP PLU  individual read */
        if ((sRetStatus = IspLabelAnalysis(pPlu)) == ISP_SUCCESS) {
            ResMsgH.sReturn = CliOpPluIndRead(pPlu, husIspOpHand);
        } else {
			PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_LABEL_ANALYSIS_FAIL);
			PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sRetStatus));
			PifLog(MODULE_LINE_NO(MODULE_ISP_LOG), (USHORT)__LINE__);
            ResMsgH.sReturn = OP_PLU_ERROR;
        }
        break;

    case CLI_FCOPDEPTINDREAD :  /* OP DEPT individual read */
        ResMsgH.sReturn = CliOpDeptIndRead((DEPTIF *)pPlu, husIspOpHand);
        break;

    case CLI_FCOPPLUASSIGN :    /* OP PLU  assign          */
        if ( 0 == (ISP_LOCKED_KEYBOARD & fsIspCleanUP)) {  /* if LOCKED falg on, then OK. */
            ResMsgH.sReturn = ISP_ERR_REQUEST_LOCKKBD;  /* Request lock keyboard */
			PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_STATUS_INVALID_STATE);
			PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(ResMsgH.sReturn));
			PifLog(MODULE_LINE_NO(MODULE_ISP_LOG), (USHORT)__LINE__);
            break;
        }
		// fall through into the update code.
    case CLI_FCOPPLUUPDATE :    /* OP PLU  assign          */
        if ((sRetStatus = IspLabelAnalysis(pPlu)) == ISP_SUCCESS) {
            ResMsgH.sReturn = SerOpPluAssign(pPlu, husIspOpHand);   /* Saratoga */
            if (OP_CHANGE_PLU == ResMsgH.sReturn || OP_NEW_PLU == ResMsgH.sReturn) {
                 fsIspCleanUP  |= ISP_CREATE_SUBINDEX_PLU;      /* Set flag */
            }
        } else {
			PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_LABEL_ANALYSIS_FAIL);
			PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sRetStatus));
			PifLog(MODULE_LINE_NO(MODULE_ISP_LOG), (USHORT)__LINE__);
            ResMsgH.sReturn = OP_PLU_ERROR;
        }
        break;

    case CLI_FCOPDEPTASSIGN :   /* OP DEPT assign          */
        if ( 0 == (ISP_LOCKED_KEYBOARD & fsIspCleanUP)) {  /* if LOCKED falg on, then OK. */
            ResMsgH.sReturn = ISP_ERR_REQUEST_LOCKKBD;  /* Request lock keyboard */
            break;
        }
        ResMsgH.sReturn = SerOpDeptAssign((DEPTIF *)pPlu, husIspOpHand );
        break;

    case CLI_FCOPDEPTDELETE :   /* OP DEPT delete, saratoga          */
        if ( 0 == (ISP_LOCKED_KEYBOARD & fsIspCleanUP)) {  /* if LOCKED falg on, then OK. */
            ResMsgH.sReturn = ISP_ERR_REQUEST_LOCKKBD;  /* Request lock keyboard */

        } else {
            ResMsgH.sReturn = IspCheckTtlDept(((DEPTIF *)pPlu)->usDeptNo); /* Saratoga */
            if ( ISP_SUCCESS == ResMsgH.sReturn ) {
                ResMsgH.sReturn = SerOpDeptDelete((DEPTIF *)pPlu, husIspOpHand);    /* Saratoga */
            }
        }
        break;

    case CLI_FCOPPLUDELETE :    /* OP PLU  delete          */
        if ( 0 == (ISP_LOCKED_KEYBOARD & fsIspCleanUP)) {  /* if LOCKED falg on, then OK. */
            ResMsgH.sReturn = ISP_ERR_REQUEST_LOCKKBD;  /* Request lock keyboard */
        } else {
            if ((sRetStatus = IspLabelAnalysis(pPlu)) == ISP_SUCCESS) {
                ResMsgH.sReturn = IspCheckTtlPLU(pPlu->auchPluNo, pPlu->uchPluAdj); /* Saratoga */
                if ( ISP_SUCCESS == ResMsgH.sReturn ) {
                    ResMsgH.sReturn = SerOpPluDelete(pPlu, husIspOpHand);   /* Saratoga */
                    if ( OP_PERFORM == ResMsgH.sReturn ) {
                        fsIspCleanUP  |= ISP_CREATE_SUBINDEX_PLU;      /* Set flag */
                    }
                }
            } else {
				PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_LABEL_ANALYSIS_FAIL);
				PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sRetStatus));
				PifLog(MODULE_LINE_NO(MODULE_ISP_LOG), (USHORT)__LINE__);
				ResMsgH.sReturn = OP_PLU_ERROR;
            }
        }
        break;

    case CLI_FCOPPLUFILEUPD :   /* OP PLU  file update sub index  */
        if ( fsIspCleanUP & ISP_CREATE_SUBINDEX_PLU ) {
            PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_CHANGE_PLU);             /* Write log  */
            ResMsgH.sReturn = SerOpPluFileUpdate( husIspOpHand );
            fsIspCleanUP   &= ~ISP_CREATE_SUBINDEX_PLU;   /* Reset flag */
			PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_CHANGE_PLU);   /* Write log */
			PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(ResMsgH.sReturn));   /* Write log */
			if (IspIamMaster() == ISP_SUCCESS) {
				// we be Master so inform Satellite Terminals of PLU change
				NbWriteMessage(NB_MESOFFSET0, NB_REQPLU);
				PifSleep(1500);   // Give message time to be broadcast to cluster before responding back to requestor
				BlFwIfReloadOEPGroup();
			}
        }
        break;

    case CLI_FCOPPLUALLREAD :   /* OP PLU  all  read,           R3.0    */
		{
			// WARNING: In order to work correctly the variable PluIf must be static
			//          This variable is used to keep the current state when iterating over the list of PLUs.
			static PLUIF    PluIf;       /* saratoga */
			PLUIF           *pPluIf = (PLUIF *)IspResp.pSavBuff->auchData;

			if ((pPlu->ulCounter == 0) || ((fsIspLockedFC & ISP_LOCK_MASSMEMORY) == 0))  {  /* if locked previously */
				memset(&PluIf, 0, sizeof(PLUIF));

				if (fsIspLockedFC & ISP_LOCK_MASSMEMORY) {  /* if locked previously */
					IspCleanUpLockMas();                    /* clean up MMM, saratoga */
					pPluIf->ulCounter = 0;                   // ensure that counter is zero to force open file
					NHPOS_ASSERT_TEXT(0, "IspRecvOpr()  CLI_FCOPPLUALLREAD  fsIspLockedFC & ISP_LOCK_MASSMEMORY.")
				}
				PluIf.ulCounter = pPluIf->ulCounter;
			}

			IspResp.pSavBuff->usDataLen = 0; /* */
			for (i = 0; i < CLI_PLU_MAX_NO; i++) {
				ResMsgH.sReturn = SerOpPluAllRead(&PluIf, husIspOpHand);
				if (ResMsgH.sReturn != OP_PERFORM) {
					if (ResMsgH.sReturn == OP_EOF) {
						ResMsgH.sReturn = OP_PERFORM;
						fsIspLockedFC &= ~ISP_LOCK_MASSMEMORY;
					}
					break;
				} else {
					husIspMasHand  = PluIf.husHandle;
					fsIspLockedFC |= ISP_LOCK_MASSMEMORY;   /* Set MASSMEMORY */
				}
				*pPluIf = PluIf; pPluIf++; IspResp.pSavBuff->usDataLen += sizeof(PLUIF); /* */
			}
		}
        break;

    case CLI_FCOPPLUREADBRC :   /* OP PLU  read by report code, R3.0    */
		{
			// WARNING: In order to work correctly the variable PluIf_Rep must be static
			//          This variable is used to keep the current state when iterating over the list of PLUs.
			static PLUIF_REP   PluIf_Rep;
			PLUIF_REP          *pPluIf_Rep = (PLUIF_REP *)IspResp.pSavBuff->auchData;

			if ((pPluIf_Rep->culCounter == 0) || ((fsIspLockedFC & ISP_LOCK_MASSMEMORY) == 0))  {  /* if locked previously */
				memset(&PluIf_Rep, 0, sizeof(PLUIF_REP));
				if (fsIspLockedFC & ISP_LOCK_MASSMEMORY) {  /* if locked previously */
					IspCleanUpLockMas();                    /* clean up MMM, saratoga */
					pPluIf_Rep->culCounter = 0;             // ensure that counter is zero to force open file
					NHPOS_ASSERT_TEXT(0, "IspRecvOpr()  CLI_FCOPPLUREADBRC  fsIspLockedFC & ISP_LOCK_MASSMEMORY.")
				}
				PluIf_Rep.culCounter = pPluIf_Rep->culCounter;
				PluIf_Rep.uchReport = pPluIf_Rep->uchReport;
			}

			IspResp.pSavBuff->usDataLen = 0; /* */
			for (i = 0; i < CLI_PLU_MAX_NO; i++) {
				ResMsgH.sReturn = SerOpRprtPluRead(&PluIf_Rep, husIspOpHand);
				if (ResMsgH.sReturn != OP_PERFORM) {
					if (ResMsgH.sReturn == OP_EOF) {
						ResMsgH.sReturn = OP_PERFORM;
						fsIspLockedFC &= ~ISP_LOCK_MASSMEMORY;  /* saratoga */
					}
					break;
				} else {
					husIspMasHand  = PluIf_Rep.husHandle;       /* saratoga */
					fsIspLockedFC |= ISP_LOCK_MASSMEMORY;   /* Set MASSMEMORY */
				}
				*pPluIf_Rep = PluIf_Rep; pPluIf_Rep++;  IspResp.pSavBuff->usDataLen += sizeof(PLUIF_REP); /* */
			}
		}
        break;

    case CLI_FCOPPLUREADBDP :   /* OP PLU  read by dept number, R3.0    */
		{
			// WARNING: In order to work correctly the variable PluIf_Dep must be static
			//          This variable is used to keep the current state when iterating over the list of PLUs.
			static PLUIF_DEP  PluIf_Dep;   /* saratoga */
			PLUIF_DEP         *pPluIf_Dep = (PLUIF_DEP *)IspResp.pSavBuff->auchData;

			if ((pPluIf_Dep->culCounter == 0) || ((fsIspLockedFC & ISP_LOCK_MASSMEMORY) == 0))  {  /* if locked previously */
				memset(&PluIf_Dep, 0, sizeof(PLUIF_DEP));

				if (fsIspLockedFC & ISP_LOCK_MASSMEMORY) {  /* if locked previously */
					IspCleanUpLockMas();                    /* clean up MMM, saratoga */
					pPluIf_Dep->culCounter = 0;             // ensure that counter is zero to force open file
					NHPOS_ASSERT_TEXT(0, "IspRecvOpr()  CLI_FCOPPLUREADBDP  fsIspLockedFC & ISP_LOCK_MASSMEMORY.")
				}
				PluIf_Dep.culCounter = pPluIf_Dep->culCounter;
				PluIf_Dep.usDept = pPluIf_Dep->usDept;          /* Saratoga */
			}

			IspResp.pSavBuff->usDataLen = 0; /* */
			for (i = 0; i < CLI_PLU_MAX_NO; i++) {
				ResMsgH.sReturn = SerOpDeptPluRead(&PluIf_Dep, husIspOpHand);
				if (ResMsgH.sReturn != OP_PERFORM) {
					if (ResMsgH.sReturn == OP_EOF) {
						ResMsgH.sReturn = OP_PERFORM;
						fsIspLockedFC &= ~ISP_LOCK_MASSMEMORY;  /* saratoga */
					}
					break;
				} else {
					husIspMasHand  = PluIf_Dep.husHandle;       /* saratoga */
					fsIspLockedFC |= ISP_LOCK_MASSMEMORY;   /* Set MASSMEMORY */
				}
				*pPluIf_Dep = PluIf_Dep; pPluIf_Dep++; IspResp.pSavBuff->usDataLen += sizeof(PLUIF_DEP); /* */
			}
		}
        break;

    case CLI_FCOPLOCK :         /* OP lock  */
        if ( 0 == (fsIspLockedFC & ISP_LOCK_ALLOPEPARA ) ) {
            ResMsgH.sReturn = SerOpLock();
            if ( 0 <= ResMsgH.sReturn ) {
                husIspOpHand          = ResMsgH.sReturn;
                ResMsgH.usHndorOffset = husIspOpHand;
                fsIspLockedFC        |= ISP_LOCK_ALLOPEPARA;
            }
        }
        ResMsgH.sResCode  = STUB_SUCCESS;
        break;

    case CLI_FCOPUNLOCK :       /* OP unlock  */
        if ( fsIspLockedFC & ISP_LOCK_ALLOPEPARA) {
            IspCleanUpLockOp();
            IspCleanUpUpdatePLU();
        }
        ResMsgH.sResCode  = STUB_SUCCESS;
        break;

    case CLI_FCOPDEPTREADBMD :  /* OP read dept by majort dept #  */
        ResMsgH.sReturn = SerOpMajorDeptRead((MDEPTIF *)pPlu, husIspOpHand);
        break;

    case CLI_FCOPREQDISPMSG :  /* OP display a message to the operator  */
        ResMsgH.sReturn = SerOpDisplayOperatorMessage((CLIREQMSGDISP *)IspRcvBuf.auchData, (CLIRESMSGDISP *)&ResMsgH);
        break;

    case CLI_FCPARAALLREAD :   /* PARA all read  rameter */
        ResMsgH.sReturn = CliParaAllRead(pReqMsgH->uchMajorClass ,
                                         IspResp.pSavBuff->auchData,
                                         pReqMsgH->usRWLen,
                                         pReqMsgH->usHndorOffset,
                                         &ResMsgH.usRWLen);

        IspResp.pSavBuff->usDataLen = ResMsgH.usRWLen;

        if ( PARA_NOTOVER_RAMSIZE == ResMsgH.sReturn ) {
            ResMsgH.sReturn = ISP_SUCCESS;
        }
        break;

    case CLI_FCPARAALLWRITE :  /* PARA all write rameter */
        ResMsgH.sReturn = IspRecvCheckPara(pReqMsgH->uchMajorClass);    /* Check parameter */

        if ( ISP_SUCCESS == ResMsgH.sReturn  ) {
            if ( CLASS_PARAFLEXMEM != pReqMsgH->uchMajorClass ) {
                ResMsgH.sReturn = CliParaAllWrite(pReqMsgH->uchMajorClass,
                                                  IspResp.pSavBuff->auchData,
                                                  IspResp.pSavBuff->usDataLen,
                                                  pReqMsgH->usHndorOffset,
                                                  &ResMsgH.usRWLen);
            }
			if (ResMsgH.sReturn >= SUCCESS)
				ResMsgH.sReturn = IspAfterProcess(pReqMsgH->uchMajorClass);    /* if need, then execute */
        }
        ResMsgH.sResCode  = STUB_SUCCESS;

        /* --- FVT #5,  Jan/20/99 --- */
        if (pReqMsgH->uchMajorClass == CLASS_PARAMDC) {
            memset(IspResetLog.aszMnemonic, 0, sizeof(IspResetLog.aszMnemonic));
            _tcscat(IspResetLog.aszMnemonic, aszParaProg);
            IspRecvResetLogInternalNote(UIF_UCHRESETLOG_LOGONLY, 0);
            sFlag = 1;
        }
        if (pReqMsgH->uchMajorClass == CLASS_PARACTLTBLMENU) {
            memset(IspResetLog.aszMnemonic, 0, sizeof(IspResetLog.aszMnemonic));
            _tcscat(IspResetLog.aszMnemonic, aszParaSPV);
            IspRecvResetLogInternalNote(UIF_UCHRESETLOG_LOGONLY, 0);
        }
        /* --- FVT #5,  Jan/25/99 --- */
        if (pReqMsgH->uchMajorClass == CLASS_PARASTOREGNO && sFlag == 0) {
            memset(IspResetLog.aszMnemonic, 0, sizeof(IspResetLog.aszMnemonic));
            _tcscat(IspResetLog.aszMnemonic, aszParaStoReg);
            IspRecvResetLogInternalNote(UIF_UCHRESETLOG_LOGWITHTRAILER, 2);
        }
        /* --- FVT #5,  Jan/20/99 --- */

        if (pPifSaveFarData != NULL) {               /* saratoga */
            (*pPifSaveFarData)();
        }

        break;

    /************************* Add R3.0 *****************************/
    case CLI_FCOPCPNASSIGN :        /* OP Coupon assign          */
        if ( 0 == (ISP_LOCKED_KEYBOARD & fsIspCleanUP)) {  /* if LOCKED falg on, then OK. */
            ResMsgH.sReturn = ISP_ERR_REQUEST_LOCKKBD;  /* Request lock keyboard */
            break;
        }
        ResMsgH.sReturn = SerOpCpnAssign((CPNIF *)pPlu, husIspOpHand);
        break;

    case CLI_FCOPCPNINDREAD :       /* OP Coupon individual read */
        ResMsgH.sReturn = CliOpCpnIndRead((CPNIF *)pPlu, husIspOpHand);
        break;

    case CLI_FCOPCSTRASSIGN :       /* OP Control String assign          */
        if ( 0 == (ISP_LOCKED_KEYBOARD & fsIspCleanUP)) {  /* if LOCKED falg on, then OK. */
            ResMsgH.sReturn = ISP_ERR_REQUEST_LOCKKBD;  /* Request lock keyboard */
            break;
        }
        ResMsgH.sReturn = SerOpCstrAssign((CSTRIF *)pPlu, husIspOpHand);
        break;

    case CLI_FCOPCSTRDELETE :       /* OP Control String delete          */
        if ( 0 == (ISP_LOCKED_KEYBOARD & fsIspCleanUP)) {  /* if LOCKED falg on, then OK. */
            ResMsgH.sReturn = ISP_ERR_REQUEST_LOCKKBD;  /* Request lock keyboard */
        }
        ResMsgH.sReturn = SerOpCstrDelete((CSTRIF *)pPlu, husIspOpHand);
        break;

    case CLI_FCOPCSTRINDREAD :      /* OP Control String individual read */
        ResMsgH.sReturn = CliOpCstrIndRead((CSTRIF *)pPlu, husIspOpHand);
        break;

    case CLI_FCOPOEPPLUREAD :      /* OP OPE PLU read */
        ResMsgH.sReturn = CliOpOepPluRead((PLUIF_OEP *)pPlu, husIspOpHand, '20'); /* '20' passed because function was updated
																						but this call does not need parameter.
																						Parameter is to hold number of items
																						requested, in function, was a hard-coded 20,
																						so passing 20 should make no difference here - CSMALL */
        break;

    /************************* End Add R3.0 *************************/
    /******* Promotional Pricing Item (Release 3.1) BEGIN ******/
    case CLI_FCOPPPIASSIGN :        /* OP PPI assign          */
        if ( 0 == (ISP_LOCKED_KEYBOARD & fsIspCleanUP)) {  /* if LOCKED falg on, then OK. */
            ResMsgH.sReturn = ISP_ERR_REQUEST_LOCKKBD;  /* Request lock keyboard */
            break;
        }
        ResMsgH.sReturn = SerOpPpiAssign((PPIIF *)pPlu, husIspOpHand);
        break;

    case CLI_FCOPPPIDELETE :       /* OP PPI delete          */
        if ( 0 == (ISP_LOCKED_KEYBOARD & fsIspCleanUP)) {  /* if LOCKED falg on, then OK. */
            ResMsgH.sReturn = ISP_ERR_REQUEST_LOCKKBD;  /* Request lock keyboard */
        }
        ResMsgH.sReturn = SerOpPpiDelete((PPIIF *)pPlu, husIspOpHand);
        break;

    case CLI_FCOPPPIINDREAD :      /* OP PPI individual read */
        ResMsgH.sReturn = CliOpPpiIndRead((PPIIF *)pPlu, husIspOpHand);
        break;
    /******* Promotional Pricing Item (Release 3.1) END ******/

    case CLI_FCOPMLDASSIGN :       /* OP MLD Mnemonic assign, V3.3  */
        if ( 0 == (ISP_LOCKED_KEYBOARD & fsIspCleanUP)) {  /* if LOCKED falg on, then OK. */
            ResMsgH.sReturn = ISP_ERR_REQUEST_LOCKKBD;  /* Request lock keyboard */
            break;
        }
        ResMsgH.sReturn = SerOpMldAssign((MLDIF *)pPlu, husIspOpHand);
        break;

    case CLI_FCOPMLDINDREAD :      /* OP MLD Mnemonics Individual read */
        ResMsgH.sReturn = CliOpMldIndRead((MLDIF *)pPlu, husIspOpHand);
        break;

    case CLI_FCOPRSNASSIGN :       /* OP Reason Code Mnemonic assign, V3.3  */
        if ( 0 == (ISP_LOCKED_KEYBOARD & fsIspCleanUP)) {  /* if LOCKED falg on, then OK. */
            ResMsgH.sReturn = ISP_ERR_REQUEST_LOCKKBD;  /* Request lock keyboard */
            break;
        }
		{
			MLDIF           *pMldInfo = (MLDIF *)pPlu;
			OPMNEMONICFILE  MnemonicFile;

			MnemonicFile.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;
			MnemonicFile.usMnemonicAddress = pMldInfo->usAddress;
			memset (MnemonicFile.aszMnemonicValue, 0, sizeof(MnemonicFile.aszMnemonicValue));
			_tcsncpy (MnemonicFile.aszMnemonicValue, pMldInfo->aszMnemonics, OP_MLD_MNEMONICS_LEN);
			ResMsgH.sReturn = OpMnemonicsFileAssign (&MnemonicFile, husIspOpHand);
		}
        break;

    case CLI_FCOPRSNINDREAD :      /* OP Reason Code Mnemonics Individual read */
		{
			MLDIF           *pMldInfo = (MLDIF *)pPlu;
			OPMNEMONICFILE  MnemonicFile;

			MnemonicFile.usMnemonicFileId = OPMNEMONICFILE_ID_REASON;
			MnemonicFile.usMnemonicAddress = pMldInfo->usAddress;
			memset (MnemonicFile.aszMnemonicValue, 0, sizeof(MnemonicFile.aszMnemonicValue));
			ResMsgH.sReturn = OpMnemonicsFileIndRead(&MnemonicFile, husIspOpHand);
			memset (pMldInfo->aszMnemonics, 0, sizeof(pMldInfo->aszMnemonics));
			_tcsncpy (pMldInfo->aszMnemonics, MnemonicFile.aszMnemonicValue, OP_MLD_MNEMONICS_LEN);
		}
        break;

    case CLI_FCPLUENTERCRITMD:      /* PLU enter crite mode,    Saratoga */
        IspRecvMasEnterCM();
        return;

    case CLI_FCPLUEXITCRITMD:       /* PLU exit crite mode,     Saratoga */
        IspRecvMasMntCom();
        return;

    default:                                    /* not used */
		PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_RCVOPR_INV_FUN);
        return;
        break;
    }

	if (ResMsgH.sReturn < 0 && ResMsgH.sReturn != OP_EOF && !(ResMsgH.sReturn <= STUB_MULTI_SUCCESS && ResMsgH.sReturn >= STUB_REQ_CONFIRM)) {
		char  xBuff[128];

		sprintf (xBuff, "==NOTE: IspRecvOpr() usFuncCode 0x%x  uchMajorClass %d  sReturn %d  sResCode %d",
			(pReqMsgH->usFunCode & CLI_RSTCONTCODE), pReqMsgH->uchMajorClass, ResMsgH.sReturn, ResMsgH.sResCode);
		NHPOS_NONASSERT_NOTE("==NOTE", xBuff);

		PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_OPR_RQST_FAIL);
		PifLog(MODULE_DATA_VALUE(MODULE_ISP_LOG), (USHORT)(pReqMsgH->usFunCode & CLI_RSTCONTCODE));
		PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(ResMsgH.sReturn));
		PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(ResMsgH.sResCode));
	}

    if ( ResMsgH.sResCode == STUB_MULTI_SEND ) {
        if ( 0 <= ResMsgH.sReturn ) {
			// if there was no error and this is a multiple message response then
			// send the message. if there was an error we need to turn off multi-send
			// and just send the error response.
            IspSendMultiple((CLIRESCOM *)&ResMsgH, sizeof(CLIRESOPPARA));
            return;
        }
        ResMsgH.sResCode = STUB_SUCCESS;
    }

    IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESOPPARA), NULL, 0);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspRecvCheckPara(UCHAR uchMajorClass);
*     Input:    uchMajorClass  - Major Class code
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS             : Success.
*               ISP_ERR_INVALID_MESSAGE : Received Invalid parameter.
*               ISP_ERR_REQUEST_LOCKKBD : Request lock keyboard.
*
** Description: This function executes for check parameter.
*===========================================================================
*/
SHORT    IspRecvCheckPara(UCHAR uchMajorClass)
{
    SHORT   sError;

    if ( 0 == (ISP_LOCKED_KEYBOARD & fsIspCleanUP) ) {  /* if LOCKED falg on, then OK. */
        return(ISP_ERR_REQUEST_LOCKKBD);  /* Request lock keyboard */
    }

    switch ( uchMajorClass ) {
    case  CLASS_PARAFLEXMEM : ;   /* FLEXIBLE MEMORY ASSIGNMENT */
        sError = IspCheckDataFlexMem ((ISPFLEXMEM  *)IspResp.pSavBuff->auchData);
        if ( ISP_SUCCESS != sError ) {
			char  xBuff[128];

			sprintf (xBuff, "==ERROR: IspCheckDataFlexMem() uchMajorClass %d  sError = %d", uchMajorClass, sError);
			NHPOS_ASSERT_TEXT((ISP_SUCCESS == sError), xBuff);
            return(sError);       /* if data is over max, then error */
        }
        IspSavFlexMem();          /* Save Flex memory data */
        break;

    case  CLASS_PARASTOREGNO :      /* STORE/REGISTER NO.                 P- 7 */
        sError = ISP_SUCCESS;       /* V3.3 */
        break;

    case  CLASS_PARAPCIF :          /* PC INTERFACE                       P-39 */
        sError = ISP_ERR_INVALID_MESSAGE;
        return(sError);
        break;

    case  CLASS_PARAMDC :           /* SET MDC                            P- 1 */
    /*    CLASS_PARAFLEXMEM : */    /* FLEXIBLE MEMORY ASSIGNMENT         P- 2 */
    case  CLASS_PARAFSC :           /* SET FSC                            P- 3 */
    case  CLASS_PARASECURITYNO :    /* SECURITY NUMBER FOR PROGRAM MODE   P- 6 */
    case  CLASS_PARASUPLEVEL :      /* SUPERVISOR NUMBER/LEVEL ASSINMENT  P- 8 */
    case  CLASS_PARAACTCODESEC :    /* ACTION CODE SECURITY               P- 9 */
    case  CLASS_PARATRANSHALO :     /* TRANSACTION HALO                   P-10 */
    case  CLASS_PARAHOURLYTIME :    /* HOURLY ACTIVITY TIME               P-17 */
    case  CLASS_PARASLIPFEEDCTL :   /* SLIP PRINTER FEED CONTROL          P-18 */
    case  CLASS_PARATRANSMNEMO :    /* TRANSACTION MNEMONICS              P-20 */
    case  CLASS_PARALEADTHRU :      /* LEAD THROUGH TABLE                 P-21 */
    case  CLASS_PARAREPORTNAME :    /* REPORT NAME                        P-22 */
    case  CLASS_PARASPECIALMNEMO :  /* SPECIAL MNEMONICS                  P-23 */
    case  CLASS_PARAADJMNEMO :      /* ADJECTIVE MNEMONICS                P-46 */
    case  CLASS_PARAPRTMODI :       /* PRINT MODIFIER MNEMONICS           P-47 */
    case  CLASS_PARAMAJORDEPT :     /* REPORT CODE MNEMONICS              P-48 */
    case  CLASS_PARAAUTOALTKITCH :  /* AUTO ALTERNATIVE KITCHEN PTR.      P-49 */
    case  CLASS_PARACHAR24 :        /* 24 CHARACTERS MNEMONICS            P-57 */
    case  CLASS_PARATTLKEYTYP :     /* TOTAL KEY TYPE ASSIGNMENT          P-60 */
    case  CLASS_PARATTLKEYCTL :     /* TOTAL TYPE KEY CONTROL             P-61 */
	case CLASS_PARATTLKEYORDERDEC:
    case  CLASS_PARATEND :          /* TENDER PARAMETER,    V3.3          P-62 */
    case  CLASS_PARAAUTOCPN :	    /* P 67		*/
    case  CLASS_TENDERKEY_INFO :    // XEPT tender information          P-98
	case  CLASS_PARATERMINALINFO:	// Terminal information Rel 2.0
    case  CLASS_PARASHRPRT :        /* SHARED PRINT PORT (Enhance)        P-50H */
    case  CLASS_PARAKDSIP:          /* KDS IP, Port #,      Saratoga      P-51  */
    case  CLASS_PARAPRESETAMOUNT :  /* Preset AMOUNT     (Enhance)        P-15H */
	case CLASS_FILEVERSION_INFO:
	case CLASS_PARAREASONCODE:
        sError = ISP_SUCCESS;
        break;

    case  CLASS_PARAPLUNOMENU :     /* SET PLU NO.ON MENU PAGE  (S)       S-  4 */
    case  CLASS_PARACTLTBLMENU :    /* SET CONTROL TABLE OF MENU PAGE (S) S-  5 */
    case  CLASS_PARAMANUALTKITCH :  /* MANUAL ALTERNATIVE KITCHEN PTR.    S-  6 */
    case  CLASS_PARACASHABASSIGN :  /* CASHIER A/B KEYS ASSIGNMENT (S)    S-  7 */

    case  CLASS_PARAROUNDTBL :      /* SET ROUNDING TABLE (S)             S- 84 */
    case  CLASS_PARADISCTBL :       /* DISCOUNT/BONUS % RATE (S)          S- 86 */
    case  CLASS_PARAPROMOTION :     /* SET SALES PROMOTION MESSAGE (S)    S- 88 */
    case  CLASS_PARACURRENCYTBL :   /* SET CURRENCY CONVERSION RATE (S)   S- 89 */
    case  CLASS_PARATARE :          /* SET TARE                           S-111 */
    case  CLASS_PARAMENUPLUTBL :    /* SET SET MENU OF PLU (S)            S-116 */

    case  CLASS_PARATAXTBL1 :       /* TAX TABLE PROGRAMMING (S)          S-124 */
    case  CLASS_PARATAXTBL2 :       /* TAX TABLE PROGRAMMING (S)          S-125 */
    case  CLASS_PARATAXTBL3 :       /* TAX TABLE PROGRAMMING (S)          S-126 */
    case  CLASS_PARATAXRATETBL :    /* SET TAX/SERVICE/RECIPROCAL RATES   S-127 */
    case  CLASS_PARAMISCPARA :      /* SET TRANSACTION LIMIT              S-128 */
    case  CLASS_PARASTOREFORWARD :  /* SET TRANSACTION LIMIT              S-128 */
    case  CLASS_PARATONECTL :       /* SET TONE VOLUME DATA               S-169 */
    case  CLASS_PARARESTRICTION:    /* SET SALES RESTRICTION (S),Saratoga S-172 */
    case  CLASS_PARABOUNDAGE:       /* SET BOUNDARY AGE (S),     Saratoga S-208 */
    case  CLASS_PARADEPTNOMENU:     /* Set Dept NO.ON MENU PAGE, Saratoga S-115 */
    case  CLASS_PARASOFTCHK :       /* Trailer Print for Soft Check AC87  S-87H  */
    case  CLASS_PARAHOTELID :       /* Hotel ID                           P-54HO */
    case  CLASS_PARAOEPTBL :        /* Order Entry Prompt Table           S-160  */
    case  CLASS_PARAFXDRIVE :       /* Flexible Drive Through             S-162  */
    case  CLASS_PARATAXTBL4 :       /* TAX TABLE PROGRAMMING (S) R3.1     S-128  */
    case  CLASS_PARAPIGRULE :       /* SET PIG RULE TABLE        R3.1     S-129  */
    case  CLASS_PARALABORCOST:      /* LABOR COST VALUE          R3.1            */
    case  CLASS_PARADISPPARA :      /* BEVERAGE DISPENSER PARAMETER R3.1         */
    case  CLASS_PARAPPI      :      /* SET PPI TABLE (S)         R3.1            */
    case  CLASS_PARASERVICETIME:    /* SET SERVICE TIME PARAM    R3.1            */
	case  CLASS_PARACOLORPALETTE:	/* SET COLOR PALETTE						 */
        sError = ISP_SUCCESS;
        break;

    default :

    /* CLASS_PARAGUESTNO :    /*   /* GUEST CHECK NO. ASSIGNMENT         S-  1 */
    /* CLASS_PARADEPT :       /*   /* MAINTENANCE DEPARTMENT             S-114 */
    /* CLASS_PARAPLU :        /*   /* ADD/DELETE/MAINTENANCE PLU         S- 63 */
    /* CLASS_PARACASHIERNO :  /*   /* CASHIER NO. ASSIGNMENT             S- 20 */
    /* CLASS_PARAWAITERNO :   /*   /* WAITER NO. ASSIGNMENT              S- 50 */

        sError = ISP_ERR_INVALID_MESSAGE;
        break;
    }

    return(sError);
}

/*
*===========================================================================
** Synopsis:    SHORT   IspAfterProcess(UCHAR uchMajorClass);
*     Input:    uchMajorClass
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function executes after process.
*===========================================================================
*/
SHORT     IspAfterProcess(UCHAR uchMajorClass)
{
    SHORT   sError;
	SYSCONFIG *pSysConfig;
	PARATERMINALINFO TermInfo;
	PARACTLTBLMENU   MenuPage;
	PARASTOREGNO    ParaRegNo;

    sError = ISP_SUCCESS;

	// because PifSysConfig returns a pointer to a const, the following
	// statement will issue a warning about assignment of a pointer to
	// a const to a non-const pointer variable.  Ignore this warning.
	//		warning C4090: '=' : different 'const' qualifiers
	pSysConfig = PifSysConfig();

    switch ( uchMajorClass ) {

    case  CLASS_PARAFLEXMEM : ;   /* FLEXIBLE MEMORY ASSIGNMENT */
        sError = IspCreateFlexMem();       /* Create Flex file */
        break;

    case  CLASS_PARATONECTL :       /* SET TONE VOLUME DATA               S-169 */
        ApplSetupDevice(APPL_AUDIBLE_TONE);  /* Set tone volume */
        break;

    case CLASS_PARATERMINALINFO:
		/*
			Setup and initialize the correct keyboard type.  We support the following
			configurations and keyboards:
				NCR 7448 Terminal with the Conventional keyboard
				NCR 7448 Terminal with the Micromotion keyboard
				NCR Terminal with the NCR 5932 Wedge keyboard with 68 keys
				NCR Touchscreen terminal with standard 102 keyboard

			The initialization of SysConfig assumes a micromotion keyboard.
			Keyboard type is a terminal dependent parameter set in the terminal
			information within the UNINIRAM.

			We also set the register number and the store number to be what has
			been provisioned in UNINIRAM.

		 */

		// guard the terminal position to ensure it is in correct range, default is we be Master
		if (!(PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName) >= 1 && PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName) <= PIF_NET_MAX_IP))
			pSysConfig->usTerminalPositionFromName = 1;

		TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
		TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);

		ParaTerminalInfoParaRead (&TermInfo);

		pSysConfig->unCodePage = TermInfo.TerminalInfo.ulCodePage;
		pSysConfig->uchKeyType = TermInfo.TerminalInfo.uchKeyBoardType;

		// Must also update the registry information for the keyboard so that
		// on next startup, the change will be there.
		PifSetKeyboardType (pSysConfig->uchKeyType);

		// Override the default menu page in uniniram with the per terminal number
		MenuPage.uchMajorClass = CLASS_PARACTLTBLMENU;
		MenuPage.uchPageNumber = TermInfo.TerminalInfo.uchMenuPageDefault;
		MenuPage.uchAddress = 1;
		ParaCtlTblMenuWrite(&MenuPage);

		// Must also update the Store Number and Register No in UNINIRAM for this
		// terminal.
        ParaRegNo.uchMajorClass = CLASS_PARASTOREGNO;
        ParaRegNo.uchAddress = 1;
		ParaRegNo.usStoreNo = TermInfo.TerminalInfo.usStoreNumber;
		ParaRegNo.usRegisterNo = TermInfo.TerminalInfo.usRegisterNumber;
		CliParaWrite (&ParaRegNo);
		break;

    case  CLASS_PARACTLTBLMENU :    /* SET CONTROL TABLE OF MENU PAGE (S) S-  5 */
		// guard the terminal position to ensure it is in correct range, default is we be Master
//		if (!(PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName) >= 1 && PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName) <= 16))
//			pSysConfig->usTerminalPositionFromName = 1;

		TermInfo.uchMajorClass = CLASS_PARATERMINALINFO;
		TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);

		ParaTerminalInfoParaRead (&TermInfo);
		// Override the default menu page in uniniram with the per terminal number
		MenuPage.uchMajorClass = CLASS_PARACTLTBLMENU;
		MenuPage.uchPageNumber = TermInfo.TerminalInfo.uchMenuPageDefault;
		MenuPage.uchAddress = 1;
		ParaCtlTblMenuWrite(&MenuPage);
		break;

    case  CLASS_PARAMDC :    /* Initialize any additional status indicated by changed MDC bits */
		break;

    default :
        break;
    }
    return(sError);
}

/*
*===========================================================================
** Synopsis:    VOID     IspRecvMasEnterCM(VOID)
*
*     Input:    nothing
*     Output:   nothing
*
** Return:      nothing
*
** Description:
*   This function executes to enter crit mode.
*===========================================================================
*/
VOID    IspRecvMasEnterCM(VOID)
{
    CLIREQPLUECM   *pReqMsgH;
    CLIRESPLUECM   ResMsgH;

    pReqMsgH = (CLIREQPLUECM *)IspRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESPLUECM));
    ResMsgH.usFunCode     = pReqMsgH->usFunCode;
    ResMsgH.sResCode      = STUB_SUCCESS;
    ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;

    if (0 == ( fsIspLockedFC & ISP_LOCK_MASSMEMORY) ) {
        switch (pReqMsgH->usFunc) {
        case FUNC_INDIV_LOCK:    /* indiv. access with lock */
        case FUNC_REPORT_RANGE:  /* report by range         */
        case FUNC_REPORT_ADJECT: /* report by adjective item */
        case FUNC_APPLY_MAINTE:  /* apply mainte. reocrds   */
        case FUNC_PURGE_MAINTE:  /* purge mainte. records   */
            ResMsgH.sReturn = SerIspPluEnterCritMode(pReqMsgH->usFile,
                                                     pReqMsgH->usFunc,
                                                     &ResMsgH.usHandle,
                                                     pReqMsgH->auchPara);
            break;

        case FUNC_REPORT_COND:   /* report by conditional   */
            ResMsgH.sReturn = SerIspPluEnterCritMode(pReqMsgH->usFile,
                                                     pReqMsgH->usFunc,
                                                     &ResMsgH.usHandle,
                                                     pReqMsgH->auchPara);
            break;

        case FUNC_REPORT_MAINTE: /* report mainte. records  */
            ResMsgH.sReturn = SerIspPluEnterCritMode(pReqMsgH->usFile,
                                                     pReqMsgH->usFunc,
                                                     &ResMsgH.usHandle,
                                                     pReqMsgH->auchPara);
            break;

        default:
			PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_RCVMS_INV_LK);
            return;
            break;
        }

        if (SPLU_COMPLETED == ResMsgH.sReturn) {
            husIspMasHand  = ResMsgH.usHandle ;
            fsIspLockedFC |= ISP_LOCK_MASSMEMORY;   /* Set MASSMEMORY */
        }

    } else {
        ResMsgH.sReturn = SPLU_TOO_MANY_USERS;  /* if alredy locked, then too many users */
    }

    IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESPLUECM) , NULL, 0);
}

/*
*===========================================================================
** Synopsis:    VOID     IspRecvMasMntCom(VOID)
*
*     Input:    nothing
*     Output:   nothing
*
** Return:      nothing
*
** Description:
*   This function executes to mainte common interface.          Saratoga
*===========================================================================
*/
VOID    IspRecvMasMntCom(VOID)
{
    CLIREQPLUCOM   *pReqMsgH;
    CLIRESPLUCOM   ResMsgH;

    pReqMsgH = (CLIREQPLUCOM *)IspRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESPLUCOM));
    ResMsgH.usFunCode     = pReqMsgH->usFunCode;
    ResMsgH.sResCode      = STUB_SUCCESS;
    ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {
    case    CLI_FCPLUEXITCRITMD:                /* PLU exit crit mode */
        if ( fsIspLockedFC & ISP_LOCK_MASSMEMORY ) {
            ResMsgH.sReturn = IspCleanUpLockMas();
        } else {
            ResMsgH.sReturn = SPLU_COMPLETED;   /* if unlocked, then ok */
        }
        break;

    default:
		PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_RCVMSMNT_INV_FUN);
        return;
        break;
    }

    IspSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESPLUCOM), NULL, 0);
}


/*
*===========================================================================
**Synopsis: SHORT   IspLabelAnalysis(PLUIF *pPlu);
*
*   Input:  none
*
*   Output: none
*
*   InOut:  PLUIF       pPlu
*
**Return:   ISP_SUCCESS
*           LDT_???_ADR - return error code if it has occerred
*
** Description: This function checkes the input label type
*               and executes the appropriate procedure.
*===========================================================================
*/
SHORT   IspLabelAnalysis(VOID *pData)
{
	LABELANALYSIS   PluLabel = {0};
    SHORT           i;
    PLUIF           *pPlu = (PLUIF *)pData;

    for (i = 0; i < OP_PLU_LEN; i++) {
        if ((pPlu->auchPluNo[i] == 0) && (i > 0)) {
            break;
        }

        if ((pPlu->auchPluNo[i] < '0') || (pPlu->auchPluNo[i] > '9')) {
            return(LABEL_DIGIT_ERR);
        }
    }

    /* ----- set plu number for label analysis ----- */
    _tcsncpy(PluLabel.aszScanPlu, pPlu->auchPluNo, OP_PLU_LEN);

    /* ----- analyze PLU number from UI ----- */
    if (RflLabelAnalysis(&PluLabel) == LABEL_OK) {
        if (PluLabel.uchType == LABEL_RANDOM) {
            if (PluLabel.uchLookup & LA_EXE_LOOKUP) {
                _tcsncpy(pPlu->auchPluNo, PluLabel.aszMaskLabel, OP_PLU_LEN);
            } else {
                return(LABEL_TYPE_ERR);
            }
        } else if (PluLabel.uchType == LABEL_COUPON) {
            return(LABEL_TYPE_ERR);
        }
        else {
            _tcsncpy(pPlu->auchPluNo, PluLabel.aszLookPlu, OP_PLU_LEN);
        }
    } else {
        return(LABEL_ANALYS_ERR);
    }

    return (ISP_SUCCESS);
}


/*===== END OF SOURCE =====*/