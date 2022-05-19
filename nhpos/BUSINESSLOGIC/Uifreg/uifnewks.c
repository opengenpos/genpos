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
* Title       : Reg New Check Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFNEWKS.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifNewKeySeq()  : Reduce Newchec, Reorder and Addcheck Key
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jul-31-98:03.03.00:hrkato     : V3.3 (New Key Sequence)
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
UIMENU FARCONST aChildRegNewKeySeqEntry[] = {{UifGCNumberEntry, CID_GCNUMBERENTRY},
                                            {NULL, 0}};
/* UIMENU FARCONST aChildRegNewKeySeqEntry[] = {{UifNewKeySeqEntry, CID_NEWKEYSEQENTRY},
                                            {NULL, 0}};
*/
UIMENU FARCONST aChildRegNewKeySeq[] = {{UifItem, CID_ITEM},
                                           {UifSeat, CID_SEATENTRY},
                                           {UifTransfItem, CID_TRANSFITEM},
                                           {UifGCInformation, CID_GCINFORMATION},
                                           {UifTotal, CID_TOTAL},
                                           {UifNewKeySeqGC, CID_NEWKEYSEQGC},
                                           {UifAddCheckEntry, CID_ADDCHECKENTRY},
                                           {UifChargeTips, CID_CHARGETIPS},
                                           {UifFeedRelease, CID_FEEDRELEASE},
                                           {UifDiscount, CID_DISCOUNT},
                                           {UifPrintOnDemand, CID_PRINTONDEMAND},
                                           {UifPayment, CID_PAYMENT},
                                           {UifCancel, CID_CANCEL},
                                           {UifCursorVoid, CID_CURSORVOID},
                                           {UifEC, CID_EC},
                                           {UifDispenser, CID_DISPENSER},
                                           {NULL, 0}};
UIMENU FARCONST aChildRegNewKeySeqBufFull[] = {{UifTotal, CID_TOTAL},
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
** Synopsis:    SHORT UifNewKeySeq(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg New Check Module
*===========================================================================
*/
SHORT   UifNewKeySeq(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildRegNewKeySeqEntry);
        break;

    case UIM_ACCEPTED:
        if (flUifRegStatus & UIFREG_FINALIZE) { /* finalize status on ? */
            flUifRegStatus &= ~(UIFREG_FINALIZE | UIFREG_BUFFERFULL);
                                                /* reset finalize and buffer full status */
            UieAccept();                        /* send accepted status to parent */
            break;
        }
        if (flUifRegStatus & UIFREG_BUFFERFULL) {   /* buffer full status on ? */
            UieNextMenu(aChildRegNewKeySeqBufFull);
                                /* open total,feed/release,print on demand,cancel children */
            break;
        }
        UieNextMenu(aChildRegNewKeySeq);
        break;

    case UIM_REJECT:
        break;

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}

/****** End of Source ******/
