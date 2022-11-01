/*
*===========================================================================
* Title       : Write Message Function
* Category    : YAM Driver
* Program Name: YAMWM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.0 by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /Asfw /W4 /G2 /Zp /BATCH /nologo   /f- /Os /Og /Oe /Gs
*               /DWINVER=0x0300 
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  :Ver.Rev.:   Name    : Description
* Sep-16-92:00.00.01:O.Nakada   : Initial
* Nov-4-92 :        :H.Shibamoto: modify for PC                         
* Nov-20-92:        :H.Shibamoto: change all subroutines               
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
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT YamWriteMessage(UCHAR uchHeader, VOID *pReadBuf,
*                                     SHORT sReadLen)
*     Input:    uchHeader - Message Header
*    Output:    pWriteBuf - Write Buffer 
*     Input:    sWriteLen - Write Buffer Length
*
** Return:      0 >              - Write Data Length
*               0 <              - Error
*                 SYSERR           - System Error
*                 TIMEOUT          - Timeout
*                 XINU_WRITE_ERROR - Write Error
*
** Description: This function transmits the message.
*===========================================================================
*/
SHORT YamWriteMessage(UCHAR uchHeader, VOID *pWriteBuf, SHORT sWriteLen)
{
    USHORT  usCrc = XinuCrc(pWriteBuf, sWriteLen, 0);
    UCHAR   auchBuffer[3];

        /* Write SOH or STX, BLK, -BLK */
        auchBuffer[0]       = uchHeader;
        auchBuffer[1]       = YamData.uchBlockNo;
        auchBuffer[2]       = (UCHAR)(0xFF - YamData.uchBlockNo);

        XinuWrite(YamData.sHsio, auchBuffer, 3);

        /* Write Message */
        XinuWrite(YamData.sHsio, pWriteBuf, (USHORT)sWriteLen);

        /* Write CRC */
        *(UCHAR FAR *)auchBuffer       = (UCHAR)(usCrc >> 8);   /* CRC High */
        *((UCHAR FAR *)auchBuffer + 1) = (UCHAR)usCrc;          /* CRC Low */
        XinuWrite(YamData.sHsio, auchBuffer, DATA_LEN_CRC);
        
        return(sWriteLen);

}

