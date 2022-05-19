/*
*===========================================================================
* Title       : Close Function
* Category    : ESC/POS Printer Controler, NCR 2170 System S/W
* Program Name: ESCPCLOS.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /c /ACw /W4 /G1s /Os /Zap                                 
* --------------------------------------------------------------------------
* Abstract:     This contains the following function:
*
*           SHORT EscpCloseCom(VOID);
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
**  Synopsis:   SHORT EscpCloseCom(SHORT sDummy)
*
**  Return:     PIF_OK              - Successfully completed
*               PIF_ERROR_SYSTEM    - Already Closed
*
**  Description: This stops the service.
*===========================================================================
*/
SHORT EscpCloseCom(SHORT sDummy)
{
    ESCP *pEscp;

    pEscp = &Escp[0];
    if (!pEscp->chOpen) {               /* see if already closed */
        return (PIF_ERROR_SYSTEM);
    }

    PifCloseCom(pEscp->sSio);             /* close the serial I/O port */

    pEscp->chOpen = FALSE;              /* Now, stop service */
    return (PIF_OK);
    sDummy = sDummy;
}

