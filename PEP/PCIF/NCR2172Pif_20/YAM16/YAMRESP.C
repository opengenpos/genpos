/*
*===========================================================================
* Title       : Response Function
* Category    : YAM Driver
* Program Name: YAMRESP.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.0 by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /Asfw /W4 /G2 /Zp /BATCH /nologo   /f- /Os /Og /Oe /Gs
*               /DWINVER=0x0300 
* --------------------------------------------------------------------------
* Abstract:     As for this file, the following function necessary to
*               receive the response is defined.
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

/**===========================================================================
** Synopsis:    SHORT YamReadResponse(VOID)
*
** Return:      0 >            - Read Character
*                 YAM_READ_C     - Read 'C'
*                 YAM_READ_ACK   - Read ACK
*                 YAM_READ_NAK   - Read NAK
*                 YAM_READ_CAN   - Read CAN CAN
*               0 <            - Error
*                 TIMEOUT        - Timeout
*
** Description: This function receives the response.
*               if illegal character is received, this routine read again.
*===========================================================================
*/
SHORT YamReadResponse(VOID)
{
    SHORT   sStatus;
    UCHAR   auchBuffer[8];
    
    for(;;){
        sStatus = XinuRead(YamData.sHsio, auchBuffer, 1);
        if (sStatus == 1) {                         /* Read Successful */
            switch (*auchBuffer) {
            case YAM_C:                             /* Read 'C' */
                return (YAM_READ_C);
    
            case YAM_ACK:                           /* Read 'ACK' */
                return (YAM_READ_ACK);
    
            case YAM_NAK:                           /* Read 'NAK' */
                return (YAM_READ_NAK);
    
            case YAM_CAN:                           /* Read 'CAN' */
                sStatus = XinuRead(YamData.sHsio,auchBuffer, 1);
                if (sStatus == 1) {                 /* Read Successful */
                    if (*auchBuffer == YAM_CAN) {   /* Read 'CAN' */
                        return (YAM_READ_CAN);
                    }
                    else {
                        continue;
                    }
                }
                else if (sStatus == TIMEOUT) {      /* Timeout */
                    return (TIMEOUT);
                }
                else {                              /* Error */
                    return (sStatus);
                }

                /* if read another character,  then read again */

            } /* end of switch */

        }
        else if (sStatus == TIMEOUT) {              /* Timeout */
            return (TIMEOUT);
        }
        else {                                      /* Error */
            return (sStatus);
        }
    
    }   /* end of loop */

}


