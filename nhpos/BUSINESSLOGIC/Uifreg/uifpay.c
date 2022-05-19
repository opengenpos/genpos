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
* Title       : Reg Payment Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFPAY.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPayment() : Reg Payment
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-09-92:00.00.01:M.Suzuki   : initial                                   
* Jan-08-96:03.01.00:hkato      : R3.1
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
#include <ecr.h>
#include <uie.h>
#include <regstrct.h>
#include <paraequ.h>
#include <uireg.h>
#include "uiregloc.h"
#include "BlFWif.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UIMENU FARCONST aChildRegPaymentFirst[] = {{UifTender, CID_TENDER},
                                           {UifSeat, CID_SEATENTRY},
                                           {UifForeignIn, CID_FOREIGNIN},{NULL, 0}};
UIMENU FARCONST aChildRegPaymentPartial[] = {{UifTender, CID_TENDER},
                                             {UifForeignIn, CID_FOREIGNIN},
                                             {UifTotal, CID_TOTAL},     /* Saratoga */
											 {UifChargeTips, CID_CHARGETIPS},
/*                                             {UifPayModifier, CID_PAYMODIFIER},   Saratoga */
                                             {UifEC, CID_EC},{NULL, 0}};
UIMENU FARCONST aChildRegPaymentForeign[] = {{UifForeignOut, CID_FOREIGNOUT},
                                             {UifForeignOutEC, CID_FOREIGNOUTEC},
                                             {UifPayModifier, CID_PAYMODIFIER},
                                             {NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifPayment(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Payment Module
*===========================================================================
*/
SHORT UifPayment(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildRegPaymentFirst);     /* open tender, foreign total */
        break;

    case UIM_ACCEPTED:                          /* R3.1 */
        if ((flUifRegStatus & UIFREG_BASETRANS) || (flUifRegStatus & UIFREG_CURSORRETURN)) {
			flUifRegStatus &= ~UIFREG_CURSORRETURN;
            UieAccept();
            break;
        }
        if (flUifRegStatus & UIFREG_FINALIZE) { /* finalize status on ? */
            flUifRegStatus &= ~UIFREG_PARTIAL;  /* reset partial status */
            UieAccept();                        /* send accepted status to parent */
            break;
        }
        if (pData->SEL.usFunc == CID_FOREIGNIN) {   /* foreign in ? */
            UieNextMenu(aChildRegPaymentForeign);   /* open foreign out child */
            break;
        }
        if (flUifRegStatus & UIFREG_BUFFERFULL) {   /* buffer full status on ? */
            if (!(flUifRegStatus & UIFREG_PARTIAL)) {   /* partial tender status off ? */
                UieAccept();                            /* send accepted status to parent */
                break;
            }
        }
        flUifRegStatus |= UIFREG_PARTIAL;           /* set partial status */
        UieNextMenu(aChildRegPaymentPartial);       /* open tender,foreignin,ec children */
        break;

    case UIM_REJECT:
        if (pData->SEL.usFunc == CID_FOREIGNOUT) {  /* foreign out ? */
            UieNextMenu(aChildRegPaymentForeign);   /* open foreign out child */
            break;
        }
        if (flUifRegStatus & UIFREG_PARTIAL) {      /* partial tender status on ? */
            UieNextMenu(aChildRegPaymentPartial);        /* open tender,foreignin,ec children */
            break;
        }
        UieAccept();                                /* send accepted status to parent */
        break;

#if 0
	// remove handling the cancel key if there is a balance due
	// this allows a partial tender with credit and then cancel resulting in
	// financials and back office being incorrect.
	// AMT-2436
	case UIM_CANCEL:
        UieAccept();                                /* send accepted status to parent */
		UifRegClearDisplay(0);						/* clear work area */
		BlFwIfDefaultUserScreen();					/* call function to bring up default window(s) */
		break;
#endif
    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/
