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
* Title       : Guest Check Number Compulsory Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDGCN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifDiaGuestNo() : Guest Check Number Compulsory Dialog
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-12-92:00.00.01:M.Suzuki   : initial                                   
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
#include "fsc.h"
#include	"..\Sa\UIE\uiedev.h"
                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UIMENU FARCONST aChildRegDiaGuestNo[] = {{UifDiaGuestNoEntry, CID_DIAGUESTNOENTRY},{NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifDiaGuestNo(UIFDIADATA *pData)
*     Input:    nothing
*     Output:   pData - guest check number buffer pointer
*     InOut:    nothing
*
** Return:      success = 0
*               cancel  = -2
*
** Description: Reg Guest Check Number Compulsory.
*
*               See also function TrnDisplayFetchGuestCheck() which uses the OEP
*               functionality to display a list of open guest checks by the specified operator Id
*               and returns the Guest Check number selected or a return code indicating no selection made.
*===========================================================================
*/
SHORT UifDiaGuestNo(UIFDIADATA *pData)
{
    KEYMSG  Data;
    USHORT  usModeStatus, usCtrlDevice;
    SHORT   sRetStatus, sStatusSave;
	USHORT  usUieGetErrorActionFlags = UIEGETMESSAGE_FLAG_MACROPAUSE;

    UieCreateDialog();                              /* create dialog */
    usModeStatus = UieModeChange(UIE_DISABLE);      /* disable mode change */
    usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER);   /* disable waiter lock */
    sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */
    UieInitMenu(aChildRegDiaGuestNo);               /* open guset check number entry menu, UifDiaGuestNo() */
    for (;;) {
        UieGetMessage(&Data, usUieGetErrorActionFlags);       /* wait key entry, UifDiaGuestNo() */
        sRetStatus = UieCheckSequence(&Data);                      /* check key */
        if (sRetStatus == UIFREG_ABORT) {  /* cancel ? */
			if (usUieGetErrorActionFlags & UIEGETMESSAGE_FLAG_MACROPAUSE)  // clear ring buffer if cancel and we have not started a control string.
				UieEmptyRingBufClearMacro();
            break;
        }
        else if (sRetStatus == UIF_DIA_SUCCESS) {                  /* success ? */
            *pData = UifRegDiaWork;                                /* set cahier number */
            sRetStatus = UIF_SUCCESS;                              /* set return status */
			if (pData->auchFsc[0] == FSC_CLEAR || pData->auchFsc[0] == FSC_ERROR) {
				sRetStatus = UIFREG_ABORT;                         /* set return status */
				UieEmptyRingBufClearMacro();
			}
            break;
		} else if (sRetStatus == UIF_STRING_START) {
			usUieGetErrorActionFlags = 0;    // control string started by operator action so let it run.
		} else {
			usUieGetErrorActionFlags = UIEGETMESSAGE_FLAG_MACROPAUSE;
		}
    }
    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }
    UieCtrlDevice(usCtrlDevice);            /* enable waiter lock */
    UieModeChange(usModeStatus);            /* restore mode change status */
    UieDeleteDialog();                      /* delete dialog */
    return(sRetStatus);
}

