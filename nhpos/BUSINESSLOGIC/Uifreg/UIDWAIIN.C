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
* Title       : Insert Waiter Lock Compulsory Module                         
* Category    : User Interface for Reg, NCR 2170 INT'L Hospitality Application         
* Program Name: UIDWAIIN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifDiaInsertLock() : Insert Waiter Lock Compulsory Dialog
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
#include <regstrct.h>
#include <item.h>
#include <uireg.h>
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UIMENU FARCONST aChildRegDiaInsertLock[] = {{UifDiaInsertLockEntry, CID_DIAINSERTLOCKENTRY},{NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifDiaInsertLock(UIFDIADATA *pData)
*     Input:    nothing
*     Output:   pData - waiter number buffer pointer
*     InOut:    nothing
*
** Return:      success = 0
*               cancel  = -2
*
** Description: Reg Insert Waiter Lock Compulsory
*===========================================================================
*/
SHORT UifDiaInsertLock(UIFDIADATA *pData)
{
    KEYMSG  Data;
    USHORT  usModeStatus;
    SHORT   sRetStatus, sStatusSave;

    UieCreateDialog();                      /* create dialog */
    usModeStatus = UieModeChange(UIE_DISABLE);      /* disable mode change */
    sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */
    UieInitMenu(aChildRegDiaInsertLock);        /* open waiter number entry */
    for (;;) {
        UieGetMessage(&Data, 0x0000);                   /* wait key entry */
        sRetStatus = UieCheckSequence(&Data);   /* check key */
        if (sRetStatus == UIFREG_ABORT) {       /* cancel ? */
            break;
        }
        else if (sRetStatus == UIF_DIA_SUCCESS) {    /* success ? */
            *pData = UifRegDiaWork;    /* set cahier number */
            sRetStatus = UIF_SUCCESS;       /* set return status */
            break;
        }
    }
    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }
    UieModeChange(usModeStatus);            /* restore mode change status */
    UieDeleteDialog();                      /* delete dialog */
    return(sRetStatus);
}

