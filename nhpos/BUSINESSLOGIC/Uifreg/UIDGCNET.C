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
* Title       : Reg Guest Check Number Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDGCNET.C
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifDiaGuestNoEntry() : Reg Guest Check Number Entry
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
#include <memory.h>

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
** Synopsis:    SHORT UifDiaGuestNoEntry(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Guest Check Number Entry Module
*===========================================================================
*/
SHORT UifDiaGuestNoEntry(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
		case FSC_VIRTUALKEYEVENT:
			if (pData->SEL.INPUT.uchMajor == FSC_VIRTUALKEYEVENT) {
				// Transform scanner date from a virtual keyboard event scanner data to a standard scanner data
				// We do this to make the virtual keyboard event look like just another scanner event.
				if (pData->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType == UIE_VIRTUALKEY_EVENT_SCANNER) {
					UIE_SCANNER  tempbuf;

					memcpy (&tempbuf, &(pData->SEL.INPUT.DEV.VIRTUALKEY.u.scanner_data), sizeof(UIE_SCANNER));
					pData->SEL.INPUT.DEV.SCA = tempbuf;
				} else {
					/* --- return lead thru message number --- */
					return(LDT_KEYOVER_ADR);
				}
				pData->SEL.INPUT.uchMajor = FSC_SCANNER;
			} else {
				/* --- return lead thru message number --- */
				return(LDT_KEYOVER_ADR);
			}
			// fall through and process like it is a scanner.
		case FSC_SCANNER:
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
			memset (UifRegDiaWork.auchTrack2, 0, sizeof(UifRegDiaWork.auchTrack2));
			_tcsncpy (UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.SCA.auchStr, TCHARSIZEOF(UifRegDiaWork.auchTrack2) - 1);
			UifRegDiaWork.ulData = _ttol(UifRegDiaWork.auchTrack2);
			return (UIF_DIA_SUCCESS);

		case FSC_NUM_TYPE:
            UifRegDiaWork.auchFsc[0] = FSC_SCANNER;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
			memset (UifRegDiaWork.auchTrack2, 0, sizeof(UifRegDiaWork.auchTrack2));
			_tcsncpy (UifRegDiaWork.auchTrack2, pData->SEL.INPUT.aszKey, TCHARSIZEOF(UifRegDiaWork.auchTrack2) - 1);
			_tcsncpy (pData->SEL.INPUT.DEV.SCA.auchStr, pData->SEL.INPUT.aszKey, TCHARSIZEOF(pData->SEL.INPUT.DEV.SCA.auchStr) - 1);
			UifRegDiaWork.ulData = _ttol(UifRegDiaWork.auchTrack2);
            return(UIF_DIA_SUCCESS);

        case FSC_GUEST_CHK_NO:
            if ((sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_DIGIT6)) != SUCCESS) { /* input check */
                return(sRetStatus);
            }
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
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

        case FSC_STRING:                            /* Control String No Key */
            sRetStatus = UifRegString(pData);
			if (sRetStatus == SUCCESS)
				sRetStatus = UIF_STRING_START;
			return sRetStatus;

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

