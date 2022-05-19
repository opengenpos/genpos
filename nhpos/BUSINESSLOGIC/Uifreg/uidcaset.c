/*

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation

*===========================================================================
* Title       : Reg Cashier Number Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDCASET.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifDiaCashierEntry() : Reg Cashier Number Entry
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-12-92:00.00.01:M.Suzuki   : initial                                   
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

#include <tchar.h>
#include <string.h>
#include <stdlib.h>

#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <regstrct.h>
#include <paraequ.h>
#include <uireg.h>
#include "uiregloc.h"

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifDiaCashierEntry(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Cashier Number Entry Module                R3.1
*===========================================================================
*/
SHORT UifDiaCashierEntry(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_AC:                   // UifDiaCashierEntry() process sign in for supervisor 
		case FSC_P1:                   // UifDiaCashierEntry() process sign in for supervisor, P1 key is same as Num Type key (FSC_NUM_TYPE) as used in several places so be consistent
        case FSC_NUM_TYPE:             // UifDiaCashierEntry() process sign in for supervisor 
        case FSC_SIGN_IN:              // UifDiaCashierEntry() process sign in for cashier 
        case FSC_B:
        case FSC_CASINT:    /* V3.3 */
        case FSC_CASINT_B:  /* V3.3 */
        case FSC_BAR_SIGNIN:/* V3.3 */
            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_ID_DIGITS)) != SUCCESS) { /* input check */
                return(sRetStatus);
            }
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
			UifRegDiaWork.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
            return(UIF_DIA_SUCCESS);

        case FSC_MSR:
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
			if (pData->SEL.INPUT.DEV.MSR.uchPaymentType == SWIPE_PAYMENT) {
				UifRegDiaWork.ulStatusFlags = UIFREGMISC_STATUS_METHOD_SWIPE;
			} else if (pData->SEL.INPUT.DEV.MSR.uchPaymentType == BIOMETRICS_ENTRY) {
				UifRegDiaWork.ulStatusFlags = UIFREGMISC_STATUS_METHOD_BIOMET;
			}

			NHPOS_ASSERT(pData->SEL.INPUT.uchLen <= NUM_NUMBER);	//NHPOS ASSERT JHHJ
            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);

            UifRegDiaWork.uchTrack1Len = pData->SEL.INPUT.DEV.MSR.uchLength1;
			NHPOS_ASSERT(pData->SEL.INPUT.DEV.MSR.uchLength1 <= PIF_LEN_TRACK1); //NHPOS ASSERT JHHJ
            _tcsncpy(UifRegDiaWork.auchTrack1, pData->SEL.INPUT.DEV.MSR.auchTrack1, pData->SEL.INPUT.DEV.MSR.uchLength1);

            UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.DEV.MSR.uchLength2;
			NHPOS_ASSERT(pData->SEL.INPUT.DEV.MSR.uchLength2 <= PIF_LEN_TRACK2); //NHPOS ASSERT JHHJ
            _tcsncpy(UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.MSR.auchTrack2, pData->SEL.INPUT.DEV.MSR.uchLength2);

            UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.DEV.MSR.uchLength2;
            return(UIF_DIA_SUCCESS);

        /* V3.3 */
        case FSC_WAITER_PEN:
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.uchMinor;   /* set input data */
            return(UIF_DIA_SUCCESS);

        case FSC_CANCEL:
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            return(UIFREG_ABORT);

        case FSC_AUTO_SIGN_OUT:
            return(UIFREG_ABORT);

        case FSC_RECEIPT_FEED:
        case FSC_JOURNAL_FEED:
        case FSC_RESET_LOG:							/* avoid key sequence error by PCIF function, 11/12/01 */
        case FSC_POWER_DOWN:
            return(UifRegDefProc(pData));           /* reg default process */

        default:                                        
            return(LDT_SEQERR_ADR);
        }

    case UIM_REDISPLAY:
    case UIM_DRAWER:
        return(UieDefProc(pData));                  /* engine default process */

    case UIM_INIT:
    case UIM_CANCEL:
        break;

    default:                                        /* not use */
        return(LDT_SEQERR_ADR);
    }
    return (SUCCESS);
}

