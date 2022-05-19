/*
*===========================================================================
* Title       : Open Function
* Category    : ESC/POS Printer Controler, NCR 2170 System S/W
* Program Name: ESCPFIFO.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /c /ACw /W4 /G1s /Os /Zap                                 
* --------------------------------------------------------------------------
* Abstract:     This contains the following function:
*
* SHORT   EscpFifoEnque(ESCP_FIFO *pFifo, UCHAR uchItem);
* SHORT   EscpFifoDeque(ESCP_FIFO *pFifo);
* SHORT   EscpFifoClear(ESCP_FIFO *pFifo);
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :Ver.Rev.:   Name    : Description
* Oct-14-99:00.00.01:T.Koyama   : Initial
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
#include "stdafx.h"
#include <ecr.h>
#include <pif.h>
#include "escp.h"

/*
*===========================================================================
**  Synopsis:  SHORT   EscpFifoEnque(ESCP_FIFO *pFifo, UCHAR uchItem)
*
*   pFifo       points the Fifo table entry
*   uchItem     sequence #
*
*
**  Return:     1 = SUCCESS
*              -1 = QUEUE IS FULL
*
**  Description: This enque 1 byte itrm  into FIFO queue.
*===========================================================================
*/


SHORT   EscpFifoEnque(ESCP_FIFO *pFifo, UCHAR uchItem)
{
    UCHAR   uchTmpPtr;

    /* FIFO full check */
    if ( pFifo->uchCount == ESCP_FIFO_SIZE ) {
        return(PIF_ERROR_SYSTEM);
    }

    /* get write position */
    uchTmpPtr = (UCHAR )( ( pFifo->uchFirst + pFifo->uchCount ) % ESCP_FIFO_SIZE );               

    pFifo->uchItem[uchTmpPtr] = uchItem;   /* set item */
    pFifo->uchCount++;              /* increase counter */
    return(PIF_OK);

}

/*
*===========================================================================
**  Synopsis:  SHORT   EscpFifoDeque(ESCP_FIFO *pFifo, UCHAR uchItem)
*
*   pFifo       points the Fifo table entry
*   uchItem     sequence #
*
*
**  Return:   0 > = dequed ITEM
*              -1 = QUEUE IS EMPTY
*
**  Description: This deque 1 byte item into FIFO queue.
*===========================================================================
*/

SHORT   EscpFifoDeque(ESCP_FIFO *pFifo)
{
    SHORT   sRetItem;

    /* FIFO empty check */
    if ( pFifo->uchCount == 0 ) {
        return(PIF_ERROR_SYSTEM);
    }

    sRetItem = ( SHORT )( pFifo->uchItem[pFifo->uchFirst] );  /* set item */

    pFifo->uchFirst++;

    if ( pFifo->uchFirst >= ESCP_FIFO_SIZE ) {
        pFifo->uchFirst  %= ESCP_FIFO_SIZE ;       /* modify ptr */
    }

    /* move first pointer */
    pFifo->uchCount--;              /* declease counter   */
    return(sRetItem);

}

/*
*===========================================================================
**  Synopsis:  SHORT   EscpFifoClear(ESCP_FIFO *pFifo, UCHAR uchItem)
*
*   pFifo       points the Fifo table entry
*   uchItem     sequence #
*
*
**  Return:    1 = SUCCESS
*
**  Description: This cleawr FIOF queue.
*===========================================================================
*/
SHORT   EscpFifoClear(ESCP_FIFO *pFifo)
{
    /* clear pointer & counter */
    pFifo->uchFirst = 0;
    pFifo->uchCount = 0;
    return(PIF_OK);
}


