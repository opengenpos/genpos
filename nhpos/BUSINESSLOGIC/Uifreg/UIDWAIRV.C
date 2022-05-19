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
* Title       : Remove Waiter Lock Compulsory Module                         
* Category    : User Interface for Reg, NCR 2170 INT'L Hospitality Application         
* Program Name: UIDWAIRV.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifDiaRemoveLock() : Remove Waiter Lock Compulsory Dialog
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Dec-15-92:00.00.01:M.Suzuki   : initial                                   
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
#include <paraequ.h>
#include <para.h>
#include <display.h>
#include <item.h>
#include <uireg.h>
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID UifDiaRemoveLock(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Reg Remove Waiter Lock Compulsory
*===========================================================================
*/
VOID UifDiaRemoveLock(VOID)
{
    KEYMSG  Data;
    USHORT  usModeStatus;
    REGDISPMSG  UifDisp;
    SHORT   sStatusSave;

    UieCreateDialog();                      /* create dialog */
    usModeStatus = UieModeChange(UIE_DISABLE);      /* disable mode change */
    sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */
    UieEchoBack(UIE_ECHO_ROW0_REG, 0);      /* set echo back */
    for (;;) {
        UieGetMessage(&Data, 0x0000);               /* wait key entry */
        if (Data.uchMsg == UIM_INPUT) {     /* key input ? */
            if (Data.SEL.INPUT.uchMajor == FSC_WAITER_PEN) { /* waiter lock ? */
                if (Data.SEL.INPUT.uchMinor == 0) {         /* remove waiter lock ? */
                    break;
                }
            } else if (Data.SEL.INPUT.uchMajor != FSC_CLEAR) {     /* clear key ? */
                UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);    /* error display */
            }
        } else if (Data.uchMsg == UIM_ERROR) {      /* error ? */
            UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);    /* error display */
        }
        memset(&UifDisp, 0, sizeof(REGDISPMSG));       
        UifDisp.uchMajorClass = CLASS_UIFREGDISP;   /* set display class */
        UifDisp.uchMinorClass = CLASS_UIFDISP_REMOVEKEY;   
        DispWrite(&UifDisp);                        /* display */
    }
    UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);  /* set echo back */
    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }
    UieModeChange(usModeStatus);            /* restore mode change status */
    UieDeleteDialog();                      /* delete dialog */
}

