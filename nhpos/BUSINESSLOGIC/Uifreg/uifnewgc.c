/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg GC Number Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFNEWGC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifNewKeySeqGC() : Reg GC Number Entry
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Aug-03-98:03.03.00: hrkato    : V3.3 (Split & Multi Check)
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
UISEQ FARCONST aszSeqRegNewKeySeqGCEntry[] = {FSC_GUEST_CHK_NO, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifNewKeySeqGC(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Split & Multi Check
*===========================================================================
*/
SHORT   UifNewKeySeqGC(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegNewKeySeqGCEntry);
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_GUEST_CHK_NO) {
            if ((sRetStatus = UifRegInputCheck(pData,UIFREG_MAX_DIGIT6)) != SUCCESS) {
                return(sRetStatus);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {
                return(sRetStatus);
            }

			sRetStatus = ItemSalesOrderDeclaration(UifRegData.regsales,0, CLASS_ORDER_DEC_NEW);
			if(sRetStatus != SUCCESS)
			{
				return sRetStatus;
			}

            UifRegData.regtransopen.uchMajorClass = CLASS_UIFREGTRANSOPEN;
            UifRegData.regtransopen.uchMinorClass = CLASS_UINEWKEYSEQGC;
            UifRegData.regtransopen.ulAmountTransOpen = (ULONG)pData->SEL.INPUT.lData;
            sRetStatus = ItemTransOpen(&UifRegData.regtransopen);
            UifRegWorkClear();
            if (sRetStatus == UIF_CANCEL) {
                UieReject();
                break;
            }
            if (sRetStatus == LDT_TAKETL_ADR) {         /* buffer full ? */
                flUifRegStatus |= UIFREG_BUFFERFULL;    /* set buffer full status */
                UieAccept();                            /* send accepted to parent */
                return(sRetStatus);
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
