/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg Reverse Key Module                         
* Category    : User Interface for Reg, NCR 2170 INT'L Hospitality Application         
* Program Name: UIFREVER.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifReverse() : Reg Reverse Key
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jul-19-93:01.00.12:K.You      : initial                                   
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

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UISEQ FARCONST aszSeqRegReverse[] = {FSC_REVERSE_PRINT, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifReverse(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Reverse Key Module
*===========================================================================
*/
SHORT UifReverse(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqRegReverse);      /* open waiter report key sequence */
        break;

    case UIM_INPUT:
        if (pData->SEL.INPUT.uchMajor == FSC_REVERSE_PRINT) {      /* a/c ? */ 
            if ((pData->SEL.INPUT.uchLen != 0) || (uchUifRegVoid != 0)) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            if (aszUifRegNumber[0] != 0) {      /* check number */
                return(LDT_SEQERR_ADR);
            }
            sRetStatus = ItemOtherEJReverse();          /* Other Modele */
            UifRegWorkClear();                          /* clear work area */
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

