/*

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation

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
#include	<tchar.h>
#include <string.h>
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
SHORT   UifDiaOEPKeyEntry(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        
        case FSC_PLU:   /* 2172 */

			NHPOS_ASSERT(sizeof(UifRegDiaWork.aszMnemonics) >= pData->SEL.INPUT.uchLen);

            if ((sRetStatus = UifRegInputCheck(pData, UIFREG_MAX_DIGIT13)) != SUCCESS) {  /* input data check */
                return(sRetStatus);
            }
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);
            //memcpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);
            return (UIF_DIA_SUCCESS);
            break;

        /* --- Scanner, 2172 --- */
        case FSC_SCANNER:

			NHPOS_ASSERT( sizeof(UifRegDiaWork.aszMnemonics) >= pData->SEL.INPUT.DEV.SCA.uchStrLen);  //JHHJ NHPOS ASSERT


            /* --- if indexed data is entered --- */
            if (pData->SEL.INPUT.uchLen != 0) {

                /* --- return sequence error message number --- */
                return(LDT_SEQERR_ADR);
            }

            /* --- check scanned data length --- */
            if (pData->SEL.INPUT.DEV.SCA.uchStrLen > UIFREG_MAX_DIGIT13+1/*RPH RSS14 ???*/) {

                /* --- return lead thru message number --- */
                return(LDT_KEYOVER_ADR);
            }
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
            _tcsncpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.DEV.SCA.auchStr, pData->SEL.INPUT.DEV.SCA.uchStrLen);
            //memcpy(UifRegDiaWork.aszMnemonics, pData->SEL.INPUT.DEV.SCA.auchStr, pData->SEL.INPUT.DEV.SCA.uchStrLen);
            return (UIF_DIA_SUCCESS);
            break;
            

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
