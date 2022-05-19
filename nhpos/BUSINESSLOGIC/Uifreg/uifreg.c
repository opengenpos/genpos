/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg Mode In Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFREG.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifReg() : Reg Mode In
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-06-92:00.00.01:M.Suzuki   : initial                                   
* Jun-18-93:01.00.12:K.You      : add ETK Feature.                                   
* Dec-07-95:03.01.00:hkato      : R3.1
* Jun-02-98:03.03.00:M.Ozawa    : add cashier interrupt key.
* Aug-06-98:03.03.00:hrkato     : V3.3 (Support Scanner)
* Aug-11-99:03.05.00:K.Iwata    : R3.5 (remove WAITER_MODEL)
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
#include "ecr.h"
#include "log.h"
#include "pif.h"
#include "uie.h"
#include "uic.h"
#include "pifmain.h"
#include "regstrct.h"
#include "item.h"
#include "paraequ.h"
#include "uireg.h"
#include "appllog.h"
#include "enhkds.h"
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Static Area Declarations
*===========================================================================
*/
UIFITEMDATA UifRegData;                         /* Item Module Interface Data */
UCHAR       uchUifRegVoid;                      /* Item Void Save Area */
TCHAR       aszUifRegNumber[NUM_NUMBER];        /* Number Save Area */
UIFDIADATA  UifRegDiaWork;                      /* Dialog Work Area */
ULONG       flUifRegStatus;                     /* UI for Reg Status Area, Saratoga */
SHORT       husUifRegHandle;                    /* UIC Handle Save Area */
UCHAR       auchUifDifFsc[2];                   /* Beverage Dispenser String Buffer, R3.1 */
UCHAR       fchUifSalesMod;                     /* Sales Modifier Save Area, 2172 */
UCHAR	    uchIsNoCheckOn;					    /* Is No Check functionality on SR 18*/
SHORT		fsPrtCompul;						/* Flag to be used when using the Check No Purchase functionality*/
SHORT		fsPrtNoPrtMask;						/* Flag to be used when using the Check No Purchase functionality*/

/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UIMENU FARCONST aChildRegModeIn[] = {{UifMSR, CID_MSR},
                                     {UifCashier, CID_CASHIER},
                                     {UifCasInt, CID_CASINT},   /* R3.3 */
                                     {UifETKTimeIn, CID_ETKTIMEIN},
                                     {UifETKTimeOut, CID_ETKTIMEOUT},
                                     {UifPostReceiptSignOut, CID_POSTRECEIPTSIGNOUT},
                                     {NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifReg(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     InOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Mode Mode In Module
*===========================================================================
*/
SHORT UifReg(KEYMSG *pData)
{
    SHORT   sRetStatus;

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieCtrlDevice(UIE_ENA_MSR);                     /* enable MSR,  R3.1 */
        UieCtrlDevice(UIE_ENA_VIRTUALKEY);              // enable Virtual Keyboard such as FreedomPay
        UieCtrlDevice(UIE_ENA_WAITER);                  /* set enable input device, V3.3 */
        ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        UifRegWorkClear();                              /* Clear UI Work Area */
        flUifRegStatus = 0;
        husUifRegHandle = 0;
        UifRegData.regmodein.uchMajorClass = CLASS_UIFREGMODE;  /* set mode in class */
        UifRegData.regmodein.uchMinorClass = CLASS_UIREGMODEIN;
        ItemModeIn(&UifRegData.regmodein);              /* Item Mode In Modele */
        /* break */                                     /* not use */

    case UIM_ACCEPTED:
        if (husUifRegHandle) {                          /* during transaction lock ? */
            if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {  /* reset transaction lock */
                PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
            }
            husUifRegHandle = 0;
            UifCPPrintUns();                            /* V3.3 */
        }
        ItemOtherDifDisableStatus();         /* R3.1 for Beverage Dispenser */
        UieEchoBack(UIE_ECHO_NO, UIFREG_MAX_INPUT);     /* set no echo back */
        UieNextMenu(aChildRegModeIn);                   /* open next menu */
        /* KdsClose();                                     / keep kds service for resend message, 12/21/01 */
        break;

    case UIM_QUIT:
        if (husUifRegHandle) {                          /* during transaction lock ? */
            if (UicResetFlag(husUifRegHandle) != UIC_SUCCESS) {  /* reset transaction lock */
                PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
            }
            husUifRegHandle = 0;
            UifCPPrintUns();                            /* V3.3 */
        }
        UieCtrlDevice(UIE_DIS_MSR);                     /* disable MSR */
        UieCtrlDevice(UIE_DIS_VIRTUALKEY);              // disable Virtual Keyboard such as FreedomPay
        UieCtrlDevice(UIE_DIS_SCANNER);                 /* disable Scanner, V3.3 */
        UieCtrlDevice(UIE_DIS_WAITER);                  /* set disable input device, V3.3 */
        ItemOtherDifDisableStatus();                    /* R3.1 for Beverage Dispenser */
        /* V3.3 */
        UifRegData.regopeclose.uchMajorClass = CLASS_UIFREGOPECLOSE;  /* set cashier sign out class */
        UifRegData.regopeclose.uchMinorClass = CLASS_UIMODELOCK;
        ItemSignOut(&UifRegData.regopeclose);           /* cashier sign out Modele for storage close, V3.3 */
        KdsClose();                                     /* close enhanced kds service, 2172 */
        break;
	case	UIM_INPUT: //SR289 ESMITH
		/*
			To implement new LDT "Not signed in" as the default error message, instead of sequence error,
			the process had to be preempted so that it would not go into the UifRegDefProc function. When
			not signed in all commands except clear key and mode changes are routed through this function, 
			UifReg, if it is an input command i.e. Dept#, Plu, etc. then it will return the LDT_NOTSIGNEDIN_ADR 
			mnemonic. It should proceed with the UifRegDefProc only if the functionality requesting is a 
			control string (0x82), reseting of the pcif log (0x2d), or the cancel button (0x5e).

			The values such as 0x82 or 0xa2 are FSC codes as listed in the file fsc.h.
		*/
		if(pData->SEL.uchMode == 0x82 || pData->SEL.uchMode == 0xa2 ||  // allow FSC_STRING or FSC_DOC_LAUNCH key press
		   pData->SEL.uchMode == 0xa5 || pData->SEL.uchMode == 0xa6 ||  // allow FSC_WINDOW_DISPLAY or FSC_WINDOW_DISMISS key press
		   pData->SEL.uchMode == 0x5e || pData->SEL.uchMode == 0xa7 ||  // allow FSC_CANCEL key press or FSC_AUTO_SIGN_OUT
		   pData->SEL.uchMode == 0x2d) {
			sRetStatus = UifRegDefProc(pData);
		} else {
			sRetStatus = LDT_NOTSIGNEDIN_ADR;
		}

		UieEchoBack(UIE_ECHO_NO, UIFREG_MAX_INPUT);     /* set no echo back */		
		return(sRetStatus);
		break;

    default:                                            /* not use */
        sRetStatus = UifRegDefProc(pData);              /* default process */
        UieEchoBack(UIE_ECHO_NO, UIFREG_MAX_INPUT);     /* set no echo back */
        return(sRetStatus);
    }
    return (SUCCESS);
}

/****** End of Source ******/
