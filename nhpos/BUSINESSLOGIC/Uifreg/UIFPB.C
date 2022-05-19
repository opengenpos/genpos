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
* Title       : Reg PB Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFPB.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifPB() : Reg PB Entry
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  : Ver.Rev. :   Name      : Description
* May-11-92: 00.00.01 : M.Suzuki    : initial                                   
* Apr-22-15: 02.02.01 : R.Chambers  : changed fbModifier to fbModifierTransOpen so easier to find.
*                                   : changed ulAmount to ulAmountTransOpen so easier to find.
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
UISEQ FARCONST aszSeqRegPB[] = {FSC_PREVIOUS, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifPB(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Previous Balance Entry Module which is normally used with
*               Pre-Guest Check and using slip paper forms to document the transaction
*               data.
*===========================================================================
*/
SHORT UifPB(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegPB);   /* open PB entry key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_PREVIOUS) { /* Previous Balance ? */
            if (pData->SEL.INPUT.uchDec != 0xFF) {       /* input decimal point ? */
                return(LDT_KEYOVER_ADR);
            }
            if (pData->SEL.INPUT.uchLen > UIFREG_MAX_AMT_DIGITS) {  /* input digit over ? */
                return(LDT_KEYOVER_ADR);
            }
            if (aszUifRegNumber[0] != 0) {     /* input number ? */
                return(LDT_SEQERR_ADR);
            }
            if (uchUifRegVoid != 0) {
				/* set item void bit if VOID key has been pressed */
                UifRegData.regtransopen.fbModifierTransOpen |= VOID_MODIFIER;
            }
            if ((pData->SEL.INPUT.uchLen != 0) && (pData->SEL.INPUT.lData == 0L)) {
                /* if digit '0' input was entered then set '0' input status */
                UifRegData.regtransopen.fbInputStatus |= INPUT_0_ONLY;
            }
            UifRegData.regtransopen.ulAmountTransOpen = (ULONG)pData->SEL.INPUT.lData;  /* set Previous Balance amount */
            UifRegData.regtransopen.uchMajorClass = CLASS_UIFREGTRANSOPEN;  /* set reorder class */
            UifRegData.regtransopen.uchMinorClass = CLASS_UIREORDER;
            sRetStatus = ItemTransOpen(&UifRegData.regtransopen);   /* Transaction Open Modele */
            UifRegWorkClear();                              /* clear work area */
            if (sRetStatus == UIF_CANCEL) {             /* cancel ? */
                UieReject();                            /* send reject to parent */
                break;
            }
            if (sRetStatus != UIF_SUCCESS) {            /* error ? */
                return(sRetStatus);
            }
            UieAccept();                               /* send accepted to parent */
            break;
        }
        /* break */                                     /* not use */

    default:                                        
        return(UifRegDefProc(pData));                   /* default process */
    }
    return (SUCCESS);
}

