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
* Title       : Reg Line Number Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDLINET.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifDiaLineNoEntry() : Reg Line Number Entry
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
** Synopsis:    SHORT UifDiaLineNoEntry(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Line Number Entry Module
*===========================================================================
*/
SHORT UifDiaLineNoEntry(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_LINE_NO:
            if ((sRetStatus = UifRegInputCheckWOAmount(pData, UIFREG_MAX_DIGIT2)) != SUCCESS) { /* input check */
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

