/*
*===========================================================================
* Title       : Initialize Function
* Category    : YAM Driver
* Program Name: YAMINIT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.0 by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /Asfw /W4 /G2 /Zp /BATCH /nologo   /f- /Os /Og /Oe /Gs
*               /DWINVER=0x0300 
* --------------------------------------------------------------------------
* Abstract:     This file initializes Ymodem Driver.
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :Ver.Rev.:   Name    : Description
* Sep-16-92:00.00.01:O.Nakada   : Initial
* Nov-4-92 :        :H.Shibamoto: modify for PC                         
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


/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <conf.h>
#include <kernel.h>
#include "yam.h"


/*
*===========================================================================
*   Public Work Area
*===========================================================================
*/
YAMDATA NEAR YamData;


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT YamInit(DEVICE *pDevTbl)
*     Input:    pDevTbl  - Device Table
*
** Return:      OK - Successful
*
** Description: This function initializes Ymodem Driver.
*===========================================================================
*/
SHORT YamInit(DEVICE *pDevTbl)
{

    /* Set I/O Block of Device Table */
    pDevTbl->dvioblk = &YamData;

    return (OK);
}
