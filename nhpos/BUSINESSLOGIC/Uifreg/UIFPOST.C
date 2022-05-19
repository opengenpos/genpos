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
* Title       : Reg Post Receipt Module                         
* Category    : User Interface for Reg, NCR 2170 INT'L Hospitality Application         
* Program Name: UIFPOST.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPostRecept() : Reg Post Receipt
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jul-01-93:01.00.12:K.You      : initial                                   
* Apr-21-95:03.00.00:hkato      : R3.0
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
#include <item.h>
#include <paraequ.h>
#include <uireg.h>
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegPostReceipt[] = {FSC_PRT_DEMAND, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifPostReceipt(KEYMSG *pData)
*
*     Input:    pData - key message pointer
*     Output:   nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Reg Post Receipt/Parking Module,        R3.0
*===========================================================================
*/

SHORT UifPostReceipt(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegPostReceipt);  /* open post receipt key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_PRT_DEMAND) {  /* post receipt ? */ 
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if ( ! ((pData->SEL.INPUT.uchMinor == FSC_POSTRECEIPT) ||
                    (pData->SEL.INPUT.uchMinor == FSC_PARKING) ||
					(pData->SEL.INPUT.uchMinor == FSC_GIFT_RECEIPT) ||
					(pData->SEL.INPUT.uchMinor == FSC_PODREPRINT))) {
				//SR 525, Gift Receipt functionality JHHJ 9-01-05
                return(LDT_SEQERR_ADR);
            } 
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {  /* check item void,number */
                return(sRetStatus);
            }
            UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;    /* set post receipt class */
            if (pData->SEL.INPUT.uchMinor == FSC_POSTRECEIPT) {     /* post receipt ? */ 
                UifRegData.regmisc.uchMinorClass = CLASS_UIPOSTRECEIPT;
            } else if(pData->SEL.INPUT.uchMinor == FSC_GIFT_RECEIPT)
			{
				//SR 525, Gift Receipt functionality JHHJ 9-01-05
				UifRegData.regmisc.uchMinorClass = CLASS_UIGIFTRECEIPT;
            } else if(pData->SEL.INPUT.uchMinor == FSC_PODREPRINT)
			{
				//Print on Demand functionality
				UifRegData.regmisc.uchMinorClass = CLASS_UIPODREPRINT;
			}else{
                UifRegData.regmisc.uchMinorClass = CLASS_UIPARKING;
            }

            sRetStatus = ItemMiscPostReceipt(&UifRegData.regmisc);
            UifRegWorkClear();                          /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                return(SUCCESS);
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                return(sRetStatus);
            }
            UieAccept();                                /* send accepted to parent */
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/
