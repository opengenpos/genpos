/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server STUB, Shared Print Source File
* Category    : Client/Server STUB, US Hospitality Enhance Model
* Program Name: CSSTBSHR.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           CliShrPrint()        Request to print shared printer
*           CliShrTermLock()     Request to lock shared printer
*           CliShrTermUnLock()   Request to unlock shared printer
*           CliShrPolling()      Request to polling 
*           CliShrPrintKP()      Request to print shared printer for KP
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jun-08-93:H.Yamaguchi: initial                                   
* Jul-27-93:H.Yamaguchi: Adds CstInitShrMsgH() for reduction                                  
* Jul-06-95:M.suzuki   : R3.0                                   
* Apl-26-96:M.suzuki   : Chg CliShrPrintKP R3.1
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
#include	<tchar.h>

#include    <memory.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <para.h>
#include    <shr.h>
#include    <csstbshr.h>
#include    <csstbpar.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    "csstubin.h"

static XGRAM       ShrSndBuf;          /* Send Buffer */    
static XGRAM       ShrRcvBuf;          /* Receive Buffer */
static CLICOMIF    ShrMsg;

/*
*===========================================================================
** Synopsis:    VOID   CstInitShrMsgH(CLIREQSHR  *pReqMsgH, CLIRESSHR  *pResMsgH)
*
*     Inout:    ReqMsgH - Request  Message header
*               ResMsgH - Response Message header
*
** Return:      noting
*
** Description: This function supports to initialize messages.
*===========================================================================
*/
static VOID   CstInitShrMsgH(CLIREQSHR  *pReqMsgH, CLIRESSHR  *pResMsgH)
{
    memset(pReqMsgH, 0, sizeof(CLIREQSHR));
    memset(pResMsgH, 0, sizeof(CLIRESSHR));
}


/*
*===========================================================================
** Synopsis:    SHORT   CliShrPrint(UCHAR  uchUAddr,
*                                   VOID   *pData,
*                                   USHORT usDataLen)
*                                   
*     InPut:    usUniqueAddress - Terget address
*           	*auchSndBuffer  - Shared print data buffer
*               usDataLen       - Shared print data length
*    OutPut:    Nothing
*
** Return:      SHR_PERFORM:    print OK
*
** Description: This function supports to print out on Shared Printer.
*===========================================================================
*/
SHORT    CliShrPrint(UCHAR  uchUAddr, VOID *pData, USHORT usDataLen)
{
    CLIREQSHR  ReqMsgH;
    CLIRESSHR  ResMsgH;
    SHORT      sRetryCo;
	SHORT		sError;

    PifRequestSem(husCliExeNet);
    CstInitShrMsgH(&ReqMsgH, &ResMsgH);

    memset(&ShrMsg, 0, sizeof(CLICOMIF));
    ShrMsg.usFunCode    = CLI_FCSPSPRINT;
    ShrMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    ShrMsg.usReqMsgHLen = sizeof(CLIREQSHR);
    ShrMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    ShrMsg.usResMsgHLen = sizeof(CLIRESSHR);
    ShrMsg.pauchReqData = pData;
    ShrMsg.usReqLen     = usDataLen;

    if (STUB_SELF == (sError = CstSendTarget(uchUAddr, &ShrMsg, &ShrSndBuf, &ShrRcvBuf))) {
		PifReleaseSem(husCliExeNet);
        sRetryCo = CstReadMDCPara(CLI_MDC_RETRY);

        do {
            sError = ShrPrint(pData);
            if ( SHR_BUSY == sError) {
                sRetryCo --;
                CstSleep();
            } else {
                sRetryCo = 0;
            }
        } while ( 0 != sRetryCo );
    }
	else {
		if (STUB_TIME_OUT == sError) {
			sError = SHR_FAIL;
		}
		else if (STUB_BUSY == sError || STUB_BUSY_MULTI == sError) {
			sError = SHR_BUSY;
		}
		else {
			sError = ShrMsg.sRetCode;

			if (sError == SHR_M_DOWN_ERROR || sError == SHR_BM_DOWN_ERROR) {
				sError = SHR_FAIL;
			}
			else if (sError == STUB_RETCODE_TIME_OUT_ERROR) {
				sError = SHR_FAIL;
			}
			else if (sError == STUB_RETCODE_BUSY_ERROR) {
				sError = SHR_BUSY;
			}

			// If there are network level errors then we need to inform the
			// user there is a problem.
			switch (ShrMsg.sError) {
	//			case    STUB_DISCOVERY:  // Removed as causes problem with false Backup Master Busy error.
					// fall through and treat same as STUB_TIME_OUT.  See also PIF_ERROR_NET_DISCOVERY
				case STUB_TIME_OUT:
					sError = SHR_FAIL;
					break;
				case STUB_BUSY_MULTI:
				case STUB_BUSY:
					sError = SHR_BUSY;
					break;
				default:
					break;
			}
		}
		PifReleaseSem(husCliExeNet);
	}
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliShrTermLock(UCHAR uchUAddr)
*                                   
*     InPut:    usUniqueAddress - Terget address
*    OutPut:    Nothing
*
** Return:      SHR_PERFORM:    print OK
*
** Description: This function supports to lock Shared Printer.
*===========================================================================
*/
SHORT    CliShrTermLock(UCHAR  uchUAddr)
{
    CLIREQSHR  ReqMsgH;
    CLIRESSHR  ResMsgH;
    SHORT      sError;

	if (uchUAddr == 0) {
		return SHR_FAIL;
	}

    PifRequestSem(husCliExeNet);
    CstInitShrMsgH(&ReqMsgH, &ResMsgH);

    memset(&ShrMsg, 0, sizeof(CLICOMIF));
    ShrMsg.usFunCode    = CLI_FCSPSTERMLOCK;
    ShrMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    ShrMsg.usReqMsgHLen = sizeof(CLIREQSHR);
    ShrMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    ShrMsg.usResMsgHLen = sizeof(CLIRESSHR);

    if (STUB_SELF == (sError = CstSendTarget(uchUAddr, &ShrMsg, &ShrSndBuf, &ShrRcvBuf))) {
		PifReleaseSem(husCliExeNet);
        sError = ShrTermLock(uchUAddr);
    }
	else {
		if (STUB_TIME_OUT == sError) {
			sError = SHR_FAIL;
		}
		else if (STUB_BUSY == sError) {
			sError = SHR_BUSY;
		}
		else {
			sError = ShrMsg.sRetCode;

			if (sError == SHR_M_DOWN_ERROR || sError == SHR_BM_DOWN_ERROR) {
				sError = SHR_FAIL;
			}
			else if (sError == STUB_RETCODE_TIME_OUT_ERROR) {
				sError = SHR_FAIL;
			}
			else if (sError == STUB_RETCODE_BUSY_ERROR) {
				sError = SHR_BUSY;
			}

			// If there are network level errors then we need to inform the
			// user there is a problem.
			switch (ShrMsg.sError) {
	//			case    STUB_DISCOVERY:  // Removed as causes problem with false Backup Master Busy error.
					// fall through and treat same as STUB_TIME_OUT.  See also PIF_ERROR_NET_DISCOVERY
				case STUB_TIME_OUT:
					sError = SHR_FAIL;
					break;
				case STUB_BUSY:
					sError = SHR_BUSY;
					break;
				default:
					break;
			}
		}
		PifReleaseSem(husCliExeNet);
	}
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliShrTermUnLock(UCHAR uchUAddr)
*                                   
*     InPut:    usUniqueAddress - Terget address
*    OutPut:    Nothing
*
** Return:      SHR_PERFORM:    print OK
*
** Description: This function supports to lock Shared Printer.
*===========================================================================
*/
SHORT    CliShrTermUnLock(UCHAR  uchUAddr)
{
    CLIREQSHR  ReqMsgH;
    CLIRESSHR  ResMsgH;
    SHORT      sError;

	if (uchUAddr == 0) {
		return SHR_FAIL;
	}

    PifRequestSem(husCliExeNet);
    CstInitShrMsgH(&ReqMsgH, &ResMsgH);

    memset(&ShrMsg, 0, sizeof(CLICOMIF));
    ShrMsg.usFunCode    = CLI_FCSPSTERMUNLOCK;
    ShrMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    ShrMsg.usReqMsgHLen = sizeof(CLIREQSHR);
    ShrMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    ShrMsg.usResMsgHLen = sizeof(CLIRESSHR);

    if (STUB_SELF == (sError = CstSendTarget(uchUAddr, &ShrMsg, &ShrSndBuf, &ShrRcvBuf))) {
       PifReleaseSem(husCliExeNet);
       sError = ShrTermUnLock();
    }
	else {
		if (STUB_TIME_OUT == sError) {
			sError = SHR_FAIL;
		}
		else if (STUB_BUSY == sError) {
			sError = SHR_BUSY;
		}
		else {
			sError = ShrMsg.sRetCode;

			if (sError == SHR_M_DOWN_ERROR || sError == SHR_BM_DOWN_ERROR) {
				sError = SHR_FAIL;
			}
			else if (sError == STUB_RETCODE_TIME_OUT_ERROR) {
				sError = SHR_FAIL;
			}
			else if (sError == STUB_RETCODE_BUSY_ERROR) {
				sError = SHR_BUSY;
			}

			// If there are network level errors then we need to inform the
			// user there is a problem.
			switch (ShrMsg.sError) {
	//			case    STUB_DISCOVERY:  // Removed as causes problem with false Backup Master Busy error.
					// fall through and treat same as STUB_TIME_OUT.  See also PIF_ERROR_NET_DISCOVERY
				case STUB_TIME_OUT:
					sError = SHR_FAIL;
					break;
				case STUB_BUSY:
					sError = SHR_BUSY;
					break;
				default:
					break;
			}
		}
       PifReleaseSem(husCliExeNet);
	}
    return sError;    
}

/************************** Add R3.0 **********************************/
/*
*===========================================================================
** Synopsis:    SHORT   CliShrPolling(UCHAR uchUAddr)
*                                   
*     InPut:    usUniqueAddress - Terget address
*    OutPut:    Nothing
*
** Return:      SHR_PERFORM:    print OK
*
** Description: This function supports to poll Shared Printer from K/P.
*===========================================================================
*/
SHORT    CliShrPolling(UCHAR  uchUAddr)
{
    CLIREQSHR  ReqMsgH;
    CLIRESSHR  ResMsgH;
    SHORT      sError;

	if (uchUAddr == 0) {
		return SHR_FAIL;
	}

    PifRequestSem(husCliExeNet);
    if (CstComReadStatus() & (CLI_STS_STOP | CLI_STS_INQUIRY)) {
        PifReleaseSem(husCliExeNet);
        return STUB_DUR_INQUIRY;
    }
    CstInitShrMsgH(&ReqMsgH, &ResMsgH);

    memset(&ShrMsg, 0, sizeof(CLICOMIF));
    ShrMsg.usFunCode    = CLI_FCSPSPOLLING;
    ShrMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    ShrMsg.usReqMsgHLen = sizeof(CLIREQSHR);
    ShrMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    ShrMsg.usResMsgHLen = sizeof(CLIRESSHR);

    if (STUB_SELF == (sError = CstSendTarget(uchUAddr, &ShrMsg, &ShrSndBuf, &ShrRcvBuf))) {
		PifReleaseSem(husCliExeNet);
        sError = ShrPolling();
    }
	else {
		if (STUB_TIME_OUT == sError) {
			sError = SHR_FAIL;
		}
		else if (STUB_BUSY == sError) {
			sError = SHR_BUSY;
		}
		else {
			sError = ShrMsg.sRetCode;

			if (sError == SHR_M_DOWN_ERROR || sError == SHR_BM_DOWN_ERROR) {
				sError = SHR_FAIL;
			}
			else if (sError == STUB_RETCODE_TIME_OUT_ERROR) {
				sError = SHR_FAIL;
			}
			else if (sError == STUB_RETCODE_BUSY_ERROR) {
				sError = SHR_BUSY;
			}

			// If there are network level errors then we need to inform the
			// user there is a problem.
			switch (ShrMsg.sError) {
	//			case    STUB_DISCOVERY:  // Removed as causes problem with false Backup Master Busy error.
					// fall through and treat same as STUB_TIME_OUT.  See also PIF_ERROR_NET_DISCOVERY
				case STUB_TIME_OUT:
					sError = SHR_FAIL;
					break;
				case STUB_BUSY:
					sError = SHR_BUSY;
					break;
				default:
					break;
			}
		}
		PifReleaseSem(husCliExeNet);
	}
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliShrPrintKP(UCHAR  uchUAddr,
*                                     VOID   *pData,
*                                     USHORT usDataLen)
*                                   
*     InPut:    usUniqueAddress - Terget address
*           	*auchSndBuffer  - Shared print data buffer
*               usDataLen       - Shared print data length
*    OutPut:    Nothing
*
** Return:      SHR_PERFORM:    print OK
*
** Description: This function supports to print out on Shared Printer for KP.
*===========================================================================
*/
SHORT    CliShrPrintKP(UCHAR  uchUAddr, VOID *pData, USHORT usDataLen)
{
    CLIREQSHR  ReqMsgH;
    CLIRESSHR  ResMsgH;
    SHORT      sError;
    SHORT      sRetryCo;

	if (uchUAddr == 0) {
		return SHR_FAIL;
	}

    PifRequestSem(husCliExeNet);
    CstInitShrMsgH(&ReqMsgH, &ResMsgH);

    memset(&ShrMsg, 0, sizeof(CLICOMIF));
    ShrMsg.usFunCode    = CLI_FCSPSPRINTKP;
    ShrMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    ShrMsg.usReqMsgHLen = sizeof(CLIREQSHR);
    ShrMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    ShrMsg.usResMsgHLen = sizeof(CLIRESSHR);
    ShrMsg.pauchReqData = pData;
    ShrMsg.usReqLen     = usDataLen;

    if (STUB_SELF == (sError = CstSendTarget(uchUAddr, &ShrMsg, &ShrSndBuf, &ShrRcvBuf))) {
		PifReleaseSem(husCliExeNet);
        sRetryCo = CstReadMDCPara(CLI_MDC_RETRY);
        do {
            sError = ShrPrint(pData);
            if (SHR_BUSY == sError) {
                sRetryCo --;
                CstSleep();
            } else {
                sRetryCo = 0;
            }
        } while ( 0 != sRetryCo );
    }
	else {
		if (STUB_TIME_OUT == sError) {
			sError = SHR_FAIL;
		}
		else if (STUB_BUSY == sError) {
			sError = SHR_BUSY;
		}
		else {
			sError = ShrMsg.sRetCode;

			if (sError == SHR_M_DOWN_ERROR || sError == SHR_BM_DOWN_ERROR) {
				sError = SHR_FAIL;
			}
			else if (sError == STUB_RETCODE_TIME_OUT_ERROR) {
				sError = SHR_FAIL;
			}
			else if (sError == STUB_RETCODE_BUSY_ERROR) {
				sError = SHR_BUSY;
			}

			// If there are network level errors then we need to inform the
			// user there is a problem.
			switch (ShrMsg.sError) {
	//			case    STUB_DISCOVERY:  // Removed as causes problem with false Backup Master Busy error.
					// fall through and treat same as STUB_TIME_OUT.  See also PIF_ERROR_NET_DISCOVERY
				case STUB_TIME_OUT:
					sError = SHR_FAIL;
					break;
				case STUB_BUSY:
					sError = SHR_BUSY;
					break;
				default:
					break;
			}
		}
		PifReleaseSem(husCliExeNet);
	}
    return sError;    
}
/************************** End Add R3.0 ******************************/

/*===== END OF SOURCE =====*/
