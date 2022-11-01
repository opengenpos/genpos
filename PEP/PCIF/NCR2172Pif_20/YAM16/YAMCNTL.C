/*
*===========================================================================
* Title       : I/O Control Function
* Category    : YAM Driver
* Program Name: YAMCNTL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.0 by Microsoft Corp.
* Memory Model: Compact Model
* Options     : /Asfw /W4 /G2 /Zp /BATCH /nologo   /f- /Os /Og /Oe /Gs
*               /DWINVER=0x0300 
* --------------------------------------------------------------------------
* Abstract:     This file offers I/O control fucntion of Ymodem driver.
*                 - The message is transmitted to the serial port without
*                   using the protocol.
*                 - The message is received to the serial port without
*                   using the protocol.
*                 - The receive timer is set.
* --------------------------------------------------------------------------
* Update Histories
*    Date  :Ver.Rev.:   Name    : Description
* Sep-16-92:00.00.01:O.Nakada   : Initial
* Nov-4-92 :        :H.Shibamoto: modify for PC                         
* Jun-22-98:        :O.Nakada   : Win32
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
** Synopsis:    SHORT YamCntl(DEVICE *pDevTbl, SHORT sFunction, ...)
*     Input:    pDevTbl   - Device Table
*               sFunction - I/O Control Function Code
*
** Return:      0 >     - Read or Write Date Length
*               OK      - Successful
*               0 <=    - Error
*                 SYSERR  - System Error
*                 TIMEOUT - Timeout
*
** Description: This function executes I/O control function.
*===========================================================================
*/
SHORT YamCntl(DEVICE *pDevTbl, SHORT sFunction, ...)
{
    SHORT sStatus;


    if (!(YamData.fchStatus & YAM_OPEN)) {  /* Close YAM Driver */
        return (SYSERR);
    }

    switch (sFunction) {
    case YAM_FUNC_READ:
        XinuControl(YamData.sHsio, YAM_SIO_FUNC_RECEIVE_TIMER,
                           (USHORT far *)&(YamData.usUserRecvTimer), 0);

#if	WIN32
        sStatus = XinuRead(YamData.sHsio,
                            *(VOID FAR **)((LONG *)&sFunction + 1),
                            *(USHORT *)((VOID FAR **)((LONG *)&sFunction + 1) + 1)); 
#else
        sStatus = XinuRead(YamData.sHsio,
                            *(VOID FAR **)(&sFunction + 1),
                            *(USHORT *)((VOID FAR **)(&sFunction + 1) + 1)); 
#endif
                

        
        return (sStatus);

    case YAM_FUNC_WRITE:
        XinuControl(YamData.sHsio, 
                    YAM_SIO_FUNC_SEND_TIMER,
                    (USHORT FAR *)&(YamData.usUserSendTimer), 0);

#if	WIN32
        sStatus = XinuWrite(YamData.sHsio,
                                 *(VOID FAR **)((LONG *)&sFunction + 1),
                                 *(USHORT *)((VOID FAR **)((LONG *)&sFunction + 1) + 1));
#else
        sStatus = XinuWrite(YamData.sHsio,
                                 *(VOID FAR **)(&sFunction + 1),
                                 *(USHORT *)((VOID FAR **)(&sFunction + 1) + 1));
#endif



        return (sStatus);

    case YAM_FUNC_USER_TIMER:
#if	WIN32
        YamData.usUserRecvTimer = *(USHORT *)((LONG *)&sFunction + 1);
#else
        YamData.usUserRecvTimer = *(USHORT *)(&sFunction + 1);
#endif
        return (OK);

    case YAM_FUNC_SEND_TIMER:
#if	WIN32
        YamData.usUserSendTimer = *(USHORT *)((LONG *)&sFunction + 1);
#else
        YamData.usUserSendTimer = *(USHORT *)(&sFunction + 1);
#endif
        return (OK);

    case YAM_FUNC_CANCEL:
        
        /* Check YAM Driver Status */
        if ((YamData.fchStatus & YAM_OPEN)) {  /* Close YAM Driver */
                YamWriteFuncReset();
                YamReadFuncCancel();
                return(OK);
        }
        else
            return(SYSERR);


    default:
        return (SYSERR);
    }

    pDevTbl = pDevTbl;                      /* Just Refer pDevTbl */
}


