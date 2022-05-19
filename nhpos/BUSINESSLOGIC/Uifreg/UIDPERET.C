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
* Title       : Reg Number of Person Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDPERET.C
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifDiaNoPersonEntry() : Reg Line Number Entry
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data   : Ver.Rev. :   Name     : Description
* May-12-92 : 00.00.01 : M.Suzuki   : initial                                   
* Jul-16-15 : 02.02.01 : R.Chambers : allow additional keys for Person Entry
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
** Synopsis:    SHORT UifDiaNoPersonEntry(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Number of Person Entry Module
*===========================================================================
*/
SHORT UifDiaNoPersonEntry(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_PRICE_ENTER:   // treat the Price Enter Key same as Number of Person key (FSC_NUM_PERSON) for UifDiaNoPersonEntry() to reduce number of buttons
        case FSC_P1:            // treat P1 key is same as Number of Person key (FSC_NUM_PERSON) for UifDiaNoPersonEntry() to reduce number of buttons
        case FSC_AC:            // treat the AC Key same as Number of Person key (FSC_NUM_PERSON) for UifDiaNoPersonEntry() to reduce number of buttons
        case FSC_NUM_TYPE:      // treat the #/Type Key same as Number of Person key (FSC_NUM_PERSON) for UifDiaNoPersonEntry() to reduce number of buttons
			pData->SEL.INPUT.uchMajor = FSC_NUM_PERSON;  // turn key press into Number of Person key for UifDiaNoPersonEntry() allowing consolidation of button types
        case FSC_NUM_PERSON:
            /* --- Saratoga, Nov/27/2000 --- */
            if ((sRetStatus = UifRegInputZeroCheck(pData, UIFREG_MAX_DIGIT3)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            if (aszUifRegNumber[0] != 0) {
                return(sRetStatus);
            }
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;   /* set fsc */
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;   
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;   /* set input data */
            if ((pData->SEL.INPUT.uchLen != 0) && (pData->SEL.INPUT.lData == 0L)) {
                UifRegDiaWork.fsInputStatus = INPUT_0_ONLY;
            }
            if (uchUifRegVoid != 0) {
                UifRegDiaWork.fbStatus |= VOID_MODIFIER;
            }
            /* --- Saratoga, Nov/27/2000 --- */
            return(UIF_DIA_SUCCESS);

        case FSC_P5:                // P5 key is same as Cancel key (FSC_CANCEL) as used in several places so be consistent
			pData->SEL.INPUT.uchMajor = FSC_CANCEL;  // turn key press into Cancel for UifDiaNoPersonEntry() allowing consolidation of button types
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

