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
* Title       : Reg Seat Number Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDSEAET.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifDiaSeatNoEntry() : Reg Seat Number Entry
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Nov-17-95:03.01.00: hkato     : R3.1
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
** Synopsis:    SHORT UifDiaSeatNoEntry(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Seat Number Entry Module
*===========================================================================
*/
SHORT UifDiaSeatNoEntry(KEYMSG *pData)
{

    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_P1:            // treat P1 key is same as Seat Number key (FSC_SEAT) for UifDiaSeatNoEntry() to reduce number of buttons
        case FSC_PRICE_ENTER:   // treat the Price Enter Key same as Seat Number key (FSC_SEAT) for UifDiaSeatNoEntry() to reduce number of buttons
        case FSC_AC:            // treat the AC Key same as Seat Number key (FSC_SEAT) for UifDiaSeatNoEntry() to reduce number of buttons
        case FSC_NUM_TYPE:      // treat the #/Type Key same as Seat Number key (FSC_SEAT) for UifDiaSeatNoEntry() to reduce number of buttons
			pData->SEL.INPUT.uchMajor = FSC_SEAT;  // turn key press into Seat Number key for UifDiaSeatNoEntry() allowing consolidation of button types
        case FSC_SEAT:
            if (pData->SEL.INPUT.uchLen > UIFREG_MAX_DIGIT1/*UIFREG_MAX_DIGIT2*/) { //SR206
                return(LDT_SEQERR_ADR);
            }
            UifRegDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifRegDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
            UifRegDiaWork.ulData = (ULONG)pData->SEL.INPUT.lData;
            if (pData->SEL.INPUT.uchLen == UIFREG_MAX_DIGIT1 &&     /* V3.3 */
                pData->SEL.INPUT.lData == 0L) {
                UifRegDiaWork.aszMnemonics[0] = '0';
            }
            return(UIF_DIA_SUCCESS);

        case FSC_P5:                // P5 key is same as Cancel key (FSC_CANCEL) as used in several places so be consistent
			pData->SEL.INPUT.uchMajor = FSC_CANCEL;  // turn key press into Cancel for UifDiaCPEntry4() allowing consolidation of button types
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
