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
* Title       : Reg Sales Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFSALES.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifSales() : Reg Cashier Sales
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-09-92:00.00.01:M.Suzuki   : initial                                   
* Jul-25-95:03.00.00:hkato      : R3.0
* Nov-20-95:03.01.00:hkato      : R3.1
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

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UIMENU FARCONST aChildRegItem[] = {{UifItem, CID_ITEM},
                                  {UifNoPersonFirst, CID_PERSONFIRST},
                                  {UifAlphaFirst, CID_ALPHAFIRST},
                                  {UifDispenserFirst, CID_DISPENSERFIRST},  /* R3.1 */
                                  {NULL, 0}};
UIMENU FARCONST aChildRegSalesTran[] = {{UifItem, CID_ITEM},
                                        {UifDiscount, CID_DISCOUNT},
                                        {UifEC, CID_EC},
                                        {UifTotal, CID_TOTAL},
                                        {UifPayment, CID_PAYMENT},
                                        {UifChargeTips, CID_CHARGETIPS},
                                        {UifFeedRelease, CID_FEEDRELEASE},
                                        {UifPrintOnDemand, CID_PRINTONDEMAND},
                                        {UifCancel, CID_CANCEL},
                                        {UifNoPerson, CID_PERSON},      /* GCA */
                                        {UifAlpha, CID_ALPHA},
                                        {UifCursorVoid, CID_CURSORVOID},/* R3.1 */
                                        {UifDispenser, CID_DISPENSER},  /* R3.1 */
                                        {UifCasIntSignInOutMSR, CID_CASINTSIGNINOUTMSR},    /* R3.3 */
                                        {NULL, 0}};
UIMENU FARCONST aChildRegSalesBufFull[] = {{UifTotal, CID_TOTAL},
                                           {UifPayment, CID_PAYMENT},
                                           {UifFeedRelease, CID_FEEDRELEASE},
                                           {UifPrintOnDemand, CID_PRINTONDEMAND},
                                           {UifEC, CID_EC},
                                           {UifCancel, CID_CANCEL},
                                           {UifCasIntSignInOutMSR, CID_CASINTSIGNINOUTMSR},    /* R3.3 */
                                           {NULL, 0}};
/* ---- fixed bug of r3.0 at 4/13/96 ----*/
UIMENU FARCONST aChildRegNoPersonAlpha[] =   {{UifItem, CID_ITEM},
                                        {UifNoPersonFirst, CID_PERSONFIRST},
                                        {UifAlphaFirst, CID_ALPHAFIRST},
                                        {UifDispenserFirst, CID_DISPENSERFIRST},  /* R3.1 */
                                        {UifCancel, CID_CANCEL},
                                        {NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifSales(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Cashier Sales Module
*===========================================================================
*/
SHORT UifSales(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INIT:
        /* cashier interrupt recall, R3.3 */
        if (flUifRegStatus & UIFREG_BUFFERFULL) {   /* buffer full status on ? */
            UieNextMenu(aChildRegSalesBufFull);

        } else
        if (flUifRegStatus & UIFREG_CASINTRECALL) { /* cashier interrupt recall ? */
            UieNextMenu(aChildRegSalesTran);        /* open all children */

        } else {

            UieNextMenu(aChildRegItem);             /* open item sales */
        }
        break;

    case UIM_ACCEPTED:
        /* R3.3 cashier interrupt */
        if (flUifRegStatus & UIFREG_CASHIERSIGNOUT) {   /* cashier sign out status on ? */
            UieAccept();                                /* send accepted status to parent */
            break;
        }
        if (flUifRegStatus & UIFREG_CASINTNEWIN) {      /* cashier interrupt new sign-in status on ? */
            flUifRegStatus &= ~(UIFREG_FINALIZE | UIFREG_BUFFERFULL);
            UieAccept();                                /* send accepted status to parent */
            break;
        }
        if (flUifRegStatus & UIFREG_FINALIZE) { /* finalize status on ? */
            flUifRegStatus &= ~(UIFREG_FINALIZE | UIFREG_BUFFERFULL |
                                UIFREG_CASINTRECALL);
                                                /* reset finalize and buffer full status */
            UieAccept();                        /* send accepted status to parent */
            break;
        }
        if (flUifRegStatus & UIFREG_BUFFERFULL) {   /* buffer full status on ? */
            flUifRegStatus &= ~(UIFREG_CASINTRECALL);
            UieNextMenu(aChildRegSalesBufFull);
                /* open total,payment,feed/release,print on demand,cancel, No person children */
            break;
        }
        switch (pData->SEL.usFunc) {                /* GCA */
        case CID_PERSONFIRST:
        case CID_ALPHAFIRST:
            UieNextMenu(aChildRegNoPersonAlpha);    /* bug fixed */
            break;

        default:
            UieNextMenu(aChildRegSalesTran);        /* open all children, GCA */
            break;
        }
        break;

    case UIM_REJECT:
        break;

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/
