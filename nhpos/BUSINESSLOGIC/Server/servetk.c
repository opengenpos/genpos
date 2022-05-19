/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1995       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : SERVER module, Employee time keeping Functions Source File                        
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVCAS.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           SerRecvEtk();       Employee Time Keeping 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jun-17-93:H.Yamaguchi: initial                                   
* Oct-05-93:H.Yamaguchi: Adds ???EtkStatRead/SerEtkCurIndWrite for FVT                                   
* Nov-09-95:T.Nakahata : R3.1 Initial
*                           Increase No. of Employees (200 => 250)
*                           Increase Employee No. ( 4 => 9digits)
*                           Add Employee Name (Max. 16 Char.)
* 12.16.02 : cwunn     : Added data packing & return to client for time in GSU SR 8
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
#include    <stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <paraequ.h>
#include    <csttl.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <csserm.h>
#include    "servmin.h"

/*
*===========================================================================
** Synopsis:    VOID    SerRecvEtk(VOID);
*
** Return:      none.
*
** Description: This function executes for ETK function request.
*
* Update Histories                                                         
*     Date  :   Name    : Description
*  12.16.02 :	cwunn	: Added data packing & return to client for time in GSU SR 8
*===========================================================================
*/
VOID    SerRecvEtk(VOID)
{
	SHORT           sSerSendStatus = 0;
    CLIREQETK       *pReqMsgH;
    CLIRESETKTIME   ResMsgH;
	CLIRESETK		ResMsgH2;
	static ULONG    aulRcvBuffer[FLEX_ETK_MAX];

    pReqMsgH = (CLIREQETK *)SerRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESETKTIME));
    ResMsgH.usFunCode     = pReqMsgH->usFunCode;
    ResMsgH.sResCode      = STUB_SUCCESS;
    ResMsgH.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.usDataLen     = 0;
    ResMsgH.usFieldNo     = pReqMsgH->usFieldNo;
    
    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {

    case CLI_FCETKTIMEIN:                   /* ETK time in */
        ResMsgH.sReturn = EtkTimeIn(pReqMsgH->ulEmployeeNo, pReqMsgH->uchJobCode, (ETK_FIELD *)ResMsgH.auchETKData);
		//Added data packaging and sending sequence as a part of SR 8 fix by cwunn
		//If EtkTimeIn performs sucessfully, auchETKData should be returned
		//  to the requesting client for local use. usDataLen describes length.
		//The first CLIRESETKTIME bytes of ResMsgH are the message header, which
		//  describes the message to the client, and was sent in ver 1.0 
		if(ResMsgH.sReturn == ETK_SUCCESS){
			ResMsgH.usSeqNo = CLI_SEQ_RECVDATA;		//signals that data is included in message
			ResMsgH.usDataLen =  sizeof(ETK_FIELD);
			sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESETKTIME), ResMsgH.auchETKData, ResMsgH.usDataLen);
			if (sSerSendStatus < 0) {
				char xBuff [128];
				sprintf (xBuff, "SerRecvEtk(): sSerSendStatus = %d, ip 0x%x, usFunCode = 0x%x, usSeqNo = 0x%x, sReturn %d", sSerSendStatus, *(ULONG *)SerRcvBuf.auchFaddr, pReqMsgH->usFunCode, pReqMsgH->usSeqNo, ResMsgH.sReturn);
				NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
			}
			return;
		}
		//End of SR 8 fix
        break;             

    case CLI_FCETKTIMEOUT:                  /* ETK time out */
        ResMsgH.sReturn = EtkTimeOut(pReqMsgH->ulEmployeeNo, (ETK_FIELD *)ResMsgH.auchETKData);
        ResMsgH.usDataLen =  sizeof(ETK_FIELD);
        break;             

    case CLI_FCETKASSIGN:                   /* ETK assign   */
        ResMsgH.sReturn = EtkAssign(pReqMsgH->ulEmployeeNo, (ETK_JOB *)pReqMsgH->auchNewJob, pReqMsgH->auchEmployeeName);
        break;             

    case CLI_FCETKDELETE:                   /* ETK delete   */
        ResMsgH.sReturn = EtkDelete(pReqMsgH->ulEmployeeNo);
        break;

    case CLI_FCETKALLRESET:                 /* ETK all reset  */
        ResMsgH.sReturn = EtkAllReset();
        break;
    
    case CLI_FCETKISSUEDSET:                /* ETK issued set */
        ResMsgH.sReturn = EtkIssuedSet();
        break;

    case CLI_FCETKSTATREAD:                /* ETK Status read  */
        ResMsgH.sReturn = EtkStatRead(pReqMsgH->ulEmployeeNo, &ResMsgH.sTMStatus, &ResMsgH.usFieldNo, (ETK_JOB *)ResMsgH.auchETKData);
        ResMsgH.usDataLen =  sizeof(ETK_JOB);
        break;

    case CLI_FCETKCURINDWRITE:              /* ETK Mainte  */
        ResMsgH.sReturn = EtkCurIndWrite(pReqMsgH->ulEmployeeNo, &ResMsgH.usFieldNo, (ETK_FIELD *)pReqMsgH->auchNewJob);
        break;

    case CLI_FCETKINDJOBREAD:               /* Individual Job Read, R3.1 */
        ResMsgH.sReturn = EtkIndJobRead(pReqMsgH->ulEmployeeNo, (ETK_JOB *)&ResMsgH.auchETKData, &ResMsgH.auchEmployeeName[0]);
        break;

    case CLI_FCETKCURALLIDRD:            /* ETK all cur id read   */
		memset(&ResMsgH2, 0, sizeof(CLIRESETK));
		ResMsgH2.usFunCode     = pReqMsgH->usFunCode;
		ResMsgH2.sResCode      = STUB_SUCCESS;
		ResMsgH2.usSeqNo       = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
		ResMsgH2.usDataLen     = 0;
		ResMsgH2.usFieldNo     = pReqMsgH->usFieldNo;
		
        SerResp.pSavBuff = (CLIREQDATA *)&auchSerTmpBuf[sizeof(CLIRESETK)];

		ResMsgH2.sReturn = EtkCurAllIdReadIncr(sizeof(ResMsgH2.auchETKData), (ULONG *)&ResMsgH2.auchETKData, ResMsgH2.usFieldNo);

        ResMsgH2.usDataLen = sizeof(ResMsgH2.auchETKData);

		sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH2, sizeof(CLIRESETK), NULL, 0);
		if (sSerSendStatus < 0) {
			char xBuff [128];
			sprintf (xBuff, "SerRecvEtk(): sSerSendStatus = %d, ip 0x%x, usFunCode = 0x%x, usSeqNo = 0x%x, sReturn %d", sSerSendStatus, *(ULONG *)SerRcvBuf.auchFaddr, pReqMsgH->usFunCode, pReqMsgH->usSeqNo, ResMsgH.sReturn);
			NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
		}
		return;
        break;

	case CLI_FCETKCURINDREAD:            /* ETK cur ind id read   */
        ResMsgH.sReturn = EtkCurIndRead(pReqMsgH->ulEmployeeNo, &ResMsgH.usFieldNo, (ETK_FIELD  *)ResMsgH.auchETKData, (ETK_TIME   *)(ResMsgH.auchETKData + sizeof(ETK_FIELD)));

        ResMsgH.usDataLen = sizeof(ETK_FIELD) + sizeof(ETK_TIME);
		break;

    default:                                   /* not used */   
        return;
        break;
    }
    sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESETKTIME), NULL, 0);
	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "SerRecvEtk(): sSerSendStatus = %d, ip 0x%x, usFunCode = 0x%x, usSeqNo = 0x%x, sReturn %d", sSerSendStatus, *(ULONG *)SerRcvBuf.auchFaddr, pReqMsgH->usFunCode, pReqMsgH->usSeqNo, ResMsgH.sReturn);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}
}

/*===== END OF SOURCE =====*/
