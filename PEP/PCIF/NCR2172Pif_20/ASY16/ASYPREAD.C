/*
*===========================================================================
* Title       : Read Function
* Category    : Asynchornous Driver for PC, NCR 2170 Operating System
* Program Name: ASYPREAD.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /Asfu /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  :Ver.Rev.:   Name    : Description
* Jul-28-92:00.00.01:O.Nakada   : Initial
* Jul-01-93:01.00.00:O.Nakada   : Modify off line check.
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
** Synopsis:    SHORT AsyRead(DEVICE *pDevTbl, VOID *pReadBuf,
*                             SHORT sReadLen)
*     Input:    pDevTbl  - Device Table
*    Output:    pReadBuf - Read Buffer 
*     Input:    sReadLen - Read Buffer Length
*
** Return:      Function Status
*                > 0             - Successful
*               ==               - Empty
*               TIMEOUT          - Timeout
*               XINU_READ_ERRROR - Read Error
*
** Description:
*===========================================================================
*/
SHORT AsyRead(DEVICE *pDevTbl, VOID *pReadBuf, SHORT sReadLen)
{
    SHORT   sStatus;
    USHORT  usLine;
    static  UCHAR auchBuffer[3] = {ASY_EOT, ASY_POL, ASY_ENQ};

    /* Check ASY Driver Status */
    if (!(AsyData.fchStatus & ASY_OPEN)) {
        return (SYSERR);
    }
        
    /* Check Read Buffer Length */
    if (sReadLen <= 0) {
        return (SYSERR);
    }

    /* check line status */
    usLine = (USHORT)XinuControl(AsyData.hsSio,
                                 ASY_SIO_FUNC_STATUS);
    if (!(usLine & ASY_SIO_STS_DSR)) {
        return (XINU_OFFLINE);              /* off line */
    }

    /* Flush Read Queue */
    XinuControl(AsyData.hsSio,
                ASY_SIO_FUNC_FLUSH_READ);

    /* Send Poll */
    if (XinuWrite(AsyData.hsSio, auchBuffer, 3) != 3) {
        return (XINU_READ_ERROR);
    }
    
    sStatus = AsyReadMessage(AsyData.hsSio, pReadBuf, sReadLen);

    return (sStatus);
        
    pDevTbl = pDevTbl;                      /* Just Refer pDevTbl */
}



/* ---------- End of ASYPREAD.C ---------- */

