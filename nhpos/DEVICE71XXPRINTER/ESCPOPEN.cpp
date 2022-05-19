/*
*===========================================================================
* Title       : Open Function
* Category    : ESC/POS Printer Controler, NCR 2170 System S/W
* Program Name: ESCPOPEN.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /c /ACw /W4 /G1s /Os /Zap                                 
* --------------------------------------------------------------------------
* Abstract:     This contains the following function:
*
*           SHORT EscpOpenCom(USHORT usPortId, PROTOCOL *pProtocol);
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :Ver.Rev.:   Name    : Description
* Oct-14-99:00.00.01:T.Koyama   : Initial
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
**  Synopsis:   SHORT EscpOpenCom(USHORT usPortId, PROTOCOL *pProtocol)
*
*       usPortId    Port Number
*
**  Return:     PIF_OK              - Device Number
*               PIF_ERROR_SYSTEM    - Already Opened
*
**  Description: This sets up the port and starts the service.
*===========================================================================
*/
SHORT EscpOpenCom(USHORT   usPortId, PROTOCOL *pProtocol)
{
    ESCP            *pEscp;
    SHORT           sStatus,sOpenStatus;
    USHORT          usTimerVal;

    pEscp = &Escp[0];
    if (pEscp->chOpen) {                /* see if already opened */
        return (PIF_ERROR_SYSTEM);
    }
    
    sOpenStatus = PifOpenCom(usPortId, pProtocol);
        
    pEscp->sSio = sOpenStatus;              /* save SIO driver handle */

    if (sOpenStatus < 0) {                  /* see if failed to open Serial I/O */
        if (sOpenStatus ==PIF_ERROR_COM_NOT_PROVIDED) {
            return ( sOpenStatus );           /* designated port is not provided */
        }
        return (PIF_ERROR_SYSTEM);
    }

    usTimerVal = ESCP_TIMER_SEND;
    sStatus = PifControlCom(sOpenStatus,
                            PIF_COM_SEND_TIME,
                            (USHORT FAR *)&usTimerVal);
                        
    if (sStatus < 0) {
        return (PIF_ERROR_SYSTEM);                /* failed to set timers */
    }


    usTimerVal = ESCP_TIMER_RECV;
    sStatus = PifControlCom(sOpenStatus,
                            PIF_COM_SET_TIME,
                            (USHORT FAR *)&usTimerVal);
    if (sStatus < 0) {
        return (PIF_ERROR_SYSTEM);                /* failed to set receive timer */
    }

    usTimerVal = ESCP_TIMER_RECV;
    sStatus = PifControlCom(sOpenStatus,
                            PIF_COM_SET_TIME,
                            (USHORT FAR *)&usTimerVal);
    if (sStatus < 0) {
        return (PIF_ERROR_SYSTEM);                /* failed to set receive timer */
    }


    pEscp->sEscvCount=0;
    PifRequestSem(pEscp->sSem);                   /* task switch off */
    EscpFifoClear(&pEscp->Fifo);        /* Clear Sequence # FIFO */
    PifReleaseSem(pEscp->sSem);                   /* task switch on */
    pEscp->chOpen = TRUE;               /* Now, ready to serve   */
    /*return (PIF_OK);*/            /* return device handle  */
    return pEscp->sSio;
}

