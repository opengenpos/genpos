/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reg Reorder Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFREODR.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifReorder() : Reg Reoder
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial                                   
* Jan-09-96:03.01.00:hkato      : R3.1
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
UIMENU FARCONST aChildRegPBEntry[] = {{UifPB, CID_PB},{NULL, 0}};
UIMENU FARCONST aChildRegReorderTran[] = {{UifItem, CID_ITEM},
                                          {UifSeat, CID_SEATENTRY},
                                          {UifTransfItem, CID_TRANSFITEM},
                                          {UifGCInformation, CID_GCINFORMATION},
                                          {UifTotal, CID_TOTAL},
                                          {UifPrintOnDemand, CID_PRINTONDEMAND},
                                          {UifFeedRelease, CID_FEEDRELEASE},
                                          {UifDiscount, CID_DISCOUNT},
                                          {UifCancel, CID_CANCEL},
                                          {UifCursorVoid, CID_CURSORVOID},/* R3.1 */
                                          {UifDispenser, CID_DISPENSER},  /* R3.1 */
                                          {NULL, 0}};
UIMENU FARCONST aChildRegReorderSales[] = {{UifItem, CID_ITEM},
                                          {UifSeat, CID_SEATENTRY},
                                          {UifTransfItem, CID_TRANSFITEM},
                                          {UifGCInformation, CID_GCINFORMATION},
                                          {UifEC, CID_EC},
                                          {UifTotal, CID_TOTAL},
                                          {UifPrintOnDemand, CID_PRINTONDEMAND},
                                          {UifFeedRelease, CID_FEEDRELEASE},
                                          {UifDiscount, CID_DISCOUNT},
                                          {UifCancel, CID_CANCEL},
                                          {UifCursorVoid, CID_CURSORVOID},/* R3.1 */
                                          {UifDispenser, CID_DISPENSER},  /* R3.1 */
                                          {NULL, 0}};
UIMENU FARCONST aChildRegReorderBufFull[] = {{UifTotal, CID_TOTAL},
                                             {UifFeedRelease, CID_FEEDRELEASE},
                                             {UifPrintOnDemand, CID_PRINTONDEMAND},
                                             {UifEC, CID_EC},
                                             {UifCancel, CID_CANCEL},
                                             {NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifReorder(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Reorder Module
*===========================================================================
*/
SHORT UifReorder(KEYMSG *pData)
{

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildRegPBEntry);    /* open PB entry */
        break;

    case UIM_ACCEPTED:
        if (flUifRegStatus & UIFREG_FINALIZE) { /* finalize status on ? */
            flUifRegStatus &= ~(UIFREG_FINALIZE | UIFREG_BUFFERFULL);
                                                /* reset finalize and buffer full status */
            UieAccept();                        /* send accepted status to parent */
            break;
        }
        if (flUifRegStatus & UIFREG_BUFFERFULL) {   /* buffer full status on ? */
            UieNextMenu(aChildRegReorderBufFull);
                                /* open total,feed/release,print on demand,line no,cancel children */
            break;
        }
        if (pData->SEL.usFunc == CID_PB) {
            UieNextMenu(aChildRegReorderTran);     /* open all children except PB entry and EC */
        } else {
            UieNextMenu(aChildRegReorderSales);    /* open all children except PB entry */
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
