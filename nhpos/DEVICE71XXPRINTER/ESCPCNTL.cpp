/*
*===========================================================================
* Title       : I/O Control Function
* Category    : ESC/POS Printer Controler, NCR 2170 System S/W
* Program Name: ESCPCNTL.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Compact Model                                               
* Options     : /c /ACw /W4 /G1s /Os /Zap                                 
* --------------------------------------------------------------------------
* Abstract:     This contains the following function:
*
*           SHORT EscpControlCom(SHORT sFunc, ...);
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :Ver.Rev.:   Name    : Description
* Oct-14-99:00.00.01:T.Koyama   : Initial
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
#include "stdafx.h"
#include <windows.h>
#include <string.h>

#include <ecr.h>
#include <pif.h>
//#include <rfl.h>
#include "escp.h"



/*
*===========================================================================
*   Synopsis:   SHORT EscpControlCom(SHORT sDummy, SHORT sFunc, ...);
*
*       sFunc       one of the following function codes:
*
*   1) ESCP_STATUS_READ  : returns printer status
*
*       The following argument required in addition:
*
*           UCHAR FAR *puchBuff;    pointer to buffer
*           SHORT     sLen;         Length of the buffer
*
*       Return > 0              length of status, alway 1 byte
*              = SYSERR         if not opened or invalid handle
*
*   2) ESCP_LOOP_TEST    : no operation
*
*       The following argument required in addition:
*
*           UCHAR FAR *puchBuff;    pointer to buffer, not used
*           SHORT     sLen;         Length of the buffer
*
*       Return = sLen
*              = SYSERR     if not opened or invalid handle
*              = Others     Serial I/O port error
*
*   3) ESCP_READ_CONF    : returns printer configuration
*
*       The following argument required in addition:
*
*           UCHAR FAR *puchBuff;    pointer to buffer
*           SHORT     sLen;         Length of the buffer
*
*       Return >= 0         length of printer configuration info.
*              = SYSERR     if not opened or invalid handle
*              = Others     Serial I/O port error
*
*       This copys printer configuration info. to pchBuff.
*
*   4) ESCP_CLEAR_SEQ    : synchronizes sequence #
*
*       Return = OK
*              = SYSERR     if not opened or invalid handle
*
*       This sweeps excessive characters in the serial I/O receive buffer. 
*
*   5) ESCP_DIRECT_WRITE : outputs data without ESC v
*
*       The following argument required in addition:
*
*           UCHAR FAR *puchBuff;    pointer to output buffer
*           SHORT     sLen;         bytes to send
*
*       Return >= 0         length actually sent
*              = SYSERR     if not opened or invalid handle
*              = Others     serial I/O port errors
*
*       This outputs the designated data to the serial I/O port without
*       control like ESC v.
*
*===========================================================================
*/
SHORT EscpControlCom(SHORT sDummy, SHORT sFunc, ...)
{
    ESCP *pEscp;
	va_list		marker;
	UCHAR FAR	*puchBuff;
	SHORT		sLen;
//    struct arg {
//        CHAR FAR *puchBuff;
//        SHORT sLen;
//    } *pArg = (struct arg *)(((int *)&sFunc)+1);

    pEscp = &Escp[0];

	va_start(marker, sFunc);
	puchBuff = va_arg(marker, UCHAR FAR *);
	sLen     = va_arg(marker, SHORT);
	va_end(marker);
    
	if (!pEscp->chOpen) {               /* see if opened */
        return (PIF_ERROR_SYSTEM);
    }
    switch (sFunc) {
    case ESCP_STATUS_READ:          /* returns printer status           */
        return (EscpReadStatus(pEscp, puchBuff, sLen));

    case ESCP_LOOP_TEST:            /* loop test performs no operation  */
        return (EscpLoopTest(pEscp, puchBuff, sLen));

    case ESCP_READ_CONF:            /* returns printer configuration    */
        return (EscpReadConf(pEscp, puchBuff, sLen));

    case ESCP_CLEAR_SEQ:            /* synchronizes sequence #          */
        return (EscpClearSeq(pEscp));

    case ESCP_DIRECT_WRITE:         /* outputs data without ESC v       */
        return (EscpDirectWrite(pEscp, puchBuff, sLen));

    default:
        return (PIF_ERROR_SYSTEM);
    }
    sDummy = sDummy;
}


/*
*===========================================================================
**  Synopsis:   SHORT EscpReadStatus(ESCP *pEscp, UCHAR FAR *puchBuff,
*                                    SHORT sLen)
*
*       pEscp       pointer to ESC/POS control block
*       puchBuff    pointer to buffer to return status
*       sLen        length of the buffer
*
*   Return:    length of status, always 1 byte
*
**  Description: This returns printer status.
*===========================================================================
*/
SHORT EscpReadStatus(ESCP *pEscp, UCHAR FAR *puchBuff, SHORT sLen)
{
//    *puchBuff   = pEscp->uchStatus & (UCHAR )ESCP_NEAR_END ;  /* Status of the printer */    
    *puchBuff   = pEscp->uchStatus;  /* Status of the printer */    
    *puchBuff |=  ( ESCP_NOT_ERROR | ESCP_INIT );
                                /* set  Not error & Initialize complete */

    return (sizeof(*puchBuff));
    sLen = sLen;
}

/*
*===========================================================================
**  Synopsis:   SHORT EscpLoopTest(ESCP *pEscp, UCHAR FAR *puchBuff,
*                                    SHORT sLen)
*
*       pEscp       pointer to ESC/POS control block
*       puchBuff    pointer to buffer, not used
*       sLen        Length of the buffer
*
*   Return:    >= 0         equals to sLen if successful
*              = Others     Serial I/O port error
*
**  Description: This performs no operation.
*===========================================================================
*/
SHORT EscpLoopTest(ESCP *pEscp, UCHAR FAR *puchBuff, SHORT sLen)
{
    SHORT sStatus;
    UCHAR uchStatus;
    SHORT sTmplen;
    CHAR  Escv[] = "\x1bv";
//    CHAR  Escv[] = "\x1d\x05";

    sTmplen = strlen(Escv);
    sStatus = PifWriteCom(pEscp->sSio, Escv, sTmplen);
    if (sStatus < 0) {
        return (sStatus);
    }

	sStatus = PifReadCom(pEscp->sSio, &uchStatus, sizeof(uchStatus));
	if (sStatus == PIF_ERROR_COM_POWER_FAILURE) {
		return(PIF_ERROR_COM_POWER_FAILURE);
	}
    else if (sStatus != sizeof(uchStatus)) {
        return(PIF_ERROR_COM_TIMEOUT);        /* TIMEOUT */
    }

    return (sLen);

    puchBuff = puchBuff;    
}

/*
*===========================================================================
**  Synopsis:   SHORT EscpReadConf(ESCP *pEscp, UCHAR FAR *puchBuff,
*                                    SHORT sLen)
*
*       pEscp       pointer to ESC/POS control block
*       puchBuff    pointer to buffer, not used
*       sLen        Length of the buffer
*
*   Return:    >= 0         length of printer configuration info.
*              = Others     Serial I/O port error
*
**  Description: This copys printer configuration info. to pchBuff.
*===========================================================================
*/
/* returns printer configuration    */
SHORT EscpReadConf(ESCP *pEscp, UCHAR FAR *puchBuff, SHORT sLen)
{
    CHAR    EscpConf[] = "ESC/POS Printer";

    if(sLen < ( LEN_NULL + sizeof(ESCP_CONF_HEADER) )  ) {
        return(0);
    }

//    _RflFMemSet(puchBuff, 0, sizeof(ESCP_CONF_HEADER));
    memset(puchBuff, 0, sizeof(ESCP_CONF_HEADER));
    ((ESCP_CONF_HEADER FAR *)(puchBuff))->auchPrtConf =  ESCP_CONF_CUTTER;
                                /* Printer Config. = Printer has cutter. */
    strncpy((CHAR *)puchBuff + sizeof(ESCP_CONF_HEADER),
            EscpConf, sLen - LEN_NULL - sizeof(ESCP_CONF_HEADER) );
    *( puchBuff + ( sLen - LEN_NULL ) ) = '\0';    /* set end of string mark */
    return( sizeof(ESCP_CONF_HEADER) 
          + strlen((const CHAR *)puchBuff + sizeof(ESCP_CONF_HEADER))
          + LEN_NULL );
    pEscp = pEscp;      /* pEscp isn't used */
 }

/*
*===========================================================================
**  Synopsis:   SHORT EscpClearSeq(ESCP *pEscp, SHORT sLen)
*
*       pEscp       pointer to ESC/POS control block
*
*   Return:    = OK
*
**  Description: This sweeps excessive characters in the serial I/O
*                receive buffer. 
*===========================================================================
*/
SHORT EscpClearSeq(ESCP *pEscp)
{
    CHAR achTemp[8];
    SHORT sStatus;

    /* if sequence queue is empty, then clear BIOS ring buffer  */
    if ( (pEscp->Fifo).uchCount == 0) {
		do {
			sStatus = PifReadCom(pEscp->sSio, achTemp, sizeof(achTemp));
			if (sStatus == PIF_ERROR_COM_POWER_FAILURE) {
				return(PIF_ERROR_COM_POWER_FAILURE);
			}
        } while (sStatus != PIF_ERROR_COM_TIMEOUT);
    }

    return(PIF_OK);

        
}

/*
*===========================================================================
**  Synopsis:   SHORT EscpDirectWrite(ESCP *pEscp, UCHAR FAR *puchBuff,
*                                    SHORT sLen)
*
*       pEscp       pointer to ESC/POS control block
*       puchBuff    pointer to buffer, not used
*       sLen        Length of the buffer
*
*   Return:    >= 0         length actually sent
*              = Others     serial I/O port errors
*
**  Description: This outputs data to the serial I/O port directly
*                without ESC v control.
*===========================================================================
*/
SHORT EscpDirectWrite(ESCP *pEscp, UCHAR FAR *puchBuff, SHORT sLen)
{
    return (PifWriteCom(pEscp->sSio, puchBuff, sLen));
}


