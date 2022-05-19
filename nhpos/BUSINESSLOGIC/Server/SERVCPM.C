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
* Title       : SERVER module, Charege Posting Source File
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVDFL.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           SerRecvCpm();       Charge Posting function handling
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Oct-28-93:H.Yamaguchi: initial                                   
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
#include    <stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <cpm.h>
#include    <eept.h>
#include    <csstbcpm.h>
#include    "servmin.h"


/*
*===========================================================================
** Synopsis:    VOID    SerRecvCpm(VOID);
*
** Return:      none.
*
** Description: This function executes for shared printer functuion request.
*===========================================================================
*/
VOID    SerRecvCpm(VOID)
{
	CLIREQCPM  *pReqMsgH = (CLIREQCPM *)SerRcvBuf.auchData;
	CLIRESCPM  ResMsgH = { 0 };
	SHORT      sSerSendStatus = 0;

	NHPOS_NONASSERT_NOTE("==PROVISIONING", "==PROVISIONING: SerRecvCpm(). CLI_FCCPM Charge Posting Function removed.");

	ResMsgH.usFunCode = pReqMsgH->usFunCode;
	ResMsgH.usSeqNo = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
	ResMsgH.usCpmReqNo = pReqMsgH->usCpmReqNo;
	ResMsgH.sResCode = STUB_SUCCESS;
	ResMsgH.sReturn = CPM_RET_FAIL;

	switch (pReqMsgH->usFunCode & CLI_RSTCONTCODE) {
	case    CLI_FCCPMSENDMESS:
		break;

	default:
		return;
		break;
	}

	sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, (sizeof(CLIRESCPM) - CLI_MAX_CPMDATA), NULL, 0);
	if (sSerSendStatus < 0) {
		char xBuff[128];
		sprintf(xBuff, "SerRecvCpm(): sSerSendStatus = %d, usFunCode = 0x%x, usSeqNo = 0x%x", sSerSendStatus, pReqMsgH->usFunCode, pReqMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}
}

#if defined(POSSIBLE_DEAD_CODE)

USHORT  ausSerCpmNo[CLI_ALLTRANSNO];

/*
*===========================================================================
** Synopsis:    VOID    SerRecvCpm(VOID);
*
** Return:      none.
*
** Description: This function executes for shared printer functuion request.
*===========================================================================
*/
VOID    SerRecvCpm(VOID)
{
    CLIREQCPM  *pReqMsgH;
    CLIRESCPM  ResMsgH;
    USHORT     usCpmNo, usUA;
	SHORT      sSerSendStatus = 0;

    pReqMsgH = (CLIREQCPM *)SerRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESCPM));
    ResMsgH.usFunCode = pReqMsgH->usFunCode;
    ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sResCode  = STUB_SUCCESS;
    ResMsgH.usCpmReqNo = pReqMsgH->usCpmReqNo;

    usUA =  SerRcvBuf.auchFaddr[CLI_POS_UA];

    usCpmNo = pReqMsgH->usCpmReqNo;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {
    case    CLI_FCCPMSENDMESS:
        if ( ausSerCpmNo[usUA -1 ]  != usCpmNo ) {
            ResMsgH.sReturn = CpmSendMessage(pReqMsgH->auchCpmData, pReqMsgH->usSndLen, CPM_FROM_NOT_SELF );     

            if ( CPM_RET_SUCCESS == ResMsgH.sReturn ) {
                memcpy(CpmSndBuf.CpmMesHed.auchFaddr, SerNetConfig.auchFaddr, PIF_LEN_IP);
                CpmSndBuf.CpmMesHed.auchFaddr[CLI_POS_UA] = (UCHAR)usUA;
                CpmSndBuf.CpmMesHed.usFport = SerRcvBuf.usFport;
                CpmSndBuf.CpmMesHed.usLport = CLI_PORT_CPM;
                memcpy(&CpmSndBuf.CpmResDat, &ResMsgH, sizeof(CLIRESCPM));
                CpmSndBuf.CpmResDat.usFunCode ++;

                ausSerCpmNo[usUA -1 ] = usCpmNo;
            }
        } else {
            ResMsgH.sReturn = CPM_RET_SUCCESS; CPM_RET_FAIL
        }
        break;

    default:    
        return;                        
        break;
    }

    sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, (sizeof(CLIRESCPM) - CLI_MAX_CPMDATA), NULL, 0);
	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "SerRecvCpm(): sSerSendStatus = %d, usFunCode = 0x%x, usSeqNo = 0x%x", sSerSendStatus, pReqMsgH->usFunCode, pReqMsgH->usSeqNo);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}
}

/*
*===========================================================================
** Synopsis:    VOID    SstCpmResetReqNo(VOID);
*
** Return:      none.
*
** Description: This function executes for shared printer functuion request.
*===========================================================================
*/
VOID    SstCpmResetReqNo(USHORT usUaddr)
{
    ausSerCpmNo[usUaddr -1] = 0;

}
#endif
/*===== END OF SOURCE =====*/

