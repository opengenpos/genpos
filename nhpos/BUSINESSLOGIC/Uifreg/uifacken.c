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
* Title       : Reg Add Check Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFACKEN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifAddCheckEntry() : Reg Add Check Entry
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial                                   
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
#include <item.h>
#include <paraequ.h>
#include <uireg.h>
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegAddCheckEntry[] = {FSC_ADD_CHECK, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifAddCheckEntry(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Add Check Entry Module
*===========================================================================
*/
SHORT UifAddCheckEntry(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegAddCheckEntry);    /* open add check entry key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_ADD_CHECK) {   /* add check ? */
            if (pData->SEL.INPUT.uchDec != 0xFF) {          /* input decimal point ? */
                return(LDT_KEYOVER_ADR);
            }
            if (pData->SEL.INPUT.uchLen > UIFREG_MAX_AMT_DIGITS) {  /* input digit over ? */
                return(LDT_KEYOVER_ADR);
            }
            if (aszUifRegNumber[0] != 0) {      /* input number ? */
                return(LDT_SEQERR_ADR);
            }
            if ((pData->SEL.INPUT.uchLen != 0) && (pData->SEL.INPUT.lData == 0L)) {
                /* if digit '0' input was entered then set '0' input status */
                UifRegData.regtransopen.fbInputStatus |= INPUT_0_ONLY;
            }
            if (uchUifRegVoid != 0) {
				/* set item void bit if VOID key has been pressed */
                UifRegData.regtransopen.fbModifierTransOpen |= VOID_MODIFIER;
            }
            UifRegData.regtransopen.uchMajorClass = CLASS_UIFREGTRANSOPEN;  /* set add check class */
            if (pData->SEL.INPUT.uchMinor == 1) {
                UifRegData.regtransopen.uchMinorClass = CLASS_UIADDCHECK1;
            } else if (pData->SEL.INPUT.uchMinor == 2) {
                UifRegData.regtransopen.uchMinorClass = CLASS_UIADDCHECK2;
            } else if (pData->SEL.INPUT.uchMinor == 3) {
                UifRegData.regtransopen.uchMinorClass = CLASS_UIADDCHECK3;
            } else { 
                return(LDT_PROHBT_ADR);
            }
            UifRegData.regtransopen.ulAmountTransOpen = (ULONG)pData->SEL.INPUT.lData;  /* set input amount */
            sRetStatus = ItemTransOpen(&UifRegData.regtransopen);   /* Transaction Open Modele */
            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
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

