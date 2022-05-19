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
* Title       : Reg Scroll Key Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDSCLET.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifDiaScrollEntry() : Reg Scroll Key Entry
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Mar-08-96:03.01.00: M.Ozawa   : R3.1
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
** Synopsis:    SHORT UifDiaScrollEntry(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Scroll Key Entry Module
*===========================================================================
*/
SHORT UifDiaScrollEntry(KEYMSG *pData)
{

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_SCROLL_UP:
        case FSC_SCROLL_DOWN:
        case FSC_RIGHT_ARROW:
        case FSC_LEFT_ARROW:
        case FSC_LEFT_DISPLAY:
            if (pData->SEL.INPUT.uchLen != 0) {
                return(LDT_SEQERR_ADR);
            }
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
            return(UIF_DIA_SUCCESS);


        case FSC_CANCEL:
            if (pData->SEL.INPUT.uchLen != 0) {
                return(LDT_SEQERR_ADR);
            }
            return(UIFREG_ABORT);

        case FSC_AUTO_SIGN_OUT:
            return(UIFREG_ABORT);

        case FSC_RECEIPT_FEED:
        case FSC_JOURNAL_FEED:
        case FSC_RESET_LOG:							/* avoid key sequence error by PCIF function, 11/12/01 */
        case FSC_POWER_DOWN:
            return(UifRegDefProc(pData));

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

/****** End of Source ******/
