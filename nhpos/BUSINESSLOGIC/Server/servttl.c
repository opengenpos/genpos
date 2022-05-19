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
* Title       : SERVER module, Total Functions Source File
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVTTL.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           SerRecvTtl();           Total function handling
*           
*           SerChekTtlUpdate();     Total update function
*           SerChekTtlReset();      Total reset EOD, PTD function
*           SerChekTtlResetCas();   Total reset of cashier function
*           SerChekTtlResetWai();   Total reset of waiter function
*           SerChekTtlResetIndFin() Total reset of individual financial
*           SerChekTranNo();        Check transaction #
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description 
* May-07-92:H.Nakashima: initial                                   
* Jun-04-93:H.Yamaguchi: Modified same as HP Int'l
* Mar-16-94:K.You      : Add flex GC file feature(mod. SerChekTtlUpdate).
* Nov-14-95:T.Nakahata : R3.1 Initial
*       Add Service Time Total (Daily/PTD) and Individual Financial (Daily)
* Aug-11-99:M.Teraki   : R3.5 remove WAITER_MODEL
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-03-99 : 01.00.00 : hrkato     : Saratoga
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
#include    <math.h>

#include    <ecr.h>
#include    <appllog.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <para.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csstbpar.h>
#include    <csserm.h>
#include    <csttl.h>
#include    <cscas.h>
#include    <cswai.h>
#include    <csgcs.h>
#include    "servmin.h"
#include    "csstbttl.h"
#include	<ispcom.h>
#include "PluTtlD.h"
#include	"regstrct.h"
#include	"ttl.h"
#include	"csop.h"
/*
*===========================================================================
** Synopsis:    VOID    SerRecvTtl(VOID);
*
** Return:      none.
*
** Description: This function executes for Total function request.
*===========================================================================
*/
VOID    SerRecvTtl(VOID)
{
    CLIREQTOTAL     *pReqMsgH;
    CLIREQRESETTL   *pReqRstH;
    CLIREQCASTEND   *pReqCasH;
    CLIREQDATA      *pSavBuff, *pRcvBuff;
    CLIRESTOTAL     ResMsgH;
    CLIRESCASTEND   ResCasH;
    SHORT           sError, i;
	USHORT          usDataLength = 0;
	SHORT           sSerSendStatus;

    pReqMsgH = (CLIREQTOTAL *)SerRcvBuf.auchData;
    pReqRstH = (CLIREQRESETTL *)SerRcvBuf.auchData;
    pReqCasH = (CLIREQCASTEND *)SerRcvBuf.auchData;

    memset(&ResMsgH, 0, sizeof(CLIRESTOTAL));
    ResMsgH.usFunCode = pReqMsgH->usFunCode;
    ResMsgH.sResCode  = STUB_SUCCESS;
    ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;

    memset(&ResCasH, 0, sizeof(CLIRESCASTEND));
    ResCasH.usFunCode = pReqCasH->usFunCode;
    ResCasH.sResCode  = STUB_SUCCESS;
    ResCasH.usSeqNo   = pReqCasH->usSeqNo & CLI_SEQ_CONT;

    sError = STUB_SUCCESS;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {
    case    CLI_FCTTLUPDATE:
        sError = SerChekTtlUpdate(pReqMsgH, &ResMsgH);
        break;

    case    CLI_FCTTLRESETTL:
        sError = SerChekTtlReset(pReqRstH, &ResMsgH);
        break;

    case    CLI_FCTTLRESETCAS:
        sError = SerChekTtlResetCas(pReqRstH, &ResMsgH);
        break;

    case    CLI_FCTTLRESETINDFIN:   /* Individual Financial, R3.1 */
        sError = SerChekTtlResetIndFin(pReqRstH, &ResMsgH);
        break;

    case    CLI_FCTTLISSUE:
        ResMsgH.sReturn = TtlIssuedSet(&pReqRstH->uchMajorClass, 
                                       pReqRstH->fIssued);
        break;

    case    CLI_FCTTLISSUERST:
        ResMsgH.sReturn = TtlIssuedReset(&pReqRstH->uchMajorClass, 
                                         pReqRstH->fIssued);
        break;

    case    CLI_FCRESETTRANSNO:
        SstResetIndTransNo(SerRcvBuf.auchFaddr[CLI_POS_UA]);
        ResMsgH.sReturn = TTL_SUCCESS;
        break;

	case    CLI_FCTTLREADTLINCR:                /* Saratoga */
        pSavBuff = (CLIREQDATA *)&SerRcvBuf.auchData[sizeof(CLIREQRESETTL)];
        pRcvBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESTOTAL)];
        memcpy(pRcvBuff->auchData, pSavBuff->auchData, pSavBuff->usDataLen);

        ResMsgH.sReturn = SerTtlTotalIncrRead(pRcvBuff->auchData);

        if (TTL_SUCCESS == ResMsgH.sReturn) {
            pRcvBuff->usDataLen = sizeof(TTLCASHIER);
            SerResp.pSavBuff = pRcvBuff;
            ResMsgH.sResCode = STUB_MULTI_SEND;
        } else {
            pRcvBuff->usDataLen = 0;
            ResMsgH.sResCode = STUB_SUCCESS;
        }
        break;

    case    CLI_FCTTLREADTL:                /* Saratoga */
        pSavBuff = (CLIREQDATA *)&SerRcvBuf.auchData[sizeof(CLIREQRESETTL)];
        pRcvBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESTOTAL)];
        memcpy(pRcvBuff->auchData, pSavBuff->auchData, pSavBuff->usDataLen);

        ResMsgH.sReturn = TtlTotalRead(pRcvBuff->auchData, &usDataLength);

        if (TTL_SUCCESS == ResMsgH.sReturn || TTL_NOTINFILE == ResMsgH.sReturn) {
            pRcvBuff->usDataLen = usDataLength;
			SerResp.pSavBuff = pRcvBuff;
            ResMsgH.sResCode = STUB_MULTI_SEND;
        } else {
            pRcvBuff->usDataLen = 0;
            ResMsgH.sResCode = STUB_SUCCESS;
        }
        break;

    case    CLI_FCTTLREADTLCASTEND:         /* Saratoga */
		{
			TTLCASTENDER    TtlCas;

			memset(&TtlCas, 0, sizeof(TTLCASTENDER));
			TtlCas.ulCashierNumber = pReqCasH->ulCashierNumber;
			ResMsgH.sReturn = TtlTenderAmountRead(&TtlCas);
			if (TTL_SUCCESS == ResMsgH.sReturn) {
				ResCasH.ulCashierNumber = TtlCas.ulCashierNumber;
				ResCasH.lCashTender     = TtlCas.lCashTender;
				ResCasH.lCheckTender    = TtlCas.lCheckTender;
				ResCasH.lChargeTender   = TtlCas.lChargeTender;
				for (i = 0; i < sizeof(TtlCas.lMiscTender)/sizeof(TtlCas.lMiscTender[0]); i++) {
					ResCasH.lMiscTender[i]  = TtlCas.lMiscTender[i];
				}
				for (i = 0; i < sizeof(TtlCas.lForeignTotal)/sizeof(TtlCas.lForeignTotal[0]); i++) {
					ResCasH.lForeignTotal[i]= TtlCas.lForeignTotal[i];
				}
			}
		}
        break;

    case    CLI_FCTTLPLUOPTIMIZE:	/* 06/26/01 */
        ResMsgH.sReturn = TtlPLUOptimize(pReqRstH->uchMinorClass);
        break;

    case    CLI_FCTTLDELETETTL:
        sError = SerChekTtlDelete(pReqRstH, &ResMsgH);
        break;

    case CLI_FCTTLREADTLPLUEX :   /* TTL plural PLU read,  Saratoga*/
        sError = SerRecvTtlReadPluEx(pReqRstH);
        if ( 0 <= sError ) {
            ResMsgH.sResCode = STUB_MULTI_SEND;
        }
        break;

    default:                                    /* not used */
        // Issue a PifLog() to record that we have an error condition then
        // return an error to the sender.  This is a bad function code.
        PifLog (MODULE_SER_LOG, LOG_EVENT_SER_TTL_BAD_FUNCODE);
        sError = STUB_ILLEGAL; 
        break;
    }
    if (STUB_SUCCESS != sError) {
        SerSendError(sError);
    } else 
	{
		if (STUB_MULTI_SEND == ResMsgH.sResCode)
		{
			if ((pReqMsgH->usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLREADTLCASTEND) {
				sSerSendStatus = SerSendMultiple((CLIRESCOM *)&ResMsgH, sizeof(CLIRESTOTAL));
				if (sSerSendStatus < 0) {
					char xBuff [128];
					sprintf (xBuff, "usFunCode = 0x%x, usSeqNo = 0x%x, sSerSendStatus = %d", ResMsgH.usFunCode, ResMsgH.usSeqNo, sSerSendStatus);
					NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
				}
			} else {
				sSerSendStatus = SerSendMultiple((CLIRESCOM *)&ResMsgH,sizeof(CLIRESTOTAL));
				if (sSerSendStatus < 0) {
					char xBuff [128];
					sprintf (xBuff, "usFunCode = 0x%x, usSeqNo = 0x%x, sSerSendStatus = %d", ResMsgH.usFunCode, ResMsgH.usSeqNo, sSerSendStatus);
					NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
				}
			}
		}
		else
		{
			if ((pReqMsgH->usFunCode & CLI_RSTCONTCODE) == CLI_FCTTLREADTLCASTEND) {
				sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResCasH, sizeof(CLIRESCASTEND), NULL, 0);
				if (sSerSendStatus < 0) {
					char xBuff [128];
					sprintf (xBuff, "usFunCode = 0x%x, usSeqNo = 0x%x, sSerSendStatus = %d", ResCasH.usFunCode, ResCasH.usSeqNo, sSerSendStatus);
					NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
				}
			} else {
				sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESTOTAL), NULL, 0);
				if (sSerSendStatus < 0) {
					char xBuff [128];
					sprintf (xBuff, "usFunCode = 0x%x, usSeqNo = 0x%x, sSerSendStatus = %d", ResMsgH.usFunCode, ResMsgH.usSeqNo, sSerSendStatus);
					NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
				}
			}
		}
    }
}

/*
*===========================================================================
** Synopsis:    SHORT    SerChekTtlUpdate(CLIREQTOTAL *pReqMsgH,
*                                         CLIRESTOTAL *pResMsgH);
*     Input:    pReqMsgH    - request from satellite
*               pResMsgH    - response to satellite
*
** Return:      STUB_SUCCESS:       All data received
*               STUB_MULTI_RECV:    still exist data on satellite
*               STUB_MULTI_SUCCESS: this inf. is already updated 
*               STUB_UNMATCH_TRANNO:    unmatched transaction No.
*               STUB_FRAME_SEQERR:  frame sequence error
*
** Description: This function executes for Total update function request.
*===========================================================================
*/
SHORT   SerChekTtlUpdate(CLIREQTOTAL *pReqMsgH,
                         CLIRESTOTAL *pResMsgH)
{
	static SHORT    ResMsgHsReturnSave = TTL_SUCCESS;  // status save to reduce repeated logs.
	SERINQSTATUS    InqData = {0};
    USHORT          usTransNoHeHas, usTransNoIHave, usUA;
    USHORT          usPreTransNoIHave, usReqMsgHLen;
	SHORT           sRetStatus = STUB_SUCCESS;
	UCHAR			uchDelayBalance = 0;

    usUA = SerRcvBuf.auchFaddr[CLI_POS_UA];
    SstReadInqStat(&InqData);
    usTransNoIHave = InqData.ausTranNo[usUA-1];
    usPreTransNoIHave = InqData.ausPreTranNo[usUA-1];
    usTransNoHeHas = pReqMsgH->usTransNo;

    if (0 == usTransNoIHave) {       /* After EOD */
        pResMsgH->usTransNo = usTransNoHeHas;       /* Change 1 -> usTransNoHeHas, Dec/1/2000 */
    } else if (usTransNoHeHas == (usTransNoIHave + 1)) {
        pResMsgH->usTransNo = usTransNoHeHas;
    } else if (usTransNoHeHas == usTransNoIHave) {
        pResMsgH->usTransNo = usTransNoHeHas;
        pResMsgH->sReturn = TTL_SUCCESS;
        return STUB_SUCCESS;
    } else if ((0 == (usTransNoIHave + 1)) && (1 == usTransNoHeHas)) {  /* Add New Check, Dec/1/2000 */
        pResMsgH->usTransNo = usTransNoHeHas;
    } else {                                /* unmatched transaction # */
		DATE_TIME           dtCurrentDateTime;
		BOOL                bPrintAssert = 0;
		static DATE_TIME    dtSaveDateTime = {0};
		static USHORT       usSaveUA = 0;

		PifGetDateTime(&dtCurrentDateTime);
		bPrintAssert = ((usSaveUA != usUA) || (memcmp (&dtSaveDateTime, &dtCurrentDateTime, sizeof(DATE_TIME)) != 0));
		dtSaveDateTime = dtCurrentDateTime;
		usSaveUA = usUA;

		if (bPrintAssert) {
			char  xBuff[128];

			sprintf (xBuff, "SerChekTtlUpdate(): STUB_UNMATCH_TRNO usUA %d, usTransNoIHave %d, usTransNoHeHas %d", usUA, usTransNoIHave, usTransNoHeHas);
			NHPOS_ASSERT_TEXT(0, xBuff);
		}
        return STUB_UNMATCH_TRNO;
    }

    usReqMsgHLen = SerChekFunCode(pReqMsgH->usFunCode);
	if(uchTtlUpdDelayBalanceFlag)
	{
		return	TTL_UPD_DB_BUSY;
	}

    pResMsgH->sReturn = TtlUpdateFileFH((SERTMPFILE_DATASTART + usReqMsgHLen), hsSerTmpFile, (USHORT)(SerResp.pSavBuff->usDataLen), &uchDelayBalance);  /* ### Mod (2171 for Win32) V1.0 */

	if (TTL_BUFFER_FULL == pResMsgH->sReturn) {
		if (ResMsgHsReturnSave != pResMsgH->sReturn) {
			PifLog (MODULE_SER_LOG, LOG_EVENT_SER_TTOTALUPDATE_FAILED);
			PifLog (MODULE_ERROR_NO(MODULE_SER_LOG), (USHORT)abs(pResMsgH->sReturn));
			PifLog (MODULE_LINE_NO(MODULE_SER_LOG), (USHORT)__LINE__);
		}
		// If the TOTALUPD file is full, then lets give it another try before giving up.
		PifSleep(150);
		pResMsgH->sReturn = TtlUpdateFileFH((SERTMPFILE_DATASTART + usReqMsgHLen), hsSerTmpFile, (USHORT)(SerResp.pSavBuff->usDataLen), &uchDelayBalance);  /* ### Mod (2171 for Win32) V1.0 */
	}

    if (TTL_SUCCESS == pResMsgH->sReturn) {
        SstIncTransNo(SerRcvBuf.auchFaddr[CLI_POS_UA], usTransNoHeHas);
    }                                       /* wait on satellite */
	else {
		if (ResMsgHsReturnSave != pResMsgH->sReturn) {
			char  xBuff[128];

			PifLog (MODULE_SER_LOG, LOG_EVENT_SER_TTOTALUPDATE_FAILED);
			PifLog (MODULE_ERROR_NO(MODULE_SER_LOG), (USHORT)abs(pResMsgH->sReturn));
			PifLog (MODULE_LINE_NO(MODULE_SER_LOG), (USHORT)__LINE__);
			sprintf (xBuff, "SerChekTtlUpdate(): TtlUpdateFileFH() unsuccessful %d, SstIncTransNo() with %d not done.", pResMsgH->sReturn, usTransNoHeHas);
			NHPOS_ASSERT_TEXT((TTL_SUCCESS == pResMsgH->sReturn), xBuff);
		}
	}
	// save this response in order to reduce repeated logs from retries that happen about once a second.
	ResMsgHsReturnSave = pResMsgH->sReturn;

    if (TTL_BUFFER_FULL == pResMsgH->sReturn) {
		//if there is a value for uchDelayBalance, that means we are writing to
		//the Delay Balance file, and we must return that it is a delay balance full.
		if(uchDelayBalance)
		{
			sRetStatus = STUB_DB_FULL;
		}else
		{
			sRetStatus = STUB_BUSY;
		}
    }

	if(TTL_NEAR_FULL == pResMsgH->sReturn)
	{
		sRetStatus = STUB_DB_NEARFULL;
	}
    if (TTL_LOCKED == pResMsgH->sReturn) {
        sRetStatus = STUB_BUSY;
    }
    if (TTL_FAIL == pResMsgH->sReturn) {
        sRetStatus = STUB_FAITAL;
    }
    return sRetStatus;
}

/*
*===========================================================================
** Synopsis:    SHORT    SerChekTtlReset(CLIREQRESETTL *pReqMsgH,
*                                        CLIRESTOTAL *pResMsgH);
*     Input:    pReqMsgH    - request from satellite
*               pResMsgH    - response to satellite
*
** Return:      STUB_SUCCESS:       All data received
*               STUB_BUSY:          I am not ready to reset
*               STUB_UNMATCH_TRANNO:    unmatched transaction No.
*
** Description: 
*  This function executes for Total reset EOD, PTD function request.
*===========================================================================
*/
SHORT   SerChekTtlReset(CLIREQRESETTL *pReqMsgH, CLIRESTOTAL *pResMsgH)
{
    SHORT   sCasError;

    sCasError = CasAllLock();                   /* cashier all lock */
    if ((CAS_PERFORM != sCasError) &&
        (CAS_FILE_NOT_FOUND != sCasError)) {
        return STUB_BUSY;
    }

    if (((CLASS_TTLEODRESET == pReqMsgH->uchMajorClass) &&
         (CliParaMDCCheck(MDC_EOD1_ADR, EVEN_MDC_BIT2))) ||
        ((CLASS_TTLPTDRESET == pReqMsgH->uchMajorClass) &&
         (CliParaMDCCheck(MDC_PTD1_ADR, EVEN_MDC_BIT2)))) {

        if ( CliParaMDCCheck(MDC_EODPTD_ADR, EVEN_MDC_BIT0) )  {
            /* No check Exist GCF or not */
        } else {
            if (GCF_EXIST == GusCheckExist()) {
                if (CAS_PERFORM == sCasError) {
                    CasAllUnLock();
                }
                return STUB_BUSY;
            }
        }
    }                                           /* Check transaction # */
    if (STUB_SUCCESS != SerChekTranNo(pReqMsgH)) {
        if (CAS_PERFORM == sCasError) {
            CasAllUnLock();
        }
        return STUB_UNMATCH_TRNO;
    }

    pResMsgH->sReturn = TtlTotalReset(&pReqMsgH->uchMajorClass, pReqMsgH->usExeMDC);
    if (CAS_PERFORM == sCasError) {
        CasAllUnLock();                         /* cashier unlock */
    }

    if (TTL_SUCCESS == pResMsgH->sReturn) {
        if ((CLASS_TTLEODRESET == pReqMsgH->uchMajorClass) ||
            (CLASS_TTLPTDRESET == pReqMsgH->uchMajorClass)) {
                                                /* clear transaction # */
            SstResetCurTransNo();
        }
    }

    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    SHORT    SerChekTtlResetCas(CLIREQRESETTL *pReqMsgH,
*                                        CLIRESTOTAL *pResMsgH);
*     Input:    pReqMsgH    - request from satellite
*               pResMsgH    - response to satellite
*
** Return:      STUB_SUCCESS:       All data received
*               STUB_BUSY:          I am not ready to reset
*
** Description:
*   This function executes for Total reset, cashier function request.
*===========================================================================
*/
SHORT   SerChekTtlResetCas(CLIREQRESETTL *pReqMsgH, CLIRESTOTAL *pResMsgH)
{
                                                /* cashier ind. lock */
    if (CAS_PERFORM != CasIndLock(pReqMsgH->ulIDNO)) {
        return STUB_BUSY;
    }                                           /*=== RESET CAS TOTAL ===*/
    pResMsgH->sReturn = TtlTotalReset(&pReqMsgH->uchMajorClass, pReqMsgH->usExeMDC);
    CasIndUnLock(pReqMsgH->ulIDNO);             /* cashier ind. unlock */

    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    SHORT    SerChekTransNo(CLIREQRESETTL *pReqMsgH);
*     Input:    pReqMsgH    - request from satellite
*
** Return:      STUB_SUCCESS:       All data received
*               STUB_UNMATCH_TRANNO:    unmatched transaction No.
*
** Description: This function checks transaction # matched or not.
*===========================================================================
*/
SHORT   SerChekTranNo(CLIREQRESETTL *pReqMsgH)
{
    SERINQSTATUS    InqData;

    SstReadInqStat(&InqData);
    if (0 != memcmp(InqData.ausTranNo, pReqMsgH->ausTranNo, CLI_ALLTRANSNO * 2)) {
        return STUB_UNMATCH_TRNO;
    }
    return STUB_SUCCESS;
}

/********************** New Function (Release 3.1) BEGIN ******************/
/*
*===========================================================================
** Synopsis:    SHORT    SerChekTtlResetIndFin(CLIREQRESETTL *pReqMsgH,
*                                              CLIRESTOTAL   *pResMsgH);
*     Input:    pReqMsgH    - request from satellite
*               pResMsgH    - response to satellite
*
** Return:      STUB_SUCCESS:       All data received
*               STUB_BUSY:          I am not ready to reset
*
** Description:
*   This function executes for Individual Financial Total reset
*===========================================================================
*/
SHORT   SerChekTtlResetIndFin(CLIREQRESETTL *pReqMsgH, CLIRESTOTAL *pResMsgH)
{
    pResMsgH->sReturn = TtlTotalReset(&pReqMsgH->uchMajorClass, pReqMsgH->usExeMDC);
    return STUB_SUCCESS;
}

/*
*===========================================================================
** Synopsis:    SHORT    SerChekTtlReset(CLIREQRESETTL *pReqMsgH,
*                                        CLIRESTOTAL *pResMsgH);
*     Input:    pReqMsgH    - request from satellite
*               pResMsgH    - response to satellite
*
** Return:      STUB_SUCCESS:       All data received
*               STUB_BUSY:          I am not ready to reset
*               STUB_UNMATCH_TRANNO:    unmatched transaction No.
*
** Description: 
*  This function executes for Total reset EOD, PTD function request.
*===========================================================================
*/
SHORT   SerChekTtlDelete(CLIREQRESETTL *pReqMsgH, CLIRESTOTAL *pResMsgH)
{
    SHORT   sCasError;
    CLITTLCASWAI    TtlCas;

    sCasError = CasAllLock();                   /* cashier all lock */
    if ((CAS_PERFORM != sCasError) &&
        (CAS_FILE_NOT_FOUND != sCasError)) {
        return STUB_BUSY;
    }

    if (STUB_SUCCESS != SerChekTranNo(pReqMsgH)) {
        if (CAS_PERFORM == sCasError) {
            CasAllUnLock();
        }
        return STUB_UNMATCH_TRNO;
    }

    TtlCas.uchMajorClass = pReqMsgH->uchMajorClass;
    TtlCas.uchMinorClass = pReqMsgH->uchMinorClass;
    TtlCas.ulCashierNo   = pReqMsgH->ulIDNO;

    pResMsgH->sReturn = TtlTotalDelete(&TtlCas);

    if (CAS_PERFORM == sCasError) {
        CasAllUnLock();                         /* cashier unlock */
    }

    return STUB_SUCCESS;
}
/*
*===========================================================================
** Synopsis:    SHORT    SerRecvTtlReadPluEx(CLIREQRESETTL *pReqMsgH)
*     Input:    pReqMsgH    - request from Ser
*     Output:   nothing
*     InOut:    nothing
*
** Return:      STUB_SUCCESS:           Success
*
** Description:
*   This function executes plural PLU Total read.       R3.0    Saratoga
*
*   See also IspRecvTtlReadPluEx() which is practically the same function that
*   is just using a different response buffer.
*===========================================================================
*/
SHORT   SerRecvTtlReadPluEx(CLIREQRESETTL *pReqMsgH)
{
    CLIREQDATA  *pReqBuff;
    TTLPLUEX    *pTtlEx;
    SHORT       sError, i, sTblId;

    SerResp.pSavBuff = (CLIREQDATA *)&auchIspTmpBuf[sizeof(CLIRESTOTAL)];
	pTtlEx = (VOID *)SerResp.pSavBuff->auchData;
	pReqBuff = (CLIREQDATA *)((UCHAR *)pReqMsgH + sizeof(CLIREQRESETTL));

    SerResp.pSavBuff->usDataLen = 0;

    memcpy(SerResp.pSavBuff->auchData, pReqBuff->auchData, pReqBuff->usDataLen);

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
                SerResp.pSavBuff->usDataLen = sizeof(TTLPLUEX);
                fsIspLockedFC &= ~ISP_LOCK_PLUTTL;   /* Reset PLUTTL Lock */
                return(SUCCESS);
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
    
    memset(&pTtlEx->Plu[0], 0, sizeof(TTLPLUTOTAL) * CLI_PLU_EX_MAX_NO);

    for (i = 0; i < CLI_PLU_EX_MAX_NO; i++) {
		USHORT      usDataLength = 0;
		TTLPLU      Ttl = { 0 };

		sError = TtlPLUGetRec(&Ttl);
        if ((sError == TTL_SUCCESS) ||
            (sError == TTL_EOF)) {

            _tcsncpy(pTtlEx->Plu[i].auchPLUNumber, Ttl.auchPLUNumber, NUM_PLU_LEN);
            pTtlEx->Plu[i].uchAdjectNo = Ttl.uchAdjectNo;
            pTtlEx->Plu[i].PLUTotal = Ttl.PLUTotal;
            
            if (i==0) {

                Ttl.uchMajorClass = pTtlEx->uchMajorClass;
                Ttl.uchMinorClass = pTtlEx->uchMinorClass;
                if (TtlTotalRead(&Ttl, &usDataLength) == TTL_SUCCESS) { /* 09/28/00 */
                /* if ((sError = SerTtlTotalRead(&Ttl)) == TTL_SUCCESS) { */
                    pTtlEx->uchResetStatus = Ttl.uchResetStatus;
                    pTtlEx->FromDate = Ttl.FromDate;
                    pTtlEx->ToDate = Ttl.ToDate; //SR275
                    pTtlEx->PLUAllTotal = Ttl.PLUAllTotal;
                }
				else if (sError == TTL_NOTCLASS) {
					PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_DATA_ERR_02);
				}
            }
            if (sError == TTL_EOF) {
                sError = SUCCESS;
                if (i != (CLI_PLU_EX_MAX_NO-1)) {   /* 09/28/00 */
                    TtlPLUCloseRec();
                    fsIspLockedFC &= ~ISP_LOCK_PLUTTL;   /* Set PLUTTL Lock */
                }
                break;

            } else {
                sError = SUCCESS;
                fsIspLockedFC |= ISP_LOCK_PLUTTL;   /* Reset PLUTTL Lock */
            }

        } else {
            if (i==0) { /* 09/28/00 */
                sError = SUCCESS; /* no more record */
            }
            TtlPLUCloseRec();
            fsIspLockedFC &= ~ISP_LOCK_PLUTTL;   /* Reset PLUTTL Lock */
            break;
        }
    }
    
    if (sError == SUCCESS) {
        SerResp.pSavBuff->usDataLen = sizeof(TTLPLUEX);
    }
    return(sError);
}


/*===== END OF SOURCE =====*/