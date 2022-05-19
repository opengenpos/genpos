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
* Title       : Reg Add Check Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFADDCK.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAddCheck() : Reg Add Check
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial                                   
* Dec-08-93:02.00.03:K.You      : del. store/recall feature.                                   
* Jan-12-95:        :hkato      : R3.0, Recover store/recall feature.
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
UIMENU FARCONST aChildRegAddCheckEntry[] = {{UifAddCheckEntry, CID_ADDCHECKENTRY},
                                            /* {UifGCNumberEntry, CID_GCNUMBERENTRY}, */
                                            {NULL, 0}};
UIMENU FARCONST aChildRegAddCheckTran[] = {{UifAddCheckEntry, CID_ADDCHECKENTRY},
                                           {UifItem, CID_ITEM},
                                           {UifSeat, CID_SEATENTRY},
                                           {UifNewKeySeqGC, CID_NEWKEYSEQGC},   /* V3.3 */
                                           {UifTransfItem, CID_TRANSFITEM},
                                           {UifGCInformation, CID_GCINFORMATION},
                                           {UifTotal, CID_TOTAL},
                                           {UifChargeTips, CID_CHARGETIPS},
                                           {UifFeedRelease, CID_FEEDRELEASE},
                                           {UifDiscount, CID_DISCOUNT},
                                           {UifPrintOnDemand, CID_PRINTONDEMAND},
                                           {UifPayment, CID_PAYMENT},
                                           {UifCancel, CID_CANCEL},
                                           {UifCursorVoid, CID_CURSORVOID},/* R3.1 */
                                           {UifDispenser, CID_DISPENSER},  /* R3.1 */
                                           {NULL, 0}};
UIMENU FARCONST aChildRegAddCheckSRSales[] = {{UifItem, CID_ITEM},
                                              {UifGCInformation, CID_GCINFORMATION},
                                              {UifTotal, CID_TOTAL},
                                              {UifChargeTips, CID_CHARGETIPS},
                                              {UifFeedRelease, CID_FEEDRELEASE},
                                              {UifDiscount, CID_DISCOUNT},
                                              {UifPrintOnDemand, CID_PRINTONDEMAND},
                                              {UifPayment, CID_PAYMENT},
                                              {UifCancel, CID_CANCEL},
                                              {UifCursorVoid, CID_CURSORVOID},/* R3.1 */
                                              {UifDispenser, CID_DISPENSER},  /* R3.1 */
                                              {NULL, 0}};
UIMENU FARCONST aChildRegAddCheckSales[] = {{UifItem, CID_ITEM},
                                            {UifGCInformation, CID_GCINFORMATION},
                                            {UifSeat, CID_SEATENTRY},
                                            {UifTransfItem, CID_TRANSFITEM},
                                            {UifEC, CID_EC},
                                            {UifTotal, CID_TOTAL},
                                            {UifChargeTips, CID_CHARGETIPS},
                                            {UifFeedRelease, CID_FEEDRELEASE},
                                            {UifDiscount, CID_DISCOUNT},
                                            {UifPrintOnDemand, CID_PRINTONDEMAND},
                                            {UifPayment, CID_PAYMENT},
                                            {UifCancel, CID_CANCEL},
                                            {UifCursorVoid, CID_CURSORVOID},/* R3.1 */
                                            {UifDispenser, CID_DISPENSER},  /* R3.1 */
                                            {NULL, 0}};
UIMENU FARCONST aChildRegAddCheckBufFull[] = {{UifTotal, CID_TOTAL},
                                              {UifPayment, CID_PAYMENT},
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
** Synopsis:    SHORT UifAddCheck(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Add Check Module
*===========================================================================
*/
SHORT UifAddCheck(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildRegAddCheckEntry);    /* open Add Check entry */
        break;

    case UIM_ACCEPTED:
        if (flUifRegStatus & UIFREG_BASETRANS) {
            flUifRegStatus &= ~UIFREG_BASETRANS;
            UieNextMenu(aChildRegAddCheckTran);
            break;
        }
        if (flUifRegStatus & UIFREG_FINALIZE) { /* finalize status on ? */
            flUifRegStatus &= ~(UIFREG_FINALIZE | UIFREG_BUFFERFULL);
                                                /* reset finalize and buffer full status */
            UieAccept();                        /* send accepted status to parent */
            break;
        }
        if (flUifRegStatus & UIFREG_BUFFERFULL) {   /* buffer full status on ? */
            UieNextMenu(aChildRegAddCheckBufFull);
                        /* open total,payment,feed/release,print on demand,line no,cancel children */
            break;
        }
        switch (pData->SEL.usFunc) {
        case CID_ADDCHECKENTRY:
        case CID_TOTAL:
            UieNextMenu(aChildRegAddCheckTran);     /* open all children except GC number entry and EC */
            break;

        case CID_GCNUMBERENTRY:
            UieNextMenu(aChildRegAddCheckSRSales);  /* open all children except GC number entry, add check and EC */
            break;

        default:
            UieNextMenu(aChildRegAddCheckSales);    /* open all children except GC number entry  and add check */
            break;
        }
        break;

    case UIM_REJECT:
        break;

    default:                                    /* not use */
        return(UifRegDefProc(pData));           /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/
