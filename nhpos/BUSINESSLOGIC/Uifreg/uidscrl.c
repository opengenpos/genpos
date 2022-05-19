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
* Title       : Scroll Key Entry Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDSCRL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifDiaScroll() : Scroll Key Entry Dialog
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Mar-08-96:03.01.00: M.Ozawa   : R3.1
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
UIMENU FARCONST aChildRegDiaScroll[] = {{UifDiaScrollEntry, CID_DIASCROLLENTRY},{NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifDiaScroll(UIFDIADATA *pData)
*     Input:    nothing
*     Output:   pData - seat number buffer pointer
*     InOut:    nothing
*
** Return:      success = 0
*               cancel  = -2
*
** Description: Reg Seat Number Compulsory
*===========================================================================
*/
SHORT UifDiaScroll(UIFDIADATA *pData)
{
    KEYMSG  Data;
    USHORT  usCtrlDevice;
    SHORT   sRetStatus, sStatusSave;

    UieCreateDialog();
    usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER);   /* disable waiter lock */
    sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */
    UieInitMenu(aChildRegDiaScroll);
    for (;;) {
        UieGetMessage(&Data, 0x0000);
        sRetStatus = UieCheckSequence(&Data);
        if (sRetStatus == UIFREG_ABORT) {
            break;
        }
        else if (sRetStatus == UIF_DIA_SUCCESS) {
            *pData = UifRegDiaWork;
            sRetStatus = UIF_SUCCESS;
            break;
        }
    }
    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }
    UieCtrlDevice(usCtrlDevice);            /* enable waiter lock */
    UieDeleteDialog();
    return(sRetStatus);
}

/****** End of Source ******/
