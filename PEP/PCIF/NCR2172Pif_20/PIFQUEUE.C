/*mhmh
**********************************************************************
**                                                                  **
**  Title:      The queue on Platform Interface                     **
**                                                                  **
**  File Name:  PIFQUEUE.C                                          **
**                                                                  **
**  Categoly:   Poratable Platform Interface of PC Interface        **
**                                                                  **
**  Abstruct:   This module provides the queue functions of         **
**              Platform Interface.                                 **
**              This module provides the following functions:       **
**                                                                  **
**                PifCreateQueue():     Create Queue                **
**                PifDeleteQueue():     Delete Queue                **
**                PifReadQueue():       Read Queue                  **
**                PifWriteQueue():      Write Queue                 **
**                                                                  **
**                                                                  **
**              But, the all function don't provided the feature.   **
**                                                                  **
**  Compiler's options: /c /ACw /W4 /Zp /Ot /G1s                    **
**                                                                  **
**********************************************************************
**                                                                  **
**  Modification History:                                           **
**                                                                  **
**      Ver.      Date        Name      Description                 **
**      1.00    Aug/17/92   T.Kojima    Initial release             **
**                                                                  **
**********************************************************************
mhmh*/

/*******************\
*   Include Files   *
\*******************/
#include    "XINU.H"    /* XINU */
#define     PIFXINU     /* switch for "PIF.H" */
#include    "PIF.H"     /* Portable Platform Interface */
#include    "LOG.H"     /* Fault Codes Definition */
#include    "PIFXINU.H" /* Platform Interface */

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT PIFENTRY PifCreateQueu(USHORT usCount);      **
**              usCount:    queue count                             **
**                                                                  **
**  Return:     0 (Queue Handle)                                    **
**                                                                  **
** Description: create new queue with the specified count.          **
**                                                                  **
**********************************************************************
fhfh*/
USHORT PIFENTRY PifCreateQueue(USHORT usCount)
{
    return(0);

    usCount = usCount;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifDeleteQueue(USHORT usQueue);       **
**              usQueue:    queue handle                            **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: deletes queue                                       **
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifDeleteQueue(USHORT usQueue)
{
    return;

    usQueue = usQueue;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID * PIFENTRY PifReadQueue(USHORT usQueue);       **
**              usQueue:    queue handle                            **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: reads the specified queue message with the queue    **
**              handle.                                             **
**                                                                  **
**********************************************************************
fhfh*/
VOID * PIFENTRY PifReadQueue(USHORT usQueue)
{
    return(NULL);

    usQueue = usQueue;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifWriteQueue(USHORT usQueue,         **
**                                          CONST VOID *pMessage);  **
**              usSem:      queue handle                            **
**              pMessage:   pointer of message                      **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: writes the specified queue message with the queue   **
**              handle.                                             **
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifWriteQueue(USHORT usQueue, CONST VOID *pMessage)
{
    return;

    usQueue  = usQueue;
    pMessage = pMessage;
}
