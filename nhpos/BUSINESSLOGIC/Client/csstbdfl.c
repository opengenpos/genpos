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
* Title       : Client/Server STUB, Display on Fly Source File
* Category    : Client/Server STUB, US Hospitality Enhance Model
* Program Name: CSSTBDFL.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           CliDflPrint()         Request to disply on PC screen
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jun-16-93:H.Yamaguchi: initial                                   
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
#include	<tchar.h>

#include    <memory.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <para.h>
#include    <dfl.h>
#include    <csstbdfl.h>
#include    <csstbpar.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    "csstubin.h"

extern XGRAM       CliSndBuf;          /* Send Buffer */    
extern XGRAM       CliRcvBuf;          /* Receive Buffer */
extern  CLICOMIF   CliMsg;

/*
*===========================================================================
** Synopsis:    SHORT   CliDflPrint(VOID   *pData,
*                                   USHORT usDataLen)
*                                   
*     Input:    *pData       - Display on Fly data buffer
*               usDataLen    - DFL data length
*    OutPut:    Nothing
*
** Return:      DFL_PERFORM:    print OK
*
** Description:
*   This function supports to print out on Shared Printer.
*===========================================================================
*/
SHORT    CliDflPrint(VOID   *pData, USHORT usDataLen)
{
    CLIREQDFL  ReqMsgH;
    CLIRESDFL  ResMsgH;
    SHORT      sRetry, sRetryCo;
	SHORT		sError;
    USHORT     usPrevious;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQDFL));
    memset(&ResMsgH, 0, sizeof(CLIRESDFL));
                   
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCDFLPRINT;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQDFL);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESDFL);
    CliMsg.pauchReqData = pData;
    CliMsg.usReqLen     = usDataLen;

    if (STUB_SELF == SstReadFAsMaster() ) {
        sRetryCo = sRetry = CstReadMDCPara(CLI_MDC_RETRY);
        do {
            CliMsg.sRetCode = DflPrint(pData);
            if (DFL_BUSY == CliMsg.sRetCode) {

                /* allow power switch at error display */
                usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
				NHPOS_ASSERT_TEXT(0, "CliDflPrint(): (DFL_BUSY == CliMsg.sRetCode)");
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
        } while (DFL_BUSY == CliMsg.sRetCode);
    } 
    sError = CliMsg.sRetCode;    
    PifReleaseSem(husCliExeNet);
    return sError;    
}


/*===== END OF SOURCE =====*/

