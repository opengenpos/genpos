/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg WIC Transaction Module
* Category    : User Interface for Reg, NCR 2170 GP US Application
* Program Name: UIFWICTR.C
* Author      : M.Sugiyama, Project#1, GTP&S
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               UifWICTran() : Reg WIC Transaction
*
* --------------------------------------------------------------------------
* Update Histories
*    Data  :Ver.Rev.:   Name    : Description
* 2-24-'94 :01.01.00: m.sugiyama: Added this source code to provide the Rel 1.1
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
/* --- WIC Transaction Key Sequence --- */
UISEQ FARCONST aszSeqRegWICTran[] = {FSC_WIC_TRN, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifWICTran(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      SUCCESS
*               LDT_????_ADR    : Lead thru message number
*
** Description: Reg WIC Transaction Module
*===========================================================================
*/
SHORT UifWICTran(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegWICTran);
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_WIC_TRN) {
            if (pData->SEL.INPUT.uchLen != 0) {
                return(LDT_SEQERR_ADR);
            }
            if ((sRetStatus = UifRegVoidNoCheck()) != SUCCESS) {
                return(sRetStatus);
            }
            sRetStatus = ItemModWIC();
            UifRegWorkClear();
            if (sRetStatus == UIF_CANCEL) {
                UieReject();
                break;
            }
            if (sRetStatus != UIF_SUCCESS) {
                return(sRetStatus);
            }
            UieAccept();
            break;
        }
        /* break */

    default:                                        
        return(UifRegDefProc(pData));
    }

    return (SUCCESS);
}

/* --- End of Source File --- */