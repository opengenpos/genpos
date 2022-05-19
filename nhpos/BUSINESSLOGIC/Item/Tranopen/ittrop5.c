/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION OPEN MODULE                                 
:   Category       : NCR 2170 US Hospitality Application 
:   Program Name   : ITTROP5.C (Flexible Drive Through)                                       
:  ---------------------------------------------------------------------  
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen)
*       2012  -> GenPOS Rel 2.2.0
*       2014  -> GenPOS Rel 2.2.1
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
:  ---------------------------------------------------------------------  
:  Abstract:            
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date   : Ver.Rev. :   Name     : Description
: Apr-24-95: 03.00.00 : hkato      : R3.0
: Nov-10-95: 03.01.00 : hkato      : R3.1
: Aug-13-99: 03.05.00 : K.Iwata    : R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
: Apr-04-15: 02.02.01 : R.Chambers : localize variables, remove unneeded includes
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include	<tchar.h>
#include    <string.h>

#include    "ecr.h"
#include    "pif.h"
#include    "uie.h"
#include    "uireg.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csttl.h"
#include    "trans.h"
#include    "rfl.h"
#include    "display.h"
#include    "item.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "itmlocal.h"
#include    "fdt.h"


/*==========================================================================
**   Synopsis:  SHORT   ItemTransACStoDeliv( UIFREGTOTAL *UifRegTransOpen )   
*
*   Input:      UIFREGTRANSOPEN     *UifRegTransOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    Recall Operation at Delivery Terminal
==========================================================================*/
SHORT   ItemTransACStoDeliv(UIFREGTRANSOPEN *UifRegTransOpen)
{
    SHORT           sStatus;    
	REGDISPMSG      DispMsg = {0};

    if (ItemModLocalPtr->fsTaxable) {
		/* sequence error if tax modifier key has been used */
        return(LDT_SEQERR_ADR);
    }
    if (UifRegTransOpen->fbModifierTransOpen & VOID_MODIFIER) {
		/* sequence error if Void key has been used */
        return(LDT_SEQERR_ADR);
    }
    if (UifRegTransOpen->ulAmountTransOpen != 0L) {
		// sequence error if a guest check number is specified.
        return(LDT_SEQERR_ADR);
    }

    if ((sStatus = FDTDeliveryRecall()) != ITM_SUCCESS) {
        return(sStatus);
    }

    DispMsg.uchMajorClass = CLASS_UIFREGTRANSOPEN;   
    DispMsg.uchMinorClass = CLASS_UIADDCHECK1;   
    DispMsg.lAmount = 0;
    RflGetLead (DispMsg.aszMnemonic, LDT_PROGRES_ADR);
    DispMsg.fbSaveControl = 0;
    DispWrite(&DispMsg);

    ItemTransACStoServTime();

    return(UIF_DELIV_RECALL);
}

/*==========================================================================
**   Synopsis:  VOID    ItemTransACStoServTime(VOID)
*                               
*   Input:      none
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    generate affection(service time) data.
==========================================================================*/
VOID    ItemTransACStoServTime(VOID)
{
    TrnInitialize(TRANI_ITEM);

    if (!(TranModeQualPtr->fbCashierStatus & MODEQUAL_CASHIER_TRAINING) && TranGCFQualPtr->usGuestNo != 0)
	{
		ITEMTRANSOPEN   ItemTransOpen = {0};
		ITEMAFFECTION   ItemAffection = {0};
		ITEMTRANSCLOSE  ItemTransClose = {0};
		SHORT           sStatus;

        /*--- Create Transaction Open Data ---*/
        ItemTransOpen.uchMajorClass = CLASS_ITEMTRANSOPEN;
        ItemTransOpen.uchMinorClass = CLASS_STORE_RECALL_DELIV;
        ItemTransOpen.fbStorageStatus = NOT_CONSOLI_STORAGE;
		ItemTransOpen.uchHourlyOffset = TranGCFQualPtr->uchHourlyOffset;
        TrnOpen(&ItemTransOpen);

        /*--- Create Affection Data ---*/
        ItemAffection.uchMajorClass = CLASS_ITEMAFFECTION;
        ItemAffection.uchMinorClass = CLASS_SERVICETIME;
        ItemAffection.fbStorageStatus = NOT_CONSOLI_STORAGE;
        ItemTransCalSec(&ItemAffection);
        TrnItem(&ItemAffection);

        /*--- Create Transaction Close Data ---*/
        ItemTransOpen.uchMajorClass = CLASS_ITEMTRANSCLOSE;
        ItemTransOpen.uchMinorClass = CLASS_CLSDELIVERY;
        TrnClose(&ItemTransClose);

        /* send to enhanced kds, 2172 */
        ItemSendKds(&ItemTransClose, 0);
    
        /*--- Send Affection Data to Total Module ---*/
        while ((sStatus = TrnSendTotal() ) != TRN_SUCCESS) {
			USHORT  usStatus = TtlConvertError(sStatus);
            UieErrorMsg(usStatus, UIE_WITHOUT);
        }
    }
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTransACStoDelivOpen(VOID)
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    Transaction Open at Delivery Terminal.
==========================================================================*/
SHORT   ItemTransACStoGetDelivOpen(VOID)
{
    if (RflGetSystemType () == FLEX_STORE_RECALL) {
		FDTPARA   WorkFDT = {0};

		FDTParameter(&WorkFDT);
        if (WorkFDT.uchTypeTerm == FX_DRIVE_DELIV_1 || WorkFDT.uchTypeTerm == FX_DRIVE_DELIV_2) {
            if (TrnInformationPtr->TranGCFQual.usGuestNo != 0) {
                return(FDTDeliveryAddon());
            }
        }
    }

    return(ITM_SUCCESS);
}

/*==========================================================================
**   Synopsis:  SHORT   ItemTransACStoOrderCounterOpen(UCHAR uchOrder)
*
*   Input:      UIFREGTRANSOPEN *UifRegTransOpen
*   Output:     none
*   InOut:      none
*
*   Return:     
*
*   Description:    Transaction Open at Delivery Terminal.
==========================================================================*/
SHORT   ItemTransACStoOrderCounterOpen(USHORT uchOrder)
{
    if (RflGetSystemType () == FLEX_STORE_RECALL) {
		FDTPARA     WorkFDT = {0};

		FDTParameter(&WorkFDT);
        if (WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE) {
            FDTTransOpen(uchOrder);
        } else if (WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_STORE_PAY
            || WorkFDT.uchTypeTerm == FX_DRIVE_COUNTER_FUL_STORE_PAY
            || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_1
            || WorkFDT.uchTypeTerm == FX_DRIVE_ORDER_TERM_2) {
            FDTTransOpenStorePay(uchOrder);
        }
    }

    return(ITM_SUCCESS);
}

/****** end of source ******/
