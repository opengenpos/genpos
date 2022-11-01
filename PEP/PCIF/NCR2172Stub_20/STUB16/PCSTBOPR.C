/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1996       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server PC STUB, Operational Parameter Source File                        
* Category    : Client/Server PC STUB, US Hospitality Model
* Program Name: PCSTBOPR.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Large Model                                               
* Options     : /c /Alfw /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*           CliReqOpPara()          Request read operational parameter
*           CliOpPluIndRead()       Read individual PLU record
*           CliOpDeptIndRead()      Read individual Dept record
*           CliOpCpnIndRead()       Read individual Coupon record    R3.0 
*           CliOpCstrIndRead()      Read individual Contorl Storing record    R3.0 
*           CliOpOepPluRead()       Read Oep PLU record              R3.0 
*           CliOpPpiIndRead()       Read Promotional Pricing Item    R3.1
* 		
*           SerChangeOpPara()       Change parameter, inform to NB
*           SerOpDeptAssign()       Assign Dept record 
*           SerOpPluAssign()        Assign PLU record 
*           SerOpCpnAssign()        Assign Coupon record             R3.0 
*           SerOpCstrAssign()       Assign Control Storing record    R3.0 
*           SerOpPpiAssign()        Assign Promotional Pricing Item  R3.1
*           SerOpPpiDelete()        Delete Promotional Pricing Item  R3.1
*           SerOpPluDelete()        Delete PLU record
*           SerOpCstrDelete()       Delete Control Storing record    R3.0 
*           SerOpPluFileUpdate()    Update PLU (index) file
*           SerOpPluAllRead()       Read All PLU record              R3.0
*           SerOpRprtPluRead()      Read PLU record by report code   R3.0
*           SerOpDeptPluRead()      Read PLU record by dept number   R3.0
*           SerOpLock()             Lock Record
*           SerOpUnLock()           UnLock Record
*           SerOpMajorDeptRead()    Read specified major dept parameter				
*
*           CliOpMldIndRead()       Read MLD mnemonic               V3.3
*           SerOpMldAssign()        Assign MLD mnemonic             V3.3
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Jun-15-95:hkato      : R3.0
* Feb-01-96:T.Nakahata : R3.1 Initial (Add Promotional Pricing Item File)
* Sep-07-98:O.Nakada   : Add V3.3
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
** Synopsis:    SHORT  STUBENTRY  CliOpPluIndRead(PLUIF FAR *pPif, 
*                                                 USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pPif        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description:
*   This function supports to read a PLU record.
*===========================================================================
*/
SHORT   STUBENTRY CliOpPluIndRead(PLUIF FAR *pPif, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPPLUINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pPif;
    CliMsg.usReqLen     = sizeof(PLUIF);
    CliMsg.pauchResData = (UCHAR *)pPif;
    CliMsg.usResLen     = sizeof(PLUIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY CliOpDeptIndRead(DEPTIF FAR *pDpi,
*                                                  USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pDpi        - Dept information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_ABNORMAL_DEPT:   PLU not found in PLU file
*               OP_LOCK:            during Locked
*
** Description:
*   This function supports to read a Dept record.
*===========================================================================
*/
SHORT   STUBENTRY CliOpDeptIndRead(DEPTIF FAR *pDpi, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPDEPTINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pDpi;
    CliMsg.usReqLen     = sizeof(DEPTIF);
    CliMsg.pauchResData = (UCHAR *)pDpi;
    CliMsg.usResLen     = sizeof(DEPTIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliOpCpnIndRead(CPNIF FAR *pCpn, USHORT usLockHnd);
*     InPut:    usLockHnd    - handle
*     InOut:    *pCpn        - Coupon information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  coupon file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to read a Coupon record.
*===========================================================================
*/
SHORT   STUBENTRY CliOpCpnIndRead(CPNIF FAR *pCpn, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPCPNINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pCpn;
    CliMsg.usReqLen     = sizeof(CPNIF);
    CliMsg.pauchResData = (UCHAR *)pCpn;
    CliMsg.usResLen     = sizeof(CPNIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliOpOepPluRead(PLUIF_OEP FAR *pPlu, USHORT usLockHnd)
*     InPut:    usLockHnd    - handle
*     InOut:    *pPlu        - OEP taget PLU
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to read OEP target PLU.
*===========================================================================
*/
SHORT   STUBENTRY CliOpOepPluRead(PLUIF_OEP FAR *pPlu, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT		sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPOEPPLUREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pPlu;
//    CliMsg.usReqLen     = sizeof(PLUIF_OEP);
    CliMsg.usReqLen     = sizeof(PLUIF);
    CliMsg.pauchResData = (UCHAR *)pPlu;
    CliMsg.usResLen     = sizeof(PLUIF_OEP);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliOpCstrIndRead(CSTRIF FAR *pCstr, USHORT usLockHnd)
*     InPut:    usLockHnd    - handle
*     InOut:    *pPlu        - OEP taget PLU
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to read control string table.
*===========================================================================
*/
SHORT   STUBENTRY CliOpCstrIndRead(CSTRIF FAR *pCstr, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPCSTRINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pCstr;
    CliMsg.usReqLen     = sizeof(CSTRIF);
    CliMsg.pauchResData = (UCHAR *)pCstr;
    CliMsg.usResLen     = sizeof(CSTRIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerOpDeptAssign(DEPTIF FAR *pDpi,
*                                                 USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pDpi        - Dept information to assign
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_ABNORMAL_DEPT:   PLU not found in PLU file
*               OP_LOCK:            during Locked
*
** Description:
*   This function supports to assign a Dept record.
*===========================================================================
*/
SHORT   STUBENTRY SerOpDeptAssign(DEPTIF FAR *pDpi, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPDEPTASSIGN;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pDpi;
    CliMsg.usReqLen     = sizeof(DEPTIF);
    CliMsg.pauchResData = (UCHAR *)pDpi;
    CliMsg.usResLen     = sizeof(DEPTIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT	STUBENTRY SerOpPluAssign(PLUIF FAR *pPif,
*                                                USHORT usLockHnd);
*	  InPut:	usLockHnd	- handle
*	  			*pPif		- PLU information to assign
*
** Return:		PERFORM:		read OK
*				FILE_NOT_FOUND:	PLU file not exist
*				NOT_IN_FILE:	PLU not in file
*				LOCK:			during Locked
*
** Description:
*   This function supports to assign a PLU record.
*===========================================================================
*/
SHORT	STUBENTRY SerOpPluAssign(PLUIF FAR *pPif, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPPLUASSIGN;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pPif;
    CliMsg.usReqLen     = sizeof(PLUIF);
    CliMsg.pauchResData = (UCHAR *)pPif;
    CliMsg.usResLen     = sizeof(PLUIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpCpnAssign(CPNIF FAR *pCpni, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pCpni      - Coupon information to assign
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  Coupon file not exist
*               OP_ABNORMAL_CPN:    abnormal coupon number (HALT)
*               OP_LOCK:            during Locked
*
** Description: This function supports to assign a Coupon record.
*===========================================================================
*/
SHORT   STUBENTRY SerOpCpnAssign(CPNIF FAR *pCpni, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPCPNASSIGN;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pCpni;
    CliMsg.usReqLen     = sizeof(CPNIF);
    CliMsg.pauchResData = (UCHAR *)pCpni;
    CliMsg.usResLen     = sizeof(CPNIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerOpCstrAssign(CSTRIF FAR *pCstr, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pCstr      - Control String information to assign
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  Control String file not exist
*               OP_ABNORMAL_CSTR:   abnormal coupon number (HALT)
*               OP_LOCK:            during Locked
*
** Description: This function supports to assign a Control String record.
*===========================================================================
*/
SHORT   STUBENTRY SerOpCstrAssign(CSTRIF FAR *pCstr, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPCSTRASSIGN;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pCstr;
    CliMsg.usReqLen     = sizeof(CSTRIF);
    CliMsg.pauchResData = (UCHAR *)pCstr;
    CliMsg.usResLen     = sizeof(CSTRIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*                      
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerOpPluDelete(PLUIF FAR *pPif,
*                                                USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pPif       - PLU information to delete
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description:
*   This function supports to delete a PLU record.
*===========================================================================
*/
SHORT   STUBENTRY SerOpPluDelete(PLUIF FAR *pPif, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPPLUDELETE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pPif;
    CliMsg.usReqLen     = sizeof(PLUIF);
    CliMsg.pauchResData = (UCHAR *)pPif;
    CliMsg.usResLen     = sizeof(PLUIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*                      
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerOpDeptDelete(PLUIF FAR *pPif,
*                                                 USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pPif       - PLU information to delete
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description:
*   This function supports to delete a PLU record.
*===========================================================================
*/
SHORT   STUBENTRY SerOpDeptDelete(DEPTIF FAR* pPif, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;

	memset(&CliMsg, 0, sizeof(CLICOMIF));
	CliMsg.usFunCode	= CLI_FCOPDEPTDELETE;
	CliMsg.pReqMsgH		= (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pPif;
    CliMsg.usReqLen     = sizeof(DEPTIF);
    CliMsg.pauchResData = (UCHAR *)pPif;
    CliMsg.usResLen     = sizeof(DEPTIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*                      
*===========================================================================
** Synopsis:    SHORT   SerOpCstrDelete(CSTRIF FAR *pCstr, USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pCstr      - Control String information to delete
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  Control Storing file not exist
*               OP_ABNORMAL_CSTR:   abnormal coupon number (HALT)
*               OP_LOCK:            during Locked
*
** Description: This function supports to delete a Control String record.
*===========================================================================
*/
SHORT   STUBENTRY SerOpCstrDelete(CSTRIF FAR *pCstr, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPCSTRDELETE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pCstr;
    CliMsg.usReqLen     = sizeof(CSTRIF);
    CliMsg.pauchResData = (UCHAR *)pCstr;
    CliMsg.usResLen     = sizeof(CSTRIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerOpPluFileUpdate(USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_LOCK:            during Locked
*
** Description:
*   This function supports to delete a PLU record.
*===========================================================================
*/
SHORT   STUBENTRY SerOpPluFileUpdate(USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPPLUFILEUPD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerOpPluAllRead(PLUIF FAR *pPif,
*                                                 USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pPif        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_EOF:             all PLU read
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_LOCK:            during Locked
*
** Description:
*   This function supports to read all PLU plural record.          R3.0
*===========================================================================
*/
SHORT   STUBENTRY SerOpPluAllRead(PLUIF FAR *pPif, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPPLUALLREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pPif;
    CliMsg.usReqLen     = sizeof(PLUIF) * CLI_PLU_MAX_NO;
    CliMsg.pauchResData = (UCHAR *)pPif;
    CliMsg.usResLen     = sizeof(PLUIF) * CLI_PLU_MAX_NO;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerOpRprtPluRead(PLUIF_REP FAR *pPrp, 
*                                                   USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pPrp        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_EOF:             all PLU read
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_PLU_FULL:        exceed PLU number
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description:
*   This function supports to read PLU plural record by report code. R3.0
*===========================================================================
*/
SHORT   STUBENTRY SerOpRprtPluRead(PLUIF_REP FAR *pPrp, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPPLUREADBRC;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pPrp;
    CliMsg.usReqLen     = sizeof(PLUIF_REP) * CLI_PLU_MAX_NO;
    CliMsg.pauchResData = (UCHAR *)pPrp;
    CliMsg.usResLen     = sizeof(PLUIF_REP) * CLI_PLU_MAX_NO;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerOpDeptPluRead(PLUIF_DEP FAR *Pdp,
*                                                   USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *Pdp        - PLU information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_EOF:             all PLU read
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_PLU_FULL:        exceed PLU number
*               OP_NOT_IN_FILE:     PLU not in file
*               OP_LOCK:            during Locked
*
** Description:
*   This function supports to read PLU plural record by dept number. R3.0
*===========================================================================
*/
SHORT   STUBENTRY SerOpDeptPluRead(PLUIF_DEP FAR *pPdp, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPPLUREADBDP;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pPdp;
    CliMsg.usReqLen     = sizeof(PLUIF_DEP) * CLI_PLU_MAX_NO;
    CliMsg.pauchResData = (UCHAR *)pPdp;
    CliMsg.usResLen     = sizeof(PLUIF_DEP) * CLI_PLU_MAX_NO;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerOpLock(VOID);
*
** Return:      Lock Handle
*               OP_LOCK:        already locked		
*
** Description:
*   This function supports to lock PLU/ Dept record.
*===========================================================================
*/
SHORT	STUBENTRY SerOpLock(VOID)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT  STUBENTRY  SerOpUnLock(VOID);
*
** Return:      OP_PERFORM:     unlock OK
*               OP_LOCK:        already locked by another handle 		
*
** Description:
*   This function supports to unlock PLU/ Dept record.
*===========================================================================
*/
SHORT   STUBENTRY SerOpUnLock(VOID)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPUNLOCK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerOpMajorDeptRead(MDEPTIF FAR *pDpi, 
*                                                    USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pDpi       - Dept information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_EOF:             all dept read
*               OP_FILE_NOT_FOUND:  PLU file not exist
*               OP_LOCK:            during Locked
*
** Description:
*   This function supports to read a Dept record.
*===========================================================================
*/
SHORT   STUBENTRY SerOpMajorDeptRead(MDEPTIF FAR *pDpi, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPDEPTREADBMD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pDpi;
    CliMsg.usReqLen     = sizeof(MDEPTIF);
    CliMsg.pauchResData = (UCHAR *)pDpi;
    CliMsg.usResLen     = sizeof(MDEPTIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/******* New Functions - Promotional Pricing Item (Release 3.1) BEGIN ******/

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY CliOpPpiIndRead(PPIIF FAR *pPpi,
*                                                 USHORT usLockHnd)
*     InPut:    usLockHnd   - handle
*     InOut:    pPpi        - PPI information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to read promotional pricing item.
*===========================================================================
*/
SHORT   STUBENTRY CliOpPpiIndRead(PPIIF FAR *pPpi, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPPPIINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pPpi;
    CliMsg.usReqLen     = sizeof(PPIIF);
    CliMsg.pauchResData = (UCHAR *)pPpi;
    CliMsg.usResLen     = sizeof(PPIIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerOpPpiAssign(PPIIF FAR *pPpi,
*                                                USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pPpi       - Promotional Pricing Item information to assign
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to assign a PPI record.
*===========================================================================
*/
SHORT   STUBENTRY SerOpPpiAssign(PPIIF FAR *pPpi, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPPPIASSIGN;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pPpi;
    CliMsg.usReqLen     = sizeof(PPIIF);
    CliMsg.pauchResData = (UCHAR *)pPpi;
    CliMsg.usResLen     = sizeof(PPIIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerOpPpiDelete(PPIIF FAR *pPpi,
*                                                USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*               *pPpi       - Promotional Pricing Item information to assign
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  file not exist
*               OP_LOCK:            during Locked
*
** Description: This function supports to delete a PPI record.
*===========================================================================
*/
SHORT   STUBENTRY SerOpPpiDelete(PPIIF FAR *pPpi, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPPPIDELETE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pPpi;
    CliMsg.usReqLen     = sizeof(PPIIF);
    CliMsg.pauchResData = (UCHAR *)pPpi;
    CliMsg.usResLen     = sizeof(PPIIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/******* New Functions - Promotional Pricing Item (Release 3.1) END ******/

/*
*===========================================================================
** Synopsis:    SHORT  STUBENTRY  CliOpMldIndRead(MLDIF FAR *pMld, 
*                                                 USHORT usLockHnd);
*     InPut:    usLockHnd   - handle
*     InOut:    *pMld       - MLD information to read
*
** Return:      OP_PERFORM:         read OK
*               OP_FILE_NOT_FOUND:  the MLD mnemonic file not exist
*               OP_LOCK:            during Locked
*
** Description:
*   This function supports to read a PLU record.
*===========================================================================
*/
SHORT   STUBENTRY CliOpMldIndRead(MLDIF FAR *pMld, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPMLDINDREAD;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pMld;
    CliMsg.usReqLen     = sizeof(MLDIF);
    CliMsg.pauchResData = (UCHAR *)pMld;
    CliMsg.usResLen     = sizeof(MLDIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT	STUBENTRY SerOpMldAssign(MLDIF FAR *pMld,
*                                                USHORT usLockHnd);
*	  InPut:	usLockHnd	- handle
*	  			*pMld		- MLD information to assign
*
** Return:		PERFORM:		read OK
*				FILE_NOT_FOUND:	the MLD mnemonic file not exist
*				LOCK:			during Locked
*
** Description:
*   This function supports to assign a multi-line display mnemonic.
*===========================================================================
*/
SHORT	STUBENTRY SerOpMldAssign(MLDIF FAR *pMld, USHORT usLockHnd)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    ReqMsgH.usHndorOffset = usLockHnd;
           
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCOPMLDASSIGN;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.pauchReqData = (UCHAR *)pMld;
    CliMsg.usReqLen     = sizeof(MLDIF);
    CliMsg.pauchResData = (UCHAR *)pMld;
    CliMsg.usResLen     = sizeof(MLDIF);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

#endif
/*===== END OF SOURCE =====*/
