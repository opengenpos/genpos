/*
******************************************************************************
**                                                                          **
**        *=*=*=*=*=*=*=*                                                   **
**        *  NCR 2170   *             NCR Corporation, E&M OISO             **
**     @  *=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1996           **
**    <|\/~             ~\/|>                                               **
**   _/^\_               _/^\_                                              **
**                                                                          **
******************************************************************************
*=============================================================================
* Title       : Display on the Fly send thread                 
* Category    : Display on the Fly Manager, NCR 2170 US HOSP Appl
* Program Name: DflIn.C                                                        
* ----------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*               DflSendProcess()  
*                -DflOpenSetPort()
*                -DflSend1Data()
*                -DflErrorDisp()
*                -DflMoveReadP()
*               
*-----------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name       : Description
* Jun-24-93 : 00.00.01 : NAKAJIMA,Ken : Init.                                   
* Apr-16-96 : 03.01.04 : T.Nakahata   : Add DFL msg sequential # (FVT R3.1)
* Nov-29-96 : 03.01.11 : M.Takeyama   : Change DFL msg sequential # (R3.1 V11)
* Aug-19-98 : 03.03.00 : M.Ozawa      : Change auchDflSpool to FAR
*
** NCR2171 **                                         
* Aug-26-99 : 01.00.00 : M.Teraki     :initial (for 2171)
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

/**
;=============================================================================
;+                  I N C L U D E     F I L E s                              +
;=============================================================================
**/
/**------- MS - C ------**/
#include	<tchar.h>

#include <memory.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <uie.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <dfl.h>
#include "dfllocal.h"

/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/
/* -- semaphore handle -- */
extern USHORT usDflSemStartDSP; /* semaphore handle for start send thread */
extern USHORT usDflSemRWSpool;  /* semaphore handle for access to spool buffer */


/* -- spool buffer & pointer for access -- */
extern UCHAR  FAR auchDflSpool[DFL_SPOOL_SIZE];
                                /* spool buffer */
extern USHORT usDflReadP;       /* spool buffer read pointer */
extern USHORT usDflWriteP;      /* spool buffer write pointer */
extern USHORT usDflBuffEnd;     /* spool buffer end offset */

extern UCHAR  fbDflControl;     /* control flag */

extern UCHAR  fbDflSpoolFull;   /* spool buffer full flag */

extern CHAR   chDflPortID;      /* number of port connected disp. fly */

extern SHORT  hsDflPort;        /* port handle */

/**
;=============================================================================
;+              P R O G R A M    D E C L A R A T I O N s                     +
;=============================================================================
**/
/* -- process definine -- */
#define ReNewSeqNo(SeqNo)  ((SeqNo < '9') ? SeqNo++ : (SeqNo = '1'))

/*
*=============================================================================
** Format  : VOID THREADENTRY DflSendProcess(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is sending spooled data to PC through RS-232 port
*=============================================================================
*/
VOID THREADENTRY  DflSendProcess(VOID)
{
    USHORT  usIntTime, usSendTimer;
    PARAMDC MDCIntv, MDCSendT;
	PROTOCOL Prot = { 0 };
    USHORT  i;
    USHORT  usLen;
    DFLINF  *pDflInf;       /* V3.3 FVT */
    UCHAR   uchDflMsgSeqNoMaster; /* (R3.1 V11) Added */

    /* -- interval time -- */
    MDCIntv.uchMajorClass = CLASS_PARAMDC;
    MDCIntv.usAddress    = MDC_DFL_INT_TIME;

    /* -- get value of send timer -- */
    MDCSendT.uchMajorClass = CLASS_PARAMDC;
    MDCSendT.usAddress    = MDC_DFL_SND_TIME;

    /* -- set protocol -- */
    Prot.fPip = PIF_COM_PROTOCOL_NON;
/*  Prot.usPipAddr;                         */
    Prot.ulComBaud = 9600;
    Prot.uchComByteFormat = 0x1B;               /* 9600, 8, Even, 1 */
/*  Prot.auchComNonEndChar[4];              */
/*  Prot.auchComEndChar[3];                 */
/*  Prot.uchComDLEChar;                     */

    /* -- set first Sequense Number -- */      /* ADD (R3.1 V11) Added */
    uchDflMsgSeqNoMaster = '0';                /* ADD (R3.1 V11) Added */



    for (;;) {                                  /* forever */

        CliParaRead(&MDCIntv);
        usIntTime = MDCIntv.uchMDCData & (ODD_MDC_BIT0 | ODD_MDC_BIT1 |
                                          ODD_MDC_BIT2 | ODD_MDC_BIT3); 
        usIntTime *= 1000;                      /* 1 sec. = 1000msec. */

        CliParaRead(&MDCSendT);
        usSendTimer = MDCSendT.uchMDCData  & (EVEN_MDC_BIT0 | EVEN_MDC_BIT1 |
                                              EVEN_MDC_BIT2 | EVEN_MDC_BIT3);
#if defined(USE_2170_ORIGINAL)
        usSendTimer = usSendTimer >> 4;
        usSendTimer = (usSendTimer == 0)? 2000 : (usSendTimer * 1000);
                                                /* default 2 sec. */
#else
        usSendTimer = ( usSendTimer >> 4 ) * 1000;
#endif
        PifRequestSem(usDflSemStartDSP);        /* wait until data written */

        DflMoveReadP(0);                        /* move read pointer */
        pDflInf = (DFLINF FAR *)&auchDflSpool[usDflReadP];

/* (R3.1 V11) Added =From This Line= */
        /* set Sequential Number & LRC */
        pDflInf->uchDflMsgSeqNo = uchDflMsgSeqNoMaster;
                                                /* set sequential Number */
        DflAtoI(&usLen, pDflInf->aszVLI, TCHARSIZEOF(pDflInf->aszVLI));
                        /* get Length for fined buffer bottom, V3.3 FVT */
        auchDflSpool[usDflReadP + usLen -1] = DflFarXor((VOID FAR *)pDflInf, (SHORT)(usLen - 1));
                                                /* set LRC, V3.3 FVT */
        ReNewSeqNo(uchDflMsgSeqNoMaster);   /* for next Sequense */
/* (R3.1 V11) Added =To This Line= */

        for (i=0; i < DFL_RETRY_COUNT; i++) {

            DflOpenSetPort(&Prot, usSendTimer); /* open port and set timer */

            if ( DFL_PERFORM == DflSend1Data( (SHORT)((i == DFL_RETRY_COUNT - 1)?   /* read spool buffer and send */
                                               DFL_LAST_TIME : DFL_NOT_LAST )) ) {

                fbDflControl &= ~DFL_ALREADY_DISP;  /* clear disp error flag */
                break;
            
            }

/* (R3.1 V11) Delete =From This Line= ***
            * ===== increment sequential # at retry (FVT R3.1) ===== *
            pDflInf->uchDflMsgSeqNo++;
                                            * set retry status *
            DflAtoI(&usLen, pDflInf->aszVLI, sizeof(pDflInf->aszVLI));
                                            * convert ASCII to binary *
            auchDflSpool[usDflReadP + usLen -1] = DflXor(pDflInf, usLen - 1);
                                            * set LRC *
            ********* item type is not changed at retry (FVT R3.1)
            if ( i == 0 ) {
                ((DFLINF *)&auchDflSpool[usDflReadP])->auchItemType[0] += 4;
                DflAtoI(&usLen, pDflInf->aszVLI, sizeof(pDflInf->aszVLI));
                auchDflSpool[usDflReadP + usLen -1] = DflXor(pDflInf, usLen - 1);
            }
            *********  4/16/96 *************************************
 ** (R3.1 V11) Delete =To This Line= ***/
                          
            if ( i == DFL_RETRY_COUNT - 1 ) {
                DflErrorDisp();                 /* display error */
            }  else {
                PifSleep(usIntTime);            /* stop processing */
            }

        }

    }
}

/*
*=============================================================================
** Format  : VOID   DflOpenSetPort(PROTOCOL *pProt, USHORT usTimerVal)
*               
*    Input : pProt                              -protocol information
*            usTimerVal                         -send timer value              
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is open port and set status.
*=============================================================================
*/
VOID   DflOpenSetPort(PROTOCOL *pProt, USHORT usTimerVal)
{
    SHORT       sRet = PIF_ERROR_COM_POWER_FAILURE;
#if !defined(USE_2170_ORIGINAL)
    /* --- Fix a glitch (05/22/2001)
        When KDS is disconnected, NHP have to wait PifReadCom function call
        until receive timer is expired.  To execute reading function quickly,
        we should use shorter value than the current one.
        If user does not use KDS timer in MDC 176, NHP assume that send timer
        is 2000ms and receive timer is 200ms.
    --- */
    USHORT  usReceiveTimer;

    /* --- calculate receive timer from MDC 176 data --- */

    usReceiveTimer = ( 0 < usTimerVal ) ? usTimerVal : DFL_DEFAULT_RECTIME;

    /* --- calculate send timer from MDC 176 data --- */

    usTimerVal = ( 0 < usTimerVal ) ? usTimerVal : DFL_DEFAULT_SNDTIME;
#endif

    /* -- open port and set send timer value -- */
    while (sRet == PIF_ERROR_COM_POWER_FAILURE) {
         
        if ( !(fbDflControl & DFL_PORT_OPEN) ) {
            hsDflPort = PifOpenCom(chDflPortID, pProt); /* open port */
            if (hsDflPort < 0) {
                fbDflControl &= ~DFL_PORT_OPEN;     /* reset open flag */
                break;
            }
            fbDflControl |= DFL_PORT_OPEN;          /* set open flag */
        }

        sRet = PifControlCom(hsDflPort, PIF_COM_SEND_TIME, (USHORT *)&usTimerVal);      /* set send timer */
        if (sRet == PIF_ERROR_COM_POWER_FAILURE || sRet == PIF_ERROR_COM_ACCESS_DENIED) {
			// PIF_ERROR_COM_ACCESS_DENIED may be seen with Virtual Serial Communications port from USB connection.
            PifCloseCom(hsDflPort);
            fbDflControl &= ~DFL_PORT_OPEN;     /* reset open flag */
            continue;
        }

#if defined(USE_2170_ORIGINAL)
        sRet = PifControlCom(hsDflPort, PIF_COM_SET_TIME, (USHORT FAR *)&usTimerVal);
                                                /* set receive timer */
#else
        sRet = PifControlCom(hsDflPort, PIF_COM_SET_TIME, &usReceiveTimer);
#endif
        if (sRet == PIF_ERROR_COM_POWER_FAILURE || sRet == PIF_ERROR_COM_ACCESS_DENIED) {
			// PIF_ERROR_COM_ACCESS_DENIED may be seen with Virtual Serial Communications port from USB connection.
            PifCloseCom(hsDflPort);
            fbDflControl &= ~DFL_PORT_OPEN;     /* reset open flag */
        }
    }
}


/*
*=============================================================================
** Format  : SHORT   DflSend1Data(SHORT  sState)
*               
*    Input : sState                             -last time ? 
*   Output : DFL_PERFORM                        -success to send data
*            DFL_FAIL                           -fail to send data      
*    InOut : none
** Return  : none
*
** Synopsis: This function opens port and sets status.
*=============================================================================
*/
SHORT   DflSend1Data(SHORT  sState)
{
    USHORT  usLen;
    UCHAR   uchENQ = DFL_ENQ;

    DflMoveReadP(0);                            /* move read pointer */

    usLen = _ttoi(((DFLINF *)&auchDflSpool[usDflReadP])->aszVLI);
    //DflFarAtoI(&usLen, ((DFLINF FAR *)&auchDflSpool[usDflReadP])->aszVLI, sizeof(((DFLINF *)&auchDflSpool[usDflReadP])->aszVLI));
                                                /* convert ASCII to binary */
#if !defined(USE_2170_ORIGINAL)
    /* --- Fix a glitch (05/22/2001)
        Avoid to send item data to KDS device which is disconnected, check
        RS232 status before sending data.
    --- */
    if ( ! DflIsReady( sState, usLen ))
    {
        return(DFL_FAIL);
    }
#endif
    /*  -- send ENQ and receive ACK -- */
    if ( DFL_FAIL == DflWriteCom(sState, usLen, (VOID FAR *)&uchENQ, sizeof(uchENQ)) ) {
        return(DFL_FAIL);
    }
    if ( DFL_FAIL == DflReadACK(sState, usLen) ) {
        return(DFL_FAIL);                       
    }


    /*  -- send Data and receive ACK -- */
    if ( DFL_FAIL == DflWriteCom(sState, usLen, &auchDflSpool[usDflReadP], usLen) ) {
        return(DFL_FAIL);
    }
    if ( DFL_FAIL == DflReadACK(sState, usLen) ) {
        return(DFL_FAIL);                       
    }

    DflMoveReadP(usLen);                        /* move read pointer */

    return (DFL_PERFORM);
}

/*
*=============================================================================
** Format  : VOID   DflErrorDisp(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is open port and set status.
*=============================================================================
*/
VOID   DflErrorDisp(VOID)
{
    if ( fbDflControl & DFL_ALREADY_DISP ) {    /* already display error ? */
        return ;
    }

    UieErrorMsg(LDT_DFLERR_ADR, UIE_WITHOUT);

    fbDflControl |= DFL_ALREADY_DISP;           /* set disp error flag */

}


/*
*=============================================================================
** Format  : VOID   DflMoveReadP(USHORT usLen)
*               
*    Input : usLen                              -increment length
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function increment buffer readpointer.
*=============================================================================
*/
VOID   DflMoveReadP(USHORT usLen)
{

    PifRequestSem(usDflSemRWSpool);             /* request sem. for access to spool buffer */

    usDflReadP += usLen;

    if ( (fbDflSpoolFull & DFL_SPOOL_FULL) &&
         (usDflReadP == usDflBuffEnd) ) {

        usDflReadP = 0;                         /* move read pointer to buffer top */
        fbDflSpoolFull &=  ~DFL_SPOOL_FULL;
    }

    PifReleaseSem(usDflSemRWSpool);             /* release sem. for access to spool buffer */
}

/*
*=============================================================================
** Format  : SHORT   DflWriteCom(SHORT sState, USHORT usMove, VOID *pData, USHORT usDataLen)
*               
*    Input : sState                             -last time ? 
*            usMove                             -read pointer move length   
*            pData                              -send data   
*            usDataLen                          -send data length   
*   Output : DFL_PERFORM                        -success to send data
*            DFL_FAIL                           -fail to send data      
*    InOut : none
** Return  : none
*
** Synopsis: This function sends  data. V3.3
*=============================================================================
*/
SHORT   DflWriteCom(SHORT sState, USHORT usMove, VOID FAR *pData, USHORT usDataLen)
{
    SHORT  sRet = PIF_ERROR_COM_ERRORS;

    if (fbDflControl & DFL_PORT_OPEN) {
        sRet = PifWriteCom(hsDflPort, pData, usDataLen);
                                                /* send data to port */
    }

    if ( DFL_FAIL == DflCheckComError(sState, sRet, usMove, usDataLen) ) {
        return(DFL_FAIL);                       /* check PifWriteCom() error */
    }

	return (DFL_PERFORM);
}

/*
*=============================================================================
** Format  : SHORT   DflCheckComError(SHORT  sState, SHORT sRet, USHORT usMove, USHORT usSeciLen)
*               
*    Input : sState                             -last time ? 
*            sRet                               -return value
*            usMove                             -read pointer move length
*            usSpeciLen                         -specified length
*   Output : none
*    InOut : none
** Return  : DFL_PERFORM                        -success
*            DFL_FAIL                           -fail

** Synopsis: This function sends ENQ and receives ACK.
*=============================================================================
*/
SHORT   DflCheckComError(SHORT  sState, SHORT sRet, USHORT usMove, USHORT usSeciLen)
{

    if (sRet < 0) {
        if (PIF_ERROR_COM_POWER_FAILURE == sRet || PIF_ERROR_COM_ACCESS_DENIED == sRet) {
			// PIF_ERROR_COM_ACCESS_DENIED may be seen with Virtual Serial Communications port from USB connection.
            if (fbDflControl & DFL_PORT_OPEN) {
                PifCloseCom(hsDflPort);
            }
            fbDflControl &= ~DFL_PORT_OPEN;
        }
        if (sState == DFL_LAST_TIME) {
            DflMoveReadP(usMove);               /* move read pointer */
        }
        return (DFL_FAIL);
    }

    if ((USHORT)sRet != usSeciLen) {            /* specifed length data was not sent */
        if (sState == DFL_LAST_TIME) {
            DflMoveReadP(usMove);               /* move read pointer */
        }
        return (DFL_FAIL);
    }

    return (DFL_PERFORM);
}

/*
*=============================================================================
** Format  : SHORT   DflReadACK(SHORT sState, USHORT usMove)
*               
*    Input : sState                             -last time ? 
*            usMove                             -read pointer move length   
*   Output : DFL_PERFORM                        -success to send data
*            DFL_FAIL                           -fail to send data      
*    InOut : none
** Return  : none
*
** Synopsis: This function sends  receives ACK.
*=============================================================================
*/
SHORT   DflReadACK(SHORT sState, USHORT usMove)
{
    UCHAR uchBuf;
    SHORT sRet = PIF_ERROR_COM_ERRORS;


    if (fbDflControl & DFL_PORT_OPEN) {

        do {
            sRet = PifReadCom(hsDflPort, &uchBuf, sizeof(uchBuf));
        } while ((uchBuf != DFL_ACK) && (sRet >= 0));
    }

    if ( DFL_FAIL == DflCheckComError(sState, sRet, usMove, sizeof(uchBuf))) {
        return(DFL_FAIL);                       /* check PifReadCom() error */
    }

    return (DFL_PERFORM);

}

/*
*=============================================================================
** Format  : UCHAR  DflFarXor(VOID FAR *pStart, SHORT sLen)
*               
*    Input : pStart                             -data start address 
*            sLen                              -data length  
*   Output : none
*    InOut : none
** Return  : fbXor                              -xor value 
*
** Synopsis: This function get "exclusive_or value"            V3.3 FVT
*=============================================================================
*/
UCHAR  DflFarXor(VOID FAR *pStart, SHORT sLen)
{
    UCHAR fbXOR;
    SHORT i;

    for (fbXOR=0, i=0; sLen > i; i++) {
        
        fbXOR ^= *(((UCHAR FAR *)pStart) + i);
    }

    return fbXOR;
}

#if !defined(USE_2170_ORIGINAL)
/*
*=============================================================================
** Format  : SHORT  DflIsReady(VOID)
*               
*    Input : SHORT  sState  -   indicates retry operation is last or not.
*            USHORT usMove  -   number of bytes to move read pointer.
*   Output : none
*    InOut : none
** Return  : TRUE   -   COM port is ready to send.
*            FALSE  -   COM port is not ready.
*
** Synopsis: This function determines whether COM port is ready to send data.
*=============================================================================
*/
#define COM_RLSD    0x0080
#define COM_DSR     0x0020
#define COM_CTS     0x0010
SHORT   DflIsReady( SHORT sState, USHORT usMove )
{
    SHORT   IsReady = FALSE;

    /* --- get state of the communication device (RS232) --- */

    if ( fbDflControl & DFL_PORT_OPEN )
    {
        SHORT   PortStatus;

        PortStatus = PifControlCom( hsDflPort, PIF_COM_GET_STATUS );

        IsReady = (( PortStatus & COM_DSR ) && ( PortStatus & COM_CTS )) ? TRUE : FALSE;
    }
    if (( ! IsReady ) && ( sState == DFL_LAST_TIME ))
    {
        /* --- If this operation is the last retry, points to the next
            item.  The failed data will be lost... --- */

        DflMoveReadP( usMove );
    }
    return IsReady;
}
#endif
/* ---------- End of DflIn.c ---------- */