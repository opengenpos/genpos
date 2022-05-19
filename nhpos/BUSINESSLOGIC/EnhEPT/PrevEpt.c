/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-2000      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : EPT MODULE
:   Category       : EPT MODULE, NCR 2170 US Hospitality Application
:   Program Name   : EPT.C (EPT function)
:  ---------------------------------------------------------------------  
:   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
:   Memory Model       : Medium Model                                     
:   Options            : /c /AM /W4 /G1s /Os /Za /Zp                       
:  ---------------------------------------------------------------------  
:  Abstract:
:   EPTConfirmMessage() ; send/receive EPT request/response data from/to EPT terminal
:   EPTGetPort()        ; get RS-232C port of EPT
:   EPTOpenSetPort()    ; set send/receive control comm. parameter
:   EPTConfirmCtlData() ; send and wait control code from/to EPT terminal 
:   EPTConfirmReqData() ; send EPT req. data and wait ACK data to/from EPT terminal
:   EPTRcvRspData()     ; receive EPT response data from EPT terminal
:   EPTClosePort()      ; close target RS-232C port
:  
:  ---------------------------------------------------------------------  
:  Update Histories                                                         
:   Date   : Ver.Rev.  : Name      : Description
: Nov-16-93: 00.00.01  : K.You     : initial                                   
:
:** NCR2171 **                                         
: Aug-26-99: 01.00.00  : Mikio T   : initial (for 2171)
: Jan-19-00: 01.00.00  : hrkato    : Saratoga
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
#include	<tchar.h>

#include    <string.h>
#include    <stdio.h>

#include    "ecr.h"
#include    "uie.h"
#include    "pif.h"
#include    "plu.h"
#include    "appllog.h"
#include    "regstrct.h"
#include    "uireg.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "pararam.h"
#include    "csstbpar.h"
#include    "prevept.h"
#include    "rfl.h"

/* define static save area */
CHAR    chEPTPortID;        /* ID save area for EPT port */
SHORT   hsEPTPort;          /* handle save area for EPT port */

/*
*==========================================================================
**  Synopsis:   SHORT EPTConfirmMessage( EPTREQDATA *EPTReqData,
*                                        EPTRSPDATA *EPTRcvData )
*   Input:      EPTREQDATA  *EPTReqData
*               EPTRSPDATA  *EPTRcvData
*   Output:     nothing
*   InOut:      nothing
*   Return:     
*
*   Description: send/receive EPT request/response data from/to EPT terminal 
*==========================================================================
*/
SHORT   EPTConfirmMessage(EPTREQDATA *EPTReqData, EPTRSPDATA *EPTRcvData)
{
    SHORT   sStatus;

    /* get RS-232C port */
    if (EPTGetPort() == EPT_NOT_EPTPORT) {
        return(LDT_PROHBT_ADR);                     /* change at V2.05.01   */
    }

    /* open/set target RS-232C port */
    if (EPTOpenSetPort() == EPT_COM_ERR) {
        EPTClosePort(hsEPTPort);
        return(LDT_EPT_OFFLINE_ADR);
    }

    /* send ENQ and wait ACK to/from EPT */
    if (EPTConfirmCtlData(EPT_ENQ) == EPT_COM_ERR) {
        EPTClosePort(hsEPTPort);
        return(LDT_EPT_OFFLINE_ADR);
    }

    /* send req. data and wait ACK data from/to EPT */

    if (EPTConfirmReqData(EPTReqData, sizeof(EPTREQDATA)) == EPT_COM_ERR) {
        EPTClosePort(hsEPTPort);

        /* print error message to journal */
        ItemTendCPPrintErr(LOG_EVENT_EPT_SND, LDT_EPT_OFFLINE_ADR);
        return(LDT_EPT_OFFLINE_ADR);
    }

    /* receive response data from EPT */
    UieSetAbort(UIE_POS3);                  /* set abort key enable for reg. position */
    sStatus = EPTRcvRspData(EPTRcvData, sizeof(EPTRSPDATA));
    UieSetAbort(UIE_POS3 | UIE_POS4 | UIE_POS5);       /* set abort key enable for super/prog. position */

    if ((sStatus == EPT_COM_ERR) || (sStatus == EPT_ABORTED)) {
        EPTClosePort(hsEPTPort);

        if (sStatus == EPT_COM_ERR) {
            sStatus = LDT_EPT_OFFLINE_ADR;
        } else {
            sStatus = LDT_CANCEL_ADR;
        }

        /* print error message to journal */
        ItemTendCPPrintErr(LOG_EVENT_EPT_RCV, sStatus);
		{
			char  xBuff[128];
			sprintf (xBuff, "EPTConfirmMessage(): LOG_EVENT_EPT_RCV sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == EPT_SUCCESS), xBuff);
		}
        return(sStatus);
    }

    /* send ACK and wait EOT from/to EPT */
    if (EPTConfirmCtlData(EPT_ACK) == EPT_COM_ERR) {
        EPTClosePort(hsEPTPort);
        return(LDT_EPT_OFFLINE_ADR);
    }

    /* close target RS-232C port */
    EPTClosePort(hsEPTPort);
    return(EPT_SUCCESS);
}                                    

/*
*==========================================================================
**  Synopsis:   SHORT EPTGetPort( VOID )
*
*   Input:      nothing
*   Output:     nothing
*   InOut:      nothing
*   Return:     
*
*   Description:    get RS-232C port of EPT 
*==========================================================================
*/
SHORT   EPTGetPort(VOID)
{
    SYSCONFIG CONST FAR *pSysConf;
    UCHAR               i;

    pSysConf = PifSysConfig();
    if (!(pSysConf->uchCom & COM_PROVIDED)) {   /* not exist RS-232C board */
        return(EPT_NOT_EPTPORT);
    }
    for (i = 0; i < PIF_LEN_PORT; i++) {        /* get port ID */
        if (pSysConf->auchComPort[i] == DEVICE_EPT) {
            chEPTPortID = i;
            return(EPT_SUCCESS);
        }
    }
    return(EPT_NOT_EPTPORT);
}

/*
*==========================================================================
**  Synopsis:   SHORT EPTOpenSetPort( VOID )
*
*   Input:      nothing 
*   Output:     nothing
*   InOut:      nothing
*   Return:     
*
*   Description: set send/receive control comm. parameter  
*==========================================================================
*/
SHORT   EPTOpenSetPort(VOID)
{
    SHORT               sRet = PIF_ERROR_COM_POWER_FAILURE;
    USHORT              usSendTimer;
    USHORT              usRcvTimer;
    SYSCONFIG CONST FAR *pSysConf;
    PROTOCOL            Prot;
    PARAMDC             MDCSendT;

    pSysConf = PifSysConfig();

    /* get send timer value */
    MDCSendT.uchMajorClass = CLASS_PARAMDC;
    MDCSendT.usAddress     = (USHORT) MDC_EPT_SND_TIME;
    CliParaRead(&MDCSendT);

    usSendTimer = MDCSendT.uchMDCData & (ODD_MDC_BIT0 |
                                         ODD_MDC_BIT1 |
                                         ODD_MDC_BIT2 |
                                         ODD_MDC_BIT3);
    usSendTimer++;                                  /* Send Value change    */
    usSendTimer *= 1000;
    usRcvTimer = 15000;

    /* set comm. protocol */
    memset(&Prot, '\0', sizeof(Prot));
    Prot.fPip = PIF_COM_PROTOCOL_NON;                   /* protocol method */

#ifdef EPTDEBUG 
    Prot.usComBaud = 9600;
    Prot.uchComByteFormat = 0x0A;                       /* odd parity, 7 bit, 1 stop bit */
#else
    Prot.usComBaud = pSysConf->ausComBaud[chEPTPortID];                 /* baud rate */
    Prot.uchComByteFormat = pSysConf->auchComByteFormat[chEPTPortID];   /* odd parity, 7 bit, 1 stop bit */ 
#endif

    /* open and set send/receive control comm. parameter */
    while (sRet == PIF_ERROR_COM_POWER_FAILURE) {

        /* open target RS-232C port */
        if ((hsEPTPort = PifOpenCom(chEPTPortID, &Prot)) < 0) { /* error case */
            return(EPT_COM_ERR);
        }
        sRet = PifControlCom(hsEPTPort, PIF_COM_SEND_TIME, (USHORT FAR *)&usSendTimer);

        if (sRet == PIF_ERROR_COM_POWER_FAILURE) {
            EPTClosePort(hsEPTPort);            /* close port */
            continue;
        } else if (sRet < 0) {
            continue;
        }
        sRet = PifControlCom(hsEPTPort, PIF_COM_SET_TIME, (USHORT FAR *)&usRcvTimer);
        if (sRet == PIF_ERROR_COM_POWER_FAILURE) {
            EPTClosePort(hsEPTPort);            /* close port */
        }
    }
    if (sRet < 0) {         /* other comm. error case */
        EPTClosePort(hsEPTPort);            /* close port */
        return(EPT_COM_ERR);
    }

    return(EPT_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT EPTConfirmCtlData( UCHAR uchCtlCode )
*
*   Input:      UCHAR   uchCtlCode
*   Output:     nothing
*   InOut:      nothing
*   Return:     
*
*   Description: send and wait control code from/to EPT terminal 
*==========================================================================
*/
SHORT   EPTConfirmCtlData(UCHAR uchCtlCode)
{
    UCHAR   auchRcvBuff[1];
    SHORT   sRet1 = PIF_ERROR_COM_POWER_FAILURE;
    SHORT   sRet2;

    /* write data to port */
    while (sRet1 == PIF_ERROR_COM_POWER_FAILURE) {
        sRet1 = PifWriteCom(hsEPTPort, &uchCtlCode, sizeof(uchCtlCode));
        if (sRet1 == PIF_ERROR_COM_POWER_FAILURE) {     
            EPTClosePort(hsEPTPort);            /* close port */
            sRet2 = EPTOpenSetPort();           /* open/set port */
            if (sRet2 == EPT_COM_ERR) {
                return(EPT_COM_ERR);
            }
        }
    }
    if (sRet1 < 0) {        /* other comm. error */
        return(EPT_COM_ERR);
    }

    sRet1 = PIF_ERROR_COM_POWER_FAILURE;

    /* read data from port */
    while(sRet1 == PIF_ERROR_COM_POWER_FAILURE) {
        sRet1 = PifReadCom(hsEPTPort, auchRcvBuff, sizeof(auchRcvBuff));
        if (sRet1 == PIF_ERROR_COM_POWER_FAILURE) {
            EPTClosePort(hsEPTPort);            /* close port */
            sRet2 = EPTOpenSetPort();           /* open/set port */
            if (sRet2 == EPT_COM_ERR) {
                return(EPT_COM_ERR);
            }
            continue;
        }
        if (sRet1 < 0) {
            return(EPT_COM_ERR);
        }
        if (uchCtlCode == EPT_ENQ) {
            if (auchRcvBuff[0] != EPT_ACK) {
                sRet1 = PIF_ERROR_COM_POWER_FAILURE;
                continue;
            }
        } else if (uchCtlCode == EPT_ACK) {
            if (auchRcvBuff[0] != EPT_EOT) {
                sRet1 = PIF_ERROR_COM_POWER_FAILURE;
                continue;
            }
        }
    }

    return(EPT_SUCCESS);
}

/*
*==========================================================================
**  Synopsis:   SHORT EPTConfirmReqData( EPTREQDATA *EPTReqData,
*                                        USHORT usDataSize )
*   Input:      EPTREQDATA  *EPTReqData
*               USHORT          usDataSize
*   Output:     nothing
*   InOut:      nothing
*   Return:     
*
*   Description: send EPT req. data and wait ACK data to/from EPT terminal 
*==========================================================================
*/
SHORT   EPTConfirmReqData(EPTREQDATA *EPTReqData, USHORT usDataSize)
{
    UCHAR   auchRcvBuff[1];
    UCHAR   uchLRC;
    SHORT   i;
    SHORT   sRet1;
    SHORT   sRet2 = PIF_ERROR_COM_POWER_FAILURE;

    /* calculate LRC check character */
    uchLRC = *(( UCHAR *)EPTReqData + 1);
    for (i = 2; i < sizeof(EPTREQDATA) - 1; i++) {
        uchLRC ^= *((( UCHAR *)EPTReqData) + i);
    }

    /* set LRC check character */
    EPTReqData->auchLRC[0] = uchLRC;

    /* write data to port */
    sRet1 = PifWriteCom(hsEPTPort, (VOID FAR *)EPTReqData, usDataSize);
    if (sRet1 < 0) {
        Para.EPTTally.usEPTSendErr++;            /* count up tally */    
        return(EPT_COM_ERR);
    }

    /* read data from port */
    while(sRet2 == PIF_ERROR_COM_POWER_FAILURE) {
        sRet1 = PifReadCom(hsEPTPort, auchRcvBuff, sizeof(auchRcvBuff));
        if (sRet1 == PIF_ERROR_COM_POWER_FAILURE) {
            EPTClosePort(hsEPTPort);            /* close port */
            sRet2 = EPTOpenSetPort();           /* open/set port */
            if (sRet2 == EPT_COM_ERR) {
                return(EPT_COM_ERR);
            }
            continue;
        }
        if (sRet1 < 0) {
            return(EPT_COM_ERR);
        }
        if (auchRcvBuff[0] != EPT_ACK) {
            sRet2 = PIF_ERROR_COM_POWER_FAILURE;
            continue;
        }
        break;
    }
                                    
    Para.EPTTally.usEPTSendOK++;             /* count up tally */    

    return(EPT_SUCCESS);
}
                    
/*
*==========================================================================
**  Synopsis:   SHORT EPTRcvRspData( EPTRSPDATA *EPTRcvData,
*                                    USHORT usDataSize )
*   Input:      EPTRCVDATA  *EPTRcvData
*               USHORT          usDataSize
*   Output:     nothing
*   InOut:      nothing
*   Return:     
*
*   Description: receive EPT response data from EPT terminal 
*==========================================================================
*/
SHORT   EPTRcvRspData(EPTRSPDATA *EPTRcvData, USHORT usDataSize)
{
    UCHAR   uchLRC = 0;
    UCHAR   uchCtlCode = EPT_NAK;
    SHORT   sRet = PIF_ERROR_COM_POWER_FAILURE;
    USHORT  usRcvDataOff;
    USHORT  i;
    USHORT  usRcvCnt;
    PARAMDC MDCRCnt;

    /* get receive timer value */
    MDCRCnt.uchMajorClass = CLASS_PARAMDC;
    MDCRCnt.usAddress     = (USHORT)MDC_EPT_RCV_TIME;
    CliParaRead(&MDCRCnt);

/*    usRcvCnt = MDCRCnt.uchMDCData & (EVEN_MDC_BIT0 |
                                     EVEN_MDC_BIT1 |
                                     EVEN_MDC_BIT2 |
                                     EVEN_MDC_BIT3);    */
    usRcvCnt = MDCRCnt.uchMDCData;
    usRcvCnt >>= 4;
    usRcvCnt++;                                         /* change '94.4.05  */
    usRcvCnt *= 2;                                      /* change '94.4.05  */

    /* search STX from response data */
    for( ; ; ) {
        for (i = 0; i < usRcvCnt; i++) {
            sRet = PifReadCom(hsEPTPort, ( VOID FAR *)EPTRcvData, 1);
            if (sRet != PIF_ERROR_COM_TIMEOUT) {
                break;
            }
            /* check abort key */
            if (UieReadAbortKey() == UIE_ENABLE) {          /* depress abort key */
                return(EPT_ABORTED);
            }
        }
        if (sRet < 0) {                         
            Para.EPTTally.usEPTRespErr++;            /* count up tally */    
            return(EPT_COM_ERR);
        }
        if (*(( UCHAR *)EPTRcvData) == EPT_STX) {
            break;
        }
    }
    for (usRcvDataOff = 1; i < usDataSize; usRcvDataOff++) {
        for (i = 0; i < usRcvCnt; i++) {
            sRet = PifReadCom(hsEPTPort, ( UCHAR *)EPTRcvData + usRcvDataOff, 1);
            if (sRet != PIF_ERROR_COM_TIMEOUT) {
                break;
            }
            /* check abort key */
            if (UieReadAbortKey() == UIE_ENABLE) {          /* depress abort key */
                return(EPT_ABORTED);
            }
        }
        if (sRet < 0) {
            Para.EPTTally.usEPTRespErr++;            /* count up tally */    
            return(EPT_COM_ERR);
        }
        if (usRcvDataOff == 1) {
            uchLRC = *(( UCHAR *)EPTRcvData + usRcvDataOff);        /* set LRC check character */
        } else {
            uchLRC ^= *(( UCHAR *)(EPTRcvData) + usRcvDataOff);     /* calculate LRC check character */
        }

        if (*(( UCHAR *)(EPTRcvData) + usRcvDataOff) == EPT_ETX) {  /* get ETX data */
            break;
        }
    }
    /* check over run data */
    if (usRcvDataOff>= usDataSize) {    /* over run case */
        Para.EPTTally.usEPTRespErr++;        /* count up tally */    
        return(EPT_COM_ERR);
    }
    for (i = 0; i < usRcvCnt; i++) {
        sRet = PifReadCom(hsEPTPort, ( UCHAR *)EPTRcvData + usRcvDataOff + 1, 1);
        if (sRet != PIF_ERROR_COM_TIMEOUT) {
            break;
        }
        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {          /* depress abort key */
            return(EPT_ABORTED);
        }
    }
    if (sRet < 0) {
        Para.EPTTally.usEPTRespErr++;        /* count up tally */    
        return(EPT_COM_ERR);
    }
    /* check LRC */
    if (*(( UCHAR *)EPTRcvData + usRcvDataOff + 1) != uchLRC) {     /* LRC unmatch */
        PifWriteCom(hsEPTPort, &uchCtlCode, sizeof(uchCtlCode));    /* send NAK to EPT */
        Para.EPTTally.usEPTRespErr++;        /* count up tally */    
        return(EPT_COM_ERR);                                    
    }
    Para.EPTTally.usEPTRespOK++;             /* count up tally */    

    return(EPT_SUCCESS);            
}

/*
*==========================================================================
**  Synopsis:   VOID EPTClosePort( SHORT hsEPTPort )
*
*   Input:      SHORT   hsEPTPort
*   Output:     nothing
*   InOut:      nothing
*   Return:     nothing
*
*   Description: close target RS-232C port  
*==========================================================================
*/
VOID    EPTClosePort(SHORT hsEPTPort)
{
    /* close target RS-232C port */
    PifCloseCom(hsEPTPort);
}

/*
*==========================================================================
**  Synopsis:   VOID EPTReadResetTally( UCHAR uchType, EPTTALLY *pTally )
*
*   Input:      UCHAR       uchType
*   Output:     nothing
*   InOut:      EPTTALLY    *pTally
*   Return:     nothing
*
*   Description: read and reset EPT comm. tally  
*==========================================================================
*/
VOID    EPTReadResetTally(UCHAR uchType, EPTTALLY *pTally)
{
	if (pTally) {
		*pTally = Para.EPTTally;
	}

    if (uchType == EPT_TALLY_RESET) {
        memset(&Para.EPTTally, 0, sizeof(EPTTALLY));
    }
}

/****** End of Source ******/