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
* Title       : Clear key Compulsory Module                                     
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDCLR.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifDiaPrice() : Price Compulsory Dialog
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Nov-09-99:        :M.Ozawa    : initial                                   
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
#include "item.h"
#include <uireg.h>
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifDiaClear(UIFDIADATA *pData)
*     Input:    nothing
*     Output:   pData - line number buffer pointer
*     InOut:    nothing
*
** Return:      success = 0
*               cancel  = -2
*
** Description: Reg Clear key Compulsory
*===========================================================================
*/
SHORT   UifDiaClear(VOID)
{
    KEYMSG  Data;               /* key message data */
    USHORT  usModeStatus;       /* mode lock enable/disable bit save area */
    USHORT  usCtrlDevice;       /* control device save area       */
    SHORT   sRetStatus;         /* return status save area */
    SHORT   sStatusSave;

    /* --- create a dialog --- */
    UieCreateDialog();

    /* --- save and disable mode change --- */
    usModeStatus = UieModeChange(UIE_DISABLE);

    /* --- disable scanner device --- */
    usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER);
    sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */

    /* --- prohibit ten key entry --- */
    UieEchoBack(UIE_ECHO_ROW0_REG, 0);

    for (;;) {

        /* --- wait key entry --- */
        UieGetMessage(&Data, 0x0000);

        switch (Data.uchMsg) {

        /* --- key --- */
        case UIM_INPUT:

            /* --- if clear key --- */
            if (Data.SEL.INPUT.uchMajor == FSC_CLEAR) {

                /* --- set return status --- */
                sRetStatus = UIF_SUCCESS;

                /* --- exit --- */
                break;
            }

            /* --- if CANCEL key --- */
            else if (Data.SEL.INPUT.uchMajor == FSC_CANCEL) {

                /* --- set return status --- */
                sRetStatus = UIF_CANCEL;

                /* --- exit --- */
                break;
            }


            /* --- if Receipt Feed key or Journal Feed key --- */
            else if (Data.SEL.INPUT.uchMajor == FSC_RECEIPT_FEED
                        || Data.SEL.INPUT.uchMajor == FSC_JOURNAL_FEED) {

                /* --- execute UI Reg Def. Proccess --- */
                UifRegDefProc(&Data);

            }
            else if (Data.SEL.INPUT.uchMajor == FSC_RESET_LOG	/* avoid key sequence error by PCIF function, 11/12/01 */
                        || Data.SEL.INPUT.uchMajor == FSC_POWER_DOWN) {
                UifRegDefProc(&Data);

            }

            /* --- else --- */
            else {

                /* --- display sequence error message number --- */
                UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);
            }

            /* --- continue for(;;) loop --- */
            continue;

        case UIM_ERROR:

            /* --- display sequence error message number --- */
            UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);

        case UIM_REDISPLAY:

            /* --- execute UI Eng. Def. process --- */
            UieDefProc(&Data);

        default:

            /* --- continue for(;;) loop --- */
            continue;

        }

        /* --- exit --- */
        break;
    }
    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }

    /* --- allow ten key entry --- */
    UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);

    /* --- restore scanner device enable/disable condition --- */
    UieCtrlDevice(usCtrlDevice);

    /* --- restore mode change enable/disable bit --- */
    UieModeChange(usModeStatus);

    /* --- delete a dialog --- */
    UieDeleteDialog();

    /* --- return status --- */
    return(sRetStatus);

}

/* --- End of Source File --- */