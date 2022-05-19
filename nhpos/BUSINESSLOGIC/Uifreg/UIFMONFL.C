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
* Title       : Money declaration-out
* Category    : User Interface for Reg, NCR 2170 GP 2.0 Application
* Program Name: UIFMONFL.C
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
#include <fsc.h>
#include <regstrct.h>
#include <item.h>
#include <paraequ.h>
#include <uireg.h>
#include "uiregloc.h"
#include "BlFWif.h"


/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegMoneyOut[] = { FSC_MONEY,
                                          0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifMoneyOut(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      SUCCESS
*               LDT_????_ADR    : Lead thru message number
*
** Description: Money declaration-out
*===========================================================================
*/
SHORT UifMoneyOut(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        /* --- add money declaration-out key sequence --- */
        UieOpenSequence(aszSeqRegMoneyOut);

        /* --- exit, return SUCCESS status --- */
        break;

    case UIM_INPUT:
        /* --- check modifier key and entered data --- */
        if (UifRegData.regmisc.lQTY != 0L ||
            UifRegData.regmisc.fbModifier != 0) {
            return(LDT_SEQERR_ADR);
        }
        if (pData->SEL.INPUT.lData != 0L) {
            return(LDT_SEQERR_ADR);
        }
        UifRegData.regmisc.uchMajorClass = CLASS_UIFREGOTHER;
        UifRegData.regmisc.uchMinorClass = CLASS_UIFMONEY_OUT;

        /* --- execute item module --- */
        sRetStatus = ItemMiscMoneyOut();

        /* --- clear UI Reg work --- */
        UifRegWorkClear();

        if (sRetStatus == UIF_CAS_SIGNOUT) {           /* cashier sign out ? */
            flUifRegStatus |= UIFREG_CASHIERSIGNOUT;   /* set cashier sign out status */
        } else if (sRetStatus != UIF_FINALIZE) {       /* error ? */
            return(sRetStatus);
        }
        UieAccept();                            
		UifRegClearDisplay(0);					/* clear work area */
		BlFwIfDefaultUserScreen();				/* call function to bring up default window(s) */
        break;

    default:
        return(UifRegDefProc(pData));
    }

    /* --- return SUCCESS status --- */
    return (SUCCESS);
}

/* --- End of Source File --- */