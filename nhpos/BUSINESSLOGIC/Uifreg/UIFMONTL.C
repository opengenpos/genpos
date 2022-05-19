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
* Program Name: UIFMONTL.C
* Author      : hkato, Project#1, GTP&S
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
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

#include "ecr.h"
#include "uie.h"
#include "fsc.h"
#include "regstrct.h"
#include "item.h"
#include "paraequ.h"
#include "uireg.h"
#include "display.h"
#include "uiregloc.h"


/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegMoneyTend[] = { FSC_TENDER,
                                        FSC_FOREIGN,
                                        FSC_QTY,
                                        0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifMoneyTend(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      SUCCESS
*               LDT_????_ADR    : Lead thru message number
*
** Description: Money declaration tender entry
*===========================================================================
*/
SHORT UifMoneyTend(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:

        /* --- add money tender entry key sequence --- */
        UieOpenSequence(aszSeqRegMoneyTend);

        /* --- exit, return SUCCESS status --- */
        break;

    case UIM_INPUT:

        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_TENDER:
        case FSC_FOREIGN:

            /* --- check EXT FSC key code --- */
            if (pData->SEL.INPUT.uchMinor == 0
			/* temporary hardcoded 11, change to MAINT_TEND_MAX when implementing full
			tender database change JHHJ 3-16-04*/
                    || pData->SEL.INPUT.uchMinor > FSC_TEND_MAX) {
                return(LDT_PROHBT_ADR);
            }

            /* --- check indexed data --- */
            if (pData->SEL.INPUT.uchLen > UIFREG_MAX_AMT_DIGITS) {
                return(LDT_KEYOVER_ADR);
            }
            if (pData->SEL.INPUT.lData == 0L) {

                /* --- return sequence error message number --- */
                return(LDT_SEQERR_ADR);
            }

            /* --- setup "Other" item data class --- */
            UifRegData.regmisc.uchMajorClass = CLASS_UIFREGMISC;
            UifRegData.regmisc.uchMinorClass = CLASS_UIFMONEY;
            if (pData->SEL.INPUT.uchMajor == FSC_TENDER) {
                UifRegData.regmisc.uchTendMinor
                    = (UCHAR)(CLASS_UITENDER1 + pData->SEL.INPUT.uchMinor - 1);
            } else {
                UifRegData.regmisc.uchTendMinor
                    = (UCHAR)(CLASS_UIFOREIGN1 + pData->SEL.INPUT.uchMinor - 1);
            }
            UifRegData.regmisc.lAmount = pData->SEL.INPUT.lData;
            if (UifRegData.regmisc.lQTY == 0L) {
                UifRegData.regmisc.lQTY = 1000L;
            }
            if (uchUifRegVoid != 0) {
                UifRegData.regmisc.fbModifier |= VOID_MODIFIER;
            }
            _tcsncpy(UifRegData.regmisc.aszNumber, aszUifRegNumber, NUM_NUMBER);
            //memcpy(UifRegData.regmisc.aszNumber, aszUifRegNumber, sizeof(aszUifRegNumber));  /* set number saratoga */
            /* memcpy(UifRegData.regmisc.aszNumber, aszUifRegNumber, NUM_NUMBER); */
             /* --- execute Item module --- */
            sRetStatus = ItemMiscMoney(&UifRegData.regmisc);

            /* --- clear UI Reg Work memory --- */
            UifRegWorkClear();

            if (sRetStatus == UIF_SUCCESS) {

                /* --- send accept to parent --- */
                UieAccept();

                /* --- exit, return SUCCESS status --- */
                break;
            }

            /* --- if item's return status is cancel --- */
            else if (sRetStatus == UIF_CANCEL) {

                /* --- send reject to parent --- */
                UieReject();

                /* --- exit, return SUCCESS status --- */
                break;
            }

            /* --- if item's status is lead thru message number --- */
            else {

                /* --- return lead thru message number --- */
                return(sRetStatus);
            }

        default:
            return(LDT_SEQERR_ADR);
        }

        break;

    default:
        return(UifRegDefProc(pData));
    }

    /* --- return SUCCESS status --- */
    return (SUCCESS);
}

/* --- End of Source File --- */