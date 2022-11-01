/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server PC STUB, System Parameter Source File
* Category    : Client/Server PC STUB, US Hospitality Model
* Program Name: PCSTBPAR.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Large Model                                               
* Options     : /c /Alfw /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*				CliParaAllRead()		Read all parameter on system RAM
*				CliParaAllWrite()		Write all parameter on system RAM
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
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
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include    <pc2170.h>
#endif
#include    <pc2172.h>

#include    <pif.h>
#include    <memory.h>
#include    "csstubin.h"
#include "mypifdefs.h"

/*
*===========================================================================
** Synopsis:    SHORT	STUBENTRY CliParaAllRead( UCHAR uchClass,
*                                       UCHAR FAR *puchRcvBuffer,
*                                       USHORT usRcvBufLen,
*                                       USHORT usStartPointer,
*                                       USHORT FAR *pusReturnLen);
*     Input:    uchClass    - Data Class
*               usRcvBufLen - Receive Buffer Length
*               usStartPointer  - Offset to Read
*    Output:    *pusReturnLen   - Read Length 
*               *puchRcvBuffer  - Receive Buffer
*
** Return:       0:  Read All RAM
*               -1:  Can not read All RAM
*
** Description:
*   This function reads all system RAM by Data Class.   
*===========================================================================
*/
SHORT	STUBENTRY CliParaAllRead( UCHAR uchClass,
                        UCHAR FAR *puchRcvBuffer,
                        USHORT usRcvBufLen,
                        USHORT usStartPointer,
                        USHORT FAR *pusReturnLen)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.uchMajorClass = uchClass;
    ReqMsgH.usHndorOffset = usStartPointer;
    ReqMsgH.usRWLen       = usRcvBufLen;   

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCPARAALLREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchResData = puchRcvBuffer;
    CliMsg.usResLen     = usRcvBufLen;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    *pusReturnLen = ResMsgH.usRWLen;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    STUBENTRY SHORT	CliParaAllWrite(UCHAR uchClass,
*                                       UCHAR FAR *puchWrtBuffer,
*                                       USHORT usRWrtBufLen,
*                                       USHORT usStartPointer,
*                                       USHORT FAR *pusReturnLen);
*     Input:    uchClass    - Data Class
*               usRWrtBufLen    - Write Buffer Length
*               usStartPointer  - Offset to Read
*    Output:    *pusReturnLen   - Read Length 
*               *puchWrtBuffer  - Receive Buffer
*
** Return:       0:  Write Ok
*               -2:  Can not write on RAM
*
** Description:
*   This function writes all system RAM by Data Class.   
*===========================================================================
*/
SHORT	STUBENTRY CliParaAllWrite(UCHAR uchClass,
                        UCHAR FAR *puchWrtBuffer,
                        USHORT usRWrtBufLen,
                        USHORT usStartPointer,
                        USHORT FAR *pusReturnLen)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.uchMajorClass = uchClass;
    ReqMsgH.usHndorOffset = usStartPointer;
    ReqMsgH.usRWLen       = usRWrtBufLen;   
       
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCPARAALLWRITE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = puchWrtBuffer;
    CliMsg.usReqLen     = usRWrtBufLen;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    *pusReturnLen = ResMsgH.usRWLen;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}


/*===== END OF SOURCE =====*/
