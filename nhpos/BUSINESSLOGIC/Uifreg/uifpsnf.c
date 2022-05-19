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
* Title       : Reg GC Information Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFPSNF.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           UifNoPersonFist() : No of person first entry at cashier operation
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Nov-25-92:00.00.01:   hkato   : initial                                   
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
UISEQ FARCONST aszSeqRegNoPersonFirst[] = {FSC_NUM_PERSON, 0};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT   UifNoPersonFirst( KEYMSG *pData )
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: No of person entry at cashier operation
*===========================================================================
*/
SHORT   UifNoPersonFirst( KEYMSG *pData )
{
    SHORT   sRetStatus;
    SHORT   sStatusSave;

    switch (pData->uchMsg) {
    case UIM_INIT:                                                  
        UieOpenSequence( aszSeqRegNoPersonFirst );                      /* open No of person information key sequence */
        break;                                  
                                                            
    case UIM_INPUT:
        sStatusSave = ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        if ( pData->SEL.INPUT.uchMajor == FSC_NUM_PERSON ) {            /* number of person ? */
            /* --- Saratoga, Nov/27/2000 --- */
            if ((sRetStatus = UifRegInputZeroCheck(pData, UIFREG_MAX_DIGIT3)) != SUCCESS) {  /* check input data */
                return(sRetStatus);
            }
            if (aszUifRegNumber[0] != 0) {
                return(sRetStatus);
            }
            UifRegData.regmodifier.uchMajorClass = CLASS_UIFREGMODIFIER;  /* set modifier major class */
            UifRegData.regmodifier.uchMinorClass = CLASS_UINOPERSON;    /* set no of person minor class */
            if ((pData->SEL.INPUT.uchLen != 0) && (pData->SEL.INPUT.lData == 0L)) {
                UifRegData.regmodifier.fbInputStatus = INPUT_0_ONLY;
            }
            UifRegData.regmodifier.usNoPerson = (USHORT)pData->SEL.INPUT.lData;  /* set no of person */
            if (uchUifRegVoid != 0) {
                UifRegData.regmodifier.fbModifier |= VOID_MODIFIER;
            }
            /* --- Saratoga, Nov/27/2000 --- */

            sRetStatus = ItemModPerson(&UifRegData.regmodifier);         

            UifRegWorkClear();                                          /* clear work area */
            if (sRetStatus == UIF_CANCEL) {                             /* cancel ? */
                if (sStatusSave) {                  /* if previous status is enable */
                    ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
                }
                UieReject();                                            /* send reject to parent */
                break;
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

