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
* Title       : Reg ETK Time In Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFETKIN.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifETKTimeIn()  : Reg ETK Time In 
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jun-18-93:01.00.12: K.you     : initial                                   
* Jul-23-93:01.00.12: K.you     : memory compaction                                   
* Nov-10-95:03.01.00: hkato     : R3.1
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
#include <tchar.h>
#include <string.h>
#include <stdio.h>

#include <ecr.h>
#include <pif.h>
#include <uie.h>
#include <fsc.h>
#include <uic.h>
#include <rfl.h>
#include <regstrct.h>
#include <item.h>
#include <paraequ.h>
#include <para.h>
#include <display.h>
#include <uireg.h>
#include <log.h>
#include <appllog.h>
#include "uiregloc.h"
#include "pifmain.h"
#include "csetk.h"
#include "cscas.h"
#include "csstbcas.h"
#include "csstbstb.h"
#include <ConnEngineObjectIf.h>

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegETKTimeIn[] = {FSC_TIME_IN, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

SHORT  UifEmployeeSignTimeMethodString (ULONG ulStatusFlags, TCHAR * aszNumber)
{
	switch (ulStatusFlags & UIFREGMISC_STATUS_METHOD_MASK) {
		case 0:
			_tcscpy (aszNumber, _T("UNKNOWN 0"));
			break;

		case UIFREGMISC_STATUS_METHOD_SWIPE:
			_tcscpy (aszNumber, _T("SWIPED"));
			break;

		case UIFREGMISC_STATUS_METHOD_MANUAL:
			_tcscpy (aszNumber, _T("MANUAL"));
			break;

		case UIFREGMISC_STATUS_METHOD_BIOMET:
			_tcscpy (aszNumber, _T("BIOMETRICS"));
			break;

		case UIFREGMISC_STATUS_METHOD_AUTO:
			_tcscpy (aszNumber, _T("AUTO"));
			break;

		case UIFREGMISC_STATUS_METHOD_FINAL:
			_tcscpy (aszNumber, _T("FINALIZE"));
			break;

		case UIFREGMISC_STATUS_METHOD_MODE:
			_tcscpy (aszNumber, _T("MODE CHANGE"));
			break;

		case 7:
			_tcscpy (aszNumber, _T("UNKNOWN 7"));
			break;

		default:
			_tcscpy (aszNumber, _T("UNKNOWN DEFAULT"));
			break;
	}

	return 0;
}
/*
*===========================================================================
** Synopsis:    SHORT UifETKTimeIn(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Reg ETK Time In Module
*===========================================================================
*/
SHORT UifEmployeeNumberTimeInLookup (UIFDIAEMPLOYEE  *pUifDiaEmployee)
{
	SHORT    sRetStatus;
	SHORT    sLookUpStatus = 0;

	if (CliParaMDCCheckField(MDC_EMPLOYEE_ID_ADR, MDC_PARAM_BIT_A) == 0) {
		// if this is not Amtrak type of installation with auxilary employee file
		// then we will just indicate employee is not on file and return the error.
		return ETK_NOT_IN_FILE;
	}

	pUifDiaEmployee->ulStatusFlags &= ~UIFREGMISC_STATUS_FIRST;
	sRetStatus = EtkStatCheckCreateFirstEmployee (pUifDiaEmployee->ulSwipeEmployeeId);
	if (sRetStatus == SUCCESS || sRetStatus == 1) {
		pUifDiaEmployee->ulStatusFlags |= UIFREGMISC_STATUS_FIRST;
		pUifDiaEmployee->usSwipeEmployeeJobCode = MSR_ID_JOBCODE_SUP_MAX;
		if (sRetStatus == 1)
			sLookUpStatus = -1;   // if EtkStatCheckCreateFirstEmployee() returns 1 then employe id not in username.txt file
		sRetStatus = SUCCESS;     // indicate that we were successful
	} else if (sRetStatus > 0) {
		ETK_JOB  EtkJob = {0};
		TCHAR    aszEmployeeName[21] = {_T('X'), 0};

		// If this is not the first employee then we will create this one with a
		// Job Code of 0 per Amtrak requirements.
		pUifDiaEmployee->usSwipeEmployeeJobCode = 0;   // employees other than first are set to 0 for Job Code
		sLookUpStatus = UifReadAmtrakEmployeeFile (pUifDiaEmployee->ulSwipeEmployeeId, aszEmployeeName);
		if (sLookUpStatus < 0)
			_tcscpy (aszEmployeeName, _T("Unknown"));

		sRetStatus = EtkAssign(pUifDiaEmployee->ulSwipeEmployeeId, &EtkJob, aszEmployeeName);
		if (sRetStatus == SUCCESS) {
			CASIF   CasIf = {0};
			SHORT   sError;

			CasIf.ulCashierNo = pUifDiaEmployee->ulSwipeEmployeeId;
			sError = SerCasIndRead(&CasIf);
			if (sError == CAS_NOT_IN_FILE) {
				// Setup a default Operator Record.  For Amtrak we will prohibit some Operator actions which the Supervisor can change.
				CasIf.ulCashierNo = pUifDiaEmployee->ulSwipeEmployeeId;
				CasIf.ulCashierSecret = pUifDiaEmployee->ulSwipeEmployeePin;
				CasIf.fbCashierStatus[CAS_CASHIERSTATUS_2] |= CAS_PIN_REQIRED_FOR_TIMEIN;     // Amtrak requested this option to be turned on by default
//				CasIf.fbCashierStatus[CAS_CASHIERSTATUS_2] |= CAS_PIN_REQIRED_FOR_SIGNIN;     // Amtrak requested this option to be turned off by default
//				CasIf.fbCashierStatus[CAS_CASHIERSTATUS_3] |= CAS_PIN_REQIRED_FOR_SUPR;       // Amtrak requested this option to be turned off by default
				if (pUifDiaEmployee->usSwipeEmployeeJobCode < MSR_ID_JOBCODE_SUP_REG)
					CasIf.fbCashierStatus[CAS_CASHIERSTATUS_0] = (CAS_OPERATOR_DENIED_AM);

//				CasIf.fbCashierStatus[CAS_CASHIERSTATUS_2] = (CAS_OPEN_CASH_DRAWER_SIGN_IN);  // Amtrak requested this option to be default  turned off, the changed their mind

				_tcscpy (CasIf.auchCashierName, aszEmployeeName);

				SerCasAssign(&CasIf);
			}
			else {
				char  xBuff[128];
				sprintf (xBuff, " EtkAssign() failed: ID = %d  status = %d", pUifDiaEmployee->ulSwipeEmployeeId, sRetStatus);
				NHPOS_NONASSERT_TEXT(xBuff);
				sRetStatus = ETK_NOT_IN_JOB;
			}
		}
		else {
			char  xBuff[128];
			sprintf (xBuff, " EtkStatCheckCreateFirstEmployee() failed: ID = %d  status = %d", pUifDiaEmployee->ulSwipeEmployeeId, sRetStatus);
			NHPOS_NONASSERT_TEXT(xBuff);
			sRetStatus = ETK_NOT_IN_FILE;
		}
	}

	if (sRetStatus == SUCCESS && sLookUpStatus < 0)
		sRetStatus = ETK_UNKNOWN_EMPLOYEE;

	return sRetStatus;
}

SHORT UifEmployeNumherForSignTime (UIFDIAEMPLOYEE  *pUifDiaEmployee)
{
	SHORT           sReturn;
	SHORT           sLen = UIFREG_MAX_DIGIT8;
	REGDISPMSG      Disp = {0};
	UIFDIADATA      DiaData = {0};

	Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
	Disp.uchMinorClass = CLASS_REGDISP_CPAN;
	RflGetLead (Disp.aszMnemonic, LDT_NUM_ADR);

	flDispRegDescrControl |= COMPLSRY_CNTRL;
	Disp.fbSaveControl = 4;
	DispWrite(&Disp);
	flDispRegDescrControl &= ~COMPLSRY_CNTRL;

	DiaData.ulOptions = UIF_PINPAD_OPTION_READ_STRIPE;

	sReturn = UifDiaETK (&DiaData, UIFREG_NO_ECHO);

	if (sReturn == UIF_SUCCESS) {
		if (DiaData.auchFsc[0] == FSC_MSR || DiaData.auchFsc[0] == FSC_VIRTUALKEYEVENT) {
			sReturn = UifTrackToUifDiaEmployee (DiaData.auchTrack2, DiaData.uchTrack2Len, pUifDiaEmployee);
		}
		else if (DiaData.auchFsc[0] == FSC_TIME_IN || DiaData.auchFsc[0] == FSC_TIME_OUT || DiaData.auchFsc[0] == FSC_NUM_TYPE) {
			pUifDiaEmployee->usSwipeOperation = MSR_ID_GENERIC;
			pUifDiaEmployee->ulSwipeEmployeeId = _ttol(DiaData.aszMnemonics);
			pUifDiaEmployee->ulSwipeEmployeePin = 0;
			sReturn = UifFillInEmployeeRecord (pUifDiaEmployee);
		}
		pUifDiaEmployee->ulStatusFlags |= DiaData.ulStatusFlags;
		return(sReturn);
	}
	else {
		return(LDT_SEQERR_ADR);
	}
}

SHORT UifEmployePinNumherForSignTime (UIFDIAEMPLOYEE  *pUifDiaEmployee)
{
	SHORT           sReturn;
	REGDISPMSG      Disp = {0};
	UIFDIADATA      DiaData = {0};

	Disp.uchMajorClass = CLASS_REGDISPCOMPLSRY;
	Disp.uchMinorClass = CLASS_REGDISP_CPAN;
	RflGetLead (Disp.aszMnemonic, LDT_SECRET_ADR);

	flDispRegDescrControl |= COMPLSRY_CNTRL;
	Disp.fbSaveControl = 4;
	DispWrite(&Disp);
	flDispRegDescrControl &= ~COMPLSRY_CNTRL;

	DiaData.ulOptions = 0;      // Require that the Pin number be manually entered not swiped.

	sReturn = UifDiaETK(&DiaData, UIFREG_NO_ECHO);

	if (sReturn == UIF_SUCCESS) {
		// Allow PIN entry followed by Time-in key press only.  do not allow MSR swipe to do PIN entry
		if (DiaData.auchFsc[0] == FSC_TIME_IN || DiaData.auchFsc[0] == FSC_TIME_OUT || DiaData.auchFsc[0] == FSC_NUM_TYPE) {
			pUifDiaEmployee->ulSwipeEmployeePin = _ttol (DiaData.aszMnemonics);
			if ((pUifDiaEmployee->ulStatusFlags & UIFDIAEMPLOYEE_STATUS_ETK_PIN_MIN)) {  // Implement changes for Amtrak
				ULONG  ulPinMin = ((pUifDiaEmployee->ulStatusFlags & UIFDIAEMPLOYEE_STATUS_ETK_PIN_MIN) >> UIFDIAEMPLOYEE_STATUS_ETK_MIN_SHFT);
				if (_tcslen(DiaData.aszMnemonics) < ulPinMin + 5)
					return LDT_PROHBT_ADR;
			}
			return (SUCCESS);
		}
	}

	return (LDT_SEQERR_ADR);
}

SHORT  UifRegEtkGetCheckEmployeeData (ULONG ulEmployeeId, UIFDIAEMPLOYEE  *pUifEmployeeData, SHORT *psTimeInStatus)
{     
	UIFDIAEMPLOYEE  UifDiaData = {0};
    SHORT   sRetStatus = SUCCESS;

	UifDiaData.ulSwipeEmployeeId = pUifEmployeeData->ulSwipeEmployeeId = ulEmployeeId;
	pUifEmployeeData->ulStatusFlags = 0;

	if (ulEmployeeId == 0) {
		// if an employee is has not been entered yet then
		// lets get the employee id and employee data
		sRetStatus = UifEmployeNumherForSignTime (pUifEmployeeData);
	} else {
		sRetStatus = UifFillInEmployeeRecord (pUifEmployeeData);
	}

	if (CliParaMDCCheckField(MDC_EMPLOYEE_ID_ADR, MDC_PARAM_BIT_A)) {  // Implement changes for Amtrak
		pUifEmployeeData->ulStatusFlags |= UIFDIAEMPLOYEE_STATUS_ETK_MIN_8;
	}

	if (sRetStatus == SUCCESS || sRetStatus == ETK_NOT_IN_FILE) {
		*psTimeInStatus = sRetStatus;   // save the status to report to Connection Engine below.
		if (CliParaMDCCheckField(MDC_EMPLOYEE_ID_ADR, MDC_PARAM_BIT_C) || (pUifEmployeeData->ulStatusFlags & UIFDIAEMPLOYEE_STATUS_ETK_PIN_REQ) != 0) {
			UifDiaData.ulSwipeEmployeeId = pUifEmployeeData->ulSwipeEmployeeId;
			UifDiaData.ulStatusFlags = pUifEmployeeData->ulStatusFlags;
			if (UifEmployePinNumherForSignTime (&UifDiaData) == SUCCESS) {
				// The following is to work around a field issue seen on some Amtrak trains in which
				// the initialization sequence which beings with the Start Log In screen in which
				// the LSA enters Employee Id, Secret Code, and a Loan Amount followed by starting up
				// the Inventory app for other train trip startup activities does not complete properly.
				// The result is that the LSA is unable to complete the Start Log In screen task and
				// is stuck with it partially complete. A control string is used to automate this task
				// so as part of the sanity check as to whether to allow this workaround, check that a
				// control string is running.
				//
				// This change checks if the LSA already has a Security Code in the Database or not to
				// determine how to do the Security Code check.
				//      Richard Chambers, Aug-17-2018, for Amtrak only
				if (pUifEmployeeData->ulSwipeEmployeePin != 0) {
					if (pUifEmployeeData->ulSwipeEmployeePin != UifDiaData.ulSwipeEmployeePin) {
						sRetStatus = LDT_KEYOVER_ADR;
					}
				} else if (pUifEmployeeData->ulSwipeEmployeeId != UifDiaData.ulSwipeEmployeePin) {
						sRetStatus = LDT_KEYOVER_ADR;
				}
			}
			else {
				sRetStatus = LDT_ETKERR_ADR;
			}
		}
	}
	else {
		sRetStatus = LDT_ETKERR_ADR;
	}

	if (sRetStatus != SUCCESS && sRetStatus != ETK_NOT_IN_FILE)
		return sRetStatus;

	// For Amtrak, if an employee is trying to Time-in and his employee number is not
	// in the employee data file then we will try to create him with a Job Code of 0.
	// The ability to create new Employee Data records can be done on Master Terminal only.
	if (CliParaMDCCheckField(MDC_EMPLOYEE_ID_ADR, MDC_PARAM_BIT_A)) {  // Check to see if Amtrak style Time-in discovery is enabled
		if (sRetStatus == ETK_NOT_IN_FILE) {
			if (CliCheckAsMaster() == STUB_SUCCESS) {   /* Master */
				pUifEmployeeData->ulSwipeEmployeePin = UifDiaData.ulSwipeEmployeePin;
				sRetStatus = UifEmployeeNumberTimeInLookup (pUifEmployeeData);
				if (sRetStatus == ETK_UNKNOWN_EMPLOYEE) {
					*psTimeInStatus = ETK_FILE_NOT_FOUND;   // save the status to report to Connection Engine below.
				}
				else if (sRetStatus < 0) {
					return(LDT_ETKERR_ADR);
				}
			}
			else {
				return(LDT_I_AM_NOT_MASTER);
			}
		} else {
			// we need to decide if this employee should be the LSA or not. we decide that
			// be looking to see if anyone else is logged in. if not then make this person an LSA.
			// otherwise we just use the employee data as provided.

			// The following is to work around a field issue seen on some Amtrak trains in which
			// the initialization sequence which beings with the Start Log In screen in which
			// the LSA enters Employee Id, Secret Code, and a Loan Amount followed by starting up
			// the Inventory app for other train trip startup activities does not complete properly.
			// The result is that the LSA is unable to complete the Start Log In screen task and
			// is stuck with it partially complete. A control string is used to automate this task
			// so as part of the sanity check as to whether to allow this workaround, check that a
			// control string is running.
			//
			// This change checks if the LSA has already done a Time-in as part of the Log-in task
			// and if so then we will just report success.
			//      Richard Chambers, Aug-17-2018, for Amtrak only
			ULONG           etkList[50];
			SHORT           sLoggedInCount = 0;

			sLoggedInCount = EtkStatCheckEmployeesLoggedIn (pUifEmployeeData->ulSwipeEmployeeId, etkList, 50);
			pUifEmployeeData->ulStatusFlags &= ~UIFREGMISC_STATUS_FIRST;
			if (sLoggedInCount < 1) {
				EtkStatCheckCreateFirstEmployee (pUifEmployeeData->ulSwipeEmployeeId);
				pUifEmployeeData->ulStatusFlags |= UIFREGMISC_STATUS_FIRST;
				pUifEmployeeData->usSwipeEmployeeJobCode = MSR_ID_JOBCODE_SUP_MAX;
			} else if (sLoggedInCount == 1 && pUifEmployeeData->ulSwipeEmployeeId == etkList[0]) {
				pUifEmployeeData->ulStatusFlags |= UIFREGMISC_STATUS_FIRST;
				pUifEmployeeData->usSwipeEmployeeJobCode = MSR_ID_JOBCODE_SUP_MAX;
			} else {
				NHPOS_NONASSERT_NOTE("==NOTE", "==NOTE: UifRegEtkGetCheckEmployeeData() EtkStatCheckEmployeesLoggedIn() reports > 0.");
				PifLog(MODULE_UI, LOG_EVENT_ETK_WARN_LOG_01);
				PifLog(MODULE_LINE_NO(MODULE_UI), __LINE__);
			}
		}
	}

	return sRetStatus;
}

SHORT UifETKTimeIn(KEYMSG *pData)
{
    SHORT   sRetStatus;
    USHORT  usLockStatus = 0;

    switch (pData->uchMsg) {
    case UIM_INIT:                                  
        UieOpenSequence(aszSeqRegETKTimeIn);        /* register key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_TIME_IN) {     
			UIFDIAEMPLOYEE  UifDiaEmployee = {0};
			SHORT           sTimeInStatus = SUCCESS;

			sRetStatus = UifRegEtkGetCheckEmployeeData ((ULONG)pData->SEL.INPUT.lData, &UifDiaEmployee, &sTimeInStatus);
			if (sRetStatus == ETK_UNKNOWN_EMPLOYEE) {
				if (! CliParaMDCCheckField(MDC_EMPLOYEE_ID_ADR, MDC_PARAM_BIT_A)) {  // Check to see if Amtrak style Time-in discovery is enabled
					return LDT_NTINFL_ADR;
				}
			}
			else if (sRetStatus != SUCCESS) {
				return EtkConvertError(sRetStatus);
			}

            if (!husUifRegHandle) {             /* not during transaction lock */
                if ((husUifRegHandle = UicCheckAndSet()) == UIC_NOT_IDLE) {  /* check transaction lock *//* ### MOD (2172 Rel1.0) */
                    husUifRegHandle = 0;
                    return(LDT_LOCK_ADR);
                }
                usLockStatus = 1;
            }

			if (pData->SEL.INPUT.lData != 0) {
				UifDiaEmployee.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
			}

			UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;  
            UifRegData.regmisc.uchMinorClass = CLASS_UIETKIN;                   
            UifRegData.regmisc.ulEmployeeNo = UifDiaEmployee.ulSwipeEmployeeId;
			UifRegData.regmisc.uchJobCode = UifDiaEmployee.usSwipeEmployeeJobCode;
			UifRegData.regmisc.sTimeInStatus = sTimeInStatus;
			UifRegData.regmisc.ulStatusFlags = UifDiaEmployee.ulStatusFlags;
            sRetStatus = ItemMiscETK(&UifRegData.regmisc);

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

            UieAccept();                                
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));               
    }
    return (SUCCESS);
}

/****** End of Source ******/
