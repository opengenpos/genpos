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
* Title       : Cashier Number Compulsory Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDCAS.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifDiaCashier() : Cashier Number Compulsory Dialog
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-05-92:00.00.01:M.Suzuki   : initial                                   
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
UIMENU FARCONST aChildRegDiaCashier[] = {{UifDiaCashierEntry, CID_DIACASHIERENTRY},{NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifDiaCashier(UIFDIADATA *pData)
*     Input:    nothing
*     Output:   pData - cashier number buffer pointer
*     InOut:    nothing
*
** Return:      success = 0
*               cancel  = -2
*
** Description: Reg Cashier Number Compulsory
*===========================================================================
*/
SHORT UifDiaGetNumericEntry(UIFDIADATA *pData, USHORT usEcho, USHORT usNoDigits, UIMENU *aUieMenu)
{
    KEYMSG  Data;
    USHORT  usModeStatus;
    SHORT   sRetStatus, sStatusSave;

    UieCreateDialog();                      /* create dialog */
    usModeStatus = UieModeChange(UIE_DISABLE);      /* disable mode change */
    sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */
    if (usEcho == UIFREG_NO_ECHO) {         /* echo back as if server entry, V3.3 */
        UieEchoBack(UIE_ECHO_NO, usNoDigits); /* no echo back */
    }

	if (aUieMenu)
		UieInitMenu(aUieMenu);       /* open cashier number entry */

    for (;;) {
        UieGetMessage(&Data, 0x0000);                   /* wait key entry */
		if (aUieMenu) {
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
		else {
			memset (pData, 0, sizeof(UIFDIADATA));
			_tcsncpy (pData->aszMnemonics, Data.SEL.INPUT.aszKey, TCHARSIZEOF(pData->aszMnemonics) - 1);
			sRetStatus = UIF_SUCCESS;       /* set return status */
			break;
		}
    }

    UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);  /* set echo back */
    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }

    UieModeChange(usModeStatus);            /* restore mode change status */
    UieDeleteDialog();                      /* delete dialog */
    return(sRetStatus);
}

SHORT UifDiaCashier(UIFDIADATA *pData, USHORT usEcho)
{
    SHORT   sRetStatus;

	sRetStatus = UifDiaGetNumericEntry(pData, usEcho, UIFREG_MAX_ID_DIGITS, aChildRegDiaCashier);
    return(sRetStatus);
}


