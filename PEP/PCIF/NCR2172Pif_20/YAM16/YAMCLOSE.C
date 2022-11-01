/*
*===========================================================================
* Title       : Close Function
* Category    : YAM Driver
* Program Name: YAMCLOSE.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.0 by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /Asfw /W4 /G2 /Zp /BATCH /nologo   /f- /Os /Og /Oe /Gs
*               /DWINVER=0x0300 
*
* --------------------------------------------------------------------------
* Abstract:     This file closes Ymodem Driver.
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
extern SHORT   sAsyncWriteLength;
extern UCHAR   auchAsyncWriteBuffer[10];


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT YamClose(DEVICE *pDevTbl)
*     Input:    pDevTbl  - Device Table
*
** Return:      OK     - Successful
*               SYSERR - System Error
*
** Description: This function closes Ymodem Driver.
*===========================================================================
*/
SHORT YamClose(DEVICE *pDevTbl)
{

    if (!(YamData.fchStatus & YAM_OPEN)) {  /* Already Closed */
        return (SYSERR);
    }

    YamCleanupAsyncWrite();                 /* clean up Async Write */

    YamReadFuncReset();                     /* Reset Read Function Status */
    YamWriteFuncReset();                    /* Reset Write Function Status */
    
    YamData.fchStatus &= ~YAM_OPEN;         /* YAM Driver Open */

    pDevTbl = pDevTbl;

    /* Close SIO Driver */
    return (XinuClose(YamData.sHsio));

}

