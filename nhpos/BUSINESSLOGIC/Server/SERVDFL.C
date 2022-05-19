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
* Title       : SERVER module, Display on Fly Source File
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVDFL.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           SerRecvDfl();       Display on Fly function handling
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
#include    <stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <dfl.h>
#include    <csstbdfl.h>
#include    "servmin.h"

/*
*===========================================================================
** Synopsis:    VOID    SerRecvDfl(VOID);
*
** Return:      none.
*
** Description:
*   This function executes for shared printer functuion request.
*===========================================================================
*/
VOID    SerRecvDfl(VOID)
{
    CLIREQDFL  *pReqMsgH;
    CLIRESDFL  ResMsgH;
	SHORT      sSerSendStatus;

    pReqMsgH = (CLIREQDFL *)SerRcvBuf.auchData;
    memset(&ResMsgH, 0, sizeof(CLIRESDFL));
    ResMsgH.usFunCode = pReqMsgH->usFunCode;
    ResMsgH.usSeqNo   = pReqMsgH->usSeqNo & CLI_SEQ_CONT;
    ResMsgH.sResCode  = STUB_SUCCESS;

    switch(pReqMsgH->usFunCode & CLI_RSTCONTCODE) {
    case    CLI_FCDFLPRINT:
        ResMsgH.sReturn = DflPrint(SerResp.pSavBuff->auchData);
        if (DFL_BUSY == ResMsgH.sReturn) {
            SerSendError(STUB_BUSY);
            return;
        }
        break;

    default:    
        return;                        
        break;
    }

    sSerSendStatus = SerSendResponse((CLIRESCOM *)&ResMsgH, sizeof(CLIRESDFL) , NULL, 0);
	if (sSerSendStatus < 0) {
		char xBuff [128];
		sprintf (xBuff, "usFunCode = 0x%x, usSeqNo = 0x%x, sSerSendStatus = %d", pReqMsgH->usFunCode, pReqMsgH->usSeqNo, sSerSendStatus);
		NHPOS_ASSERT_TEXT((sSerSendStatus >= 0), xBuff);
	}
}


/*===== END OF SOURCE =====*/

