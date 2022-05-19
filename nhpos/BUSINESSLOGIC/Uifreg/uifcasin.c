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
* Title       : Reg Cashier Sign In Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFCASIN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifCashierSignIn() : Reg Cashier Sign In
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-07-92:00.00.01:M.Suzuki   : initial                                   
* Jun-21-93:01.00.12:K.You      : add direct menu shift key.
* Jun-02-98:03.03.00:M.Ozawa    : add cashier interrupt key.
*          :        :           :                                    
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
#include	<stdlib.h>

#include <ecr.h>
#include <log.h>
#include <pif.h>
#include <uie.h>
#include <fsc.h>
#include <uic.h>
#include <regstrct.h>
#include <item.h>
#include <paraequ.h>
#include <uireg.h>
#include <appllog.h>
#include "uiregloc.h"
#include <para.h>
#include <csstbpar.h>
#include "transact.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegCasSignIn[] = {FSC_SIGN_IN, FSC_B, FSC_TAX_MODIF,
                                       FSC_VOID, FSC_MENU_SHIFT, FSC_NUM_TYPE,
                                       FSC_D_MENU_SHIFT, 0};

UISEQ FARCONST aszSeqRegCasIntSignIn[] = {FSC_CASINT, FSC_CASINT_B, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifCashierSignIn(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Cashier Sign In Module
*===========================================================================
*/
SHORT UifCashierSignIn(KEYMSG *pData)
{
    SHORT   sRetStatus;
	TRANMODEQUAL    WorkMode;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegCasSignIn);            /* open cashier sign in key sequence */
        break;

    case UIM_INPUT:
        if ((pData->SEL.INPUT.uchMajor == FSC_SIGN_IN) || (pData->SEL.INPUT.uchMajor == FSC_B)) {
			UIFDIAEMPLOYEE  UifDiaEmployee;

            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_ID_DIGITS)) != SUCCESS) { /* input check */
                return(sRetStatus);
            }
            if (aszUifRegNumber[0] != 0) {              /* input number ? */
                return(LDT_SEQERR_ADR);
            }

			// check and set the keyboard and transaction lock.  this lock is used
			// to ensure that Cashier can not Sign-in if there is a remote application
			// that has locked the keyboard for actions that must not be done in
			// parallel to Cashier actions.
			// if the lock works, you must use UicResetFlag() to unlock.  we have seen
			// mysterious During Lock (error 22) which were due to a successful lock
			// that was not reset with an error during Sign-in.
            if ((husUifRegHandle = UicCheckAndSet()) == UIC_NOT_IDLE) {  /* check transaction lock *//* ### MOD (2172 Rel1.0) */
                husUifRegHandle = 0;
				NHPOS_NONASSERT_NOTE("==STATE", "==STATE: Sign-in failed UicCheckAndSet() != UIC_NOT_IDLE");
                return(LDT_LOCK_ADR);
            }

			UifDiaEmployee.usSwipeOperation = MSR_ID_GENERIC;
			UifDiaEmployee.ulSwipeEmployeeId = (ULONG)pData->SEL.INPUT.lData;
			UifDiaEmployee.ulSwipeEmployeePin = 0;
			sRetStatus = UifFillInEmployeeRecord (&UifDiaEmployee);
			UifDiaEmployee.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
			if (sRetStatus != UIF_SUCCESS) {
				// there was a problem with the employee lookup so we need to
				// reset the lock created above before returning an error.
                if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {  /* reset transaction lock */
					PifLog(MODULE_UI, LOG_EVENT_UIFUIE_REG_SIGNIN);                       /* Write sign-in log     */
                    PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
                    PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sRetStatus));
                    PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
                    PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                }
                husUifRegHandle = 0;
				return LDT_NTINFL_ADR;
			}

            UifRegData.regopeopen.uchMajorClass = CLASS_UIFREGOPEN;
            if (pData->SEL.INPUT.uchMajor == FSC_SIGN_IN) {     /* sign in key ? */
                UifRegData.regopeopen.uchMinorClass = CLASS_UICASHIERIN;
            } else {
                UifRegData.regopeopen.uchMinorClass = CLASS_UIB_IN;
            }
            UifRegData.regopeopen.ulID = (ULONG)pData->SEL.INPUT.lData; /* set cashier number */
			UifRegData.regopeopen.ulStatusFlags = UifDiaEmployee.ulStatusFlags;
			if (UifDiaEmployee.ulStatusFlags & UIFDIAEMPLOYEE_STATUS_REG_DENY_A) {
				UifRegData.regopeopen.uchMinorClass = CLASS_UIB_IN;   // if Sign-in and Drawer A is denied then we will auto assign Drawer B
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

            sRetStatus = ItemSignIn(&UifRegData.regopeopen);    /* Operator Sign In Modele */
			UifRegCheckDrawerLimit(0);					//SR155 CashDrawer Limit
            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
				// there was a problem with the Sign-in so we need to
				// reset the lock created above before returning an error.
                if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {  /* reset transaction lock */
					PifLog(MODULE_UI, LOG_EVENT_UIFUIE_REG_SIGNIN);                       /* Write sign-in log     */
                    PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
                    PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sRetStatus));
                    PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
                    PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                }
                husUifRegHandle = 0;
                if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                    UieReject();                            /* send reject to parent */
                    return (SUCCESS);
                }
                return(sRetStatus);
            }

			PifLog(MODULE_UI, LOG_EVENT_UIFUIE_REG_SIGNIN);                       /* Write sign-in log     */
			PifLog(MODULE_DATA_VALUE(MODULE_UI), pData->SEL.INPUT.uchMajor);      /* Write FSC drawer log  */

			// The following code is used to store the drawer status
			// that is used in the regester mode pickup functionality
			// ***PDINU
			TrnGetModeQual( &WorkMode );     /* get mode qualifier data */
			WorkMode.uchCashierDrawer = pData->SEL.INPUT.uchMajor;
			TrnPutModeQual( &WorkMode );     /* put mode qualifier data */

            UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);  /* set echo back */
            UieAccept();                                /* send accepted to parent */
	        UieCtrlDevice(UIE_ENA_SCANNER);                 /* correct enable position, V1.0.13 */
            break;
        }
        if ((pData->SEL.INPUT.uchMajor == FSC_TAX_MODIF) ||
            (pData->SEL.INPUT.uchMajor == FSC_VOID) ||
            (pData->SEL.INPUT.uchMajor == FSC_MENU_SHIFT) ||
            (pData->SEL.INPUT.uchMajor == FSC_D_MENU_SHIFT) ||
            (pData->SEL.INPUT.uchMajor == FSC_NUM_TYPE)) {
            return(LDT_SEQERR_ADR);
        }
        /* break */                                     /* not use */

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }

    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT UifCasIntSignIn(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Cashier Interrupt Sign In Module, R3.3
*===========================================================================
*/
SHORT UifCasIntSignIn(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegCasIntSignIn);             /* open cashier interrupt sign in key sequence */
        break;

    case UIM_INPUT:
        if ((pData->SEL.INPUT.uchMajor == FSC_CASINT) || (pData->SEL.INPUT.uchMajor == FSC_CASINT_B)) {
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_ID_DIGITS)) != SUCCESS) { /* input check */
                return(sRetStatus);
            }
            if (aszUifRegNumber[0] != 0) {              /* input number ? */
                return(LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.uchLen == 0) {
                return(LDT_SEQERR_ADR);                 /* key sequence error at no sign-in case */
            } else {                                    /* exist input data ? */
				// check and set the keyboard and transaction lock.  this lock is used
				// to ensure that Cashier can not Sign-in if there is a remote application
				// that has locked the keyboard for actions that must not be done in
				// parallel to Cashier actions.
                if ((husUifRegHandle = UicCheckAndSet()) == UIC_NOT_IDLE) {  /* check transaction lock *//* ### MOD (2172 Rel1.0) */
                    husUifRegHandle = 0;
                    return(LDT_LOCK_ADR);
                }

                /* first sign-in case */
                UifRegData.regopeopen.uchMajorClass = CLASS_UIFREGOPEN;  /* set cashier sign in class */
                if (pData->SEL.INPUT.uchMajor == FSC_CASINT) {     /* sign in key ? */
                    UifRegData.regopeopen.uchMinorClass = CLASS_UICASINTIN;
                } else {
                    UifRegData.regopeopen.uchMinorClass = CLASS_UICASINTB_IN;
                }
                UifRegData.regopeopen.ulID = (ULONG)pData->SEL.INPUT.lData; /* set cashier number */
                sRetStatus = ItemSignIn(&UifRegData.regopeopen);    /* Operator Sign In Modele */
            }

            UifRegWorkClear();                          /* clear work area */
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
				// there was a problem with the Sign-in so we need to
				// reset the lock created above before returning an error.
                if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {  /* reset transaction lock */
					PifLog(MODULE_UI, LOG_EVENT_UIFUIE_REG_SIGNIN);                       /* Write sign-in log     */
                    PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
                    PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sRetStatus));
                    PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
                    PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                }
                husUifRegHandle = 0;
                if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                    UieReject();                            /* send reject to parent */
                    return (SUCCESS);
                }
                return(sRetStatus);
            }

            flUifRegStatus |= UIFREG_CASINTSIGNIN;
            UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);  /* set echo back */

            /* execute to recall transaction from GCF */
            UifRegData.regtransopen.uchMajorClass = CLASS_UIFREGTRANSOPEN;  /* set add check class */
            UifRegData.regtransopen.uchMinorClass = CLASS_UICASINTRECALL;   /* cashier interrupt recall */
            sRetStatus = ItemTransOpen(&UifRegData.regtransopen);   /* Transaction Open Modele */
            UifRegWorkClear();                          /* clear work area */
            if (sRetStatus == UIF_SUCCESS) {
                flUifRegStatus |= UIFREG_CASINTRECALL;  /* set cashier interrupt recall status */
                flUifRegStatus &= ~UIFREG_BUFFERFULL;   /* reset buffer full status */
                UieAccept();                            /* send accepted to parent */
		        UieCtrlDevice(UIE_ENA_SCANNER);         /* correct enable position, V1.0.13 */
                return(UIF_SUCCESS);
            } else
            if (sRetStatus == LDT_TAKETL_ADR) {         /* buffer full ? */
                flUifRegStatus &= ~UIFREG_CASINTRECALL; /* reset cashier interrupt recall status */
                flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
                UieAccept();                            /* send accepted to parent */
    		    UieCtrlDevice(UIE_ENA_SCANNER);         /* correct enable position, V1.0.13 */
                return(sRetStatus);
            } else
            if (sRetStatus == LDT_NTINFL_ADR) {
                flUifRegStatus &= ~UIFREG_CASINTRECALL; /* reset cashier interrupt recall status */
                flUifRegStatus &= ~UIFREG_BUFFERFULL;   /* reset buffer full status */
                UieAccept();                            /* send accepted to parent */
	        	UieCtrlDevice(UIE_ENA_SCANNER);         /* correct enable position, V1.0.13 */
                return(UIF_SUCCESS);
            }
            UieAccept();                                /* send accepted to parent */
        	UieCtrlDevice(UIE_ENA_SCANNER);         /* correct enable position, V1.0.13 */
            return(sRetStatus);
            break;
        }
        /* break */                                     /* not use */

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (UIF_SUCCESS);
}


