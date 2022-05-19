/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Money declaration-in
* Category    : User Interface for Reg, NCR 2170 GP 2.0 Application
* Program Name: UIFMONIN.C
* Author      : hkato, Project#1, GTP&S
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
* --------------------------------------------------------------------------
* Update Histories
*    Data  :Ver.Rev.:   Name    : Description
* Nov-25-99:01.00.00: hrkato    : Saratoga
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
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <uic.h>
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
UISEQ FARCONST aszSeqRegMoneyIn[] = { FSC_MONEY,
                                         0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifMoneyIn(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      SUCCESS
*               LDT_????_ADR    : Lead thru message number
*
** Description: Money declaration-in
*===========================================================================
*/
SHORT UifMoneyIn(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:

        /* --- add money declaration-in in key sequence --- */
        UieOpenSequence(aszSeqRegMoneyIn);

        /* --- exit, return SUCCESS status --- */
        break;

    case UIM_INPUT:

        /* --- check entered data --- */
        if (UifRegData.regmisc.fbModifier !=0
                || pData->SEL.INPUT.uchLen != 0) {

            /* --- return sequence error message number --- */
            return(LDT_SEQERR_ADR);
        }

        /* --- setup "Money-in" data class --- */
        UifRegData.regother.uchMajorClass = CLASS_UIFREGOTHER;
        UifRegData.regother.uchMinorClass = CLASS_UIFMONEY_IN;

        /* --- execute item module --- */
        sRetStatus = ItemMiscMoneyIn();

        /* --- clear UI Reg work --- */
        UifRegWorkClear();

        /* --- if item's return status is successful --- */
        if (sRetStatus == UIF_SUCCESS ) {

            /* --- send accept to parent --- */
            UieAccept();
        }

        /* --- if item's status is CANCEL --- */
        else if (sRetStatus == UIF_CANCEL) {

            /* --- send reject to parent --- */
            UieReject();
        }

        /* --- if item's status is lead thru message no. --- */
        else {

            /* --- return Lead thru message number --- */
            return(sRetStatus);
        }

        /* --- exit, return SUCCESS --- */
        break;

    default:
        return(UifRegDefProc(pData));
    }

    /* --- return SUCCESS status --- */
    return (SUCCESS);
}

/* --- End of Source File --- */