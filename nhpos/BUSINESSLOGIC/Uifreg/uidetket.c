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
* Title       : Reg ETK Job Code Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDETKET.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*                               
*               UifDiaETKEntry(): Reg ETK Job Code Entry
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jun-18-93:01.00.12: K.You     : initial                                   
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
** Synopsis:    SHORT UifDiaETKEntry(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Reg ETK Job Code Entry
*===========================================================================
*/
SHORT UifDiaETKEntry(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_TIME_OUT:          // UifDiaETKEntry() allow use of Time-out key to enter employee number 
        case FSC_TIME_IN:           // UifDiaETKEntry() allow use of Time-in key to enter employee number 
        case FSC_NUM_TYPE:          // UifDiaETKEntry() allow use of #/Type key to enter employee number 
            if ((sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_DIGIT9)) != SUCCESS) {
                return(sRetStatus);
            }
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
			memset (UifRegDiaWork.aszMnemonics, 0, sizeof(UifRegDiaWork.aszMnemonics));
            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);
			UifRegDiaWork.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
			pData->SEL.INPUT.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
            return(UIF_DIA_SUCCESS);

        case FSC_VIRTUALKEYEVENT:
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
			UifRegDiaWork.uchPaymentType = SWIPE_PAYMENT;

			NHPOS_ASSERT(pData->SEL.INPUT.uchLen <= NUM_NUMBER);	//NHPOS ASSERT JHHJ
            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);

			NHPOS_ASSERT(pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength1 <= PIF_LEN_TRACK1); //NHPOS ASSERT JHHJ
            _tcsncpy(UifRegDiaWork.auchTrack1, pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.auchTrack1, pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength1);

            UifRegDiaWork.uchTrack1Len = pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength1;

			NHPOS_ASSERT(pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength2 <= PIF_LEN_TRACK2); //NHPOS ASSERT JHHJ
            _tcsncpy(UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.auchTrack2, pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength2);

            UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.DEV.VIRTUALKEY.u.msr_data.uchLength2;
            return(UIF_DIA_SUCCESS);

        case FSC_MSR:
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            memset(&UifRegDiaWork, '\0', sizeof(UifRegDiaWork));
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;

			NHPOS_ASSERT(pData->SEL.INPUT.uchLen <= NUM_NUMBER);	//NHPOS ASSERT JHHJ
            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);

			NHPOS_ASSERT(pData->SEL.INPUT.DEV.MSR.uchLength2 <= PIF_LEN_TRACK2); //NHPOS ASSERT JHHJ
            _tcsncpy(UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.MSR.auchTrack2, pData->SEL.INPUT.DEV.MSR.uchLength2);

            UifRegDiaWork.uchTrack2Len = pData->SEL.INPUT.DEV.MSR.uchLength2;
			if (pData->SEL.INPUT.DEV.MSR.uchPaymentType == BIOMETRICS_ENTRY) {
				UifRegDiaWork.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_BIOMET;
				pData->SEL.INPUT.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_BIOMET;
			} else {
				UifRegDiaWork.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_SWIPE;
				pData->SEL.INPUT.ulStatusFlags |= UIFREGMISC_STATUS_METHOD_SWIPE;
			}
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

