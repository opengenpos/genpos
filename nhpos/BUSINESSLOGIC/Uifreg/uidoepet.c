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
* Title       : Order Number Entry 
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDOEPET.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               UifDiaOEPEntry() : Order Number Number Entry
* --------------------------------------------------------------------------
* Update Histories
*    Data  :Ver.Rev.:   Name    : Description
* Feb-28-95:03.00.00: hkato     : R3.0
* Dec-01-95:03.01.00: M.Ozawa   : Enhanced to R3.1 feature
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
#include <pifmain.h>
#include "uiregloc.h"
#include "BlFWif.h"

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifDiaOEPEntry(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Order Number Entry Module
*===========================================================================
*/
SHORT   UifDiaOEPEntry(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
			// allow fewer than 4 digits to be terminated by a #/Type key press.
			// this is for keyboard type systems.
        case FSC_AN:
            if (pData->SEL.INPUT.uchLen != 2 && pData->SEL.INPUT.uchLen != 4) {         /* not 2 or 4 digits then not an OEP entry */
                return (LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.aszKey[0] < '0' ||
                pData->SEL.INPUT.aszKey[0] > '9' ||
                pData->SEL.INPUT.aszKey[1] < '0' ||
                pData->SEL.INPUT.aszKey[1] > '9') {
                return (LDT_SEQERR_ADR);
            }
            if (pData->SEL.INPUT.lData != 0L) {
                if ((sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_DIGIT4)) != SUCCESS) {
                    return (sRetStatus);
                }
            }
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
            UifRegDiaWork.ulData     = (ULONG)pData->SEL.INPUT.lData;
            return (UIF_DIA_SUCCESS);
        
        case FSC_KEYED_STRING:
            sRetStatus = UifRegString(pData);
            if (pData->SEL.INPUT.uchLen != 0) {      
                return(LDT_SEQERR_ADR);
            }
            if (sRetStatus != UIF_SUCCESS) {
                return(sRetStatus);
            }
            UieKeepMessage();                       /* keep message */
            return(SUCCESS);

		case FSC_VIRTUALKEYEVENT:
			if (pData->SEL.INPUT.uchMajor == FSC_VIRTUALKEYEVENT) {
				// Transform scanner date from a virtual keyboard event scanner data to a standard scanner data
				// We do this to make the virtual keyboard event look like just another scanner event.
				if (pData->SEL.INPUT.DEV.VIRTUALKEY.usKeyEventType == UIE_VIRTUALKEY_EVENT_SCANNER) {
					UIE_SCANNER  tempbuf = {0};

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
			if (UifRegDiaWork.ulOptions & UIF_PAUSE_OPTION_ALLOW_SCAN) {
				UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
				UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
				memset (UifRegDiaWork.auchTrack2, 0, sizeof(UifRegDiaWork.auchTrack2));
				_tcsncpy (UifRegDiaWork.auchTrack2, pData->SEL.INPUT.DEV.SCA.auchStr, TCHARSIZEOF(UifRegDiaWork.auchTrack2) - 1);
				UifRegDiaWork.ulData = _ttol(UifRegDiaWork.auchTrack2);
				return (UIF_DIA_SUCCESS);
			} else {
				/* --- return lead thru message number --- */
				return(LDT_KEYOVER_ADR);
			}

        case FSC_SCROLL_UP:                         /* R3.1 */
        case FSC_SCROLL_DOWN:
        case FSC_AC:
        case FSC_KEYED_PLU:
        case FSC_D_MENU_SHIFT:
        case FSC_ADJECTIVE:
            if (pData->SEL.INPUT.uchLen) {          /* not 2 digits */
                return (LDT_SEQERR_ADR);
            }
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
            return (UIF_DIA_SUCCESS);

        case FSC_MENU_SHIFT:                        /* R3.1 */
            if (pData->SEL.INPUT.uchLen > 1) { 
                return (LDT_SEQERR_ADR);
            }
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
            UifRegDiaWork.ulData     = (ULONG)pData->SEL.INPUT.lData;
            return (UIF_DIA_SUCCESS);

        case FSC_CANCEL:
            if (pData->SEL.INPUT.uchLen != 0) {     /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            return(UIFREG_ABORT);

		case FSC_WINDOW_DISPLAY:
			sRetStatus = UIF_DIA_RETRY_INPUT;
			if (pData->SEL.INPUT.uchLen < 1) {
				sRetStatus = LDT_SEQERR_ADR;
			}
			else {
				if (pData->SEL.INPUT.uchLen > 4 && pData->SEL.INPUT.aszKey[0] == _T('9')) {
					BlFwIfPopupWindowGroup(pData->SEL.INPUT.aszKey+4);	//the sequence of digits previous to this button press should be the window we want to popup
				} else {
					BlFwIfPopupWindowPrefix(pData->SEL.INPUT.aszKey);	//the sequence of digits previous to this button press should be the window we want to popup
				}
			}

			// just return rather than doing any kind of UIE menu tree action or cleanup.
			// the window display is basically transparent to the UIE functionality.
			return(sRetStatus);

		case FSC_WINDOW_DISMISS:
			sRetStatus = UIF_DIA_RETRY_INPUT;
			if (pData->SEL.INPUT.uchLen < 1) {
				sRetStatus = LDT_SEQERR_ADR;
			}
			else {
				if (pData->SEL.INPUT.uchLen > 4 && pData->SEL.INPUT.aszKey[0] == _T('9')) {
					BlFwIfPopdownWindowGroup(pData->SEL.INPUT.aszKey+4);	//the sequence of digits previous to this button press should be the window we want to popup
				} else {
					BlFwIfPopdownWindowPrefix(pData->SEL.INPUT.aszKey);
				}
			}

			// just return rather than doing any kind of UIE menu tree action or cleanup.
			// the window dismiss is basically transparent to the UIE functionality.
			return(sRetStatus);

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
        UifRegDiaWork.auchFsc[0] = FSC_CLEAR;
        UifRegDiaWork.auchFsc[1] = 0;
        return (UIF_DIA_SUCCESS);

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

/****** end of file ******/
