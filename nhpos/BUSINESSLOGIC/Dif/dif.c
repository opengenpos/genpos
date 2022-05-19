/*
******************************************************************************
**                                                                          **
**        *=*=*=*=*=*=*=*                                                   **
**        *  NCR 2170   *             NCR Corporation, E&M OISO             **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1996                **
**    <|\/~             ~\/|>                                               **
**   _/^\_               _/^\_                                              **
**                                                                          **
******************************************************************************
*=============================================================================
* Title       : Beverage Dispenser(TMI 6400) Interface Routines
* Category    : Beverage Dispenser Interface, NCR 2170 RESTRANT MODEL
* Program Name: DIF.C
* ----------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*               DifInit();
*               DifGetComPort();
*               DifMainReceiveTask();
*               DifCheckLRC();
*               DifCheckSeqNum();
*               DifEnableStatus();
*               DifEnableStatus2();
*               DifDisableStatus();
*               DifRewriteRingBuffer();
*               
*-----------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name       : Description
* Feb-14-96 : 03.01.00 : M.Ozawa      : Initial                     
* Apr-17-96 : 03.01.04 : M.Ozawa      : Modify for RS232C noise trouble
*=============================================================================
*=============================================================================
* PVCS Entry
* ----------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*=============================================================================
*/

#if defined(USE_2170_ORIGINAL)

/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
/**------- MS - C ------**/
#include <tchar.h>
#include <memory.h>

/**------- 2170 local------**/
#include "ecr.h"
#include "pif.h"
#include "appllog.h"
#include "termcfg.h"
#include "paraequ.h"
#include "uie.h"
#include "fsc.h"
#include "dif.h"
#include "diflocal.h"



/*
*===========================================================================
*   General Declarations
*===========================================================================
*/

UCHAR   uchDifEnableStatus;
CHAR    chDifPortID;
DIFMSG  DifMsgSave;
UCHAR   uchDifSeqNum;

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*=============================================================================
*
** Format  : VOID   DifInit(VOID);
*               
*    Input :    nothing
*
*   Output :    nothing
*    InOut :    nothing 
*
** Return  :    nothing
*
** Synopsis:    This function initializes Beverage Dispenser Interface.
*               This Creates semaphore, creates thread of receive,
*               and opens port for RS-232.
*=============================================================================
*/
VOID DifInit(VOID)
{
	/**     thread control  **/   
	UCHAR  CONST *aszDifRcvThreadName = "DIF_RCV";      /* receive name     */
	VOID (THREADENTRY *pFunc)(VOID) = DifMainReceiveTask;

	uchDifEnableStatus = 0;

    chDifPortID = (UCHAR)DifGetComPort();           /* get RS-232 port ID   */
    if (chDifPortID == DIF_PORT_NONE) {         
        return;
    }

    uchDifEnableStatus |= DIF_SERVICE_PROVIDED;
    uchDifSeqNum = 0;
    memset(&DifMsgSave, 0x00, sizeof(DifMsgSave));
    PifBeginThread(pFunc, 0, 0, PRIO_DIF_RCV, aszDifRcvThreadName);
}


/*                                  
*=============================================================================
** Format  : SHORT   DifGetComPort(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : 0 >=          ; Port ID
*            DIF_PORT_NONE ; nothing  
*
** Synopsis: This function search port id for Beverage Dispenser I/F
*=============================================================================
*/
SHORT   DifGetComPort(VOID)
{
    SYSCONFIG CONST FAR *pSys;
    USHORT i;

    pSys = PifSysConfig();                      /* get system configration  */

    if (!(pSys->uchCom & COM_PROVIDED)) {       /* exist RS-232 board ?     */
        return(DIF_PORT_NONE);
    }

    for (i=1; i < PIF_LEN_PORT; i++) {          /* get port ID              */
                                                /* auchComPort[0] = R/J     */
        if (pSys->auchComPort[i] == DEVICE_BEVERAGE) {

            return((SHORT)i);
        }
    }
    return(DIF_PORT_NONE);
}

/*
*=============================================================================
*
** Format  : VOID THREADENTRY   DifMainReceiveTask(VOID);
*               
*    Input :    nothing
*
*   Output :    nothing
*    InOut :    nothing 
*
** Return  :    nothing
*
** Synopsis:    This function start of recive task.
*
*=============================================================================
*/
VOID THREADENTRY DifMainReceiveTask(VOID)
{
    SHORT       sResult;
    SHORT       hsHandle;
    USHORT      usTimeout;
    FSCTBL      Ring;
    PROTOCOL    Protocol;
    DATE_TIME   DateTime;
    UCHAR       uchCtlCode;
    USHORT      usMin;
    USHORT      usSec;
    USHORT      usCount;
    USHORT      usAck;
    UCHAR       auchPrevData[5];    /* to retry */
    UCHAR       auchBuffer[sizeof(DIFREQDATA)];
    SYSCONFIG CONST FAR *pSysConf;


    pSysConf = PifSysConfig();

    /* --- initialization --- */

    Ring.uchMajor = FSC_BEVERAGE;
    Ring.uchMinor = 0;
    usTimeout     = DIF_WAIT_REQ_TIME;

    uchDifEnableStatus &= ~DIF_ENABLE_STATUS;

    memset(&DifMsgSave, 0x00, sizeof(DifMsgSave));

    /* --- set power down recovery status --- */
    uchDifEnableStatus |= DIF_POWER_DOWN;
    usCount = 2;

    for (;;) {                              /* endless loop */

        /* --- open serial port for 6400 --- */

        memset(&Protocol, 0, sizeof(Protocol));
        Protocol.fPip = PIF_COM_PROTOCOL_NON;                   /* protocol method */

#ifdef DIFDEBUG 
        Protocol.usComBaud = 2400;
        Protocol.uchComByteFormat = (COM_BYTE_EVEN_PARITY|COM_BYTE_2_STOP_BITS|COM_BYTE_7_BITS_DATA);
#else
        Protocol.usComBaud = pSysConf->ausComBaud[chDifPortID];                 /* baud rate */
        Protocol.uchComByteFormat = pSysConf->auchComByteFormat[chDifPortID];   /* even parity, 7 bit, 2 stop bit */ 
#endif

        hsHandle = PifOpenCom((USHORT)chDifPortID,
                                    (PROTOCOL FAR *)&Protocol);

        if (hsHandle < 0) {                     /* open error            */
            PifSleep(DIF_WAIT_PORT_TIME*1000);
            continue;                           /* retry ...             */
        }

        /* --- set receive timer --- */

        if (PifControlCom(hsHandle,
                          PIF_COM_SET_TIME,
                          (USHORT FAR *)&usTimeout) != PIF_OK) {
            PifCloseCom(hsHandle);              /* close port            */
            continue;                           /* re-open port          */
        }

        usAck = 0;
        memset(&auchPrevData, 0x00, sizeof(auchPrevData));

        for (;;) {

            /* --- receive dispenser data --- */

            sResult = PifReadCom(hsHandle,
                                 auchBuffer,
                                 sizeof(auchBuffer));

            if (sResult < 0) {                  /* error occured         */
                switch (sResult) {
                case PIF_ERROR_COM_TIMEOUT:     /* timeout               */
                    continue;

                case PIF_ERROR_COM_POWER_FAILURE: /* power down recovery */

                    /* --- set power down recovery status --- */
                    uchDifEnableStatus |= DIF_POWER_DOWN;
                    usCount = 2;
                    break;

                default:                        /* other error           */
                    break;

                }
                break;                          /* exit loop             */
            }

            if (auchBuffer[0] != DIF_STX) {     /* noise of 6400 power down */

                continue;
            }

            /* not proceed request message after power down recovery
               for RS232C noise affection                             */

            if (uchDifEnableStatus & DIF_POWER_DOWN) {

                if (usCount) {
                    usCount--;
                } else {
                    uchDifEnableStatus &= ~DIF_POWER_DOWN;  /* go to receive process */
                }
                /* not send response */
                continue;
            }

            /* Wait for Application Process */

            if ((uchDifEnableStatus & DIF_ENABLE_STATUS) == 0) {

                uchCtlCode = DIF_WACK;
                PifWriteCom(hsHandle, &uchCtlCode, sizeof(uchCtlCode));    /* send WACK */

                continue;                       /* next ...              */
            }

            memset(&DifMsgSave, 0x00, sizeof(DifMsgSave));

            PifGetDateTime(&DateTime);
            usMin = DateTime.usMin;
            usSec = DateTime.usSec;

            /* --- check read data form dispenser --- */

            /* LRC check */
            if (DifCheckLRC(auchBuffer,
                                sizeof(DIFREQDATA)) != DIF_SUCCESS) {

                uchCtlCode = DIF_NAK;
                PifWriteCom(hsHandle, &uchCtlCode, sizeof(uchCtlCode));    /* send NAK */

                PifLog(MODULE_DIF, LOG_ERROR_DIF_LRC_ERROR);

                continue;                       /* next ...              */
            }

            /* Sequence Number Check */
            if (DifCheckSeqNum((DIFMSG *)&auchBuffer[1]) != DIF_SUCCESS) {

                if ((memcmp(&auchPrevData, &auchBuffer[3], sizeof(auchPrevData)) == 0) &&
                    (usAck)) {
                    /* resend ack if same seq# and same plu# is received */

                    uchCtlCode = DIF_ACK;

                } else {

                    uchCtlCode = DIF_WACK;
                }
                PifWriteCom(hsHandle, &uchCtlCode, sizeof(uchCtlCode));    /* send NAK */

                continue;                       /* next ...              */
            }

            usAck = 0;  /* reset previous ack status */
            memset(&auchPrevData, 0x00, sizeof(auchPrevData));

            /* PLU Search */
            if (DifSearchData((DIFMSG *)&auchBuffer[1]) != DIF_SUCCESS) {

                /* Time Out check */
                PifGetDateTime(&DateTime);
                if ((SHORT)(DateTime.usSec - usSec) < 0) {
                    DateTime.usSec += 60;
                }
                if ((DateTime.usSec - usSec) < 4) {
                    uchCtlCode = DIF_WACK;
                    PifWriteCom(hsHandle, &uchCtlCode, sizeof(uchCtlCode));    /* send WACK */
                }
                continue;                       /* next ...              */
            }

            /* Time Out check */
            PifGetDateTime(&DateTime);
            if (DateTime.usMin != usMin) {
                DateTime.usSec += 60;
            }
            if ((DateTime.usSec - usSec) > 1) {
                if ((DateTime.usSec - usSec) < 4) {
                    uchCtlCode = DIF_WACK;
                    PifWriteCom(hsHandle, &uchCtlCode, sizeof(uchCtlCode));    /* send WACK */
                }
                continue;                       /* next ...              */
            }

            uchCtlCode = DIF_ACK;
            PifWriteCom(hsHandle, &uchCtlCode, sizeof(uchCtlCode));    /* send ACK */

            if (auchBuffer[1] == DIF_POUR_REQ_MSG) {
                memcpy(&DifMsgSave, &auchBuffer[1], sizeof(DifMsgSave));
                memcpy(&auchPrevData, &auchBuffer[3], sizeof(auchPrevData));
            } else {
                memset(&DifMsgSave, 0, sizeof(DifMsgSave));
            }
            uchDifSeqNum = auchBuffer[2];

            /* write to ring buffer */
            UieWriteRingBuf(Ring, (VOID *)&auchBuffer[1], sizeof(DIFMSG));

            uchDifEnableStatus &= ~DIF_ENABLE_STATUS;

            usAck = 1;  /* set previous ack status */
        }

        PifCloseCom(hsHandle);              /* close port            */

    }                                       /* endless loop          */
}


/*==========================================================================
**  Synopsis:   SHORT DifCheckLRC( UCHAR *DifRcvData,
*                                    USHORT usDataSize )
*
*   Input:      DIFRCVDATA  *DifRcvData
*               USHORT          usDataSize
*
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     
*
*   Description: Verify LRC of the received data from 6400
==========================================================================*/
SHORT DifCheckLRC( UCHAR *DifRcvData, USHORT usDataSize )
{

    UCHAR   uchLRC1 = 0, uchLRC2 = 0;
    USHORT  usRcvDataOff;

    /* search STX from response data */

    if (*DifRcvData != DIF_STX) {
        return(DIF_COM_ERR);
    }
    
    for (usRcvDataOff = 1; usRcvDataOff <= usDataSize; usRcvDataOff++) {

        if (usRcvDataOff == 1) {
            uchLRC1 = *(DifRcvData + usRcvDataOff);        /* set LRC check character */
        } else {
            uchLRC1 ^= *(DifRcvData + usRcvDataOff);     /* calculate LRC check character */
        }

        if (*(DifRcvData + usRcvDataOff) == DIF_ETX) {  /* get ETX data */
            break;
        }
    }

    /* check over run data */

    if (usRcvDataOff > usDataSize) {    /* over run case */
        return(DIF_COM_ERR);
    }

    /* check LRC */

    uchLRC2 = uchLRC1;
    uchLRC1 = (UCHAR)((uchLRC1 & 0xF0) >> 4);
    if (uchLRC1 < 0x0A) {
        uchLRC1 |= 0x30;
    } else {
        uchLRC1 += 0x37;
    }
    if (*(DifRcvData + usRcvDataOff + 1) != uchLRC1) {     /* LRC unmatch */
        return(DIF_COM_ERR);                                    
    }
    uchLRC2 = (UCHAR)(uchLRC2 & 0x0F);
    if (uchLRC2 < 0x0A) {
        uchLRC2 |= 0x30;
    } else {
        uchLRC2 += 0x37;
    }
    if (*(DifRcvData + usRcvDataOff + 2) != uchLRC2) {     /* LRC unmatch */
        return(DIF_COM_ERR);                                    
    }

    return(DIF_SUCCESS);            
}

/*==========================================================================
**  Synopsis:   SHORT DifCheckSeqNum( UCHAR *DifRcvData,
*                                    USHORT usDataSize )
*
*   Input:      DIFRCVDATA  *DifRcvData
*               USHORT          usDataSize
*
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     
*
*   Description: Verify LRC of the received data from 6400
==========================================================================*/
SHORT DifCheckSeqNum(DIFMSG *pDifMsg)
{

/****
    if (pDifMsg->uchSeqNum == DIF_INI_SEQ_NUM) {    / power on of 6400 /

        return(DIF_SUCCESS);
    }
****/

    if (uchDifSeqNum) {

        if (pDifMsg->uchSeqNum == uchDifSeqNum) {

            return(DIF_COM_ERR);
        }
    }

    return(DIF_SUCCESS);

}

/*==========================================================================
**  Synopsis:   SHORT DifEnableStatus( VOID)
*
*   Input:      nothing
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     
*
*   Description: start receiving message from 6400
==========================================================================*/

VOID DifEnableStatus( VOID)
{

    if (uchDifEnableStatus & DIF_SERVICE_PROVIDED) {

        memset(&DifMsgSave, 0x00, sizeof(DifMsgSave));

        uchDifEnableStatus |= DIF_ENABLE_STATUS;

    }
}

/*==========================================================================
**  Synopsis:   SHORT DifEnableStatus2( VOID)
*
*   Input:      nothing
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     
*
*   Description: start receiving message from 6400 by clear key
==========================================================================*/

VOID DifEnableStatus2( VOID)
{

    if (uchDifEnableStatus & DIF_SERVICE_PROVIDED) {

        if (DifMsgSave.uchSeqNum == 0) {

            uchDifEnableStatus |= DIF_ENABLE_STATUS;
        }
    }
}

/*==========================================================================
**  Synopsis:   SHORT DifDisableStatus( VOID)
*
*   Input:      nothing
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     
*
*   Description : stop receiving message from 6400
==========================================================================*/

SHORT DifDisableStatus( VOID)
{
    if ((uchDifEnableStatus & DIF_ENABLE_STATUS) == 0) {

        return(FALSE);
    }

    uchDifEnableStatus &= ~DIF_ENABLE_STATUS;

    return(TRUE);
}

/*==========================================================================
**  Synopsis:   SHORT DifRewriteRingBuffer( VOID)
*
*   Input:      nothing
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     
*
*   Description: rewrite 6400 message to ring buffer after clear key depression
==========================================================================*/

VOID DifRewriteRingBuffer( VOID)
{
    FSCTBL      Ring;

    if (uchDifEnableStatus & DIF_SERVICE_PROVIDED) {

        if (DifMsgSave.uchSeqNum) {

            if (DifMsgSave.uchMsg == DIF_POUR_REQ_MSG) {

                Ring.uchMajor = FSC_BEVERAGE;
                Ring.uchMinor = 0;

                /* write to ring buffer */
                UieWriteRingBuf(Ring, (VOID *)&DifMsgSave, sizeof(DIFMSG));
            }
        }
    }
}

/*==========================================================================
**  Synopsis:   SHORT DifClearSavedBuffer( VOID)
*
*   Input:      nothing
*   Output:     nothing
*   InOut:      nothing
*
*   Return:     
*
*   Description: Clear Request Message Data after Sales Process
==========================================================================*/

VOID DifClearSavedBuffer( VOID)
{

    if (uchDifEnableStatus & DIF_SERVICE_PROVIDED) {

        memset(&DifMsgSave, 0x00, sizeof(DifMsgSave));

    }
}

#endif
/* ====================================== */
/* ========== End of DIF.C ============== */
/* ====================================== */
