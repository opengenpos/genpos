/*
*===========================================================================
* Title       : Init Function
* Category    : ESC/POS Printer Controler, NCR 2170 System S/W
* Program Name: ESCPINIT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /c /ACw /W4 /G1s /Os /Zap                                 
* --------------------------------------------------------------------------
* Abstract:     This contains the following function:
*
*           SHORT EscpInitCom(VOID);
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
**  ESC/POS I/O Control Block Instances
*/
ESCP    Escp[Nesc];            /* nEsc is the number of entries   */

/*
*===========================================================================
**  Synopsis:   SHORT EscpInitCom(DEVICE *pDevice)
*
*       pDevice     points the device table entry
*
**  Return:     OK
*
**  Description: This initializes the driver.
*===========================================================================
*/
SHORT EscpInitCom(VOID)
{
    ESCP *pEscp;

    pEscp = &Escp[0];
    pEscp->chOpen = FALSE;              /* not opened */
    pEscp->sEscvCount = 0;
    pEscp->sSem = PifCreateSem(1);
    
    return (PIF_OK);                        /* return device handle */
}


