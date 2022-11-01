/*
*===========================================================================
* Title       : Open Function
* Category    : YAM Driver
* Program Name: YAMOPEN.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.0 by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /Asfw /W4 /G2 /Zp /BATCH /nologo   /f- /Os /Og /Oe /Gs
*               /DWINVER=0x0300 
* --------------------------------------------------------------------------
* Abstract:     This file opens Ymodem Driver.
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :Ver.Rev.:   Name    : Description
* Sep-16-92:00.00.01:O.Nakada   : Initial
* Nov-4-92 :        :H.Shibamoto: modify for PC                         
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
** Synopsis:    SHORT YamOpen(DEVICE *pDevTbl, ...)
*     Input:    pDevTbl      - Device Table
*
** Return:      0 >=   - Device Number
*               SYSERR - System Error
*
** Description: This file opens Ymodem Driver.
*===========================================================================
*/
SHORT YamOpen(DEVICE *pDevTbl, ...)
{
    SHORT       sStatus;
    COMCONTROL  tmpdata;
    YAMPORT     *userdata;
    UCHAR       auchCancelBuf[8] = {YAM_CAN, YAM_CAN, YAM_CAN, YAM_CAN,
                                 YAM_CAN, YAM_CAN, YAM_CAN, YAM_CAN};


    if (YamData.fchStatus & YAM_OPEN) {     /* Already Opened */
        return (SYSERR);
    }

    /*** parameter comvert ***/

    userdata =  *(YAMPORT FAR **)(&pDevTbl + 1);

    /* Port # */
    tmpdata.uchPort     = (UCHAR)userdata->usPort ;    
    /* BaudRate */
    tmpdata.usBaudRate  = userdata->usBaudRate;

    /* Byte Size of Data */
    if( ( userdata->uchByteFormat & BFMT_WLEN_MASK ) ==  BFMT_WLEN_8 ) {
        tmpdata.uchByteSize = CTRL_BYTESIZE_8;   /* Byte Size = 8 bit */
    }
    else {
        tmpdata.uchByteSize = CTRL_BYTESIZE_7;   /* Byte Size = 7 bit */
    }

    /* Parity Bits */
    switch(userdata->uchByteFormat & BFMT_PARITY_MASK){

        case BFMT_PARITY_ODD:
            tmpdata.uchParity = CTRL_PARITY_ODD; 
            break;

        case BFMT_PARITY_NONE:
            tmpdata.uchParity = CTRL_PARITY_NONE;
            break;

        case BFMT_PARITY_EVEN:
            tmpdata.uchParity = CTRL_PARITY_EVEN;
            break;

        default:
            tmpdata.uchParity = CTRL_PARITY_ZERO;
            break;

    }

     /* Stop Bits */
    if(userdata->uchByteFormat & BFMT_STOP_2 ) {
        tmpdata.uchStopBits = CTRL_STOPBIT_2;
    }
    else {
        tmpdata.uchStopBits = CTRL_STOPBIT_1;
    }


    /* Open Serial Port */
    sStatus = XinuOpen(COM, &tmpdata, sizeof(tmpdata) );
    if (sStatus < 0) {
        return (sStatus);
    }

    YamData.sHsio = sStatus;                    /* SIO Driver Handle */
    YamData.usPort = (UCHAR)userdata->usPort;   /* set port No. */

    /* receive timer initialize */
    YamData.usReceiveTimer = YAM_TIMER_WAIT_ACK;    /* Timer to wait ACK  */
    XinuControl(YamData.sHsio, YAM_SIO_FUNC_RECEIVE_TIMER,
                            (USHORT far *)&(YamData.usReceiveTimer), 0);


    /* send timer initialize */
    YamData.usSendTimer  = YAM_TIMER_CTS;              /* Timer to wait CTS */
    XinuControl(YamData.sHsio, YAM_SIO_FUNC_SEND_TIMER,
                           (USHORT far *)&(YamData.usSendTimer), 0);

    /* send CAN, CAN, CAN, CAN, CAN, CAN, CAN, CAN */   
    if((sStatus = XinuWrite(YamData.sHsio, auchCancelBuf, sizeof(auchCancelBuf) )) < 0 ){
        XinuClose(YamData.sHsio);
        return(XINU_NOT_PROVIDED);      /* not provided */
    }

    YamReadFuncReset();                     /* Reset Read Function Status  */
    YamWriteFuncReset();                    /* Reset Write Function Status */
    YamData.fchStatus |= YAM_OPEN;          /* YAM Driver Open */


    return (pDevTbl->dvnum);

}
