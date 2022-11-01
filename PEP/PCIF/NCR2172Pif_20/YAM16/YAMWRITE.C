/*
*===========================================================================
* Title       : Write Function
* Category    : YAM Driver
* Program Name: YAMWRITE.C                                                      
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
* Nov-9-92 :        :H.Shibamoto: add XinuSleepm in YamWriteNullPathname                                    
* Nov-20-92:        :H.Shibamoto: change all subroutines               
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
** Synopsis:    SHORT YamWrite(DEVICE *pDevTbl, VOID *pWriteBuf,
*                              SHORT sWriteLen)
*     Input:    pDevTbl   - Device Table
*    Output:    pWriteBuf - Write Buffer 
*     Input:    sWriteLen - Write Buffer Length
*
** Return:      0 >              - Write Data Length
*               0 <              - Error
*                 SYSERR           - System Error
*                 TIMEOUT          - Timeout
*                 XINU_WRITE_ERROR - Write Error
*
** Description: This function transmits the message according to the
*               protocol of Ymodem.
*===========================================================================
*/
SHORT YamWrite(DEVICE *pDevTbl, VOID *pWriteBuf, SHORT sWriteLen)
{
    SHORT   sStatus;

    /* Check YAM Driver Status */
    if (!(YamData.fchStatus & YAM_OPEN)) {  /* Close YMODEM Driver */
        return (SYSERR);
    }
        
    /* Check Write Buffer Length */
    if (sWriteLen == 0) {
        return (0);
    }
    else if (sWriteLen < 0) {
        return (SYSERR);
    }

    sStatus = YamWriteSwitch(pWriteBuf, sWriteLen);

    return (sStatus);

    pDevTbl = pDevTbl;                      /* Just Refer pDevTbl */
}


/*
*===========================================================================
** Synopsis:    SHORT YamWriteSwitch(VOID *pWriteBuf, SHORT sWriteLen)
*    Output:    pWriteBuf - Write Buffer 
*     Input:    sWriteLen - Write Buffer Length
*
** Return:      0 >              - Write Data Length
*               0 <              - Error
*                 TIMEOUT          - Timeout
*                 XINU_WRITE_ERROR - Write Error
*
** Description: This function judges the transmitted message with Pathname,
*               Block, and Null Pathname.
*===========================================================================
*/
SHORT YamWriteSwitch(VOID *pWriteBuf, SHORT sWriteLen)
{
    SHORT   sStatus;

    YamData.usReceiveTimer = YAM_TIMER_WAIT_ACK;    /* Timer to wait ACK  */
    XinuControl(YamData.sHsio, YAM_SIO_FUNC_RECEIVE_TIMER,
                            (USHORT far *)&(YamData.usReceiveTimer), 0);

    YamData.usSendTimer  = YAM_TIMER_CTS;           /* Timer to check CTS */
    XinuControl(YamData.sHsio, YAM_SIO_FUNC_SEND_TIMER,
                           (USHORT far *)&(YamData.usSendTimer), 0);


    if (!(YamData.fchMessage & YAM_W_BLOCK_N )) {
        /*                                       */
        /* PHASE   --- read 'C' & send filename  */
        /*                                       */

        if((sStatus=YamWritePath(pWriteBuf, sWriteLen)) < 0 ){
            return(sStatus);
        }

        YamData.fchMessage |= YAM_W_BLOCK_N;    /* set PHASE flag */
        return(sStatus);

    }
    else {
        if(sWriteLen > DATA_LEN_PATHNAME) {
            /*                                                    */
            /* PHASE   --- send 1024 byte data  & receive ACK     */
            /*                                                    */

            return(YamWrite1Kblock(pWriteBuf, sWriteLen));

        }
        else {
    
            /*                                                    */
    	    /* PHASE   --- send EOT & receive ACK                 */
            /*             receive "C"                            */
            /*             send 128 bytes & receive ACK           */
            /*                                                    */
            /*                                                    */


            sStatus = YamWriteNullpath(pWriteBuf, sWriteLen);
            YamData.fchMessage &= ~YAM_W_BLOCK_N;    /* reset PHASE flag */
            return(sWriteLen);

        } /* end of else if(sWriteLen < DATA_LEN_PATHNAME)  */

    } /* end of else */


}  /* end of YamWriteSwitch */


/*
*===========================================================================
** Synopsis:    SHORT YamWritePath(VOID *pWriteBuf, SHORT sWriteLen);
*
** Return:      0 > status ... ERROR
*               0 <= status ... received data length
*
** Description: This function transmits the cancellation and initializes the
*               writer function status.
*===========================================================================
*/

SHORT YamWritePath(VOID *pWriteBuf, SHORT sWriteLen)
{

    SHORT   sStatus;

    /*                                       */
    /* PHASE   --- read 'C' & send filename  */
    /*                                       */


    /* Read 'C' */

    while( (sStatus = YamReadResponse()) != YAM_READ_C ){
        if (sStatus == YAM_READ_CAN) {      /* Read CAN CAN */
            YamWriteFuncReset();            /* Reset Write Function Status */
            return (XINU_ACCESS_DENIED);
        }
    	else if (sStatus == TIMEOUT) {          /* Receive Timeout */
            YamWriteFuncCancel();            /* Reset Write Function Status */
	        return (TIMEOUT);
 	    }
        else if (sStatus < 0) {                 /* Error */
            YamWriteFuncReset();            /* Reset Write Function Status */
        	return (sStatus);
        }
    } /* end of loop */

    /* reset Block No.  */
    YamData.uchBlockNo = 0;

    /* Write filename & receive ACK */

    for(;;){
        YamWriteMessage(YAM_SOH, pWriteBuf, sWriteLen);
        if( (sStatus = YamReadResponse()) == YAM_READ_ACK  ){
            break;
        }

        if (sStatus == YAM_READ_CAN) {     /* Read CAN CAN */
            YamWriteFuncReset();                /* Reset Write Function Status */
            return (XINU_ACCESS_DENIED);
        }
    
        else if ( sStatus == TIMEOUT ) {        /* Receive Timeout */
            YamWriteFuncCancel();               /* send CAN * 8 & reset flags */
            return (TIMEOUT);
        }

        else if (sStatus < 0) {                 /* Error */
            YamWriteFuncReset();                /* Reset Write Function Status */
            return (sStatus);
        }
    } /* end of loop */


    /* Read 'C' */
    while((sStatus = YamReadResponse())!=YAM_READ_C){
        if (sStatus == YAM_READ_CAN) {     /* Read CAN CAN */
            YamWriteFuncReset();                /* Reset Write Function Status */
            return (XINU_ACCESS_DENIED);
        }
        else if (sStatus == TIMEOUT) {          /* Receive Timeout */
            YamWriteFuncCancel();           /* Reset Write Function Status */
            return (TIMEOUT);
        }
        else if (sStatus < 0) {                 /* Error */
            return (sStatus);
        }
            
        /* if receive ACK , NAK then ignore */    
    }

    return(sWriteLen);             /* return successful status */

}


/*
*===========================================================================
** Synopsis:    SHORT YamWrite1Kblock(VOID *pWriteBuf, SHORT sWriteLen);
*
** Return:      0 > status ... ERROR
*               0 <= status ... received data length
*
** Description: This function transmits the cancellation and initializes the
*               writer function status.
*===========================================================================
*/

SHORT YamWrite1Kblock(VOID *pWriteBuf, SHORT sWriteLen) {
    /*                                                    */
    /* PHASE   --- send 1024 byte data  & receive ACK     */
    /*                                                    */

    SHORT   sStatus;

    /* inclease Block No.  */
    YamData.uchBlockNo++;

    /* send 1024 data */
    for(;;){
        YamClearBuffer();                           /* clear receive buffer */
        YamWriteMessage(YAM_STX, pWriteBuf, sWriteLen);     /* send data */
        if( (sStatus = YamReadResponse()) == YAM_READ_ACK  ){
            break;
        }
        
       	if (sStatus == YAM_READ_CAN) {     /* Read CAN CAN */
            YamWriteFuncReset();                /* Reset Write Function Status */
           	return (XINU_ACCESS_DENIED);
        }
        else if (sStatus == TIMEOUT) {          /* Receive Timeout */
            YamWriteFuncCancel();               /* Cancel Write Function Status */
            return (TIMEOUT);
        }
        else if (sStatus < 0) {                 /* Error */
   	        return (sStatus);
       	}
    
    } /* end of loop */

   	return (sWriteLen);


}


/*
*===========================================================================
** Synopsis:    SHORT YamWriteNullpath(VOID *pWriteBuf, SHORT sWriteLen);
*
** Return:      0 > status ... ERROR
*               0 <= status ... received data length
*
** Description: This function transmits the cancellation and initializes the
*               writer function status.
*===========================================================================
*/

SHORT YamWriteNullpath(VOID *pWriteBuf, SHORT sWriteLen){


    /*                                                    */
    /* PHASE   --- send EOT & receive ACK                 */
    /*             receive "C"                            */
    /*             send 128 bytes & receive ACK           */
    /*                                                    */
    /*                                                    */

    SHORT   sStatus;
    UCHAR   auchBuffer[5];


    /* send EOT & receive ACK */
    for(;;){
        YamClearBuffer();       /* clear receive buffer */
        auchBuffer[0] = YAM_EOT;
        XinuWrite(YamData.sHsio, auchBuffer, DATA_LEN_HEADER); 
        if((sStatus = YamReadResponse()) == YAM_READ_ACK ){
            break;
        }

        if (sStatus == YAM_READ_CAN) {      /* Read CAN CAN */
            YamWriteFuncReset();            /* Reset Write Function Status */
            return (XINU_ACCESS_DENIED);
   	    }
       	else if (sStatus == TIMEOUT) {          /* Receive Timeout */
       	    YamWriteFuncCancel();               /* Reset Write Function Status */
            return (TIMEOUT);
        }
       	else if (sStatus < 0) {                 /* Error */
            return (sStatus);
        }

    }  /* end of loop */


    /* receive C */

    while( ( sStatus = YamReadResponse() ) != YAM_READ_C ) {
        if (sStatus == YAM_READ_CAN) {     /* Read CAN CAN */
            YamWriteFuncReset();            /* Reset Write Function Status */
            return (XINU_ACCESS_DENIED);
   	    }
        else if (sStatus == TIMEOUT) {          /* Receive Timeout */
            YamWriteFuncCancel();            /* Reset Write Function Status */
            return (TIMEOUT);
       	}
        else if (sStatus < 0) {                 /* Error */
            return (sStatus);
        }

    }

    /* reset Block No.  */
    YamData.uchBlockNo = 0;

    /* send 128 bytes & receive ACK  */

    for(;;){
        YamWriteMessage(YAM_SOH, pWriteBuf, sWriteLen); 
        if((sStatus = YamReadResponse())==YAM_READ_ACK){
            break;
        }

        if (sStatus == YAM_READ_CAN) {     /* Read CAN CAN */
            YamWriteFuncReset();            /* Reset Write Function Status */
            return (XINU_ACCESS_DENIED);
   	    }
        else if (sStatus == TIMEOUT) {          /* Receive Timeout */
            YamWriteFuncCancel();            /* Reset Write Function Status */
            return (TIMEOUT);
        }
        else if (sStatus < 0) {                 /* Error */
            return (sStatus);
        }
    
	} /* end of loop */

   	return(sWriteLen);

}


/*
*===========================================================================
** Synopsis:    VOID YamWriteFuncCancel(VOID)
*
** Return:      XINU_WRITE_ERROR - Write Error
*
** Description: This function transmits the cancellation and initializes the
*               writer function status.
*===========================================================================
*/
VOID YamWriteFuncCancel(VOID)
{
    UCHAR   auchBuffer[8] = {YAM_CAN, YAM_CAN, YAM_CAN, YAM_CAN,
                             YAM_CAN, YAM_CAN, YAM_CAN, YAM_CAN};

    XinuWrite(YamData.sHsio, auchBuffer, sizeof(auchBuffer));

    YamWriteFuncReset();
    return;
}


/*
*===========================================================================
** Synopsis:    VOID YamWriteFuncReset(VOID)
*
** Return:      nothing
*
** Description: This function initializes the writer function status.
*===========================================================================
*/
VOID YamWriteFuncReset(VOID)
{
    YamClearBuffer();                   /* clear receive buffer */
    YamData.fchMessage &= ~YAM_W_BLOCK_N;
    YamData.uchBlockNo = 0;
    return;

}

