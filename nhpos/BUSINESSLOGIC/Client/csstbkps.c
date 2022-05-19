/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server STUB, Kitchin Printer Source File
* Category    : Client/Server STUB, US Hospitality Model
* Program Name: CSSTBKPS.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               CliKpsPrint()               print request on kitchen printer
*               SerAlternativeKP()          alternative kitchen printer #
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-08-92:H.Nakashima: initial                                   
* Dec-16-99:hrkato     : Saratoga
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
#include    <cskp.h>
#include    <csstbkps.h>
#include    <csstbpar.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    "csstubin.h"

extern XGRAM       CliSndBuf;          /* Send Buffer */    
extern XGRAM       CliRcvBuf;          /* Receive Buffer */
extern  CLICOMIF   CliMsg;

/*
*===========================================================================
** Synopsis:    SHORT   CliKpsPrint(UCHAR *auchSndBuffer,
*                                   USHORT usSndBufLen,
*                                   UCHAR uchUniqueAddress,
*                                   USHORT *pusPrintStatus)
*     InOut:    *auchRcvBuffer  - print data buffer
*               usRcvBufLen     - print data length
*               usUniqueAddress - unique address
*    OutPut:    *pusPrintStatus - status of printer    
*
** Return:      KPS_PERFORM:    print OK
*
** Description: This function supports to print out on Kitchen Printer.
*===========================================================================
*/
SHORT   CliKpsPrint(UCHAR *auchSndBuffer, USHORT usSndBufLen,
                    UCHAR uchUniqueAddress, USHORT *pusPrintStatus)
{
    CLIREQKITPRINT  ReqMsgH;
    CLIRESKITPRINT  ResMsgH;
    SHORT           sRetry, sRetryCo;
	SHORT			sError;
    USHORT          usPrevious;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQKITPRINT));
    memset(&ResMsgH, 0, sizeof(CLIRESKITPRINT));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCKPSPRINT;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQKITPRINT);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESKITPRINT);
    CliMsg.pauchReqData = auchSndBuffer;
    CliMsg.usReqLen     = usSndBufLen;

    if (STUB_SELF == SstReadFAsMaster()) {
        PifReleaseSem(husCliExeNet);
        sRetryCo = sRetry = CstReadMDCPara(CLI_MDC_RETRY);
        do {
            sError = KpsPrint(auchSndBuffer,
                              usSndBufLen,
                              uchUniqueAddress,
                              pusPrintStatus,
                              0); /* ### ADD 2172 Rel1.0 (Saratoga) DUMMY */
            if (KPS_PRINTER_BUSY == sError) {

                /* allow power switch at error display */
                usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
				NHPOS_ASSERT_TEXT(0, "CliKpsPrint(): (KPS_PRINTER_BUSY == sError)");
                if (0 > --sRetryCo) {
                    if (CstDisplayError(STUB_KPS_BUSY, CliSndBuf.auchFaddr[CLI_POS_UA]) == 1) {
						break;
					}
                    sRetryCo = sRetry;
                } else {
                    CstSleep();
                }
                PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
            }
        } while (KPS_PRINTER_BUSY == sError);
    } else {
        *pusPrintStatus = ResMsgH.usKptStatus;
        sError = CliMsg.sRetCode;    
        PifReleaseSem(husCliExeNet);
		if (sError == KPS_TIME_OUT) {
			NHPOS_ASSERT_TEXT(0, "CliKpsPrint(): (sError == KPS_TIME_OUT)");
			sError = KPS_PRINTER_DOWN;
		}
		else if (sError == KPS_PRINTER_BUSY) {
			NHPOS_ASSERT_TEXT(0, "CliKpsPrint(): (sError == KPS_PRINTER_BUSY)");
			sError = KPS_PRINTER_BUSY;
		}
		else if (sError < 0) {
			NHPOS_ASSERT_TEXT(0, "CliKpsPrint(): (sError < 0)");
			sError = KPS_PRINTER_DOWN;
		}
    }
    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   CliKpsPrintEx(UCHAR *auchSndBuffer, USHORT usSndBufLen,
*                                   UCHAR uchUniqueAddress, USHORT *pusPrintStatus,
*                                   UCHAR uchTarget,SHORT nKPNo)
*     InOut:    *auchRcvBuffer  - print data buffer
*               usRcvBufLen     - print data length
*               usUniqueAddress - unique address
*               uchTarget       - Target Terminal Number
*               usOutPrinterInfo- Target Kitchen Printer Number & Alt flag  ### ADD 2172 Rel1.0(Saratoga)
*                                 PrinterNo : 1 <= 0x00?? <= MAX_DEST_SIZE (==8)
*                                 AltFlag   : 0x80??(Manual)/0x40??(Auto)/0x00??(normal)
*    OutPut:    *pusPrintStatus - status of printer    
*
** Return:      KPS_PERFORM:    print OK
*
** Description: This function supports to print out on Kitchen Printer.
*===========================================================================
*/

SHORT   CliKpsPrintEx(UCHAR *auchSndBuffer, USHORT usSndBufLen,
                    UCHAR uchUniqueAddress, USHORT *pusPrintStatus, UCHAR uchTarget,USHORT usOutPrinterInfo)
{
    CLIREQKITPRINT  ReqMsgH;
    CLIRESKITPRINT  ResMsgH;
    SHORT           sRetry, sRetryCo;
	SHORT			sError;
    USHORT          usPrevious;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQKITPRINT));
    ReqMsgH.usOutPrinterInfo = usOutPrinterInfo;    /* ### ADD 2172 Rel1.0 (Saratoga) */
    memset(&ResMsgH, 0, sizeof(CLIRESKITPRINT));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCKPSPRINT;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQKITPRINT);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESKITPRINT);
    CliMsg.pauchReqData = auchSndBuffer;
    CliMsg.usReqLen     = usSndBufLen;
	CliMsg.sRetCode		= KPS_PRINTER_DOWN;

    /* --- Send K/P Data to Target Terminal,    Saratoga --- */
    if (STUB_SELF == CstSendTarget(uchTarget, &CliMsg, &CliSndBuf, &CliRcvBuf)) {
        PifReleaseSem(husCliExeNet);
        sRetryCo = sRetry = CstReadMDCPara(CLI_MDC_RETRY);
        do {
            sError = KpsPrint(auchSndBuffer,
                              usSndBufLen,
                              uchUniqueAddress,
                              pusPrintStatus,
                              usOutPrinterInfo); /* ### ADD 2172 Rel1.0 (Saratoga) DUMMY */
            if (KPS_PRINTER_BUSY == sError) {

                /* allow power switch at error display */
                usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
				NHPOS_ASSERT_TEXT(0, "CliKpsPrintEx(): (KPS_PRINTER_BUSY == sError)");
                if (0 > --sRetryCo) {
                    if (CstDisplayError(STUB_KPS_BUSY, CliSndBuf.auchFaddr[CLI_POS_UA]) == 1) {
						break;
					}
                    sRetryCo = sRetry;
                } else {
                    CstSleep();
                }
                PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
            }
        } while (KPS_PRINTER_BUSY == sError);

    } else {
        *pusPrintStatus = ResMsgH.usKptStatus;
        sError = CliMsg.sRetCode;    
        PifReleaseSem(husCliExeNet);
		if (sError == KPS_TIME_OUT) {
			NHPOS_ASSERT_TEXT(0, "CliKpsPrintEx(): (sError == KPS_TIME_OUT)");
			sError = KPS_PRINTER_DOWN;
		}
		else if (sError == KPS_PRINTER_BUSY) {
			NHPOS_ASSERT_TEXT(0, "CliKpsPrintEx(): (sError == KPS_PRINTER_BUSY)");
			sError = KPS_PRINTER_BUSY;
		}
		else if (sError < 0) {
			NHPOS_ASSERT_TEXT(0, "CliKpsPrintEx(): (sError < 0)");
			sError = KPS_PRINTER_DOWN;
		}
    }

    return sError;    
}

/*
*===========================================================================
** Synopsis:    SHORT   SerAlternativeKP(UCHAR *puchSendBuf,
*                                        USHORT  usSize);
*     InPut:    *puchSendBuf    - buffer (alternative priter #)
*               usSize  - buffer size  
*
** Return:      KPS_PERFORM:    OK
*               KPS_NOT_MASTER: error
*
** Description: This function supports to set alternative Kitchen Printer #.
*===========================================================================
*/
SHORT   SerAlternativeKP(UCHAR *puchSendBuf,
                            USHORT usSize)
{
    SHORT           sRetCode;
    CLIREQALTKP     ReqMsgH;
    CLIRESALTKP     ResMsgH;
    USHORT          usWsize;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQALTKP));
    memset(&ResMsgH, 0, sizeof(CLIRESALTKP));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCKPSALTMANKP | CLI_FCWBACKUP | CLI_FCSERVERWB;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQALTKP);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESALTKP);
    CliMsg.pauchReqData = puchSendBuf;
    CliMsg.usReqLen     = usSize;

    if (STUB_SELF == SstUpdateAsMaster()) {
        sRetCode = CliParaAllWrite(CLASS_PARAMANUALTKITCH,
                                   puchSendBuf,
                                   usSize,
                                   0,
                                   &usWsize);
    } else {
        sRetCode = KPS_NOT_MASTER;
    }
    if (0 == sRetCode) {
        SstUpdateBackUp();
        sRetCode = KPS_PERFORM;
    } 
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*===== END OF SOURCE =====*/

