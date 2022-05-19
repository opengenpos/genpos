/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg Cashier Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFMSR.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifMSR()   : Reg MSR
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Dec-07-95:03.01.00:  hkato    : Initial
* Apr-23-96:03.01.04:  M.Ozawa  : Change to ignor long digit msr data.
* Jun-02-98:03.03.00:  M.Ozawa  : Add Cashier Interrupt Sign-In
* Aug-11-99:03.05.00:  K.Iwata  : Remove WAITER_MODEL
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
#include <stdlib.h>
#include <stdio.h>

#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "uie.h"
#include "appllog.h"
#include "fsc.h"
#include "pifmain.h"
#include "regstrct.h"
#include "item.h"
#include "paraequ.h"
#include "para.h"
#include "uireg.h"
#include "uic.h"
#include "uiregloc.h"
#include "csetk.h"
#include "cscas.h"
#include "csstbpar.h"
#include "csstbetk.h"
#include "csstbcas.h"
#include "BlFWif.h"
                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UIMENU FARCONST aChildRegSignInMSR[] = {{UifSignInMSR, CID_SIGNINMSR},
                                       {NULL, 0}};

UIMENU FARCONST aChildRegCasTranMSR[] = {{UifTransVoid, CID_TRANSVOID},
                                        {UifWICTran, CID_WICTRAN},        /* Saratoga */
                                        {UifCashierWaiterMSR, CID_CASHIERWAITERMSR},
                                        {UifSales, CID_SALES},
                                        {UifNewCheck, CID_NEWCHECK},
                                        {UifReorder, CID_REORDER},
                                        {UifAddCheck, CID_ADDCHECK},
                                        {UifSingleTrans, CID_SINGLETRANS},
                                        {UifChangeComp, CID_CHANGECOMP},
                                        {UifReceipt, CID_RECEIPT},
                                        {UifPostReceipt, CID_POSTRECEIPT},
                                        {UifReverse, CID_REVERSE},
                                        {UifDeclared, CID_DECLARED}, /* V3.3 */
										{UifNewKeySeq, CID_NEWKEYSEQ},    /* V3.3 */
                                        {NULL, 0}};
UIMENU FARCONST aChildRegBarTranMSR[] = {{UifWaiterSignOutMSR, CID_WAITERSIGNOUTMSR},
                                      {UifTransVoid, CID_TRANSVOID},
                                      {UifWICTran, CID_WICTRAN},        /* Saratoga */
                                      {UifSales, CID_SALES},
                                      {UifDeclared, CID_DECLARED},
                                      {UifChangeComp, CID_CHANGECOMP},
                                      {UifReceipt, CID_RECEIPT},
                                      {UifPostReceipt, CID_POSTRECEIPT},
                                      {UifReverse, CID_REVERSE},
                                      {NULL, 0}};

/* R3.3 */
UIMENU FARCONST aChildRegCasIntTranMSR[] = {{UifCasIntSignInOutMSR, CID_CASINTSIGNINOUTMSR},
                                      {UifTransVoid, CID_TRANSVOID},
                                      {UifWICTran, CID_WICTRAN},        /* Saratoga */
                                      {UifSales, CID_SALES},
                                      /* {UifDeclared, CID_DECLARED}, */
                                      {UifChangeComp, CID_CHANGECOMP},
                                      {UifReceipt, CID_RECEIPT},
                                      {UifPostReceipt, CID_POSTRECEIPT},
                                      {UifReverse, CID_REVERSE},
                                      {NULL, 0}};

extern UIMENU FARCONST aChildRegCasIntTran[];   /* defined at uifcas.c */
extern UIMENU FARCONST aChildRegCasIntRecallTran[];   /* defined at uifcas.c */

UIMENU FARCONST aChildRegETKTranMSR[] = {NULL, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

SHORT  UifCheckMsrEmployeeTrackData (TCHAR *auchTrack2, USHORT usLength2)
{
	USHORT  usLength;

	for (usLength = 0; usLength < usLength2; usLength++) {
		if ((auchTrack2[usLength] < '0') || (auchTrack2[usLength] > '9')) {
			break;
		}
	}
	if (usLength2 > 30 || auchTrack2[usLength] == ITM_MSR_SEPA || auchTrack2[usLength] == ITM_MSR_GIFT_SEPA) {
		// looks like a credit card to me since it has a separator so
		// lets just ignore this swipe.
		return (-1);
	}

	return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   UifMSR(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg MSR Entry Module
*===========================================================================
*/
SHORT   UifMSR(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildRegSignInMSR);
        break;

    case UIM_ACCEPTED:
        if (flUifRegStatus & UIFREG_CASHIERSIGNOUT) {   /* cashier sign out status on ? */
            flUifRegStatus &= ~(UIFREG_CASHIERSIGNIN | UIFREG_CASHIERSIGNOUT |
                                UIFREG_CASINTSIGNIN | UIFREG_CASINTRECALL | UIFREG_CASINTNEWIN |
                                UIFREG_FINALIZE | UIFREG_BUFFERFULL);     /* reset status */
            /* --- Print Unsoli Report,  V3.3 --- */
            UifCPPrintUns();

            UieAccept();                                /* send accepted status to parent */
            break;
        }
        if (flUifRegStatus & UIFREG_WAITERSIGNOUT) {   /* cashier sign out status on ? */
            flUifRegStatus &= ~(UIFREG_WAITERSIGNIN | UIFREG_BARSIGNIN | UIFREG_WAITERSIGNOUT | UIFREG_FINALIZE | UIFREG_BUFFERFULL);     /* reset status */
            /* --- Print Unsoli Report,  V3.3 --- */
            UifCPPrintUns();

            UieAccept();                                /* send accepted status to parent */
            break;
        }
        if (flUifRegStatus & UIFREG_CASHIERSIGNIN) {
            UieNextMenu(aChildRegCasTranMSR);
        } else if (flUifRegStatus & UIFREG_CASINTSIGNIN) {  /* R3.3 */
            flUifRegStatus &= ~UIFREG_CASINTNEWIN;  /* reset new sign-in status */
            if (flUifRegStatus & UIFREG_CASINTRECALL) {     /* cashier interrupt recall ? */
                UieNextMenu(aChildRegCasIntRecallTran);     /* open children for recall trans. */
            } else {
                UieNextMenu(aChildRegCasIntTran);
            }
        } else if (flUifRegStatus & UIFREG_ETKSIGNIN) {
            flUifRegStatus &= ~UIFREG_ETKSIGNIN;
            UieAccept();
        }

        UieCtrlDevice(UIE_ENA_SCANNER);                 /* V3.3 */
        /* --- Print Unsoli Report,  V3.3 --- */
        UifCPPrintUns();

        break;

    case UIM_QUIT:
        UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set cashier sign out class */
        UifRegData.regopeclose.uchMinorClass = CLASS_UIMODELOCK;
        ItemSignOut(&UifRegData.regopeclose);           /* cashier sign out Modele */
        /* --- Print Unsoli Report,  V3.3 --- */
        UifCPPrintUns();

        return(SUCCESS);

    case UIM_REJECT:
        break;

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   UifSignInMSR(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Sign In Module
*               This function is invoked when the operator uses an mag stripe
*               card to sign into the terminal.  This function performs the
*               various actions needed to process the sign-in function.
*
*               NOTE:
*               When making changes to this function, review the source code
*               in function UifCashierSignIn() in file uifcasin.c which handles the
*               sign-in with manual entry of operator number and secret code.
*===========================================================================
*/
UISEQ FARCONST aszSeqRegSignInMSR[] = {FSC_MSR, 0};

SHORT   UifFillInEmployeeRecord (UIFDIAEMPLOYEE  *pUifEmployeeData)
{
	SHORT           sReturn;

	{
		CASIF casRec = {0};

		casRec.ulCashierNo = pUifEmployeeData->ulSwipeEmployeeId;

		//this is to replace amtrak behavior with a more generally desired behavior
		//and to implement new functionality
		pUifEmployeeData->usSwipeEmployeeJobCode = 0;
		pUifEmployeeData->usSwipeOperationSecondary = 0;
		pUifEmployeeData->ulSwipeSupervisorNumber = 0;
		pUifEmployeeData->ulStatusFlags = 0;
		if (CliParaMDCCheckField(MDC_EMPLOYEE_ID_ADR, MDC_PARAM_BIT_A)) {  // Implement changes for Amtrak
			pUifEmployeeData->ulSwipeEmployeePin = pUifEmployeeData->ulSwipeEmployeeId;    // Amtrak default PIN
		} else {
			pUifEmployeeData->ulSwipeEmployeePin = 0xffffff;    // bogus PIN
		}

		if((sReturn = SerCasIndRead(&casRec)) == CAS_PERFORM){
			int regModeProhibited, supModePINReq;

			pUifEmployeeData->ulSwipeEmployeePin = casRec.ulCashierSecret;

			//checking on status bits
			regModeProhibited	= casRec.fbCashierStatus[CAS_CASHIERSTATUS_3] & CAS_REG_MODE_PROHIBITED;
			supModePINReq		= casRec.fbCashierStatus[CAS_CASHIERSTATUS_3] & CAS_PIN_REQIRED_FOR_SUPR;

			pUifEmployeeData->ulStatusFlags |= (casRec.fbCashierStatus[CAS_CASHIERSTATUS_2] & CAS_PIN_REQIRED_FOR_TIMEIN) ? UIFDIAEMPLOYEE_STATUS_ETK_PIN_REQ : 0;
			pUifEmployeeData->ulStatusFlags |= (casRec.fbCashierStatus[CAS_CASHIERSTATUS_1] & CAS_NOT_DRAWER_A) ? UIFDIAEMPLOYEE_STATUS_REG_DENY_A : 0;
			pUifEmployeeData->ulStatusFlags |= (casRec.fbCashierStatus[CAS_CASHIERSTATUS_1] & CAS_NOT_DRAWER_B) ? UIFDIAEMPLOYEE_STATUS_REG_DENY_B : 0;

			if(casRec.usSupervisorID != 0){
				pUifEmployeeData->ulSwipeSupervisorNumber = casRec.usSupervisorID;
//				pUifEmployeeData->ulSwipeSupervisorNumber *= 100;
//				pUifEmployeeData->ulSwipeSupervisorNumber += 99;		// gives the expected 5-digit format ending in 99 for old-style PIN

				pUifEmployeeData->usSwipeOperationSecondary = MSR_ID_SUPERVISOR;

				if(supModePINReq)	//supervisor mode requires a pin
					pUifEmployeeData->ulStatusFlags |= UIFDIAEMPLOYEE_STATUS_SUP_PIN_REQ;
				
				if(!regModeProhibited && CliParaMDCCheckField(MDC_EMPLOYEE_ID_ADR, MDC_PARAM_BIT_A))	//can sign in too
					pUifEmployeeData->usSwipeEmployeeJobCode = MSR_ID_JOBCODE_SUP_MAX;	

			} else if(!regModeProhibited)
				pUifEmployeeData->usSwipeOperationSecondary = MSR_ID_CASHIER;
			else
				pUifEmployeeData->usSwipeOperationSecondary = MSR_ID_EMPLOYEE;

			if (CliParaMDCCheckField(MDC_EMPLOYEE_ID_ADR, MDC_PARAM_BIT_A)) {  // Implement changes for Amtrak
				ETK_JOB         EtkJob;
				USHORT          usMaxFieldNo;
				SHORT           sReturn;
				SHORT           sTimeIOStat;

				// following source is to implement Amtrak requirement for checking the Job Code associated with an Employee
				// in order to determine if the Employee is allowed particular privileges and access.  This is to check to see
				// if the Employee has the appropriate Job Code to access the Supervisor Mode functionality.
				pUifEmployeeData->usSwipeEmployeeJobCode = 0;
				pUifEmployeeData->usSwipeOperationSecondary = 0;
				pUifEmployeeData->ulSwipeSupervisorNumber = 0;
				if ((sReturn = CliEtkStatRead(pUifEmployeeData->ulSwipeEmployeeId, &sTimeIOStat, &usMaxFieldNo, &EtkJob)) == ETK_SUCCESS) {
					pUifEmployeeData->usSwipeEmployeeJobCode = EtkJob.uchJobCode1;

					if (pUifEmployeeData->usSwipeEmployeeJobCode >= MSR_ID_JOBCODE_SUP_MIN && pUifEmployeeData->usSwipeEmployeeJobCode <= MSR_ID_JOBCODE_SUP_MAX) {
						pUifEmployeeData->usSwipeOperationSecondary = MSR_ID_SUPERVISOR;

						pUifEmployeeData->ulSwipeSupervisorNumber = pUifEmployeeData->usSwipeEmployeeJobCode;
						pUifEmployeeData->ulSwipeSupervisorNumber %= 100;     // ensure that job code is only a 2 digit number
						pUifEmployeeData->ulSwipeSupervisorNumber += 800;     // Create a Supervisor number from 880 through 899
					}
					else if (pUifEmployeeData->usSwipeEmployeeJobCode >= MSR_ID_JOBCODE_REG_MIN && pUifEmployeeData->usSwipeEmployeeJobCode <= MSR_ID_JOBCODE_REG_MAX) {
						pUifEmployeeData->usSwipeOperationSecondary = MSR_ID_CASHIER;
					}
					else if (pUifEmployeeData->usSwipeEmployeeJobCode != MSR_ID_JOBCODE_UNASSIGNED) {
						pUifEmployeeData->usSwipeOperationSecondary = MSR_ID_EMPLOYEE;
					}
				}
			}
		} else {
			char  xBuff[128];
			sprintf (xBuff, " SerCasIndRead() failed: status = %d", sReturn);
			NHPOS_NONASSERT_TEXT(xBuff);
			return ETK_NOT_IN_FILE;
		}
	}

	return(UIF_SUCCESS);
}

SHORT   UifTrackToUifDiaEmployee (TCHAR *auchTrack2, USHORT usLength2, UIFDIAEMPLOYEE  *pUifEmployeeData)
{
	USHORT  usIndex;
	TCHAR   aszTempBuffer[32];

	memset (pUifEmployeeData, 0, sizeof(UIFDIAEMPLOYEE));

	// Find the number of characters in the track 2 data on the mag stripe.
	// Then determine if the track 2 data is invalid.
	// The track 2 data contains the following information for old style card
	//    - single digit indicator as to type of MSR card
	//    - three digit operator or employee number
	//    - two digit secret code if site is using secret code as part of sign-in/time-in
	// The track 2 data contains the following information for a new style Amtrak card
	//    - five (5) digits indicating ?
	//    - eight (8) digits for the operator or employee number; UIFREG_MAX_ID_DIGITS
	//    - seven (7) digits indicating ?
	//    - five (5) digits indicating ?
    for (usIndex = 0; usIndex < usLength2; usIndex++) {
        if ((auchTrack2[usIndex] < '0') || (auchTrack2[usIndex] > '9')) {
            break;
        }
    }

	if (usIndex >= 17) {
		// assume that this is an Amtrak type of card
		pUifEmployeeData->usSwipeOperation = MSR_ID_GENERIC;
		memset (aszTempBuffer, 0, sizeof(aszTempBuffer));
		_tcsncpy (aszTempBuffer, auchTrack2 + 5, 8);
		pUifEmployeeData->ulSwipeEmployeeId = _ttol(aszTempBuffer);
		pUifEmployeeData->ulSwipeEmployeePin = 0;
	}
	else if (usIndex > 0) {
		// assume this is a standard NeighborhoodPOS type of card
		// parse out the various fields that are on the old style of card
		// Employee Id and PIN
		pUifEmployeeData->usSwipeOperation = auchTrack2[0];
		pUifEmployeeData->usSwipeOperationSecondary = auchTrack2[0];
		memset (aszTempBuffer, 0, sizeof(aszTempBuffer));
		_tcsncpy (aszTempBuffer, auchTrack2 + 1, 3);
		pUifEmployeeData->ulSwipeEmployeeId = _ttol(aszTempBuffer);
		pUifEmployeeData->ulSwipeEmployeePin = 0;
        if (usIndex > UIFREG_MAX_DIGIT4) {    /* ID + CASHIER# */
			memset (aszTempBuffer, 0, sizeof(aszTempBuffer));
			_tcsncpy (aszTempBuffer, auchTrack2 + 4, 2);
			pUifEmployeeData->ulSwipeEmployeePin = _ttol(aszTempBuffer);
		}

		/* validate entered cashier No. */
		if (pUifEmployeeData->ulSwipeEmployeeId == 0L) {        
			return(LDT_SEQERR_ADR);             /* operation sequence error */
		}

		if (pUifEmployeeData->ulSwipeEmployeeId > 999L) {
			return(LDT_KEYOVER_ADR);    /* key entry over limitation error */
		}

		if (pUifEmployeeData->usSwipeOperation == MSR_ID_EMPLOYEE) {
			if (usIndex > UIFREG_MAX_DIGIT9) {    /* ID + EMPLOYEE# + JOB CODE*/
				memset (aszTempBuffer, 0, sizeof(aszTempBuffer));
				_tcsncpy (aszTempBuffer, auchTrack2 + 6, 3);
				pUifEmployeeData->usSwipeEmployeeJobCode = (USHORT)_ttol(aszTempBuffer);
			}
		}
	}
    else {
        return(LDT_KEYOVER_ADR);
    }

	return UifFillInEmployeeRecord (pUifEmployeeData);
}

SHORT   UifSignInMSR(KEYMSG *pData)
{
    SHORT   sRetStatus;
    USHORT  usLockStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegSignInMSR);
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_VIRTUALKEYEVENT || pData->SEL.INPUT.uchMajor == FSC_MSR) {
			UIFDIAEMPLOYEE  UifDiaEmployee;

            if (pData->SEL.INPUT.uchLen) {
                return(LDT_SEQERR_ADR);
            }

			if (pData->SEL.INPUT.uchMajor == FSC_VIRTUALKEYEVENT) {
				UIE_MSR     MSR;

				memset (&MSR, 0, sizeof(MSR));
				MSR.uchPaymentType = SWIPE_PAYMENT;

				memcpy (MSR.auchTrack1, pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.auchTrack1, sizeof(MSR.auchTrack1));
				MSR.uchLength1 = pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength1;
				memcpy (MSR.auchTrack2, pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.auchTrack2, sizeof(MSR.auchTrack2));
				MSR.uchLength2 = pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength2;

				pData->SEL.INPUT.DEV.MSR = MSR;
				pData->SEL.INPUT.uchMajor = FSC_MSR;
			}

			if (UifCheckMsrEmployeeTrackData (pData->SEL.INPUT.DEV.MSR.auchTrack2, pData->SEL.INPUT.DEV.MSR.uchLength2) != 0) {
				NHPOS_NONASSERT_TEXT("MSR swipe credit card ignored.");
				UieReject();
				return (SUCCESS);
			}

			sRetStatus = UifTrackToUifDiaEmployee (pData->SEL.INPUT.DEV.MSR.auchTrack2, pData->SEL.INPUT.DEV.MSR.uchLength2, &UifDiaEmployee);
			if (pData->SEL.INPUT.DEV.MSR.uchPaymentType == SWIPE_PAYMENT) {
				UifDiaEmployee.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_SWIPE;
			} else if (pData->SEL.INPUT.DEV.MSR.uchPaymentType == BIOMETRICS_ENTRY) {
				UifDiaEmployee.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_BIOMET;
			} else if (pData->SEL.INPUT.DEV.MSR.uchPaymentType == MANUAL_ENTRY) {
				UifDiaEmployee.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
			}

			if (sRetStatus != UIF_SUCCESS) {
				char  xBuff[128];
				sprintf (xBuff, " UifTrackToUifDiaEmployee() failed: status = %d", sRetStatus);
				NHPOS_NONASSERT_TEXT(xBuff);
				return LDT_NTINFL_ADR;
			}

			// For Amtrak we will perform a check and if the this employee has a job code for a Cashier then
			// lets make sure that an Operator record exists and is available
            if (UifDiaEmployee.usSwipeOperation == MSR_ID_GENERIC) {
				if (CliParaMDCCheckField(MDC_EMPLOYEE_ID_ADR, MDC_PARAM_BIT_A)) {  // Implement changes for Amtrak
					if (UifDiaEmployee.usSwipeOperationSecondary == MSR_ID_SUPERVISOR) {
						// For Amtrak we will allow a Supervisor to also act as a Cashier if Job Code is high enough.
						if (UifDiaEmployee.usSwipeEmployeeJobCode >= MSR_ID_JOBCODE_SUP_REG)
							UifDiaEmployee.usSwipeOperationSecondary = MSR_ID_CASHIER;
					}

					if (UifDiaEmployee.usSwipeOperationSecondary != MSR_ID_CASHIER) {
						// For Amtrak auto Sign-in is used only for Cashier type employees so if this is not
						// a Cashier then we do not allow this to work.
						UifDiaEmployee.usSwipeOperationSecondary = 0;
					}
				} else {
					UifDiaEmployee.usSwipeOperationSecondary = MSR_ID_CASHIER;
				}
			}

            if ((UifDiaEmployee.usSwipeOperationSecondary == MSR_ID_CASHIER) || (UifDiaEmployee.usSwipeOperationSecondary == MSR_ID_SERVER)) {    /* compatible with R3.1 */
                /*----- Cashier Sign-in Operation -----*/
                if (aszUifRegNumber[0] != 0) {
                    return(LDT_SEQERR_ADR);
                }

                if ((husUifRegHandle = UicCheckAndSet()) == UIC_NOT_IDLE) { /* ### MOD (2172 Rel1.0) */
                    husUifRegHandle = 0;
                    return(LDT_LOCK_ADR);
                }

                UifRegData.regopeopen.uchMajorClass = CLASS_UIFREGOPEN;
				UifRegData.regopeopen.uchMinorClass = CLASS_UICASHIERIN;
				if (UifDiaEmployee.ulStatusFlags & UIFDIAEMPLOYEE_STATUS_REG_DENY_A) {
					UifRegData.regopeopen.uchMinorClass = CLASS_UIB_IN;   // if MSR Sign-in and Drawer A is denied then we will auto assign Drawer B
				}
				{
					PARACASHABASSIGN   CasNoBuff;     // With MSR Sign-in, determine if AC7 indicates specific drawer for this operator id
					UCHAR              uchMinorClass = UifRegData.regopeopen.uchMinorClass;

					CasNoBuff.uchMajorClass = CLASS_PARACASHABASSIGN;
					CasNoBuff.uchAddress = ASN_AKEY_ADR;                        /* SIGNIN key address */
					CliParaRead(&CasNoBuff);
					if ( (CasNoBuff.ulCashierNo != 0) && (CasNoBuff.ulCashierNo == UifDiaEmployee.ulSwipeEmployeeId) ) {
						UifRegData.regopeopen.uchMinorClass = CLASS_UICASHIERIN;
					}

					CasNoBuff.uchAddress = ASN_BKEY_ADR;                        /* B key address */
					CliParaRead(&CasNoBuff);
					if ( (CasNoBuff.ulCashierNo != 0) && (CasNoBuff.ulCashierNo == UifDiaEmployee.ulSwipeEmployeeId) ) {
						UifRegData.regopeopen.uchMinorClass = CLASS_UIB_IN;
					}
				}
                UifRegData.regopeopen.ulID = UifDiaEmployee.ulSwipeEmployeeId;
                UifRegData.regopeopen.ulPin = UifDiaEmployee.ulSwipeEmployeePin;
				UifRegData.regopeopen.ulStatusFlags = UifDiaEmployee.ulStatusFlags;
                UifRegData.regopeopen.fbInputStatus |= INPUT_MSR;
                sRetStatus = ItemSignIn(&UifRegData.regopeopen);
				UifRegCheckDrawerLimit(0);					//SR155 CashDrawer Limit
                UifRegWorkClear();
                if (sRetStatus != UIF_SUCCESS) {
                    if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {
                        PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                    }
                    husUifRegHandle = 0;
                    if (sRetStatus == UIF_CANCEL) {
                        UieReject();
                        return (SUCCESS);
                    }
					{
						char  xBuff[128];
						sprintf (xBuff, " ItemSignIn() failed: status = %d", sRetStatus);
						NHPOS_NONASSERT_TEXT(xBuff);
					}
                    return(sRetStatus);
                } else {
                    flUifRegStatus |= UIFREG_CASHIERSIGNIN;
                }
                UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);
                UieAccept();
				UieCtrlDevice(UIE_ENA_SCANNER);                 /* correct enable position, V1.0.13 */
                break;
            } else if (UifDiaEmployee.usSwipeOperationSecondary == MSR_ID_EMPLOYEE) {
                /*----- ETK Sign-in Operation -----*/
                /*----- Convert TRACK II STRING Data to LONG -----*/
                /* saratoga */

				if (!husUifRegHandle) {             /* not during transaction lock */
                    if ((husUifRegHandle = UicCheckAndSet()) == UIC_NOT_IDLE) {  /* check transaction lock *//* ### MOD (2172 Rel1.0) */
                        husUifRegHandle = 0;
                        return(LDT_LOCK_ADR);
                    }
                    usLockStatus = 1;
                }
                /* Try by TIME-OUT process */
                UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;
                UifRegData.regmisc.uchMinorClass = CLASS_UIETKOUT;
                UifRegData.regmisc.ulEmployeeNo = UifDiaEmployee.ulSwipeEmployeeId;
                UifRegData.regmisc.uchJobCode = (UCHAR)UifDiaEmployee.usSwipeEmployeeJobCode;
				UifRegData.regmisc.sTimeInStatus = 0;
				UifRegData.regmisc.ulStatusFlags = UifDiaEmployee.ulStatusFlags;
                sRetStatus = ItemMiscETK(&UifRegData.regmisc);

                if (sRetStatus == LDT_ETKERR_ADR) { /* not time-in/out */
                    /* Retry by TIME-IN process */
                    UifRegData.regmisc.uchMinorClass = CLASS_UIETKIN;
                    sRetStatus = ItemMiscETK(&UifRegData.regmisc);
                }

                UifRegWorkClear();                          /* clear work area */
                if (usLockStatus) {                         /* check transaction lock */
                    if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {  /* reset transaction lock */
                        PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                    }
                    husUifRegHandle = 0;
                }
                if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                    if (sRetStatus == UIF_CANCEL) {         /* abort ? */
                        UieReject();                        
                        return (SUCCESS);
					}
					{
						char  xBuff[128];
						sprintf (xBuff, " ItemMiscETK() failed: status = %d", sRetStatus);
						NHPOS_NONASSERT_TEXT(xBuff);
					}
                    return(sRetStatus);
                }
                flUifRegStatus |= UIFREG_ETKSIGNIN;
                UieAccept();
                break;
            } else if (UifDiaEmployee.usSwipeOperation == MSR_ID_CASINT) {
                /*----- Cashier Sign-in Operation -----*/
                if (aszUifRegNumber[0] != 0) {
                    return(LDT_SEQERR_ADR);
                }

                if ((husUifRegHandle = UicCheckAndSet()) == UIC_NOT_IDLE) { /* ### MOD (2172 Rel1.0) */
                    husUifRegHandle = 0;
                    return(LDT_LOCK_ADR);
                }

                UifRegData.regopeopen.uchMajorClass = CLASS_UIFREGOPEN;
                UifRegData.regopeopen.uchMinorClass = CLASS_UICASINTIN;
                UifRegData.regopeopen.ulID = UifDiaEmployee.ulSwipeEmployeeId;
				UifRegData.regopeopen.ulStatusFlags = UifDiaEmployee.ulStatusFlags;
                UifRegData.regopeopen.fbInputStatus |= INPUT_MSR;
                sRetStatus = ItemSignIn(&UifRegData.regopeopen);
				UifRegCheckDrawerLimit(0);					//SR155 CashDrawer Limit
                UifRegWorkClear();

                if (sRetStatus != UIF_SUCCESS) {
                    if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {
                        PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                    }
                    husUifRegHandle = 0;
                    if (sRetStatus == UIF_CANCEL) {
                        UieReject();
                        return (SUCCESS);
                    }
                    return(sRetStatus);
                } else {
                    flUifRegStatus |= UIFREG_CASINTSIGNIN;
                }
                UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);

                /* execute to recall transaction from GCF */
                UifRegData.regtransopen.uchMajorClass = CLASS_UIFREGTRANSOPEN;  /* set add check class */
                UifRegData.regtransopen.uchMinorClass = CLASS_UICASINTRECALL;   /* cashier interrupt recall */
                sRetStatus = ItemTransOpen(&UifRegData.regtransopen);   /* Transaction Open Modele */
                UifRegWorkClear();                          /* clear work area */
                if (sRetStatus == UIF_SUCCESS) {
                    flUifRegStatus |= UIFREG_CASINTRECALL;  /* set cashier interrupt recall status */
                    flUifRegStatus &= ~UIFREG_BUFFERFULL;   /* reset buffer full status */
                    UieAccept();                            /* send accepted to parent */
                    return(UIF_SUCCESS);
                } else if (sRetStatus == LDT_TAKETL_ADR) {         /* buffer full ? */
                    flUifRegStatus &= ~UIFREG_CASINTRECALL; /* reset cashier interrupt recall status */
                    flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
                    UieAccept();                            /* send accepted to parent */
                    return(sRetStatus);
                } else if (sRetStatus == LDT_NTINFL_ADR) {
                    flUifRegStatus &= ~UIFREG_CASINTRECALL; /* reset cashier interrupt recall status */
                    flUifRegStatus &= ~UIFREG_BUFFERFULL;   /* reset buffer full status */
                    UieAccept();                            /* send accepted to parent */
                    return(UIF_SUCCESS);
                }
                UieAccept();                                /* send accepted to parent */
                return (sRetStatus);
            } else {                                        /* Illegal Card */
				char  xBuff[128];
				sprintf (xBuff, " LOG_EVENT_UIC_INVALID_MSR_CARD: Sec = %d  status = %d", UifDiaEmployee.usSwipeOperationSecondary, sRetStatus);
				NHPOS_NONASSERT_TEXT(xBuff);
				PifLog (MODULE_UIC_LOG, LOG_EVENT_UIC_INVALID_MSR_CARD);
                return(LDT_SEQERR_ADR);
            }
            /* break; */
        }
        /* break */                                         /* not use */

    default:
        return(UifRegDefProc(pData));
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   UifTimeMSR(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Time In/Out Module while operator is signed in
*               This function is invoked when the operator uses an mag stripe
*               card to time in/out on the terminal while an operator is signed
*				in.  This function performs thevarious actions needed to process 
*				the time-in/out function.
*===========================================================================
*/
SHORT   UifTimeMSR(KEYMSG *pData)
{
	SHORT   sRetStatus;
    USHORT  usLength, usLockStatus;

	// Find the number of characters in the track 2 data on the mag stripe.
	// Then determine if the track 2 data is invalid.
	// The track 2 data contains the following information:
	//    - single digit indicator as to type of MSR card
	//    - three digit operator or employee number
	//    - two digit secret code if site is using secret code as part of sign-in/time-in
    for (usLength = 0; usLength < pData->SEL.INPUT.DEV.MSR.uchLength2; usLength++) {
		if ((pData->SEL.INPUT.DEV.MSR.auchTrack2[usLength] < '0') ||
			(pData->SEL.INPUT.DEV.MSR.auchTrack2[usLength] > '9')) {

			break;
        }
    }
            
	if (usLength == 0) {
		return(LDT_KEYOVER_ADR);
    }
	
	if (pData->SEL.INPUT.DEV.MSR.auchTrack2[0] == MSR_ID_EMPLOYEE) {
		/*----- ETK Sign-in Operation -----*/
        /*----- Convert TRACK II STRING Data to LONG -----*/
        /* saratoga */
                if (usLength > UIFREG_MAX_DIGIT10) {
                    usLength = UIFREG_MAX_DIGIT10;
                }
                _tcsncpy(&pData->SEL.INPUT.aszKey[0], &pData->SEL.INPUT.DEV.MSR.auchTrack2[0], usLength);
                pData->SEL.INPUT.uchLen = (UCHAR)(usLength);
                pData->SEL.INPUT.lData = _ttol(&pData->SEL.INPUT.aszKey[1]);

                if (!husUifRegHandle) {             /* not during transaction lock */
                    if ((husUifRegHandle = UicCheckAndSet()) == UIC_NOT_IDLE) {  /* check transaction lock *//* ### MOD (2172 Rel1.0) */
                        husUifRegHandle = 0;
                        return(LDT_LOCK_ADR);
                    }
                    usLockStatus = 1;
                }
                /* Try by TIME-OUT process */
                UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;
                UifRegData.regmisc.uchMinorClass = CLASS_UIETKOUT;
                UifRegData.regmisc.ulEmployeeNo = (ULONG)pData->SEL.INPUT.lData;
				UifRegData.regmisc.uchJobCode = 0;
				UifRegData.regmisc.sTimeInStatus = 0;
                sRetStatus = ItemMiscETKSignIn(&UifRegData.regmisc);

                if (sRetStatus == LDT_ETKERR_ADR) { /* not time-in/out */
                    /* Retry by TIME-IN process */
                    UifRegData.regmisc.uchMinorClass = CLASS_UIETKIN;
                    sRetStatus = ItemMiscETKSignIn(&UifRegData.regmisc);
                }

                UifRegWorkClear();                          /* clear work area */
                if (usLockStatus) {                         /* check transaction lock */
                    if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {  /* reset transaction lock */
                        PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                    }
                    husUifRegHandle = 0;
                }
                if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                    if (sRetStatus == UIF_CANCEL) {         /* abort ? */
                        UieReject();                        
                        return (SUCCESS);
                    }
                    return(sRetStatus);
                }
                flUifRegStatus |= UIFREG_ETKSIGNIN;
                UieAccept();
				return sRetStatus;
	}
	return(LDT_ETKERR_ADR);

}

/*
*===========================================================================
** Synopsis:    SHORT   UifCashierWaiterMSR(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Waiter Module
*===========================================================================
*/
UIMENU FARCONST aChildRegCasOutWaiInMSR[] = {{UifCasSignOutWaiSignInMSR, CID_CASOUTWAIINMSR},
                                          {NULL, 0}};
SHORT   UifCashierWaiterMSR(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildRegCasOutWaiInMSR);
        break;

    case UIM_ACCEPTED:
        if (flUifRegStatus & UIFREG_CASHIERSIGNOUT) {   /* cashier sign out status on ? */
            /* not reset cashier sign out status */
            UieAccept();                                /* send accepted status to parent */
            break;
        }
        if (flUifRegStatus & UIFREG_WAITERSIGNOUT) {   /* waiter sign out status on ? */
            flUifRegStatus &= ~(UIFREG_WAITERSIGNIN | UIFREG_BARSIGNIN |
                                UIFREG_WAITERSIGNOUT | UIFREG_FINALIZE |
                                UIFREG_BUFFERFULL);     /* reset status */
            UieAccept();                                /* send accepted status to parent */
            break;
        }
        if (flUifRegStatus & UIFREG_BARSIGNIN) {

            UieNextMenu(aChildRegBarTranMSR);
        }
        break;

    case UIM_QUIT:
        UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set waiter sign out class */
        UifRegData.regopeclose.uchMinorClass = CLASS_UIMODELOCK;
        ItemSignOut(&UifRegData.regopeclose);           /* cashier sign out Modele */
        return(SUCCESS);

    case UIM_REJECT:
        break;

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT   UifCasSignOutWaiSignInMSR(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Cashier Sign Out/Waiter Sign In Module
*===========================================================================
*/
UISEQ FARCONST aszSeqRegCasOutWaiInMSR[] = {FSC_MSR, FSC_SIGN_IN, FSC_B,
                                            FSC_BAR_SIGNIN, 0};
                                            /* FSC_WAITER, FSC_BAR_SIGNIN, 0};  V3.3 */

SHORT   UifCasSignOutWaiSignInMSR(KEYMSG *pData)
{
    SHORT   sRetStatus;
    USHORT  usLockStatus = 0, usLength;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegCasOutWaiInMSR);
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_SIGN_IN
            || pData->SEL.INPUT.uchMajor == FSC_B
            || pData->SEL.INPUT.uchMajor == FSC_MSR) {
            if (pData->SEL.INPUT.uchLen != 0) {
                return(LDT_SEQERR_ADR);
            }
            if (aszUifRegNumber[0] != 0) {
                return(LDT_SEQERR_ADR);
            }
            UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set cashier sign out class */

        } else if (pData->SEL.INPUT.uchMajor == FSC_BAR_SIGNIN) {
            if ((sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_DIGIT5)) != SUCCESS) { /* input check */
                return(sRetStatus);
            }
            if (aszUifRegNumber[0] != 0) {              /* input number ? */
                return(LDT_SEQERR_ADR);
            }
            if (!husUifRegHandle) {                     /* not during transaction lock */
                if ((husUifRegHandle = UicCheckAndSet()) == UIC_NOT_IDLE) {  /* check transaction lock *//* ### MOD (2172 Rel1.0) */
                    husUifRegHandle = 0;
                    return(LDT_LOCK_ADR);
                }
                usLockStatus = 1;
            }
            UifRegData.regopeopen.uchMajorClass = CLASS_UIFREGOPEN;
            UifRegData.regopeopen.ulID = (ULONG)pData->SEL.INPUT.lData;
        }

		if (pData->SEL.INPUT.uchMajor == FSC_MSR) {
			if (pData->SEL.INPUT.DEV.MSR.uchPaymentType == BIOMETRICS_ENTRY) {
				UifRegData.regopeclose.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_BIOMET;
			} else {
				UifRegData.regopeclose.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_SWIPE;
			}
		}
		else {
			UifRegData.regopeclose.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
		}

        if (pData->SEL.INPUT.uchMajor == FSC_SIGN_IN || pData->SEL.INPUT.uchMajor == FSC_B) {
            if (pData->SEL.INPUT.uchMajor == FSC_SIGN_IN) {
                UifRegData.regopeclose.uchMinorClass = CLASS_UICASHIEROUT;
            } else {
                UifRegData.regopeclose.uchMinorClass = CLASS_UIB_OUT;
            }
            sRetStatus = ItemSignOut(&UifRegData.regopeclose);    /* Operator Sign Out Modele */
            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                sRetStatus = (SUCCESS);
            } else if (sRetStatus == UIF_CAS_SIGNOUT) {          /* success or sign out ? */
                flUifRegStatus |= UIFREG_CASHIERSIGNOUT;  /* set cashier sign out status */
				BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                UieAccept();                              /* send accepted to parent */
                sRetStatus = (SUCCESS);
            }
            return(sRetStatus);
        } else if (pData->SEL.INPUT.uchMajor == FSC_BAR_SIGNIN) {   /* Bartender */
            UifRegData.regopeopen.uchMinorClass = CLASS_UIBARTENDERIN;
            sRetStatus = ItemSignIn(&UifRegData.regopeopen);
            UifRegWorkClear();
            if (sRetStatus != UIF_SUCCESS) {
                if (usLockStatus) {                     /* check transaction lock */
                    if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {  /* reset transaction lock */
                        PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                    }
                    husUifRegHandle = 0;
                }
                if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                    UieReject();                            /* send reject to parent */
                    sRetStatus = (SUCCESS);
                }
                return(sRetStatus);
            } else {
                flUifRegStatus |= UIFREG_BARSIGNIN;
            }
            UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);  /* set echo back */
            UieAccept();                                /* send accepted to parent */
            break;
        } else if (pData->SEL.INPUT.uchMajor == FSC_MSR) {
            if (pData->SEL.INPUT.uchLen) {
                return(LDT_SEQERR_ADR);
            }
            for (usLength = 0; usLength < pData->SEL.INPUT.DEV.MSR.uchLength2; usLength++) {
                if ((pData->SEL.INPUT.DEV.MSR.auchTrack2[usLength] < '0') ||
                    (pData->SEL.INPUT.DEV.MSR.auchTrack2[usLength] > '9')) {
                    break;
                }
            }

			if (pData->SEL.INPUT.DEV.MSR.uchLength2 > 30 || pData->SEL.INPUT.DEV.MSR.auchTrack2[usLength] == ITM_MSR_SEPA || pData->SEL.INPUT.DEV.MSR.auchTrack2[usLength] == ITM_MSR_GIFT_SEPA) {
				// looks like a credit card to me since it has a separator so
				// lets just ignore this swipe.
				NHPOS_NONASSERT_TEXT("MSR swipe credit card ignored.");
                UieReject();
                return (SUCCESS);
			}

            if (usLength == 0) {
                return(LDT_KEYOVER_ADR);
            }

            if ((pData->SEL.INPUT.DEV.MSR.auchTrack2[0] == MSR_ID_CASHIER) || (pData->SEL.INPUT.DEV.MSR.auchTrack2[0] == MSR_ID_SERVER)) {    /* compatible with R3.1 */
                /*----- Convert TRACK II STRING Data to LONG -----*/
                /* saratoga */
                if (usLength > UIFREG_MAX_DIGIT6) {
                    usLength = UIFREG_MAX_DIGIT6;
                }
                _tcsncpy(&pData->SEL.INPUT.aszKey[0], &pData->SEL.INPUT.DEV.MSR.auchTrack2[0], usLength);
                pData->SEL.INPUT.uchLen = (UCHAR)(usLength);
                pData->SEL.INPUT.lData = _ttol(&pData->SEL.INPUT.aszKey[1]);

                /* add dummy secret no. if secret no. is not set */
                if (usLength <= UIFREG_MAX_DIGIT4) {    /* ID + CASHIER# */
                    pData->SEL.INPUT.lData *= 100L;
                }

                UifRegData.regopeclose.uchMinorClass = CLASS_UICASHIEROUT;
                UifRegData.regopeclose.fbInputStatus |= INPUT_MSR;
                sRetStatus = ItemSignOut(&UifRegData.regopeclose);
                UifRegWorkClear();
                if (sRetStatus == UIF_CANCEL) {
                    UieReject();
                    sRetStatus = (SUCCESS);
                } else if (sRetStatus == UIF_CAS_SIGNOUT) {
                    flUifRegStatus |= UIFREG_CASHIERSIGNOUT;
					BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                    UieAccept();
                    sRetStatus = (SUCCESS);
                }
                return(sRetStatus);
            } else if (pData->SEL.INPUT.DEV.MSR.auchTrack2[0] == MSR_ID_BARTENDER) {
				/*----- Convert TRACK II STRING Data to LONG -----*/
                /* saratoga */
                if (usLength > UIFREG_MAX_DIGIT6) {
                    usLength = UIFREG_MAX_DIGIT6;
                }
                _tcsncpy(&pData->SEL.INPUT.aszKey[0], &pData->SEL.INPUT.DEV.MSR.auchTrack2[0], usLength);
                pData->SEL.INPUT.uchLen = (UCHAR)(usLength);
                pData->SEL.INPUT.lData = _ttol(&pData->SEL.INPUT.aszKey[1]);
                /* add dummy secret no. if secret no. is not set */
                if (usLength <= UIFREG_MAX_DIGIT4) {    /* ID + WAITER# */
                    pData->SEL.INPUT.lData *= 100L;
                }
            } else if(pData->SEL.INPUT.DEV.MSR.auchTrack2[0] == MSR_ID_EMPLOYEE) {
				/* JMASON -- Begin MSR ETK Time-In/Out Process */
				sRetStatus = UifItem(pData);
				return(sRetStatus);
			} else {
                return(LDT_SEQERR_ADR);
            }
            UifRegData.regopeopen.uchMajorClass = CLASS_UIFREGOPEN;
            UifRegData.regopeopen.uchMinorClass = CLASS_UIBARTENDERIN;
            UifRegData.regopeopen.ulID = (ULONG)pData->SEL.INPUT.lData;
            UifRegData.regopeopen.fbInputStatus |= INPUT_MSR;
            sRetStatus = ItemSignIn(&UifRegData.regopeopen);    /* Operator Sign In Modele */
            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                if (usLockStatus) {                     /* check transaction lock */
                    if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {  /* reset transaction lock */
                        PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                    }
                    husUifRegHandle = 0;
                }
                if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                    UieReject();                            /* send reject to parent */
                    sRetStatus = (SUCCESS);
                }
                return(sRetStatus);
            } else {
                flUifRegStatus |= UIFREG_BARSIGNIN;
            }
            UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);  /* set echo back */
            UieAccept();                                /* send accepted to parent */
            break;
        } else {
            return(LDT_KEYOVER_ADR);
        }

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT UifWaiterSignOutMSR(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Waiter Sign Out Module
*===========================================================================
*/
UISEQ FARCONST aszSeqRegWaiSignOutMSR[] = {FSC_MSR, FSC_BAR_SIGNIN, 0};
/* UISEQ FARCONST aszSeqRegWaiSignOutMSR[] = {FSC_MSR, FSC_WAITER, FSC_BAR_SIGNIN, 0};  V3.3 */

SHORT   UifWaiterSignOutMSR(KEYMSG *pData)
{
    SHORT   sRetStatus;
    USHORT  usLength;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegWaiSignOutMSR);
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_BAR_SIGNIN) {
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if (aszUifRegNumber[0] != 0) {              /* input number ? */
                return(LDT_SEQERR_ADR);
            }
            UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;
        }
        if (pData->SEL.INPUT.uchMajor == FSC_BAR_SIGNIN) {
            if (flUifRegStatus & UIFREG_WAITERSIGNIN) {
                return(LDT_SEQERR_ADR);
            }
            UifRegData.regopeclose.uchMinorClass = CLASS_UIBARTENDEROUT;
            sRetStatus = ItemSignOut(&UifRegData.regopeclose);
            UifRegWorkClear();
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus != UIF_WAI_SIGNOUT){         /* error ? */
                return(sRetStatus);
            }
            flUifRegStatus |= UIFREG_WAITERSIGNOUT;     /* set waiter sign out status */
            UieAccept();                                /* send accepted to parent */
            break;
        }

        if (pData->SEL.INPUT.uchMajor == FSC_MSR) {
            if (pData->SEL.INPUT.uchLen) {
                return(LDT_SEQERR_ADR);
            }
            for (usLength = 0; usLength < pData->SEL.INPUT.DEV.MSR.uchLength2; usLength++) {
                if ((pData->SEL.INPUT.DEV.MSR.auchTrack2[usLength] < '0') ||
                    (pData->SEL.INPUT.DEV.MSR.auchTrack2[usLength] > '9')) {
                    break;
                }
            }

			if (pData->SEL.INPUT.DEV.MSR.uchLength2 > 30 || pData->SEL.INPUT.DEV.MSR.auchTrack2[usLength] == ITM_MSR_SEPA || pData->SEL.INPUT.DEV.MSR.auchTrack2[usLength] == ITM_MSR_GIFT_SEPA) {
				// looks like a credit card to me since it has a separator so
				// lets just ignore this swipe.
				NHPOS_NONASSERT_TEXT("MSR swipe credit card ignored.");
                UieReject();
                return (SUCCESS);
			}

            if (usLength == 0) {
                return(LDT_KEYOVER_ADR);
            }
            /*----- Convert TRACK II STRING Data to LONG -----*/
            if (pData->SEL.INPUT.DEV.MSR.auchTrack2[0] == MSR_ID_BARTENDER) {
                /* saratoga */
                if (usLength > UIFREG_MAX_DIGIT6) {
                    usLength = UIFREG_MAX_DIGIT6;
                }
                _tcsncpy(&pData->SEL.INPUT.aszKey[0], &pData->SEL.INPUT.DEV.MSR.auchTrack2[0], usLength);
                pData->SEL.INPUT.uchLen = (UCHAR)(usLength);
                 pData->SEL.INPUT.lData = _ttol(&pData->SEL.INPUT.aszKey[1]);
                /* add dummy secret no. if secret no. is not set */
                if (usLength <= UIFREG_MAX_DIGIT4) {    /* ID + WAITER# */
                    pData->SEL.INPUT.lData *= 100L;
                }
            } else {
                return(LDT_SEQERR_ADR);
            }

            if (aszUifRegNumber[0] != 0) {              /* input number ? */
                return(LDT_SEQERR_ADR);
            }
            UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;
            UifRegData.regopeclose.uchMinorClass = CLASS_UIBARTENDEROUT;
            UifRegData.regopeclose.fbInputStatus |= INPUT_MSR;
            sRetStatus = ItemSignOut(&UifRegData.regopeclose);    /* Operator Sign Out Modele */
            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus != UIF_WAI_SIGNOUT){         /* error ? */
                return(sRetStatus);
            }
            flUifRegStatus |= UIFREG_WAITERSIGNOUT;     /* set waiter sign out status */
            UieAccept();                                /* send accepted to parent */
            break;
        }

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT UifCasIntSignInOutMSR(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Cashier Interrupt Sign In/Out Module
*===========================================================================
*/
UISEQ FARCONST aszSeqRegCasIntSignInOutMSR[] = {FSC_MSR, FSC_CASINT, FSC_CASINT_B, 0};

SHORT   UifCasIntSignInOutMSR(KEYMSG *pData)
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegCasIntSignInOutMSR);
        break;

    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        if ((pData->SEL.INPUT.uchMajor == FSC_CASINT) ||
            (pData->SEL.INPUT.uchMajor == FSC_CASINT_B)) {
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                /* do store transaction process */
                _tcsncpy(UifRegData.regtotal.aszNumber, aszUifRegNumber, NUM_NUMBER);
                UifRegData.regtotal.uchMajorClass = CLASS_UIFREGTOTAL;  /* set total class */
                UifRegData.regtotal.uchMinorClass = CLASS_UICASINT;
                sRetStatus = ItemTotal(&UifRegData.regtotal);   /* Total Modele */
                UifRegWorkClear();                              /* clear work area */
                if (sRetStatus == UIF_SUCCESS) {
                    ;   /* successed */
                } else if (sRetStatus == UIF_FINALIZE) {
                    ;   /* successed */
                } else if (sRetStatus == UIF_CAS_SIGNOUT) { 
                    ;   /* successed */
                } else if (sRetStatus == LDT_TAKETL_ADR) {
                    ;   /* successed */
                } else {
                    if (sStatusSave) {                  /* if previous status is enable */
                        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                    }
                    return(sRetStatus);
                }

                /* do current cashier sign-out process */
                if (sRetStatus == UIF_CAS_SIGNOUT) { 
                    /* release on finalize case, sigin-out is done completely */
                    sRetStatus = UIF_SUCCESS;
                } else {
                    UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set cashier sign out class */
                    if (pData->SEL.INPUT.uchMajor == FSC_CASINT) {     /* sign in key ? */
                        UifRegData.regopeclose.uchMinorClass = CLASS_UICASINTOUT;
                    } else {
                        UifRegData.regopeclose.uchMinorClass = CLASS_UICASINTB_OUT;
                    }
                    sRetStatus = ItemSignOut(&UifRegData.regopeclose);    /* Operator Sign Out Modele */
                }

                /* do new cashier sign-in process */
                if (sRetStatus == UIF_SUCCESS) {
                    UifRegData.regopeopen.uchMajorClass = CLASS_UIFREGOPEN;  /* set cashier sign in class */
                    if (pData->SEL.INPUT.uchMajor == FSC_CASINT) {     /* sign in key ? */
                        UifRegData.regopeopen.uchMinorClass = CLASS_UICASINTIN;
                    } else {
                        UifRegData.regopeopen.uchMinorClass = CLASS_UICASINTB_IN;
                    }
                    UifRegData.regopeopen.ulID = (ULONG)pData->SEL.INPUT.lData; /* set cashier number */
                    sRetStatus = ItemSignIn(&UifRegData.regopeopen);    /* Operator Sign In Modele */

                    if (sRetStatus == UIF_SUCCESS) {
                        /* execute to recall transaction from GCF */
                        UifRegData.regtransopen.uchMajorClass = CLASS_UIFREGTRANSOPEN;  /* set add check class */
                        UifRegData.regtransopen.uchMinorClass = CLASS_UICASINTRECALL;   /* cashier interrupt recall */
                        sRetStatus = ItemTransOpen(&UifRegData.regtransopen);   /* Transaction Open Modele */
                        UifRegWorkClear();                          /* clear work area */
                        flUifRegStatus |= UIFREG_CASINTNEWIN;       /* set cashier interrupt new sign-in status */
                        if (sRetStatus == UIF_SUCCESS) {
                            flUifRegStatus |= UIFREG_CASINTRECALL;  /* set cashier interrupt recall status */
                            flUifRegStatus &= ~UIFREG_BUFFERFULL;   /* reset buffer full status */
                            UieAccept();                            /* send accepted to parent */
                            return(UIF_SUCCESS);
                        } else
                        if (sRetStatus == LDT_TAKETL_ADR) {         /* buffer full ? */
                            flUifRegStatus &= ~UIFREG_CASINTRECALL; /* reset cashier interrupt recall status */
                            flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
                            UieAccept();                            /* send accepted to parent */
                            return(sRetStatus);
                        } else
                        if (sRetStatus == LDT_NTINFL_ADR) {
                            flUifRegStatus &= ~UIFREG_CASINTRECALL; /* reset cashier interrupt recall status */
                            flUifRegStatus &= ~UIFREG_BUFFERFULL;   /* reset buffer full status */
                            UieAccept();                            /* send accepted to parent */
                            return(UIF_SUCCESS);
                        }
                        UieAccept();                                 /* send accepted to parent */
                        return (sRetStatus);
                    } else {
                        /* if new cashier sign-in is failed, clear sign-in status */
                        UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set cashier sign out class */
                        UifRegData.regopeclose.uchMinorClass = CLASS_UICASINTOUT2;
                        ItemSignOut(&UifRegData.regopeclose);    /* Operator Sign Out Modele */
                        flUifRegStatus |= UIFREG_CASHIERSIGNOUT; /* set cashier sign out status */
                        UieAccept();                             /* send accepted to parent */
                        if (sRetStatus == UIF_CANCEL) {
                            sRetStatus = UIF_SUCCESS;
                        }
                        return(sRetStatus);
                    }
                } else {
                    UieReject();
                    return(sRetStatus);
                }
            } else {    /* no data input, only sign-out */
                /* do current cashier pre sign-out process */
                UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set cashier sign out class */
                if (pData->SEL.INPUT.uchMajor == FSC_CASINT) {     /* sign in key ? */
                    UifRegData.regopeclose.uchMinorClass = CLASS_UICASINT_PREOUT;
                } else {
                    UifRegData.regopeclose.uchMinorClass = CLASS_UICASINTB_PREOUT;
                }
                sRetStatus = ItemSignOut(&UifRegData.regopeclose);
                if (sRetStatus != UIF_SUCCESS) {
                    if (sStatusSave) {                  /* if previous status is enable */
                        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                    }
                    return(sRetStatus);
                }

                /* do store transaction process */
                _tcsncpy(UifRegData.regtotal.aszNumber, aszUifRegNumber, NUM_NUMBER);
                UifRegData.regtotal.uchMajorClass = CLASS_UIFREGTOTAL;  /* set total class */
                UifRegData.regtotal.uchMinorClass = CLASS_UICASINT;
                sRetStatus = ItemTotal(&UifRegData.regtotal);   /* Total Modele */
                UifRegWorkClear();                              /* clear work area */
                if (sRetStatus == UIF_SUCCESS) {
                    ;   /* successed */
                } else
                if (sRetStatus == UIF_FINALIZE) {
                    ;   /* successed */
                } else
                if (sRetStatus == LDT_TAKETL_ADR) {
                    ;   /* successed */
                } else
                if (sRetStatus == UIF_CAS_SIGNOUT) { 
                    ;   /* successed */
                } else {
                    if (sStatusSave) {                  /* if previous status is enable */
                        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                    }
                    return(sRetStatus);
                }

                /* do current cashier sign-out process */
                if (sRetStatus == UIF_CAS_SIGNOUT) { 
                    /* release on finalize case, sigin-out is done completely */
                    ;
                } else {
                    UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;
                    if (pData->SEL.INPUT.uchMajor == FSC_CASINT) {     /* sign in key ? */
                        UifRegData.regopeclose.uchMinorClass = CLASS_UICASHIEROUT;
                    } else {
                        UifRegData.regopeclose.uchMinorClass = CLASS_UIB_OUT;
                    }
                    sRetStatus = ItemSignOut(&UifRegData.regopeclose);    /* Operator Sign Out Modele */
                }
            }
            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus == UIF_CAS_SIGNOUT) { 
                                                        /* success or sign out ? */
                flUifRegStatus |= UIFREG_CASHIERSIGNOUT; /* set cashier sign out status */
				BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                UieAccept();                            /* send accepted to parent */
                return(SUCCESS);
            }
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                break;
            } else {
                return (sRetStatus);
            }
            break;
        }

        if (pData->SEL.INPUT.uchMajor == FSC_MSR) {
			UIFDIAEMPLOYEE  UifEmployeeData;

            if (pData->SEL.INPUT.uchLen) {
                return(LDT_SEQERR_ADR);
            }

			if (UifCheckMsrEmployeeTrackData (pData->SEL.INPUT.DEV.MSR.auchTrack2, pData->SEL.INPUT.DEV.MSR.uchLength2) != 0) {
				NHPOS_NONASSERT_TEXT("MSR swipe credit card ignored.");
                UieReject();                            /* send reject to parent */
                break;
			}

			sRetStatus = UifTrackToUifDiaEmployee (pData->SEL.INPUT.DEV.MSR.auchTrack2, pData->SEL.INPUT.DEV.MSR.uchLength2, &UifEmployeeData);
			if (pData->SEL.INPUT.DEV.MSR.uchPaymentType == SWIPE_PAYMENT) {
				UifEmployeeData.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_SWIPE;
			} else if (pData->SEL.INPUT.DEV.MSR.uchPaymentType == BIOMETRICS_ENTRY) {
				UifEmployeeData.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_BIOMET;
			}

			if (sRetStatus != UIF_SUCCESS)
				return LDT_NTINFL_ADR;

			if (UifEmployeeData.usSwipeOperation != MSR_ID_CASHIER) {
				return(LDT_SEQERR_ADR);
			}

            /* do store transaction process */
            _tcsncpy(UifRegData.regtotal.aszNumber, aszUifRegNumber, NUM_NUMBER);
            UifRegData.regtotal.uchMajorClass = CLASS_UIFREGTOTAL;  /* set total class */
            UifRegData.regtotal.uchMinorClass = CLASS_UICASINT;
            sRetStatus = ItemTotal(&UifRegData.regtotal);   /* Total Modele */
            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus == UIF_SUCCESS) {
                ;   /* successed */
            } else
            if (sRetStatus == UIF_FINALIZE) {
                ;   /* successed */
            } else
            if (sRetStatus == LDT_TAKETL_ADR) {
                ;   /* successed */
            } else
            if (sRetStatus == UIF_CAS_SIGNOUT) { 
                ;   /* successed */
            } else {
                if (sStatusSave) {                  /* if previous status is enable */
                    ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                }
                return(sRetStatus);
            }

            /* do current cashier sign-out process */
            if (sRetStatus == UIF_CAS_SIGNOUT) { 
                /* release on finalize case, sigin-out is done completely */
                sRetStatus = UIF_SUCCESS;
            } else {
                UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;
                UifRegData.regopeclose.uchMinorClass = CLASS_UICASINTOUT;
                UifRegData.regopeclose.ulID = UifEmployeeData.ulSwipeEmployeeId;
                UifRegData.regopeclose.fbInputStatus |= INPUT_MSR;
                sRetStatus = ItemSignOut(&UifRegData.regopeclose);    /* Operator Sign Out Modele */
            }

            /* do new cashier sign-in process */
            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus == UIF_SUCCESS) {
                UifRegData.regopeopen.uchMajorClass = CLASS_UIFREGOPEN;  /* set cashier sign in class */
                UifRegData.regopeopen.uchMinorClass = CLASS_UICASINTIN;
                UifRegData.regopeopen.ulID = UifEmployeeData.ulSwipeEmployeeId; /* set cashier number */
				UifRegData.regopeopen.ulStatusFlags = UifEmployeeData.ulStatusFlags;
                UifRegData.regopeopen.fbInputStatus |= INPUT_MSR;
                sRetStatus = ItemSignIn(&UifRegData.regopeopen);    /* Operator Sign In Modele */

                /* if new cashier sign-in is failed, clear sign-in status */
                if (sRetStatus != UIF_SUCCESS) {
                    UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set cashier sign out class */
                    UifRegData.regopeclose.uchMinorClass = CLASS_UICASINTOUT2;
                    sRetStatus = ItemSignOut(&UifRegData.regopeclose);    /* Operator Sign Out Modele */
                }
            }

            if (sRetStatus == UIF_CAS_SIGNOUT) { 
                                                        /* success or sign out ? */
                flUifRegStatus |= UIFREG_CASHIERSIGNOUT; /* set cashier sign out status */
				BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
                UieAccept();                            /* send accepted to parent */
                return(SUCCESS);
            }
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                break;
            }
            UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);  /* set echo back */

            /* execute to recall transaction from GCF */
            UifRegData.regtransopen.uchMajorClass = CLASS_UIFREGTRANSOPEN;  /* set add check class */
            UifRegData.regtransopen.uchMinorClass = CLASS_UICASINTRECALL;   /* cashier interrupt recall */
            sRetStatus = ItemTransOpen(&UifRegData.regtransopen);   /* Transaction Open Modele */
            UifRegWorkClear();                          /* clear work area */
            flUifRegStatus |= UIFREG_CASINTNEWIN;       /* set cashier interrupt new sign-in status */
            if (sRetStatus == UIF_SUCCESS) {
                flUifRegStatus |= UIFREG_CASINTRECALL;  /* set cashier interrupt recall status */
                flUifRegStatus &= ~UIFREG_BUFFERFULL;   /* reset buffer full status */
                UieAccept();                            /* send accepted to parent */
                return(UIF_SUCCESS);
            } else
            if (sRetStatus == LDT_TAKETL_ADR) {         /* buffer full ? */
                flUifRegStatus &= ~UIFREG_CASINTRECALL; /* reset cashier interrupt recall status */
                flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
                UieAccept();                            /* send accepted to parent */
                return(sRetStatus);
            } else
            if (sRetStatus == LDT_NTINFL_ADR) {
                flUifRegStatus &= ~UIFREG_CASINTRECALL; /* reset cashier interrupt recall status */
                flUifRegStatus &= ~UIFREG_BUFFERFULL;   /* reset buffer full status */
                UieAccept();                            /* send accepted to parent */
                return(UIF_SUCCESS);
            }
            UieAccept();                                /* send accepted to parent */
            return(sRetStatus);
            break;
        }

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}


/*===== End of Source =====*/
