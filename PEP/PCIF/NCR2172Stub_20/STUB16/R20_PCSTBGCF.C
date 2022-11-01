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
* Title       : Client/Server PC STUB, Guest Check File Source File                        
* Category    : Client/Server PC STUB, US Hospitality Model
* Program Name: PCSTBGCF.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Large Model                                               
* Options     : /c /Alfw /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               SerGusReadLock()            read & lock GCF
*               SerGusDelete()              delete GCF
*               SerGusIndRead()             individual read GCF
*               SerGusAllIdRead()           read all GCN
*               SerGusAllLock()             lock all GCF
*               SerGusAllUnLock()           unlock all GCF
*               SerGusAssignNo()            assign GCN
*               SerGusResetReadFlag()       reset read flag for GCF
*               SerGusCheckExist()          check exist specified GCN
*               SerGusAllIdReadBW()         read all GCN by waiter
*               SerGusResetDeliveryQueue()  reset delivery queue add R3.0
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-08-92:H.Nakashima: initial                                   
* Aug-22-95:M.Suzuki   : R3.0                                   
*          :           :                                    
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
#include    <r20_pc2172.h>

#include    <r20_pif.h>
#include    <memory.h>
#include    "csstubin.h"
#include "mypifdefs.h"

/*
*===========================================================================
** Synopsis:    SHORT  STUBENTRY  SerGusReadLock(	USHORT usGCNumber,
*                                               UCHAR FAR *puchRcvBuffer,
*                                               USHORT usSize);
*     InPut:    usGCNumber  - GCN
*               usSize  - size of buffer
*    OutPut:    *puchRcvBuffer	- receive buffer pointer to store
*
** Return:      Data Length can read
*               GCF_NOT_IN: GCN not in file			
*               GCF_LOCKED: already read
*               GCF_ALL_LOCKED: during reporting
** Description:
*   This function supports to read and to lock Guest Check File.
*===========================================================================
*/
SHORT   STUBENTRY SerGusReadLock(USHORT usGCNumber,
                                 UCHAR *puchRcvBuffer,
                                 USHORT usSize)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQGCF));
    memset(&ResMsgH, 0, sizeof(CLIRESGCF));

    ReqMsgH.usGCN       = usGCNumber;
    ReqMsgH.usGCType    = GCF_SUPER;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCGCFREADLOCK /* + CLI_FCWCONFIRM */ ;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = puchRcvBuffer;
    CliMsg.usResLen     = usSize;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerGusDelete(USHORT usGCNumber);
*     InPut:    usGCNumber  - GCN to delete
*
** Return:      GCF_SUCCESS:    deleted
*               GCF_NOT_IN: GCN not in file			
** Description:
*   This function supports to delete Guest Check File.
*===========================================================================
*/
SHORT   STUBENTRY SerGusDelete(USHORT usGCNumber)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQGCF));
    memset(&ResMsgH, 0, sizeof(CLIRESGCF));

    ReqMsgH.usGCN       = usGCNumber;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCGCFDELETE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerGusIndRead(USHORT usGCNumber,
*                                               UCHAR FAR *puchRcvBuffer,
*                                               USHORT usSize);
*     InPut:    usGCNumber  - GCN
*               usSize  - size of buffer
*    OutPut:    *puchRcvBuffer  - receive buffer pointer to read
*
** Return:      Data Length can read
*               GCF_NOT_IN:	GCN not in file			
** Description:
*   This function supports to read a Guest Check File.
*===========================================================================
*/
SHORT   STUBENTRY SerGusIndRead(USHORT usGCNumber,
                                UCHAR *puchRcvBuffer,
                                USHORT usSize)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQGCF));
    memset(&ResMsgH, 0, sizeof(CLIRESGCF));

    ReqMsgH.usGCN       = usGCNumber;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCGCFINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = puchRcvBuffer;
    CliMsg.usResLen     = usSize;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerGusAllIdRead(USHORT usType,
*                                               USHORT FAR *pusRcvBuffer,
*                                               USHORT usSize);
*     InPut:    usType  - 0:counter type, 1:drive through
*               usSize  - size of buffer
*    OutPut:    *pusRcvBuffer  - receive buffer pointer to store
*
** Return:      Data Length can read
*               GCF_NOT_IN:	GCN not in file			
** Description:
*   This function supports to read all Guest Check Number.
*===========================================================================
*/
SHORT   STUBENTRY SerGusAllIdRead(USHORT usType,
                                USHORT FAR *pusRcvBuffer,
                                USHORT usSize)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQGCF));
    memset(&ResMsgH, 0, sizeof(CLIRESGCF));

    ReqMsgH.usGCType    = usType;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCGCFREADAGCNO;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = (UCHAR *)pusRcvBuffer;
    CliMsg.usResLen     = usSize;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerGusAllLock(VOID);
*
** Return:      GCF_SUCCESS:    all locked
*               GCF_LOCKED:
*               GCF_DUR_LOCKED:	
** Description:
*   This function supports to lock all Guest Check File.
*===========================================================================
*/
SHORT   STUBENTRY SerGusAllLock(VOID)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQGCF));
    memset(&ResMsgH, 0, sizeof(CLIRESGCF));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCGCFALLLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerGusAllUnLock(VOID);
*
** Return:      GCF_SUCCESS:    all unlocked
** Description:
*   This function supports to unlock all Guest Check File.
*===========================================================================
*/
SHORT   STUBENTRY SerGusAllUnLock(VOID)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQGCF));
    memset(&ResMsgH, 0, sizeof(CLIRESGCF));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCGCFALLUNLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT  STUBENTRY  SerGusAssignNo(USHORT usGCAssignNo);
*     InPut:    usGCAssignNo    - GCN to assign
*
** Return:      GCF_SUCCESS:    assigned
** Description:
*   This function supports to assign GCN.
*===========================================================================
*/
SHORT  STUBENTRY  SerGusAssignNo(USHORT usGCAssignNo)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQGCF));
    memset(&ResMsgH, 0, sizeof(CLIRESGCF));

    ReqMsgH.usGCN       = usGCAssignNo;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCGCFASSIGN;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT  STUBENTRY  SerGusResetReadFlag(USHORT usGCNumber);
*     InPut:    usGCNumber  - GCN to reset read flag
*
** Return:      GCF_SUCCESS:    reset read flag
*               GCF_NOT_IN: GCN not in file			
*               GCF_ALL_LOCKED: during reporting
** Description:
*   This function supports to reset read flag on GCF.
*===========================================================================
*/
SHORT  STUBENTRY 	SerGusResetReadFlag(USHORT usGCNumber)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQGCF));
    memset(&ResMsgH, 0, sizeof(CLIRESGCF));

    ReqMsgH.usGCN       = usGCNumber;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCGCFRESETFG;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerGusCheckExist(VOID);
*
** Return:      GCF_SUCCESS:    not exist GCF
*               GCF_EXIST:      exist GCF
*               GCF_ALL_LOCKED: during reporting
** Description:
*   This function supports to check that GCF exist or not.
*===========================================================================
*/
SHORT	STUBENTRY SerGusCheckExist(VOID)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQGCF));
    memset(&ResMsgH, 0, sizeof(CLIRESGCF));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCGCFCHKEXIST;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerGusAllIdReadBW(USHORT usType,
*                                                   USHORT usWaiterNo;
*                                                   USHORT FAR *pusRcvBuffer,
*                                                   USHORT usSize);
*     InPut:    usType  - 0:counter type, 1:drive through
*               usSize  - size of buffer
*               usWaiterNo  - waiter number want to be read GCF
*    OutPut:    *pusRcvBuffer  - receive buffer pointer to store
*
** Return:      Data Length can read
*               GCF_NOT_IN:	GCN not in file			
** Description:
*   This function supports to read Guest Check File by waiter number.
*===========================================================================
*/
SHORT	STUBENTRY SerGusAllIdReadBW(USHORT usType,
                                    ULONG  ulWaiterNo,
                                    USHORT *pusRcvBuffer,
                                    USHORT usSize)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQGCF));
    memset(&ResMsgH, 0, sizeof(CLIRESGCF));

    ReqMsgH.usGCType    = usType;
    ReqMsgH.ulWaiterNo  = ulWaiterNo;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCGCFALLIDBW;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);
    CliMsg.pauchResData = (UCHAR *)pusRcvBuffer;
    CliMsg.usResLen     = usSize;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}
/**************************** Add R3.0 ******************************/
/*
*===========================================================================
** Synopsis:    SHORT	STUBENTRY SerGusResetDeliveryQueue(VOID);
*     InPut:    Noting
*
** Return:      GCF_SUCCESS:    deleted
*
** Description: This function supports to reset delivery queue.
*===========================================================================
*/
SHORT	STUBENTRY SerGusResetDeliveryQueue(VOID)
{
    CLIREQGCF       ReqMsgH;
    CLIRESGCF       ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQGCF));
    memset(&ResMsgH, 0, sizeof(CLIRESGCF));

    CliMsg.usFunCode    = CLI_FCGCFRESETDVQUE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQGCF);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESGCF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}
/**************************** End Add R3.0 **************************/

/*===== END OF SOURCE =====*/
