/*
*===========================================================================
* Title       : Read Message Function
* Category    : YAM Driver
* Program Name: YAMRM.C
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
** Synopsis:    SHORT YamReadBlocktoCrc(VOID *pReadBuf, SHORT sLength, UCHAR *pusRcvBlockNum)
*     Input:    sReadLen  - Read Buffer Length
*               sLength   - Read Length
*
** Return:      Function Status
*               0 >             - Read Data Length
*               0 ==            - Continue
*               XINU_READ_ERROR - Read Error
*               XINU_MESSAGE_ERROR - Bloca No is invalid
*                                    *puchRcvBlockNum = received message number
*
** Description: This function receives the data from block No. to CRC.
*===========================================================================
*/
SHORT YamReadBlocktoCrc(VOID *pReadBuf, SHORT sLength, UCHAR *puchRcvBlockNum)
{
    SHORT   sStatus;
    UCHAR   auchBuffer[4];

    *puchRcvBlockNum = 0;

    /* Read Block No. */
    sStatus = YamReadAfterAsyncWrite(YamData.sHsio, auchBuffer, DATA_LEN_BNUMBER );

    if (sStatus == DATA_LEN_BNUMBER) {

        /* Read Message */
        sStatus = YamReadAfterAsyncWrite(YamData.sHsio, pReadBuf, sLength );
        if (sStatus == sLength) {
            /* Read CRC */
            sStatus = YamReadAfterAsyncWrite(YamData.sHsio, auchBuffer + DATA_LEN_BNUMBER , DATA_LEN_CRC );
            if (sStatus == DATA_LEN_CRC) {
                if( auchBuffer[0] == (UCHAR )(0xFF-auchBuffer[1]) ) {   /* if number is valid */
                    *puchRcvBlockNum = *auchBuffer;     /* set received block No.*/
                    /* Check Current Block No. */
                    if (auchBuffer[0] == YamData.uchBlockNo) {
                        if ((sStatus=YamCheckCrc(pReadBuf, sLength, auchBuffer + DATA_LEN_BNUMBER))==sLength){
                            /* successful */
                            ++YamData.uchBlockNo;   /* increase BLOCK no. */
                            return(sStatus);
                        }
                    }
                    /* Check Previous Block No. */
                    else if (auchBuffer[0] == (UCHAR)(YamData.uchBlockNo - 1)) {
                        return (XINU_ALREADY_EXIST);
                    }
                    /* Block No. Error */
                    else {
                        return (XINU_MESSAGE_ERROR);        /* return MESSAGE ERROR  */
                    }
                } /* end of if number is valid*/
            } /* end of if reading message is success. */
        } /* end of if reading message is success. */
    
    }
    return (XINU_READ_ERROR);     /* receive data length is not enough. */

}




/*
*===========================================================================
** Synopsis:    SHORT YamCheckCrc(VOID *pReadBuf, SHORT sLength, VOID *pCrc )
*    Output:    pReadBuf - Read Buffer 
*     Input:    sLength  - Read Buffer Length
*               pCrc     - CRC Data
*               sCurrent - Current or Previous Status
*
** Return:      Function Status
*               0 >             - Read Data Length
*               0 ==            - Continue
*               XINU_READ_ERROR - Read Error
*
** Description: This function calculates received the data and CRC.
*               An aknowledge or a negative response is transmitted.
*===========================================================================
*/
SHORT YamCheckCrc(VOID *pReadBuf, SHORT sLength, VOID *pCrc )
{
    USHORT  usCrc;

    usCrc = XinuCrc(pReadBuf, sLength, 0);
    usCrc = XinuCrc(pCrc, DATA_LEN_BNUMBER, usCrc);
    if (!usCrc) {                                                   /* CRC Successful */
            return (sLength);
    }
        
    return (0);                                     /* CRC Error */
}


