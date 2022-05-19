/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Order Number Entry 
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDTBL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               UifDiaOEP() : Compulsory Order Number Entry
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Feb-28-95:03.00.00: hkato     : R3.0
* Apr-02-96:03.01.02: M.Ozawa   : modify sequence error control for 2x20 oep
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
#include <item.h>
#include <uireg.h>
#include "uiregloc.h"

/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UIMENU FARCONST aChildRegDiaOEPKey[] = {{UifDiaOEPKeyEntry, CID_DIAOEPKEYENTRY},{NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifDiaOEPKey(UIFDIADATA *pData)
*     Input:    nothing
*     Output:   pData - table number buffer pointer
*     InOut:    nothing
*
** Return:      success = 0
*               cancel  = -2
*
** Description: Compulsory Order Number Entry
*===========================================================================
*/
SHORT   UifDiaOEPKey(UIFDIADATA *pData)
{
    KEYMSG  Data;
    USHORT  usModeStatus, usCtrlDevice;
    SHORT   sRetStatus, sStatusSave;

    UieCreateDialog();                              /* create dialog */
    usModeStatus = UieModeChange(UIE_DISABLE);      /* disable mode change */
    usCtrlDevice = UieCtrlDevice(UIE_DIS_WAITER);   /* disable waiter lock */
    /* usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER);   / disable waiter lock */
    sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */
    /* usMessage    = UieSetMessage(2);                / 2 digits */
    UieInitMenu(aChildRegDiaOEPKey);                   /* open table number entry */
    for (;;) {
        UieGetMessage(&Data, 0x0000);                       /* wait key entry */
        sRetStatus = UieCheckSequence(&Data);       /* check key */
        if (sRetStatus == UIFREG_ABORT) {           /* cancel ? */
            break;
        }
        else if (sRetStatus == UIF_DIA_SUCCESS) {        /* success ? */
            *pData = UifRegDiaWork;
            sRetStatus = UIF_SUCCESS;
            break;
        }
        else if (sRetStatus == LDT_SEQERR_ADR) {         /* redisplay after sequence error */
            pData->auchFsc[0] = FSC_CLEAR;
            pData->auchFsc[1] = 0;
            sRetStatus = UIF_SUCCESS;
            break;
        }
    }
    /* UieSetMessage(usMessage);                       / restore */
    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }
    UieCtrlDevice(usCtrlDevice);            /* enable waiter lock */
    UieModeChange(usModeStatus);                    /* restore mode change status */
    UieDeleteDialog();                              /* delete dialog */
    return(sRetStatus);
}

/****** end of file ******/
