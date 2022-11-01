/*
*===========================================================================
* Title       : Open Function
* Category    : Asynchornous Driver for PC, NCR 2170 Operating System
* Program Name: ASYPOPEN.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /Asfu /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*
*    Date    Ver.      Name         Description
* Jul-28-92  00.00.01  O.Nakada     Initial
* Dec-17-92  00.01.04  O.Nakada     Modify send timer calculation.
*
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


/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <conf.h>
#include <kernel.h>
#include "asy.h"


/*                                                                                
*===========================================================================
*   Compile Message Declarations
*===========================================================================
*/


/*                                                                                
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Public Work Area
*===========================================================================
*/


/*
*===========================================================================
*   Static Work Area 
*===========================================================================
*/


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT AsyOpen(DEVICE *pDevTbl, ...)
*     Input:    pDevTbl - Device Table
*
** Return:      OK                 - Successful
*               SYSERR             - System Error
*               XINU_ACCESS_DENIED - Access is Denied
*
** Description:
*===========================================================================
*/
SHORT AsyOpen(DEVICE *pDevTbl, ...)
{
    SHORT   sStatus;
    USHORT  usTimer;

    if (AsyData.fchStatus & ASY_OPEN) {     /* Already Opened */
        return (SYSERR);
    }

    /* Open Serial Port */
    sStatus = XinuOpen(COM,
                       *(COMCONTROL FAR **)(&pDevTbl + 1),
                       sizeof(COMCONTROL));
    if (sStatus < 0) {
        return (sStatus);
    }
    AsyData.hsSio = sStatus;                /* SIO Driver Handle */

    /* Set Send Timer */
    usTimer = (USHORT)(1000 *               /* 1 sec */
                       (1 + 8 + 1) /        /* data length */
                       (*(COMCONTROL FAR **)(&pDevTbl + 1))->usBaudRate *
                       512);                /* maximum message length */

    usTimer += 1000;                        /* minimum send timer */
    XinuControl(AsyData.hsSio,
                ASY_SIO_FUNC_SEND_TIMER,
                (USHORT FAR *)&usTimer,
                sizeof(usTimer));

    /* Set Source DLC Address */
    AsyData.uchDlcAddr =
        *(UCHAR FAR *)((COMCONTROL FAR **)(&pDevTbl + 1) + 1);  /* DLC Address */

    AsyData.fchStatus |= ASY_OPEN;          /* ASY Driver Open */

    return (pDevTbl->dvnum);
}
