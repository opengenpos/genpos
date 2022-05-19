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
* Title       : Price Compulsory Module                                     
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDPRC.C
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifDiaPrice() : Price Compulsory Dialog
*               UifDiaDept()  : Department Number Compulsory Dialog
*               UifDiaTare()  : Tare Compulsory Dialog
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data   : Ver.Rev. :   Name      : Description
* Nov-09-99 :          : M.Ozawa     : initial                                   
* Aug-01-15 : 02.02.01 : R.Chambers  : consolidated logic with UifDiaPriceDeptTare(), added UifDiaDept()
* Nov-07-15 : 02.02.01 : R.Chambers  : last changes introduced bug in Request Tare needed fixing.
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

static SHORT UifDiaPriceDeptTare (UIFDIADATA *pData, UIMENU CONST *aChildRegDia)
{
    KEYMSG  Data;
    USHORT  usModeStatus, usCtrlDevice;
    SHORT   sRetStatus, sStatusSave;

    UieCreateDialog();                      /* create dialog */
    usModeStatus = UieModeChange (UIE_DISABLE);      /* disable mode change */
    usCtrlDevice = UieCtrlDevice (UIE_DIS_SCANNER|UIE_DIS_WAITER);   /* disable waiter lock */
    sStatusSave = ItemOtherDifDisableStatus ();      /* R3.1 for Beverage Dispenser */
    UieInitMenu (aChildRegDia);      /* open department number entry */
    for (;;) {
        UieGetMessage (&Data, 0x0000);                   /* wait key entry */
        sRetStatus = UieCheckSequence (&Data);   /* check key */
        if (sRetStatus == UIFREG_ABORT) {       /* cancel ? */
            break;
        }
		else if (sRetStatus == UIF_DIA_SUCCESS || sRetStatus == UIF_SUCCESS) {    /* success ? */
			if (Data.SEL.INPUT.uchMajor == FSC_CLEAR) {
				// Testing we are seeing that a first time of pressing
				// the Clear Key is not always being processed in the standard
				// way such that the UifRegDiaWork struct is properly populated
				// so should the key press data indicate the Clear Key was pressed
				// we will just set the UifRegDiaWork FSC data as being an
				// Error Correct Key press.
				UifRegDiaWork.auchFsc[0] = FSC_EC;   // turn FSC_CLEAR into FSC_EC or Error Correct
			}
			// some menus of aChildRegDia return UIF_DIA_SUCCESS for success. Others return UIF_SUCCESS
			// and naturally the two are defined as different values.  Big SIGH!!
            *pData = UifRegDiaWork;    /* set cahier number */
            sRetStatus = UIF_SUCCESS;       /* set return status */
			if (UifRegDiaWork.auchFsc[0] == FSC_CLEAR ||
				UifRegDiaWork.auchFsc[0] == FSC_CANCEL ||
				UifRegDiaWork.auchFsc[0] == FSC_EC) {
				UieEmptyRingBufClearMacro();
			}
			else {
				break;
			}
        }
	}
    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus (1);    /* R3.1 for Beverage Dispenser */
    }
    UieCtrlDevice (usCtrlDevice);            /* enable waiter lock */
    UieModeChange (usModeStatus);            /* restore mode change status */
    UieDeleteDialog ();                      /* delete dialog */
    return (sRetStatus);
}

/*
*===========================================================================
** Synopsis:    SHORT UifDiaPrice(UIFDIADATA *pData)
*     Input:    nothing
*     Output:   pData - line number buffer pointer
*     InOut:    nothing
*
** Return:      success = 0
*               cancel  = -2
*
** Description: Register Mode Price Entry Compulsory
*===========================================================================
*/
SHORT UifDiaPrice(UIFDIADATA *pData)
{
	UIMENU CONST aChildRegDiaPrice[] = {{UifDiaPriceEntry, CID_DIAPRICEENTRY},{NULL, 0}};

	return UifDiaPriceDeptTare (pData, aChildRegDiaPrice);
}

/*
*===========================================================================
** Synopsis:    SHORT UifDiaDept(UIFDIADATA *pData)
*     Input:    nothing
*     Output:   pData - line number buffer pointer
*     InOut:    nothing
*
** Return:      success = 0
*               cancel  = -2
*
** Description: Register Mode Department Number Entry Compulsory
*===========================================================================
*/
SHORT UifDiaDept (UIFDIADATA *pData)
{
	UIMENU CONST aChildRegDiaDept[] = {{UifDiaDeptEntry, CID_DIADEPTNO},{NULL, 0}};

	return UifDiaPriceDeptTare (pData, aChildRegDiaDept);
}

/*
*===========================================================================
** Synopsis:    SHORT UifDiaTare(UIFDIADATA *pData)
*     Input:    nothing
*     Output:   pData - line number buffer pointer
*     InOut:    nothing
*
** Return:      success = 0
*               cancel  = -2
*
** Description: Register Mode Tare Entry Compulsory
*===========================================================================
*/
SHORT UifDiaTare(UIFDIADATA *pData)
{
	UIMENU CONST aChildRegDiaTare[] = {{UifDiaTareEntry, CID_DIATAREENTRY},{NULL,0}};

	return UifDiaPriceDeptTare (pData, aChildRegDiaTare);
}
