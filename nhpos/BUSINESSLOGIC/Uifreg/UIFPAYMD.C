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
* Title       : Reg Payment Modifier Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFFORIN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPayModifier() : Reg Payment Modifier
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Sep-21-92:00.00.01:M.Suzuki   : initial                                   
* Jun-21-93:01.00.12:K.You      : add direct menu shift key.                                   
* May-19-95:03.00.00:hkato      : R3.0
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
#include "ecr.h"
#include "uie.h"
#include "fsc.h"
#include "regstrct.h"
#include "item.h"
#include "paraequ.h"
#include "uireg.h"
#include "uiregloc.h"
                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegPayModifier[] = {FSC_TAX_MODIF, 0};     /* R3.0 */

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifPayModifier(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Payment Modifier Module
*===========================================================================
*/
SHORT   UifPayModifier(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegPayModifier);        /* open Payment Modifier key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_TAX_MODIF) {  /* tax modifier ?,   R3.0 */
            return(LDT_SEQERR_ADR);
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }

    return (SUCCESS);
}

/* --- End of Source File --- */