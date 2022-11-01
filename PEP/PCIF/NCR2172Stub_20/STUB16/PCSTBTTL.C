/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server PC STUB, Total Function Source File                        
* Category    : Client/Server PC STUB, US Hospitality Model
* Program Name: PCSTBTTL.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Large Model                                               
* Options     : /c /Alfw /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*           SerTtlTotalRead()           Read Specified total file
*           SerTtlTotalReadPlu()        Read plural PLU total file     R3.0
*           SerTtlTotalReset()          Reset Specified total file
*           SerTtlIssuedSet()           Set issued report flag
*           SerTtlIssuedCheck()         Check issued report flag
*           SerTtlWaiTotalCheck()		Check Waiter file
*           SerTtlCasTotalCheck()		Check Cashier file
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-08-92:H.Nakashima: initial                                   
* May-19-94:M.Suzuki   : Add SerTtlWaiTotalCheck(),SerTtlCasTotalCheck()
* Jun-15-95:hkato      : R3.0
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
#include    <csserm.h>
#include    "csstubin.h"
#include	"mypifdefs.h"

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerTtlTotalRead(VOID FAR *pTotal);
*     InOut:    *pTotal 	- store specified total file
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file     
*
** Description:
*   This function supports to read Total file.
*===========================================================================
*/
SHORT   STUBENTRY SerTtlTotalRead(VOID FAR *pTotal)
{
    CLIREQRESETTL  	ReqMsgH;
    CLIRESTOTAL  	ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQRESETTL));
    memset(&ResMsgH, 0, sizeof(CLIRESTOTAL));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLREADTL;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
	CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);
    CliMsg.pauchReqData = (UCHAR *)pTotal;
    CliMsg.usReqLen     = sizeof(CLITTLCASWAI);
    CliMsg.pauchResData = (UCHAR *)pTotal;
    CliMsg.usResLen     = sizeof(TTLHOURLY);
/*  CliMsg.usResLen     = sizeof(TTLREGFIN);*/
                          
    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerTtlTotalReadPlu(VOID FAR *pTotal)
*     InOut:    *pTotal 	- store specified total file
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file     
*
** Description:
*   This function supports to read plural PLU Total file.      R3.0
*===========================================================================
*/
SHORT   STUBENTRY SerTtlTotalReadPlu(VOID FAR *pTotal)
{
    CLIREQRESETTL  	ReqMsgH;
    CLIRESTOTAL  	ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQRESETTL));
    memset(&ResMsgH, 0, sizeof(CLIRESTOTAL));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLREADTLPLU;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
	 CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);
    CliMsg.pauchReqData = (UCHAR *)pTotal;
    CliMsg.usReqLen     = sizeof(TTLPLU) * CLI_PLU_MAX_NO;
    CliMsg.pauchResData = (UCHAR *)pTotal;
    CliMsg.usResLen     = sizeof(TTLPLU) * CLI_PLU_MAX_NO;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerTtlTotalReadPluEx(VOID FAR *pTotal)
*     InOut:    *pTotal 	- store specified total file
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file     
*
** Description:
*   This function supports to reads 20 PLU Total file.	Saratoga
*===========================================================================
*/
SHORT   STUBENTRY SerTtlTotalReadPluEx(VOID FAR *pTotal)
{
    CLIREQRESETTL  	ReqMsgH;
    CLIRESTOTAL     ResMsgH;
    SHORT           sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQRESETTL));
    memset(&ResMsgH, 0, sizeof(CLIRESTOTAL));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLREADTLPLUEX;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);
    CliMsg.pauchReqData = (UCHAR *)pTotal;
    CliMsg.usReqLen     = sizeof(TTLPLU);
    CliMsg.pauchResData = (UCHAR *)pTotal;
    CliMsg.usResLen     = sizeof(TTLPLUEX);

    CstSendMaster();
    sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerTtlTotalReset(VOID FAR *pTotal,
*                                           USHORT usMDCBit);
*  	  InOut:    *pTotal     - store specified total file
*               usMDCBit   - A/C 99, 18 execution bit
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file     
*               TTL_NOTINFILE:  not exist total file
*               TTL_NOT_ISSUED: report has not beed issued
*               TTL_LOCKED:     total file locked
*
** Description:
*   This function supports to reset Total file.
*===========================================================================
*/
SHORT   STUBENTRY SerTtlTotalReset(VOID FAR *pTotal, USHORT usMDCBit)
{
    CLIREQRESETTL  	ReqMsgH;
    CLIRESTOTAL  	ResMsgH;
    TTLCASHIER FAR *pTtl;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQRESETTL));
    memset(&ResMsgH, 0, sizeof(CLIRESTOTAL));

    pTtl = (TTLCASHIER FAR *)pTotal;
    ReqMsgH.uchMajorClass = pTtl->uchMajorClass;
    ReqMsgH.uchMinorClass = pTtl->uchMinorClass;
    ReqMsgH.usIDNO      = pTtl->usCashierNumber;
    ReqMsgH.usExeMDC    = usMDCBit;
       
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    switch(pTtl->uchMajorClass) {
    case    CLASS_TTLCASHIER:
        CliMsg.usFunCode = CLI_FCTTLRESETCAS;
        break;

    case    CLASS_TTLWAITER:
        CliMsg.usFunCode = CLI_FCTTLRESETWAI;
        break;

    case    CLASS_TTLINDFIN:
        CliMsg.usFunCode = CLI_FCTTLRESETINDFIN;
        break;

    default:
        CliMsg.usFunCode = CLI_FCTTLRESETTL;
        break;
    }
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
	CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerTtlIssuedReset(VOID FAR *pTotal,
*                                         UCHAR fIssued);
*     InPut:	*pTotal     - store specified total file
*               fIssued     - status of report flag	
*
** Return:		TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file     
*               TTL_LOCKED:     total file locked
*
** Description:
*   This function supports to set report issued flag.
*===========================================================================
*/
SHORT   STUBENTRY SerTtlIssuedReset(VOID FAR *pTotal, UCHAR fIssued)
{
    CLIREQRESETTL  	ReqMsgH;
    CLIRESTOTAL  	ResMsgH;
    TTLCASHIER      *pTtl;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQRESETTL));
    memset(&ResMsgH, 0, sizeof(CLIRESTOTAL));

    pTtl = (TTLCASHIER *)pTotal;
    ReqMsgH.uchMajorClass = pTtl->uchMajorClass;
    ReqMsgH.uchMinorClass = pTtl->uchMinorClass;
    ReqMsgH.usIDNO      = pTtl->usCashierNumber;
    ReqMsgH.fIssued     = fIssued;
       
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLISSUERST;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
	CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerTtlIssuedCheck(VOID FAR *pTotal);
*     InPut:    *pTotal     - specify total file
*
** Return:      
*
** Description:
*   This function supports to check whether total is printed or not.
*===========================================================================
*/
SHORT   STUBENTRY SerTtlIssuedCheck(VOID FAR *pTotal, UCHAR fIssued)
{
    CLIREQRESETTL  	ReqMsgH;
    CLIRESTOTAL  	ResMsgH;
    TTLCASHIER      *pTtl;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQRESETTL));
    memset(&ResMsgH, 0, sizeof(CLIRESTOTAL));

    pTtl = (TTLCASHIER *)pTotal;
    ReqMsgH.uchMajorClass = pTtl->uchMajorClass;
    ReqMsgH.uchMinorClass = pTtl->uchMinorClass;
    ReqMsgH.usIDNO      = pTtl->usCashierNumber;
    ReqMsgH.fIssued     = fIssued;
       
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLISSUECHK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
	CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerTtlWaiTotalCheck(VOID);
*     InPut:    none
*
** Return:      
*
** Description:
*   This function supports to check waiter total.
*===========================================================================
*/
SHORT   STUBENTRY SerTtlWaiTotalCheck(VOID)
{
    CLIREQRESETTL  	ReqMsgH;
    CLIRESTOTAL  	ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQRESETTL));
    memset(&ResMsgH, 0, sizeof(CLIRESTOTAL));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLWAITTLCHK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
	CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerTtlCasTotalCheck(VOID);
*     InPut:    none
*
** Return:      
*
** Description:
*   This function supports to check cashier total.
*===========================================================================
*/
SHORT   STUBENTRY SerTtlCasTotalCheck(VOID)
{
    CLIREQRESETTL  	ReqMsgH;
    CLIRESTOTAL  	ResMsgH;
	SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQRESETTL));
    memset(&ResMsgH, 0, sizeof(CLIRESTOTAL));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLCASTTLCHK;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQRESETTL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
	CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/* ###ADD Saratoga */

/*
*===========================================================================
** Synopsis:    SHORT   STUBENTRY SerTtlUpdateFile(ISPPICKUP FAR *pData,
*                                                  USHORT    usDataLen)
*     Input:    *pData      - Pickup/Loan Data Pointer
*               usDataLen   - Length of Pickup/Loan data
*
** Return:      TTL_SUCCESS:    success to read file
*               TTL_FAIL:       failed to read file
*
** Description:
*   This function supports to update for pickup/loan.
*===========================================================================
*/
SHORT   STUBENTRY SerTtlUpdateFile(ISPPICKUP FAR *pData , USHORT usDataLen)
{
    CLIREQTOTAL     ReqMsgH;
    CLIRESTOTAL     ResMsgH;
    SHORT			sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQTOTAL));
    memset(&ResMsgH, 0, sizeof(CLIRESTOTAL));

    ReqMsgH.usTransNo   = usCliTranNo ;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCTTLUPDATE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQTOTAL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESTOTAL);
    CliMsg.pauchReqData = (UCHAR *)pData;
    CliMsg.usReqLen     = usDataLen;

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if ( TTL_SUCCESS ==  CliMsg.sRetCode ) {
        usCliTranNo++;                       /* increment counter */
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*===== END OF SOURCE =====*/
