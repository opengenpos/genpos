/*
*===========================================================================
* Title       : Read Function
* Category    : YAM Driver
* Program Name: YAMREAD.C                                                      
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

#include <memory.h>
#include <string.h>

/*
*===========================================================================
*   Public Work Area
*===========================================================================
*/

/* Async Write Length */
SHORT   sAsyncWriteLength = 0;
UCHAR   auchAsyncWriteBuffer[10];


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT YamRead(DEVICE *pDevTbl, VOID *pReadBuf,
*                             SHORT sReadLen)
*     Input:    pDevTbl  - Device Table
*    Output:    pReadBuf - Read Buffer 
*     Input:    sReadLen - Read Buffer Length
*
** Return:      0 >             - Read Data Length
*               0 <=            - Error
*                 SYSERR          - System Error
*                 TIMEOUT         - Timeout
*                 XINU_READ_ERROR - Read Error
*
** Description: This function receives the message according to the protocol
*               of Ymodem.
*===========================================================================
*/
SHORT YamRead(DEVICE *pDevTbl, VOID *pReadBuf, SHORT sReadLen)
{
    SHORT   sStatus;

    /* Check YAM Driver Status */
    if (!(YamData.fchStatus & YAM_OPEN)) {  /* Close YAM Driver */
        return (SYSERR);
    }
        

    sStatus = YamReadSwitch(pReadBuf, sReadLen);

    return (sStatus);

    pDevTbl = pDevTbl;                      /* Just Refer pDevTbl */
}


/*
*===========================================================================
** Synopsis:    SHORT YamReadSwitch(VOID *pReadBuf, SHORT sReadLen)
*    Output:    pReadBuf - Read Buffer 
*     Input:    sReadLen - Read Buffer Length
*
** Return:      Function Status
*               0 >             - Read Data Length
*               0 <=            - Error
*                 TIMEOUT         - Timeout
*                 XINU_READ_ERROR - Read Error
*
** Description: This function judges the received message with Pathname,
*               Block, and Null Pathname.
*===========================================================================
*/
SHORT YamReadSwitch(VOID *pReadBuf, SHORT sReadLen)
{
    SHORT   sStatus;
    UCHAR   auchBuffer[DATA_LEN_BLOCK];
    USHORT  usRetry;
    UCHAR   uchRcvBlockNum;

        YamData.usSendTimer  = YAM_TIMER_CTS;           /* Timer to wait CTS */
        XinuControl(YamData.sHsio, YAM_SIO_FUNC_SEND_TIMER,
                           (USHORT far *)&(YamData.usSendTimer), 0);


    if (!(YamData.fchMessage & YAM_R_BLOCK_N )) {

        if( sReadLen != DATA_LEN_PATHNAME  ){
            YamReadFuncCancel();
            return(XINU_READ_ERROR);
        }

        /*                                       */
        /* PHASE   --- send 'C' & read filename  */
        /*             & send ACK                */

        /* read Pathname ( with Retry )*/
        YamData.uchBlockNo = 0;
        if((sStatus = YamReadPath(pReadBuf, sReadLen )) != sReadLen ) {
            return(sStatus);
        }

        YamCleanupAsyncWrite();           /* clean up Async Write */
        XinuSleepm(YAM_TIMER_SEND_C);     /* wait 1sec */

        YamData.fchMessage |= YAM_R_BLOCK_N;    /* set PHASE flag */
        return(sStatus);

    }
    else {
        /* (YamData.fchMessage & YAM_R_BLOCK_N ) */

        for(usRetry=0;usRetry < YAM_RETRY; usRetry++){    

            /* Waiting BLOCK 1 , then send "C" */
            if( YamData.uchBlockNo == BLOCK_NUM_1 )  {

                auchBuffer[0] = YAM_C;
                if( ( sStatus =YamAsyncWrite(YamData.sHsio, auchBuffer, DATA_LEN_HEADER ) ) != DATA_LEN_HEADER) {
                    YamReadFuncCancel();
                    return (sStatus);
                }
                /* Timer to wait 1st BLOCK */
                YamData.usReceiveTimer = YAM_TIMER_WAIT_1ST;
                XinuControl(YamData.sHsio, YAM_SIO_FUNC_RECEIVE_TIMER,
                                    (USHORT far *)&(YamData.usReceiveTimer), 0);

            }

            if((sStatus = YamReadAfterAsyncWrite(YamData.sHsio,
                                                 auchBuffer,
                                                 DATA_LEN_1BYTE)) != DATA_LEN_1BYTE ){

                YamClearBuffer();                   /* clear receive buffer */
                if( YamData.uchBlockNo != BLOCK_NUM_1 ){
                    auchBuffer[0] = YAM_NAK;
                    if (YamAsyncWrite(YamData.sHsio,
                                      auchBuffer,
                                      DATA_LEN_HEADER) != DATA_LEN_HEADER) {
                        YamReadFuncCancel();
                        return(XINU_READ_ERROR);
                    }
                    YamData.usReceiveTimer = YAM_TIMER_NEXTBLOCK;    /* Timer to wait block */
                    XinuControl(YamData.sHsio, YAM_SIO_FUNC_RECEIVE_TIMER,
                                (USHORT far *)&(YamData.usReceiveTimer), 0);
                }
                continue;
            }

            if ( auchBuffer[0] == YAM_SOH) {                          /* Read SOH */


                /* dummy Read Message */
                sStatus = YamReadBlocktoCrc(auchBuffer, DATA_LEN_PATHNAME, &uchRcvBlockNum); 

                /* Dummy ACK */
                auchBuffer[0] = YAM_ACK;
                if ( ( sStatus =YamAsyncWrite(YamData.sHsio, 
                                              auchBuffer,
                                              DATA_LEN_HEADER ) ) != DATA_LEN_HEADER) {
                    YamReadFuncCancel();
                    return (sStatus);
                }

                YamData.usReceiveTimer = YAM_TIMER_NEXTBLOCK;    /* Timer to wait block */
                            XinuControl(YamData.sHsio, YAM_SIO_FUNC_RECEIVE_TIMER,
                               (USHORT far *)&(YamData.usReceiveTimer), 0);

                continue;   /* read again */
            }


            if ( auchBuffer[0] == YAM_CAN) {                          /* Read CAN */
                
                if((sStatus = YamReadAfterAsyncWrite(YamData.sHsio,
                                                     auchBuffer,
                                                     DATA_LEN_1BYTE )) != DATA_LEN_1BYTE ){
                    continue;
                    
                }
                
                if ( auchBuffer[0] == YAM_CAN) {    /* Read CAN     */
                    YamReadFuncReset();
                    return(XINU_ACCESS_DENIED);
                }
                continue;                       /* then , retry */

            }

            if (auchBuffer[0] == YAM_STX) {

               	/*                                                    */
                /* PHASE   --- receive 1024 byte data  & send ACK     */
               	/*                                                    */


                if((sStatus = YamRead1KblockWoHdr( pReadBuf, sReadLen))
                                        != sReadLen ){

                    YamClearBuffer();                   /* clear receive buffer */
                    if (sStatus == XINU_MESSAGE_ERROR) {    /* block No is invalid */
                        YamReadFuncCancel();
                        return (sStatus);
                    }

                    if (sStatus == XINU_ALREADY_EXIST) {    /* block No is old */
                    
                        auchBuffer[0] = YAM_ACK;
                        if ((sStatus = YamAsyncWrite(YamData.sHsio,
                                          auchBuffer,
                                          DATA_LEN_HEADER)) != DATA_LEN_HEADER) {
                            YamReadFuncCancel();
                            return (sStatus);
                        }
                        continue;           /* read again */       

                    }




                    if (YamData.uchBlockNo != BLOCK_NUM_1) {
                        auchBuffer[0] = YAM_NAK;
                        if (YamAsyncWrite(YamData.sHsio,
                                          auchBuffer,
                                          DATA_LEN_HEADER) != DATA_LEN_HEADER) {
                            YamReadFuncCancel();
                            return(XINU_READ_ERROR);
                        }
    
                        YamData.usReceiveTimer = YAM_TIMER_NEXTBLOCK;    /* Timer to wait C */
                        XinuControl(YamData.sHsio, YAM_SIO_FUNC_RECEIVE_TIMER,
                                    (USHORT far *)&(YamData.usReceiveTimer), 0);
                    } 
                    continue;
                }
                else{
                    return(sStatus);
                }


            }   /* end of PHASE */

            else if (auchBuffer[0] == YAM_EOT) {

                /* PHASE   --- (receive EOT)                        */
                /*             send ACK                             */
                /*             send "C"                             */
                /*             receive 128 bytes & send ACK         */
                /*                                                  */
                /*                                                  */


                auchBuffer[0] = YAM_ACK;
                if ( ( sStatus =YamAsyncWrite(  YamData.sHsio,
                                    auchBuffer, 
                                    DATA_LEN_HEADER ) ) != DATA_LEN_HEADER) {
                YamReadFuncCancel();
                    return (sStatus);
                }

                YamCleanupAsyncWrite();           /* clean up Async Write */
                XinuSleepm(YAM_TIMER_SEND_C);     /* wait 1sec */

                /* read Pathname ( with Retry )*/
                YamData.uchBlockNo = 0;
                if( ( sStatus = YamReadPath(pReadBuf, DATA_LEN_PATHNAME )) 
                                                    != DATA_LEN_PATHNAME ) {
                    return(sStatus);
                }

                YamData.fchMessage &= ~YAM_R_BLOCK_N;    /* reset PHASE flag */
                return(sStatus);

            } /* end of PHASE */


            YamClearBuffer();                   /* clear receive buffer */

        } /* end of loop */
        
        YamReadFuncCancel();
        return(XINU_READ_ERROR);



    }  /*   end of else if (!(YamData.fchMessage & YAM_R_BLOCK_N )) */
    


}  /* end of YamReadSwitch */


/*
*===========================================================================
** Synopsis:    SHORT YamReadPath(VOID *pReadBuf, SHORT sReadLen)
*
** Return:      value >=0 ... receive length
*               value < 0 ... error
*
** Description: 
*           PHASE   --- 1) send 'C' & read filename with header
*                       2) send ACK
*
*===========================================================================
*/


SHORT YamReadPath(VOID *pReadBuf, SHORT sReadLen)
{


    SHORT   sStatus;
    UCHAR   auchBuffer[DATA_LEN_BLOCK];
    UCHAR   uchRcvBlockNum;
    USHORT  usRetry;

    /* Timer to wait C */
    YamData.usReceiveTimer = YAM_TIMER_WAIT_1ST;
    XinuControl(YamData.sHsio, YAM_SIO_FUNC_RECEIVE_TIMER,
                    (USHORT far *)&(YamData.usReceiveTimer), 0);

    for(usRetry=0; usRetry < YAM_RETRY; usRetry++){

        YamClearBuffer();           /* clear receive buffer */

        auchBuffer[0] = YAM_C;
        if ( ( sStatus = YamAsyncWrite(YamData.sHsio, auchBuffer, DATA_LEN_HEADER ) ) 
                                                                    != DATA_LEN_HEADER) {
            YamReadFuncCancel();
            return (sStatus);
        }

        /* Read SOH */

        if( ( sStatus = YamReadAfterAsyncWrite(YamData.sHsio,
                                           auchBuffer,
                                           DATA_LEN_HEADER) ) != DATA_LEN_HEADER){
            continue;       /* then retry */
        } 

        if ( auchBuffer[0] == YAM_CAN) {                          /* Read CAN */
            if((sStatus = YamReadAfterAsyncWrite(YamData.sHsio,
                                             auchBuffer,
                                             DATA_LEN_HEADER )) != DATA_LEN_HEADER ){
                continue;       /* then retry */
            }
            if ( auchBuffer[0] == YAM_CAN) {                          /* Read CAN */
                YamReadFuncReset();
                return(XINU_ACCESS_DENIED);
            }
        }

            
        if( auchBuffer[0] == YAM_SOH  ) {
            if((sStatus = YamReadBlocktoCrc(pReadBuf, sReadLen, &uchRcvBlockNum)) != sReadLen ){         /* Read Message */
                /* block read is failure */
                if(sStatus == XINU_MESSAGE_ERROR){
                    YamReadFuncCancel();
                    return(XINU_MESSAGE_ERROR);
                 }
                else{
                    /* read again */
                    continue;
                } 
            }
            else {  
                /* block read is successful */
                auchBuffer[0] = YAM_ACK;
                if ((sStatus = YamAsyncWrite(YamData.sHsio,
                                  auchBuffer,
                                  DATA_LEN_HEADER)) != DATA_LEN_HEADER) {
                    YamReadFuncCancel();
                    return (sStatus);
                }
                return(sReadLen);
            }

        } /* end of else if receive SOH */
        
        /* if receive EOT then ignore */

    } /* end of loop */

    /* retry over */
    YamReadFuncCancel();
    return(XINU_READ_ERROR);

}

/*
*===========================================================================
** Synopsis:    SHORT YamRead1KblockWoHdr(VOID *pReadBuf, SHORT sReadLen, UCHAR *puchRetryStatus)
*
** Return:      value >=0 ... receive length
*               value < 0 ... error
*
** Description:
*           PHASE   --- 1) receive 1024 byte data
*                       2) send ACK
*
*===========================================================================
*/

SHORT YamRead1KblockWoHdr(VOID *pReadBuf, SHORT sReadLen)
{


    SHORT   sStatus;
    UCHAR   auchBuffer[DATA_LEN_BLOCK];
    UCHAR   uchRcvBlockNum;


    /*                                                    */
    /* PHASE   --- receive 1024 byte data  & send ACK     */
    /*                                                    */

    if( (sStatus = YamReadBlocktoCrc(pReadBuf, sReadLen, &uchRcvBlockNum ))!= sReadLen ) {  
        if(sStatus == XINU_MESSAGE_ERROR) {
            YamReadFuncCancel();
            return (sStatus);
        }
        else{   /* other error */
            return(sStatus);
        }
    }
    else { 
        /* block read is successful */
        auchBuffer[0] = YAM_ACK;
        if ((sStatus = YamAsyncWrite(  YamData.sHsio,
                        auchBuffer,
                        DATA_LEN_HEADER)) != DATA_LEN_HEADER) {
            YamReadFuncCancel();
            return (sStatus);
        }

        YamData.usReceiveTimer = YAM_TIMER_NEXTBLOCK;    /* Timer to wait block */
        XinuControl(YamData.sHsio, YAM_SIO_FUNC_RECEIVE_TIMER,
                       (USHORT far *)&(YamData.usReceiveTimer), 0);

        return(sReadLen);

    } /* end of else */

}

/*
*===========================================================================
** Synopsis:    VOID YamReadFuncCancel(VOID)
*
** Return:      value >=0 ... receive length
*               value < 0 ... error
*
** Description: This function transmits the cancellation and initializes the
*               reader function status.
*===========================================================================
*/
VOID YamReadFuncCancel(VOID)
{
    UCHAR   auchBuffer[8] = {YAM_CAN, YAM_CAN, YAM_CAN, YAM_CAN,
                             YAM_CAN, YAM_CAN, YAM_CAN, YAM_CAN};

    YamAsyncWrite(YamData.sHsio, auchBuffer, sizeof(auchBuffer));
    YamReadFuncReset();
    return;
}


/*
*===========================================================================
** Synopsis:    VOID YamReadFuncReset(VOID)
*
** Return:      nothing
*
** Description: This function initializes the reader function status.
*===========================================================================
*/
VOID YamReadFuncReset(VOID)
{
    YamClearBuffer();                   /* clear receive buffer */

    YamData.fchMessage &= ~YAM_R_BLOCK_N;
    YamData.uchBlockNo = 0;

    return;

}

/*
*===========================================================================
** Synopsis:    VOID YamClearBuffer(VOID)
*
** Return:      nothing
*
** Description: This function clears receive buffer in WINDOWS Driver.
*===========================================================================
*/
VOID YamClearBuffer(VOID)
{
    UCHAR   auchBuffer[DATA_LEN_PATHNAME];
    SHORT   sStatus;
    USHORT  usTmpTimer = YAM_TIMER_SKIP;
    XinuControl(YamData.sHsio, YAM_SIO_FUNC_RECEIVE_TIMER,
                           (USHORT far *)&usTmpTimer, 0);
    do {
          sStatus = YamReadAfterAsyncWrite(YamData.sHsio, auchBuffer, sizeof(auchBuffer));
    } while (!((sStatus == TIMEOUT) ||
             (sStatus >= 0 && sStatus != sizeof(auchBuffer))));
    XinuControl(YamData.sHsio, YAM_SIO_FUNC_RECEIVE_TIMER,
                        (USHORT FAR *)&(YamData.usReceiveTimer), 0);
    return;

}



/*
*===========================================================================
** Synopsis:    SHORT YamAsyncWrite(SHORT dev, VOID *buff, SHORT count)
*
** Return:      nothing
*
** Description: This function is used as XinuWrite
*===========================================================================
*/

SHORT   YamAsyncWrite(SHORT dev, VOID *buff, SHORT count)
{
    


    if(sAsyncWriteLength != 0){
        XinuWrite(dev, auchAsyncWriteBuffer, sAsyncWriteLength);
    }


    if( count > sizeof(auchAsyncWriteBuffer) ){
        return(XINU_WRITE_ERROR);
    }

    sAsyncWriteLength = count;


    memcpy(auchAsyncWriteBuffer, buff, count);


    return(count);

}



/*
*===========================================================================
** Synopsis:    SHORT YamReadAfterAsyncWrite(SHORT dev, VOID *buffer, SHORT numchars)
*
** Return:      nothing
*
** Description: This function is used as XinuRead
*===========================================================================
*/
SHORT   YamReadAfterAsyncWrite(SHORT dev, VOID *buffer, SHORT numchars)
{


    if(sAsyncWriteLength != 0){
        XinuWrite(dev, auchAsyncWriteBuffer, sAsyncWriteLength);
        sAsyncWriteLength = 0;
    }

    return(XinuRead(dev, buffer, numchars));


}

/*
*===========================================================================
** Synopsis:    VOID YamCleanupAsyncWrite();
*
** Return:      nothing
*
** Description: This function send the data in Async Buffer if it exists.
*===========================================================================
*/
VOID    YamCleanupAsyncWrite()
{
    if(sAsyncWriteLength != 0){
        XinuWrite(YamData.sHsio, auchAsyncWriteBuffer, sAsyncWriteLength);
        sAsyncWriteLength = 0;
    }
    return;
}



